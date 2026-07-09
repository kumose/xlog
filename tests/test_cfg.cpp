
#include "includes.h"
#include "test_sink.h"

#include <xlog/cfg/env.h>
#include <xlog/cfg/argv.h>

using xlog::cfg::load_argv_levels;
using xlog::cfg::load_env_levels;
using xlog::sinks::test_sink_st;

TEST_CASE("env", "[cfg]") {
    xlog::drop("l1");
    auto l1 = xlog::create<test_sink_st>("l1");
#ifdef _WIN32
    _putenv_s("XLOG_LEVEL", "l1=warn");
#else
    setenv("XLOG_LEVEL", "l1=warn", 1);
#endif
    load_env_levels();
    REQUIRE(l1->level() == xlog::level::warn);

#ifdef _WIN32
    _putenv_s("MYAPP_LEVEL", "l1=trace");
#else
    setenv("MYAPP_LEVEL", "l1=trace", 1);
#endif
    load_env_levels("MYAPP_LEVEL");
    REQUIRE(l1->level() == xlog::level::trace);

    xlog::set_default_logger(xlog::create<test_sink_st>("cfg-default"));
    REQUIRE(xlog::default_logger()->level() == xlog::level::info);
}

TEST_CASE("argv1", "[cfg]") {
    xlog::drop("l1");
    const char *argv[] = {"ignore", "XLOG_LEVEL=l1=warn"};
    load_argv_levels(2, argv);
    auto l1 = xlog::create<xlog::sinks::test_sink_st>("l1");
    REQUIRE(l1->level() == xlog::level::warn);
    REQUIRE(xlog::default_logger()->level() == xlog::level::info);
}

TEST_CASE("argv2", "[cfg]") {
    xlog::drop("l1");
    const char *argv[] = {"ignore", "XLOG_LEVEL=l1=warn,trace"};
    load_argv_levels(2, argv);
    auto l1 = xlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == xlog::level::warn);
    REQUIRE(xlog::default_logger()->level() == xlog::level::trace);
}

TEST_CASE("argv3", "[cfg]") {
    xlog::set_level(xlog::level::trace);

    xlog::drop("l1");
    const char *argv[] = {"ignore", "XLOG_LEVEL=junk_name=warn"};
    load_argv_levels(2, argv);
    auto l1 = xlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == xlog::level::trace);
    REQUIRE(xlog::default_logger()->level() == xlog::level::trace);
}

TEST_CASE("argv4", "[cfg]") {
    xlog::set_level(xlog::level::info);
    xlog::drop("l1");
    const char *argv[] = {"ignore", "XLOG_LEVEL=junk"};
    load_argv_levels(2, argv);
    auto l1 = xlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == xlog::level::info);
}

TEST_CASE("argv5", "[cfg]") {
    xlog::set_level(xlog::level::info);
    xlog::drop("l1");
    const char *argv[] = {"ignore", "ignore", "XLOG_LEVEL=l1=warn,trace"};
    load_argv_levels(3, argv);
    auto l1 = xlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == xlog::level::warn);
    REQUIRE(xlog::default_logger()->level() == xlog::level::trace);
    xlog::set_level(xlog::level::info);
}

TEST_CASE("argv6", "[cfg]") {
    xlog::set_level(xlog::level::err);
    const char *argv[] = {""};
    load_argv_levels(1, argv);
    REQUIRE(xlog::default_logger()->level() == xlog::level::err);
    xlog::set_level(xlog::level::info);
}

TEST_CASE("argv7", "[cfg]") {
    xlog::set_level(xlog::level::err);
    const char *argv[] = {""};
    load_argv_levels(0, argv);
    REQUIRE(xlog::default_logger()->level() == xlog::level::err);
    xlog::set_level(xlog::level::info);
}

TEST_CASE("level-not-set-test1", "[cfg]") {
    xlog::drop("l1");
    const char *argv[] = {"ignore", ""};
    load_argv_levels(2, argv);
    auto l1 = xlog::create<xlog::sinks::test_sink_st>("l1");
    l1->set_level(xlog::level::trace);
    REQUIRE(l1->level() == xlog::level::trace);
    REQUIRE(xlog::default_logger()->level() == xlog::level::info);
}

TEST_CASE("level-not-set-test2", "[cfg]") {
    xlog::drop("l1");
    xlog::drop("l2");
    const char *argv[] = {"ignore", "XLOG_LEVEL=l1=trace"};

    auto l1 = xlog::create<xlog::sinks::test_sink_st>("l1");
    l1->set_level(xlog::level::warn);
    auto l2 = xlog::create<xlog::sinks::test_sink_st>("l2");
    l2->set_level(xlog::level::warn);

    load_argv_levels(2, argv);

    REQUIRE(l1->level() == xlog::level::trace);
    REQUIRE(l2->level() == xlog::level::warn);
    REQUIRE(xlog::default_logger()->level() == xlog::level::info);
}

TEST_CASE("level-not-set-test3", "[cfg]") {
    xlog::drop("l1");
    xlog::drop("l2");
    const char *argv[] = {"ignore", "XLOG_LEVEL=l1=trace"};

    load_argv_levels(2, argv);

    auto l1 = xlog::create<xlog::sinks::test_sink_st>("l1");
    auto l2 = xlog::create<xlog::sinks::test_sink_st>("l2");

    REQUIRE(l1->level() == xlog::level::trace);
    REQUIRE(l2->level() == xlog::level::info);
    REQUIRE(xlog::default_logger()->level() == xlog::level::info);
}

TEST_CASE("level-not-set-test4", "[cfg]") {
    xlog::drop("l1");
    xlog::drop("l2");
    const char *argv[] = {"ignore", "XLOG_LEVEL=l1=trace,warn"};

    load_argv_levels(2, argv);

    auto l1 = xlog::create<xlog::sinks::test_sink_st>("l1");
    auto l2 = xlog::create<xlog::sinks::test_sink_st>("l2");

    REQUIRE(l1->level() == xlog::level::trace);
    REQUIRE(l2->level() == xlog::level::warn);
    REQUIRE(xlog::default_logger()->level() == xlog::level::warn);
}

TEST_CASE("level-not-set-test5", "[cfg]") {
    xlog::drop("l1");
    xlog::drop("l2");
    const char *argv[] = {"ignore", "XLOG_LEVEL=l1=junk,warn"};

    load_argv_levels(2, argv);

    auto l1 = xlog::create<xlog::sinks::test_sink_st>("l1");
    auto l2 = xlog::create<xlog::sinks::test_sink_st>("l2");

    REQUIRE(l1->level() == xlog::level::warn);
    REQUIRE(l2->level() == xlog::level::warn);
    REQUIRE(xlog::default_logger()->level() == xlog::level::warn);
}

TEST_CASE("restore-to-default", "[cfg]") {
    xlog::drop("l1");
    xlog::drop("l2");
    const char *argv[] = {"ignore", "XLOG_LEVEL=info"};
    load_argv_levels(2, argv);
    REQUIRE(xlog::default_logger()->level() == xlog::level::info);
}
