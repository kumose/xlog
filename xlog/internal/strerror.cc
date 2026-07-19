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
// Adapted from Abseil absl/base/internal/strerror.cc (Apache-2.0).

#include <xlog/internal/strerror.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <type_traits>

// strerror_r is POSIX (not in the C++ standard); use the C header.
// musl / bionic: XSI (int); glibc with _GNU_SOURCE: GNU (char*).
#include <string.h>

namespace xlog {
namespace log_internal {
namespace {

    // Restores errno on scope exit (strerror_* may clobber it).
    class ErrnoSaver {
    public:
        ErrnoSaver() : saved_(errno) {}
        ~ErrnoSaver() { errno = saved_; }
        int operator()() const { return saved_; }

    private:
        int saved_;
    };

    const char *StrErrorAdaptor(int errnum, char *buf, size_t buflen) {
        if (buflen == 0) {
            return "";
        }
        buf[0] = '\0';
#if defined(_WIN32)
        // MSVC / UCRT / MinGW-w64: thread-safe; unknown codes often yield
        // "Unknown error" with rc==0 — treat as empty so we can normalize.
        const int rc = strerror_s(buf, buflen, errnum);
        buf[buflen - 1] = '\0';
        if (rc == 0 && std::strncmp(buf, "Unknown error", buflen) == 0) {
            buf[0] = '\0';
        }
        if (rc != 0) {
            buf[0] = '\0';
        }
        return buf;
#else
        // musl / Android bionic: XSI strerror_r (returns int).
        // glibc (_GNU_SOURCE): GNU strerror_r (returns char*).
        // Both branches must compile; only one runs per platform.
        auto ret = strerror_r(errnum, buf, buflen);
        if constexpr (std::is_same_v<decltype(ret), int>) {
            if (ret != 0) {
                buf[0] = '\0';
            }
            return buf;
        } else {
            return ret != nullptr ? ret : buf;
        }
#endif
    }

    std::string StrErrorInternal(int errnum) {
        char buf[128];
        const char *str = StrErrorAdaptor(errnum, buf, sizeof(buf));
        if (str == nullptr || str[0] == '\0') {
            std::snprintf(buf, sizeof(buf), "Unknown error %d", errnum);
            str = buf;
        }
        return std::string(str);
    }

}  // namespace

    std::string StrError(int errnum) {
        ErrnoSaver errno_saver;
        return StrErrorInternal(errnum);
    }

}  // namespace log_internal
}  // namespace xlog
