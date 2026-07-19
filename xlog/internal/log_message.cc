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

#include <xlog/internal/log_message.h>

#include <cstdlib>
#include <xlog/initialize.h>
#include <xlog/log_sink_set.h>
#include <xlog/utility.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#if defined(__linux__)
#include <sys/syscall.h>
#endif
#endif

namespace xlog {
    namespace {
        uint64_t CurrentPid() {
#if defined(_WIN32)
            return static_cast<uint64_t>(::GetCurrentProcessId());
#else
            return static_cast<uint64_t>(::getpid());
#endif
        }

        uint64_t CurrentTid() {
#if defined(_WIN32)
            return static_cast<uint64_t>(::GetCurrentThreadId());
#elif defined(__linux__)
            return static_cast<uint64_t>(::syscall(SYS_gettid));
#elif defined(__APPLE__)
            uint64_t tid = 0;
            ::pthread_threadid_np(nullptr, &tid);
            return tid;
#else
            return static_cast<uint64_t>(::pthread_self());
#endif
        }
    } // namespace

    LogMessage::LogMessage(const char *file, int line, LogSeverity severity) {
        _entry.filename = file ? file : "";
        _entry.line = line;
        _entry.log_severity = severity;
        _entry.thread_identify = get_thread_identify();
        _entry.pid = CurrentPid();
        _entry.tid = CurrentTid();
        _entry.timestamp = std::chrono::system_clock::now();
    }

    LogMessage::LogMessage(const char *file, int line,
                           std::string_view failure_msg)
        : LogMessage(file, line, LogSeverity::kSeverityFatal) {
        *this << "Check failed: " << failure_msg << " ";
    }

    LogMessage::~LogMessage() { flush(); }

    LogMessage &LogMessage::at_location(std::string_view file, int line) {
        _entry.filename = file;
        _entry.line = line;
        return *this;
    }

    LogMessage &LogMessage::with_verbosity(uint32_t verbose_level) {
        _entry.verbose_level = verbose_level;
        return *this;
    }

    LogMessage &LogMessage::to_sink_also(LogSink *sink) {
        if (sink != nullptr) {
            _extra_sinks.push_back(sink);
        }
        return *this;
    }

    LogMessage &LogMessage::to_sink_only(LogSink *sink) {
        _extra_sinks.clear();
        if (sink != nullptr) {
            _extra_sinks.push_back(sink);
        }
        _extra_sinks_only = true;
        return *this;
    }

    LogMessage &LogMessage::operator<<(std::string_view v) {
        fmt::format_to(std::back_inserter(_entry.buffer), "{}", v);
        return *this;
    }

    LogMessage &LogMessage::operator<<(const char *v) {
        if (v != nullptr) {
            fmt::format_to(std::back_inserter(_entry.buffer), "{}", v);
        }
        return *this;
    }

    LogMessage &LogMessage::operator<<(std::ostream &(*m)(std::ostream &os)) {
        if (m == static_cast<std::ostream &(*)(std::ostream &)>(std::endl) ||
            m == static_cast<std::ostream &(*)(std::ostream &)>(std::ends)) {
            _entry.buffer.push_back('\n');
        }
        return *this;
    }

    LogMessage &LogMessage::operator<<(std::ios_base &(*)(std::ios_base &)) {
        return *this;
    }

    void LogMessage::flush() {
        if (_flushed) {
            return;
        }
        _flushed = true;

        const bool fatal =
                _entry.log_severity == LogSeverity::kSeverityFatal;
        const auto &cfg = LogConfig::instance();
        if (!should_log(_entry.log_severity, cfg.min_log_level)) {
            if (fatal) {
                std::abort();
            }
            return;
        }
        if (_entry.verbose_level > 0 &&
            static_cast<int>(_entry.verbose_level) > cfg.verbosity) {
            if (fatal) {
                std::abort();
            }
            return;
        }

        log_to_sinks(std::move(_entry), _extra_sinks, _extra_sinks_only, fatal);
        if (fatal) {
            std::abort();
        }
    }
} // namespace xlog
