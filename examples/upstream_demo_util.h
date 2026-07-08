// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <xlog/details/os.h>
#include <xlog/sinks/basic_file_sink.h>
#include <xlog/xlog.h>

inline std::shared_ptr<xlog::logger> make_file_logger(const char *log_path) {
    xlog::details::os::create_dir(xlog::details::os::dir_name(XLOG_FILENAME_T(log_path)));
    auto sink = std::make_shared<xlog::sinks::basic_file_sink_mt>(log_path, true);
    auto logger = std::make_shared<xlog::logger>("upstream_demo", sink);
    logger->set_pattern("[%l] %v");
    logger->set_level(xlog::level::trace);
    return logger;
}

inline bool file_contains(const char *path, const std::string &needle) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return content.find(needle) != std::string::npos;
}

inline int verify_or_fail(const char *path, const std::string &needle) {
    if (file_contains(path, needle)) {
        std::cout << "PASS: found \"" << needle << "\" in " << path << std::endl;
        return 0;
    }
    std::cerr << "FAIL: missing \"" << needle << "\" in " << path << std::endl;
    return 1;
}
