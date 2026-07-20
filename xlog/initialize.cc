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

#include <xlog/initialize.h>

#include <mutex>
#include <utility>

namespace xlog {

    void initialize_log() {
        auto &cfg = LogConfig::instance();
        std::unique_lock<std::shared_mutex> lock(cfg.log_mutex);
        if (cfg.initialized) {
            return;
        }

        cfg.stderr_threshold = LogSeverity::kSeverityFatal;
        cfg.min_log_level = LogSeverity::kSeverityInfo;
        cfg.log_with_prefix = true;
        cfg.log_truncate = false;
        cfg.verbosity = 0;
        cfg.utc = false;
        cfg.initialized = true;
    }

    bool is_initialized() {
        auto &cfg = LogConfig::instance();
        std::shared_lock<std::shared_mutex> lock(cfg.log_mutex);
        return cfg.initialized;
    }

    LogSeverity stderr_threshold() {
        auto &cfg = LogConfig::instance();
        std::shared_lock<std::shared_mutex> lock(cfg.log_mutex);
        return cfg.stderr_threshold;
    }

    LogSeverity min_log_level() {
        auto &cfg = LogConfig::instance();
        std::shared_lock<std::shared_mutex> lock(cfg.log_mutex);
        return cfg.min_log_level;
    }

    int verbosity() {
        auto &cfg = LogConfig::instance();
        std::shared_lock<std::shared_mutex> lock(cfg.log_mutex);
        return cfg.verbosity;
    }

    bool utc() {
        auto &cfg = LogConfig::instance();
        std::shared_lock<std::shared_mutex> lock(cfg.log_mutex);
        return cfg.utc;
    }

    bool log_with_prefix() {
        auto &cfg = LogConfig::instance();
        std::shared_lock<std::shared_mutex> lock(cfg.log_mutex);
        return cfg.log_with_prefix;
    }

    void set_min_log_level(LogSeverity severity) {
        auto &cfg = LogConfig::instance();
        std::unique_lock<std::shared_mutex> lock(cfg.log_mutex);
        cfg.min_log_level = severity;
    }

    void set_stderr_threshold(LogSeverity severity) {
        auto &cfg = LogConfig::instance();
        std::unique_lock<std::shared_mutex> lock(cfg.log_mutex);
        cfg.stderr_threshold = severity;
    }

    void set_verbosity(int verbosity) {
        auto &cfg = LogConfig::instance();
        std::unique_lock<std::shared_mutex> lock(cfg.log_mutex);
        cfg.verbosity = verbosity;
    }

    void set_utc(bool utc) {
        auto &cfg = LogConfig::instance();
        std::unique_lock<std::shared_mutex> lock(cfg.log_mutex);
        cfg.utc = utc;
    }

    void set_log_with_prefix(bool enabled) {
        auto &cfg = LogConfig::instance();
        std::unique_lock<std::shared_mutex> lock(cfg.log_mutex);
        cfg.log_with_prefix = enabled;
    }

    void set_app_name(std::string name) {
        auto &cfg = LogConfig::instance();
        std::unique_lock<std::shared_mutex> lock(cfg.log_mutex);
        cfg.app_name = std::move(name);
    }

}  // namespace xlog
