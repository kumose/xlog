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
// Helpers for XCHECK_EQ and friends.

#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <xlog/internal/conditions.h>
#include <xlog/internal/nullstream.h>
#include <xlog/internal/strip.h>
#include <xlog/log_severity.h>

#ifdef XLOG_MIN_LOG_LEVEL
#define XLOG_INTERNAL_STRIP_STRING_LITERAL(literal)                          \
    (::xlog::LogSeverity::kSeverityFatal >=                                  \
             static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL)            \
         ? (literal)                                                         \
         : "")
#else
#define XLOG_INTERNAL_STRIP_STRING_LITERAL(literal) (literal)
#endif

#ifdef NDEBUG
#define XLOG_INTERNAL_DCHECK_NOP(x, y)   \
    while (false && ((void)(x), (void)(y), 0)) \
    ::xlog::NullStream().internal_stream()
#endif

#if defined(__GNUC__) || defined(__clang__)
#define XLOG_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define XLOG_UNLIKELY(x) (!!(x))
#endif

#define XLOG_INTERNAL_CHECK_OP(name, op, val1, val1_text, val2, val2_text)   \
    while (::std::string *xlog_internal_check_op_result                      \
               XLOG_INTERNAL_ATTRIBUTE_UNUSED_IF_STRIP_LOG =                 \
                   ::xlog::log_internal::name##Impl(                         \
                       ::xlog::log_internal::GetReferenceableValue(val1),    \
                       ::xlog::log_internal::GetReferenceableValue(val2),    \
                       XLOG_INTERNAL_STRIP_STRING_LITERAL(                   \
                           val1_text " " #op " " val2_text)))                \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                       \
    XLOG_INTERNAL_CHECK(*xlog_internal_check_op_result).internal_stream()

#define XLOG_INTERNAL_CHECK_STROP(func, op, expected, s1, s1_text, s2,       \
                                  s2_text)                                   \
    while (::std::string *xlog_internal_check_strop_result =                 \
               ::xlog::log_internal::Check##func##expected##Impl(            \
                   (s1), (s2),                                               \
                   XLOG_INTERNAL_STRIP_STRING_LITERAL(s1_text " " #op        \
                                                              " " s2_text))) \
        XLOG_INTERNAL_CONDITION_FATAL(STATELESS, true)                       \
    XLOG_INTERNAL_CHECK(*xlog_internal_check_strop_result).internal_stream()

namespace xlog {
namespace log_internal {

    class CheckOpMessageBuilder final {
    public:
        explicit CheckOpMessageBuilder(const char *exprtext);

        std::ostream &ForVar1() { return stream_; }

        std::ostream &ForVar2();

        std::string *NewString();

    private:
        std::ostringstream stream_;
    };

    template <typename T>
    inline void MakeCheckOpValueString(std::ostream &os, const T &v) {
        os << v;
    }

    void MakeCheckOpValueString(std::ostream &os, char v);
    void MakeCheckOpValueString(std::ostream &os, signed char v);
    void MakeCheckOpValueString(std::ostream &os, unsigned char v);
    void MakeCheckOpValueString(std::ostream &os, const void *p);

    template <typename T1, typename T2>
    std::string *MakeCheckOpString(T1 v1, T2 v2, const char *exprtext) {
        CheckOpMessageBuilder comb(exprtext);
        MakeCheckOpValueString(comb.ForVar1(), v1);
        MakeCheckOpValueString(comb.ForVar2(), v2);
        return comb.NewString();
    }

#ifdef XLOG_MIN_LOG_LEVEL
#define XLOG_INTERNAL_CHECK_OP_IMPL_RESULT(v1, v2, exprtext)                 \
    ((::xlog::LogSeverity::kSeverityFatal >=                                 \
      static_cast<::xlog::LogSeverity>(XLOG_MIN_LOG_LEVEL))                  \
         ? MakeCheckOpString(v1, v2, exprtext)                               \
         : new std::string())
#else
#define XLOG_INTERNAL_CHECK_OP_IMPL_RESULT(v1, v2, exprtext) \
    MakeCheckOpString(v1, v2, exprtext)
#endif

#define XLOG_INTERNAL_CHECK_OP_IMPL(name, op)                                \
    template <typename T1, typename T2>                                      \
    inline ::std::string *name##Impl(const T1 &v1, const T2 &v2,             \
                                     const char *exprtext) {                 \
        return XLOG_UNLIKELY(!(v1 op v2))                                    \
                   ? XLOG_INTERNAL_CHECK_OP_IMPL_RESULT(v1, v2, exprtext)    \
                   : nullptr;                                                \
    }                                                                        \
    inline ::std::string *name##Impl(int v1, int v2, const char *exprtext) { \
        return name##Impl<int, int>(v1, v2, exprtext);                       \
    }

    XLOG_INTERNAL_CHECK_OP_IMPL(Check_EQ, ==)
    XLOG_INTERNAL_CHECK_OP_IMPL(Check_NE, !=)
    XLOG_INTERNAL_CHECK_OP_IMPL(Check_LE, <=)
    XLOG_INTERNAL_CHECK_OP_IMPL(Check_LT, <)
    XLOG_INTERNAL_CHECK_OP_IMPL(Check_GE, >=)
    XLOG_INTERNAL_CHECK_OP_IMPL(Check_GT, >)

#undef XLOG_INTERNAL_CHECK_OP_IMPL_RESULT
#undef XLOG_INTERNAL_CHECK_OP_IMPL

    std::string *CheckstrcmptrueImpl(const char *s1, const char *s2,
                                     const char *exprtext);
    std::string *CheckstrcmpfalseImpl(const char *s1, const char *s2,
                                      const char *exprtext);
    std::string *CheckstrcasecmptrueImpl(const char *s1, const char *s2,
                                         const char *exprtext);
    std::string *CheckstrcasecmpfalseImpl(const char *s1, const char *s2,
                                          const char *exprtext);

    template <typename T>
    inline constexpr const T &GetReferenceableValue(const T &t) {
        return t;
    }

    inline constexpr char GetReferenceableValue(char t) { return t; }
    inline constexpr unsigned char GetReferenceableValue(unsigned char t) {
        return t;
    }
    inline constexpr signed char GetReferenceableValue(signed char t) {
        return t;
    }
    inline constexpr short GetReferenceableValue(short t) { return t; }
    inline constexpr unsigned short GetReferenceableValue(unsigned short t) {
        return t;
    }
    inline constexpr int GetReferenceableValue(int t) { return t; }
    inline constexpr unsigned int GetReferenceableValue(unsigned int t) {
        return t;
    }
    inline constexpr long GetReferenceableValue(long t) { return t; }
    inline constexpr unsigned long GetReferenceableValue(unsigned long t) {
        return t;
    }
    inline constexpr long long GetReferenceableValue(long long t) { return t; }
    inline constexpr unsigned long long GetReferenceableValue(
        unsigned long long t) {
        return t;
    }

}  // namespace log_internal
}  // namespace xlog
