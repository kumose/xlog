/*
 * This content is released under the MIT License as specified in
 * https://raw.githubusercontent.com/gabime/spdlog/master/LICENSE
 */
#include "includes.h"

#define TEST_FILENAME "test_logs/file_helper_test.txt"

using xlog::details::file_helper;

static void write_with_helper(file_helper &helper, size_t howmany) {
    xlog::memory_buf_t formatted;
    xlog::fmt_lib::format_to(std::back_inserter(formatted), "{}", std::string(howmany, '1'));
    helper.write(formatted);
    helper.flush();
}

TEST_CASE("file_helper_filename", "[file_helper::filename()]") {
    prepare_logdir();

    file_helper helper;
    xlog::filename_t target_filename = XLOG_FILENAME_T(TEST_FILENAME);
    helper.open(target_filename);
    REQUIRE(helper.filename() == target_filename);
}

TEST_CASE("file_helper_size", "[file_helper::size()]") {
    prepare_logdir();
    xlog::filename_t target_filename = XLOG_FILENAME_T(TEST_FILENAME);
    size_t expected_size = 123;
    {
        file_helper helper;
        helper.open(target_filename);
        write_with_helper(helper, expected_size);
        REQUIRE(static_cast<size_t>(helper.size()) == expected_size);
    }
    REQUIRE(get_filesize(TEST_FILENAME) == expected_size);
}

TEST_CASE("file_helper_reopen", "[file_helper::reopen()]") {
    prepare_logdir();
    xlog::filename_t target_filename = XLOG_FILENAME_T(TEST_FILENAME);
    file_helper helper;
    helper.open(target_filename);
    write_with_helper(helper, 12);
    REQUIRE(helper.size() == 12);
    helper.reopen(true);
    REQUIRE(helper.size() == 0);
}

TEST_CASE("file_helper_reopen2", "[file_helper::reopen(false)]") {
    prepare_logdir();
    xlog::filename_t target_filename = XLOG_FILENAME_T(TEST_FILENAME);
    size_t expected_size = 14;
    file_helper helper;
    helper.open(target_filename);
    write_with_helper(helper, expected_size);
    REQUIRE(helper.size() == expected_size);
    helper.reopen(false);
    REQUIRE(helper.size() == expected_size);
}

static void test_split_ext(const xlog::filename_t::value_type *fname,
                           const xlog::filename_t::value_type *expect_base,
                           const xlog::filename_t::value_type *expect_ext) {
    xlog::filename_t filename(fname);
    xlog::filename_t expected_base(expect_base);
    xlog::filename_t expected_ext(expect_ext);

    xlog::filename_t basename;
    xlog::filename_t ext;
    std::tie(basename, ext) = file_helper::split_by_extension(filename);
    REQUIRE(basename == expected_base);
    REQUIRE(ext == expected_ext);
}

TEST_CASE("file_helper_split_by_extension", "[file_helper::split_by_extension()]") {
    test_split_ext(XLOG_FILENAME_T("mylog.txt"), XLOG_FILENAME_T("mylog"),
                   XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T(".mylog.txt"), XLOG_FILENAME_T(".mylog"),
                   XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T(".mylog"), XLOG_FILENAME_T(".mylog"), XLOG_FILENAME_T(""));
    test_split_ext(XLOG_FILENAME_T("/aaa/bb.d/mylog"), XLOG_FILENAME_T("/aaa/bb.d/mylog"),
                   XLOG_FILENAME_T(""));
    test_split_ext(XLOG_FILENAME_T("/aaa/bb.d/mylog.txt"), XLOG_FILENAME_T("/aaa/bb.d/mylog"),
                   XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T("aaa/bbb/ccc/mylog.txt"),
                   XLOG_FILENAME_T("aaa/bbb/ccc/mylog"), XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T("aaa/bbb/ccc/mylog."), XLOG_FILENAME_T("aaa/bbb/ccc/mylog."),
                   XLOG_FILENAME_T(""));
    test_split_ext(XLOG_FILENAME_T("aaa/bbb/ccc/.mylog.txt"),
                   XLOG_FILENAME_T("aaa/bbb/ccc/.mylog"), XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T("/aaa/bbb/ccc/mylog.txt"),
                   XLOG_FILENAME_T("/aaa/bbb/ccc/mylog"), XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T("/aaa/bbb/ccc/.mylog"),
                   XLOG_FILENAME_T("/aaa/bbb/ccc/.mylog"), XLOG_FILENAME_T(""));
    test_split_ext(XLOG_FILENAME_T("../mylog.txt"), XLOG_FILENAME_T("../mylog"),
                   XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T(".././mylog.txt"), XLOG_FILENAME_T(".././mylog"),
                   XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T(".././mylog.txt/xxx"), XLOG_FILENAME_T(".././mylog.txt/xxx"),
                   XLOG_FILENAME_T(""));
    test_split_ext(XLOG_FILENAME_T("/mylog.txt"), XLOG_FILENAME_T("/mylog"),
                   XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T("//mylog.txt"), XLOG_FILENAME_T("//mylog"),
                   XLOG_FILENAME_T(".txt"));
    test_split_ext(XLOG_FILENAME_T(""), XLOG_FILENAME_T(""), XLOG_FILENAME_T(""));
    test_split_ext(XLOG_FILENAME_T("."), XLOG_FILENAME_T("."), XLOG_FILENAME_T(""));
    test_split_ext(XLOG_FILENAME_T("..txt"), XLOG_FILENAME_T("."), XLOG_FILENAME_T(".txt"));
}

TEST_CASE("file_event_handlers", "[file_helper]") {
    enum class flags { before_open, after_open, before_close, after_close };
    prepare_logdir();

    xlog::filename_t test_filename = XLOG_FILENAME_T(TEST_FILENAME);
    // define event handles that update vector of flags when called
    std::vector<flags> events;
    xlog::file_event_handlers handlers;
    handlers.before_open = [&](xlog::filename_t filename) {
        REQUIRE(filename == test_filename);
        events.push_back(flags::before_open);
    };
    handlers.after_open = [&](xlog::filename_t filename, std::FILE *fstream) {
        REQUIRE(filename == test_filename);
        REQUIRE(fstream);
        fputs("after_open\n", fstream);
        events.push_back(flags::after_open);
    };
    handlers.before_close = [&](xlog::filename_t filename, std::FILE *fstream) {
        REQUIRE(filename == test_filename);
        REQUIRE(fstream);
        fputs("before_close\n", fstream);
        events.push_back(flags::before_close);
    };
    handlers.after_close = [&](xlog::filename_t filename) {
        REQUIRE(filename == test_filename);
        events.push_back(flags::after_close);
    };
    {
        xlog::details::file_helper helper{handlers};
        REQUIRE(events.empty());

        helper.open(test_filename);
        REQUIRE(events == std::vector<flags>{flags::before_open, flags::after_open});

        events.clear();
        helper.close();
        REQUIRE(events == std::vector<flags>{flags::before_close, flags::after_close});
        REQUIRE(file_contents(TEST_FILENAME) == "after_open\nbefore_close\n");

        helper.reopen(true);
        events.clear();
    }
    // make sure that the file_helper destructor calls the close callbacks if needed
    REQUIRE(events == std::vector<flags>{flags::before_close, flags::after_close});
    REQUIRE(file_contents(TEST_FILENAME) == "after_open\nbefore_close\n");
}

TEST_CASE("file_helper_open", "[file_helper]") {
    prepare_logdir();
    xlog::filename_t target_filename = XLOG_FILENAME_T(TEST_FILENAME);
    file_helper helper;
    helper.open(target_filename);
    helper.close();

    target_filename += XLOG_FILENAME_T("/invalid");
    REQUIRE_THROWS_AS(helper.open(target_filename), xlog::spdlog_ex);
}
