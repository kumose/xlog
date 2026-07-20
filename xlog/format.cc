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

#include <xlog/format.h>
#include <xlog/initialize.h>
#include <xlog/log_severity.h>

#include <chrono>
#include <ctime>
#include <string_view>

namespace xlog {
namespace {

// Per-second RFC3339 cache (to seconds + zone). Microseconds are not cached.
struct TimeStringCache {
    bool valid{false};
    std::chrono::seconds last_sec{};
    bool last_utc{false};
    char datetime[19]{};  // YYYY-MM-DDTHH:MM:SS
    char zone[6]{};       // Z or ±HH:MM
    size_t zone_len{0};
};

TimeStringCache &TlsTimeCache() {
    thread_local TimeStringCache cache;
    return cache;
}

void FillTimeCache(TimeStringCache &cache,
                   std::chrono::system_clock::time_point tp, bool utc) {
    const CivilDay civil = to_civil_day(tp, utc);
    fmt::format_to_n(cache.datetime, sizeof(cache.datetime),
                     "{:04}-{:02}-{:02}T{:02}:{:02}:{:02}", civil.year,
                     civil.month, civil.day, civil.hour, civil.minute,
                     civil.second);
    if (civil.utc) {
        cache.zone[0] = 'Z';
        cache.zone_len = 1;
    } else {
        int off = civil.offset_seconds;
        const char sign = off >= 0 ? '+' : '-';
        if (off < 0) {
            off = -off;
        }
        const auto r = fmt::format_to_n(cache.zone, sizeof(cache.zone),
                                        "{}{:02}:{:02}", sign, off / 3600,
                                        (off % 3600) / 60);
        cache.zone_len = r.size;
    }
    cache.last_sec =
        std::chrono::floor<std::chrono::seconds>(tp).time_since_epoch();
    cache.last_utc = utc;
    cache.valid = true;
}

std::tm ToTm(std::time_t tt, bool utc) {
    std::tm tm{};
#if defined(_WIN32)
    if (utc) {
        gmtime_s(&tm, &tt);
    } else {
        localtime_s(&tm, &tt);
    }
#else
    if (utc) {
        gmtime_r(&tt, &tm);
    } else {
        localtime_r(&tt, &tm);
    }
#endif
    return tm;
}

// Offset east of UTC from an already-filled local tm + original time_t.
// Does not call localtime again.
int OffsetFromLocalTm(std::tm local_tm, std::time_t tt) {
#if defined(_WIN32)
    const std::time_t as_utc = _mkgmtime(&local_tm);
    if (as_utc == static_cast<std::time_t>(-1)) {
        return 0;
    }
    return static_cast<int>(as_utc - tt);
#elif defined(__tm_gmtoff) || defined(__APPLE__) || defined(__FreeBSD__) ||     \
    defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__) ||       \
    defined(__ANDROID__) || defined(__EMSCRIPTEN__) || defined(__CYGWIN__)
    (void)tt;
    return static_cast<int>(local_tm.tm_gmtoff);
#else
    return static_cast<int>(timegm(&local_tm) - tt);
#endif
}

std::string_view BaseName(std::string_view path) {
    const auto pos = path.find_last_of("/\\");
    if (pos != std::string_view::npos) {
        path.remove_prefix(pos + 1);
    }
    return path;
}

}  // namespace

CivilDay to_civil_day(std::chrono::system_clock::time_point tp, bool utc) {
    using clock = std::chrono::system_clock;

    const auto secs = std::chrono::floor<std::chrono::seconds>(tp);
    const auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(tp - secs);

    const std::time_t tt = clock::to_time_t(tp);
    std::tm tm = ToTm(tt, utc);  // one localtime / gmtime

    CivilDay out{};
    out.year = static_cast<uint16_t>(tm.tm_year + 1900);
    out.month = static_cast<uint16_t>(tm.tm_mon + 1);
    out.day = static_cast<uint8_t>(tm.tm_mday);
    out.hour = static_cast<uint8_t>(tm.tm_hour);
    out.minute = static_cast<uint8_t>(tm.tm_min);
    out.second = static_cast<uint8_t>(tm.tm_sec);
    out.nanosecond = static_cast<uint64_t>(ns.count() < 0 ? 0 : ns.count());
    out.utc = utc;
    out.offset_seconds = utc ? 0 : OffsetFromLocalTm(tm, tt);
    return out;
}

// Formats entry into format_buffer.
//
// With prefix (log_with_prefix == true):
//   I2026-07-20T03:17:05.123456+08:00 13982(main) foo.cc:42] hello world
//   W2026-07-20T03:17:05.123456+08:00 13982 bar.cc:108] disk almost full
//   I2026-07-20T03:17:05.123456Z 13982 foo.cc:42] hello world   // utc
//
// Template:
//   L{RFC3339} tid[(thread_identify)] file:line Vn] message\n  (Vn if verbose)
//   L = T/D/I/W/E/F  (XVLOG stays L=I)
//   RFC3339 = YYYY-MM-DDThh:mm:ss.ssssssZ or ...±hh:mm
//   thread = tid(thread_identify) if name set, else tid
//
// Without prefix:
//   hello world\n
void xlog_format(LogEntry &entry) {
    entry.format_buffer.clear();

    const auto &cfg = LogConfig::instance();
    const std::string_view msg(entry.buffer.data(), entry.buffer.size());

    if (cfg.log_with_prefix && entry.prefix) {
        const auto tp = entry.timestamp;
        const auto secs = std::chrono::floor<std::chrono::seconds>(tp);
        const auto us = static_cast<unsigned>(
            std::chrono::duration_cast<std::chrono::microseconds>(tp - secs)
                .count());

        auto &cache = TlsTimeCache();
        if (!cache.valid || cache.last_sec != secs.time_since_epoch() ||
            cache.last_utc != cfg.utc) {
            FillTimeCache(cache, tp, cfg.utc);
        }

        fmt::format_to(
            std::back_inserter(entry.format_buffer), "{}{}.{:06}{}",
            severity_to_upper_char(entry.log_severity),
            std::string_view(cache.datetime, sizeof(cache.datetime)), us,
            std::string_view(cache.zone, cache.zone_len));

        const std::string_view file = BaseName(entry.filename);
        // XVLOG: keep INFO letter; mark numeric level before ']' (e.g. " V2]").
        if (!entry.thread_identify.empty()) {
            if (entry.verbose_level > 0) {
                fmt::format_to(std::back_inserter(entry.format_buffer),
                               " {}({}) {}:{} V{}] ", entry.tid,
                               entry.thread_identify, file, entry.line,
                               entry.verbose_level);
            } else {
                fmt::format_to(std::back_inserter(entry.format_buffer),
                               " {}({}) {}:{}] ", entry.tid,
                               entry.thread_identify, file, entry.line);
            }
        } else if (entry.verbose_level > 0) {
            fmt::format_to(std::back_inserter(entry.format_buffer),
                           " {} {}:{} V{}] ", entry.tid, file, entry.line,
                           entry.verbose_level);
        } else {
            fmt::format_to(std::back_inserter(entry.format_buffer),
                           " {} {}:{}] ", entry.tid, file, entry.line);
        }
    }

    fmt::format_to(std::back_inserter(entry.format_buffer), "{}", msg);
    if (msg.empty() || msg.back() != '\n') {
        entry.format_buffer.push_back('\n');
    }
}

}  // namespace xlog
