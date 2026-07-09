// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <xlog/xlog.h>
#include <xlog/common.h>
#include <xlog/pattern_formatter.h>

namespace xlog {

XLOG_INLINE void initialize_logger(std::shared_ptr<logger> logger) {
    details::registry::instance().initialize_logger(std::move(logger));
}

XLOG_INLINE std::shared_ptr<logger> get(const std::string &name) {
    return details::registry::instance().get(name);
}

XLOG_INLINE void set_formatter(std::unique_ptr<xlog::formatter> formatter) {
    details::registry::instance().set_formatter(std::move(formatter));
}

XLOG_INLINE void set_pattern(std::string pattern, pattern_time_type time_type) {
    set_formatter(
        std::unique_ptr<xlog::formatter>(new pattern_formatter(std::move(pattern), time_type)));
}

XLOG_INLINE void enable_backtrace(size_t n_messages) {
    details::registry::instance().enable_backtrace(n_messages);
}

XLOG_INLINE void disable_backtrace() { details::registry::instance().disable_backtrace(); }

XLOG_INLINE void dump_backtrace() { default_logger_raw()->dump_backtrace(); }

XLOG_INLINE level::level_enum get_level() { return default_logger_raw()->level(); }

XLOG_INLINE bool should_log(level::level_enum log_level) {
    return default_logger_raw()->should_log(log_level);
}

XLOG_INLINE void set_level(level::level_enum log_level) {
    details::registry::instance().set_level(log_level);
}

XLOG_INLINE void flush_on(level::level_enum log_level) {
    details::registry::instance().flush_on(log_level);
}

XLOG_INLINE void set_error_handler(void (*handler)(const std::string &msg)) {
    details::registry::instance().set_error_handler(handler);
}

XLOG_INLINE void register_logger(std::shared_ptr<logger> logger) {
    details::registry::instance().register_logger(std::move(logger));
}

XLOG_INLINE void register_or_replace(std::shared_ptr<logger> logger) {
    details::registry::instance().register_or_replace(std::move(logger));
}

XLOG_INLINE void apply_all(const std::function<void(std::shared_ptr<logger>)> &fun) {
    details::registry::instance().apply_all(fun);
}

XLOG_INLINE void drop(const std::string &name) { details::registry::instance().drop(name); }

XLOG_INLINE void drop_all() { details::registry::instance().drop_all(); }

XLOG_INLINE void shutdown() { details::registry::instance().shutdown(); }

XLOG_INLINE void set_automatic_registration(bool automatic_registration) {
    details::registry::instance().set_automatic_registration(automatic_registration);
}

XLOG_INLINE std::shared_ptr<xlog::logger> default_logger() {
    return details::registry::instance().default_logger();
}

XLOG_INLINE xlog::logger *default_logger_raw() {
    return details::registry::instance().get_default_raw();
}

XLOG_INLINE void set_default_logger(std::shared_ptr<xlog::logger> default_logger) {
    details::registry::instance().set_default_logger(std::move(default_logger));
}

XLOG_INLINE void apply_logger_env_levels(std::shared_ptr<logger> logger) {
    details::registry::instance().apply_logger_env_levels(std::move(logger));
}

}  // namespace xlog
