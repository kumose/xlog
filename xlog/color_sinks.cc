// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef XLOG_COMPILED_LIB
#error Please define XLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>

#include <xlog/async.h>
#include <xlog/details/null_mutex.h>
//
// color sinks
//
#ifdef _WIN32
#include <xlog/sinks/wincolor_sink-inl.h>
template class XLOG_API xlog::sinks::wincolor_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::wincolor_sink<xlog::details::console_nullmutex>;
template class XLOG_API xlog::sinks::wincolor_stdout_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::wincolor_stdout_sink<xlog::details::console_nullmutex>;
template class XLOG_API xlog::sinks::wincolor_stderr_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::wincolor_stderr_sink<xlog::details::console_nullmutex>;
#else
#include <xlog/sinks/ansicolor_sink-inl.h>
template class XLOG_API xlog::sinks::ansicolor_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::ansicolor_sink<xlog::details::console_nullmutex>;
template class XLOG_API xlog::sinks::ansicolor_stdout_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::ansicolor_stdout_sink<xlog::details::console_nullmutex>;
template class XLOG_API xlog::sinks::ansicolor_stderr_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::ansicolor_stderr_sink<xlog::details::console_nullmutex>;
#endif

// factory methods for color loggers
#include <xlog/sinks/stdout_color_sinks-inl.h>
template XLOG_API std::shared_ptr<xlog::logger>
xlog::stdout_color_mt<xlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);
template XLOG_API std::shared_ptr<xlog::logger>
xlog::stdout_color_st<xlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);
template XLOG_API std::shared_ptr<xlog::logger>
xlog::stderr_color_mt<xlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);
template XLOG_API std::shared_ptr<xlog::logger>
xlog::stderr_color_st<xlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);

template XLOG_API std::shared_ptr<xlog::logger> xlog::stdout_color_mt<xlog::async_factory>(
    const std::string &logger_name, color_mode mode);
template XLOG_API std::shared_ptr<xlog::logger> xlog::stdout_color_st<xlog::async_factory>(
    const std::string &logger_name, color_mode mode);
template XLOG_API std::shared_ptr<xlog::logger> xlog::stderr_color_mt<xlog::async_factory>(
    const std::string &logger_name, color_mode mode);
template XLOG_API std::shared_ptr<xlog::logger> xlog::stderr_color_st<xlog::async_factory>(
    const std::string &logger_name, color_mode mode);
