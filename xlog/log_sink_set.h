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

#include <memory>
#include <mutex>
#include <vector>
#include <xlog/log_sink.h>
#include <unordered_map>

namespace xlog {
    // One sink set. Hot path:
    //   1) format_log on the logging thread (no set lock)
    //   2) lock `_mutex`, then dispatch_locked (sink send/flush + stderr)
    //
    // The set mutex serializes I/O across threads so built-in sinks need no
    // per-sink mutex. Re-entrant XLOG from inside send() skips the lock and
    // writes stderr only (absl/turbo-style).
    class LogSinkSet {
    public:
        explicit LogSinkSet(const std::vector<LogSink *> &log_files);

        virtual ~LogSinkSet() = default;

        const std::vector<LogSink *> &sinks();

        virtual void do_log(xlog::LogEntry &&entry,
                            const std::vector<xlog::LogSink *> &extra_sinks,
                            bool extra_sinks_only, bool flush);

    protected:
        // Runs unlocked on the logging thread. Override for custom layout.
        virtual void format_log(xlog::LogEntry &entry);

        // Runs with `_mutex` held. `format_buffer` is already filled.
        // Default: fan-out to extras + set sinks, then stderr_threshold.
        // Override to skip I/O (e.g. FormatOnlySinkSet) or change dispatch.
        virtual void dispatch_locked(LogEntry &entry,
                                     const std::vector<LogSink *> &extra_sinks,
                                     bool extra_sinks_only, bool flush);

        std::mutex _mutex;

    private:
        std::vector<LogSink *> _sinks;
    };

    /// LogSinkSet no delete or remove
    class LogSinkRegistry {
    public:
        LogSinkRegistry(const LogSinkRegistry &) = delete;

        LogSinkRegistry &operator=(const LogSinkRegistry &) = delete;

        LogSinkRegistry(LogSinkRegistry &&) = delete;

        LogSinkRegistry &operator=(LogSinkRegistry &&) = delete;

        static LogSinkRegistry &instance() {
            static LogSinkRegistry s_log_sink_registry;
            return s_log_sink_registry;
        }

        // Owns `sink` for the process lifetime (no remove).
        uint32_t add_log_sink(std::unique_ptr<LogSink> sink);

        // Owns all sinks; one set containing them (no remove).
        uint32_t add_log_sinks(std::vector<std::unique_ptr<LogSink>> sinks);

        // Non-owning set (for tests / externally managed sinks).
        uint32_t add_log_sinks(const std::vector<LogSink *> &sinks);

        // Owns a custom LogSinkSet subclass (e.g. FormatOnlySinkSet).
        uint32_t add_log_sink_set(std::unique_ptr<LogSinkSet> sink_set);

        bool set_default(uint32_t sink_id);

        bool contains(uint32_t sink_id);

        LogSinkSet *sink_set(uint32_t sink_id) const;

        LogSinkSet *default_sink_set() const;

        uint32_t default_sink_id() const;

    private:
        LogSinkRegistry();

        static std::unique_ptr<LogSinkSet> create_default();

    private:
        LogSinkSet *_default_sink{nullptr};
        uint32_t _default_id{0};
        std::mutex _sink_set_mutex;
        std::unordered_map<uint32_t, std::unique_ptr<LogSinkSet> > _sink_sets;
        std::vector<std::unique_ptr<LogSink>> _owned_sinks;
        uint32_t _seq_no{0};
    };

    inline uint32_t add_log_sink(std::unique_ptr<LogSink> sink) {
        return LogSinkRegistry::instance().add_log_sink(std::move(sink));
    }

    inline uint32_t add_log_sinks(std::vector<std::unique_ptr<LogSink>> sinks) {
        return LogSinkRegistry::instance().add_log_sinks(std::move(sinks));
    }

    inline uint32_t add_log_sinks(const std::vector<LogSink *> &sinks) {
        return LogSinkRegistry::instance().add_log_sinks(sinks);
    }

    inline uint32_t add_log_sink_set(std::unique_ptr<LogSinkSet> sink_set) {
        return LogSinkRegistry::instance().add_log_sink_set(std::move(sink_set));
    }

    inline bool set_default_sink(uint32_t sink_id) {
        return LogSinkRegistry::instance().set_default(sink_id);
    }

    // This function may log to two sets of sinks:
    //
    // * If `extra_sinks_only` is true, it will dispatch only to `extra_sinks`.
    //   `LogMessage::to_sink_also` and `LogMessage::to_sink_only` are used to attach
    //    extra sinks to the entry.
    // * Otherwise it will also log to the global sinks set. This set is managed
    //   by `xlog::add_log_sink` and `xlog::remove_log_sink`.
    inline void log_to_sinks(xlog::LogEntry &&entry,
                             const std::vector<xlog::LogSink *> &extra_sinks, bool extra_sinks_only, bool flush = false) {
        LogSinkRegistry::instance().default_sink_set()->do_log(std::move(entry), extra_sinks, extra_sinks_only, flush);
    }
} // namespace xlog
