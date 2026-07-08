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
#include <string>

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
// TPLOG — fmt-style logging with errno appended (PKLOG equivalent)
// ============================================================================

#define XLOG_INTERNAL_TLOG_PLOG_MSG(...) \
    fmt::format(FMT_STRING("{}: {} [{}]"), \
        fmt::format(__VA_ARGS__), std::strerror(errno), errno)

#define TPLOG_BODY_TRACE(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::trace, XLOG_INTERNAL_TLOG_PLOG_MSG(__VA_ARGS__))

#define TPLOG_BODY_DEBUG(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::debug, XLOG_INTERNAL_TLOG_PLOG_MSG(__VA_ARGS__))

#define TPLOG_BODY_INFO(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::info, XLOG_INTERNAL_TLOG_PLOG_MSG(__VA_ARGS__))

#define TPLOG_BODY_WARNING(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::warn, XLOG_INTERNAL_TLOG_PLOG_MSG(__VA_ARGS__))

#define TPLOG_BODY_ERROR(...) \
    ::xlog::default_logger_raw()->log( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::err, XLOG_INTERNAL_TLOG_PLOG_MSG(__VA_ARGS__))

#define TPLOG_BODY_FATAL(...) \
    (::xlog::default_logger_raw()->log( \
         ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
         ::xlog::level::critical, XLOG_INTERNAL_TLOG_PLOG_MSG(__VA_ARGS__)), \
     ::std::abort())

#define TPLOG(severity, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity) \
        TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_IF(severity, condition, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity && (condition)) \
        TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_EVERY_N(severity, n, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(n); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_FIRST_N(severity, n, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(n); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_ONCE(severity, ...) \
    TPLOG_FIRST_N(severity, 1, __VA_ARGS__)

#define TPLOG_EVERY_POW_2(severity, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_EVERY_N_SEC(severity, n_seconds, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(n_seconds); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_EVERY_SEC(severity, ...) \
    TPLOG_EVERY_N_SEC(severity, 1, __VA_ARGS__)

#define TPLOG_EVERY_MIN(severity, ...) \
    TPLOG_EVERY_N_SEC(severity, 60, __VA_ARGS__)

#define TPLOG_IF_EVERY_N(severity, condition, n, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(n); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_IF_FIRST_N(severity, condition, n, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(n); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_IF_ONCE(severity, condition, ...) \
    TPLOG_IF_FIRST_N(severity, condition, 1, __VA_ARGS__)

#define TPLOG_IF_EVERY_POW_2(severity, condition, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, ...) \
    for (bool _tplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _tplog_enabled_; _tplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _tplog_state_; \
             _tplog_enabled_ && _tplog_state_.ShouldLog(n_seconds); \
             _tplog_enabled_ = false) \
            TPLOG_BODY_##severity(__VA_ARGS__)

#define TPLOG_IF_EVERY_SEC(severity, condition, ...) \
    TPLOG_IF_EVERY_N_SEC(severity, condition, 1, __VA_ARGS__)

#define TPLOG_IF_EVERY_MIN(severity, condition, ...) \
    TPLOG_IF_EVERY_N_SEC(severity, condition, 60, __VA_ARGS__)

#define TPLOG_LEVEL(severity, ...) \
    do { \
        auto _tplog_sev_ = (severity); \
        if (::xlog::log_level() <= _tplog_sev_) { \
            ::xlog::default_logger_raw()->log( \
                ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
                _tplog_sev_, XLOG_INTERNAL_TLOG_PLOG_MSG(__VA_ARGS__)); \
            if (_tplog_sev_ == ::xlog::level::critical) ::std::abort(); \
        } \
    } while (0)

// ============================================================================
// TVLOG — verbose logging
// ============================================================================

#define TVLOG(verbose_level, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        if (::xlog::vlog_is_on(_tlog_verbose_)) \
            TLOG_BODY_TRACE(__VA_ARGS__)

#define TVLOG_EVERY_N(verbose_level, n, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::vlog_is_on(_tlog_verbose_); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryNState _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_TRACE(__VA_ARGS__)

#define TVLOG_FIRST_N(verbose_level, n, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::vlog_is_on(_tlog_verbose_); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogFirstNState _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(n); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_TRACE(__VA_ARGS__)

#define TVLOG_ONCE(verbose_level, ...) \
    TVLOG_FIRST_N(verbose_level, 1, __VA_ARGS__)

#define TVLOG_EVERY_POW_2(verbose_level, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::vlog_is_on(_tlog_verbose_); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryPow2State _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_TRACE(__VA_ARGS__)

#define TVLOG_EVERY_N_SEC(verbose_level, n_seconds, ...) \
    switch (const int _tlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _tlog_enabled_ = ::xlog::vlog_is_on(_tlog_verbose_); \
             _tlog_enabled_; _tlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryNSecState _tlog_state_; \
                 _tlog_enabled_ && _tlog_state_.ShouldLog(n_seconds); \
                 _tlog_enabled_ = false) \
                TLOG_BODY_TRACE(__VA_ARGS__)

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

#define DTPLOG(severity, ...)                  TPLOG(severity, __VA_ARGS__)
#define DTPLOG_IF(severity, cond, ...)          TPLOG_IF(severity, cond, __VA_ARGS__)
#define DTPLOG_EVERY_N(severity, n, ...)        TPLOG_EVERY_N(severity, n, __VA_ARGS__)
#define DTPLOG_FIRST_N(severity, n, ...)        TPLOG_FIRST_N(severity, n, __VA_ARGS__)
#define DTPLOG_ONCE(severity, ...)              TPLOG_ONCE(severity, __VA_ARGS__)
#define DTPLOG_EVERY_POW_2(severity, ...)       TPLOG_EVERY_POW_2(severity, __VA_ARGS__)
#define DTPLOG_EVERY_N_SEC(severity, s, ...)    TPLOG_EVERY_N_SEC(severity, s, __VA_ARGS__)
#define DTPLOG_EVERY_SEC(severity, ...)         TPLOG_EVERY_SEC(severity, __VA_ARGS__)
#define DTPLOG_EVERY_MIN(severity, ...)         TPLOG_EVERY_MIN(severity, __VA_ARGS__)
#define DTPLOG_IF_EVERY_N(severity, c, n, ...)  TPLOG_IF_EVERY_N(severity, c, n, __VA_ARGS__)
#define DTPLOG_IF_FIRST_N(severity, c, n, ...)  TPLOG_IF_FIRST_N(severity, c, n, __VA_ARGS__)
#define DTPLOG_IF_ONCE(severity, c, ...)        TPLOG_IF_ONCE(severity, c, __VA_ARGS__)
#define DTPLOG_IF_EVERY_POW_2(severity, c, ...) TPLOG_IF_EVERY_POW_2(severity, c, __VA_ARGS__)
#define DTPLOG_IF_EVERY_N_SEC(severity, c, s, ...) TPLOG_IF_EVERY_N_SEC(severity, c, s, __VA_ARGS__)
#define DTPLOG_IF_EVERY_SEC(severity, c, ...)   TPLOG_IF_EVERY_SEC(severity, c, __VA_ARGS__)
#define DTPLOG_IF_EVERY_MIN(severity, c, ...)   TPLOG_IF_EVERY_MIN(severity, c, __VA_ARGS__)

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

#define DTPLOG(severity, ...)                   (void)0
#define DTPLOG_IF(severity, cond, ...)          (void)0
#define DTPLOG_EVERY_N(severity, n, ...)        (void)0
#define DTPLOG_FIRST_N(severity, n, ...)        (void)0
#define DTPLOG_ONCE(severity, ...)              (void)0
#define DTPLOG_EVERY_POW_2(severity, ...)       (void)0
#define DTPLOG_EVERY_N_SEC(severity, s, ...)    (void)0
#define DTPLOG_EVERY_SEC(severity, ...)         (void)0
#define DTPLOG_EVERY_MIN(severity, ...)         (void)0
#define DTPLOG_IF_EVERY_N(severity, c, n, ...)  (void)0
#define DTPLOG_IF_FIRST_N(severity, c, n, ...)  (void)0
#define DTPLOG_IF_ONCE(severity, c, ...)        (void)0
#define DTPLOG_IF_EVERY_POW_2(severity, c, ...) (void)0
#define DTPLOG_IF_EVERY_N_SEC(severity, c, s, ...) (void)0
#define DTPLOG_IF_EVERY_SEC(severity, c, ...)   (void)0
#define DTPLOG_IF_EVERY_MIN(severity, c, ...)   (void)0

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
// TCHECK — fmt-style CHECK family
// ============================================================================

#define TCHECK(condition, ...) \
    if (!(condition)) \
        ::xlog::internal::CheckFailFmtImpl(__FILE__, __LINE__, #condition, ##__VA_ARGS__)

#define TCHECK_ERR(invocation) \
    if ((invocation) == -1) \
        ::xlog::internal::CheckFailFmtImpl(__FILE__, __LINE__, #invocation, \
            ": {} [{}]", std::strerror(errno), errno)

#define TCHECK_NOTNULL(val) \
    ::xlog::internal::CheckNotNull(__FILE__, __LINE__, \
        "'" #val "' Must be non nullptr", (val))

#define TCHECK_EQ(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_EQ, ==, val1, val2)
#define TCHECK_NE(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_NE, !=, val1, val2)
#define TCHECK_LE(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_LE, <=, val1, val2)
#define TCHECK_LT(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_LT, <, val1, val2)
#define TCHECK_GE(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_GE, >=, val1, val2)
#define TCHECK_GT(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_GT, >, val1, val2)

#define TCHECK_STREQ(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcmp, ==, true, s1, s2)
#define TCHECK_STRNE(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcmp, !=, false, s1, s2)
#define TCHECK_STRCASEEQ(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcasecmp, ==, true, s1, s2)
#define TCHECK_STRCASENE(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcasecmp, !=, false, s1, s2)

#ifndef NDEBUG

#define DTCHECK(...)                         TCHECK(__VA_ARGS__)
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

#define DTCHECK(cond, ...)                   ((void)0)
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
