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

#include <cstdlib>

#include <xlog/log_setting.h>
#include <xlog/logger.h>
#include <xlog/internal/conditions.h>
#include <xlog/internal/check_op.h>
#include <xlog/xlog.h>

// ============================================================================
// TLOG internal log body macros
// ============================================================================

#define TLOG_BODY_TRACE(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::trace, __VA_ARGS__)

#define TLOG_BODY_DEBUG(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::debug, __VA_ARGS__)

#define TLOG_BODY_INFO(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::info, __VA_ARGS__)

#define TLOG_BODY_WARNING(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::warn, __VA_ARGS__)

#define TLOG_BODY_ERROR(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::err, __VA_ARGS__)

#define TLOG_BODY_FATAL(...) \
    (::xlog::default_logger_raw()->log( \
         ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
         ::xlog::level::critical, __VA_ARGS__), \
     ::std::abort())

// ============================================================================
// TLOG — format-string logging with runtime level filter
// ============================================================================

#define TLOG(severity, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity) \
        TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_IF(severity, condition, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity && (condition)) \
        TLOG_BODY_##severity(__VA_ARGS__)

// ============================================================================
// Rate-limited variants
// ============================================================================

#define TLOG_EVERY_N(severity, n, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_FIRST_N(severity, n, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_ONCE(severity, ...) \
    TLOG_FIRST_N(severity, 1, __VA_ARGS__)

#define TLOG_EVERY_POW_2(severity, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_EVERY_N_SEC(severity, n_seconds, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(n_seconds); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_EVERY_SEC(severity, ...) \
    TLOG_EVERY_N_SEC(severity, 1, __VA_ARGS__)

#define TLOG_EVERY_MIN(severity, ...) \
    TLOG_EVERY_N_SEC(severity, 60, __VA_ARGS__)

// ============================================================================
// Conditional rate-limited variants
// ============================================================================

#define TLOG_IF_EVERY_N(severity, condition, n, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_IF_FIRST_N(severity, condition, n, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_IF_ONCE(severity, condition, ...) \
    TLOG_IF_FIRST_N(severity, condition, 1, __VA_ARGS__)

#define TLOG_IF_EVERY_POW_2(severity, condition, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, ...) \
    for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tlog_enabled_; _tlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _tlog_state_; \
             _tlog_enabled_ && _tlog_state_.ShouldLog(n_seconds); \
             _tlog_enabled_ = false) \
            TLOG_BODY_##severity(__VA_ARGS__)

#define TLOG_IF_EVERY_SEC(severity, condition, ...) \
    TLOG_IF_EVERY_N_SEC(severity, condition, 1, __VA_ARGS__)

#define TLOG_IF_EVERY_MIN(severity, condition, ...) \
    TLOG_IF_EVERY_N_SEC(severity, condition, 60, __VA_ARGS__)

// ============================================================================
// TVLOG — verbose logging
// ============================================================================

#define TVLOG(verbose_level, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        if (::xlog::log_level() <= XLOG_LEVEL_INFO && \
            _tlog_verbose_ <= ::xlog::vlog_level()) \
            TLOG_BODY_INFO(__VA_ARGS__)

#define TVLOG_EVERY_N(verbose_level, n, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_INFO && \
                                   _tlog_verbose_ <= ::xlog::vlog_level(); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryNState _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_INFO(__VA_ARGS__)

#define TVLOG_FIRST_N(verbose_level, n, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_INFO && \
                                   _tlog_verbose_ <= ::xlog::vlog_level(); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogFirstNState _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_INFO(__VA_ARGS__)

#define TVLOG_ONCE(verbose_level, ...) \
    TVLOG_FIRST_N(verbose_level, 1, __VA_ARGS__)

#define TVLOG_EVERY_POW_2(verbose_level, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_INFO && \
                                   _tlog_verbose_ <= ::xlog::vlog_level(); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryPow2State _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_INFO(__VA_ARGS__)

#define TVLOG_EVERY_N_SEC(verbose_level, n_seconds, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_INFO && \
                                   _tlog_verbose_ <= ::xlog::vlog_level(); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryNSecState _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(n_seconds); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_INFO(__VA_ARGS__)

#define TVLOG_EVERY_SEC(verbose_level, ...) \
    TVLOG_EVERY_N_SEC(verbose_level, 1, __VA_ARGS__)

#define TVLOG_EVERY_MIN(verbose_level, ...) \
    TVLOG_EVERY_N_SEC(verbose_level, 60, __VA_ARGS__)

// ============================================================================
// DTLOG / DTVLOG — debug-only variants
// ============================================================================

#ifndef NDEBUG

#define DTLOG(severity, ...)                  TLOG(severity, __VA_ARGS__)
#define DTLOG_IF(severity, cond, ...)          TLOG_IF(severity, cond, __VA_ARGS__)
#define DTLOG_EVERY_N(severity, n, ...)        TLOG_EVERY_N(severity, n, __VA_ARGS__)
#define DTLOG_FIRST_N(severity, n, ...)        TLOG_FIRST_N(severity, n, __VA_ARGS__)
#define DTLOG_ONCE(severity, ...)              TLOG_ONCE(severity, __VA_ARGS__)
#define DTLOG_EVERY_POW_2(severity, ...)       TLOG_EVERY_POW_2(severity, __VA_ARGS__)
#define DTLOG_EVERY_N_SEC(severity, s, ...)    TLOG_EVERY_N_SEC(severity, s, __VA_ARGS__)
#define DTLOG_EVERY_SEC(severity, ...)         TLOG_EVERY_SEC(severity, __VA_ARGS__)
#define DTLOG_EVERY_MIN(severity, ...)         TLOG_EVERY_MIN(severity, __VA_ARGS__)
#define DTLOG_IF_EVERY_N(severity, c, n, ...)  TLOG_IF_EVERY_N(severity, c, n, __VA_ARGS__)
#define DTLOG_IF_FIRST_N(severity, c, n, ...)  TLOG_IF_FIRST_N(severity, c, n, __VA_ARGS__)
#define DTLOG_IF_ONCE(severity, c, ...)        TLOG_IF_ONCE(severity, c, __VA_ARGS__)
#define DTLOG_IF_EVERY_POW_2(severity, c, ...) TLOG_IF_EVERY_POW_2(severity, c, __VA_ARGS__)
#define DTLOG_IF_EVERY_N_SEC(severity, c, s, ...) TLOG_IF_EVERY_N_SEC(severity, c, s, __VA_ARGS__)
#define DTLOG_IF_EVERY_SEC(severity, c, ...)   TLOG_IF_EVERY_SEC(severity, c, __VA_ARGS__)
#define DTLOG_IF_EVERY_MIN(severity, c, ...)   TLOG_IF_EVERY_MIN(severity, c, __VA_ARGS__)

#define DTVLOG(v, ...)                         TVLOG(v, __VA_ARGS__)
#define DTVLOG_EVERY_N(v, n, ...)              TVLOG_EVERY_N(v, n, __VA_ARGS__)
#define DTVLOG_FIRST_N(v, n, ...)              TVLOG_FIRST_N(v, n, __VA_ARGS__)
#define DTVLOG_ONCE(v, ...)                    TVLOG_ONCE(v, __VA_ARGS__)
#define DTVLOG_EVERY_POW_2(v, ...)             TVLOG_EVERY_POW_2(v, __VA_ARGS__)
#define DTVLOG_EVERY_N_SEC(v, s, ...)          TVLOG_EVERY_N_SEC(v, s, __VA_ARGS__)
#define DTVLOG_EVERY_SEC(v, ...)               TVLOG_EVERY_SEC(v, __VA_ARGS__)
#define DTVLOG_EVERY_MIN(v, ...)               TVLOG_EVERY_MIN(v, __VA_ARGS__)

#else // NDEBUG

#define DTLOG(severity, ...)                   (void)0
#define DTLOG_IF(severity, cond, ...)          (void)0
#define DTLOG_EVERY_N(severity, n, ...)        (void)0
#define DTLOG_FIRST_N(severity, n, ...)        (void)0
#define DTLOG_ONCE(severity, ...)              (void)0
#define DTLOG_EVERY_POW_2(severity, ...)       (void)0
#define DTLOG_EVERY_N_SEC(severity, s, ...)    (void)0
#define DTLOG_EVERY_SEC(severity, ...)         (void)0
#define DTLOG_EVERY_MIN(severity, ...)         (void)0
#define DTLOG_IF_EVERY_N(severity, c, n, ...)  (void)0
#define DTLOG_IF_FIRST_N(severity, c, n, ...)  (void)0
#define DTLOG_IF_ONCE(severity, c, ...)        (void)0
#define DTLOG_IF_EVERY_POW_2(severity, c, ...) (void)0
#define DTLOG_IF_EVERY_N_SEC(severity, c, s, ...) (void)0
#define DTLOG_IF_EVERY_SEC(severity, c, ...)   (void)0
#define DTLOG_IF_EVERY_MIN(severity, c, ...)   (void)0

#define DTVLOG(v, ...)                         (void)0
#define DTVLOG_EVERY_N(v, n, ...)              (void)0
#define DTVLOG_FIRST_N(v, n, ...)              (void)0
#define DTVLOG_ONCE(v, ...)                    (void)0
#define DTVLOG_EVERY_POW_2(v, ...)             (void)0
#define DTVLOG_EVERY_N_SEC(v, s, ...)          (void)0
#define DTVLOG_EVERY_SEC(v, ...)               (void)0
#define DTVLOG_EVERY_MIN(v, ...)               (void)0

#endif // NDEBUG

// ============================================================================
// TLOG_LEVEL — runtime-evaluated severity
// ============================================================================

#define TLOG_LEVEL(severity, ...) \
    do { \
        auto _tlog_sev_ = (severity); \
        if (::xlog::log_level() <= _tlog_sev_) { \
            ::xlog::default_logger_raw()->log( \
                ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
                _tlog_sev_, __VA_ARGS__); \
            if (_tlog_sev_ == ::xlog::level::critical) ::std::abort(); \
        } \
    } while (0)

// ============================================================================
// TCHECK — format-style CHECK family
// ============================================================================

#define TLOG_INTERNAL_CHECK_FAIL(msg) \
    do { \
        ::xlog::default_logger_raw()->log( \
            ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
            ::xlog::level::critical, \
            ::xlog::string_view_t(msg)); \
        ::std::abort(); \
    } while (0)

#define TCHECK(condition, ...) \
    if (!(condition)) \
        TLOG_BODY_FATAL("Check failed: " #condition __VA_OPT__(:) __VA_ARGS__)

#define TCHECK_ERR(invocation) \
    if ((invocation) == -1) { \
        std::string _tcheck_err_msg_ = \
            std::string("Check failed: " #invocation ": ") + \
            std::strerror(errno) + " [" + std::to_string(errno) + "]"; \
        TLOG_INTERNAL_CHECK_FAIL(_tcheck_err_msg_); \
    }

#define TCHECK_NOTNULL(val) \
    ::xlog::internal::TCheckNotNull(__FILE__, __LINE__, \
        "'" #val "' Must be non nullptr", (val))

#define TCHECK_EQ(val1, val2) \
    TLOG_INTERNAL_CHECK_OP_IMPL(Check_EQ, ==, val1, val2)

#define TCHECK_NE(val1, val2) \
    TLOG_INTERNAL_CHECK_OP_IMPL(Check_NE, !=, val1, val2)

#define TCHECK_LE(val1, val2) \
    TLOG_INTERNAL_CHECK_OP_IMPL(Check_LE, <=, val1, val2)

#define TCHECK_LT(val1, val2) \
    TLOG_INTERNAL_CHECK_OP_IMPL(Check_LT, <, val1, val2)

#define TCHECK_GE(val1, val2) \
    TLOG_INTERNAL_CHECK_OP_IMPL(Check_GE, >=, val1, val2)

#define TCHECK_GT(val1, val2) \
    TLOG_INTERNAL_CHECK_OP_IMPL(Check_GT, >, val1, val2)

#define TLOG_INTERNAL_CHECK_OP_IMPL(name, op, val1, val2)                       \
    while (auto _tcheck_op_result_ =                                            \
               ::xlog::internal::name##Impl(                                    \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " " #op " " #val2))                                    \
        TLOG_INTERNAL_CHECK_FAIL(*_tcheck_op_result_)

#define TCHECK_STREQ(s1, s2)                                                    \
    TLOG_INTERNAL_CHECK_STROP(strcmp, ==, true, s1, s2)

#define TCHECK_STRNE(s1, s2)                                                    \
    TLOG_INTERNAL_CHECK_STROP(strcmp, !=, false, s1, s2)

#define TCHECK_STRCASEEQ(s1, s2)                                                \
    TLOG_INTERNAL_CHECK_STROP(strcasecmp, ==, true, s1, s2)

#define TCHECK_STRCASENE(s1, s2)                                                \
    TLOG_INTERNAL_CHECK_STROP(strcasecmp, !=, false, s1, s2)

#define TLOG_INTERNAL_CHECK_STROP(func, op, expected, s1, s2)                   \
    while (auto _tcheck_strop_result_ =                                         \
               ::xlog::internal::Check##func##expected##Impl(                   \
                   (s1), (s2),                                                  \
                   #s1 " " #op " " #s2))                                        \
        TLOG_INTERNAL_CHECK_FAIL(*_tcheck_strop_result_)

// ============================================================================
// DTCHECK — debug-only CHECK variants
// ============================================================================

#ifndef NDEBUG

#define DTCHECK(cond)                        TCHECK(cond)
#define DTCHECK_EQ(v1, v2)                   TCHECK_EQ(v1, v2)
#define DTCHECK_NE(v1, v2)                   TCHECK_NE(v1, v2)
#define DTCHECK_LE(v1, v2)                   TCHECK_LE(v1, v2)
#define DTCHECK_LT(v1, v2)                   TCHECK_LT(v1, v2)
#define DTCHECK_GE(v1, v2)                   TCHECK_GE(v1, v2)
#define DTCHECK_GT(v1, v2)                   TCHECK_GT(v1, v2)
#define DTCHECK_STREQ(s1, s2)                TCHECK_STREQ(s1, s2)
#define DTCHECK_STRNE(s1, s2)                TCHECK_STRNE(s1, s2)
#define DTCHECK_STRCASEEQ(s1, s2)            TCHECK_STRCASEEQ(s1, s2)
#define DTCHECK_STRCASENE(s1, s2)            TCHECK_STRCASENE(s1, s2)
#define DTCHECK_ERR(invocation)              TCHECK_ERR(invocation)
#define DTCHECK_NOTNULL(val)                 TCHECK_NOTNULL(val)

#else // NDEBUG

#define DTCHECK(cond)                        ((void)0)
#define DTCHECK_EQ(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DTCHECK_NE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DTCHECK_LE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DTCHECK_LT(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DTCHECK_GE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DTCHECK_GT(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DTCHECK_STREQ(s1, s2)                ((void)(s1), (void)(s2), (void)0)
#define DTCHECK_STRNE(s1, s2)                ((void)(s1), (void)(s2), (void)0)
#define DTCHECK_STRCASEEQ(s1, s2)            ((void)(s1), (void)(s2), (void)0)
#define DTCHECK_STRCASENE(s1, s2)            ((void)(s1), (void)(s2), (void)0)
#define DTCHECK_ERR(invocation)              ((void)(invocation), (void)0)
#define DTCHECK_NOTNULL(val)                 (val)

#endif // NDEBUG

#define TUNREACHABLE() TCHECK(false)

// ============================================================================
// TCheckNotNull helper
// ============================================================================

namespace xlog {
namespace internal {

template <typename T>
inline T TCheckNotNull(const char *file, int line, const char *names, T &&t) {
    if (t == nullptr) {
        std::string msg = std::string("Check failed: ") + names;
        ::xlog::default_logger_raw()->log(
            ::xlog::source_loc{file, line, XLOG_FUNCTION},
            ::xlog::level::critical,
            ::xlog::string_view_t(msg));
        ::std::abort();
    }
    return std::forward<T>(t);
}

} // namespace internal
} // namespace xlog

// ============================================================================
// TLOG_LOGGER / TLOG shortcut macros — format-style with compile-time stripping
// ============================================================================

#ifndef TLOG_NO_SOURCE_LOC
#define TLOG_LOGGER_CALL(logger, level, ...) \
    (logger)->log(::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, level, __VA_ARGS__)
#else
#define TLOG_LOGGER_CALL(logger, level, ...) \
    (logger)->log(::xlog::source_loc{}, level, __VA_ARGS__)
#endif

#if XLOG_ACTIVE_LEVEL <= XLOG_LEVEL_TRACE
#define TLOG_LOGGER_TRACE(logger, ...)  TLOG_LOGGER_CALL(logger, ::xlog::level::trace, __VA_ARGS__)
#define TLOG_TRACE(...)                 TLOG_LOGGER_TRACE(::xlog::default_logger_raw(), __VA_ARGS__)
#else
#define TLOG_LOGGER_TRACE(logger, ...)  (void)0
#define TLOG_TRACE(...)                 (void)0
#endif

#if XLOG_ACTIVE_LEVEL <= XLOG_LEVEL_DEBUG
#define TLOG_LOGGER_DEBUG(logger, ...)  TLOG_LOGGER_CALL(logger, ::xlog::level::debug, __VA_ARGS__)
#define TLOG_DEBUG(...)                 TLOG_LOGGER_DEBUG(::xlog::default_logger_raw(), __VA_ARGS__)
#else
#define TLOG_LOGGER_DEBUG(logger, ...)  (void)0
#define TLOG_DEBUG(...)                 (void)0
#endif

#if XLOG_ACTIVE_LEVEL <= XLOG_LEVEL_INFO
#define TLOG_LOGGER_INFO(logger, ...)   TLOG_LOGGER_CALL(logger, ::xlog::level::info, __VA_ARGS__)
#define TLOG_INFO(...)                  TLOG_LOGGER_INFO(::xlog::default_logger_raw(), __VA_ARGS__)
#else
#define TLOG_LOGGER_INFO(logger, ...)   (void)0
#define TLOG_INFO(...)                  (void)0
#endif

#if XLOG_ACTIVE_LEVEL <= XLOG_LEVEL_WARNING
#define TLOG_LOGGER_WARN(logger, ...)   TLOG_LOGGER_CALL(logger, ::xlog::level::warn, __VA_ARGS__)
#define TLOG_WARN(...)                  TLOG_LOGGER_WARN(::xlog::default_logger_raw(), __VA_ARGS__)
#else
#define TLOG_LOGGER_WARN(logger, ...)   (void)0
#define TLOG_WARN(...)                  (void)0
#endif

#if XLOG_ACTIVE_LEVEL <= XLOG_LEVEL_ERROR
#define TLOG_LOGGER_ERROR(logger, ...)  TLOG_LOGGER_CALL(logger, ::xlog::level::err, __VA_ARGS__)
#define TLOG_ERROR(...)                 TLOG_LOGGER_ERROR(::xlog::default_logger_raw(), __VA_ARGS__)
#else
#define TLOG_LOGGER_ERROR(logger, ...)  (void)0
#define TLOG_ERROR(...)                 (void)0
#endif

#if XLOG_ACTIVE_LEVEL <= XLOG_LEVEL_FATAL
#define TLOG_LOGGER_CRITICAL(logger, ...) TLOG_LOGGER_CALL(logger, ::xlog::level::critical, __VA_ARGS__)
#define TLOG_CRITICAL(...)                TLOG_LOGGER_CRITICAL(::xlog::default_logger_raw(), __VA_ARGS__)
#else
#define TLOG_LOGGER_CRITICAL(logger, ...) (void)0
#define TLOG_CRITICAL(...)                (void)0
#endif
