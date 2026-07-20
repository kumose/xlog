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
// Smoke test for ScopedMockLog + matchers (test harness step 1).

#include "scoped_mock_log.h"
#include "test_helpers.h"
#include "test_matchers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/tlog.h>
#include <xlog/xlog.h>
#include <xlog/zlog.h>

namespace {

    using ::testing::_;
    using ::testing::AllOf;
    using ::testing::Eq;
    using ::xlog::LogSeverity;
    using ::xlog::ScopedMockLog;
    using ::xlog::test::Severity;
    using ::xlog::test::TextMessage;

    class ScopedMockLogTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
        }
    };

    TEST_F(ScopedMockLogTest, CapturesXlogInfo) {
        ScopedMockLog log;
        EXPECT_CALL(log, Log(LogSeverity::kSeverityInfo, _, Eq("hello stream")));
        log.StartCapturingLogs();
        XLOG(INFO) << "hello stream";
    }

    TEST_F(ScopedMockLogTest, CapturesTlogAndZlog) {
        ScopedMockLog log;
        EXPECT_CALL(log, Log(LogSeverity::kSeverityWarning, _, Eq("fmt 42")));
        EXPECT_CALL(log, Log(LogSeverity::kSeverityError, _, Eq("printf 7")));
        log.StartCapturingLogs();
        TLOG(WARNING, "fmt {}", 42);
        ZLOG(ERROR, "printf %d", 7);
    }

    TEST_F(ScopedMockLogTest, SendMatcherSeesBuffer) {
        ScopedMockLog log;
        EXPECT_CALL(log, Send(AllOf(Severity(LogSeverity::kSeverityInfo),
                                    TextMessage(Eq("via send")))));
        log.StartCapturingLogs();
        XLOG(INFO) << "via send";
    }

    TEST_F(ScopedMockLogTest, UseAsLocalSinkOnly) {
        ScopedMockLog log;
        EXPECT_CALL(log, Log(LogSeverity::kSeverityInfo, _, Eq("local only")));
        // Do not StartCapturingLogs — only the attached sink should see this.
        XLOG(INFO).to_sink_only(&log.UseAsLocalSink()) << "local only";
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
