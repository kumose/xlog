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
// NullStream / NullStreamMaybeFatal / NullStreamFatal implement a subset of the
// LogMessage API and are used when logging is disabled (strip / failed IF).

#pragma once

#ifdef _WIN32
#include <cstdlib>
#else
#include <unistd.h>
#endif

#include <ios>
#include <ostream>
#include <string_view>
#include <xlog/log_severity.h>

namespace xlog {

    // Implements LogMessage's chaining API but does nothing. Inline so the
    // compiler can eliminate the instance and anything streamed in.
    class NullStream {
    public:
        NullStream &at_location(std::string_view, int) { return *this; }

        NullStream &with_verbosity(uint32_t) { return *this; }

        template <typename LogSinkType>
        NullStream &to_sink_also(LogSinkType *) {
            return *this;
        }

        template <typename LogSinkType>
        NullStream &to_sink_only(LogSinkType *) {
            return *this;
        }

        NullStream &internal_stream() { return *this; }

        template <typename... Args>
        NullStream &print(Args &&...) {
            return *this;
        }

        template <typename... Args>
        NullStream &printf(Args &&...) {
            return *this;
        }

        template <typename T>
        NullStream &operator<<(const T &) {
            return *this;
        }

        NullStream &operator<<(std::ostream &(*)(std::ostream &)) {
            return *this;
        }

        NullStream &operator<<(std::ios_base &(*)(std::ios_base &)) {
            return *this;
        }
    };

    // Terminates on destroy if severity is FATAL (for DFATAL / LEVEL(...)).
    class NullStreamMaybeFatal final : public NullStream {
    public:
        explicit NullStreamMaybeFatal(LogSeverity severity)
            : fatal_(severity == LogSeverity::kSeverityFatal) {}

        ~NullStreamMaybeFatal() {
            if (fatal_) {
#ifdef _WIN32
                std::_Exit(1);
#else
                _exit(1);
#endif
            }
        }

    private:
        bool fatal_;
    };

    // Always terminates on destroy (stripped FATAL path).
    class NullStreamFatal final : public NullStream {
    public:
        NullStreamFatal() = default;

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning(disable : 4722)
#endif
        [[noreturn]] ~NullStreamFatal() {
#ifdef _WIN32
            std::_Exit(1);
#else
            _exit(1);
#endif
        }
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
    };

}  // namespace xlog
