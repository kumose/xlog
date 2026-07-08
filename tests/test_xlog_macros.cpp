#include "doctest.h"
#include "test_sink.h"

#include <xlog/xlog.h>
#include <xlog/logging.h>
#include <xlog/internal/check_op.h>

#if defined(__linux__) || defined(__APPLE__)
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#endif

using namespace xlog::sinks;

namespace {

struct TestFixture {
    std::shared_ptr<test_sink_st> sink;
    std::shared_ptr<xlog::logger> logger;
    std::shared_ptr<xlog::logger> orig_logger;
    int saved_level = XLOG_LEVEL_INFO;
    int saved_vlog = 0;

    TestFixture(const char *pattern = "%v") {
        saved_level = xlog::log_level();
        saved_vlog = xlog::vlog_level();
        xlog::log_level() = XLOG_LEVEL_TRACE;
        xlog::vlog_level() = 0;
        sink = std::make_shared<test_sink_st>();
        logger = std::make_shared<xlog::logger>("test", sink);
        logger->set_pattern(pattern);
        logger->set_level(xlog::level::trace);
        orig_logger = xlog::default_logger();
        xlog::set_default_logger(logger);
    }

    ~TestFixture() {
        xlog::set_default_logger(orig_logger);
        xlog::log_level() = saved_level;
        xlog::vlog_level() = saved_vlog;
    }
};

#if defined(__linux__) || defined(__APPLE__)
template <typename Fn>
bool run_in_child_expect_abort(Fn &&fn) {
    const pid_t pid = fork();
    if (pid == 0) {
        ::signal(SIGABRT, SIG_DFL);
        fn();
        _exit(1);
    }
    if (pid <= 0) {
        return false;
    }
    int status = 0;
    if (waitpid(pid, &status, 0) != pid) {
        return false;
    }
    return WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT;
}
#endif

}  // namespace

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

TEST_CASE("XLOG log_level macro filter") {
    TestFixture f;
    xlog::log_level() = XLOG_LEVEL_WARN;

    XLOG(INFO) << "should not appear";
    XLOG(ERROR) << "should appear";

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "should appear");
}

TEST_CASE("XLOG logger level filter") {
    TestFixture f;
    f.logger->set_level(xlog::level::warn);

    XLOG(INFO) << "should not appear";
    XLOG(ERROR) << "should appear";

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "should appear");
}

TEST_CASE("XLOG_EVERY_N") {
    TestFixture f;

    for (int i = 0; i < 5; ++i) {
        XLOG_EVERY_N(INFO, 2) << "tick";
    }

    REQUIRE(f.sink->lines().size() == 3);
    REQUIRE(f.sink->lines()[0] == "tick");
    REQUIRE(f.sink->lines()[1] == "tick");
    REQUIRE(f.sink->lines()[2] == "tick");
}

// ============================================================================
// TLOG — fmt style
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

TEST_CASE("TLOG log_level macro filter") {
    TestFixture f;
    xlog::log_level() = XLOG_LEVEL_WARN;

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

TEST_CASE("TVLOG verbose at TRACE") {
    TestFixture f("%L %v");

    xlog::vlog_level() = 2;

    TVLOG(1, "pass {}", 1);
    TVLOG(3, "block");

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "T pass 1");
}

TEST_CASE("TVLOG blocked when log_level above TRACE") {
    TestFixture f;

    xlog::log_level() = XLOG_LEVEL_INFO;
    xlog::vlog_level() = 99;

    TVLOG(1, "should not appear");

    REQUIRE(f.sink->lines().empty());
}

// ============================================================================
// ZLOG — printf style
// ============================================================================

TEST_CASE("ZLOG basic printf") {
    TestFixture f;

    ZLOG(INFO, "hello %s", "world");
    ZLOG(DEBUG, "num %d", 42);
    ZLOG(WARNING, "float %.1f", 1.5);

    REQUIRE(f.sink->lines().size() == 3);
    REQUIRE(f.sink->lines()[0] == "hello world");
    REQUIRE(f.sink->lines()[1] == "num 42");
    REQUIRE(f.sink->lines()[2] == "float 1.5");
}

TEST_CASE("ZLOG_IF") {
    TestFixture f;

    ZLOG_IF(INFO, true, "yes %d", 1);
    ZLOG_IF(INFO, false, "no %d", 1);

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "yes 1");
}

TEST_CASE("ZLOG log_level macro filter") {
    TestFixture f;
    xlog::log_level() = XLOG_LEVEL_WARN;

    ZLOG(INFO, "should not appear");
    ZLOG(WARNING, "should appear %d", 1);

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "should appear 1");
}

TEST_CASE("ZLOG_LEVEL dynamic severity") {
    TestFixture f;

    ZLOG_LEVEL(xlog::level::info, "dynamic %d", 99);
    ZLOG_LEVEL(xlog::level::warn, "warn msg");

    REQUIRE(f.sink->lines().size() == 2);
    REQUIRE(f.sink->lines()[0] == "dynamic 99");
    REQUIRE(f.sink->lines()[1] == "warn msg");
}

TEST_CASE("VZLOG verbose at TRACE") {
    TestFixture f("%L %v");

    xlog::vlog_level() = 2;

    VZLOG(1, "pass %d", 1);
    VZLOG(3, "block %d", 3);

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "T pass 1");
}

// ============================================================================
// VXLOG — stream verbose
// ============================================================================

TEST_CASE("VXLOG verbose at TRACE") {
    TestFixture f("%L %v");

    xlog::vlog_level() = 2;

    VXLOG(1) << "pass " << 1;
    VXLOG(3) << "block";

    REQUIRE(f.sink->lines().size() == 1);
    REQUIRE(f.sink->lines()[0] == "T pass 1");
}

TEST_CASE("vlog_is_on") {
    xlog::log_level() = XLOG_LEVEL_TRACE;
    xlog::vlog_level() = 2;

    REQUIRE(xlog::vlog_is_on(1));
    REQUIRE(xlog::vlog_is_on(2));
    REQUIRE_FALSE(xlog::vlog_is_on(3));

    xlog::log_level() = XLOG_LEVEL_INFO;
    REQUIRE_FALSE(xlog::vlog_is_on(1));
}

// ============================================================================
// CHECK helpers
// ============================================================================

TEST_CASE("check_op comparison helpers") {
    {
        std::unique_ptr<std::string> msg(
            xlog::internal::Check_EQImpl(1, 2, "1 == 2"));
        REQUIRE(msg != nullptr);
        REQUIRE(*msg == "Check failed: 1 == 2 (1 vs. 2)");
    }
    REQUIRE(xlog::internal::Check_EQImpl(3, 3, "3 == 3") == nullptr);

    {
        std::unique_ptr<std::string> msg(
            xlog::internal::CheckstrcmptrueImpl("abc", "xyz", "a == b"));
        REQUIRE(msg != nullptr);
    }
    REQUIRE(xlog::internal::CheckstrcmptrueImpl("same", "same", "s == s") == nullptr);
}

TEST_CASE("CHECK pass without abort") {
    TestFixture f;

    XCHECK(true);
    XCHECK_EQ(1, 1);
    XCHECK_STREQ("a", "a");
    XCHECK_NOTNULL(f.sink.get());

    TCHECK(true);
    TCHECK(true, "ok {}", 1);
    TCHECK_EQ(2, 2);
    TCHECK_NOTNULL(f.sink.get());

    ZCHECK(true);
    ZCHECK(true, "ok %d", 1);
    ZCHECK_EQ(3, 3);
    ZCHECK_NOTNULL(f.sink.get());

    REQUIRE(f.sink->lines().empty());
}

#if defined(__linux__) || defined(__APPLE__)

TEST_CASE("CHECK fail aborts process") {
    TestFixture f;

    REQUIRE(run_in_child_expect_abort([] {
        TestFixture child;
        XCHECK(false) << "boom";
    }));

    REQUIRE(run_in_child_expect_abort([] {
        TestFixture child;
        TCHECK(false, "fmt {}", 1);
    }));

    REQUIRE(run_in_child_expect_abort([] {
        TestFixture child;
        ZCHECK(false, "printf %d", 1);
    }));

    REQUIRE(run_in_child_expect_abort([] {
        TestFixture child;
        TCHECK_EQ(1, 2);
    }));

    REQUIRE(f.sink->lines().empty());
}

#ifndef NDEBUG

TEST_CASE("DXCHECK stripped in child on failure only in debug") {
    REQUIRE(run_in_child_expect_abort([] {
        TestFixture child;
        DXCHECK(false);
    }));
}

TEST_CASE("DXCHECK pass in debug") {
    TestFixture f;
    DXCHECK(true);
    DXCHECK_EQ(1, 1);
    REQUIRE(f.sink->lines().empty());
}

#else

TEST_CASE("DXCHECK no-op in release") {
    TestFixture f;
    DXCHECK(false);
    DTCHECK(false);
    DZCHECK(false, "nope %d", 1);
    REQUIRE(f.sink->lines().empty());
}

#endif  // NDEBUG

#endif  // __linux__ || __APPLE__
