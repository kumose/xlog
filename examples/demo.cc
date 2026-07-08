// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Macro families demonstrated:
//   XLOG / TLOG / ZLOG          — stream / fmt / printf
//   XPLOG / TPLOG / ZPLOG       — perror suffix variants
//   VXLOG / TVLOG / VZLOG       — verbose (TRACE), vlog_is_on()
//   DXLOG / DTLOG / DZLOG       — debug-only (NDEBUG strips)
//   D*PLOG / DV*LOG / D*CHECK   — debug-only counterparts
//   XCHECK / TCHECK / ZCHECK    — pass-path checks only
//   *_IF / *_EVERY_N / *_ONCE / *_FIRST_N / *_LEVEL / *_IF_EVERY_N
//
// Build: cmake with KMCMAKE_BUILD_EXAMPLES=ON, run the demo binary.

#include <xlog/xlog.h>
#include <xlog/logging.h>
#include <xlog/log_setting.h>
#include <xlog/sinks/stdout_color_sinks.h>

#include <cerrno>
#include <cstring>

#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>
#endif

namespace {

void setup_demo_logger() {
    auto console = xlog::stdout_color_mt("xlog_macro_demo");
    console->set_pattern("[%L] %v");
    console->set_level(xlog::level::trace);
    xlog::set_default_logger(console);

    // Runtime filters used by XLOG/TLOG/ZLOG and VLOG families.
    xlog::log_level() = XLOG_LEVEL_TRACE;
    xlog::vlog_level() = 2;
}

// ---------------------------------------------------------------------------
// XLOG — stream style (<<)
// ---------------------------------------------------------------------------
void demo_xlog() {
    XLOG(INFO) << "XLOG(INFO) stream message, n=" << 42;

    XLOG_IF(DEBUG, true) << "XLOG_IF when condition is true";
    XLOG_IF(DEBUG, false) << "XLOG_IF should not appear";

    for (int i = 0; i < 5; ++i) {
        XLOG_EVERY_N(INFO, 2) << "XLOG_EVERY_N(2) i=" << i;
    }

    XLOG_LEVEL(xlog::level::warn) << "XLOG_LEVEL dynamic severity (warn)";

    XLOG_ONCE(INFO) << "XLOG_ONCE — prints once per callsite";
    XLOG_IF_ONCE(INFO, true) << "XLOG_IF_ONCE when condition holds";

    for (int i = 0; i < 4; ++i) {
        XLOG_IF_EVERY_N(INFO, i % 2 == 0, 2) << "XLOG_IF_EVERY_N even i=" << i;
    }
}

// ---------------------------------------------------------------------------
// TLOG — fmt {} style
// ---------------------------------------------------------------------------
void demo_tlog() {
    TLOG(INFO, "TLOG(INFO) fmt message, n={}", 42);
    TLOG_IF(DEBUG, true, "TLOG_IF enabled, value={}", 1);
    TLOG_IF(DEBUG, false, "TLOG_IF disabled");

    for (int i = 0; i < 5; ++i) {
        TLOG_EVERY_N(INFO, 2, "TLOG_EVERY_N(2) i={}", i);
    }

    TLOG_LEVEL(xlog::level::warn, "TLOG_LEVEL dynamic severity (warn)");

    TLOG_ONCE(INFO, "TLOG_ONCE");
    TLOG_IF_ONCE(INFO, true, "TLOG_IF_ONCE");
    TLOG_FIRST_N(INFO, 2, "TLOG_FIRST_N n={}", 1);
}

// ---------------------------------------------------------------------------
// ZLOG — printf % style
// ---------------------------------------------------------------------------
void demo_zlog() {
    ZLOG(INFO, "ZLOG(INFO) printf message, n=%d", 42);
    ZLOG_IF(DEBUG, true, "ZLOG_IF enabled, value=%d", 1);
    ZLOG_IF(DEBUG, false, "ZLOG_IF disabled, value=%d", 1);

    for (int i = 0; i < 5; ++i) {
        ZLOG_EVERY_N(INFO, 2, "ZLOG_EVERY_N(2) i=%d", i);
    }

    ZLOG_LEVEL(xlog::level::warn, "ZLOG_LEVEL dynamic severity (warn)");

    ZLOG_ONCE(INFO, "ZLOG_ONCE");
    ZLOG_IF_ONCE(INFO, true, "ZLOG_IF_ONCE");
    ZLOG_FIRST_N(INFO, 2, "ZLOG_FIRST_N n=%d", 1);
}

// ---------------------------------------------------------------------------
// PLOG — append errno (XPLOG / TPLOG / ZPLOG)
// ---------------------------------------------------------------------------
void demo_plog() {
#if defined(__linux__) || defined(__APPLE__)
    if (open("/nonexistent_xlog_demo_path", O_RDONLY) == -1) {
        XPLOG(WARNING) << "XPLOG stream open failed";
        TPLOG(WARNING, "TPLOG fmt open failed");
        ZPLOG(WARNING, "ZPLOG printf open failed");
    }
#else
    errno = ENOENT;
    XPLOG(WARNING) << "XPLOG simulated errno";
    TPLOG(WARNING, "TPLOG simulated errno");
    ZPLOG(WARNING, "ZPLOG simulated errno");
#endif
}

// ---------------------------------------------------------------------------
// VLOG — verbose (always at TRACE), gated by vlog_is_on()
// ---------------------------------------------------------------------------
void demo_vlog() {
    if (VXLOG_IS_ON(1)) {
        VXLOG(1) << "VXLOG(1) stream verbose";
    }
    TVLOG(1, "TVLOG(1) fmt verbose, x={}", 1);
    VZLOG(1, "VZLOG(1) printf verbose, x=%d", 1);

    // verbose_level 3 > vlog_level() (2) — should not print
    VXLOG(3) << "VXLOG(3) should not appear";
    TVLOG(3, "TVLOG(3) should not appear");
    VZLOG(3, "VZLOG(3) should not appear");
}

// ---------------------------------------------------------------------------
// CHECK — pass paths only (failure aborts the process)
// ---------------------------------------------------------------------------
void demo_check_pass() {
    XCHECK(true);
    XCHECK_EQ(1, 1);
    XCHECK_STREQ("a", "a");

    TCHECK(true);
    TCHECK(true, "detail {}", 1);
    TCHECK_EQ(2, 2);

    ZCHECK(true);
    ZCHECK(true, "detail %d", 1);
    ZCHECK_EQ(3, 3);
}

#ifndef NDEBUG
// ---------------------------------------------------------------------------
// Debug-only macros (stripped to zero cost in release / NDEBUG)
// ---------------------------------------------------------------------------
void demo_debug_macros() {
    DXLOG(DEBUG) << "DXLOG debug stream";
    DTLOG(DEBUG, "DTLOG debug fmt {}", 1);
    DZLOG(DEBUG, "DZLOG debug printf %d", 1);

    DXPLOG(DEBUG) << "DXPLOG debug stream with perror";
    DTPLOG(DEBUG, "DTPLOG debug fmt");
    DZPLOG(DEBUG, "DZPLOG debug printf");

    DVXLOG(1) << "DVXLOG debug verbose stream";
    DTVLOG(1, "DTVLOG debug verbose fmt");
    DVZLOG(1, "DVZLOG debug verbose printf");

    DXCHECK(true);
    DTCHECK(true);
    DZCHECK(true);
}
#endif

void demo_log_level_filter() {
    const int saved = xlog::log_level();
    xlog::log_level() = XLOG_LEVEL_WARN;

    XLOG(INFO) << "filtered by log_level — should not appear";
    TLOG(INFO, "filtered — should not appear");
    ZLOG(INFO, "filtered — should not appear");

    XLOG(WARNING) << "passes log_level filter";
    TLOG(WARNING, "passes log_level filter");
    ZLOG(WARNING, "passes log_level filter");

    xlog::log_level() = saved;
}

}  // namespace

int main() {
    try {
        setup_demo_logger();

        XLOG(INFO) << "======== xlog macro demo start ========";

        demo_xlog();
        demo_tlog();
        demo_zlog();
        demo_plog();
        demo_vlog();
        demo_check_pass();
#ifndef NDEBUG
        demo_debug_macros();
#endif
        demo_log_level_filter();

        XLOG(INFO) << "======== xlog macro demo done ========";
        xlog::shutdown();
    } catch (const xlog::spdlog_ex &ex) {
        std::fprintf(stderr, "Log initialization failed: %s\n", ex.what());
        return 1;
    }
    return 0;
}
