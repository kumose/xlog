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

#include <xlog/sinks/daily_file_sink.h>
#include <xlog/internal/append_file.h>
#include <xlog/internal/fs_helper.h>

namespace xlog {

    DailyFileSink::DailyFileSink(std::string_view base_filename,
                                 uint16_t max_files, int check_interval_s,
                                 bool truncate, bool utc)
        : _base(base_filename),
          _truncate(truncate),
          _utc(utc),
          _max_files(max_files),
          _check_interval_s(check_interval_s),
          _next_check(clock::now() + std::chrono::seconds(check_interval_s)),
          _file(std::make_unique<log_internal::AppendFile>()) {
        rotate_file(clock::now());
    }

    DailyFileSink::~DailyFileSink() {
        if (_file) {
            _file->close();
        }
    }

    void DailyFileSink::send(const LogEntry &entry) {
        // Called under LogSinkSet mutex — no sink-level lock.
        rotate_file(entry.timestamp);
        if (!_file) {
            return;
        }
        const std::string_view data(entry.format_buffer.data(),
                                    entry.format_buffer.size());
        _file->write(data);
    }

    void DailyFileSink::flush() {
        if (_file) {
            _file->flush();
        }
    }

    void DailyFileSink::rotate_file(clock::time_point stamp) {
        if (stamp >= _next_check) {
            _next_check = stamp + std::chrono::seconds(_check_interval_s);
            if (_file) {
                _file->reopen();
            }
        }
        const std::string path = daily_log_path(_base, stamp, _utc);
        if (_file && _file->file_path() == path) {
            return;
        }
        if (_truncate) {
            log_internal::remove_path(path);
        }
        if (_file) {
            _file->close();
        }
        _file = std::make_unique<log_internal::AppendFile>();
        _file->initialize(path);

        if (_max_files == 0) {
            return;
        }
        if (_files.size() >= _max_files) {
            log_internal::remove_path(_files.front());
            _files.pop_front();
        }
        _files.push_back(path);
    }

}  // namespace xlog
