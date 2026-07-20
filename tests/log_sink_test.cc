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
// LogSinkRegistry + to_sink_also / to_sink_only.

#include "scoped_mock_log.h"
#include "test_helpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/log_sink_set.h>
#include <xlog/xlog.h>

namespace {

    using ::testing::_;
    using ::testing::Eq;
    using ::xlog::LogSeverity;
    using ::xlog::MockLogDefault;
    using ::xlog::ScopedMockLog;

    class LogSinkTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
            saved_default_ = xlog::LogSinkRegistry::instance().default_sink_id();
        }

        void TearDown() override {
            xlog::LogSinkRegistry::instance().set_default(saved_default_);
        }

        uint32_t saved_default_{0};
    };

    TEST_F(LogSinkTest, StartStopCapturing) {
        ScopedMockLog test_sink(MockLogDefault::kDisallowUnexpected);

        EXPECT_CALL(test_sink, Log(_, _, "hello world")).Times(0);
        EXPECT_CALL(test_sink, Log(LogSeverity::kSeverityInfo, _, Eq("Test : 42")));
        EXPECT_CALL(test_sink,
                    Log(LogSeverity::kSeverityWarning, _, Eq("Danger ahead")));

        XLOG(INFO) << "hello world";
        test_sink.StartCapturingLogs();
        XLOG(INFO) << "Test : " << 42;
        XLOG(WARNING) << "Danger" << ' ' << "ahead";
        test_sink.StopCapturingLogs();
        XLOG(INFO) << "Goodbye world";
    }

    TEST_F(LogSinkTest, SwitchDefaultBetweenSinks) {
        ScopedMockLog sink1(MockLogDefault::kDisallowUnexpected);
        ScopedMockLog sink2(MockLogDefault::kDisallowUnexpected);

        EXPECT_CALL(sink1, Log(LogSeverity::kSeverityInfo, _, Eq("First")));
        EXPECT_CALL(sink2, Log(LogSeverity::kSeverityInfo, _, Eq("First")))
            .Times(0);
        EXPECT_CALL(sink1, Log(LogSeverity::kSeverityInfo, _, Eq("Second")))
            .Times(0);
        EXPECT_CALL(sink2, Log(LogSeverity::kSeverityInfo, _, Eq("Second")));

        sink1.StartCapturingLogs();
        XLOG(INFO) << "First";
        sink1.StopCapturingLogs();

        sink2.StartCapturingLogs();
        XLOG(INFO) << "Second";
        sink2.StopCapturingLogs();
    }

    TEST_F(LogSinkTest, ToSinkOnly) {
        ScopedMockLog global_sink(MockLogDefault::kDisallowUnexpected);
        ScopedMockLog local_sink(MockLogDefault::kDisallowUnexpected);

        EXPECT_CALL(global_sink, Log).Times(0);
        EXPECT_CALL(local_sink,
                    Log(LogSeverity::kSeverityInfo, _, Eq("only local")));

        global_sink.StartCapturingLogs();
        XLOG(INFO).to_sink_only(&local_sink.UseAsLocalSink()) << "only local";
    }

    TEST_F(LogSinkTest, ToSinkAlso) {
        ScopedMockLog global_sink(MockLogDefault::kDisallowUnexpected);
        ScopedMockLog also_sink(MockLogDefault::kDisallowUnexpected);

        EXPECT_CALL(global_sink,
                    Log(LogSeverity::kSeverityInfo, _, Eq("both")));
        EXPECT_CALL(also_sink, Log(LogSeverity::kSeverityInfo, _, Eq("both")));

        global_sink.StartCapturingLogs();
        XLOG(INFO).to_sink_also(&also_sink.UseAsLocalSink()) << "both";
    }

    TEST_F(LogSinkTest, SetDefaultById) {
        ScopedMockLog sink(MockLogDefault::kDisallowUnexpected);
        auto &reg = xlog::LogSinkRegistry::instance();
        const uint32_t id = reg.add_log_sinks({&sink.UseAsLocalSink()});
        EXPECT_TRUE(reg.contains(id));
        EXPECT_TRUE(reg.set_default(id));
        EXPECT_EQ(reg.default_sink_id(), id);

        EXPECT_CALL(sink, Log(LogSeverity::kSeverityInfo, _, Eq("via id")));
        XLOG(INFO) << "via id";

        EXPECT_TRUE(reg.set_default(saved_default_));
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
