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

#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

#include <fmt/format.h>
#include <xlog/log_severity.h>

namespace xlog {

    // Unconditionally write a `message` to stderr. If `severity` exceeds kInfo
    // we also flush the stderr stream.
    void write_to_stderr(std::string_view message, LogSeverity severity);

    std::string_view get_thread_identify();

    void set_thread_identify(std::string_view thread_identify);

    // ---------------------------------------------------------------------------
    // hex_string — fmt-only helpers for XLOG << / TLOG "{}"
    //
    //   nullptr / null pointer → "0x00"
    //   other pointers         → fmt::ptr (e.g. "0x7fff...")
    //   integers / enums       → "{:#x}"
    //   string_view / bytes    → contiguous lowercase hex (no separators)
    // ---------------------------------------------------------------------------

    inline std::string hex_string(std::nullptr_t) {
        return fmt::format("{:#04x}", 0);
    }

    inline std::string hex_string(const void *p) {
        if (p == nullptr) {
            return fmt::format("{:#04x}", 0);
        }
        return fmt::format("{}", p);
    }

    template <typename T>
    inline std::string hex_string(T *p) {
        return hex_string(static_cast<const void *>(p));
    }

    template <typename T>
    inline std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>,
                            std::string>
    hex_string(T value) {
        using U = std::make_unsigned_t<T>;
        return fmt::format("{:#x}", static_cast<U>(value));
    }

    inline std::string hex_string(bool value) {
        return fmt::format("{:#x}", static_cast<unsigned>(value ? 1 : 0));
    }

    template <typename E>
    inline std::enable_if_t<std::is_enum_v<E>, std::string> hex_string(E value) {
        using U = std::make_unsigned_t<std::underlying_type_t<E>>;
        return fmt::format("{:#x}", static_cast<U>(value));
    }

    // Byte-wise hex dump (e.g. "68656c6c6f"). Content, not pointer address.
    // Use hex_string(static_cast<const void*>(s)) for a C-string address.
    inline std::string hex_string(std::string_view bytes) {
        std::string out;
        out.reserve(bytes.size() * 2);
        for (unsigned char c : bytes) {
            fmt::format_to(std::back_inserter(out), "{:02x}", c);
        }
        return out;
    }

    inline std::string hex_string(const std::string &s) {
        return hex_string(std::string_view(s));
    }

}  // namespace xlog

