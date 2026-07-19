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

#include "scoped_mock_log.h"

#include <cstdlib>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/log_sink_set.h>

namespace xlog {

    ScopedMockLog::ScopedMockLog(MockLogDefault default_exp)
        : sink_(this), is_capturing_logs_(false), is_triggered_(false) {
        if (default_exp == MockLogDefault::kIgnoreUnexpected) {
            EXPECT_CALL(*this, Log).Times(::testing::AnyNumber());
        } else {
            EXPECT_CALL(*this, Log).Times(0);
        }
        EXPECT_CALL(*this, Send)
            .Times(::testing::AnyNumber())
            .WillRepeatedly([this](const LogEntry &entry) {
                is_triggered_.store(true, std::memory_order_relaxed);
                Log(entry.log_severity, std::string(entry.filename),
                    std::string(entry.buffer.data(), entry.buffer.size()));
            });
        EXPECT_CALL(*this, Flush).Times(::testing::AnyNumber());
    }

    ScopedMockLog::~ScopedMockLog() {
        EXPECT_TRUE(is_triggered_.load(std::memory_order_relaxed))
            << "Did you forget to call StartCapturingLogs() or UseAsLocalSink()?";
        if (is_capturing_logs_) {
            StopCapturingLogs();
        }
    }

    void ScopedMockLog::StartCapturingLogs() {
        ASSERT_FALSE(is_capturing_logs_)
            << "StartCapturingLogs() called while already capturing";

        auto &registry = LogSinkRegistry::instance();
        previous_default_id_ = registry.default_sink_id();
        if (!mock_sink_registered_) {
            mock_sink_id_ = registry.add_log_sinks({&sink_});
            mock_sink_registered_ = true;
        }
        ASSERT_TRUE(registry.set_default(mock_sink_id_));

        is_capturing_logs_ = true;
        is_triggered_.store(true, std::memory_order_relaxed);
    }

    void ScopedMockLog::StopCapturingLogs() {
        ASSERT_TRUE(is_capturing_logs_)
            << "StopCapturingLogs() called while not capturing";

        auto &registry = LogSinkRegistry::instance();
        ASSERT_TRUE(registry.set_default(previous_default_id_));
        is_capturing_logs_ = false;
    }

    LogSink &ScopedMockLog::UseAsLocalSink() {
        is_triggered_.store(true, std::memory_order_relaxed);
        return sink_;
    }

}  // namespace xlog
