// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.

#include "upstream_demo_util.h"

#include <turbo/log/initialize.h>
#include <turbo/log/logging.h>

#include <xlog/upstream/turbo.h>

int main() {
    constexpr const char *kLogPath = "logs/upstream_turbo.log";
    constexpr const char *kMarker = "turbo-upstream-marker-77801";

    auto logger = make_file_logger(kLogPath);
    xlog::set_default_logger(logger);

    turbo::initialize_log();
    xlog::upstream::enable_turbo(logger);

    KLOG(INFO) << kMarker;

    logger->flush();
    xlog::upstream::disable_turbo();
    xlog::shutdown();

    return verify_or_fail(kLogPath, kMarker);
}
