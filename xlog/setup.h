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
//
// One-shot helpers: initialize_log + register sink set as default.
//
//   xlog::setup_color_stderr();
//   xlog::setup_rotating_file(xlog::make_default_log_filename(argv[0]));

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace xlog {

    // "logs/<basename>_log.txt" from argv0 (last path component).
    std::string make_default_log_filename(std::string_view argv0);

    // DefaultSink already covers stderr; sets stderr_threshold to FATAL.
    uint32_t setup_stderr();

    // Color stdout; stderr_threshold ERROR (mirror ERROR+ to plain stderr).
    uint32_t setup_color_stdout();
    // Color stderr; stderr_threshold FATAL (avoid double-print).
    uint32_t setup_color_stderr();

    // File sinks: stderr_threshold ERROR (file + ERROR+ stderr mirror).
    // max_file_size_mb: rotate when active file reaches this size.
    uint32_t setup_rotating_file(std::string_view base_filename,
                                 int max_file_size_mb = 100,
                                 size_t max_files = 100,
                                 int check_interval_s = 60);

    uint32_t setup_daily_file(std::string_view base_filename,
                              uint16_t max_files = 7,
                              int check_interval_s = 60, bool truncate = false);

    uint32_t setup_hourly_file(std::string_view base_filename,
                               uint16_t max_files = 84,
                               int check_interval_s = 60, bool truncate = false);

    // File + colored stderr; stderr_threshold FATAL.
    uint32_t setup_rotating_file_and_color_stderr(
        std::string_view base_filename, int max_file_size_mb = 100,
        size_t max_files = 100, int check_interval_s = 60);

}  // namespace xlog
