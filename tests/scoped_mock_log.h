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
// Captures XLOG/TLOG/ZLOG via a temporary default LogSinkSet (non-owning).

#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include <gmock/gmock.h>
#include <xlog/log_entry.h>
#include <xlog/log_severity.h>
#include <xlog/log_sink.h>

namespace xlog {

    enum class MockLogDefault { kIgnoreUnexpected, kDisallowUnexpected };

    // Intercepts log traffic while capturing is active by switching the
    // registry default sink set to a ForwardingSink owned by this object.
    //
    //   ScopedMockLog log;
    //   EXPECT_CALL(log, Log(LogSeverity::kSeverityInfo, _, "hi"));
    //   log.StartCapturingLogs();
    //   XLOG(INFO) << "hi";
    class ScopedMockLog final {
    public:
        explicit ScopedMockLog(
            MockLogDefault default_exp = MockLogDefault::kIgnoreUnexpected);
        ScopedMockLog(const ScopedMockLog &) = delete;
        ScopedMockLog &operator=(const ScopedMockLog &) = delete;
        ~ScopedMockLog();

        void StartCapturingLogs();
        void StopCapturingLogs();

        // For XLOG(...).to_sink_only(&log.UseAsLocalSink()).
        LogSink &UseAsLocalSink();

        MOCK_METHOD(void, Log,
                    (LogSeverity severity, const std::string &file_path,
                     const std::string &message));

        MOCK_METHOD(void, Send, (const LogEntry &));

        MOCK_METHOD(void, Flush, ());

    private:
        class ForwardingSink final : public LogSink {
        public:
            explicit ForwardingSink(ScopedMockLog *sml) : sml_(sml) {}
            ForwardingSink(const ForwardingSink &) = delete;
            ForwardingSink &operator=(const ForwardingSink &) = delete;
            void send(const LogEntry &entry) override { sml_->Send(entry); }
            void flush() override { sml_->Flush(); }

        private:
            ScopedMockLog *sml_;
        };

        ForwardingSink sink_;
        bool is_capturing_logs_{false};
        std::atomic<bool> is_triggered_{false};
        uint32_t previous_default_id_{0};
        uint32_t mock_sink_id_{0};
        bool mock_sink_registered_{false};
    };

}  // namespace xlog
