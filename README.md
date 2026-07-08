# xlog

[中文版](./README_CN.md)

xlog is a C++ logging library built on **[spdlog](https://github.com/gabime/spdlog)**. The writer layer (`logger`, `pattern_formatter`, sinks, async thread pool, registry, and bundled fmt) comes from spdlog and is renamed to the `xlog` namespace. On top of that, xlog adds macro families, runtime level control, and optional upstream adapters for glog, turbo, and abseil.

## Based on spdlog

| From spdlog (kept) | Added by xlog |
|--------------------|---------------|
| `logger`, `log_msg`, registry | `XLOG` / `TLOG` / `ZLOG` and `_IF` / `_EVERY_N` / … variants |
| `pattern_formatter`, sinks (file, rotating, daily, async, …) | `XPLOG` / `TPLOG` / `ZPLOG`, `VXLOG` / `TVLOG` / `VZLOG` |
| bundled fmt | `XCHECK` / `TCHECK` / `ZCHECK`, debug `D*` macros (release strip) |
| | `xlog::log_level()`, `xlog::vlog_level()` |
| | `<xlog/upstream/*.h>` — glog / turbo / abseil → `log_msg` |

spdlog API names are largely preserved inside `<xlog/xlog.h>` (`create`, `set_pattern`, `default_logger`, …). Use `<xlog/logging.h>` when you need the xlog macro layer.

Original spdlog code is under the MIT License (see file headers). xlog project code is under the Apache License 2.0 — see [LICENSE](LICENSE).

## Features

| Area | Details |
|------|---------|
| **Macros** | `XLOG` (stream `<<`), `TLOG` (fmt `{}`), `ZLOG` (printf `%`) |
| **Variants** | `XPLOG` / `TPLOG` / `ZPLOG` (append `errno`), `VXLOG` / `TVLOG` / `VZLOG` (verbose at TRACE) |
| **Checks** | `XCHECK` / `TCHECK` / `ZCHECK` (+ debug `D*CHECK`, zero-cost in release) |
| **Conditions** | `_IF`, `_EVERY_N`, `_FIRST_N`, `_ONCE`, `_EVERY_POW_2`, `_EVERY_N_SEC`, `_LEVEL`, and `_IF_*` counterparts |
| **Runtime filter** | `xlog::log_level()`, `xlog::vlog_level()` — shared by X/T/Z and VLOG macros |
| **Debug strip** | `DXLOG` / `DTLOG` / `DZLOG` and matching PLOG/VLOG/CHECK families (`NDEBUG` → no-op) |
| **Writer** | spdlog-derived `logger`, `pattern_formatter`, sync/async, daily/hourly/rotating file sinks, and more |
| **Upstream** | Optional `#include <xlog/upstream/{turbo,glog,absl}.h>` — foreign `LogEntry` → `log_msg` → same formatter/sinks |

Include `<xlog/logging.h>` for all macro families. Include `<xlog/xlog.h>` for the logger API only.

Bundled **fmt 12.1.0** only; do not mix external fmt.

## Quick start

```cpp
#include <xlog/xlog.h>
#include <xlog/logging.h>
#include <xlog/log_setting.h>

int main() {
    xlog::log_level() = XLOG_LEVEL_INFO;
    xlog::vlog_level() = 1;

    XLOG(INFO) << "stream " << 42;
    TLOG(INFO, "fmt {}", 42);
    ZLOG(INFO, "printf %d", 42);

    TVLOG(1, "verbose when vlog_level >= 1");

    xlog::shutdown();
    return 0;
}
```

See [examples/demo_1.cc](examples/demo_1.cc) for a minimal program and [examples/demo.cc](examples/demo.cc) for the full macro demo.

## Logger and sinks

Configure the default logger (or create named loggers) with sinks and a single pattern — all macros write through it.

```cpp
#include <xlog/xlog.h>
#include <xlog/sinks/rotating_file_sink.h>

auto logger = xlog::rotating_logger_mt("app", "logs/app.log", 1024 * 1024 * 10, 3);
logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
xlog::set_default_logger(logger);
```

Async logging, `dist_sink`, `null_sink`, color console, systemd, and other sinks are available under `xlog/sinks/`.

## Upstream adapters (optional)

Header-only sinks that implement each framework's `LogSink`, convert structured fields to `log_msg`, and write through your xlog logger. **Not built into libxlog** — link glog/turbo/absl yourself and include only what you need.

```cpp
#include <xlog/xlog.h>
#include <xlog/upstream/turbo.h>
#include <turbo/log/initialize.h>
#include <turbo/log/logging.h>

auto logger = xlog::default_logger();
logger->set_pattern("[%l] %v");

turbo::initialize_log();
xlog::upstream::enable_turbo(logger);

KLOG(INFO) << "goes through xlog formatter and sinks";

xlog::upstream::disable_turbo();
```

| Header | Enable | Foreign API |
|--------|--------|-------------|
| `<xlog/upstream/turbo.h>` | `enable_turbo()` | `KLOG`, … |
| `<xlog/upstream/glog.h>` | `enable_glog()` | `LOG`, … |
| `<xlog/upstream/absl.h>` | `enable_absl()` | `ABSL_LOG`, … |

Examples: [demo_upstream_turbo.cc](examples/demo_upstream_turbo.cc), [demo_upstream_glog.cc](examples/demo_upstream_glog.cc), [demo_upstream_absl.cc](examples/demo_upstream_absl.cc).

## Build

This project uses [kmpkg](https://github.com/kumose/kmcmake) for dependency management.

### Prerequisites

- Linux (Ubuntu 20.04+ / CentOS 7+ recommended)
- CMake ≥ 3.24
- GCC ≥ 9.4 / Clang ≥ 12
- [kmpkg](https://kumo-pub.github.io/docs/category/%E6%8C%81%E7%BB%AD%E9%9B%86%E6%88%90----kmpkg) installed

Dependencies are listed in [`kmpkg.json`](kmpkg.json). Optional packages (benchmark, glog, turbo, abseil) are used for benchmarks, tests, and upstream examples only.

### Build

```bash
mkdir -p build && cd build
cmake .. -DKMCMAKE_BUILD_EXAMPLES=ON
cmake --build . -j$(nproc)
```

Optional CMake flags:

| Flag | Purpose |
|------|---------|
| `KMCMAKE_BUILD_TEST=ON` | unit tests |
| `KMCMAKE_BUILD_BENCHMARK=ON` | benchmarks |
| `KMCMAKE_BUILD_EXAMPLES=ON` | example binaries |

### Tests

```bash
ctest --test-dir build
```

## License

xlog project code: [Apache License 2.0](LICENSE).

spdlog-derived portions retain the MIT License per their source file headers.
