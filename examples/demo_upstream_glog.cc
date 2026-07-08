// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.

#include "upstream_demo_util.h"

#include <glog/logging.h>

#include <xlog/upstream/glog.h>

int main(int argc, char **argv) {
    constexpr const char *kLogPath = "logs/upstream_glog.log";
    constexpr const char *kMarker = "glog-upstream-marker-77802";

    auto logger = make_file_logger(kLogPath);
    xlog::set_default_logger(logger);

    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = false;
    FLAGS_log_dir = "";

    xlog::upstream::enable_glog(logger);

    LOG(INFO) << kMarker;

    logger->flush();
    xlog::upstream::disable_glog();
    google::ShutdownGoogleLogging();
    xlog::shutdown();

    return verify_or_fail(kLogPath, kMarker);
}
