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
// Shared helpers for optional upstream sinks (header-only, not built into libxlog).
// Maps foreign severities and submits structured fields to an xlog logger so that
// xlog's pattern_formatter produces the unified output format.

#pragma once

#include <memory>
#include <string>

#include <xlog/xlog.h>

namespace xlog {
    namespace upstream {
        // glog / turbo / absl all use 0=info, 1=warning, 2=error, 3=fatal.
        inline xlog::level::level_enum to_xlog_level(int severity) {
            switch (severity) {
                case 1:
                    return xlog::level::warn;
                case 2:
                    return xlog::level::err;
                case 3:
                    return xlog::level::critical;
                case 0:
                default:
                    return xlog::level::info;
            }
        }

        inline void submit_log(const std::shared_ptr<xlog::logger> &logger,
                               xlog::log_clock::time_point stamp,
                               xlog::source_loc loc,
                               xlog::level::level_enum lvl,
                               xlog::string_view_t payload) {
            if (!logger) {
                return;
            }
            logger->log(stamp, loc, lvl, payload);
        }

        inline void submit_log(const std::shared_ptr<xlog::logger> &logger,
                               xlog::source_loc loc,
                               xlog::level::level_enum lvl,
                               xlog::string_view_t payload) {
            if (!logger) {
                return;
            }
            logger->log(loc, lvl, payload);
        }

        inline void submit_log_with_stacktrace(const std::shared_ptr<xlog::logger> &logger,
                                               xlog::log_clock::time_point stamp,
                                               xlog::source_loc loc,
                                               xlog::level::level_enum lvl,
                                               xlog::string_view_t message,
                                               xlog::string_view_t stacktrace) {
            if (!logger) {
                return;
            }
            if (stacktrace.size() == 0) {
                submit_log(logger, stamp, loc, lvl, message);
                return;
            }
            std::string payload(message.data(), message.size());
            payload.push_back('\n');
            payload.append(stacktrace.data(), stacktrace.size());
            submit_log(logger, stamp, loc, lvl, xlog::string_view_t(payload));
        }
    } // namespace upstream
} // namespace xlog
