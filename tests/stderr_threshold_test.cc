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
// LogSinkSet stderr mirror + FATAL abort under stderr_threshold.

#include "test_helpers.h"

#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/log_sink_set.h>
#include <xlog/sinks/null_sink.h>
#include <xlog/xlog.h>

namespace {

    using ::testing::HasSubstr;
    using ::testing::Not;
    using ::xlog::LogSeverity;
    using ::xlog::test::ScopedMinLogLevel;
    using ::xlog::test::ScopedStderrThreshold;

    class StderrThresholdTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(LogSeverity::kSeverityInfo);
            saved_default_ = xlog::LogSinkRegistry::instance().default_sink_id();
            saved_threshold_ = xlog::stderr_threshold();

            auto null = std::make_unique<xlog::NullSink>();
            null_id_ = xlog::add_log_sink(std::move(null));
            ASSERT_TRUE(xlog::set_default_sink(null_id_));
        }

        void TearDown() override {
            xlog::LogSinkRegistry::instance().set_default(saved_default_);
            xlog::set_stderr_threshold(saved_threshold_);
        }

        uint32_t saved_default_{0};
        uint32_t null_id_{0};
        LogSeverity saved_threshold_{LogSeverity::kSeverityFatal};
    };

    TEST_F(StderrThresholdTest, MirrorsAtOrAboveThreshold) {
        ScopedStderrThreshold thr(LogSeverity::kSeverityError);

        ::testing::internal::CaptureStderr();
        XLOG(INFO) << "info_not_mirrored";
        XLOG(WARNING) << "warn_not_mirrored";
        XLOG(ERROR) << "error_mirrored_marker";
        const std::string err = ::testing::internal::GetCapturedStderr();

        EXPECT_THAT(err, Not(HasSubstr("info_not_mirrored")));
        EXPECT_THAT(err, Not(HasSubstr("warn_not_mirrored")));
        EXPECT_THAT(err, HasSubstr("error_mirrored_marker"));
    }

    TEST_F(StderrThresholdTest, FatalThresholdSkipsErrorMirror) {
        ScopedStderrThreshold thr(LogSeverity::kSeverityFatal);

        ::testing::internal::CaptureStderr();
        XLOG(ERROR) << "error_not_mirrored";
        const std::string err = ::testing::internal::GetCapturedStderr();

        EXPECT_THAT(err, Not(HasSubstr("error_not_mirrored")));
    }

#if GTEST_HAS_DEATH_TEST
    TEST_F(StderrThresholdTest, FatalAbortsEvenWithFatalThreshold) {
        ScopedStderrThreshold thr(LogSeverity::kSeverityFatal);
        ScopedMinLogLevel min_lv(LogSeverity::kSeverityInfo);

        EXPECT_DEATH(XLOG(FATAL) << "fatal_via_do_log",
                     HasSubstr("fatal_via_do_log"));
    }
#endif

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
