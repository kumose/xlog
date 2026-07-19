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

#include <xlog/internal/log_message.h>
#include <xlog/internal/nullstream.h>
#include <xlog/log_severity.h>

// XLOG_LOGGING_INTERNAL_LOG_* expands to LogMessage or NullStream*.

#if defined(XLOG_STRIP_LOG) && XLOG_STRIP_LOG

#define XLOG_LOGGING_INTERNAL_LOG_TRACE ::xlog::NullStream()
#define XLOG_LOGGING_INTERNAL_LOG_DEBUG ::xlog::NullStream()
#define XLOG_LOGGING_INTERNAL_LOG_INFO ::xlog::NullStream()
#define XLOG_LOGGING_INTERNAL_LOG_WARNING ::xlog::NullStream()
#define XLOG_LOGGING_INTERNAL_LOG_ERROR ::xlog::NullStream()
#define XLOG_LOGGING_INTERNAL_LOG_FATAL ::xlog::NullStreamFatal()
#define XLOG_LOGGING_INTERNAL_LOG_LEVEL(severity) \
    ::xlog::NullStreamMaybeFatal(xlog_internal_severity)

#define XLOG_LOGGING_INTERNAL_DLOG_FATAL \
    ::xlog::NullStreamMaybeFatal(::xlog::LogSeverity::kSeverityFatal)

#define XLOG_INTERNAL_CHECK(failure_message) XLOG_LOGGING_INTERNAL_LOG_FATAL

#define XLOG_INTERNAL_ATTRIBUTE_UNUSED_IF_STRIP_LOG [[maybe_unused]]

#else

#define XLOG_INTERNAL_ATTRIBUTE_UNUSED_IF_STRIP_LOG

#define XLOG_LOGGING_INTERNAL_LOG_TRACE                                   \
    ::xlog::LogMessage(__FILE__, __LINE__,                                \
                       ::xlog::LogSeverity::kSeverityTrace)
#define XLOG_LOGGING_INTERNAL_LOG_DEBUG                                   \
    ::xlog::LogMessage(__FILE__, __LINE__,                                \
                       ::xlog::LogSeverity::kSeverityDebug)
#define XLOG_LOGGING_INTERNAL_LOG_INFO                                    \
    ::xlog::LogMessage(__FILE__, __LINE__,                                \
                       ::xlog::LogSeverity::kSeverityInfo)
#define XLOG_LOGGING_INTERNAL_LOG_WARNING                                 \
    ::xlog::LogMessage(__FILE__, __LINE__,                                \
                       ::xlog::LogSeverity::kSeverityWarning)
#define XLOG_LOGGING_INTERNAL_LOG_ERROR                                   \
    ::xlog::LogMessage(__FILE__, __LINE__,                                \
                       ::xlog::LogSeverity::kSeverityError)
#define XLOG_LOGGING_INTERNAL_LOG_FATAL                                   \
    ::xlog::LogMessage(__FILE__, __LINE__,                                \
                       ::xlog::LogSeverity::kSeverityFatal)
#define XLOG_LOGGING_INTERNAL_LOG_LEVEL(severity) \
    ::xlog::LogMessage(__FILE__, __LINE__, xlog_internal_severity)

#define XLOG_LOGGING_INTERNAL_DLOG_FATAL XLOG_LOGGING_INTERNAL_LOG_FATAL

#define XLOG_INTERNAL_CHECK(failure_message) \
    ::xlog::LogMessage(__FILE__, __LINE__, failure_message)

#endif

#define XLOG_LOGGING_INTERNAL_DLOG_TRACE XLOG_LOGGING_INTERNAL_LOG_TRACE
#define XLOG_LOGGING_INTERNAL_DLOG_DEBUG XLOG_LOGGING_INTERNAL_LOG_DEBUG
#define XLOG_LOGGING_INTERNAL_DLOG_INFO XLOG_LOGGING_INTERNAL_LOG_INFO
#define XLOG_LOGGING_INTERNAL_DLOG_WARNING XLOG_LOGGING_INTERNAL_LOG_WARNING
#define XLOG_LOGGING_INTERNAL_DLOG_ERROR XLOG_LOGGING_INTERNAL_LOG_ERROR
#define XLOG_LOGGING_INTERNAL_DLOG_LEVEL XLOG_LOGGING_INTERNAL_LOG_LEVEL
