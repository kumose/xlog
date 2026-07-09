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
// Optional header-only sink: turbo::LogEntry -> xlog logger.
// Requires turbo in the user's build. Not compiled into libxlog.
//
// Usage:
//   #include <xlog/upstream/turbo.h>
//   xlog::upstream::enable_turbo();

#pragma once

#include <memory>

#include <turbo/log/log_entry.h>
#include <turbo/log/log_sink.h>
#include <turbo/log/log_sink_registry.h>
#include <turbo/times/time.h>

#include <xlog/upstream/common.h>

namespace xlog {
namespace upstream {

class turbo_log_sink final : public turbo::LogSink {
public:
    void set_logger(std::shared_ptr<xlog::logger> logger) { logger_ = std::move(logger); }

    void Send(const turbo::LogEntry &entry) override {
        if (!logger_) {
            return;
        }

        const xlog::source_loc loc{entry.source_filename().data(), entry.source_line(), ""};
        const auto stamp = turbo::Time::to_chrono(entry.timestamp());
        const auto lvl = to_xlog_level(static_cast<int>(entry.log_severity()));
        const xlog::string_view_t message(entry.text_message().data(), entry.text_message().size());
        const xlog::string_view_t stacktrace(entry.stacktrace().data(), entry.stacktrace().size());

        submit_log_with_stacktrace(logger_, stamp, loc, lvl, message, stacktrace);
    }

    void Flush() override {
        if (logger_) {
            logger_->flush();
        }
    }

private:
    std::shared_ptr<xlog::logger> logger_;
};

inline turbo_log_sink &turbo_sink() {
    static turbo_log_sink sink;
    return sink;
}

inline void enable_turbo(std::shared_ptr<xlog::logger> logger = xlog::default_logger()) {
    turbo_sink().set_logger(std::move(logger));
    turbo::add_log_sink(&turbo_sink());
}

inline void disable_turbo() { turbo::remove_log_sink(&turbo_sink()); }

}  // namespace upstream
}  // namespace xlog
