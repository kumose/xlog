#pragma once

#include <string>
#include <type_traits>
#include <cstring>
#include <strings.h>
#include <cstdlib>

#include <xlog/common.h>
#include <xlog/xlog.h>
#include <xlog/fmt/bundled/printf.h>

namespace xlog {
namespace internal {

class CheckOpMessageBuilder {
    std::string msg_;
public:
    explicit CheckOpMessageBuilder(const char *exprtext)
        : msg_("Check failed: ") {
        msg_ += exprtext;
        msg_ += " (";
    }

    void AddValue(const std::string &v) { msg_ += v; }

    template <typename T>
    void AddValue(const T &v) {
        fmt::format_to(std::back_inserter(msg_), "{}", v);
    }

    std::string *NewString() {
        msg_ += ")";
        return new std::string(std::move(msg_));
    }
};

template <typename T1, typename T2>
inline std::string *MakeCheckOpString(const T1 &v1, const T2 &v2,
                                      const char *exprtext) {
    CheckOpMessageBuilder b(exprtext);
    b.AddValue(v1);
    b.AddValue(std::string(" vs. "));
    b.AddValue(v2);
    return b.NewString();
}

#define XLOG_INTERNAL_CHECK_OP_IMPL(name, op)                                \
    template <typename T1, typename T2>                                       \
    inline std::string *name##Impl(const T1 &v1, const T2 &v2,                \
                                   const char *exprtext) {                    \
        if (v1 op v2) return nullptr;                                         \
        return MakeCheckOpString(v1, v2, exprtext);                           \
    }                                                                         \
    inline std::string *name##Impl(int v1, int v2, const char *exprtext) {    \
        return name##Impl<int, int>(v1, v2, exprtext);                        \
    }

XLOG_INTERNAL_CHECK_OP_IMPL(Check_EQ, ==)
XLOG_INTERNAL_CHECK_OP_IMPL(Check_NE, !=)
XLOG_INTERNAL_CHECK_OP_IMPL(Check_LE, <=)
XLOG_INTERNAL_CHECK_OP_IMPL(Check_LT, <)
XLOG_INTERNAL_CHECK_OP_IMPL(Check_GE, >=)
XLOG_INTERNAL_CHECK_OP_IMPL(Check_GT, >)

#undef XLOG_INTERNAL_CHECK_OP_IMPL

template <typename T>
inline constexpr const T &GetReferenceableValue(const T &t) { return t; }

inline constexpr char GetReferenceableValue(char t) { return t; }
inline constexpr unsigned char GetReferenceableValue(unsigned char t) { return t; }
inline constexpr signed char GetReferenceableValue(signed char t) { return t; }
inline constexpr short GetReferenceableValue(short t) { return t; }
inline constexpr unsigned short GetReferenceableValue(unsigned short t) { return t; }
inline constexpr int GetReferenceableValue(int t) { return t; }
inline constexpr unsigned int GetReferenceableValue(unsigned int t) { return t; }
inline constexpr long GetReferenceableValue(long t) { return t; }
inline constexpr unsigned long GetReferenceableValue(unsigned long t) { return t; }
inline constexpr long long GetReferenceableValue(long long t) { return t; }
inline constexpr unsigned long long GetReferenceableValue(unsigned long long t) { return t; }

inline std::string *MakeStrCheckFailString(const char *s1, const char *s2,
                                           const char *exprtext) {
    auto result = std::string(exprtext) + " (";
    result += s1 ? s1 : "null";
    result += " vs. ";
    result += s2 ? s2 : "null";
    result += ")";
    return new std::string(std::move(result));
}

inline std::string *CheckstrcmptrueImpl(const char *s1, const char *s2,
                                        const char *exprtext) {
    if ((s1 == nullptr && s2 == nullptr) ||
        (s1 != nullptr && s2 != nullptr && strcmp(s1, s2) == 0)) {
        return nullptr;
    }
    return MakeStrCheckFailString(s1, s2, exprtext);
}

inline std::string *CheckstrcmpfalseImpl(const char *s1, const char *s2,
                                         const char *exprtext) {
    if (s1 == nullptr && s2 == nullptr) {
        return MakeStrCheckFailString(s1, s2, exprtext);
    }
    if (s1 == nullptr || s2 == nullptr || strcmp(s1, s2) != 0) {
        return nullptr;
    }
    return MakeStrCheckFailString(s1, s2, exprtext);
}

inline std::string *CheckstrcasecmptrueImpl(const char *s1, const char *s2,
                                            const char *exprtext) {
    if ((s1 == nullptr && s2 == nullptr) ||
        (s1 != nullptr && s2 != nullptr && strcasecmp(s1, s2) == 0)) {
        return nullptr;
    }
    return MakeStrCheckFailString(s1, s2, exprtext);
}

inline std::string *CheckstrcasecmpfalseImpl(const char *s1, const char *s2,
                                             const char *exprtext) {
    if (s1 == nullptr && s2 == nullptr) {
        return MakeStrCheckFailString(s1, s2, exprtext);
    }
    if (s1 == nullptr || s2 == nullptr || strcasecmp(s1, s2) != 0) {
        return nullptr;
    }
    return MakeStrCheckFailString(s1, s2, exprtext);
}

[[noreturn]] inline void CheckFail(const char *file, int line, const std::string &msg) {
    ::xlog::default_logger_raw()->log(
        ::xlog::source_loc{file, line, XLOG_FUNCTION},
        ::xlog::level::critical,
        ::xlog::string_view_t(msg));
    ::std::abort();
}

inline void CheckFailFmtImpl(const char *file, int line, const char *cond) {
    CheckFail(file, line, std::string("Check failed: ") + cond);
}

template <typename... Args>
inline void CheckFailFmtImpl(const char *file, int line, const char *cond,
                             format_string_t<Args...> fmt, Args &&...args) {
    const std::string detail = fmt::format(fmt, std::forward<Args>(args)...);
    CheckFail(file, line,
              fmt::format(FMT_STRING("Check failed: {}: {}"), cond, detail));
}

inline void CheckFailPrintfImpl(const char *file, int line, const char *cond) {
    CheckFail(file, line, std::string("Check failed: ") + cond);
}

inline void CheckFailPrintfImpl(const char *file, int line, const char *cond,
                                const char *fmt) {
    CheckFail(file, line, std::string("Check failed: ") + cond + ": " + fmt);
}

template <typename... Args>
inline void CheckFailPrintfImpl(const char *file, int line, const char *cond,
                                const char *fmt, Args &&...args) {
    const std::string detail = fmt::sprintf(fmt, std::forward<Args>(args)...);
    CheckFail(file, line,
              std::string("Check failed: ") + cond + ": " + detail);
}

template <typename T>
inline T CheckNotNull(const char *file, int line, const char *names, T &&t) {
    if (t == nullptr) {
        CheckFail(file, line, std::string("Check failed: ") + names);
    }
    return std::forward<T>(t);
}

} // namespace internal
} // namespace xlog

#define XLOG_INTERNAL_CHECK_OP_FAIL(name, op, val1, val2)                       \
    while (auto _xcheck_op_result_ =                                            \
               ::xlog::internal::name##Impl(                                    \
                   ::xlog::internal::GetReferenceableValue(val1),               \
                   ::xlog::internal::GetReferenceableValue(val2),               \
                   #val1 " " #op " " #val2))                                    \
        ::xlog::internal::CheckFail(__FILE__, __LINE__, *_xcheck_op_result_)

#define XLOG_INTERNAL_CHECK_STROP(func, op, expected, s1, s2)                   \
    while (auto _xcheck_strop_result_ =                                         \
               ::xlog::internal::Check##func##expected##Impl(                   \
                   (s1), (s2),                                                  \
                   #s1 " " #op " " #s2))                                        \
        ::xlog::internal::CheckFail(__FILE__, __LINE__, *_xcheck_strop_result_)
