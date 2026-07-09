#include "includes.h"
#include "test_sink.h"
#include <xlog/fmt/bin_to_hex.h>

TEST_CASE("to_hex", "[to_hex]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<xlog::sinks::ostream_sink_mt>(oss);
    xlog::logger oss_logger("oss", oss_sink);

    std::vector<unsigned char> v{9, 0xa, 0xb, 0xc, 0xff, 0xff};
    oss_logger.info("{}", xlog::to_hex(v));

    auto output = oss.str();
    REQUIRE(ends_with(output,
                      "0000: 09 0a 0b 0c ff ff" + std::string(xlog::details::os::default_eol)));
}

TEST_CASE("to_hex_upper", "[to_hex]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<xlog::sinks::ostream_sink_mt>(oss);
    xlog::logger oss_logger("oss", oss_sink);

    std::vector<unsigned char> v{9, 0xa, 0xb, 0xc, 0xff, 0xff};
    oss_logger.info("{:X}", xlog::to_hex(v));

    auto output = oss.str();
    REQUIRE(ends_with(output,
                      "0000: 09 0A 0B 0C FF FF" + std::string(xlog::details::os::default_eol)));
}

TEST_CASE("to_hex_no_delimiter", "[to_hex]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<xlog::sinks::ostream_sink_mt>(oss);
    xlog::logger oss_logger("oss", oss_sink);

    std::vector<unsigned char> v{9, 0xa, 0xb, 0xc, 0xff, 0xff};
    oss_logger.info("{:sX}", xlog::to_hex(v));

    auto output = oss.str();
    REQUIRE(
        ends_with(output, "0000: 090A0B0CFFFF" + std::string(xlog::details::os::default_eol)));
}

TEST_CASE("to_hex_show_ascii", "[to_hex]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<xlog::sinks::ostream_sink_mt>(oss);
    xlog::logger oss_logger("oss", oss_sink);

    std::vector<unsigned char> v{9, 0xa, 0xb, 0x41, 0xc, 0x4b, 0xff, 0xff};
    oss_logger.info("{:Xsa}", xlog::to_hex(v, 8));

    REQUIRE(ends_with(oss.str(), "0000: 090A0B410C4BFFFF  ...A.K.." +
                                     std::string(xlog::details::os::default_eol)));
}

TEST_CASE("to_hex_different_size_per_line", "[to_hex]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<xlog::sinks::ostream_sink_mt>(oss);
    xlog::logger oss_logger("oss", oss_sink);

    std::vector<unsigned char> v{9, 0xa, 0xb, 0x41, 0xc, 0x4b, 0xff, 0xff};

    oss_logger.info("{:Xsa}", xlog::to_hex(v, 10));
    REQUIRE(ends_with(oss.str(), "0000: 090A0B410C4BFFFF  ...A.K.." +
                                     std::string(xlog::details::os::default_eol)));

    oss_logger.info("{:Xs}", xlog::to_hex(v, 10));
    REQUIRE(ends_with(oss.str(),
                      "0000: 090A0B410C4BFFFF" + std::string(xlog::details::os::default_eol)));

    oss_logger.info("{:Xsa}", xlog::to_hex(v, 6));
    REQUIRE(ends_with(
        oss.str(), "0000: 090A0B410C4B  ...A.K" + std::string(xlog::details::os::default_eol) +
                       "0006: FFFF          .." + std::string(xlog::details::os::default_eol)));

    oss_logger.info("{:Xs}", xlog::to_hex(v, 6));
    REQUIRE(ends_with(oss.str(), "0000: 090A0B410C4B" +
                                     std::string(xlog::details::os::default_eol) + "0006: FFFF" +
                                     std::string(xlog::details::os::default_eol)));
}

TEST_CASE("to_hex_no_ascii", "[to_hex]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<xlog::sinks::ostream_sink_mt>(oss);
    xlog::logger oss_logger("oss", oss_sink);

    std::vector<unsigned char> v{9, 0xa, 0xb, 0x41, 0xc, 0x4b, 0xff, 0xff};
    oss_logger.info("{:Xs}", xlog::to_hex(v, 8));

    REQUIRE(ends_with(oss.str(),
                      "0000: 090A0B410C4BFFFF" + std::string(xlog::details::os::default_eol)));

    oss_logger.info("{:Xsna}", xlog::to_hex(v, 8));

    REQUIRE(
        ends_with(oss.str(), "090A0B410C4BFFFF" + std::string(xlog::details::os::default_eol)));
}
