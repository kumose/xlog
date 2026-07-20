# xlog logging architecture

Design notes for the hot path, sink layers, filtering, and FATAL / stderr
mirroring. Companion: [中文](./logging_CN.md).

**AI agents:** also read `xlog/skills.h` and the xlog section of [AI.md](./AI.md)
before changing sinks or format paths.

## Pipeline

```text
macro (XLOG / TLOG / ZLOG / …)
  → LogMessage (metadata + message body in entry.buffer)
  → ~LogMessage / flush
       ├─ min_log_level / verbosity gates  (may abort FATAL without dispatch)
       └─ log_to_sinks → default LogSinkSet::do_log
              ├─ format_log          ← once per entry (set layer)
              ├─ extra sinks send
              ├─ set sinks send
              └─ stderr_threshold branch
                     write_to_stderr
                     if FATAL → abort
```

`LogEntry` carries severity, location, timestamp, tid/pid, optional `verbose_level`,
raw `buffer`, and after format, `format_buffer` (what sinks write).

## Two extension layers

Customization is intentionally split. Do not confuse “where to write” with
“how to format for a whole set”.

| Layer | Type | Role | Override |
|-------|------|------|----------|
| **Sink** | `LogSink` | One destination (stderr, file, color, null, mock…) | `send` / `flush` |
| **Sink set** | `LogSinkSet` | Format unlocked on caller thread; **mutex only around** sink fan-out + stderr / FATAL | `format_log`, `dispatch_locked`, optionally `do_log` |

### Sink layer (`LogSink`)

- Receives an already-formatted `LogEntry` (`format_buffer` filled).
- Responsible only for I/O / buffering for that destination.
- Built-ins: `DefaultSink`, `NullSink`, `AnsiColorSink`,
  `RotatingFileSink`, `DailyFileSink`, `HourlyFileSink`.
- Per-statement attach: `LogMessage::to_sink_also` / `to_sink_only`
  (extra sinks on the entry; still formatted by the **current default set**).

Do **not** re-implement prefix / layout inside every `send` if the goal is a
shared custom format for several destinations — that repeats work N times.

### Sink-set layer (`LogSinkSet`)

- `do_log`: **`format_log` on the logging thread (unlocked)** → take set
  mutex → `dispatch_locked` (broadcast + `stderr_threshold`).
- Built-in sinks **must not** take a per-sink mutex; ordering is the set lock.
- Re-entrant `XLOG` from `send`/`flush`: TLS guard, skip set lock, stderr only.
- Default `format_log` → `xlog_format`. Custom layout → override `format_log`;
  custom fan-out → override `dispatch_locked` (`FormatOnlySinkSet` skips I/O).
  Register with `add_log_sink_set`.

```cpp
class JsonLogSinkSet : public xlog::LogSinkSet {
 public:
  using LogSinkSet::LogSinkSet;
 protected:
  void format_log(xlog::LogEntry& entry) override {
    // fill entry.format_buffer once for every sink in this set
  }
};
```

Registry today builds plain `LogSinkSet` via `add_log_sink` / `add_log_sinks`.
Registering a custom set subclass requires owning a `unique_ptr<LogSinkSet>` in
the registry (same “no remove” lifetime rules). Prefer extending the set layer
for format; keep sinks thin.

### Registry (`LogSinkRegistry`)

- Map `id → LogSinkSet`; one default id / pointer.
- **No remove** — hot path may keep a raw `LogSinkSet*` after `set_default`.
- Id `0` is the built-in set created at first use: `DefaultSink` (stderr), and
  on Android / Windows also `AndroidLogSink` / `WindowsDebuggerLogSink` as
  **sibling** sinks in the same set (absl/turbo-style platform terminals).
- `set_default_sink(id)` switches the process-wide default used by
  `log_to_sinks`.

## Filtering (orthogonal knobs)

| Knob | Where | Effect |
|------|--------|--------|
| `XLOG_MIN_LOG_LEVEL` / `XLOG_STRIP_LOG` | compile-time macros | Drop or null-stream statements before `LogMessage` work |
| `min_log_level` | `LogMessage::flush` | Drop entry before sinks; **FATAL still aborts** |
| `verbosity` / `XLOG_MAX_VLOG_VERBOSITY` | flush + macros | `XVLOG`/`TVLOG`/`ZVLOG` are **INFO** with `verbose_level`; gate vs `verbosity()` |
| `stderr_threshold` | `LogSinkSet::do_log` (after sinks) | Extra plain-stderr mirror; nested FATAL abort |

`min_log_level` decides whether the message is logged at all.
`stderr_threshold` decides whether it is **also** mirrored to stderr after sinks
ran. They are not the same.

## stderr_threshold and FATAL

Contract (threshold is always ≤ FATAL, so FATAL always enters the outer branch):

```text
if (severity >= stderr_threshold) {
    write_to_stderr(...);          // raw mirror, not back into registry
    if (severity == FATAL)
        abort();
}
```

- There is no separate `on_fatal_error`; terminate lives in this nested check.
- Filtered-out FATAL (failed `min_log_level` / verbosity in `flush`) still
  `abort()` in `flush` without calling `do_log`.
- When a sink **already** owns stderr, raise threshold to **FATAL** so routine
  levels are not double-printed; FATAL still mirrors once more then aborts.

| Setup / situation | Typical `stderr_threshold` |
|-------------------|----------------------------|
| Built-in `DefaultSink` / `initialize_log` | `FATAL` |
| `setup_stderr` / `setup_color_stderr` / file+color | `FATAL` |
| File-only (`setup_rotating/daily/hourly_file`) | `ERROR` (file + ERROR+ stderr) |
| `setup_color_stdout` | `ERROR` (color on stdout; ERROR+ also on stderr) |

## Macros and message styles (summary)

| Family | Body | Notes |
|--------|------|--------|
| `XLOG` / `DXLOG` | `operator<<` | `DFATAL`, `.no_prefix()`, `.with_perror()` / `XPLOG` |
| `TLOG` / `DTLOG` | `fmt` | |
| `ZLOG` / `DZLOG` | printf via fmt | |
| `XVLOG` / `TVLOG` / `ZVLOG` | same styles | severity INFO; prefix may show `Vn` |
| `XCHECK*` | FATAL on failure | |

Compile-time: `XLOG_MIN_LOG_LEVEL`, `XLOG_STRIP_LOG`, `XLOG_MAX_VLOG_VERBOSITY`.
No vmodule. Stack traces are out of tree (separate project).

## Setup helpers

`<xlog/setup.h>`: `initialize_log` + register a sink set as default + set
`stderr_threshold` appropriately. Prefer these for apps; use registry APIs for
tests / multi-set switching (`ScopedMockLog` swaps the default set).

## Design rules (short)

1. **Format at set scope, write at sink scope.**
2. **One format pass per entry** for all sinks in the set (+ extras).
3. **stderr mirror is set policy**, not a second `LogSink` in the default path
   for threshold logic (though `DefaultSink` itself is a normal sink that writes
   stderr for the built-in set).
4. **FATAL always terminates** if it reaches the threshold branch or is filtered
   in `flush`; do not rely on sinks to abort.
5. Registry sets are **append-only**; switch with `set_default_sink`.
