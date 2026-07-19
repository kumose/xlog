//
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

#include <xlog/internal/log_sink_set.h>
#include <xlog/internal/no_destructor.h>

#ifndef KUMO_HAVE_THREAD_LOCAL
#include <pthread.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <algorithm>
#include <vector>
#include <mutex>
#include <xlog/internal/log_sink_set.h>
#include <xlog/initialize.h>
#include <xlog/internal/no_destructor.h>

namespace xlog::log_internal {
    namespace {
        class StderrLogSink final : public LogSink {
        public:
            ~StderrLogSink() override = default;

            void send(const xlog::LogEntry &entry) override {
                if (entry.log_severity < xlog::stderr_threshold() &&
                    xlog::is_initialized()) {
                    return;
                }

                static std::once_flag warn_if_not_initialized;
                std::call_once(warn_if_not_initialized, []() {
                    if (xlog::is_initialized()) return;
                    const char w[] =
                            "WARNING: All log messages before xlog::initialize_log() is called"
                            " are written to STDERR\n";
                    xlog::log_internal::WriteToStderr(w, xlog::LogSeverity::kWarning);
                });

                if (!entry.stacktrace().empty()) {
                    xlog::log_internal::WriteToStderr(entry.stacktrace(),
                                                      entry.log_severity());
                } else {
                    // TODO(b/226937039): do this outside else condition once we avoid
                    // ReprintFatalMessage
                    auto log_data = entry.newline()
                                        ? entry.text_message_with_prefix_and_newline()
                                        : entry.text_message_with_prefix();
                    xlog::log_internal::WriteToStderr(
                        log_data, entry.log_severity());
                }
            }
        };

#if defined(__ANDROID__)
        class AndroidLogSink final : public LogSink {
        public:
            ~AndroidLogSink() override = default;

            void Send(const xlog::LogEntry &entry) override {
                const int level = AndroidLogLevel(entry);
                const char *const tag = GetAndroidNativeTag();
                __android_log_write(level, tag,
                                    entry.text_message_with_prefix_and_newline_c_str());
                if (entry.log_severity() == xlog::LogSeverity::kFatal)
                    __android_log_write(ANDROID_LOG_FATAL, tag, "terminating.\n");
            }

        private:
            static int AndroidLogLevel(const xlog::LogEntry &entry) {
                switch (entry.log_severity()) {
                    case xlog::LogSeverity::kFatal:
                        return ANDROID_LOG_FATAL;
                    case xlog::LogSeverity::kError:
                        return ANDROID_LOG_ERROR;
                    case xlog::LogSeverity::kWarning:
                        return ANDROID_LOG_WARN;
                    default:
                        if (entry.verbosity() >= 2) return ANDROID_LOG_VERBOSE;
                        if (entry.verbosity() == 1) return ANDROID_LOG_DEBUG;
                        return ANDROID_LOG_INFO;
                }
            }
        };
#endif  // !defined(__ANDROID__)

#if defined(_WIN32)
        class WindowsDebuggerLogSink final : public LogSink {
        public:
            ~WindowsDebuggerLogSink() override = default;

            void Send(const xlog::LogEntry &entry) override {
                if (entry.log_severity() < xlog::stderr_threshold() &&
                    xlog::log_internal::IsInitialized()) {
                    return;
                }
                ::OutputDebugStringA(entry.text_message_with_prefix_and_newline_c_str());
            }
        };
#endif  // !defined(_WIN32)

        class GlobalLogSinkSet final {
        public:
            GlobalLogSinkSet() {
#if defined(__myriad2__) || defined(__Fuchsia__)
                // myriad2 and Fuchsia do not log to stderr by default.
#else
                static NoDestructor<StderrLogSink> stderr_log_sink;
                add_log_sink(stderr_log_sink.get());
#endif
#ifdef __ANDROID__
                static xlog::NoDestructor<AndroidLogSink> android_log_sink;
                add_log_sink(android_log_sink.get());
#endif
#if defined(_WIN32)
                static xlog::NoDestructor<WindowsDebuggerLogSink> debugger_log_sink;
                add_log_sink(debugger_log_sink.get());
#endif  // !defined(_WIN32)
            }

            void log_to_sinks(const xlog::LogEntry &entry,
                              const std::vector<xlog::LogSink *> &extra_sinks, bool extra_sinks_only) {
                SendToSinks(entry, extra_sinks);

                if (!extra_sinks_only) {
                    if (thread_is_logging_to_log_sink()) {
                        auto log_data = entry.newline()
                                            ? entry.text_message_with_prefix_and_newline()
                                            : entry.text_message_with_prefix();
                        xlog::log_internal::WriteToStderr(
                            log_data, entry.log_severity);
                    } else {
                        std::lock_guard<std::mutex> global_sinks_lock(guard_);
                        SendToSinks(entry, xlog::MakeSpan(sinks_));
                    }
                }
            }

            void add_log_sink(xlog::LogSink *sink) {
                {
                    std::lock_guard<std::mutex> global_sinks_lock(guard_);
                    auto pos = std::find(sinks_.begin(), sinks_.end(), sink);
                    if (pos == sinks_.end()) {
                        sinks_.push_back(sink);
                        return;
                    }
                }
                TURBO_INTERNAL_LOG(FATAL, "Duplicate log sinks are not supported");
            }

            void remove_log_sink(xlog::LogSink *sink) {
                {
                    std::lock_guard<std::mutex> global_sinks_lock(guard_);
                    auto pos = std::find(sinks_.begin(), sinks_.end(), sink);
                    if (pos != sinks_.end()) {
                        sinks_.erase(pos);
                        return;
                    }
                }
                TURBO_INTERNAL_LOG(FATAL, "Mismatched log sink being removed");
            }

            void flush_log_sinks() {
                if (thread_is_logging_to_log_sink()) {
                    // The thread_local condition demonstrates that we're already holding the
                    // lock in order to iterate over `sinks_` for dispatch.  The thread-safety
                    // annotations don't know this, so we use `TURBO_NO_THREAD_SAFETY_ANALYSIS`

                    FlushLogSinksLocked();
                } else {
                    std::lock_guard<std::mutex> global_sinks_lock(guard_);
                    FlushLogSinksLocked();
                }
            }

        private:
            void FlushLogSinksLocked() {
                for (xlog::LogSink *sink: sinks_) {
                    sink->flush();
                }
            }

            // Helper routine for log_to_sinks.
            static void SendToSinks(const xlog::LogEntry &entry,
                                    const std::vector<xlog::LogSink *> &sinks) {
                for (xlog::LogSink *sink: sinks) {
                    sink->send(entry);
                }
            }

            using LogSinksSet = std::vector<xlog::LogSink *>;
            std::mutex guard_;
            LogSinksSet sinks_;
        };

        // Returns reference to the global LogSinks set.
        GlobalLogSinkSet &GlobalSinks() {
            static NoDestructor<GlobalLogSinkSet> global_sinks;
            return *global_sinks;
        }
    } // namespace

    void log_to_sinks(const xlog::LogEntry &entry,
                      const std::vector<xlog::LogSink *> &extra_sinks, bool extra_sinks_only) {
        log_internal::GlobalSinks().log_to_sinks(entry, extra_sinks, extra_sinks_only);
    }

    void add_log_sink(xlog::LogSink *sink) {
        log_internal::GlobalSinks().add_log_sink(sink);
    }

    void remove_log_sink(xlog::LogSink *sink) {
        log_internal::GlobalSinks().remove_log_sink(sink);
    }

    void flush_log_sinks() { log_internal::GlobalSinks().flush_log_sinks(); }
} // namespace xlog::log_internal
