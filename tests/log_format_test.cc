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
// xlog_format / RFC3339 prefix (xlog-specific).

#include "test_helpers.h"

#include <chrono>
#include <string>
#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/format.h>
#include <xlog/initialize.h>
#include <xlog/log_entry.h>
#include <xlog/log_severity.h>
#include <xlog/utility.h>
#include <xlog/xlog.h>

#include "scoped_mock_log.h"
#include "test_matchers.h"

namespace {

    using ::testing::AllOf;
    using ::testing::HasSubstr;
    using ::testing::MatchesRegex;
    using ::testing::StartsWith;
    using ::xlog::LogEntry;
    using ::xlog::LogSeverity;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;
    using ::xlog::test::FormattedMessage;

    class LogFormatTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            prev_utc_ = xlog::utc();
            prev_prefix_ = xlog::log_with_prefix();
            xlog::set_utc(true);
            xlog::set_log_with_prefix(true);
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
        }

        void TearDown() override {
            xlog::set_utc(prev_utc_);
            xlog::set_log_with_prefix(prev_prefix_);
        }

        static LogEntry MakeEntry(std::string_view msg) {
            LogEntry e;
            e.filename = "/path/to/foo.cc";
            e.line = 42;
            e.log_severity = LogSeverity::kSeverityInfo;
            e.tid = 13982;
            e.pid = 1;
            e.timestamp = std::chrono::system_clock::time_point{
                std::chrono::seconds(1) + std::chrono::microseconds(123456)};
            e.buffer.append(msg.data(), msg.data() + msg.size());
            return e;
        }

        bool prev_utc_{false};
        bool prev_prefix_{true};
    };

    TEST_F(LogFormatTest, UtcPrefixShape) {
        LogEntry e = MakeEntry("hello world");
        xlog::xlog_format(e);
        const std::string out(e.format_buffer.data(), e.format_buffer.size());
        EXPECT_THAT(out, StartsWith("I1970-01-01T00:00:01.123456Z "));
        EXPECT_THAT(out, HasSubstr(" 13982 foo.cc:42] hello world\n"));
    }

    TEST_F(LogFormatTest, ThreadIdentifyInPrefix) {
        LogEntry e = MakeEntry("hi");
        e.thread_identify = "main";
        xlog::xlog_format(e);
        const std::string out(e.format_buffer.data(), e.format_buffer.size());
        EXPECT_THAT(out, HasSubstr(" 13982(main) foo.cc:42] hi\n"));
    }

    TEST_F(LogFormatTest, SeverityLetter) {
        for (auto sev :
             {LogSeverity::kSeverityTrace, LogSeverity::kSeverityDebug,
              LogSeverity::kSeverityInfo, LogSeverity::kSeverityWarning,
              LogSeverity::kSeverityError, LogSeverity::kSeverityFatal}) {
            LogEntry e = MakeEntry("m");
            e.log_severity = sev;
            xlog::xlog_format(e);
            EXPECT_EQ(e.format_buffer[0], xlog::severity_to_upper_char(sev));
        }
    }

    TEST_F(LogFormatTest, NoPrefix) {
        xlog::set_log_with_prefix(false);
        LogEntry e = MakeEntry("plain");
        xlog::xlog_format(e);
        const std::string out(e.format_buffer.data(), e.format_buffer.size());
        EXPECT_EQ(out, "plain\n");
    }

    TEST_F(LogFormatTest, LocalOffsetRegex) {
        xlog::set_utc(false);
        LogEntry e = MakeEntry("x");
        xlog::xlog_format(e);
        const std::string out(e.format_buffer.data(), e.format_buffer.size());
        // LYYYY-MM-DDThh:mm:ss.us±hh:mm ...
        EXPECT_THAT(out, MatchesRegex(
                             "[TDIWEF][0-9]{4}-[0-9]{2}-[0-9]{2}T"
                             "[0-9]{2}:[0-9]{2}:[0-9]{2}\\.[0-9]{6}"
                             "[+-][0-9]{2}:[0-9]{2} .*foo\\.cc:42] x\n"));
    }

    TEST_F(LogFormatTest, ViaXlogMock) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(
            log, Send(FormattedMessage(AllOf(
                     HasSubstr("foo message"),
                     MatchesRegex("I[0-9]{4}-.*] foo message\n")))));
        log.StartCapturingLogs();
        XLOG(INFO) << "foo message";
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
