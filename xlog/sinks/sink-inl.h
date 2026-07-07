// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <xlog/sinks/sink.h>

#include <xlog/common.h>

XLOG_INLINE bool xlog::sinks::sink::should_log(xlog::level::level_enum msg_level) const {
    return msg_level >= level_.load(std::memory_order_relaxed);
}

XLOG_INLINE void xlog::sinks::sink::set_level(level::level_enum log_level) {
    level_.store(log_level, std::memory_order_relaxed);
}

XLOG_INLINE xlog::level::level_enum xlog::sinks::sink::level() const {
    return static_cast<xlog::level::level_enum>(level_.load(std::memory_order_relaxed));
}
