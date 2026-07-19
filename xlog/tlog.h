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
// fmt-style logging macros (TLOG → LogMessage::print).
//
//   TLOG(INFO, "Found {} cookies", n);
//   TLOG_IF(WARNING, n > 10, "lots {}", n);
//   TLOG_EVERY_N(ERROR, 100, "again {}", COUNTER);
//   TLOG(LEVEL(sev), "dynamic {}", x);

#pragma once

#include <xlog/internal/log_impl.h>

#define TLOG(severity, ...) XLOG_INTERNAL_TLOG_IMPL(_##severity, __VA_ARGS__)
#define DTLOG(severity, ...) XLOG_INTERNAL_DTLOG_IMPL(_##severity, __VA_ARGS__)

#define TLOG_IF(severity, condition, ...) \
    XLOG_INTERNAL_TLOG_IF_IMPL(_##severity, condition, __VA_ARGS__)
#define DTLOG_IF(severity, condition, ...) \
    XLOG_INTERNAL_DTLOG_IF_IMPL(_##severity, condition, __VA_ARGS__)

#define TLOG_EVERY_N(severity, n, ...) \
    XLOG_INTERNAL_TLOG_EVERY_N_IMPL(_##severity, n, __VA_ARGS__)
#define TLOG_FIRST_N(severity, n, ...) \
    XLOG_INTERNAL_TLOG_FIRST_N_IMPL(_##severity, n, __VA_ARGS__)
#define TLOG_FIRST(...) \
    XLOG_INTERNAL_TLOG_FIRST_N_IMPL(_INFO, 1, __VA_ARGS__)
#define TLOG_EVERY_POW_2(severity, ...) \
    XLOG_INTERNAL_TLOG_EVERY_POW_2_IMPL(_##severity, __VA_ARGS__)
#define TLOG_EVERY_N_SEC(severity, n_seconds, ...) \
    XLOG_INTERNAL_TLOG_EVERY_N_SEC_IMPL(_##severity, n_seconds, __VA_ARGS__)
#define TLOG_EVERY_SECOND(severity, ...) \
    XLOG_INTERNAL_TLOG_EVERY_N_SEC_IMPL(_##severity, 1, __VA_ARGS__)
#define TLOG_EVERY_MINUTE(severity, ...) \
    XLOG_INTERNAL_TLOG_EVERY_N_SEC_IMPL(_##severity, 60, __VA_ARGS__)

#define TLOG_IF_EVERY_N(severity, condition, n, ...) \
    XLOG_INTERNAL_TLOG_IF_EVERY_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define TLOG_IF_FIRST_N(severity, condition, n, ...) \
    XLOG_INTERNAL_TLOG_IF_FIRST_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define TLOG_IF_FIRST(severity, condition, ...) \
    XLOG_INTERNAL_TLOG_IF_FIRST_N_IMPL(_##severity, condition, 1, __VA_ARGS__)
#define TLOG_IF_EVERY_POW_2(severity, condition, ...) \
    XLOG_INTERNAL_TLOG_IF_EVERY_POW_2_IMPL(_##severity, condition, __VA_ARGS__)
#define TLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, ...)          \
    XLOG_INTERNAL_TLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds, \
                                           __VA_ARGS__)

#define DTLOG_EVERY_N(severity, n, ...) \
    XLOG_INTERNAL_DTLOG_EVERY_N_IMPL(_##severity, n, __VA_ARGS__)
#define DTLOG_FIRST_N(severity, n, ...) \
    XLOG_INTERNAL_DTLOG_FIRST_N_IMPL(_##severity, n, __VA_ARGS__)
#define DTLOG_EVERY_POW_2(severity, ...) \
    XLOG_INTERNAL_DTLOG_EVERY_POW_2_IMPL(_##severity, __VA_ARGS__)
#define DTLOG_EVERY_N_SEC(severity, n_seconds, ...) \
    XLOG_INTERNAL_DTLOG_EVERY_N_SEC_IMPL(_##severity, n_seconds, __VA_ARGS__)

#define DTLOG_IF_EVERY_N(severity, condition, n, ...) \
    XLOG_INTERNAL_DTLOG_IF_EVERY_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define DTLOG_IF_FIRST_N(severity, condition, n, ...) \
    XLOG_INTERNAL_DTLOG_IF_FIRST_N_IMPL(_##severity, condition, n, __VA_ARGS__)
#define DTLOG_IF_EVERY_POW_2(severity, condition, ...) \
    XLOG_INTERNAL_DTLOG_IF_EVERY_POW_2_IMPL(_##severity, condition, __VA_ARGS__)
#define DTLOG_IF_EVERY_N_SEC(severity, condition, n_seconds, ...)          \
    XLOG_INTERNAL_DTLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds, \
                                            __VA_ARGS__)
