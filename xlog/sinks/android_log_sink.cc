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

#include <xlog/sinks/android_log_sink.h>

#if defined(__ANDROID__)

#include <android/log.h>

#include <string>

#include <xlog/log_severity.h>

namespace xlog {
namespace {

    const char *kAndroidTag = "xlog";

    int AndroidLogLevel(const LogEntry &entry) {
        switch (entry.log_severity) {
            case LogSeverity::kSeverityFatal:
                return ANDROID_LOG_FATAL;
            case LogSeverity::kSeverityError:
                return ANDROID_LOG_ERROR;
            case LogSeverity::kSeverityWarning:
                return ANDROID_LOG_WARN;
            case LogSeverity::kSeverityInfo:
                return ANDROID_LOG_INFO;
            case LogSeverity::kSeverityDebug:
                return ANDROID_LOG_DEBUG;
            case LogSeverity::kSeverityTrace:
            default:
                if (entry.verbose_level >= 2) {
                    return ANDROID_LOG_VERBOSE;
                }
                if (entry.verbose_level == 1) {
                    return ANDROID_LOG_DEBUG;
                }
                return ANDROID_LOG_VERBOSE;
        }
    }

}  // namespace

    void AndroidLogSink::send(const LogEntry &entry) {
        // format_buffer is not guaranteed NUL-terminated.
        const std::string msg(entry.format_buffer.data(),
                              entry.format_buffer.size());
        __android_log_write(AndroidLogLevel(entry), kAndroidTag, msg.c_str());
        if (entry.log_severity == LogSeverity::kSeverityFatal) {
            __android_log_write(ANDROID_LOG_FATAL, kAndroidTag, "terminating.\n");
        }
    }

}  // namespace xlog

#endif  // __ANDROID__
