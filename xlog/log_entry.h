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

#pragma once

#include <fmt/format.h>
#include <string_view>
#include <xlog/log_severity.h>
#include <chrono>

namespace xlog {
    struct LogEntry {
        std::string_view filename;
        int line{0};
        LogSeverity log_severity;
        std::string_view thread_identify;
        uint64_t pid{0};
        uint64_t tid{0};
        fmt::memory_buffer buffer;
        uint32_t verbose_level{0};
        bool prefix{true};  // false after LogMessage::no_prefix()
        std::chrono::time_point<std::chrono::system_clock> timestamp;

        fmt::memory_buffer format_buffer;
    };
} // namespace xlog
