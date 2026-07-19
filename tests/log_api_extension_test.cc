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
// P0/P1: no_prefix, DFATAL, XVLOG, XPLOG / with_perror.

#include "scoped_mock_log.h"
#include "test_helpers.h"
#include "test_matchers.h"

#include <cerrno>
#include <cstring>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/internal/strerror.h>
#include <xlog/log_severity.h>
#include <xlog/logging.h>

namespace {

    using ::testing::_;
    using ::testing::AllOf;
    using ::testing::Eq;
    using ::testing::HasSubstr;
    using ::testing::Not;
    using ::xlog::LogSeverity;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;
    using ::xlog::test::FormattedMessage;
    using ::xlog::test::Severity;
    using ::xlog::test::TextMessage;

    class LogApiExtensionTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
            prev_verbosity_ = xlog::verbosity();
            xlog::set_verbosity(0);
            xlog::set_log_with_prefix(true);
        }

        void TearDown() override { xlog::set_verbosity(prev_verbosity_); }

        int prev_verbosity_{0};
    };

    TEST_F(LogApiExtensionTest, NoPrefix) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Send(AllOf(TextMessage(Eq("raw line")),
                                    FormattedMessage(Eq("raw line\n")))));
        log.StartCapturingLogs();
        XLOG(INFO).no_prefix() << "raw line";
    }

    TEST_F(LogApiExtensionTest, DfatalSeverityMatchesConstant) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Send(Severity(Eq(xlog::kLogDebugFatal))));
        log.StartCapturingLogs();
#ifndef NDEBUG
        // Would abort — skip body logging path in this binary via LEVEL instead.
        XLOG(LEVEL(xlog::kLogDebugFatal)) << "via level";
#else
        XLOG(DFATAL) << "dfatal release is error";
#endif
    }

#ifndef NDEBUG
#if GTEST_HAS_DEATH_TEST
    TEST_F(LogApiExtensionTest, DfatalAbortsInDebug) {
        EXPECT_DEATH(XLOG(DFATAL) << "die", "");
    }
#endif
#else
    TEST_F(LogApiExtensionTest, DfatalIsErrorInRelease) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Send(AllOf(Severity(Eq(LogSeverity::kSeverityError)),
                                    TextMessage(Eq("dfatal err")))));
        log.StartCapturingLogs();
        XLOG(DFATAL) << "dfatal err";
    }
#endif

    TEST_F(LogApiExtensionTest, VlogGated) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Log).Times(0);
        log.StartCapturingLogs();
        XVLOG(2) << "hidden";
    }

    TEST_F(LogApiExtensionTest, VlogEnabled) {
        xlog::set_verbosity(2);
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        EXPECT_CALL(log, Send(AllOf(Severity(Eq(LogSeverity::kSeverityInfo)),
                                    TextMessage(Eq("seen")))));
        log.StartCapturingLogs();
        XVLOG(2) << "seen";
        EXPECT_TRUE(XVLOG_IS_ON(2));
        EXPECT_FALSE(XVLOG_IS_ON(3));
    }

    TEST_F(LogApiExtensionTest, PlogAppendsErrno) {
        ScopedMockLog log(MockLogDefault::kDisallowUnexpected);
        errno = ENOENT;
        const std::string expected =
            std::string("open failed: ") +
            xlog::log_internal::StrError(ENOENT) + " [" +
            std::to_string(ENOENT) + "]";
        EXPECT_CALL(log, Send(TextMessage(Eq(expected))));
        log.StartCapturingLogs();
        errno = ENOENT;
        XPLOG(ERROR) << "open failed";
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
