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
// at_location / with_verbosity / to_sink_*.

#include "scoped_mock_log.h"
#include "test_helpers.h"
#include "test_matchers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/xlog.h>

namespace {

    using ::testing::_;
    using ::testing::AllOf;
    using ::testing::Eq;
    using ::testing::HasSubstr;
    using ::xlog::LogSeverity;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;
    using ::xlog::test::FormattedMessage;
    using ::xlog::test::Severity;
    using ::xlog::test::SourceBasename;
    using ::xlog::test::SourceFilename;
    using ::xlog::test::SourceLine;
    using ::xlog::test::TextMessage;
    using ::xlog::test::Verbosity;

    class LogModifierMethodsTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
            prev_verbosity_ = xlog::verbosity();
            xlog::set_verbosity(0);
        }

        void TearDown() override { xlog::set_verbosity(prev_verbosity_); }

        int prev_verbosity_{0};
    };

    TEST_F(LogModifierMethodsTest, AtLocationFileLine) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(
            log,
            Send(AllOf(
                SourceFilename(Eq("/my/very/very/very_long_source_file.cc")),
                SourceBasename(Eq("very_long_source_file.cc")),
                SourceLine(Eq(777)), TextMessage(Eq("hello world")),
                FormattedMessage(HasSubstr("very_long_source_file.cc:777]")))));
        log.StartCapturingLogs();
        XLOG(INFO)
            .at_location("/my/very/very/very_long_source_file.cc", 777)
            << "hello world";
    }

    TEST_F(LogModifierMethodsTest, WithVerbosityPassesWhenEnabled) {
        xlog::set_verbosity(3);
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Send(AllOf(Verbosity(Eq(2u)),
                                    TextMessage(Eq("vok")))));
        log.StartCapturingLogs();
        XLOG(INFO).with_verbosity(2) << "vok";
    }

    TEST_F(LogModifierMethodsTest, WithVerbosityFiltered) {
        xlog::set_verbosity(1);
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log).Times(0);
        log.StartCapturingLogs();
        XLOG(INFO).with_verbosity(2) << "filtered";
    }

    TEST_F(LogModifierMethodsTest, ToSinkAlsoAndOnly) {
        ScopedMockLog primary(MockLogDefault::kDisallowUnexpected);
        ScopedMockLog also(MockLogDefault::kDisallowUnexpected);
        ScopedMockLog only(MockLogDefault::kDisallowUnexpected);

        EXPECT_CALL(primary, Log(LogSeverity::kSeverityInfo, _, Eq("also")));
        EXPECT_CALL(also, Log(LogSeverity::kSeverityInfo, _, Eq("also")));
        EXPECT_CALL(primary, Log(_, _, Eq("only"))).Times(0);
        EXPECT_CALL(only, Log(LogSeverity::kSeverityInfo, _, Eq("only")));

        primary.StartCapturingLogs();
        XLOG(INFO).to_sink_also(&also.UseAsLocalSink()) << "also";
        XLOG(INFO).to_sink_only(&only.UseAsLocalSink()) << "only";
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
