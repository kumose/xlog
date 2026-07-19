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
// Built with -DXLOG_MAX_VLOG_VERBOSITY=1.

#include "scoped_mock_log.h"
#include "test_helpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/xlog.h>

namespace {

    using ::testing::_;
    using ::testing::Eq;
    using ::xlog::LogSeverity;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;

    class MaxVlogVerbosityTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
            prev_ = xlog::verbosity();
            xlog::set_verbosity(10);  // runtime allows high VLOG
        }

        void TearDown() override { xlog::set_verbosity(prev_); }

        int prev_{0};
    };

    TEST_F(MaxVlogVerbosityTest, LevelOnePasses) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log(LogSeverity::kSeverityInfo, _, Eq("v1")));
        log.StartCapturingLogs();
        XVLOG(1) << "v1";
        EXPECT_TRUE(XVLOG_IS_ON(1));
    }

    TEST_F(MaxVlogVerbosityTest, LevelTwoCompileGated) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log).Times(0);
        log.StartCapturingLogs();
        XVLOG(2) << "v2 stripped";
        EXPECT_FALSE(XVLOG_IS_ON(2));
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
