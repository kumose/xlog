// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Minimal xlog setup: default console logger + runtime log_level / vlog_level.

#include <xlog/xlog.h>
#include <xlog/logging.h>
#include <xlog/log_setting.h>

int main() {
    // Default logger is stdout_color_mt; set runtime verbosity.
    xlog::log_level() = XLOG_LEVEL_INFO;
    xlog::vlog_level() = 1;

    XLOG(INFO) << "hello from XLOG(INFO)";
    TLOG(INFO, "hello from TLOG(INFO, \"{}\")", "fmt");
    ZLOG(INFO, "hello from ZLOG(INFO, \"%s\")", "printf");

    TVLOG(1, "verbose fmt when vlog_level >= 1");
    TVLOG(2, "should not print when vlog_level is 1");

    xlog::shutdown();
    return 0;
}
