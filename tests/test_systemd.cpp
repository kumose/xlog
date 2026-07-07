#include "includes.h"
#include <xlog/sinks/systemd_sink.h>

TEST_CASE("systemd", "[all]") {
    auto systemd_sink = std::make_shared<xlog::sinks::systemd_sink_st>();
    xlog::logger logger("spdlog_systemd_test", systemd_sink);
    logger.set_level(xlog::level::trace);
    logger.trace("test spdlog trace");
    logger.debug("test spdlog debug");
    XLOG_LOGGER_INFO((&logger), "test spdlog info");
    XLOG_LOGGER_WARN((&logger), "test spdlog warn");
    XLOG_LOGGER_ERROR((&logger), "test spdlog error");
    XLOG_LOGGER_CRITICAL((&logger), "test spdlog critical");
}
