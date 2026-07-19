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

#include <xlog/utility.h>
#include <cstdio>
#include <string>
#include <string_view>
#if defined(__EMSCRIPTEN__)
#include <emscripten/console.h>
#include <emscripten/version.h>
#endif
namespace xlog {
    namespace {
        std::string_view StripTrailingNewline(std::string_view message) {
            if (!message.empty() && message.back() == '\n') {
                message.remove_suffix(1);
            }
            return message;
        }
    } // namespace
    void write_to_stderr(std::string_view message, LogSeverity severity) {
        if (message.empty()) return;
#if defined(__EMSCRIPTEN__)
        const std::string_view msg = StripTrailingNewline(message);
#if __EMSCRIPTEN_major__ * 1000000 + __EMSCRIPTEN_minor__ * 1000 + \
        __EMSCRIPTEN_tiny__ >=                                    \
    3001043
        emscripten_errn(msg.data(), msg.size());
#else
        const std::string null_terminated(msg);
        _emscripten_err(null_terminated.c_str());
#endif
#else
        // 退出阶段 std::cerr 可能已半销毁，用 fwrite
        std::fwrite(message.data(), 1, message.size(), stderr);
#endif
#if defined(_WIN32) || defined(_WIN64) || defined(_WIN16)
        // MS CRT 会缓冲 stderr，Warning+ 必须 flush
        if (severity >= LogSeverity::kSeverityWarning) {
            std::fflush(stderr);
        }
#else
        (void) severity;
#endif
    }

    static std::string &internal_thread_identify() {
        static thread_local std::string thread_identify;
        return thread_identify;
    }

    std::string_view get_thread_identify() {
        return internal_thread_identify();
    }

    void set_thread_identify(std::string_view thread_identify) {
        internal_thread_identify() = thread_identify;
    }

} // namespace xlog
