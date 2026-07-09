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
// Optional header-only sink: google::LogSink -> xlog logger.
// Requires glog in the user's build. Not compiled into libxlog.
//
// Usage:
//   #include <xlog/upstream/glog.h>
//   xlog::upstream::enable_glog();

#pragma once

#include <memory>

#ifndef GLOG_USE_GLOG_EXPORT
#define GLOG_USE_GLOG_EXPORT
#endif

#include <glog/logging.h>

#include <xlog/upstream/common.h>

namespace xlog {
namespace upstream {

class glog_log_sink final : public google::LogSink {
public:
    void set_logger(std::shared_ptr<xlog::logger> logger) { logger_ = std::move(logger); }

    void send(google::LogSeverity severity,
              const char *full_filename,
              const char * /*base_filename*/,
              int line,
              const google::LogMessageTime &time,
              const char *message,
              size_t message_len) override {
        if (!logger_) {
            return;
        }

        const xlog::source_loc loc{full_filename, line, ""};
        const auto stamp = time.when();
        const auto lvl = to_xlog_level(static_cast<int>(severity));
        submit_log(logger_, stamp, loc, lvl, xlog::string_view_t(message, message_len));
    }

    void WaitTillSent() override {}

private:
    std::shared_ptr<xlog::logger> logger_;
};

inline glog_log_sink &glog_sink() {
    static glog_log_sink sink;
    return sink;
}

inline void enable_glog(std::shared_ptr<xlog::logger> logger = xlog::default_logger()) {
    glog_sink().set_logger(std::move(logger));
    google::AddLogSink(&glog_sink());
}

inline void disable_glog() { google::RemoveLogSink(&glog_sink()); }

}  // namespace upstream
}  // namespace xlog
