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
// XLOG_IF / EVERY_N / FIRST_N / EVERY_POW_2 / EVERY_N_SEC.

#include "scoped_mock_log.h"
#include "test_helpers.h"

#include <chrono>
#include <thread>

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

    class LogConditionsTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
        }
    };

    TEST_F(LogConditionsTest, IfFalseSkips) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log).Times(0);
        log.StartCapturingLogs();
        XLOG_IF(INFO, false) << "nope";
    }

    TEST_F(LogConditionsTest, IfTrueLogs) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log(LogSeverity::kSeverityInfo, _, Eq("yes")));
        log.StartCapturingLogs();
        XLOG_IF(INFO, true) << "yes";
    }

    TEST_F(LogConditionsTest, EveryN) {
        // Logs when counter % 3 == 0 → hits 0,3,6 (3 of 7 iterations).
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log(LogSeverity::kSeverityInfo, _, _))
            .Times(3);
        log.StartCapturingLogs();
        for (int i = 0; i < 7; ++i) {
            XLOG_EVERY_N(INFO, 3) << "n=" << COUNTER;
        }
    }

    TEST_F(LogConditionsTest, FirstN) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log(LogSeverity::kSeverityWarning, _, _)).Times(2);
        log.StartCapturingLogs();
        for (int i = 0; i < 5; ++i) {
            XLOG_FIRST_N(WARNING, 2) << "first " << COUNTER;
        }
    }

    TEST_F(LogConditionsTest, EveryPow2) {
        // counter after LossyIncrement+1: 1,2,3,4,5 → pow2 at 1,2,4 (3 times).
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log(LogSeverity::kSeverityError, _, _)).Times(3);
        log.StartCapturingLogs();
        for (int i = 0; i < 5; ++i) {
            XLOG_EVERY_POW_2(ERROR) << "p2 " << COUNTER;
        }
    }

    TEST_F(LogConditionsTest, EveryNSec) {
        // Same macro site shares one static state (separate lines do not).
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log(LogSeverity::kSeverityInfo, _, Eq("tick")))
            .Times(2);
        log.StartCapturingLogs();
        auto tick = [] { XLOG_EVERY_N_SEC(INFO, 0.05) << "tick"; };
        tick();
        tick();  // same window → skip
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        tick();
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
