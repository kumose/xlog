// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <ctime>  // std::time_t
#include <xlog/common.h>

namespace xlog {
    namespace details {
        namespace os {
            XLOG_API xlog::log_clock::time_point now() XLOG_NOEXCEPT;

            XLOG_API std::tm localtime(const std::time_t &time_tt) XLOG_NOEXCEPT;

            XLOG_API std::tm localtime() XLOG_NOEXCEPT;

            XLOG_API std::tm gmtime(const std::time_t &time_tt) XLOG_NOEXCEPT;

            XLOG_API std::tm gmtime() XLOG_NOEXCEPT;

            // eol definition
#if !defined(XLOG_EOL)
#ifdef _WIN32
#define XLOG_EOL "\r\n"
#else
#define XLOG_EOL "\n"
#endif
#endif

            XLOG_CONSTEXPR static const char *default_eol = XLOG_EOL;

            // folder separator
#if !defined(XLOG_FOLDER_SEPS)
#ifdef _WIN32
#define XLOG_FOLDER_SEPS "\\/"
#else
#define XLOG_FOLDER_SEPS "/"
#endif
#endif

            XLOG_CONSTEXPR static const char folder_seps[] = XLOG_FOLDER_SEPS;
            XLOG_CONSTEXPR static const filename_t::value_type folder_seps_filename[] =
                    XLOG_FILENAME_T(XLOG_FOLDER_SEPS);

            // fopen_s on non windows for writing
            XLOG_API bool fopen_s(FILE **fp, const filename_t &filename, const filename_t &mode);

            // Remove filename. return 0 on success
            XLOG_API int remove(const filename_t &filename) XLOG_NOEXCEPT;

            // Remove file if exists. return 0 on success
            // Note: Non atomic (might return failure to delete if concurrently deleted by other process/thread)
            XLOG_API int remove_if_exists(const filename_t &filename) XLOG_NOEXCEPT;

            XLOG_API int rename(const filename_t &filename1, const filename_t &filename2) XLOG_NOEXCEPT;

            // Return if file exists.
            XLOG_API bool path_exists(const filename_t &filename) XLOG_NOEXCEPT;

            // Return file size according to open FILE* object
            XLOG_API size_t filesize(FILE *f);

            // Return utc offset in minutes or throw spdlog_ex on failure
            XLOG_API int utc_minutes_offset(const std::tm &tm = details::os::localtime());

            // Return current thread id as size_t
            // It exists because the std::this_thread::get_id() is much slower(especially
            // under VS 2013)
            XLOG_API size_t _thread_id() XLOG_NOEXCEPT;

            // Return current thread id as size_t (from thread local storage)
            XLOG_API size_t thread_id() XLOG_NOEXCEPT;

            // This is avoid msvc issue in sleep_for that happens if the clock changes.
            // See https://github.com/gabime/spdlog/issues/609
            XLOG_API void sleep_for_millis(unsigned int milliseconds) XLOG_NOEXCEPT;

            XLOG_API std::string filename_to_str(const filename_t &filename);

            XLOG_API int pid() XLOG_NOEXCEPT;

            // Determine if the terminal supports colors
            // Source: https://github.com/agauniyal/rang/
            XLOG_API bool is_color_terminal() XLOG_NOEXCEPT;

            // Determine if the terminal attached
            // Source: https://github.com/agauniyal/rang/
            XLOG_API bool in_terminal(FILE *file) XLOG_NOEXCEPT;

#if (defined(XLOG_WCHAR_TO_UTF8_SUPPORT) || defined(XLOG_WCHAR_FILENAMES)) && defined(_WIN32)
            XLOG_API void wstr_to_utf8buf(wstring_view_t wstr, memory_buf_t &target);

            XLOG_API void utf8_to_wstrbuf(string_view_t str, wmemory_buf_t &target);
#endif

            // Return directory name from given path or empty string
            // "abc/file" => "abc"
            // "abc/" => "abc"
            // "abc" => ""
            // "abc///" => "abc//"
            XLOG_API filename_t dir_name(const filename_t &path);

            // Create a dir from the given path.
            // Return true if succeeded or if this dir already exists.
            XLOG_API bool create_dir(const filename_t &path);

            // non thread safe, cross platform getenv/getenv_s
            // return empty string if field not found
            XLOG_API std::string getenv(const char *field);

            // Do fsync by FILE objectpointer.
            // Return true on success.
            XLOG_API bool fsync(FILE *fp);

            // Do non-locking fwrite if possible by the os or use the regular locking fwrite
            // Return true on success.
            XLOG_API bool fwrite_bytes(const void *ptr, const size_t n_bytes, FILE *fp);
        } // namespace os
    } // namespace details
} // namespace xlog
