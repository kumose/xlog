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
// ZLOG internal log body macros — printf-style via log_printf()
// ============================================================================

#define ZLOG_BODY_TRACE(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::trace, fmt , ##__VA_ARGS__)

#define ZLOG_BODY_DEBUG(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::debug, fmt , ##__VA_ARGS__)

#define ZLOG_BODY_INFO(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::info, fmt , ##__VA_ARGS__)

#define ZLOG_BODY_WARNING(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::warn, fmt , ##__VA_ARGS__)

#define ZLOG_BODY_ERROR(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::err, fmt , ##__VA_ARGS__)

#define ZLOG_BODY_FATAL(fmt, ...) \
    (::xlog::default_logger_raw()->log_printf( \
         ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
         ::xlog::level::critical, fmt , ##__VA_ARGS__)), \
     ::std::abort())

// ============================================================================
// ZLOG — printf-style logging with runtime level filter
// ============================================================================

#define ZLOG(severity, fmt, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity) \
        ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_IF(severity, condition, fmt, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity && (condition)) \
        ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

// ============================================================================
// Rate-limited variants
// ============================================================================

#define ZLOG_EVERY_N(severity, n, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(n); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_FIRST_N(severity, n, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(n); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_ONCE(severity, fmt, ...) \
    ZLOG_FIRST_N(severity, 1, fmt , ##__VA_ARGS__)

#define ZLOG_EVERY_POW_2(severity, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_EVERY_N_SEC(severity, n_seconds, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(n_seconds); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_EVERY_SEC(severity, fmt, ...) \
    ZLOG_EVERY_N_SEC(severity, 1, fmt , ##__VA_ARGS__)

#define ZLOG_EVERY_MIN(severity, fmt, ...) \
    ZLOG_EVERY_N_SEC(severity, 60, fmt , ##__VA_ARGS__)

// ============================================================================
// Conditional rate-limited variants
// ============================================================================

#define ZLOG_IF_EVERY_N(severity, condition, n, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(n); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_IF_FIRST_N(severity, condition, n, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(n); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_IF_ONCE(severity, condition, fmt, ...) \
    ZLOG_IF_FIRST_N(severity, condition, 1, fmt , ##__VA_ARGS__)

#define ZLOG_IF_EVERY_POW_2(severity, condition, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, fmt, ...) \
    for (bool _zlog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zlog_enabled_; _zlog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _zlog_state_; \
             _zlog_enabled_ && _zlog_state_.ShouldLog(n_seconds); \
             _zlog_enabled_ = false) \
            ZLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZLOG_IF_EVERY_SEC(severity, condition, fmt, ...) \
    ZLOG_IF_EVERY_N_SEC(severity, condition, 1, fmt , ##__VA_ARGS__)

#define ZLOG_IF_EVERY_MIN(severity, condition, fmt, ...) \
    ZLOG_IF_EVERY_N_SEC(severity, condition, 60, fmt , ##__VA_ARGS__)

// ============================================================================
// ZPLOG — printf-style logging with errno appended (PKLOG equivalent)
// ============================================================================

#define ZPLOG_BODY_TRACE(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::trace, fmt ": %s [%d]" , ##__VA_ARGS__, std::strerror(errno), errno)

#define ZPLOG_BODY_DEBUG(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::debug, fmt ": %s [%d]" , ##__VA_ARGS__, std::strerror(errno), errno)

#define ZPLOG_BODY_INFO(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::info, fmt ": %s [%d]" , ##__VA_ARGS__, std::strerror(errno), errno)

#define ZPLOG_BODY_WARNING(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::warn, fmt ": %s [%d]" , ##__VA_ARGS__, std::strerror(errno), errno)

#define ZPLOG_BODY_ERROR(fmt, ...) \
    ::xlog::default_logger_raw()->log_printf( \
        ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
        ::xlog::level::err, fmt ": %s [%d]" , ##__VA_ARGS__, std::strerror(errno), errno)

#define ZPLOG_BODY_FATAL(fmt, ...) \
    (::xlog::default_logger_raw()->log_printf( \
         ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
         ::xlog::level::critical, fmt ": %s [%d]" , ##__VA_ARGS__, \
         std::strerror(errno), errno), \
     ::std::abort())

#define ZPLOG(severity, fmt, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity) \
        ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_IF(severity, condition, fmt, ...) \
    if (::xlog::log_level() <= XLOG_LEVEL_##severity && (condition)) \
        ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_EVERY_N(severity, n, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(n); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_FIRST_N(severity, n, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(n); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_ONCE(severity, fmt, ...) \
    ZPLOG_FIRST_N(severity, 1, fmt , ##__VA_ARGS__)

#define ZPLOG_EVERY_POW_2(severity, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_EVERY_N_SEC(severity, n_seconds, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity; \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(n_seconds); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_EVERY_SEC(severity, fmt, ...) \
    ZPLOG_EVERY_N_SEC(severity, 1, fmt , ##__VA_ARGS__)

#define ZPLOG_EVERY_MIN(severity, fmt, ...) \
    ZPLOG_EVERY_N_SEC(severity, 60, fmt , ##__VA_ARGS__)

#define ZPLOG_IF_EVERY_N(severity, condition, n, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNState _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(n); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_IF_FIRST_N(severity, condition, n, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogFirstNState _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(n); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_IF_ONCE(severity, condition, fmt, ...) \
    ZPLOG_IF_FIRST_N(severity, condition, 1, fmt , ##__VA_ARGS__)

#define ZPLOG_IF_EVERY_POW_2(severity, condition, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryPow2State _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, fmt, ...) \
    for (bool _zplog_enabled_ = ::xlog::log_level() <= XLOG_LEVEL_##severity && (condition); \
         _zplog_enabled_; _zplog_enabled_ = false) \
        for (static ::xlog::internal::LogEveryNSecState _zplog_state_; \
             _zplog_enabled_ && _zplog_state_.ShouldLog(n_seconds); \
             _zplog_enabled_ = false) \
            ZPLOG_BODY_##severity(fmt , ##__VA_ARGS__)

#define ZPLOG_IF_EVERY_SEC(severity, condition, fmt, ...) \
    ZPLOG_IF_EVERY_N_SEC(severity, condition, 1, fmt , ##__VA_ARGS__)

#define ZPLOG_IF_EVERY_MIN(severity, condition, fmt, ...) \
    ZPLOG_IF_EVERY_N_SEC(severity, condition, 60, fmt , ##__VA_ARGS__)

#define ZPLOG_LEVEL(severity, fmt, ...) \
    do { \
        auto _zplog_sev_ = (severity); \
        if (::xlog::log_level() <= _zplog_sev_) { \
            ::xlog::default_logger_raw()->log_printf( \
                ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
                _zplog_sev_, fmt ": %s [%d]" , ##__VA_ARGS__, \
                std::strerror(errno), errno); \
            if (_zplog_sev_ == ::xlog::level::critical) ::std::abort(); \
        } \
    } while (0)

// ============================================================================
// VZLOG — verbose logging (always at TRACE severity)
// ============================================================================

#define VZLOG(verbose_level, fmt, ...) \
    switch (const int _zlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        if (::xlog::vlog_is_on(_zlog_verbose_)) \
            ZLOG_BODY_TRACE(fmt , ##__VA_ARGS__)

#define VZLOG_EVERY_N(verbose_level, n, fmt, ...) \
    switch (const int _zlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _zlog_enabled_ = ::xlog::vlog_is_on(_zlog_verbose_); \
             _zlog_enabled_; _zlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryNState _zlog_state_; \
                 _zlog_enabled_ && _zlog_state_.ShouldLog(n); \
                 _zlog_enabled_ = false) \
                ZLOG_BODY_TRACE(fmt , ##__VA_ARGS__)

#define VZLOG_FIRST_N(verbose_level, n, fmt, ...) \
    switch (const int _zlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _zlog_enabled_ = ::xlog::vlog_is_on(_zlog_verbose_); \
             _zlog_enabled_; _zlog_enabled_ = false) \
            for (static ::xlog::internal::LogFirstNState _zlog_state_; \
                 _zlog_enabled_ && _zlog_state_.ShouldLog(n); \
                 _zlog_enabled_ = false) \
                ZLOG_BODY_TRACE(fmt , ##__VA_ARGS__)

#define VZLOG_ONCE(verbose_level, fmt, ...) \
    VZLOG_FIRST_N(verbose_level, 1, fmt , ##__VA_ARGS__)

#define VZLOG_EVERY_POW_2(verbose_level, fmt, ...) \
    switch (const int _zlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _zlog_enabled_ = ::xlog::vlog_is_on(_zlog_verbose_); \
             _zlog_enabled_; _zlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryPow2State _zlog_state_; \
                 _zlog_enabled_ && _zlog_state_.ShouldLog(); \
                 _zlog_enabled_ = false) \
                ZLOG_BODY_TRACE(fmt , ##__VA_ARGS__)

#define VZLOG_EVERY_N_SEC(verbose_level, n_seconds, fmt, ...) \
    switch (const int _zlog_verbose_ = (verbose_level)) \
    case 0: \
    default: \
        for (bool _zlog_enabled_ = ::xlog::vlog_is_on(_zlog_verbose_); \
             _zlog_enabled_; _zlog_enabled_ = false) \
            for (static ::xlog::internal::LogEveryNSecState _zlog_state_; \
                 _zlog_enabled_ && _zlog_state_.ShouldLog(n_seconds); \
                 _zlog_enabled_ = false) \
                ZLOG_BODY_TRACE(fmt , ##__VA_ARGS__)

#define VZLOG_EVERY_SEC(verbose_level, fmt, ...) \
    VZLOG_EVERY_N_SEC(verbose_level, 1, fmt , ##__VA_ARGS__)

#define VZLOG_EVERY_MIN(verbose_level, fmt, ...) \
    VZLOG_EVERY_N_SEC(verbose_level, 60, fmt , ##__VA_ARGS__)

// ============================================================================
// DZLOG / DVZLOG — debug-only variants
// ============================================================================

#ifndef NDEBUG

#define DZLOG(severity, fmt, ...)                  ZLOG(severity, fmt , ##__VA_ARGS__)
#define DZLOG_IF(severity, cond, fmt, ...)          ZLOG_IF(severity, cond, fmt , ##__VA_ARGS__)
#define DZLOG_EVERY_N(severity, n, fmt, ...)        ZLOG_EVERY_N(severity, n, fmt , ##__VA_ARGS__)
#define DZLOG_FIRST_N(severity, n, fmt, ...)        ZLOG_FIRST_N(severity, n, fmt , ##__VA_ARGS__)
#define DZLOG_ONCE(severity, fmt, ...)              ZLOG_ONCE(severity, fmt , ##__VA_ARGS__)
#define DZLOG_EVERY_POW_2(severity, fmt, ...)       ZLOG_EVERY_POW_2(severity, fmt , ##__VA_ARGS__)
#define DZLOG_EVERY_N_SEC(severity, s, fmt, ...)    ZLOG_EVERY_N_SEC(severity, s, fmt , ##__VA_ARGS__)
#define DZLOG_EVERY_SEC(severity, fmt, ...)         ZLOG_EVERY_SEC(severity, fmt , ##__VA_ARGS__)
#define DZLOG_EVERY_MIN(severity, fmt, ...)         ZLOG_EVERY_MIN(severity, fmt , ##__VA_ARGS__)
#define DZLOG_IF_EVERY_N(severity, c, n, fmt, ...)  ZLOG_IF_EVERY_N(severity, c, n, fmt , ##__VA_ARGS__)
#define DZLOG_IF_FIRST_N(severity, c, n, fmt, ...)  ZLOG_IF_FIRST_N(severity, c, n, fmt , ##__VA_ARGS__)
#define DZLOG_IF_ONCE(severity, c, fmt, ...)        ZLOG_IF_ONCE(severity, c, fmt , ##__VA_ARGS__)
#define DZLOG_IF_EVERY_POW_2(severity, c, fmt, ...) ZLOG_IF_EVERY_POW_2(severity, c, fmt , ##__VA_ARGS__)
#define DZLOG_IF_EVERY_N_SEC(severity, c, s, fmt, ...) ZLOG_IF_EVERY_N_SEC(severity, c, s, fmt , ##__VA_ARGS__)
#define DZLOG_IF_EVERY_SEC(severity, c, fmt, ...)   ZLOG_IF_EVERY_SEC(severity, c, fmt , ##__VA_ARGS__)
#define DZLOG_IF_EVERY_MIN(severity, c, fmt, ...)   ZLOG_IF_EVERY_MIN(severity, c, fmt , ##__VA_ARGS__)

#define DZPLOG(severity, fmt, ...)                  ZPLOG(severity, fmt , ##__VA_ARGS__)
#define DZPLOG_IF(severity, cond, fmt, ...)          ZPLOG_IF(severity, cond, fmt , ##__VA_ARGS__)
#define DZPLOG_EVERY_N(severity, n, fmt, ...)        ZPLOG_EVERY_N(severity, n, fmt , ##__VA_ARGS__)
#define DZPLOG_FIRST_N(severity, n, fmt, ...)        ZPLOG_FIRST_N(severity, n, fmt , ##__VA_ARGS__)
#define DZPLOG_ONCE(severity, fmt, ...)              ZPLOG_ONCE(severity, fmt , ##__VA_ARGS__)
#define DZPLOG_EVERY_POW_2(severity, fmt, ...)       ZPLOG_EVERY_POW_2(severity, fmt , ##__VA_ARGS__)
#define DZPLOG_EVERY_N_SEC(severity, s, fmt, ...)    ZPLOG_EVERY_N_SEC(severity, s, fmt , ##__VA_ARGS__)
#define DZPLOG_EVERY_SEC(severity, fmt, ...)         ZPLOG_EVERY_SEC(severity, fmt , ##__VA_ARGS__)
#define DZPLOG_EVERY_MIN(severity, fmt, ...)         ZPLOG_EVERY_MIN(severity, fmt , ##__VA_ARGS__)
#define DZPLOG_IF_EVERY_N(severity, c, n, fmt, ...)  ZPLOG_IF_EVERY_N(severity, c, n, fmt , ##__VA_ARGS__)
#define DZPLOG_IF_FIRST_N(severity, c, n, fmt, ...)  ZPLOG_IF_FIRST_N(severity, c, n, fmt , ##__VA_ARGS__)
#define DZPLOG_IF_ONCE(severity, c, fmt, ...)        ZPLOG_IF_ONCE(severity, c, fmt , ##__VA_ARGS__)
#define DZPLOG_IF_EVERY_POW_2(severity, c, fmt, ...) ZPLOG_IF_EVERY_POW_2(severity, c, fmt , ##__VA_ARGS__)
#define DZPLOG_IF_EVERY_N_SEC(severity, c, s, fmt, ...) ZPLOG_IF_EVERY_N_SEC(severity, c, s, fmt , ##__VA_ARGS__)
#define DZPLOG_IF_EVERY_SEC(severity, c, fmt, ...)   ZPLOG_IF_EVERY_SEC(severity, c, fmt , ##__VA_ARGS__)
#define DZPLOG_IF_EVERY_MIN(severity, c, fmt, ...)   ZPLOG_IF_EVERY_MIN(severity, c, fmt , ##__VA_ARGS__)

#define DVZLOG(v, fmt, ...)                         VZLOG(v, fmt , ##__VA_ARGS__)
#define DVZLOG_EVERY_N(v, n, fmt, ...)              VZLOG_EVERY_N(v, n, fmt , ##__VA_ARGS__)
#define DVZLOG_FIRST_N(v, n, fmt, ...)              VZLOG_FIRST_N(v, n, fmt , ##__VA_ARGS__)
#define DVZLOG_ONCE(v, fmt, ...)                    VZLOG_ONCE(v, fmt , ##__VA_ARGS__)
#define DVZLOG_EVERY_POW_2(v, fmt, ...)             VZLOG_EVERY_POW_2(v, fmt , ##__VA_ARGS__)
#define DVZLOG_EVERY_N_SEC(v, s, fmt, ...)          VZLOG_EVERY_N_SEC(v, s, fmt , ##__VA_ARGS__)
#define DVZLOG_EVERY_SEC(v, fmt, ...)               VZLOG_EVERY_SEC(v, fmt , ##__VA_ARGS__)
#define DVZLOG_EVERY_MIN(v, fmt, ...)               VZLOG_EVERY_MIN(v, fmt , ##__VA_ARGS__)

#else // NDEBUG

#define DZLOG(severity, fmt, ...)                   (void)0
#define DZLOG_IF(severity, cond, fmt, ...)          (void)0
#define DZLOG_EVERY_N(severity, n, fmt, ...)        (void)0
#define DZLOG_FIRST_N(severity, n, fmt, ...)        (void)0
#define DZLOG_ONCE(severity, fmt, ...)              (void)0
#define DZLOG_EVERY_POW_2(severity, fmt, ...)       (void)0
#define DZLOG_EVERY_N_SEC(severity, s, fmt, ...)    (void)0
#define DZLOG_EVERY_SEC(severity, fmt, ...)         (void)0
#define DZLOG_EVERY_MIN(severity, fmt, ...)         (void)0
#define DZLOG_IF_EVERY_N(severity, c, n, fmt, ...)  (void)0
#define DZLOG_IF_FIRST_N(severity, c, n, fmt, ...)  (void)0
#define DZLOG_IF_ONCE(severity, c, fmt, ...)        (void)0
#define DZLOG_IF_EVERY_POW_2(severity, c, fmt, ...) (void)0
#define DZLOG_IF_EVERY_N_SEC(severity, c, s, fmt, ...) (void)0
#define DZLOG_IF_EVERY_SEC(severity, c, fmt, ...)   (void)0
#define DZLOG_IF_EVERY_MIN(severity, c, fmt, ...)   (void)0

#define DZPLOG(severity, fmt, ...)                         (void)0
#define DZPLOG_IF(severity, cond, fmt, ...)                (void)0
#define DZPLOG_EVERY_N(severity, n, fmt, ...)              (void)0
#define DZPLOG_FIRST_N(severity, n, fmt, ...)              (void)0
#define DZPLOG_ONCE(severity, fmt, ...)                    (void)0
#define DZPLOG_EVERY_POW_2(severity, fmt, ...)             (void)0
#define DZPLOG_EVERY_N_SEC(severity, s, fmt, ...)          (void)0
#define DZPLOG_EVERY_SEC(severity, fmt, ...)               (void)0
#define DZPLOG_EVERY_MIN(severity, fmt, ...)               (void)0
#define DZPLOG_IF_EVERY_N(severity, c, n, fmt, ...)        (void)0
#define DZPLOG_IF_FIRST_N(severity, c, n, fmt, ...)        (void)0
#define DZPLOG_IF_ONCE(severity, c, fmt, ...)              (void)0
#define DZPLOG_IF_EVERY_POW_2(severity, c, fmt, ...)       (void)0
#define DZPLOG_IF_EVERY_N_SEC(severity, c, s, fmt, ...)    (void)0
#define DZPLOG_IF_EVERY_SEC(severity, c, fmt, ...)         (void)0
#define DZPLOG_IF_EVERY_MIN(severity, c, fmt, ...)         (void)0

#define DVZLOG(v, fmt, ...)                         (void)0
#define DVZLOG_EVERY_N(v, n, fmt, ...)              (void)0
#define DVZLOG_FIRST_N(v, n, fmt, ...)              (void)0
#define DVZLOG_ONCE(v, fmt, ...)                    (void)0
#define DVZLOG_EVERY_POW_2(v, fmt, ...)             (void)0
#define DVZLOG_EVERY_N_SEC(v, s, fmt, ...)          (void)0
#define DVZLOG_EVERY_SEC(v, fmt, ...)               (void)0
#define DVZLOG_EVERY_MIN(v, fmt, ...)               (void)0

#endif // NDEBUG

// ============================================================================
// ZLOG_LEVEL — runtime-evaluated severity
// ============================================================================

#define ZLOG_LEVEL(severity, fmt, ...) \
    do { \
        auto _zlog_sev_ = (severity); \
        if (::xlog::log_level() <= _zlog_sev_) { \
            ::xlog::default_logger_raw()->log_printf( \
                ::xlog::source_loc{__FILE__, __LINE__, XLOG_FUNCTION}, \
                _zlog_sev_, fmt , ##__VA_ARGS__); \
            if (_zlog_sev_ == ::xlog::level::critical) ::std::abort(); \
        } \
    } while (0)

// ============================================================================
// ZCHECK — printf-style CHECK family
// ============================================================================

#define ZCHECK(condition, ...) \
    if (!(condition)) \
        ::xlog::internal::CheckFailPrintfImpl(__FILE__, __LINE__, #condition, ##__VA_ARGS__)

#define ZCHECK_ERR(invocation) \
    if ((invocation) == -1) \
        ZPLOG_BODY_FATAL("Check failed: " #invocation)

#define ZCHECK_NOTNULL(val) \
    ::xlog::internal::CheckNotNull(__FILE__, __LINE__, \
        "'" #val "' Must be non nullptr", (val))

#define ZCHECK_EQ(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_EQ, ==, val1, val2)
#define ZCHECK_NE(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_NE, !=, val1, val2)
#define ZCHECK_LE(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_LE, <=, val1, val2)
#define ZCHECK_LT(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_LT, <, val1, val2)
#define ZCHECK_GE(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_GE, >=, val1, val2)
#define ZCHECK_GT(val1, val2) XLOG_INTERNAL_CHECK_OP_FAIL(Check_GT, >, val1, val2)

#define ZCHECK_STREQ(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcmp, ==, true, s1, s2)
#define ZCHECK_STRNE(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcmp, !=, false, s1, s2)
#define ZCHECK_STRCASEEQ(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcasecmp, ==, true, s1, s2)
#define ZCHECK_STRCASENE(s1, s2) XLOG_INTERNAL_CHECK_STROP(strcasecmp, !=, false, s1, s2)

#ifndef NDEBUG

#define DZCHECK(...)                         ZCHECK(__VA_ARGS__)
#define DZCHECK_EQ(v1, v2)                   ZCHECK_EQ(v1, v2)
#define DZCHECK_NE(v1, v2)                   ZCHECK_NE(v1, v2)
#define DZCHECK_LE(v1, v2)                   ZCHECK_LE(v1, v2)
#define DZCHECK_LT(v1, v2)                   ZCHECK_LT(v1, v2)
#define DZCHECK_GE(v1, v2)                   ZCHECK_GE(v1, v2)
#define DZCHECK_GT(v1, v2)                   ZCHECK_GT(v1, v2)
#define DZCHECK_STREQ(s1, s2)                ZCHECK_STREQ(s1, s2)
#define DZCHECK_STRNE(s1, s2)                ZCHECK_STRNE(s1, s2)
#define DZCHECK_STRCASEEQ(s1, s2)            ZCHECK_STRCASEEQ(s1, s2)
#define DZCHECK_STRCASENE(s1, s2)            ZCHECK_STRCASENE(s1, s2)
#define DZCHECK_ERR(invocation)              ZCHECK_ERR(invocation)
#define DZCHECK_NOTNULL(val)                 ZCHECK_NOTNULL(val)

#else // NDEBUG

#define DZCHECK(cond, ...)                   ((void)0)
#define DZCHECK_EQ(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DZCHECK_NE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DZCHECK_LE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DZCHECK_LT(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DZCHECK_GE(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DZCHECK_GT(v1, v2)                   ((void)(v1), (void)(v2), (void)0)
#define DZCHECK_STREQ(s1, s2)                ((void)(s1), (void)(s2), (void)0)
#define DZCHECK_STRNE(s1, s2)                ((void)(s1), (void)(s2), (void)0)
#define DZCHECK_STRCASEEQ(s1, s2)            ((void)(s1), (void)(s2), (void)0)
#define DZCHECK_STRCASENE(s1, s2)            ((void)(s1), (void)(s2), (void)0)
#define DZCHECK_ERR(invocation)              ((void)(invocation), (void)0)
#define DZCHECK_NOTNULL(val)                 (val)

#endif // NDEBUG

#define ZUNREACHABLE() ZCHECK(false)
