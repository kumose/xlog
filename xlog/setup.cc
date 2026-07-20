// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <xlog/setup.h>

#include <cstdio>
#include <memory>
#include <vector>

#include <fmt/format.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/log_sink_set.h>
#include <xlog/sinks/ansi_color_sink.h>
#include <xlog/sinks/daily_file_sink.h>
#include <xlog/sinks/default_sink.h>
#include <xlog/sinks/hourly_file_sink.h>
#include <xlog/sinks/rotating_file_sink.h>

namespace xlog {
namespace {

    uint32_t InstallDefault(std::unique_ptr<LogSink> sink,
                            LogSeverity stderr_thr) {
        initialize_log();
        const uint32_t id = add_log_sink(std::move(sink));
        set_default_sink(id);
        set_stderr_threshold(stderr_thr);
        return id;
    }

    uint32_t InstallDefault(std::vector<std::unique_ptr<LogSink>> sinks,
                            LogSeverity stderr_thr) {
        initialize_log();
        const uint32_t id = add_log_sinks(std::move(sinks));
        set_default_sink(id);
        set_stderr_threshold(stderr_thr);
        return id;
    }

}  // namespace

    std::string make_default_log_filename(std::string_view argv0) {
        std::string_view base = argv0;
        const auto slash = base.find_last_of("/\\");
        if (slash != std::string_view::npos) {
            base.remove_prefix(slash + 1);
        }
        if (base.empty()) {
            base = "app";
        }
        return fmt::format("logs/{}_log.txt", base);
    }

    uint32_t setup_stderr() {
        // DefaultSink already writes stderr; mirror only FATAL (+ abort).
        return InstallDefault(std::make_unique<DefaultSink>(),
                              LogSeverity::kSeverityFatal);
    }

    uint32_t setup_color_stdout() {
        // Color on stdout; keep ERROR+ mirrored to plain stderr.
        return InstallDefault(std::make_unique<AnsiColorSink>(stdout),
                              LogSeverity::kSeverityError);
    }

    uint32_t setup_color_stderr() {
        return InstallDefault(std::make_unique<AnsiColorSink>(stderr),
                              LogSeverity::kSeverityFatal);
    }

    uint32_t setup_rotating_file(std::string_view base_filename,
                                 int max_file_size_mb, size_t max_files,
                                 int check_interval_s) {
        constexpr size_t kMb = 1024 * 1024;
        const size_t max_bytes =
            max_file_size_mb > 0
                ? static_cast<size_t>(max_file_size_mb) * kMb
                : size_t{0};
        return InstallDefault(
            std::make_unique<RotatingFileSink>(base_filename, max_bytes,
                                               max_files, check_interval_s),
            LogSeverity::kSeverityError);
    }

    uint32_t setup_daily_file(std::string_view base_filename,
                              uint16_t max_files, int check_interval_s,
                              bool truncate) {
        return InstallDefault(
            std::make_unique<DailyFileSink>(base_filename, max_files,
                                            check_interval_s, truncate,
                                            /*utc=*/utc()),
            LogSeverity::kSeverityError);
    }

    uint32_t setup_hourly_file(std::string_view base_filename,
                               uint16_t max_files, int check_interval_s,
                               bool truncate) {
        return InstallDefault(
            std::make_unique<HourlyFileSink>(base_filename, max_files,
                                             check_interval_s, truncate,
                                             /*utc=*/utc()),
            LogSeverity::kSeverityError);
    }

    uint32_t setup_rotating_file_and_color_stderr(
        std::string_view base_filename, int max_file_size_mb, size_t max_files,
        int check_interval_s) {
        constexpr size_t kMb = 1024 * 1024;
        const size_t max_bytes =
            max_file_size_mb > 0
                ? static_cast<size_t>(max_file_size_mb) * kMb
                : size_t{0};
        std::vector<std::unique_ptr<LogSink>> sinks;
        sinks.push_back(std::make_unique<RotatingFileSink>(
            base_filename, max_bytes, max_files, check_interval_s));
        sinks.push_back(std::make_unique<AnsiColorSink>(stderr));
        return InstallDefault(std::move(sinks), LogSeverity::kSeverityFatal);
    }

}  // namespace xlog
