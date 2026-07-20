# Copyright (C) Kumo inc. and its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

configure_file(${PROJECT_SOURCE_DIR}/tests/config.h.in ${PROJECT_SOURCE_DIR}/tests/config.h @ONLY)

find_package(GTest REQUIRED)

kmcmake_cc_library(
        NAMESPACE xlog
        NAME test_support
        SOURCES
        scoped_mock_log.cc
        test_helpers.cc
        test_matchers.cc
        LINKS
        xlog::xlog_static
        GTest::gmock
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
        PLINKS
        xlog::xlog_static
        GTest::gmock
        GTest::gtest
        ${KMCMAKE_DEPS_LINK}
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)

kmcmake_cc_test(
        NAME fnmatch_test
        MODULE base
        SOURCES fnmatch_test.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        GTest::gmock
        GTest::gtest
        xlog::xlog_static
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAME scoped_mock_log_test
        MODULE log
        SOURCES scoped_mock_log_test.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        GTest::gmock
        GTest::gtest
        xlog::test_support_static
        xlog::xlog_static
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAME log_basic_test
        MODULE log
        SOURCES log_basic_test.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        GTest::gmock
        GTest::gtest
        xlog::test_support_static
        xlog::xlog_static
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAME check_test
        MODULE log
        SOURCES check_test.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        GTest::gmock
        GTest::gtest
        xlog::test_support_static
        xlog::xlog_static
        ${KMCMAKE_DEPS_LINK}
)

set(XLOG_LOG_TESTS
        log_format_test
        log_sink_test
        log_conditions_test
        log_modifier_methods_test
        log_api_extension_test
        strerror_test
        utility_test
        log_sink_file_test
        stderr_threshold_test
)

foreach (test ${XLOG_LOG_TESTS})
    kmcmake_cc_test(
            NAME ${test}
            MODULE log
            SOURCES ${test}.cc
            CXXOPTS ${KMCMAKE_CXX_OPTIONS}
            LINKS
            GTest::gmock
            GTest::gtest
            xlog::test_support_static
            xlog::xlog_static
            ${KMCMAKE_DEPS_LINK}
    )
endforeach ()

# Compile-time gates (separate TUs / defines).
kmcmake_cc_test(
        NAME min_log_level_test
        MODULE log
        SOURCES min_log_level_test.cc
        DEFINES XLOG_MIN_LOG_LEVEL=3
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        GTest::gmock
        GTest::gtest
        xlog::test_support_static
        xlog::xlog_static
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAME strip_log_test
        MODULE log
        SOURCES strip_log_test.cc
        DEFINES XLOG_STRIP_LOG=1
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        GTest::gmock
        GTest::gtest
        xlog::test_support_static
        xlog::xlog_static
        ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_test(
        NAME max_vlog_verbosity_test
        MODULE log
        SOURCES max_vlog_verbosity_test.cc
        DEFINES XLOG_MAX_VLOG_VERBOSITY=1
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS
        GTest::gmock
        GTest::gtest
        xlog::test_support_static
        xlog::xlog_static
        ${KMCMAKE_DEPS_LINK}
)
