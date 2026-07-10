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
#include <cstring>
#include <cerrno>
#include <iterator>
#include <string>
#include <type_traits>

#include <xlog/common.h>
#include <xlog/xlog.h>
#include <xlog/log_setting.h>
#include <xlog/internal/nullstream.h>
#include <xlog/internal/conditions.h>
#include <xlog/internal/check_op.h>

// ============================================================================
// Severity helpers
// ============================================================================

namespace xlog {

struct NoLogPrefix {};
struct NoNewLine {};

constexpr NoLogPrefix NOPREFIX{};
constexpr NoNewLine NONEWLINE{};

} // namespace xlog

// ============================================================================
// XlogStreamBuf — streaming accumulator backed by fmt::memory_buffer
// ============================================================================

namespace xlog {
namespace internal {

class XlogStreamBuf {
public:
    XlogStreamBuf(const char *file, int line, xlog::level::level_enum level,
                  bool fatal = false)
        : level_(level), fatal_(fatal) {
        loc_.filename = file;
        loc_.line = line;
    }

    XlogStreamBuf(const char *file, int line, xlog::level::level_enum level,
                  const std::string &msg, bool fatal = false)
        : level_(level), fatal_(fatal) {
        loc_.filename = file;
        loc_.line = line;
        buf_.append(msg.data(), msg.data() + msg.size());
    }

    XlogStreamBuf(const XlogStreamBuf &) = delete;
    XlogStreamBuf &operator=(const XlogStreamBuf &) = delete;

    ~XlogStreamBuf() {
        if (buf_.size() > 0) {
            xlog::default_logger_raw()->log(
                loc_, level_,
                xlog::string_view_t(buf_.data(), buf_.size()));
        }
        if (fatal_) std::abort();
    }

    XlogStreamBuf &internal_stream() { return *this; }

    XlogStreamBuf &no_prefix() { return *this; }
    XlogStreamBuf &no_newline() {
        if (buf_.size() > 0 && buf_.data()[buf_.size() - 1] == '\n')
            buf_.resize(buf_.size() - 1);
        return *this;
    }
    XlogStreamBuf &with_verbosity(int) { return *this; }
    XlogStreamBuf &with_timestamp(void *) { return *this; }
    XlogStreamBuf &with_perror() {
        fmt::format_to(std::back_inserter(buf_), ": {} [{}]",
                       std::strerror(errno), errno);
        return *this;
    }

    XlogStreamBuf &operator<<(const NoLogPrefix &) { return no_prefix(); }
    XlogStreamBuf &operator<<(const NoNewLine &) { return no_newline(); }

    XlogStreamBuf &operator<<(char v) {
        buf_.push_back(v);
        return *this;
    }
    XlogStreamBuf &operator<<(signed char v) {
        buf_.push_back(static_cast<char>(v));
        return *this;
    }
    XlogStreamBuf &operator<<(unsigned char v) {
        buf_.push_back(static_cast<char>(v));
        return *this;
    }
    XlogStreamBuf &operator<<(const char *v) {
        if (v) buf_.append(v, v + std::strlen(v));
        return *this;
    }
    XlogStreamBuf &operator<<(const std::string &v) {
        buf_.append(v.data(), v.data() + v.size());
        return *this;
    }
    XlogStreamBuf &operator<<(std::string_view v) {
        buf_.append(v.data(), v.data() + v.size());
        return *this;
    }

    template <typename T>
    XlogStreamBuf &operator<<(const T &v) {
        fmt::format_to(std::back_inserter(buf_), "{}", v);
        return *this;
    }

private:
    xlog::memory_buf_t buf_;
    xlog::source_loc loc_;
    xlog::level::level_enum level_;
    bool fatal_;
};

} // namespace internal
} // namespace xlog

// ============================================================================
// Internal log body definitions (with compile-time stripping)
// ============================================================================

#define XLOG_INTERNAL_LOG_TRACE \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__, xlog::level::trace)
#define XLOG_INTERNAL_LOG_DEBUG \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__, xlog::level::debug)
#define XLOG_INTERNAL_LOG_INFO \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__, xlog::level::info)
#define XLOG_INTERNAL_LOG_WARNING \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__, xlog::level::warn)
#define XLOG_INTERNAL_LOG_ERROR \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__, xlog::level::err)
#define XLOG_INTERNAL_LOG_FATAL \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__, xlog::level::critical, true)
#define XLOG_INTERNAL_LOG_OFF \
    ::xlog::internal::NullStream()

#define XLOG_INTERNAL_LOG_CHECK_TRACE   (::xlog::log_level() <= XLOG_LEVEL_TRACE)
#define XLOG_INTERNAL_LOG_CHECK_DEBUG   (::xlog::log_level() <= XLOG_LEVEL_DEBUG)
#define XLOG_INTERNAL_LOG_CHECK_INFO    (::xlog::log_level() <= XLOG_LEVEL_INFO)
#define XLOG_INTERNAL_LOG_CHECK_WARNING (::xlog::log_level() <= XLOG_LEVEL_WARNING)
#define XLOG_INTERNAL_LOG_CHECK_ERROR   (::xlog::log_level() <= XLOG_LEVEL_ERROR)
#define XLOG_INTERNAL_LOG_CHECK_FATAL   (::xlog::log_level() <= XLOG_LEVEL_FATAL)

#define XLOG_INTERNAL_LOG_LEVEL(unused) \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__, _xlog_severity_, \
                                    _xlog_severity_ == xlog::level::critical)

// ============================================================================
// Condition macros
// ============================================================================

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
#define XLOG_INTERNAL_CONDITION_LEVEL(severity)                         \
    for (int _xlog_sev_loop_ = 1; _xlog_sev_loop_; _xlog_sev_loop_ = 0) \
        for (const xlog::level::level_enum _xlog_severity_ =          \
                 (severity);                                            \
             _xlog_sev_loop_; _xlog_sev_loop_ = 0)                     \
    XLOG_INTERNAL_CONDITION_LEVEL_IMPL

#define XLOG_INTERNAL_CONDITION_LEVEL_IMPL(type, condition) \
    XLOG_INTERNAL_##type##_CONDITION(condition)

// ============================================================================
// Log implementation macros
// ============================================================================

#define XLOG_INTERNAL_LOG_IMPL(severity) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, XLOG_INTERNAL_LOG_CHECK##severity) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_IMPL(severity, condition) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, (condition) && XLOG_INTERNAL_LOG_CHECK##severity) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_EVERY_N_IMPL(severity, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(EveryN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_FIRST_N_IMPL(severity, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(FirstN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_EVERY_POW_2_IMPL(severity) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(EveryPow2) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(severity, n_seconds) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(EveryNSec, n_seconds) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_EVERY_N_IMPL(severity, condition, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(EveryN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_FIRST_N_IMPL(severity, condition, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(FirstN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_EVERY_POW_2_IMPL(severity, condition) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(EveryPow2) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_EVERY_N_SEC_IMPL(severity, condition, n_seconds) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(EveryNSec, n_seconds) \
    XLOG_INTERNAL_LOG##severity.internal_stream()

// PLOG — stream bodies append errno via with_perror()
#define XLOG_INTERNAL_PLOG_IMPL(severity) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, XLOG_INTERNAL_LOG_CHECK##severity) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_IF_IMPL(severity, condition) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, (condition) && XLOG_INTERNAL_LOG_CHECK##severity) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_EVERY_N_IMPL(severity, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(EveryN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_FIRST_N_IMPL(severity, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(FirstN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_EVERY_POW_2_IMPL(severity) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(EveryPow2) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_EVERY_N_SEC_IMPL(severity, n_seconds) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, XLOG_INTERNAL_LOG_CHECK##severity)(EveryNSec, n_seconds) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_IF_EVERY_N_IMPL(severity, condition, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(EveryN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_IF_FIRST_N_IMPL(severity, condition, n) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(FirstN, n) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_IF_EVERY_POW_2_IMPL(severity, condition) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(EveryPow2) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

#define XLOG_INTERNAL_PLOG_IF_EVERY_N_SEC_IMPL(severity, condition, n_seconds) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, (condition) && XLOG_INTERNAL_LOG_CHECK##severity)(EveryNSec, n_seconds) \
    XLOG_INTERNAL_LOG##severity.internal_stream().with_perror()

// Release debug strip: bare NullStream absorbs << at zero runtime cost.
#define XLOG_INTERNAL_NULL_STREAM() \
    ::xlog::internal::NullStream().internal_stream()

// ============================================================================
// Public API — XLOG family
// ============================================================================

#define XLOG(severity) \
    XLOG_INTERNAL_LOG_IMPL(_##severity)

#define XLOG_IF(severity, condition) \
    XLOG_INTERNAL_LOG_IF_IMPL(_##severity, condition)

#define XLOG_EVERY_N(severity, n) \
    XLOG_INTERNAL_LOG_EVERY_N_IMPL(_##severity, n)

#define XLOG_FIRST_N(severity, n) \
    XLOG_INTERNAL_LOG_FIRST_N_IMPL(_##severity, n)

#define XLOG_ONCE(severity) \
    XLOG_INTERNAL_LOG_FIRST_N_IMPL(_##severity, 1)

#define XLOG_EVERY_POW_2(severity) \
    XLOG_INTERNAL_LOG_EVERY_POW_2_IMPL(_##severity)

#define XLOG_EVERY_N_SEC(severity, n_seconds) \
    XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(_##severity, n_seconds)

#define XLOG_EVERY_SEC(severity) \
    XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(_##severity, 1)

#define XLOG_EVERY_MIN(severity) \
    XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(_##severity, 60)

#define XLOG_IF_EVERY_N(severity, condition, n) \
    XLOG_INTERNAL_LOG_IF_EVERY_N_IMPL(_##severity, condition, n)

#define XLOG_IF_FIRST_N(severity, condition, n) \
    XLOG_INTERNAL_LOG_IF_FIRST_N_IMPL(_##severity, condition, n)

#define XLOG_IF_ONCE(severity, condition) \
    XLOG_INTERNAL_LOG_IF_FIRST_N_IMPL(_##severity, condition, 1)

#define XLOG_IF_EVERY_POW_2(severity, condition) \
    XLOG_INTERNAL_LOG_IF_EVERY_POW_2_IMPL(_##severity, condition)

#define XLOG_IF_EVERY_N_SEC(severity, condition, n_seconds) \
    XLOG_INTERNAL_LOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds)

#define XLOG_IF_EVERY_SEC(severity, condition) \
    XLOG_INTERNAL_LOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, 1)

#define XLOG_IF_EVERY_MIN(severity, condition) \
    XLOG_INTERNAL_LOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, 60)

// ============================================================================
// XLOG_LEVEL — runtime-evaluated severity (stream)
// ============================================================================

#define XLOG_LEVEL(severity) \
    for (int _xlog_sev_loop_ = 1; _xlog_sev_loop_; _xlog_sev_loop_ = 0) \
        for (const xlog::level::level_enum _xlog_severity_ = (severity); \
             _xlog_sev_loop_; _xlog_sev_loop_ = 0) \
            if (::xlog::log_level() <= static_cast<int>(_xlog_severity_)) \
                XLOG_INTERNAL_LOG_LEVEL(_).internal_stream()

// ============================================================================
// XPLOG — perror-appending stream logging (PKLOG equivalent)
// ============================================================================

#define XPLOG(severity) \
    XLOG_INTERNAL_PLOG_IMPL(_##severity)

#define XPLOG_IF(severity, condition) \
    XLOG_INTERNAL_PLOG_IF_IMPL(_##severity, condition)

#define XPLOG_EVERY_N(severity, n) \
    XLOG_INTERNAL_PLOG_EVERY_N_IMPL(_##severity, n)

#define XPLOG_FIRST_N(severity, n) \
    XLOG_INTERNAL_PLOG_FIRST_N_IMPL(_##severity, n)

#define XPLOG_ONCE(severity) \
    XLOG_INTERNAL_PLOG_FIRST_N_IMPL(_##severity, 1)

#define XPLOG_EVERY_POW_2(severity) \
    XLOG_INTERNAL_PLOG_EVERY_POW_2_IMPL(_##severity)

#define XPLOG_EVERY_N_SEC(severity, n_seconds) \
    XLOG_INTERNAL_PLOG_EVERY_N_SEC_IMPL(_##severity, n_seconds)

#define XPLOG_EVERY_SEC(severity) \
    XLOG_INTERNAL_PLOG_EVERY_N_SEC_IMPL(_##severity, 1)

#define XPLOG_EVERY_MIN(severity) \
    XLOG_INTERNAL_PLOG_EVERY_N_SEC_IMPL(_##severity, 60)

#define XPLOG_IF_EVERY_N(severity, condition, n) \
    XLOG_INTERNAL_PLOG_IF_EVERY_N_IMPL(_##severity, condition, n)

#define XPLOG_IF_FIRST_N(severity, condition, n) \
    XLOG_INTERNAL_PLOG_IF_FIRST_N_IMPL(_##severity, condition, n)

#define XPLOG_IF_ONCE(severity, condition) \
    XLOG_INTERNAL_PLOG_IF_FIRST_N_IMPL(_##severity, condition, 1)

#define XPLOG_IF_EVERY_POW_2(severity, condition) \
    XLOG_INTERNAL_PLOG_IF_EVERY_POW_2_IMPL(_##severity, condition)

#define XPLOG_IF_EVERY_N_SEC(severity, condition, n_seconds) \
    XLOG_INTERNAL_PLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds)

#define XPLOG_IF_EVERY_SEC(severity, condition) \
    XLOG_INTERNAL_PLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, 1)

#define XPLOG_IF_EVERY_MIN(severity, condition) \
    XLOG_INTERNAL_PLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, 60)

// ============================================================================
// VXLOG / DVXLOG — verbose logging
// ============================================================================

#define VXLOG(verbose_level)                                                    \
    switch (const int _xlog_verbose_level_ = (verbose_level))                   \
    case 0:                                                                    \
    default:                                                                   \
        XLOG_IF(TRACE, ::xlog::vlog_is_on(_xlog_verbose_level_))               \
            .with_verbosity(_xlog_verbose_level_)

#define VXLOG_EVERY_N(verbose_level, n)                                         \
    switch (const int _xlog_verbose_level_ = (verbose_level))                   \
    case 0:                                                                    \
    default:                                                                   \
        XLOG_INTERNAL_CONDITION_TRACE(                                          \
            STATEFUL, ::xlog::vlog_is_on(_xlog_verbose_level_))                 \
        (EveryN, n) XLOG_INTERNAL_LOG_TRACE.internal_stream()                   \
            .with_verbosity(_xlog_verbose_level_)

#define VXLOG_FIRST_N(verbose_level, n)                                         \
    switch (const int _xlog_verbose_level_ = (verbose_level))                   \
    case 0:                                                                    \
    default:                                                                   \
        XLOG_INTERNAL_CONDITION_TRACE(                                          \
            STATEFUL, ::xlog::vlog_is_on(_xlog_verbose_level_))                 \
        (FirstN, n) XLOG_INTERNAL_LOG_TRACE.internal_stream()                   \
            .with_verbosity(_xlog_verbose_level_)

#define VXLOG_ONCE(verbose_level) VXLOG_FIRST_N(verbose_level, 1)

#define VXLOG_EVERY_POW_2(verbose_level)                                        \
    switch (const int _xlog_verbose_level_ = (verbose_level))                   \
    case 0:                                                                    \
    default:                                                                   \
        XLOG_INTERNAL_CONDITION_TRACE(                                          \
            STATEFUL, ::xlog::vlog_is_on(_xlog_verbose_level_))                 \
        (EveryPow2) XLOG_INTERNAL_LOG_TRACE.internal_stream()                   \
            .with_verbosity(_xlog_verbose_level_)

#define VXLOG_EVERY_N_SEC(verbose_level, n_seconds)                             \
    switch (const int _xlog_verbose_level_ = (verbose_level))                   \
    case 0:                                                                    \
    default:                                                                   \
        XLOG_INTERNAL_CONDITION_TRACE(                                          \
            STATEFUL, ::xlog::vlog_is_on(_xlog_verbose_level_))                 \
        (EveryNSec, n_seconds) XLOG_INTERNAL_LOG_TRACE.internal_stream()        \
            .with_verbosity(_xlog_verbose_level_)

#define VXLOG_EVERY_SEC(verbose_level) VXLOG_EVERY_N_SEC(verbose_level, 1)
#define VXLOG_EVERY_MIN(verbose_level) VXLOG_EVERY_N_SEC(verbose_level, 60)

// ============================================================================
// DXLOG / DVXLOG — debug-only variants
// ============================================================================

#ifndef NDEBUG

#define DXLOG(severity) XLOG(severity)
#define DXLOG_IF(severity, condition) XLOG_IF(severity, condition)
#define DXLOG_EVERY_N(severity, n) XLOG_EVERY_N(severity, n)
#define DXLOG_FIRST_N(severity, n) XLOG_FIRST_N(severity, n)
#define DXLOG_ONCE(severity) XLOG_ONCE(severity)
#define DXLOG_EVERY_POW_2(severity) XLOG_EVERY_POW_2(severity)
#define DXLOG_EVERY_N_SEC(severity, n_seconds) XLOG_EVERY_N_SEC(severity, n_seconds)
#define DXLOG_EVERY_SEC(severity) XLOG_EVERY_SEC(severity)
#define DXLOG_EVERY_MIN(severity) XLOG_EVERY_MIN(severity)
#define DXLOG_IF_EVERY_N(severity, cond, n) XLOG_IF_EVERY_N(severity, cond, n)
#define DXLOG_IF_FIRST_N(severity, cond, n) XLOG_IF_FIRST_N(severity, cond, n)
#define DXLOG_IF_ONCE(severity, cond) XLOG_IF_ONCE(severity, cond)
#define DXLOG_IF_EVERY_POW_2(severity, cond) XLOG_IF_EVERY_POW_2(severity, cond)
#define DXLOG_IF_EVERY_N_SEC(severity, cond, s) XLOG_IF_EVERY_N_SEC(severity, cond, s)
#define DXLOG_IF_EVERY_SEC(severity, cond) XLOG_IF_EVERY_SEC(severity, cond)
#define DXLOG_IF_EVERY_MIN(severity, cond) XLOG_IF_EVERY_MIN(severity, cond)

#define DXPLOG(severity) XPLOG(severity)
#define DXPLOG_IF(severity, condition) XPLOG_IF(severity, condition)
#define DXPLOG_EVERY_N(severity, n) XPLOG_EVERY_N(severity, n)
#define DXPLOG_FIRST_N(severity, n) XPLOG_FIRST_N(severity, n)
#define DXPLOG_ONCE(severity) XPLOG_ONCE(severity)
#define DXPLOG_EVERY_POW_2(severity) XPLOG_EVERY_POW_2(severity)
#define DXPLOG_EVERY_N_SEC(severity, n_seconds) XPLOG_EVERY_N_SEC(severity, n_seconds)
#define DXPLOG_EVERY_SEC(severity) XPLOG_EVERY_SEC(severity)
#define DXPLOG_EVERY_MIN(severity) XPLOG_EVERY_MIN(severity)
#define DXPLOG_IF_EVERY_N(severity, cond, n) XPLOG_IF_EVERY_N(severity, cond, n)
#define DXPLOG_IF_FIRST_N(severity, cond, n) XPLOG_IF_FIRST_N(severity, cond, n)
#define DXPLOG_IF_ONCE(severity, cond) XPLOG_IF_ONCE(severity, cond)
#define DXPLOG_IF_EVERY_POW_2(severity, cond) XPLOG_IF_EVERY_POW_2(severity, cond)
#define DXPLOG_IF_EVERY_N_SEC(severity, cond, s) XPLOG_IF_EVERY_N_SEC(severity, cond, s)
#define DXPLOG_IF_EVERY_SEC(severity, cond) XPLOG_IF_EVERY_SEC(severity, cond)
#define DXPLOG_IF_EVERY_MIN(severity, cond) XPLOG_IF_EVERY_MIN(severity, cond)

#define DVXLOG(verbose_level) VXLOG(verbose_level)
#define DVXLOG_EVERY_N(v, n) VXLOG_EVERY_N(v, n)
#define DVXLOG_FIRST_N(v, n) VXLOG_FIRST_N(v, n)
#define DVXLOG_ONCE(v) VXLOG_ONCE(v)
#define DVXLOG_EVERY_POW_2(v) VXLOG_EVERY_POW_2(v)
#define DVXLOG_EVERY_N_SEC(v, s) VXLOG_EVERY_N_SEC(v, s)
#define DVXLOG_EVERY_SEC(v) VXLOG_EVERY_SEC(v)
#define DVXLOG_EVERY_MIN(v) VXLOG_EVERY_MIN(v)

#else // NDEBUG

#define DXLOG(severity)                         XLOG_INTERNAL_NULL_STREAM()
#define DXLOG_IF(severity, condition)           ((void)(condition), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_EVERY_N(severity, n)              ((void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_FIRST_N(severity, n)              ((void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_ONCE(severity)                    XLOG_INTERNAL_NULL_STREAM()
#define DXLOG_EVERY_POW_2(severity)             XLOG_INTERNAL_NULL_STREAM()
#define DXLOG_EVERY_N_SEC(severity, n_seconds)   ((void)(n_seconds), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_EVERY_SEC(severity)                 XLOG_INTERNAL_NULL_STREAM()
#define DXLOG_EVERY_MIN(severity)                 XLOG_INTERNAL_NULL_STREAM()
#define DXLOG_IF_EVERY_N(severity, cond, n)     ((void)(cond), (void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_IF_FIRST_N(severity, cond, n)     ((void)(cond), (void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_IF_ONCE(severity, cond)           ((void)(cond), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_IF_EVERY_POW_2(severity, cond)    ((void)(cond), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_IF_EVERY_N_SEC(severity, cond, s) ((void)(cond), (void)(s), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_IF_EVERY_SEC(severity, cond)      ((void)(cond), XLOG_INTERNAL_NULL_STREAM())
#define DXLOG_IF_EVERY_MIN(severity, cond)      ((void)(cond), XLOG_INTERNAL_NULL_STREAM())

#define DXPLOG(severity)                         XLOG_INTERNAL_NULL_STREAM()
#define DXPLOG_IF(severity, condition)           ((void)(condition), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_EVERY_N(severity, n)              ((void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_FIRST_N(severity, n)              ((void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_ONCE(severity)                    XLOG_INTERNAL_NULL_STREAM()
#define DXPLOG_EVERY_POW_2(severity)             XLOG_INTERNAL_NULL_STREAM()
#define DXPLOG_EVERY_N_SEC(severity, n_seconds)   ((void)(n_seconds), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_EVERY_SEC(severity)                 XLOG_INTERNAL_NULL_STREAM()
#define DXPLOG_EVERY_MIN(severity)                 XLOG_INTERNAL_NULL_STREAM()
#define DXPLOG_IF_EVERY_N(severity, cond, n)     ((void)(cond), (void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_IF_FIRST_N(severity, cond, n)     ((void)(cond), (void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_IF_ONCE(severity, cond)           ((void)(cond), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_IF_EVERY_POW_2(severity, cond)    ((void)(cond), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_IF_EVERY_N_SEC(severity, cond, s) ((void)(cond), (void)(s), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_IF_EVERY_SEC(severity, cond)      ((void)(cond), XLOG_INTERNAL_NULL_STREAM())
#define DXPLOG_IF_EVERY_MIN(severity, cond)      ((void)(cond), XLOG_INTERNAL_NULL_STREAM())

#define DVXLOG(v)                                 ((void)(v), XLOG_INTERNAL_NULL_STREAM())
#define DVXLOG_EVERY_N(v, n)                      ((void)(v), (void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DVXLOG_FIRST_N(v, n)                      ((void)(v), (void)(n), XLOG_INTERNAL_NULL_STREAM())
#define DVXLOG_ONCE(v)                            ((void)(v), XLOG_INTERNAL_NULL_STREAM())
#define DVXLOG_EVERY_POW_2(v)                     ((void)(v), XLOG_INTERNAL_NULL_STREAM())
#define DVXLOG_EVERY_N_SEC(v, s)                   ((void)(v), (void)(s), XLOG_INTERNAL_NULL_STREAM())
#define DVXLOG_EVERY_SEC(v)                        ((void)(v), XLOG_INTERNAL_NULL_STREAM())
#define DVXLOG_EVERY_MIN(v)                        ((void)(v), XLOG_INTERNAL_NULL_STREAM())

#endif // NDEBUG

// ============================================================================
// XCHECK — stream-style CHECK family
// ============================================================================

#define XLOG_INTERNAL_CHECK_X()                                                 \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__,                         \
        xlog::level::critical, "Check failed: ", true)

#define XLOG_INTERNAL_CHECK_OP_FATAL(msg)                                       \
    ::xlog::internal::XlogStreamBuf(__FILE__, __LINE__,                         \
        xlog::level::critical, msg, true)

#define XCHECK(condition)                                                       \
    XLOG_INTERNAL_CONDITION_FATAL(STATELESS, !(condition))                      \
    XLOG_INTERNAL_CHECK_X().internal_stream()

#define XCHECK_ERR(invocation) \
    XPLOG_IF(FATAL, (invocation) == -1) << #invocation

#define XCHECK_NOTNULL(val)                                                     \
    ::xlog::internal::CheckNotNull(__FILE__, __LINE__,                          \
        "'" #val "' Must be non nullptr", (val))

#define XCHECK_EQ(val1, val2)                                                   \
    while (auto _xcheck_op_result_ =                                            \
               ::xlog::internal::Check_EQImpl(                                  \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " == " #val2))                                         \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_op_result_).internal_stream()

#define XCHECK_NE(val1, val2)                                                   \
    while (auto _xcheck_op_result_ =                                            \
               ::xlog::internal::Check_NEImpl(                                  \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " != " #val2))                                         \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_op_result_).internal_stream()

#define XCHECK_LE(val1, val2)                                                   \
    while (auto _xcheck_op_result_ =                                            \
               ::xlog::internal::Check_LEImpl(                                  \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " <= " #val2))                                         \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_op_result_).internal_stream()

#define XCHECK_LT(val1, val2)                                                   \
    while (auto _xcheck_op_result_ =                                            \
               ::xlog::internal::Check_LTImpl(                                  \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " < " #val2))                                          \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_op_result_).internal_stream()

#define XCHECK_GE(val1, val2)                                                   \
    while (auto _xcheck_op_result_ =                                            \
               ::xlog::internal::Check_GEImpl(                                  \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " >= " #val2))                                         \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_op_result_).internal_stream()

#define XCHECK_GT(val1, val2)                                                   \
    while (auto _xcheck_op_result_ =                                            \
               ::xlog::internal::Check_GTImpl(                                  \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " > " #val2))                                          \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_op_result_).internal_stream()

#define XCHECK_STREQ(s1, s2)                                                    \
    while (auto _xcheck_strop_result_ =                                         \
               ::xlog::internal::CheckstrcmptrueImpl(                           \
                   (s1), (s2), #s1 " == " #s2))                                  \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_strop_result_).internal_stream()

#define XCHECK_STRNE(s1, s2)                                                    \
    while (auto _xcheck_strop_result_ =                                         \
               ::xlog::internal::CheckstrcmpfalseImpl(                           \
                   (s1), (s2), #s1 " != " #s2))                                  \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_strop_result_).internal_stream()

#define XCHECK_STRCASEEQ(s1, s2)                                                \
    while (auto _xcheck_strop_result_ =                                         \
               ::xlog::internal::CheckstrcasecmptrueImpl(                       \
                   (s1), (s2), #s1 " == " #s2))                                  \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_strop_result_).internal_stream()

#define XCHECK_STRCASENE(s1, s2)                                                \
    while (auto _xcheck_strop_result_ =                                         \
               ::xlog::internal::CheckstrcasecmpfalseImpl(                      \
                   (s1), (s2), #s1 " != " #s2))                                  \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                          \
    XLOG_INTERNAL_CHECK_OP_FATAL(*_xcheck_strop_result_).internal_stream()

#ifndef NDEBUG

#define DXCHECK(cond) XCHECK(cond)
#define DXCHECK_EQ(v1, v2) XCHECK_EQ(v1, v2)
#define DXCHECK_NE(v1, v2) XCHECK_NE(v1, v2)
#define DXCHECK_LE(v1, v2) XCHECK_LE(v1, v2)
#define DXCHECK_LT(v1, v2) XCHECK_LT(v1, v2)
#define DXCHECK_GE(v1, v2) XCHECK_GE(v1, v2)
#define DXCHECK_GT(v1, v2) XCHECK_GT(v1, v2)
#define DXCHECK_STREQ(s1, s2) XCHECK_STREQ(s1, s2)
#define DXCHECK_STRNE(s1, s2) XCHECK_STRNE(s1, s2)
#define DXCHECK_STRCASEEQ(s1, s2) XCHECK_STRCASEEQ(s1, s2)
#define DXCHECK_STRCASENE(s1, s2) XCHECK_STRCASENE(s1, s2)
#define DXCHECK_ERR(invocation) XCHECK_ERR(invocation)
#define DXCHECK_NOTNULL(val) XCHECK_NOTNULL(val)

#else // NDEBUG

#define DXCHECK(cond)                        ((void)0)
#define DXCHECK_EQ(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DXCHECK_NE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DXCHECK_LE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DXCHECK_LT(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DXCHECK_GE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DXCHECK_GT(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DXCHECK_STREQ(s1, s2)                ((void)(s1), (void)(s2), (void)0)
#define DXCHECK_STRNE(s1, s2)                ((void)(s1), (void)(s2), (void)0)
#define DXCHECK_STRCASEEQ(s1, s2)            ((void)(s1), (void)(s2), (void)0)
#define DXCHECK_STRCASENE(s1, s2)            ((void)(s1), (void)(s2), (void)0)
#define DXCHECK_ERR(invocation)              ((void)(invocation), (void)0)
#define DXCHECK_NOTNULL(val)                 (val)

#endif // NDEBUG

#define UNREACHABLE() XCHECK(false)
