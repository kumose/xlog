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
// Built with -DXLOG_MIN_LOG_LEVEL=3 (WARNING): INFO is compile-gated off.

#include "scoped_mock_log.h"
#include "test_helpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/logging.h>

#ifndef XLOG_MIN_LOG_LEVEL
#error XLOG_MIN_LOG_LEVEL must be defined for this test
#endif

namespace {

    using ::testing::_;
    using ::testing::Eq;
    using ::xlog::LogSeverity;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;

    class MinLogLevelTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            // Runtime threshold must not block WARNING+ that we expect.
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
        }
    };

    TEST_F(MinLogLevelTest, InfoCompileGatedOff) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log).Times(0);
        log.StartCapturingLogs();
        XLOG(INFO) << "should not emit";
        TLOG(INFO, "should not emit {}", 1);
        ZLOG(INFO, "should not emit %d", 1);
    }

    TEST_F(MinLogLevelTest, WarningStillLogs) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log(LogSeverity::kSeverityWarning, _, Eq("warn ok")));
        log.StartCapturingLogs();
        XLOG(WARNING) << "warn ok";
    }

#if GTEST_HAS_DEATH_TEST
    TEST_F(MinLogLevelTest, FatalStillAborts) {
        EXPECT_DEATH(XLOG(FATAL) << "die", "");
    }
#endif

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
