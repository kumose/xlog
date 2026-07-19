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
// Conditional log helpers: STATELESS/STATEFUL macros and Log*State classes
// (EVERY_N / FIRST_N / EVERY_POW_2 / EVERY_N_SEC).

#pragma once

#if defined(_WIN32)
#include <cstdlib>
#else
#include <unistd.h>
#endif

#include <atomic>
#include <cstdint>
#include <cstdlib>

#include <xlog/internal/voidify.h>
#include <xlog/log_severity.h>

// Expands to either (void)0 or Voidify() && <LogMessage expression>.
#define XLOG_INTERNAL_STATELESS_CONDITION(condition) \
    switch (0)                                       \
    case 0:                                          \
    default:                                         \
        !(condition) ? (void)0 : ::xlog::log_internal::Voidify() &&

// Stateful macros (EVERY_N etc.): static Log*State + COUNTER.
#define XLOG_INTERNAL_STATEFUL_CONDITION(condition)                 \
    for (bool xlog_internal_stateful_condition_do_log(condition);   \
         xlog_internal_stateful_condition_do_log;                   \
         xlog_internal_stateful_condition_do_log = false)           \
    XLOG_INTERNAL_STATEFUL_CONDITION_IMPL

#define XLOG_INTERNAL_STATEFUL_CONDITION_IMPL(kind, ...)                      \
    for (static ::xlog::log_internal::Log##kind##State                        \
             xlog_internal_stateful_condition_state;                          \
         xlog_internal_stateful_condition_do_log &&                           \
         xlog_internal_stateful_condition_state.should_log(__VA_ARGS__);      \
         xlog_internal_stateful_condition_do_log = false)                     \
        for (const uint32_t COUNTER [[maybe_unused]] =                        \
                 xlog_internal_stateful_condition_state.counter();            \
             xlog_internal_stateful_condition_do_log;                         \
             xlog_internal_stateful_condition_do_log = false)

#ifdef XLOG_MIN_LOG_LEVEL
#define XLOG_INTERNAL_CONDITION_TRACE(type, condition)                       \
    XLOG_INTERNAL_##type##_CONDITION(                                        \
        (condition) &&                                                       \
        ::xlog::LogSeverity::kSeverityTrace >=                               \
            static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL))
#define XLOG_INTERNAL_CONDITION_DEBUG(type, condition)                       \
    XLOG_INTERNAL_##type##_CONDITION(                                        \
        (condition) &&                                                       \
        ::xlog::LogSeverity::kSeverityDebug >=                               \
            static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL))
#define XLOG_INTERNAL_CONDITION_INFO(type, condition)                        \
    XLOG_INTERNAL_##type##_CONDITION(                                        \
        (condition) &&                                                       \
        ::xlog::LogSeverity::kSeverityInfo >=                                \
            static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL))
#define XLOG_INTERNAL_CONDITION_WARNING(type, condition)                     \
    XLOG_INTERNAL_##type##_CONDITION(                                        \
        (condition) &&                                                       \
        ::xlog::LogSeverity::kSeverityWarning >=                             \
            static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL))
#define XLOG_INTERNAL_CONDITION_ERROR(type, condition)                       \
    XLOG_INTERNAL_##type##_CONDITION(                                        \
        (condition) &&                                                       \
        ::xlog::LogSeverity::kSeverityError >=                               \
            static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL))
#define XLOG_INTERNAL_CONDITION_FATAL(type, condition)                       \
    XLOG_INTERNAL_##type##_CONDITION(                                        \
        ((condition)                                                         \
             ? (::xlog::LogSeverity::kSeverityFatal >=                       \
                        static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL)  \
                    ? true                                                   \
                    : (::xlog::log_internal::abort_quietly(), false))        \
             : false))
#define XLOG_INTERNAL_CONDITION_LEVEL(severity)                               \
    for (int xlog_internal_severity_loop = 1; xlog_internal_severity_loop;     \
         xlog_internal_severity_loop = 0)                                     \
        for (const ::xlog::LogSeverity xlog_internal_severity = (severity);   \
             xlog_internal_severity_loop; xlog_internal_severity_loop = 0)     \
    XLOG_INTERNAL_CONDITION_LEVEL_IMPL
#define XLOG_INTERNAL_CONDITION_LEVEL_IMPL(type, condition)                  \
    XLOG_INTERNAL_##type##_CONDITION(                                        \
        ((condition) &&                                                      \
         (xlog_internal_severity >=                                           \
              static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL) ||         \
          (xlog_internal_severity == ::xlog::LogSeverity::kSeverityFatal &&   \
           (::xlog::log_internal::abort_quietly(), false)))))
#else
#define XLOG_INTERNAL_CONDITION_TRACE(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)
#define XLOG_INTERNAL_CONDITION_DEBUG(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)
#define XLOG_INTERNAL_CONDITION_INFO(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)
#define XLOG_INTERNAL_CONDITION_WARNING(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)
#define XLOG_INTERNAL_CONDITION_ERROR(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)
#define XLOG_INTERNAL_CONDITION_FATAL(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)
#define XLOG_INTERNAL_CONDITION_LEVEL(severity)                               \
    for (int xlog_internal_severity_loop = 1; xlog_internal_severity_loop;     \
         xlog_internal_severity_loop = 0)                                     \
        for (const ::xlog::LogSeverity xlog_internal_severity = (severity);   \
             xlog_internal_severity_loop; xlog_internal_severity_loop = 0)     \
    XLOG_INTERNAL_CONDITION_LEVEL_IMPL
#define XLOG_INTERNAL_CONDITION_LEVEL_IMPL(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)
#endif

namespace xlog {
namespace log_internal {

    // Class names must be Log + kind + State for STATEFUL_CONDITION_IMPL.
    class LogEveryNState final {
    public:
        bool should_log(int n);
        uint32_t counter() { return counter_.load(std::memory_order_relaxed); }

    private:
        std::atomic<uint32_t> counter_{0};
    };

    class LogFirstNState final {
    public:
        bool should_log(int n);
        uint32_t counter() { return counter_.load(std::memory_order_relaxed); }

    private:
        std::atomic<uint32_t> counter_{0};
    };

    class LogEveryPow2State final {
    public:
        bool should_log();
        uint32_t counter() { return counter_.load(std::memory_order_relaxed); }

    private:
        std::atomic<uint32_t> counter_{0};
    };

    class LogEveryNSecState final {
    public:
        bool should_log(double seconds);
        uint32_t counter() { return counter_.load(std::memory_order_relaxed); }

    private:
        std::atomic<uint32_t> counter_{0};
        std::atomic<int64_t> next_log_time_ns_{0};
    };

    [[noreturn]] inline void abort_quietly() { std::abort(); }

    [[noreturn]] inline void exit_quietly() {
#ifdef _WIN32
        std::_Exit(1);
#else
        _exit(1);
#endif
    }

}  // namespace log_internal
}  // namespace xlog
