// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef XLOG_COMPILED_LIB
#error Please define XLOG_COMPILED_LIB to compile this file.
#endif

#include <xlog/details/file_helper-inl.h>
#include <xlog/details/null_mutex.h>
#include <xlog/sinks/base_sink-inl.h>
#include <xlog/sinks/basic_file_sink-inl.h>

#include <mutex>

template class XLOG_API xlog::sinks::basic_file_sink<std::mutex>;
template class XLOG_API xlog::sinks::basic_file_sink<xlog::details::null_mutex>;

#include <xlog/sinks/rotating_file_sink-inl.h>
template class XLOG_API xlog::sinks::rotating_file_sink<std::mutex>;
template class XLOG_API xlog::sinks::rotating_file_sink<xlog::details::null_mutex>;
