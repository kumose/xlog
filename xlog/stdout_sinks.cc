// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef XLOG_COMPILED_LIB
#error Please define XLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>

#include <xlog/async.h>
#include <xlog/details/null_mutex.h>
#include <xlog/sinks/stdout_sinks-inl.h>

template class XLOG_API xlog::sinks::stdout_sink_base<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::stdout_sink_base<xlog::details::console_nullmutex>;
template class XLOG_API xlog::sinks::stdout_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::stdout_sink<xlog::details::console_nullmutex>;
template class XLOG_API xlog::sinks::stderr_sink<xlog::details::console_mutex>;
template class XLOG_API xlog::sinks::stderr_sink<xlog::details::console_nullmutex>;

template XLOG_API std::shared_ptr<xlog::logger>
xlog::stdout_logger_mt<xlog::synchronous_factory>(const std::string &logger_name);
template XLOG_API std::shared_ptr<xlog::logger>
xlog::stdout_logger_st<xlog::synchronous_factory>(const std::string &logger_name);
template XLOG_API std::shared_ptr<xlog::logger>
xlog::stderr_logger_mt<xlog::synchronous_factory>(const std::string &logger_name);
template XLOG_API std::shared_ptr<xlog::logger>
xlog::stderr_logger_st<xlog::synchronous_factory>(const std::string &logger_name);

template XLOG_API std::shared_ptr<xlog::logger> xlog::stdout_logger_mt<xlog::async_factory>(
    const std::string &logger_name);
template XLOG_API std::shared_ptr<xlog::logger> xlog::stdout_logger_st<xlog::async_factory>(
    const std::string &logger_name);
template XLOG_API std::shared_ptr<xlog::logger> xlog::stderr_logger_mt<xlog::async_factory>(
    const std::string &logger_name);
template XLOG_API std::shared_ptr<xlog::logger> xlog::stderr_logger_st<xlog::async_factory>(
    const std::string &logger_name);
