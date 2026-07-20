# xlog 日志架构

热路径、两层 sink、过滤、FATAL / stderr 镜像的设计说明。英文版见 [logging.md](./logging.md)。

**给 AI：** 改 sink / 格式路径前先读 `xlog/skills.h` 与 [AI.md](./AI.md) 顶部的 xlog 段。

## 管线

```text
宏 (XLOG / TLOG / ZLOG / …)
  → LogMessage（元数据 + 正文写入 entry.buffer）
  → 析构 / flush
       ├─ min_log_level / verbosity 门控  （FATAL 可在未分发时 abort）
       └─ log_to_sinks → 默认 LogSinkSet::do_log
              ├─ format_log          ← 每条只做一次（set 层）
              ├─ extra sinks send
              ├─ set 内 sinks send
              └─ stderr_threshold 分支
                     write_to_stderr
                     若 FATAL → abort
```

`LogEntry` 含级别、位置、时间、tid/pid、可选 `verbose_level`、原始 `buffer`，
以及 format 后的 `format_buffer`（sink 真正写出的内容）。

## 两层扩展

扩展点故意拆成两层：**写到哪** 和 **这一组怎么统一排版** 不要混在一起。

| 层 | 类型 | 职责 | 重写点 |
|----|------|------|--------|
| **Sink** | `LogSink` | 单路出口（stderr / 文件 / 彩色 / null / mock…） | `send` / `flush` |
| **Sink set** | `LogSinkSet` | 发起线程先 format（无锁）；**mutex 只罩** sink 扇出 + stderr / FATAL | `format_log`、`dispatch_locked`，必要时 `do_log` |

### Sink 层（`LogSink`）

- 收到的是**已经 format 过**的 `LogEntry`（`format_buffer` 已填好）。
- 只负责该目的地的 I/O / 缓冲。
- 内置：`DefaultSink`、`NullSink`、`AnsiColorSink`、
  `RotatingFileSink`、`DailyFileSink`、`HourlyFileSink`。
- 单条附加：`to_sink_also` / `to_sink_only`（extra sinks；格式仍由**当前默认 set** 的 `format_log` 决定）。

若要对多个目的地共用一种自定义版式，**不要**在每个 `send` 里各自拼前缀——会做 N 次。

### Sink set 层（`LogSinkSet`）

- `do_log`：**发起线程先 `format_log`（不加锁）** → 再拿 set mutex →
  `dispatch_locked`（广播 + `stderr_threshold`）。
- 内置 sink **不要**再加自己的 mutex；顺序由 set 锁保证。
- `send`/`flush` 里再打 `XLOG`：TLS 重入，不二次抢锁，只写 stderr。
- 改排版重写 `format_log`；改分发重写 `dispatch_locked`（`FormatOnlySinkSet` 跳过 I/O）。
  用 `add_log_sink_set` 注册。

```cpp
class JsonLogSinkSet : public xlog::LogSinkSet {
 public:
  using LogSinkSet::LogSinkSet;
 protected:
  void format_log(xlog::LogEntry& entry) override {
    // 只填一次 format_buffer，供本 set 内所有 sink 使用
  }
};
```

当前 registry 的 `add_log_sink` / `add_log_sinks` 建的是普通 `LogSinkSet`。
挂自定义 set 子类需要把 `unique_ptr<LogSinkSet>` 放进 registry（同样「不删除」的生命周期）。
**格式改 set 层，sink 保持薄。**

### Registry（`LogSinkRegistry`）

- `id → LogSinkSet`；同一时刻一个 default。
- **不提供 remove**——热路径可在 `set_default` 后长期持有裸 `LogSinkSet*`。
- id `0`：内置 set — `DefaultSink`（stderr），Android / Windows 上再并列挂
  `AndroidLogSink` / `WindowsDebuggerLogSink`（与 absl/turbo 默认终端通道一致）。
- `set_default_sink(id)` 切换进程默认 set（`log_to_sinks` 走这里）。

## 过滤（正交旋钮）

| 旋钮 | 位置 | 作用 |
|------|------|------|
| `XLOG_MIN_LOG_LEVEL` / `XLOG_STRIP_LOG` | 编译期宏 | 宏侧短路或变成 `NullStream` |
| `min_log_level` | `LogMessage::flush` | 整条不进 sink；**FATAL 仍 abort** |
| `verbosity` / `XLOG_MAX_VLOG_VERBOSITY` | flush + 宏 | `XVLOG`/`TVLOG`/`ZVLOG` 固定 **INFO** + `verbose_level`；对照 `verbosity()` |
| `stderr_threshold` | `LogSinkSet::do_log`（sinks 之后） | 额外裸 stderr 镜像；内嵌 FATAL abort |

`min_log_level`：这条日志打不打。  
`stderr_threshold`：打了之后还要不要再镜像一份 stderr。二者无关。

## stderr_threshold 与 FATAL

约定：threshold 恒 ≤ FATAL，故 FATAL 一定进入外层：

```text
if (severity >= stderr_threshold) {
    write_to_stderr(...);          // 裸写，不再进 registry
    if (severity == FATAL)
        abort();
}
```

- 已无独立 `on_fatal_error`；终止就在这段嵌套逻辑里。
- 在 `flush` 被 `min_log_level` / verbosity 滤掉的 FATAL：不进 `do_log`，仍在 `flush` 里 `abort()`。
- sink **已经占用 stderr** 时，把 threshold 提到 **FATAL**，避免日常级别双打；FATAL 仍会镜像再 abort。

| 场景 | 典型 `stderr_threshold` |
|------|-------------------------|
| 内置 `DefaultSink` / `initialize_log` | `FATAL` |
| `setup_stderr` / `setup_color_stderr` / 文件+彩色 | `FATAL` |
| 仅文件（`setup_rotating/daily/hourly_file`） | `ERROR`（文件全量 + ERROR+ stderr） |
| `setup_color_stdout` | `ERROR`（彩色走 stdout；ERROR+ 再镜像 stderr） |

## 宏与写法（摘要）

| 族 | 正文 | 说明 |
|----|------|------|
| `XLOG` / `DXLOG` | `operator<<` | `DFATAL`、`.no_prefix()`、`.with_perror()` / `XPLOG` |
| `TLOG` / `DTLOG` | `fmt` | |
| `ZLOG` / `DZLOG` | printf（经 fmt） | |
| `XVLOG` / `TVLOG` / `ZVLOG` | 同上 | 级别 INFO；前缀可标 `Vn` |
| `XCHECK*` | 失败即 FATAL | |

编译期：`XLOG_MIN_LOG_LEVEL`、`XLOG_STRIP_LOG`、`XLOG_MAX_VLOG_VERBOSITY`。  
无 vmodule。堆栈回溯不在本仓库（另项）。

## 一键 setup

`<xlog/setup.h>`：`initialize_log` + 注册默认 set + 设好 `stderr_threshold`。  
应用优先用 setup；测试 / 多 set 切换用 registry（`ScopedMockLog` 会换 default set）。

## 设计要点（短）

1. **排版在 set，写出在 sink。**
2. **每条日志对整组 sink（含 extra）只 format 一次。**
3. **stderr 镜像是 set 策略**（threshold 分支），不要和「再挂一个会递归的 sink」搞混；内置 `DefaultSink` 本身仍是普通 stderr sink。
4. **FATAL 必终止**：进 threshold 分支，或在 `flush` 被滤掉时 abort；不要指望 sink 去 abort。
5. Registry **只增不删**，用 `set_default_sink` 切换。
