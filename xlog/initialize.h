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

#include <xlog/log_severity.h>
#include <string>
#include <shared_mutex>

namespace xlog {
    struct LogConfig {
        static LogConfig &instance() {
            static LogConfig config;
            return config;
        }

        std::shared_mutex log_mutex;
        LogSeverity stderr_threshold;
        LogSeverity min_log_level;

        bool log_with_prefix{true};
        bool log_truncate{false};
        int verbosity{0};
        std::string log_base_filename;

        bool utc{false};
        bool initialized{false};
        std::string app_name;
    };

    LogSeverity stderr_threshold();

    bool is_initialized();
} // namespace xlog
