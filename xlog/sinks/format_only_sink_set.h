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
// LogSinkSet that formats (unlocked in do_log) then skips sink I/O in
// dispatch_locked. For benchmarks / format-cost measurement.
// FATAL still writes stderr and aborts under the set lock.

#pragma once

#include <vector>

#include <xlog/log_sink_set.h>

namespace xlog {

    class FormatOnlySinkSet final : public LogSinkSet {
    public:
        FormatOnlySinkSet() : LogSinkSet(std::vector<LogSink *>{}) {}

    protected:
        void dispatch_locked(LogEntry &entry,
                             const std::vector<LogSink *> &extra_sinks,
                             bool extra_sinks_only, bool flush) override;
    };

}  // namespace xlog
