// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <xlog/common.h>
#include <algorithm>
#include <iterator>

namespace xlog {
namespace level {

#if __cplusplus >= 201703L
constexpr
#endif
    static string_view_t level_string_views[] XLOG_LEVEL_NAMES;

static const char *short_level_names[] XLOG_SHORT_LEVEL_NAMES;

XLOG_INLINE const string_view_t &to_string_view(xlog::level::level_enum l) XLOG_NOEXCEPT {
    return level_string_views[l];
}

XLOG_INLINE const char *to_short_c_str(xlog::level::level_enum l) XLOG_NOEXCEPT {
    return short_level_names[l];
}

XLOG_INLINE xlog::level::level_enum from_str(const std::string &name) XLOG_NOEXCEPT {
    auto it = std::find(std::begin(level_string_views), std::end(level_string_views), name);
    if (it != std::end(level_string_views))
        return static_cast<level::level_enum>(std::distance(std::begin(level_string_views), it));

    // check also for "warn" and "err" before giving up..
    if (name == "warn") {
        return level::warn;
    }
    if (name == "err") {
        return level::err;
    }
    return level::off;
}
}  // namespace level

XLOG_INLINE spdlog_ex::spdlog_ex(std::string msg)
    : msg_(std::move(msg)) {}

XLOG_INLINE spdlog_ex::spdlog_ex(const std::string &msg, int last_errno) {
#ifdef XLOG_USE_STD_FORMAT
    msg_ = std::system_error(std::error_code(last_errno, std::generic_category()), msg).what();
#else
    memory_buf_t outbuf;
    fmt::format_system_error(outbuf, last_errno, msg.c_str());
    msg_ = fmt::to_string(outbuf);
#endif
}

XLOG_INLINE const char *spdlog_ex::what() const XLOG_NOEXCEPT { return msg_.c_str(); }

XLOG_INLINE void throw_spdlog_ex(const std::string &msg, int last_errno) {
    XLOG_THROW(spdlog_ex(msg, last_errno));
}

XLOG_INLINE void throw_spdlog_ex(std::string msg) { XLOG_THROW(spdlog_ex(std::move(msg))); }

}  // namespace xlog
