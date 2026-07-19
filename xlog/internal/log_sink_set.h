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

#pragma once

#include <xlog/log_entry.h>
#include <xlog/log_sink.h>
#include <vector>

namespace xlog::log_internal {
    // Returns true if a globally-registered `LogSink`'s `Send()` is currently
    // being invoked on this thread.
    bool thread_is_logging_to_log_sink();

    // This function may log to two sets of sinks:
    //
    // * If `extra_sinks_only` is true, it will dispatch only to `extra_sinks`.
    //   `LogMessage::to_sink_also` and `LogMessage::to_sink_only` are used to attach
    //    extra sinks to the entry.
    // * Otherwise it will also log to the global sinks set. This set is managed
    //   by `xlog::add_log_sink` and `xlog::remove_log_sink`.
    void log_to_sinks(const xlog::LogEntry &entry,
                      const std::vector<xlog::LogSink *> &extra_sinks, bool extra_sinks_only);

    // Implementation for operations with log sink set.
    void add_log_sink(xlog::LogSink *sink);

    void remove_log_sink(xlog::LogSink *sink);

    void flush_log_sinks();
} // namespace xlog::log_internal
