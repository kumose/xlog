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
// Basic XLOG/TLOG/ZLOG coverage (adapted from turbo tests/log/log_basic_test).

#include "scoped_mock_log.h"
#include "test_helpers.h"
#include "test_matchers.h"

#include <sstream>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/tlog.h>
#include <xlog/xlog.h>
#include <xlog/zlog.h>

namespace {

    using ::testing::_;
    using ::testing::AllOf;
    using ::testing::Eq;
    using ::xlog::LogSeverity;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;
    using ::xlog::test::LoggingEnabledAt;
    using ::xlog::test::ScopedMinLogLevel;
    using ::xlog::test::Severity;
    using ::xlog::test::SourceBasename;
    using ::xlog::test::SourceFilename;
    using ::xlog::test::SourceLine;
    using ::xlog::test::TextMessage;

#if GTEST_HAS_DEATH_TEST
    using ::xlog::test::DiedOfFatal;
#endif

    class BasicLogTest : public ::testing::TestWithParam<LogSeverity> {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
        }
    };

    std::string ThresholdName(testing::TestParamInfo<LogSeverity> severity) {
        std::ostringstream ostr;
        ostr << severity.param;
        return ostr.str();
    }

    INSTANTIATE_TEST_SUITE_P(WithParam, BasicLogTest,
                             testing::Values(LogSeverity::kSeverityInfo,
                                             LogSeverity::kSeverityWarning,
                                             LogSeverity::kSeverityError,
                                             LogSeverity::kSeverityFatal),
                             ThresholdName);

    TEST_P(BasicLogTest, Info) {
        ScopedMinLogLevel scoped_min(GetParam());
        ScopedMockLog test_sink(MockLogDefault::kDisallowUnexpected);

        const int log_line = __LINE__ + 1;
        auto do_log = [] { XLOG(INFO) << "hello world"; };

        if (LoggingEnabledAt(LogSeverity::kSeverityInfo)) {
            EXPECT_CALL(
                test_sink,
                Send(AllOf(SourceFilename(Eq(__FILE__)),
                           SourceBasename(Eq("log_basic_test.cc")),
                           SourceLine(Eq(log_line)),
                           Severity(Eq(LogSeverity::kSeverityInfo)),
                           TextMessage(Eq("hello world")))));
        }

        test_sink.StartCapturingLogs();
        do_log();
    }

    TEST_P(BasicLogTest, Warning) {
        ScopedMinLogLevel scoped_min(GetParam());
        ScopedMockLog test_sink(MockLogDefault::kDisallowUnexpected);

        const int log_line = __LINE__ + 1;
        auto do_log = [] { XLOG(WARNING) << "hello world"; };

        if (LoggingEnabledAt(LogSeverity::kSeverityWarning)) {
            EXPECT_CALL(
                test_sink,
                Send(AllOf(SourceFilename(Eq(__FILE__)),
                           SourceBasename(Eq("log_basic_test.cc")),
                           SourceLine(Eq(log_line)),
                           Severity(Eq(LogSeverity::kSeverityWarning)),
                           TextMessage(Eq("hello world")))));
        }

        test_sink.StartCapturingLogs();
        do_log();
    }

    TEST_P(BasicLogTest, Error) {
        ScopedMinLogLevel scoped_min(GetParam());
        ScopedMockLog test_sink(MockLogDefault::kDisallowUnexpected);

        const int log_line = __LINE__ + 1;
        auto do_log = [] { XLOG(ERROR) << "hello world"; };

        if (LoggingEnabledAt(LogSeverity::kSeverityError)) {
            EXPECT_CALL(
                test_sink,
                Send(AllOf(SourceFilename(Eq(__FILE__)),
                           SourceBasename(Eq("log_basic_test.cc")),
                           SourceLine(Eq(log_line)),
                           Severity(Eq(LogSeverity::kSeverityError)),
                           TextMessage(Eq("hello world")))));
        }

        test_sink.StartCapturingLogs();
        do_log();
    }

    TEST_P(BasicLogTest, Level) {
        ScopedMinLogLevel scoped_min(GetParam());

        for (auto severity : {LogSeverity::kSeverityInfo,
                              LogSeverity::kSeverityWarning,
                              LogSeverity::kSeverityError}) {
            ScopedMockLog test_sink(MockLogDefault::kDisallowUnexpected);

            const int log_line = __LINE__ + 2;
            auto do_log = [severity] {
                XLOG(LEVEL(severity)) << "hello world";
            };

            if (LoggingEnabledAt(severity)) {
                EXPECT_CALL(
                    test_sink,
                    Send(AllOf(SourceFilename(Eq(__FILE__)),
                               SourceBasename(Eq("log_basic_test.cc")),
                               SourceLine(Eq(log_line)),
                               Severity(Eq(severity)),
                               TextMessage(Eq("hello world")))));
            }

            test_sink.StartCapturingLogs();
            do_log();
        }
    }

    TEST_P(BasicLogTest, TlogAndZlog) {
        ScopedMinLogLevel scoped_min(GetParam());
        ScopedMockLog test_sink(MockLogDefault::kDisallowUnexpected);

        if (LoggingEnabledAt(LogSeverity::kSeverityInfo)) {
            EXPECT_CALL(test_sink,
                        Send(AllOf(Severity(Eq(LogSeverity::kSeverityInfo)),
                                   TextMessage(Eq("fmt 1")))));
            EXPECT_CALL(test_sink,
                        Send(AllOf(Severity(Eq(LogSeverity::kSeverityInfo)),
                                   TextMessage(Eq("printf 2")))));
        }

        test_sink.StartCapturingLogs();
        TLOG(INFO, "fmt {}", 1);
        ZLOG(INFO, "printf %d", 2);
    }

#if GTEST_HAS_DEATH_TEST
    using BasicLogDeathTest = BasicLogTest;

    INSTANTIATE_TEST_SUITE_P(WithParam, BasicLogDeathTest,
                             testing::Values(LogSeverity::kSeverityInfo,
                                             LogSeverity::kSeverityFatal),
                             ThresholdName);

    TEST_P(BasicLogDeathTest, Fatal) {
        ScopedMinLogLevel scoped_min(GetParam());

        const int log_line = __LINE__ + 1;
        auto do_log = [] { XLOG(FATAL) << "hello world"; };

        EXPECT_EXIT(
            {
                ScopedMockLog test_sink(MockLogDefault::kDisallowUnexpected);
                // Default Send→Log is Times(0) under kDisallowUnexpected; allow
                // the FATAL send we expect, ignore nothing else.
                if (LoggingEnabledAt(LogSeverity::kSeverityFatal)) {
                    EXPECT_CALL(
                        test_sink,
                        Send(AllOf(
                            SourceFilename(Eq(__FILE__)),
                            SourceBasename(Eq("log_basic_test.cc")),
                            SourceLine(Eq(log_line)),
                            Severity(Eq(LogSeverity::kSeverityFatal)),
                            TextMessage(Eq("hello world")))))
                        .Times(1);
                }
                test_sink.StartCapturingLogs();
                do_log();
            },
            DiedOfFatal, "");
    }
#endif

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
