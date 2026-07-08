/*
 * This content is released under the MIT License as specified in
 * https://raw.githubusercontent.com/gabime/spdlog/master/LICENSE
 */
#include "includes.h"
#include <xlog/sinks/stdout_sinks.h>
#include <xlog/sinks/stdout_color_sinks.h>

TEST_CASE("stdout_st", "[stdout]") {
    auto l = xlog::stdout_logger_st("test");
    l->set_pattern("%+");
    l->set_level(xlog::level::trace);
    l->trace("Test stdout_st");
    xlog::drop_all();
}

TEST_CASE("stdout_mt", "[stdout]") {
    auto l = xlog::stdout_logger_mt("test");
    l->set_pattern("%+");
    l->set_level(xlog::level::debug);
    l->debug("Test stdout_mt");
    xlog::drop_all();
}

TEST_CASE("stderr_st", "[stderr]") {
    auto l = xlog::stderr_logger_st("test");
    l->set_pattern("%+");
    l->info("Test stderr_st");
    xlog::drop_all();
}

TEST_CASE("stderr_mt", "[stderr]") {
    auto l = xlog::stderr_logger_mt("test");
    l->set_pattern("%+");
    l->info("Test stderr_mt");
    l->warn("Test stderr_mt");
    l->error("Test stderr_mt");
    l->critical("Test stderr_mt");
    xlog::drop_all();
}

// color loggers
TEST_CASE("stdout_color_st", "[stdout]") {
    auto l = xlog::stdout_color_st("test");
    l->set_pattern("%+");
    l->info("Test stdout_color_st");
    xlog::drop_all();
}

TEST_CASE("stdout_color_mt", "[stdout]") {
    auto l = xlog::stdout_color_mt("test");
    l->set_pattern("%+");
    l->set_level(xlog::level::trace);
    l->trace("Test stdout_color_mt");
    xlog::drop_all();
}

TEST_CASE("stderr_color_st", "[stderr]") {
    auto l = xlog::stderr_color_st("test");
    l->set_pattern("%+");
    l->set_level(xlog::level::debug);
    l->debug("Test stderr_color_st");
    xlog::drop_all();
}

TEST_CASE("stderr_color_mt", "[stderr]") {
    auto l = xlog::stderr_color_mt("test");
    l->set_pattern("%+");
    l->info("Test stderr_color_mt");
    l->warn("Test stderr_color_mt");
    l->error("Test stderr_color_mt");
    l->critical("Test stderr_color_mt");
    xlog::drop_all();
}

TEST_CASE("show_utc_offset", "[stdout]") {
    auto l = xlog::stdout_color_mt("test");
    l->set_pattern("[%c %z] [%n] [%^%l%$] %v");
    l->info("Full date");
    xlog::drop_all();
}

#ifdef XLOG_WCHAR_TO_UTF8_SUPPORT
TEST_CASE("wchar_api", "[stdout]") {
    auto l = xlog::stdout_logger_st("wchar_logger");
    l->set_pattern("%+");
    l->set_level(xlog::level::trace);
    l->trace(L"Test wchar_api");
    l->trace(L"Test wchar_api {}", L"param");
    l->trace(L"Test wchar_api {}", 1);
    l->trace(L"Test wchar_api {}", std::wstring{L"wstring param"});
    l->trace(std::wstring{L"Test wchar_api wstring"});
    l->debug(L"Test logger debug {}", L"param");
    xlog::drop_all();
}
#endif
