// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <xlog/details/log_msg.h>
#include <xlog/details/os.h>

namespace xlog {
namespace details {

XLOG_INLINE log_msg::log_msg(xlog::log_clock::time_point log_time,
                               xlog::source_loc loc,
                               string_view_t a_logger_name,
                               xlog::level::level_enum lvl,
                               xlog::string_view_t msg)
    : logger_name(a_logger_name),
      level(lvl),
      time(log_time)
#ifndef XLOG_NO_THREAD_ID
      ,
      thread_id(os::thread_id())
#endif
      ,
      source(loc),
      payload(msg) {
}

XLOG_INLINE log_msg::log_msg(xlog::source_loc loc,
                               string_view_t a_logger_name,
                               xlog::level::level_enum lvl,
                               xlog::string_view_t msg)
    : log_msg(os::now(), loc, a_logger_name, lvl, msg) {}

XLOG_INLINE log_msg::log_msg(string_view_t a_logger_name,
                               xlog::level::level_enum lvl,
                               xlog::string_view_t msg)
    : log_msg(os::now(), source_loc{}, a_logger_name, lvl, msg) {}

}  // namespace details
}  // namespace xlog
