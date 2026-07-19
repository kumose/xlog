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

#include <xlog/internal/conditions.h>
#include <xlog/internal/strip.h>

#include <xlog/initialize.h>

// XLOG()
#define XLOG_INTERNAL_LOG_IMPL(severity)             \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, true) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

// XPLOG() — XLOG + errno suffix
#define XLOG_INTERNAL_PLOG_IMPL(severity)              \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, true)  \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream() \
            .with_perror()

// DXLOG()
#ifndef NDEBUG
#define XLOG_INTERNAL_DLOG_IMPL(severity)            \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, true) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()
#else
#define XLOG_INTERNAL_DLOG_IMPL(severity)             \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, false) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()
#endif

#define XLOG_INTERNAL_LOG_IF_IMPL(severity, condition)    \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, condition) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_PLOG_IF_IMPL(severity, condition)   \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, condition) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream() \
            .with_perror()

#ifndef NDEBUG
#define XLOG_INTERNAL_DLOG_IF_IMPL(severity, condition)   \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, condition) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()
#else
#define XLOG_INTERNAL_DLOG_IF_IMPL(severity, condition)              \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, false && (condition)) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()
#endif

// XVLOG / DXVLOG — INFO gated by verbosity
#ifdef XLOG_MAX_VLOG_VERBOSITY
#define XVLOG_IS_ON(verbose_level)                                       \
    ((verbose_level) <= (XLOG_MAX_VLOG_VERBOSITY) &&                     \
     (verbose_level) <= ::xlog::verbosity())
#else
#define XVLOG_IS_ON(verbose_level) ((verbose_level) <= ::xlog::verbosity())
#endif

#define XLOG_INTERNAL_VLOG_IMPL(verbose_level)                              \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_LOG_IF_IMPL(_INFO, XVLOG_IS_ON(xlog_internal_verbose_level)) \
            .with_verbosity(static_cast<uint32_t>(xlog_internal_verbose_level))

#ifndef NDEBUG
#define XLOG_INTERNAL_DVLOG_IMPL(verbose_level)                             \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_DLOG_IF_IMPL(_INFO, XVLOG_IS_ON(xlog_internal_verbose_level)) \
            .with_verbosity(static_cast<uint32_t>(xlog_internal_verbose_level))
#else
#define XLOG_INTERNAL_DVLOG_IMPL(verbose_level)                             \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_DLOG_IF_IMPL(                                         \
            _INFO, false && XVLOG_IS_ON(xlog_internal_verbose_level))       \
            .with_verbosity(static_cast<uint32_t>(xlog_internal_verbose_level))
#endif

// XVLOG + stateful conditions (INFO severity; gated by XVLOG_IS_ON).
#define XLOG_INTERNAL_VLOG_EVERY_N_IMPL(verbose_level, n)                   \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, XVLOG_IS_ON(xlog_internal_verbose_level))             \
        (EveryN, n)                                                         \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))

#define XLOG_INTERNAL_VLOG_FIRST_N_IMPL(verbose_level, n)                   \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, XVLOG_IS_ON(xlog_internal_verbose_level))             \
        (FirstN, n)                                                         \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))

#define XLOG_INTERNAL_VLOG_EVERY_POW_2_IMPL(verbose_level)                  \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, XVLOG_IS_ON(xlog_internal_verbose_level))             \
        (EveryPow2)                                                         \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))

#define XLOG_INTERNAL_VLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds)       \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, XVLOG_IS_ON(xlog_internal_verbose_level))             \
        (EveryNSec, n_seconds)                                              \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))

#ifndef NDEBUG
#define XLOG_INTERNAL_DVLOG_EVERY_N_IMPL(verbose_level, n)                  \
    XLOG_INTERNAL_VLOG_EVERY_N_IMPL(verbose_level, n)
#define XLOG_INTERNAL_DVLOG_FIRST_N_IMPL(verbose_level, n)                  \
    XLOG_INTERNAL_VLOG_FIRST_N_IMPL(verbose_level, n)
#define XLOG_INTERNAL_DVLOG_EVERY_POW_2_IMPL(verbose_level)                 \
    XLOG_INTERNAL_VLOG_EVERY_POW_2_IMPL(verbose_level)
#define XLOG_INTERNAL_DVLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds)      \
    XLOG_INTERNAL_VLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds)
#else
#define XLOG_INTERNAL_DVLOG_EVERY_N_IMPL(verbose_level, n)                  \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, false && XVLOG_IS_ON(xlog_internal_verbose_level))    \
        (EveryN, n)                                                         \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))
#define XLOG_INTERNAL_DVLOG_FIRST_N_IMPL(verbose_level, n)                  \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, false && XVLOG_IS_ON(xlog_internal_verbose_level))    \
        (FirstN, n)                                                         \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))
#define XLOG_INTERNAL_DVLOG_EVERY_POW_2_IMPL(verbose_level)                 \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, false && XVLOG_IS_ON(xlog_internal_verbose_level))    \
        (EveryPow2)                                                         \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))
#define XLOG_INTERNAL_DVLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds)      \
    switch (const int xlog_internal_verbose_level = (verbose_level))        \
    case 0:                                                                 \
    default:                                                                \
        XLOG_INTERNAL_CONDITION_INFO(                                       \
            STATEFUL, false && XVLOG_IS_ON(xlog_internal_verbose_level))    \
        (EveryNSec, n_seconds)                                              \
            XLOG_LOGGING_INTERNAL_LOG_INFO.internal_stream()                \
                .with_verbosity(                                            \
                    static_cast<uint32_t>(xlog_internal_verbose_level))
#endif

#define XLOG_INTERNAL_LOG_EVERY_N_IMPL(severity, n)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryN, n) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_FIRST_N_IMPL(severity, n)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(FirstN, n) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_EVERY_POW_2_IMPL(severity)           \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryPow2) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(severity, n_seconds)           \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_EVERY_N_IMPL(severity, condition, n)   \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryN, n) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_FIRST_N_IMPL(severity, condition, n)   \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(FirstN, n) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_EVERY_POW_2_IMPL(severity, condition)  \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryPow2) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#define XLOG_INTERNAL_LOG_IF_EVERY_N_SEC_IMPL(severity, condition, n_seconds) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_LOG##severity.internal_stream()

#ifndef NDEBUG
#define XLOG_INTERNAL_DLOG_EVERY_N_IMPL(severity, n)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_FIRST_N_IMPL(severity, n)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(FirstN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_EVERY_POW_2_IMPL(severity)           \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryPow2) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_EVERY_N_SEC_IMPL(severity, n_seconds)           \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_EVERY_N_IMPL(severity, condition, n)   \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_FIRST_N_IMPL(severity, condition, n)   \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(FirstN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_EVERY_POW_2_IMPL(severity, condition)  \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryPow2) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_EVERY_N_SEC_IMPL(severity, condition, n_seconds) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()
#else
#define XLOG_INTERNAL_DLOG_EVERY_N_IMPL(severity, n)             \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_FIRST_N_IMPL(severity, n)             \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(FirstN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_EVERY_POW_2_IMPL(severity)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryPow2) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_EVERY_N_SEC_IMPL(severity, n_seconds)             \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_EVERY_N_IMPL(severity, condition, n)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(EveryN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_FIRST_N_IMPL(severity, condition, n)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(FirstN, n) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_EVERY_POW_2_IMPL(severity, condition)           \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(EveryPow2) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()

#define XLOG_INTERNAL_DLOG_IF_EVERY_N_SEC_IMPL(severity, condition, n_seconds) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(          \
        EveryNSec, n_seconds)                                                  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.internal_stream()
#endif

// ---------------------------------------------------------------------------
// TLOG — fmt::format style (.print)
// ---------------------------------------------------------------------------

#define XLOG_INTERNAL_TLOG_IMPL(severity, ...)           \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, true)    \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#ifndef NDEBUG
#define XLOG_INTERNAL_DTLOG_IMPL(severity, ...)          \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, true)    \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)
#else
#define XLOG_INTERNAL_DTLOG_IMPL(severity, ...)           \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, false)   \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)
#endif

#define XLOG_INTERNAL_TLOG_IF_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, condition)   \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#ifndef NDEBUG
#define XLOG_INTERNAL_DTLOG_IF_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, condition)    \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)
#else
#define XLOG_INTERNAL_DTLOG_IF_IMPL(severity, condition, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, false && (condition)) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)
#endif

#define XLOG_INTERNAL_TLOG_EVERY_N_IMPL(severity, n, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryN, n)   \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_TLOG_FIRST_N_IMPL(severity, n, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(FirstN, n)   \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_TLOG_EVERY_POW_2_IMPL(severity, ...)        \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryPow2)   \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_TLOG_EVERY_N_SEC_IMPL(severity, n_seconds, ...)       \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_TLOG_IF_EVERY_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryN, n)    \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_TLOG_IF_FIRST_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(FirstN, n)    \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_TLOG_IF_EVERY_POW_2_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryPow2)      \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_TLOG_IF_EVERY_N_SEC_IMPL(severity, condition, n_seconds, \
                                               ...)                          \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryNSec,         \
                                                         n_seconds)          \
        XLOG_LOGGING_INTERNAL_LOG##severity.print(__VA_ARGS__)

#ifndef NDEBUG
#define XLOG_INTERNAL_DTLOG_EVERY_N_IMPL(severity, n, ...)        \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryN, n)  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_FIRST_N_IMPL(severity, n, ...)        \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(FirstN, n)  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_EVERY_POW_2_IMPL(severity, ...)      \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryPow2)  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_EVERY_N_SEC_IMPL(severity, n_seconds, ...)     \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_EVERY_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryN, n)     \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_FIRST_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(FirstN, n)     \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_EVERY_POW_2_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryPow2)       \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_EVERY_N_SEC_IMPL(severity, condition,       \
                                                n_seconds, ...)            \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryNSec,       \
                                                         n_seconds)        \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)
#else
#define XLOG_INTERNAL_DTLOG_EVERY_N_IMPL(severity, n, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryN, n)  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_FIRST_N_IMPL(severity, n, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(FirstN, n)  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_EVERY_POW_2_IMPL(severity, ...)        \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryPow2)  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_EVERY_N_SEC_IMPL(severity, n_seconds, ...)      \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_EVERY_N_IMPL(severity, condition, n, ...)     \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(EveryN, \
                                                                    n)      \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_FIRST_N_IMPL(severity, condition, n, ...)     \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(FirstN, \
                                                                    n)      \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_EVERY_POW_2_IMPL(severity, condition, ...)    \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(        \
        EveryPow2)                                                          \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)

#define XLOG_INTERNAL_DTLOG_IF_EVERY_N_SEC_IMPL(severity, condition,         \
                                                n_seconds, ...)             \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(        \
        EveryNSec, n_seconds)                                               \
        XLOG_LOGGING_INTERNAL_DLOG##severity.print(__VA_ARGS__)
#endif

// ---------------------------------------------------------------------------
// ZLOG — printf style (.printf)
// ---------------------------------------------------------------------------

#define XLOG_INTERNAL_ZLOG_IMPL(severity, ...)            \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, true)     \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#ifndef NDEBUG
#define XLOG_INTERNAL_DZLOG_IMPL(severity, ...)           \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, true)     \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)
#else
#define XLOG_INTERNAL_DZLOG_IMPL(severity, ...)            \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, false)    \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)
#endif

#define XLOG_INTERNAL_ZLOG_IF_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, condition)   \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#ifndef NDEBUG
#define XLOG_INTERNAL_DZLOG_IF_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, condition)     \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)
#else
#define XLOG_INTERNAL_DZLOG_IF_IMPL(severity, condition, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATELESS, false && (condition))  \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)
#endif

#define XLOG_INTERNAL_ZLOG_EVERY_N_IMPL(severity, n, ...)          \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryN, n)    \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_ZLOG_FIRST_N_IMPL(severity, n, ...)          \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(FirstN, n)    \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_ZLOG_EVERY_POW_2_IMPL(severity, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryPow2)    \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_ZLOG_EVERY_N_SEC_IMPL(severity, n_seconds, ...)       \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_ZLOG_IF_EVERY_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryN, n)     \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_ZLOG_IF_FIRST_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(FirstN, n)     \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_ZLOG_IF_EVERY_POW_2_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryPow2)       \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_ZLOG_IF_EVERY_N_SEC_IMPL(severity, condition, n_seconds, \
                                               ...)                           \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryNSec,          \
                                                         n_seconds)           \
        XLOG_LOGGING_INTERNAL_LOG##severity.printf(__VA_ARGS__)

#ifndef NDEBUG
#define XLOG_INTERNAL_DZLOG_EVERY_N_IMPL(severity, n, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryN, n)   \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_FIRST_N_IMPL(severity, n, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(FirstN, n)   \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_EVERY_POW_2_IMPL(severity, ...)       \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryPow2)   \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_EVERY_N_SEC_IMPL(severity, n_seconds, ...)     \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, true)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_EVERY_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryN, n)      \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_FIRST_N_IMPL(severity, condition, n, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(FirstN, n)      \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_EVERY_POW_2_IMPL(severity, condition, ...) \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryPow2)        \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_EVERY_N_SEC_IMPL(severity, condition,        \
                                                n_seconds, ...)             \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, condition)(EveryNSec,        \
                                                         n_seconds)         \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)
#else
#define XLOG_INTERNAL_DZLOG_EVERY_N_IMPL(severity, n, ...)          \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryN, n)   \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_FIRST_N_IMPL(severity, n, ...)          \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(FirstN, n)   \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_EVERY_POW_2_IMPL(severity, ...)         \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryPow2)   \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_EVERY_N_SEC_IMPL(severity, n_seconds, ...)      \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false)(EveryNSec, n_seconds) \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_EVERY_N_IMPL(severity, condition, n, ...)     \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(EveryN, \
                                                                    n)       \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_FIRST_N_IMPL(severity, condition, n, ...)     \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(FirstN, \
                                                                    n)       \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_EVERY_POW_2_IMPL(severity, condition, ...)    \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(        \
        EveryPow2)                                                           \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)

#define XLOG_INTERNAL_DZLOG_IF_EVERY_N_SEC_IMPL(severity, condition,         \
                                                n_seconds, ...)              \
    XLOG_INTERNAL_CONDITION##severity(STATEFUL, false && (condition))(         \
        EveryNSec, n_seconds)                                                \
        XLOG_LOGGING_INTERNAL_DLOG##severity.printf(__VA_ARGS__)
#endif
