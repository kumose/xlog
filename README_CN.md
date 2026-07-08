# xlog

[English](./README.md)

xlog 是在 **[spdlog](https://github.com/gabime/spdlog)** 基础上开发的 C++ 日志库。写入层（`logger`、`pattern_formatter`、各类 sink、async 线程池、registry、内置 fmt）继承自 spdlog，命名空间改为 `xlog`。在此之上，xlog 增加了宏体系、运行时级别控制，以及可选的 glog / turbo / abseil 上游适配头文件。

## 与 spdlog 的关系

| 继承自 spdlog | xlog 新增 |
|---------------|-----------|
| `logger`、`log_msg`、registry | `XLOG` / `TLOG` / `ZLOG` 及 `_IF` / `_EVERY_N` / … 变体 |
| `pattern_formatter`、file / rotating / daily / async 等 sink | `XPLOG` / `TPLOG` / `ZPLOG`，`VXLOG` / `TVLOG` / `VZLOG` |
| 内置 fmt | `XCHECK` / `TCHECK` / `ZCHECK`，debug `D*` 宏（Release 剥离） |
| | `xlog::log_level()`、`xlog::vlog_level()` |
| | `<xlog/upstream/*.h>` — glog / turbo / abseil → `log_msg` |

`<xlog/xlog.h>` 中 spdlog 风格 API 基本保留（`create`、`set_pattern`、`default_logger` 等）。宏层请 `#include <xlog/logging.h>`。

spdlog 原始代码为 MIT 许可证（见各文件头）。xlog 项目代码为 Apache License 2.0 — 见 [LICENSE](LICENSE)。

## 功能

| 类别 | 说明 |
|------|------|
| **宏** | `XLOG`（stream `<<`）、`TLOG`（fmt `{}`）、`ZLOG`（printf `%`） |
| **变体** | `XPLOG` / `TPLOG` / `ZPLOG`（附带 `errno`）、`VXLOG` / `TVLOG` / `VZLOG`（VERBOSE，TRACE 级别） |
| **检查** | `XCHECK` / `TCHECK` / `ZCHECK`（及 debug 版 `D*CHECK`，Release 零开销） |
| **条件** | `_IF`、`_EVERY_N`、`_FIRST_N`、`_ONCE`、`_EVERY_POW_2`、`_EVERY_N_SEC`、`_LEVEL` 及对应 `_IF_*` |
| **运行时过滤** | `xlog::log_level()`、`xlog::vlog_level()` — X/T/Z 与 VLOG 共用 |
| **Debug 剥离** | `DXLOG` / `DTLOG` / `DZLOG` 及 PLOG/VLOG/CHECK 的 D 系列（`NDEBUG` 下为空操作） |
| **写入** | 继承 spdlog 的 `logger`、`pattern_formatter`、同步/异步、daily/hourly/rotating 等 sink |
| **上游** | 可选 `#include <xlog/upstream/{turbo,glog,absl}.h>` — 外部 entry → `log_msg` → 同一 formatter/sink |

宏全家桶：`#include <xlog/logging.h>`。仅使用 logger API：`#include <xlog/xlog.h>`。

仅使用内置 **fmt 12.1.0**，勿与外部 fmt 混用。

## 快速开始

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

    TVLOG(1, "vlog_level >= 1 时输出");

    xlog::shutdown();
    return 0;
}
```

最小示例见 [examples/demo_1.cc](examples/demo_1.cc)，完整宏演示见 [examples/demo.cc](examples/demo.cc)。

## Logger 与 Sink

配置 default logger（或创建命名 logger）的 sink 与 pattern，所有宏经同一管道输出。

```cpp
#include <xlog/xlog.h>
#include <xlog/sinks/rotating_file_sink.h>

auto logger = xlog::rotating_logger_mt("app", "logs/app.log", 1024 * 1024 * 10, 3);
logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
xlog::set_default_logger(logger);
```

异步、dist_sink、null_sink、彩色控制台、systemd 等见 `xlog/sinks/`。

## 上游适配（可选）

头文件实现各框架的 `LogSink`，将结构化字段转为 `log_msg` 写入 xlog logger。**不编入 libxlog** — 用户自行链接 glog/turbo/absl，按需 include。

```cpp
#include <xlog/xlog.h>
#include <xlog/upstream/turbo.h>
#include <turbo/log/initialize.h>
#include <turbo/log/logging.h>

auto logger = xlog::default_logger();
logger->set_pattern("[%l] %v");

turbo::initialize_log();
xlog::upstream::enable_turbo(logger);

KLOG(INFO) << "经 xlog formatter 与 sink 输出";

xlog::upstream::disable_turbo();
```

| 头文件 | 启用 | 外部 API |
|--------|------|----------|
| `<xlog/upstream/turbo.h>` | `enable_turbo()` | `KLOG` 等 |
| `<xlog/upstream/glog.h>` | `enable_glog()` | `LOG` 等 |
| `<xlog/upstream/absl.h>` | `enable_absl()` | `ABSL_LOG` 等 |

示例：[demo_upstream_turbo.cc](examples/demo_upstream_turbo.cc)、[demo_upstream_glog.cc](examples/demo_upstream_glog.cc)、[demo_upstream_absl.cc](examples/demo_upstream_absl.cc)。

## 构建

使用 [kmpkg](https://github.com/kumose/kmcmake) 管理依赖。

### 环境

- Linux（推荐 Ubuntu 20.04+ / CentOS 7+）
- CMake ≥ 3.24
- GCC ≥ 9.4 / Clang ≥ 12
- 已安装 [kmpkg](https://kumo-pub.github.io/docs/category/%E6%8C%81%E7%BB%AD%E9%9B%86%E6%88%90----kmpkg)

依赖见 [`kmpkg.json`](kmpkg.json)。benchmark、glog、turbo、abseil 用于 benchmark、测试与 upstream 示例。

### 编译

```bash
mkdir -p build && cd build
cmake .. -DKMCMAKE_BUILD_EXAMPLES=ON
cmake --build . -j$(nproc)
```

可选 CMake 开关：

| 选项 | 用途 |
|------|------|
| `KMCMAKE_BUILD_TEST=ON` | 单元测试 |
| `KMCMAKE_BUILD_BENCHMARK=ON` | 性能测试 |
| `KMCMAKE_BUILD_EXAMPLES=ON` | 示例程序 |

### 测试

```bash
ctest --test-dir build
```

## 许可证

xlog 项目代码：[Apache License 2.0](LICENSE)。

继承自 spdlog 的代码仍按各文件头中的 MIT 许可证保留。
