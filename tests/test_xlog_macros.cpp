#include "doctest.h"
#include "test_sink.h"

#include <xlog/xlog.h>
#include <xlog/logging.h>

using namespace xlog::sinks;

struct TestFixture {
    std::shared_ptr<test_sink_st> sink;
    std::shared_ptr<xlog::logger> logger;
    std::shared_ptr<xlog::logger> orig_logger;
    int saved_level;

    TestFixture() {
        saved_level = xlog::log_level();
        xlog::log_level() = XLOG_LEVEL_TRACE;
        sink = std::make_shared<test_sink_st>();
        logger = std::make_shared<xlog::logger>("test", sink);
        logger->set_pattern("%v");
        logger->set_level(xlog::level::trace);
        orig_logger = xlog::default_logger();
        xlog::set_default_logger(logger);
    }

    ~TestFixture() {
        xlog::set_default_logger(orig_logger);
        xlog::log_level() = saved_level;
    }
};

// ============================================================================
// XLOG — streaming style
// ============================================================================

TEST_CASE("XLOG basic streaming") {
    TestFixture f;

    XLOG(TRACE) << "trace msg";
    XLOG(DEBUG) << "debug " << 42;
    XLOG(INFO) << "info";
    XLOG(WARNING) << "warn " << 1.5;
    XLOG(ERROR) << "err";

    REQUIRE(f.sink->lines().size() == 5);
    REQUIRE(f.sink->lines()[0] == "trace msg");
    REQUIRE(f.sink->lines()[1] == "debug 42");
    REQUIRE(f.sink->lines()[2] == "info");
    REQUIRE(f.sink->lines()[3] == "warn 1.5");
    REQUIRE(f.sink->lines()[4] == "err");
}

TEST_CASE("XLOG_IF") {
    TestFixture f;

    XLOG_IF(INFO, true) << "yes";
    XLOG_IF(INFO, false) << "no";

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "yes");
}

TEST_CASE("XLOG level filter") {
    TestFixture f;
    f.logger->set_level(xlog::level::warn);

    XLOG(INFO) << "should not appear";
    XLOG(ERROR) << "should appear";

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "should appear");
}

// ============================================================================
// TLOG — format style
// ============================================================================

TEST_CASE("TLOG basic format") {
    TestFixture f;

    TLOG(INFO, "hello {}", "world");
    TLOG(DEBUG, "num {}", 42);
    TLOG(WARNING, "float {}", 1.5);

    REQUIRE(f.sink->lines().size() == 3);
    REQUIRE(f.sink->lines()[0] == "hello world");
    REQUIRE(f.sink->lines()[1] == "num 42");
    REQUIRE(f.sink->lines()[2] == "float 1.5");
}

TEST_CASE("TLOG_IF") {
    TestFixture f;

    TLOG_IF(INFO, true, "yes {}", 1);
    TLOG_IF(INFO, false, "no");

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "yes 1");
}

TEST_CASE("TLOG level filter") {
    TestFixture f;
    f.logger->set_level(xlog::level::warn);

    TLOG(INFO, "should not appear");
    TLOG(WARNING, "should appear {}", 1);

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "should appear 1");
}

TEST_CASE("TLOG_LEVEL dynamic severity") {
    TestFixture f;

    TLOG_LEVEL(xlog::level::info, "dynamic {}", 99);
    TLOG_LEVEL(xlog::level::warn, "warn msg");

    REQUIRE(f.sink->lines().size() == 2);
    REQUIRE(f.sink->lines()[0] == "dynamic 99");
    REQUIRE(f.sink->lines()[1] == "warn msg");
}

TEST_CASE("TVLOG verbose") {
    TestFixture f;

    auto saved = xlog::vlog_level();
    xlog::vlog_level() = 2;

    TVLOG(1, "pass {}", 1);
    TVLOG(3, "block");

    xlog::vlog_level() = saved;

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "pass 1");
}


