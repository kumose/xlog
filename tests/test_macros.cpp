#include "includes.h"
#include <xlog/logging.h>

#define TEST_FILENAME "test_logs/simple_log"

namespace {
int throw_if_param_evaluated() {
    throw std::runtime_error("Should not be evaluated");
}
}  // namespace

TEST_CASE("debug and trace w/o format string") {
    prepare_logdir();
    xlog::filename_t filename = XLOG_FILENAME_T(TEST_FILENAME);

    auto logger = xlog::create<xlog::sinks::basic_file_sink_mt>("logger", filename);
    logger->set_pattern("%v");
    logger->set_level(xlog::level::trace);

    const int saved_level = xlog::log_level();
    auto orig_default_logger = xlog::default_logger();
    xlog::log_level() = XLOG_LEVEL_TRACE;
    xlog::set_default_logger(logger);

    TLOG(TRACE, "Test message 1");
    TLOG(DEBUG, "Test message 2");
    logger->flush();

    using xlog::details::os::default_eol;
    REQUIRE(ends_with(file_contents(TEST_FILENAME),
                      xlog::fmt_lib::format("Test message 2{}", default_eol)));
    REQUIRE(count_lines(TEST_FILENAME) == 2);

    TLOG(TRACE, "Test message 3");
    TLOG(DEBUG, "Test message {}", 4);
    logger->flush();

    require_message_count(TEST_FILENAME, 4);
    REQUIRE(ends_with(file_contents(TEST_FILENAME),
                      xlog::fmt_lib::format("Test message 4{}", default_eol)));

    xlog::set_default_logger(std::move(orig_default_logger));
    xlog::log_level() = saved_level;
}

TEST_CASE("disable param evaluation") {
    const int saved_level = xlog::log_level();
    xlog::log_level() = XLOG_LEVEL_INFO;
    TLOG(TRACE, "Test message {}", throw_if_param_evaluated());
    xlog::log_level() = saved_level;
}

TEST_CASE("pass logger pointer") {
    auto logger = xlog::create<xlog::sinks::null_sink_mt>("refmacro");
    const int saved_level = xlog::log_level();
    auto orig_default_logger = xlog::default_logger();
    xlog::log_level() = XLOG_LEVEL_TRACE;
    xlog::set_default_logger(logger);
    TLOG(TRACE, "Test message 1");
    TLOG(DEBUG, "Test message 2");
    xlog::set_default_logger(std::move(orig_default_logger));
    xlog::log_level() = saved_level;
}
