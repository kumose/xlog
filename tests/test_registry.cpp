#include "includes.h"

static const char *const tested_logger_name = "null_logger";
static const char *const tested_logger_name2 = "null_logger2";

#ifndef XLOG_NO_EXCEPTIONS
TEST_CASE("register_drop", "[registry]") {
    xlog::drop_all();
    xlog::create<xlog::sinks::null_sink_mt>(tested_logger_name);
    REQUIRE(xlog::get(tested_logger_name) != nullptr);
    // Throw if registering existing name
    REQUIRE_THROWS_AS(xlog::create<xlog::sinks::null_sink_mt>(tested_logger_name),
                      xlog::spdlog_ex);
}

TEST_CASE("explicit register", "[registry]") {
    xlog::drop_all();
    auto logger = std::make_shared<xlog::logger>(tested_logger_name,
                                                   std::make_shared<xlog::sinks::null_sink_st>());
    xlog::register_logger(logger);
    REQUIRE(xlog::get(tested_logger_name) != nullptr);
    // Throw if registering existing name
    REQUIRE_THROWS_AS(xlog::create<xlog::sinks::null_sink_mt>(tested_logger_name),
                      xlog::spdlog_ex);
}
#endif

TEST_CASE("register_or_replace", "[registry]") {
    xlog::drop_all();
    auto logger1 = std::make_shared<xlog::logger>(
        tested_logger_name, std::make_shared<xlog::sinks::null_sink_st>());
    xlog::register_logger(logger1);
    REQUIRE(xlog::get(tested_logger_name) == logger1);

    auto logger2 = std::make_shared<xlog::logger>(
        tested_logger_name, std::make_shared<xlog::sinks::null_sink_st>());
    xlog::register_or_replace(logger2);
    REQUIRE(xlog::get(tested_logger_name) == logger2);
}

TEST_CASE("apply_all", "[registry]") {
    xlog::drop_all();
    auto logger = std::make_shared<xlog::logger>(tested_logger_name,
                                                   std::make_shared<xlog::sinks::null_sink_st>());
    xlog::register_logger(logger);
    auto logger2 = std::make_shared<xlog::logger>(
        tested_logger_name2, std::make_shared<xlog::sinks::null_sink_st>());
    xlog::register_logger(logger2);

    int counter = 0;
    xlog::apply_all([&counter](std::shared_ptr<xlog::logger>) { counter++; });
    REQUIRE(counter == 2);

    counter = 0;
    xlog::drop(tested_logger_name2);
    xlog::apply_all([&counter](std::shared_ptr<xlog::logger> l) {
        REQUIRE(l->name() == tested_logger_name);
        counter++;
    });
    REQUIRE(counter == 1);
}

TEST_CASE("drop", "[registry]") {
    xlog::drop_all();
    xlog::create<xlog::sinks::null_sink_mt>(tested_logger_name);
    xlog::drop(tested_logger_name);
    REQUIRE_FALSE(xlog::get(tested_logger_name));
}

TEST_CASE("drop-default", "[registry]") {
    xlog::set_default_logger(xlog::null_logger_st(tested_logger_name));
    xlog::drop(tested_logger_name);
    REQUIRE_FALSE(xlog::default_logger());
    REQUIRE_FALSE(xlog::get(tested_logger_name));
}

TEST_CASE("drop_all", "[registry]") {
    xlog::drop_all();
    xlog::create<xlog::sinks::null_sink_mt>(tested_logger_name);
    xlog::create<xlog::sinks::null_sink_mt>(tested_logger_name2);
    xlog::drop_all();
    REQUIRE_FALSE(xlog::get(tested_logger_name));
    REQUIRE_FALSE(xlog::get(tested_logger_name2));
    REQUIRE_FALSE(xlog::default_logger());
}

TEST_CASE("drop non existing", "[registry]") {
    xlog::drop_all();
    xlog::create<xlog::sinks::null_sink_mt>(tested_logger_name);
    xlog::drop("some_name");
    REQUIRE_FALSE(xlog::get("some_name"));
    REQUIRE(xlog::get(tested_logger_name));
    xlog::drop_all();
}

TEST_CASE("default logger", "[registry]") {
    xlog::drop_all();
    xlog::set_default_logger(xlog::null_logger_st(tested_logger_name));
    REQUIRE(xlog::get(tested_logger_name) == xlog::default_logger());
    xlog::drop_all();
}

TEST_CASE("set_default_logger(nullptr)", "[registry]") {
    xlog::set_default_logger(nullptr);
    REQUIRE_FALSE(xlog::default_logger());
}

TEST_CASE("disable automatic registration", "[registry]") {
    // set some global parameters
    xlog::level::level_enum log_level = xlog::level::level_enum::warn;
    xlog::set_level(log_level);
    // but disable automatic registration
    xlog::set_automatic_registration(false);
    auto logger1 = xlog::create<xlog::sinks::daily_file_sink_st>(
        tested_logger_name, XLOG_FILENAME_T("filename"), 11, 59);
    auto logger2 = xlog::create_async<xlog::sinks::stdout_color_sink_mt>(tested_logger_name2);
    // loggers should not be part of the registry
    REQUIRE_FALSE(xlog::get(tested_logger_name));
    REQUIRE_FALSE(xlog::get(tested_logger_name2));
    // but make sure they are still initialized according to global defaults
    REQUIRE(logger1->level() == log_level);
    REQUIRE(logger2->level() == log_level);
    xlog::set_level(xlog::level::info);
    xlog::set_automatic_registration(true);
}
