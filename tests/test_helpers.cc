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

#include "test_helpers.h"

#include <csignal>

#include <gtest/gtest.h>
#include <xlog/initialize.h>

namespace xlog {
namespace test {

    bool LoggingEnabledAt(LogSeverity severity) {
        return severity >= kXlogMinLogLevel && severity >= min_log_level();
    }

#if GTEST_HAS_DEATH_TEST

    bool DiedOfFatal(int exit_status) {
#if defined(_WIN32)
        return ::testing::ExitedWithCode(3)(exit_status & 0x7fffffff);
#else
        return ::testing::KilledBySignal(SIGABRT)(exit_status);
#endif
    }

#endif

    void LogTestEnvironment::SetUp() {
        if (!is_initialized()) {
            initialize_log();
        }
    }

}  // namespace test
}  // namespace xlog
