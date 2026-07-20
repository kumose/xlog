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

#include <fmt/format.h>
#include <fmt/printf.h>
#include <ios>
#include <ostream>
#include <string_view>
#include <utility>
#include <vector>
#include <xlog/log_entry.h>
#include <xlog/log_sink.h>

namespace xlog {

    // Temporary log statement object. ~LogMessage dispatches to sinks.
    //
    //   // stream (XLOG)
    //   LogMessage(__FILE__, __LINE__, LogSeverity::kSeverityInfo)
    //       .internal_stream() << "hello " << 42;
    //   // fmt (TLOG)
    //   LogMessage(...).print("hello {}", 42);
    //   // printf (ZLOG)
    //   LogMessage(...).printf("hello %d", 42);
    class LogMessage {
    public:
        LogMessage(const char *file, int line, LogSeverity severity);

        // FATAL + prefix "Check failed: <failure_msg> " (for XCHECK*).
        LogMessage(const char *file, int line, std::string_view failure_msg);

        LogMessage(const LogMessage &) = delete;

        LogMessage &operator=(const LogMessage &) = delete;

        ~LogMessage();

        LogMessage &at_location(std::string_view file, int line);

        LogMessage &no_prefix();

        LogMessage &with_verbosity(uint32_t verbose_level);

        // Append ": <strerror> [<errno>]" using CRT errno captured at
        // construction (not Win32 GetLastError). Message via portable StrError.
        LogMessage &with_perror();

        LogMessage &to_sink_also(LogSink *sink);

        LogMessage &to_sink_only(LogSink *sink);

        LogMessage &internal_stream() { return *this; }

        // fmt::format style → _entry.buffer
        template <typename... Args>
        LogMessage &print(fmt::format_string<Args...> format, Args &&...args) {
            fmt::format_to(std::back_inserter(_entry.buffer), format,
                           std::forward<Args>(args)...);
            return *this;
        }

        // printf style → _entry.buffer
        template <typename... Args>
        LogMessage &printf(fmt::string_view format, const Args &...args) {
            const auto s = fmt::sprintf(format, args...);
            _entry.buffer.append(s.data(), s.data() + s.size());
            return *this;
        }

        LogMessage &operator<<(std::string_view v);

        LogMessage &operator<<(const char *v);

        LogMessage &operator<<(std::ostream &(*m)(std::ostream &os));

        LogMessage &operator<<(std::ios_base &(*m)(std::ios_base &os));

        template <typename T>
        LogMessage &operator<<(const T &v) {
            fmt::format_to(std::back_inserter(_entry.buffer), "{}", v);
            return *this;
        }

    private:
        void flush();

        // First so construction captures errno before other side effects.
        int _saved_errno;
        LogEntry _entry;
        std::vector<LogSink *> _extra_sinks;
        bool _extra_sinks_only{false};
        bool _is_perror{false};
        bool _flushed{false};
    };

}  // namespace xlog

