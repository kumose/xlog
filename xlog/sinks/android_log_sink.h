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
// Android system log (logcat). Only available when __ANDROID__ is defined.

#pragma once

#if defined(__ANDROID__)

#include <xlog/log_sink.h>

namespace xlog {

    // Forwards to __android_log_write. No per-sink mutex (set serializes).
    class AndroidLogSink final : public LogSink {
    public:
        void send(const LogEntry &entry) override;
    };

}  // namespace xlog

#endif  // __ANDROID__
