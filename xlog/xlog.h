// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// spdlog main header file.
// see example.cpp for usage example

#ifndef XLOG_H
#define XLOG_H

#pragma once

#include <xlog/common.h>
#include <xlog/details/registry.h>
#include <xlog/details/synchronous_factory.h>
#include <xlog/logger.h>
#include <xlog/version.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace xlog {

using default_factory = synchronous_factory;

// Create and register a logger with a templated sink type
// The logger's level, formatter and flush level will be set according to the
// global settings.
//
// Example:
//   xlog::create<daily_file_sink_st>("logger_name", "dailylog_filename", 11, 59);
template <typename Sink, typename... SinkArgs>
inline std::shared_ptr<xlog::logger> create(std::string logger_name, SinkArgs &&...sink_args) {
    return default_factory::create<Sink>(std::move(logger_name),
                                         std::forward<SinkArgs>(sink_args)...);
}

// Initialize and register a logger,
// formatter and flush level will be set according the global settings.
//
// Useful for initializing manually created loggers with the global settings.
//
// Example:
//   auto mylogger = std::make_shared<xlog::logger>("mylogger", ...);
//   xlog::initialize_logger(mylogger);
XLOG_API void initialize_logger(std::shared_ptr<logger> logger);

// Return an existing logger or nullptr if a logger with such a name doesn't
// exist.
// example: xlog::get("my_logger")->info("hello {}", "world");
XLOG_API std::shared_ptr<logger> get(const std::string &name);

// Set global formatter. Each sink in each logger will get a clone of this object
XLOG_API void set_formatter(std::unique_ptr<xlog::formatter> formatter);

// Set global format string.
// example: xlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
XLOG_API void set_pattern(std::string pattern,
                            pattern_time_type time_type = pattern_time_type::local);

// enable global backtrace support
XLOG_API void enable_backtrace(size_t n_messages);

// disable global backtrace support
XLOG_API void disable_backtrace();

// call dump backtrace on default logger
XLOG_API void dump_backtrace();

// Get global logging level
XLOG_API level::level_enum get_level();

// Set the global logging level
XLOG_API void set_level(level::level_enum log_level);

// Determine whether the default logger should log messages with a certain level
XLOG_API bool should_log(level::level_enum lvl);

// Set a global flush level
XLOG_API void flush_on(level::level_enum log_level);

// Start/Restart a periodic flusher thread
// Warning: Use only if all your loggers are thread safe!
template <typename Rep, typename Period>
inline void flush_every(std::chrono::duration<Rep, Period> interval) {
    details::registry::instance().flush_every(interval);
}

// Set global error handler
XLOG_API void set_error_handler(void (*handler)(const std::string &msg));

// Register the given logger with the given name
// Will throw if a logger with the same name already exists.
XLOG_API void register_logger(std::shared_ptr<logger> logger);

// Register the given logger with the given name
// Will replace any existing logger with the same name.
XLOG_API void register_or_replace(std::shared_ptr<logger> logger);

// Apply a user-defined function on all registered loggers
// Example:
// xlog::apply_all([&](std::shared_ptr<xlog::logger> l) {l->flush();});
XLOG_API void apply_all(const std::function<void(std::shared_ptr<logger>)> &fun);

// Drop the reference to the given logger
XLOG_API void drop(const std::string &name);

// Drop all references from the registry
XLOG_API void drop_all();

// stop any running threads started by spdlog and clean registry loggers
XLOG_API void shutdown();

// Automatic registration of loggers when using xlog::create() or xlog::create_async
XLOG_API void set_automatic_registration(bool automatic_registration);

// API for using default logger (stdout_color_mt),
// e.g.: xlog::info("Message {}", 1);
//
// The default logger object can be accessed using the xlog::default_logger():
// For example, to add another sink to it:
// xlog::default_logger()->sinks().push_back(some_sink);
//
// The default logger can be replaced using xlog::set_default_logger(new_logger).
// For example, to replace it with a file logger.
//
// IMPORTANT:
// The default API is thread safe (for _mt loggers), but:
// set_default_logger() *should not* be used concurrently with the default API.
// e.g., do not call set_default_logger() from one thread while calling xlog::info() from another.

XLOG_API std::shared_ptr<xlog::logger> default_logger();

XLOG_API xlog::logger *default_logger_raw();

XLOG_API void set_default_logger(std::shared_ptr<xlog::logger> default_logger);

// Initialize logger level based on environment configs.
//
// Useful for applying XLOG_LEVEL to manually created loggers.
//
// Example:
//   auto mylogger = std::make_shared<xlog::logger>("mylogger", ...);
//   xlog::apply_logger_env_levels(mylogger);
XLOG_API void apply_logger_env_levels(std::shared_ptr<logger> logger);

template <typename... Args>
inline void log(source_loc source,
                level::level_enum lvl,
                format_string_t<Args...> fmt,
                Args &&...args) {
    default_logger_raw()->log(source, lvl, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void log(level::level_enum lvl, format_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void trace(format_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(format_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(format_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(format_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(format_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->error(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void critical(format_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->critical(fmt, std::forward<Args>(args)...);
}

template <typename T>
inline void log(source_loc source, level::level_enum lvl, const T &msg) {
    default_logger_raw()->log(source, lvl, msg);
}

template <typename T>
inline void log(level::level_enum lvl, const T &msg) {
    default_logger_raw()->log(lvl, msg);
}

#ifdef XLOG_WCHAR_TO_UTF8_SUPPORT
template <typename... Args>
inline void log(source_loc source,
                level::level_enum lvl,
                wformat_string_t<Args...> fmt,
                Args &&...args) {
    default_logger_raw()->log(source, lvl, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void log(level::level_enum lvl, wformat_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void trace(wformat_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(wformat_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(wformat_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(wformat_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(wformat_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->error(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void critical(wformat_string_t<Args...> fmt, Args &&...args) {
    default_logger_raw()->critical(fmt, std::forward<Args>(args)...);
}
#endif

template <typename T>
inline void trace(const T &msg) {
    default_logger_raw()->trace(msg);
}

template <typename T>
inline void debug(const T &msg) {
    default_logger_raw()->debug(msg);
}

template <typename T>
inline void info(const T &msg) {
    default_logger_raw()->info(msg);
}

template <typename T>
inline void warn(const T &msg) {
    default_logger_raw()->warn(msg);
}

template <typename T>
inline void error(const T &msg) {
    default_logger_raw()->error(msg);
}

template <typename T>
inline void critical(const T &msg) {
    default_logger_raw()->critical(msg);
}

}  // namespace xlog

#endif  // XLOG_H
