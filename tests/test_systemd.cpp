#include "includes.h"
#include <xlog/sinks/systemd_sink.h>

TEST_CASE("systemd", "[all]") {
    auto systemd_sink = std::make_shared<xlog::sinks::systemd_sink_st>();
    xlog::logger logger("spdlog_systemd_test", systemd_sink);
    logger.set_level(xlog::level::trace);
    logger.trace("test spdlog trace");
    logger.debug("test spdlog debug");
    logger.info("test spdlog info");
    logger.warn("test spdlog warn");
    logger.error("test spdlog error");
    logger.critical("test spdlog critical");
}
