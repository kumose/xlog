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
// printf-style logging macros (ZLOG → LogMessage::printf / fmt::sprintf).
//
//   ZLOG(INFO, "Found %d cookies", n);
//   ZVLOG(2, "verbose %d", n);   // INFO if verbosity >= 2
//   ZLOG_IF(WARNING, n > 10, "lots %d", n);
//   ZLOG_EVERY_N(ERROR, 100, "again %u", COUNTER);
//   ZLOG(LEVEL(sev), "dynamic %d", x);

#pragma once

#include <xlog/internal/log_impl.h>

#define ZLOG(severity, ...) XLOG_INTERNAL_ZLOG_IMPL(_##severity, __VA_ARGS__)
#define DZLOG(severity, ...) XLOG_INTERNAL_DZLOG_IMPL(_##severity, __VA_ARGS__)

// ZVLOG(n, ...) — printf verbose INFO (same gates as XVLOG).
#define ZVLOG(verbose_level, ...) \
    XLOG_INTERNAL_ZVLOG_IMPL(verbose_level, __VA_ARGS__)
#define DZVLOG(verbose_level, ...) \
    XLOG_INTERNAL_DZVLOG_IMPL(verbose_level, __VA_ARGS__)

#define ZVLOG_EVERY_N(verbose_level, n, ...) \
    XLOG_INTERNAL_ZVLOG_EVERY_N_IMPL(verbose_level, n, __VA_ARGS__)
#define ZVLOG_FIRST_N(verbose_level, n, ...) \
    XLOG_INTERNAL_ZVLOG_FIRST_N_IMPL(verbose_level, n, __VA_ARGS__)
#define ZVLOG_ONCE(verbose_level, ...) \
    XLOG_INTERNAL_ZVLOG_FIRST_N_IMPL(verbose_level, 1, __VA_ARGS__)
#define ZVLOG_EVERY_POW_2(verbose_level, ...) \
    XLOG_INTERNAL_ZVLOG_EVERY_POW_2_IMPL(verbose_level, __VA_ARGS__)
#define ZVLOG_EVERY_N_SEC(verbose_level, n_seconds, ...) \
    XLOG_INTERNAL_ZVLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds, __VA_ARGS__)

#define DZVLOG_EVERY_N(verbose_level, n, ...) \
    XLOG_INTERNAL_DZVLOG_EVERY_N_IMPL(verbose_level, n, __VA_ARGS__)
#define DZVLOG_FIRST_N(verbose_level, n, ...) \
    XLOG_INTERNAL_DZVLOG_FIRST_N_IMPL(verbose_level, n, __VA_ARGS__)
#define DZVLOG_ONCE(verbose_level, ...) \
    XLOG_INTERNAL_DZVLOG_FIRST_N_IMPL(verbose_level, 1, __VA_ARGS__)
#define DZVLOG_EVERY_POW_2(verbose_level, ...) \
    XLOG_INTERNAL_DZVLOG_EVERY_POW_2_IMPL(verbose_level, __VA_ARGS__)
#define DZVLOG_EVERY_N_SEC(verbose_level, n_seconds, ...) \
    XLOG_INTERNAL_DZVLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds, \
                                          __VA_ARGS__)

#define ZLOG_IF(severity, condition, ...) \
    XLOG_INTERNAL_ZLOG_IF_IMPL(_##severity, condition, __VA_ARGS__)
#define DZLOG_IF(severity, condition, ...) \
    XLOG_INTERNAL_DZLOG_IF_IMPL(_##severity, condition, __VA_ARGS__)

#define ZLOG_EVERY_N(severity, n, ...) \
    XLOG_INTERNAL_ZLOG_EVERY_N_IMPL(_##severity, n, __VA_ARGS__)
#define ZLOG_FIRST_N(severity, n, ...) \
    XLOG_INTERNAL_ZLOG_FIRST_N_IMPL(_##severity, n, __VA_ARGS__)
#define ZLOG_FIRST(...) \
    XLOG_INTERNAL_ZLOG_FIRST_N_IMPL(_INFO, 1, __VA_ARGS__)
#define ZLOG_EVERY_POW_2(severity, ...) \
    XLOG_INTERNAL_ZLOG_EVERY_POW_2_IMPL(_##severity, __VA_ARGS__)
#define ZLOG_EVERY_N_SEC(severity, n_seconds, ...) \
    XLOG_INTERNAL_ZLOG_EVERY_N_SEC_IMPL(_##severity, n_seconds, __VA_ARGS__)
#define ZLOG_EVERY_SECOND(severity, ...) \
    XLOG_INTERNAL_ZLOG_EVERY_N_SEC_IMPL(_##severity, 1, __VA_ARGS__)
#define ZLOG_EVERY_MINUTE(severity, ...) \
    XLOG_INTERNAL_ZLOG_EVERY_N_SEC_IMPL(_##severity, 60, __VA_ARGS__)

#define ZLOG_IF_EVERY_N(severity, condition, n, ...) \
    XLOG_INTERNAL_ZLOG_IF_EVERY_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define ZLOG_IF_FIRST_N(severity, condition, n, ...) \
    XLOG_INTERNAL_ZLOG_IF_FIRST_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define ZLOG_IF_FIRST(severity, condition, ...) \
    XLOG_INTERNAL_ZLOG_IF_FIRST_N_IMPL(_##severity, condition, 1, __VA_ARGS__)
#define ZLOG_IF_EVERY_POW_2(severity, condition, ...) \
    XLOG_INTERNAL_ZLOG_IF_EVERY_POW_2_IMPL(_##severity, condition, __VA_ARGS__)
#define ZLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, ...)          \
    XLOG_INTERNAL_ZLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds, \
                                           __VA_ARGS__)

#define DZLOG_EVERY_N(severity, n, ...) \
    XLOG_INTERNAL_DZLOG_EVERY_N_IMPL(_##severity, n, __VA_ARGS__)
#define DZLOG_FIRST_N(severity, n, ...) \
    XLOG_INTERNAL_DZLOG_FIRST_N_IMPL(_##severity, n, __VA_ARGS__)
#define DZLOG_EVERY_POW_2(severity, ...) \
    XLOG_INTERNAL_DZLOG_EVERY_POW_2_IMPL(_##severity, __VA_ARGS__)
#define DZLOG_EVERY_N_SEC(severity, n_seconds, ...) \
    XLOG_INTERNAL_DZLOG_EVERY_N_SEC_IMPL(_##severity, n_seconds, __VA_ARGS__)

#define DZLOG_IF_EVERY_N(severity, condition, n, ...) \
    XLOG_INTERNAL_DZLOG_IF_EVERY_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define DZLOG_IF_FIRST_N(severity, condition, n, ...) \
    XLOG_INTERNAL_DZLOG_IF_FIRST_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define DZLOG_IF_EVERY_POW_2(severity, condition, ...) \
    XLOG_INTERNAL_DZLOG_IF_EVERY_POW_2_IMPL(_##severity, condition, __VA_ARGS__)
#define DZLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, ...)          \
    XLOG_INTERNAL_DZLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds, \
                                            __VA_ARGS__)
