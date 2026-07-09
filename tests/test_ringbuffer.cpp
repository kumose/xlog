#include "includes.h"
#include <xlog/sinks/ringbuffer_sink.h>

TEST_CASE("ringbuffer invalid size", "[ringbuffer]") {
    REQUIRE_THROWS_AS(xlog::sinks::ringbuffer_sink_mt(0), xlog::spdlog_ex);
}

TEST_CASE("ringbuffer stores formatted messages", "[ringbuffer]") {
    xlog::sinks::ringbuffer_sink_st sink(3);
    sink.set_pattern("%v");

    sink.log(xlog::details::log_msg{"test", xlog::level::info, "msg1"});
    sink.log(xlog::details::log_msg{"test", xlog::level::info, "msg2"});
    sink.log(xlog::details::log_msg{"test", xlog::level::info, "msg3"});

    auto formatted = sink.last_formatted();
    REQUIRE(formatted.size() == 3);
    using xlog::details::os::default_eol;
    REQUIRE(formatted[0] == xlog::fmt_lib::format("msg1{}", default_eol));
    REQUIRE(formatted[1] == xlog::fmt_lib::format("msg2{}", default_eol));
    REQUIRE(formatted[2] == xlog::fmt_lib::format("msg3{}", default_eol));
}

TEST_CASE("ringbuffer overrun keeps last items", "[ringbuffer]") {
    xlog::sinks::ringbuffer_sink_st sink(2);
    sink.set_pattern("%v");

    sink.log(xlog::details::log_msg{"test", xlog::level::info, "first"});
    sink.log(xlog::details::log_msg{"test", xlog::level::info, "second"});
    sink.log(xlog::details::log_msg{"test", xlog::level::info, "third"});

    auto formatted = sink.last_formatted();
    REQUIRE(formatted.size() == 2);
    using xlog::details::os::default_eol;
    REQUIRE(formatted[0] == xlog::fmt_lib::format("second{}", default_eol));
    REQUIRE(formatted[1] == xlog::fmt_lib::format("third{}", default_eol));
}

TEST_CASE("ringbuffer retrieval limit", "[ringbuffer]") {
    xlog::sinks::ringbuffer_sink_st sink(3);
    sink.set_pattern("%v");

    sink.log(xlog::details::log_msg{"test", xlog::level::info, "A"});
    sink.log(xlog::details::log_msg{"test", xlog::level::info, "B"});
    sink.log(xlog::details::log_msg{"test", xlog::level::info, "C"});

    auto formatted = sink.last_formatted(2);
    REQUIRE(formatted.size() == 2);
    using xlog::details::os::default_eol;
    REQUIRE(formatted[0] == xlog::fmt_lib::format("B{}", default_eol));
    REQUIRE(formatted[1] == xlog::fmt_lib::format("C{}", default_eol));
}
