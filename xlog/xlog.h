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
// Stream-style logging macros (XLOG).
//
//   XLOG(INFO) << "Found " << n << " cookies";
//   XLOG(DFATAL) << "debug-fatal / release-error";
//   XLOG(INFO).no_prefix() << "raw";
//   XPLOG(ERROR) << "open failed";
//   XVLOG(2) << "verbose";              // INFO if verbosity >= 2
//   XVLOG_EVERY_N(2, 100) << "again " << COUNTER;
//   XLOG_IF(WARNING, n > 10) << "lots";
//   XLOG_EVERY_N(ERROR, 100) << "again " << COUNTER;
//   XLOG(LEVEL(sev)) << "dynamic severity";
//
// Also: <xlog/tlog.h> (fmt), <xlog/zlog.h> (printf).

#pragma once

#include <xlog/internal/log_impl.h>

// XLOG(severity) — TRACE/DEBUG/INFO/WARNING/ERROR/FATAL/DFATAL or LEVEL(expr).
#define XLOG(severity) XLOG_INTERNAL_LOG_IMPL(_##severity)

// DXLOG — same as XLOG in debug builds; compiles away under NDEBUG.
#define DXLOG(severity) XLOG_INTERNAL_DLOG_IMPL(_##severity)

// XPLOG — XLOG + ": strerror [errno]" (CRT errno at LogMessage ctor;
// not Win32 GetLastError).
#define XPLOG(severity) XLOG_INTERNAL_PLOG_IMPL(_##severity)
#define XPLOG_IF(severity, condition) \
    XLOG_INTERNAL_PLOG_IF_IMPL(_##severity, condition)

// XVLOG(n) — severity INFO when XVLOG_IS_ON(n); attaches verbosity metadata.
// Gate: n <= xlog::verbosity() (and optional XLOG_MAX_VLOG_VERBOSITY).
// No XVLOG_IF (argument evaluation order). No vmodule.
#define XVLOG(verbose_level) XLOG_INTERNAL_VLOG_IMPL(verbose_level)
#define DXVLOG(verbose_level) XLOG_INTERNAL_DVLOG_IMPL(verbose_level)

#define XVLOG_EVERY_N(verbose_level, n) \
    XLOG_INTERNAL_VLOG_EVERY_N_IMPL(verbose_level, n)
#define XVLOG_FIRST_N(verbose_level, n) \
    XLOG_INTERNAL_VLOG_FIRST_N_IMPL(verbose_level, n)
#define XVLOG_ONCE(verbose_level) XLOG_INTERNAL_VLOG_FIRST_N_IMPL(verbose_level, 1)
#define XVLOG_EVERY_POW_2(verbose_level) \
    XLOG_INTERNAL_VLOG_EVERY_POW_2_IMPL(verbose_level)
#define XVLOG_EVERY_N_SEC(verbose_level, n_seconds) \
    XLOG_INTERNAL_VLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds)

#define DXVLOG_EVERY_N(verbose_level, n) \
    XLOG_INTERNAL_DVLOG_EVERY_N_IMPL(verbose_level, n)
#define DXVLOG_FIRST_N(verbose_level, n) \
    XLOG_INTERNAL_DVLOG_FIRST_N_IMPL(verbose_level, n)
#define DXVLOG_ONCE(verbose_level) \
    XLOG_INTERNAL_DVLOG_FIRST_N_IMPL(verbose_level, 1)
#define DXVLOG_EVERY_POW_2(verbose_level) \
    XLOG_INTERNAL_DVLOG_EVERY_POW_2_IMPL(verbose_level)
#define DXVLOG_EVERY_N_SEC(verbose_level, n_seconds) \
    XLOG_INTERNAL_DVLOG_EVERY_N_SEC_IMPL(verbose_level, n_seconds)

#define XLOG_IF(severity, condition) \
    XLOG_INTERNAL_LOG_IF_IMPL(_##severity, condition)
#define DXLOG_IF(severity, condition) \
    XLOG_INTERNAL_DLOG_IF_IMPL(_##severity, condition)

#define XLOG_EVERY_N(severity, n) \
    XLOG_INTERNAL_LOG_EVERY_N_IMPL(_##severity, n)
#define XLOG_FIRST_N(severity, n) \
    XLOG_INTERNAL_LOG_FIRST_N_IMPL(_##severity, n)
#define XLOG_FIRST \
    XLOG_INTERNAL_LOG_FIRST_N_IMPL(_INFO, 1)
#define XLOG_EVERY_POW_2(severity) \
    XLOG_INTERNAL_LOG_EVERY_POW_2_IMPL(_##severity)
#define XLOG_EVERY_N_SEC(severity, n_seconds) \
    XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(_##severity, n_seconds)
#define XLOG_EVERY_SECOND(severity) \
    XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(_##severity, 1)
#define XLOG_EVERY_MINUTE(severity) \
    XLOG_INTERNAL_LOG_EVERY_N_SEC_IMPL(_##severity, 60)

#define XLOG_IF_EVERY_N(severity, condition, n) \
    XLOG_INTERNAL_LOG_IF_EVERY_N_IMPL(_##severity, condition, n)
#define XLOG_IF_FIRST_N(severity, condition, n) \
    XLOG_INTERNAL_LOG_IF_FIRST_N_IMPL(_##severity, condition, n)
#define XLOG_IF_FIRST(severity, condition) \
    XLOG_INTERNAL_LOG_IF_FIRST_N_IMPL(_##severity, condition, 1)
#define XLOG_IF_EVERY_POW_2(severity, condition) \
    XLOG_INTERNAL_LOG_IF_EVERY_POW_2_IMPL(_##severity, condition)
#define XLOG_IF_EVERY_N_SEC(severity, condition, n_seconds) \
    XLOG_INTERNAL_LOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds)

#define DXLOG_EVERY_N(severity, n) \
    XLOG_INTERNAL_DLOG_EVERY_N_IMPL(_##severity, n)
#define DXLOG_FIRST_N(severity, n) \
    XLOG_INTERNAL_DLOG_FIRST_N_IMPL(_##severity, n)
#define DXLOG_EVERY_POW_2(severity) \
    XLOG_INTERNAL_DLOG_EVERY_POW_2_IMPL(_##severity)
#define DXLOG_EVERY_N_SEC(severity, n_seconds) \
    XLOG_INTERNAL_DLOG_EVERY_N_SEC_IMPL(_##severity, n_seconds)

#define DXLOG_IF_EVERY_N(severity, condition, n) \
    XLOG_INTERNAL_DLOG_IF_EVERY_N_IMPL(_##severity, condition, n)
#define DXLOG_IF_FIRST_N(severity, condition, n) \
    XLOG_INTERNAL_DLOG_IF_FIRST_N_IMPL(_##severity, condition, n)
#define DXLOG_IF_EVERY_POW_2(severity, condition) \
    XLOG_INTERNAL_DLOG_IF_EVERY_POW_2_IMPL(_##severity, condition)
#define DXLOG_IF_EVERY_N_SEC(severity, condition, n_seconds) \
    XLOG_INTERNAL_DLOG_IF_EVERY_N_SEC_IMPL(_##severity, condition, n_seconds)

// Dynamic severity: XLOG(LEVEL(sev_expr)) << "...";
// (LEVEL is only a token for pasting; do not #define LEVEL.)
