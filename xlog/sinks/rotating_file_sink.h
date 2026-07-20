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

#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include <xlog/log_sink.h>
#include <xlog/sinks/log_filename.h>

namespace xlog {
namespace log_internal {
    class AppendFile;
}  // namespace log_internal

    // Size-based rotation: active file is `base_filename`;
    // rotated files are `basename_0001.ext` ... `basename_NNNN.ext`.
    class RotatingFileSink : public LogSink {
    public:
        RotatingFileSink(std::string_view base_filename, size_t max_size_bytes,
                         size_t max_files = 100, int check_interval_s = 60);

        ~RotatingFileSink() override;

        void send(const LogEntry &entry) override;
        void flush() override;

    private:
        void maybe_rotate(std::chrono::system_clock::time_point now);

        std::string _active_path;
        BaseFilename _base;
        size_t _max_size;
        size_t _max_files;
        int _check_interval_s;
        std::chrono::system_clock::time_point _next_check{};
        std::unique_ptr<log_internal::AppendFile> _file;
        std::mutex _mutex;
    };

}  // namespace xlog
