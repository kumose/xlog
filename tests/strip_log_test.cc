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
// Built with -DXLOG_STRIP_LOG=1: non-fatal logs become NullStream.

#include "scoped_mock_log.h"
#include "test_helpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/logging.h>

#if !defined(XLOG_STRIP_LOG) || !XLOG_STRIP_LOG
#error XLOG_STRIP_LOG=1 required for this test
#endif

namespace {

    using ::testing::_;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;

    class StripLogTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(xlog::LogSeverity::kSeverityInfo);
        }
    };

    TEST_F(StripLogTest, NonFatalIsNullStream) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log).Times(0);
        log.StartCapturingLogs();
        XLOG(INFO) << "stripped";
        XLOG(WARNING) << "stripped";
        XLOG(ERROR) << "stripped";
        TLOG(INFO, "stripped {}", 1);
        ZLOG(ERROR, "stripped %d", 1);
    }

#if GTEST_HAS_DEATH_TEST
    TEST_F(StripLogTest, FatalStillTerminates) {
        // Stripped FATAL uses NullStreamFatal → process exit.
        EXPECT_DEATH(XLOG(FATAL) << "die", "");
    }

    TEST_F(StripLogTest, CheckStillTerminates) {
        EXPECT_DEATH(XCHECK(false), "");
    }
#endif

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
