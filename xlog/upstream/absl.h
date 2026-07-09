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
// Optional header-only sink: absl::LogEntry -> xlog logger.
// Requires abseil log in the user's build. Not compiled into libxlog.
//
// Usage:
//   #include <xlog/upstream/absl.h>
//   xlog::upstream::enable_absl();

#pragma once

#include <chrono>
#include <memory>

#include <absl/log/log_entry.h>
#include <absl/log/log_sink.h>
#include <absl/log/log_sink_registry.h>
#include <absl/time/time.h>

#include <xlog/upstream/common.h>

namespace xlog {
namespace upstream {

inline xlog::log_clock::time_point to_log_clock(absl::Time time) {
    const auto ns = absl::ToUnixNanos(time);
    return xlog::log_clock::time_point{std::chrono::duration_cast<xlog::log_clock::duration>(
        std::chrono::nanoseconds(ns))};
}

class absl_log_sink final : public absl::LogSink {
public:
    void set_logger(std::shared_ptr<xlog::logger> logger) { logger_ = std::move(logger); }

    void Send(const absl::LogEntry &entry) override {
        if (!logger_) {
            return;
        }

        const xlog::source_loc loc{entry.source_filename().data(), entry.source_line(), ""};
        const auto stamp = to_log_clock(entry.timestamp());
        const auto lvl = to_xlog_level(static_cast<int>(entry.log_severity()));
        const xlog::string_view_t message(entry.text_message().data(), entry.text_message().size());

        submit_log_with_stacktrace(logger_, stamp, loc, lvl, message, entry.stacktrace());
    }

    void Flush() override {
        if (logger_) {
            logger_->flush();
        }
    }

private:
    std::shared_ptr<xlog::logger> logger_;
};

inline absl_log_sink &absl_sink() {
    static absl_log_sink sink;
    return sink;
}

inline void enable_absl(std::shared_ptr<xlog::logger> logger = xlog::default_logger()) {
    absl_sink().set_logger(std::move(logger));
    absl::AddLogSink(&absl_sink());
}

inline void disable_absl() { absl::RemoveLogSink(&absl_sink()); }

}  // namespace upstream
}  // namespace xlog
