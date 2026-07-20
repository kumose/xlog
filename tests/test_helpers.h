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

#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>

namespace xlog {
namespace test {

#ifdef XLOG_MIN_LOG_LEVEL
    constexpr auto kXlogMinLogLevel =
        static_cast<LogSeverity>(XLOG_MIN_LOG_LEVEL);
#else
    constexpr auto kXlogMinLogLevel = LogSeverity::kSeverityInfo;
#endif

    bool LoggingEnabledAt(LogSeverity severity);

    // RAII restore of min_log_level.
    class ScopedMinLogLevel {
    public:
        explicit ScopedMinLogLevel(LogSeverity severity)
            : previous_(min_log_level()) {
            set_min_log_level(severity);
        }
        ScopedMinLogLevel(const ScopedMinLogLevel &) = delete;
        ScopedMinLogLevel &operator=(const ScopedMinLogLevel &) = delete;
        ~ScopedMinLogLevel() { set_min_log_level(previous_); }

    private:
        LogSeverity previous_;
    };

    // RAII restore of stderr_threshold.
    class ScopedStderrThreshold {
    public:
        explicit ScopedStderrThreshold(LogSeverity severity)
            : previous_(stderr_threshold()) {
            set_stderr_threshold(severity);
        }
        ScopedStderrThreshold(const ScopedStderrThreshold &) = delete;
        ScopedStderrThreshold &operator=(const ScopedStderrThreshold &) = delete;
        ~ScopedStderrThreshold() { set_stderr_threshold(previous_); }

    private:
        LogSeverity previous_;
    };

#if GTEST_HAS_DEATH_TEST
    bool DiedOfFatal(int exit_status);
#endif

    class LogTestEnvironment : public ::testing::Environment {
    public:
        ~LogTestEnvironment() override = default;
        void SetUp() override;
    };

}  // namespace test
}  // namespace xlog
