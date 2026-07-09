// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.

#include "upstream_demo_util.h"

#include <absl/log/absl_log.h>
#include <absl/log/initialize.h>

#include <xlog/upstream/absl.h>

int main() {
    constexpr const char *kLogPath = "logs/upstream_absl.log";
    constexpr const char *kMarker = "absl-upstream-marker-77803";

    auto logger = make_file_logger(kLogPath);
    xlog::set_default_logger(logger);

    absl::InitializeLog();
    xlog::upstream::enable_absl(logger);

    ABSL_LOG(INFO) << kMarker;

    logger->flush();
    xlog::upstream::disable_absl();
    xlog::shutdown();

    return verify_or_fail(kLogPath, kMarker);
}
