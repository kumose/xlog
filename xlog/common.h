// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <xlog/details/null_mutex.h>
#include <xlog/tweakme.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <type_traits>

#ifdef XLOG_USE_STD_FORMAT
#include <version>
#if __cpp_lib_format >= 202207L
#include <format>
#else
#include <string_view>
#endif
#endif

#ifdef XLOG_COMPILED_LIB
#if defined(XLOG_SHARED_LIB)
#if defined(_WIN32)
#ifdef spdlog_EXPORTS
#define XLOG_API __declspec(dllexport)
#else  // !spdlog_EXPORTS
#define XLOG_API __declspec(dllimport)
#endif
#else  // !defined(_WIN32)
#define XLOG_API __attribute__((visibility("default")))
#endif
#else  // !defined(XLOG_SHARED_LIB)
#define XLOG_API
#endif
#define XLOG_INLINE
#else  // !defined(XLOG_COMPILED_LIB)
#define XLOG_API
#define XLOG_INLINE inline
#endif  // #ifdef XLOG_COMPILED_LIB

#include <xlog/fmt/fmt.h>

#if !defined(XLOG_USE_STD_FORMAT) && \
FMT_VERSION >= 80000  // backward compatibility with fmt versions older than 8
#define XLOG_FMT_RUNTIME(format_string) fmt::runtime(format_string)
#define XLOG_FMT_STRING(format_string) FMT_STRING(format_string)
#if defined(XLOG_WCHAR_FILENAMES) || defined(XLOG_WCHAR_TO_UTF8_SUPPORT)
#include <xlog/fmt/xchar.h>
#endif
#else
#define XLOG_FMT_RUNTIME(format_string) format_string
#define XLOG_FMT_STRING(format_string) format_string
#endif

// visual studio up to 2013 does not support noexcept nor constexpr
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define XLOG_NOEXCEPT _NOEXCEPT
#define XLOG_CONSTEXPR
#else
#define XLOG_NOEXCEPT noexcept
#define XLOG_CONSTEXPR constexpr
#endif

// If building with std::format, can just use constexpr, otherwise if building with fmt
// XLOG_CONSTEXPR_FUNC needs to be set the same as FMT_CONSTEXPR to avoid situations where
// a constexpr function in spdlog could end up calling a non-constexpr function in fmt
// depending on the compiler
// If fmt determines it can't use constexpr, we should inline the function instead
#ifdef XLOG_USE_STD_FORMAT
#define XLOG_CONSTEXPR_FUNC constexpr
#else  // Being built with fmt
#if FMT_USE_CONSTEXPR
#define XLOG_CONSTEXPR_FUNC FMT_CONSTEXPR
#else
#define XLOG_CONSTEXPR_FUNC inline
#endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#define XLOG_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define XLOG_DEPRECATED __declspec(deprecated)
#else
#define XLOG_DEPRECATED
#endif

// disable thread local on msvc 2013
#ifndef XLOG_NO_TLS
#if (defined(_MSC_VER) && (_MSC_VER < 1900)) || defined(__cplusplus_winrt)
#define XLOG_NO_TLS 1
#endif
#endif

#ifndef XLOG_FUNCTION
#define XLOG_FUNCTION static_cast<const char *>(__FUNCTION__)
#endif

#ifdef XLOG_NO_EXCEPTIONS
#define XLOG_TRY
#define XLOG_THROW(ex)                               \
    do {                                               \
        printf("spdlog fatal error: %s\n", ex.what()); \
        std::abort();                                  \
    } while (0)
#define XLOG_CATCH_STD
#else
#define XLOG_TRY try
#define XLOG_THROW(ex) throw(ex)
#define XLOG_CATCH_STD             \
    catch (const std::exception &) { \
    }
#endif

namespace xlog {
    class formatter;

    namespace sinks {
        class sink;
    }

#if defined(_WIN32) && defined(XLOG_WCHAR_FILENAMES)
    using filename_t = std::wstring;
    // allow macro expansion to occur in XLOG_FILENAME_T
#define XLOG_FILENAME_T_INNER(s) L##s
#define XLOG_FILENAME_T(s) XLOG_FILENAME_T_INNER(s)
#else
    using filename_t = std::string;
#define XLOG_FILENAME_T(s) s
#endif

    using log_clock = std::chrono::system_clock;
    using sink_ptr = std::shared_ptr<sinks::sink>;
    using sinks_init_list = std::initializer_list<sink_ptr>;
    using err_handler = std::function<void(const std::string &err_msg)>;
#ifdef XLOG_USE_STD_FORMAT
    namespace fmt_lib = std;

    using string_view_t = std::string_view;
    using memory_buf_t = std::string;

    template<typename... Args>

#if __cpp_lib_format >= 202207L
    using format_string_t = std::format_string<Args...>;
#else
    using format_string_t = std::string_view;
#endif

    template<class T, class Char = char>
    struct is_convertible_to_basic_format_string
            : std::integral_constant<bool, std::is_convertible<T, std::basic_string_view<Char> >::value> {
    };

#if defined(XLOG_WCHAR_FILENAMES) || defined(XLOG_WCHAR_TO_UTF8_SUPPORT)
    using wstring_view_t = std::wstring_view;
    using wmemory_buf_t = std::wstring;

    template<typename... Args>

#if __cpp_lib_format >= 202207L
    using wformat_string_t = std::wformat_string<Args...>;
#else
    using wformat_string_t = std::wstring_view;
#endif
#endif
#define XLOG_BUF_TO_STRING(x) x
#else  // use fmt lib instead of std::format
    namespace fmt_lib = fmt;

    using string_view_t = fmt::basic_string_view<char>;
    using memory_buf_t = fmt::basic_memory_buffer<char, 250>;

    template<typename... Args>
    using format_string_t = fmt::format_string<Args...>;

    template<class T>
    using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

    template<typename Char>
#if FMT_VERSION >= 90101
    using fmt_runtime_string = fmt::runtime_format_string<Char>;
#else
    using fmt_runtime_string = fmt::basic_runtime<Char>;
#endif

    // clang doesn't like SFINAE disabled constructor in std::is_convertible<> so have to repeat the
    // condition from basic_format_string here, in addition, fmt::basic_runtime<Char> is only
    // convertible to basic_format_string<Char> but not basic_string_view<Char>
    template<class T, class Char = char>
    struct is_convertible_to_basic_format_string
            : std::integral_constant<bool,
                std::is_convertible<T, fmt::basic_string_view<Char> >::value ||
                std::is_same<remove_cvref_t<T>, fmt_runtime_string<Char> >::value> {
    };

#if defined(XLOG_WCHAR_FILENAMES) || defined(XLOG_WCHAR_TO_UTF8_SUPPORT)
    using wstring_view_t = fmt::basic_string_view<wchar_t>;
    using wmemory_buf_t = fmt::basic_memory_buffer<wchar_t, 250>;

    template<typename... Args>
    using wformat_string_t = fmt::wformat_string<Args...>;
#endif
#define XLOG_BUF_TO_STRING(x) fmt::to_string(x)
#endif

#ifdef XLOG_WCHAR_TO_UTF8_SUPPORT
#ifndef _WIN32
#error XLOG_WCHAR_TO_UTF8_SUPPORT only supported on windows
#endif  // _WIN32
#endif  // XLOG_WCHAR_TO_UTF8_SUPPORT

    template<class T>
    struct is_convertible_to_any_format_string
            : std::integral_constant<bool,
                is_convertible_to_basic_format_string<T, char>::value ||
                is_convertible_to_basic_format_string<T, wchar_t>::value> {
    };

#if defined(XLOG_NO_ATOMIC_LEVELS)
    using level_t = details::null_atomic_int;
#else
    using level_t = std::atomic<int>;
#endif

#define XLOG_LEVEL_TRACE 0
#define XLOG_LEVEL_DEBUG 1
#define XLOG_LEVEL_INFO 2
#define XLOG_LEVEL_WARN 3
#define XLOG_LEVEL_ERROR 4
#define XLOG_LEVEL_CRITICAL 5
#define XLOG_LEVEL_OFF 6

#ifndef XLOG_LEVEL_WARNING
#define XLOG_LEVEL_WARNING XLOG_LEVEL_WARN
#endif
#ifndef XLOG_LEVEL_FATAL
#define XLOG_LEVEL_FATAL XLOG_LEVEL_CRITICAL
#endif

#if !defined(XLOG_ACTIVE_LEVEL)
#define XLOG_ACTIVE_LEVEL XLOG_LEVEL_INFO
#endif

    // Log level enum
    namespace level {
        enum level_enum : int {
            trace = XLOG_LEVEL_TRACE,
            debug = XLOG_LEVEL_DEBUG,
            info = XLOG_LEVEL_INFO,
            warn = XLOG_LEVEL_WARN,
            err = XLOG_LEVEL_ERROR,
            critical = XLOG_LEVEL_CRITICAL,
            off = XLOG_LEVEL_OFF,
            n_levels
        };

#define XLOG_LEVEL_NAME_TRACE xlog::string_view_t("trace", 5)
#define XLOG_LEVEL_NAME_DEBUG xlog::string_view_t("debug", 5)
#define XLOG_LEVEL_NAME_INFO xlog::string_view_t("info", 4)
#define XLOG_LEVEL_NAME_WARNING xlog::string_view_t("warning", 7)
#define XLOG_LEVEL_NAME_ERROR xlog::string_view_t("error", 5)
#define XLOG_LEVEL_NAME_CRITICAL xlog::string_view_t("critical", 8)
#define XLOG_LEVEL_NAME_OFF xlog::string_view_t("off", 3)

#if !defined(XLOG_LEVEL_NAMES)
#define XLOG_LEVEL_NAMES                                                                  \
    {                                                                                       \
        XLOG_LEVEL_NAME_TRACE, XLOG_LEVEL_NAME_DEBUG, XLOG_LEVEL_NAME_INFO,           \
            XLOG_LEVEL_NAME_WARNING, XLOG_LEVEL_NAME_ERROR, XLOG_LEVEL_NAME_CRITICAL, \
            XLOG_LEVEL_NAME_OFF                                                           \
    }
#endif

#if !defined(XLOG_SHORT_LEVEL_NAMES)

#define XLOG_SHORT_LEVEL_NAMES \
    { "T", "D", "I", "W", "E", "C", "O" }
#endif

        XLOG_API const string_view_t &to_string_view(xlog::level::level_enum l) XLOG_NOEXCEPT;

        XLOG_API const char *to_short_c_str(xlog::level::level_enum l) XLOG_NOEXCEPT;

        XLOG_API xlog::level::level_enum from_str(const std::string &name) XLOG_NOEXCEPT;
    } // namespace level

    //
    // Color mode used by sinks with color support.
    //
    enum class color_mode { always, automatic, never };

    //
    // Pattern time - specific time getting to use for pattern_formatter.
    // local time by default
    //
    enum class pattern_time_type {
        local, // log localtime
        utc // log utc
    };

    //
    // Log exception
    //
    class XLOG_API spdlog_ex : public std::exception {
    public:
        explicit spdlog_ex(std::string msg);

        spdlog_ex(const std::string &msg, int last_errno);

        const char *what() const XLOG_NOEXCEPT override;

    private:
        std::string msg_;
    };

    [[noreturn]] XLOG_API void throw_spdlog_ex(const std::string &msg, int last_errno);

    [[noreturn]] XLOG_API void throw_spdlog_ex(std::string msg);

    struct source_loc {
        XLOG_CONSTEXPR source_loc() = default;

        XLOG_CONSTEXPR source_loc(const char *filename_in, int line_in, const char *funcname_in)
            : filename{filename_in},
              line{line_in},
              funcname{funcname_in} {
        }

        XLOG_CONSTEXPR bool empty() const XLOG_NOEXCEPT { return line <= 0; }
        const char *filename{nullptr};
        int line{0};
        const char *funcname{nullptr};
    };

    struct file_event_handlers {
        file_event_handlers()
            : before_open(nullptr),
              after_open(nullptr),
              before_close(nullptr),
              after_close(nullptr) {
        }

        std::function<void(const filename_t &filename)> before_open;
        std::function<void(const filename_t &filename, std::FILE *file_stream)> after_open;
        std::function<void(const filename_t &filename, std::FILE *file_stream)> before_close;
        std::function<void(const filename_t &filename)> after_close;
    };

    namespace details {
        // to_string_view

        XLOG_CONSTEXPR_FUNC xlog::string_view_t to_string_view(const memory_buf_t &buf)
            XLOG_NOEXCEPT {
            return xlog::string_view_t{buf.data(), buf.size()};
        }

        XLOG_CONSTEXPR_FUNC xlog::string_view_t to_string_view(xlog::string_view_t str)
            XLOG_NOEXCEPT {
            return str;
        }

#if defined(XLOG_WCHAR_FILENAMES) || defined(XLOG_WCHAR_TO_UTF8_SUPPORT)
        XLOG_CONSTEXPR_FUNC xlog::wstring_view_t to_string_view(const wmemory_buf_t &buf)
            XLOG_NOEXCEPT {
            return xlog::wstring_view_t{buf.data(), buf.size()};
        }

        XLOG_CONSTEXPR_FUNC xlog::wstring_view_t to_string_view(xlog::wstring_view_t str)
            XLOG_NOEXCEPT {
            return str;
        }
#endif

#if defined(XLOG_USE_STD_FORMAT) && __cpp_lib_format >= 202207L
        template<typename T, typename... Args>
        XLOG_CONSTEXPR_FUNC std::basic_string_view<T> to_string_view(
            std::basic_format_string<T, Args...> fmt) XLOG_NOEXCEPT {
            return fmt.get();
        }
#endif

        // make_unique support for pre c++14
        using std::enable_if_t;
        using std::make_unique;
        // to avoid useless casts (see https://github.com/nlohmann/json/issues/2893#issuecomment-889152324)
        template<typename T, typename U, enable_if_t<!std::is_same<T, U>::value, int> = 0>
        constexpr T conditional_static_cast(U value) {
            return static_cast<T>(value);
        }

        template<typename T, typename U, enable_if_t<std::is_same<T, U>::value, int> = 0>
        constexpr T conditional_static_cast(U value) {
            return value;
        }
    } // namespace details
} // namespace xlog
