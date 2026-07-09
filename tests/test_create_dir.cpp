/*
 * This content is released under the MIT License as specified in
 * https://raw.githubusercontent.com/gabime/spdlog/master/LICENSE
 */
#include "includes.h"

using xlog::details::os::create_dir;
using xlog::details::os::path_exists;

bool try_create_dir(const xlog::filename_t &path, const xlog::filename_t &normalized_path) {
    auto rv = create_dir(path);
    REQUIRE(rv == true);
    return path_exists(normalized_path);
}

TEST_CASE("create_dir", "[create_dir]") {
    prepare_logdir();

    REQUIRE(try_create_dir(XLOG_FILENAME_T("test_logs/dir1/dir1"),
                           XLOG_FILENAME_T("test_logs/dir1/dir1")));
    REQUIRE(try_create_dir(XLOG_FILENAME_T("test_logs/dir1/dir1"),
                           XLOG_FILENAME_T("test_logs/dir1/dir1")));  // test existing
    REQUIRE(try_create_dir(XLOG_FILENAME_T("test_logs/dir1///dir2//"),
                           XLOG_FILENAME_T("test_logs/dir1/dir2")));
    REQUIRE(try_create_dir(XLOG_FILENAME_T("./test_logs/dir1/dir3"),
                           XLOG_FILENAME_T("test_logs/dir1/dir3")));
    REQUIRE(try_create_dir(XLOG_FILENAME_T("test_logs/../test_logs/dir1/dir4"),
                           XLOG_FILENAME_T("test_logs/dir1/dir4")));

#ifdef WIN32
    // test backslash folder separator
    REQUIRE(try_create_dir(XLOG_FILENAME_T("test_logs\\dir1\\dir222"),
                           XLOG_FILENAME_T("test_logs\\dir1\\dir222")));
    REQUIRE(try_create_dir(XLOG_FILENAME_T("test_logs\\dir1\\dir223\\"),
                           XLOG_FILENAME_T("test_logs\\dir1\\dir223\\")));
    REQUIRE(try_create_dir(XLOG_FILENAME_T(".\\test_logs\\dir1\\dir2\\dir99\\..\\dir23"),
                           XLOG_FILENAME_T("test_logs\\dir1\\dir2\\dir23")));
    REQUIRE(try_create_dir(XLOG_FILENAME_T("test_logs\\..\\test_logs\\dir1\\dir5"),
                           XLOG_FILENAME_T("test_logs\\dir1\\dir5")));
#endif
}

TEST_CASE("create_invalid_dir", "[create_dir]") {
    REQUIRE(create_dir(XLOG_FILENAME_T("")) == false);
    REQUIRE(create_dir(xlog::filename_t{}) == false);
#ifdef __linux__
    REQUIRE(create_dir("/proc/spdlog-utest") == false);
#endif
}

TEST_CASE("dir_name", "[create_dir]") {
    using xlog::details::os::dir_name;
    REQUIRE(dir_name(XLOG_FILENAME_T("")).empty());
    REQUIRE(dir_name(XLOG_FILENAME_T("dir")).empty());

#ifdef WIN32
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(dir\)")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(dir\\\)")) == XLOG_FILENAME_T(R"(dir\\)"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(dir\file)")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(dir/file)")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(dir\file.txt)")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(dir/file)")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(dir\file.txt\)")) ==
            XLOG_FILENAME_T(R"(dir\file.txt)"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(\dir\file.txt)")) == XLOG_FILENAME_T(R"(\dir)"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(\\dir\file.txt)")) == XLOG_FILENAME_T(R"(\\dir)"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(..\file.txt)")) == XLOG_FILENAME_T(".."));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(.\file.txt)")) == XLOG_FILENAME_T("."));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(c:\\a\b\c\d\file.txt)")) ==
            XLOG_FILENAME_T(R"(c:\\a\b\c\d)"));
    REQUIRE(dir_name(XLOG_FILENAME_T(R"(c://a/b/c/d/file.txt)")) ==
            XLOG_FILENAME_T(R"(c://a/b/c/d)"));
#endif
    REQUIRE(dir_name(XLOG_FILENAME_T("dir/")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T("dir///")) == XLOG_FILENAME_T("dir//"));
    REQUIRE(dir_name(XLOG_FILENAME_T("dir/file")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T("dir/file.txt")) == XLOG_FILENAME_T("dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T("dir/file.txt/")) == XLOG_FILENAME_T("dir/file.txt"));
    REQUIRE(dir_name(XLOG_FILENAME_T("/dir/file.txt")) == XLOG_FILENAME_T("/dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T("//dir/file.txt")) == XLOG_FILENAME_T("//dir"));
    REQUIRE(dir_name(XLOG_FILENAME_T("../file.txt")) == XLOG_FILENAME_T(".."));
    REQUIRE(dir_name(XLOG_FILENAME_T("./file.txt")) == XLOG_FILENAME_T("."));
}

#ifdef _WIN32

//
// test windows cases when drive letter is given e.g. C:\\some-folder
//
#include <windows.h>
#include <fileapi.h>

std::string get_full_path(const std::string &relative_folder_path) {
    char full_path[MAX_PATH];

    DWORD result = ::GetFullPathNameA(relative_folder_path.c_str(), MAX_PATH, full_path, nullptr);
    // Return an empty string if failed to get full path
    return result > 0 && result < MAX_PATH ? std::string(full_path) : std::string();
}

std::wstring get_full_path(const std::wstring &relative_folder_path) {
    wchar_t full_path[MAX_PATH];
    DWORD result = ::GetFullPathNameW(relative_folder_path.c_str(), MAX_PATH, full_path, nullptr);
    return result > 0 && result < MAX_PATH ? std::wstring(full_path) : std::wstring();
}

xlog::filename_t::value_type find_non_existing_drive() {
    for (char drive = 'A'; drive <= 'Z'; ++drive) {
        std::string root_path = std::string(1, drive) + ":\\";
        UINT drive_type = GetDriveTypeA(root_path.c_str());
        if (drive_type == DRIVE_NO_ROOT_DIR) {
            return static_cast<xlog::filename_t::value_type>(drive);
        }
    }
    return '\0';  // No available drive found
}

TEST_CASE("create_abs_path1", "[create_dir]") {
    prepare_logdir();
    auto abs_path = get_full_path(XLOG_FILENAME_T("test_logs\\logdir1"));
    REQUIRE(!abs_path.empty());
    REQUIRE(create_dir(abs_path) == true);
}

TEST_CASE("create_abs_path2", "[create_dir]") {
    prepare_logdir();
    auto abs_path = get_full_path(XLOG_FILENAME_T("test_logs/logdir2"));
    REQUIRE(!abs_path.empty());
    REQUIRE(create_dir(abs_path) == true);
}

TEST_CASE("non_existing_drive", "[create_dir]") {
    prepare_logdir();
    xlog::filename_t path;

    auto non_existing_drive = find_non_existing_drive();
    path += non_existing_drive;
    path += XLOG_FILENAME_T(":\\");
    REQUIRE(create_dir(path) == false);
    path += XLOG_FILENAME_T("subdir");
    REQUIRE(create_dir(path) == false);
}
// #endif  // XLOG_WCHAR_FILENAMES
#endif  // _WIN32
