#include "includes.h"
#include <xlog/logging.h>

#if XLOG_ACTIVE_LEVEL != XLOG_LEVEL_DEBUG
#error "Invalid XLOG_ACTIVE_LEVEL in test. Should be XLOG_LEVEL_DEBUG"
#endif

#define TEST_FILENAME "test_logs/simple_log"

TEST_CASE("debug and trace w/o format string") {
    prepare_logdir();
    xlog::filename_t filename = XLOG_FILENAME_T(TEST_FILENAME);

    auto logger = xlog::create<xlog::sinks::basic_file_sink_mt>("logger", filename);
    logger->set_pattern("%v");
    logger->set_level(xlog::level::trace);

    TLOG_LOGGER_TRACE(logger, "Test message 1");
    TLOG_LOGGER_DEBUG(logger, "Test message 2");
    logger->flush();

    using xlog::details::os::default_eol;
    REQUIRE(ends_with(file_contents(TEST_FILENAME),
                      xlog::fmt_lib::format("Test message 2{}", default_eol)));
    REQUIRE(count_lines(TEST_FILENAME) == 1);

    auto orig_default_logger = xlog::default_logger();
    xlog::set_default_logger(logger);

    TLOG_TRACE("Test message 3");
    TLOG_DEBUG("Test message {}", 4);
    logger->flush();

    require_message_count(TEST_FILENAME, 2);
    REQUIRE(ends_with(file_contents(TEST_FILENAME),
                      xlog::fmt_lib::format("Test message 4{}", default_eol)));
    xlog::set_default_logger(std::move(orig_default_logger));
}

TEST_CASE("disable param evaluation") {
    TLOG_TRACE("Test message {}", throw std::runtime_error("Should not be evaluated"));
}

TEST_CASE("pass logger pointer") {
    auto logger = xlog::create<xlog::sinks::null_sink_mt>("refmacro");
    auto &ref = *logger;
    TLOG_LOGGER_TRACE(&ref, "Test message 1");
    TLOG_LOGGER_DEBUG(&ref, "Test message 2");
}
