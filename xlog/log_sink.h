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

namespace xlog {

    // xlog::LogSink
    //
    // Destination I/O only. Registered sinks are invoked from
    // `LogSinkSet::dispatch_locked` while the **set mutex is held**, so
    // built-in sinks do not need their own mutex for ordering — the set
    // serializes send/flush across threads.
    //
    // Do not take locks that the `XLOG` caller might hold. Prefer not taking
    // a second mutex in send/flush (nested lock / deadlock risk with the set).
    // Re-entrant `XLOG` from send() is supported via the set TLS guard.
    class LogSink {
    public:
        virtual ~LogSink() = default;

        // Called under LogSinkSet's mutex with format_buffer already filled.
        virtual void send(const xlog::LogEntry &entry) = 0;

        // Optional; also called under the set mutex when flush was requested.
        virtual void flush() {}

    protected:
        LogSink() = default;

        // Implementations may be copyable and/or movable.
        LogSink(const LogSink &) = default;

        LogSink &operator=(const LogSink &) = default;

    private:
        // https://lld.llvm.org/missingkeyfunction.html#missing-key-function
        virtual void key_function() const final;  // NOLINT(readability/inheritance)
    };

    
}  // namespace xlog
