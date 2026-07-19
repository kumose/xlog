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

#include <xlog/log_sink_set.h>
#include <xlog/sinks/default_sink.h>
#include <xlog/utility.h>
#include <xlog/format.h>

namespace xlog {
    LogSinkSet::LogSinkSet(const std::vector<LogSink* > &log_files) : _sinks(log_files) {
    }

    const std::vector<LogSink* > &LogSinkSet::sinks() {
        return _sinks;
    }

    void LogSinkSet::do_log(xlog::LogEntry &&entry,
                      const std::vector<xlog::LogSink *> &extra_sinks, bool extra_sinks_only, bool flush) {
        xlog::LogEntry log_entry = std::move(entry);
        format_log(log_entry);
        for (LogSink *sink : extra_sinks) {
            sink->send(log_entry);
            if (flush) {
                sink->flush();
            }
        }
        if (!extra_sinks_only) {
            for (LogSink *sink : _sinks) {
                sink->send(log_entry);
                if (flush) {
                    sink->flush();
                }
            }
        }
    }

    void LogSinkSet::format_log(xlog::LogEntry &entry) {
        xlog_format(entry);
    }

    void LogSinkSet::on_fatal_error(xlog::LogEntry &entry) {
        write_to_stderr({entry.format_buffer.data(), entry.format_buffer.size()}, entry.log_severity);
        std::abort();
    }

    LogSinkRegistry::LogSinkRegistry() {
        _sink_sets[0] = create_default();
        _seq_no = 1;
        _default_sink  = _sink_sets[0].get();
    }


    std::unique_ptr<LogSinkSet> LogSinkRegistry::create_default() {
        std::vector<LogSink* > sinks;
        static DefaultSink default_sink;
        sinks.push_back(&default_sink);
        auto sink_set = std::make_unique<LogSinkSet>(sinks);
        return sink_set;
    }

    uint32_t LogSinkRegistry::add_log_sink(std::unique_ptr<LogSink> sink) {
        std::lock_guard<std::mutex> lock(_sink_set_mutex);
        const uint32_t id = _seq_no++;
        std::vector<LogSink *> sinks;
        sinks.push_back(sink.release());
        _sink_sets.emplace(id, std::make_unique<LogSinkSet>(sinks));
        return id;
    }

    uint32_t LogSinkRegistry::add_log_sinks(const std::vector<LogSink *> &sinks) {
        std::lock_guard<std::mutex> lock(_sink_set_mutex);
        const uint32_t id = _seq_no++;
        _sink_sets.emplace(id, std::make_unique<LogSinkSet>(sinks));
        return id;
    }

    bool LogSinkRegistry::set_default(uint32_t sink_id) {
        std::lock_guard<std::mutex> lock(_sink_set_mutex);
        auto it = _sink_sets.find(sink_id);
        if (it == _sink_sets.end()) {
            return false;
        }
        _default_sink = it->second.get();
        _default_id = sink_id;
        return true;
    }

    bool LogSinkRegistry::contains(uint32_t sink_id) {
        std::lock_guard<std::mutex> lock(_sink_set_mutex);
        return _sink_sets.find(sink_id) != _sink_sets.end();
    }

    LogSinkSet *LogSinkRegistry::sink_set(uint32_t sink_id) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(_sink_set_mutex));
        auto it = _sink_sets.find(sink_id);
        if (it == _sink_sets.end()) {
            return nullptr;
        }
        return it->second.get();
    }

    LogSinkSet *LogSinkRegistry::default_sink_set() const {
        return _default_sink;
    }

    uint32_t LogSinkRegistry::default_sink_id() const {
        return _default_id;
    }
} // namespace xlog
