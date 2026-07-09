# Copyright (C) Kumo inc. and its affiliates.
# Author: Jeff.li lijippy@163.com
# All rights reserved.
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

set(xlog_test_sources
    test_misc
    test_async
    test_backtrace
    test_bin_to_hex
    test_cfg
    test_circular_q
    test_create_dir
    test_custom_callbacks
    test_daily_logger
    test_dup_filter
    test_errors
    test_eventlog
    test_file_helper
    test_file_logging
    test_fmt_helper
    test_macros
    test_mpmc_q
    test_pattern_formatter
    test_ringbuffer
    test_stdout_api
    test_stopwatch
    test_time_point
    test_timezone
    test_registry
    test_xlog_macros
)

foreach(test_name ${xlog_test_sources})
    kmcmake_cc_test(
        NAME ${test_name}
        MODULE ${PROJECT_NAME}
        SOURCES ${test_name}.cpp main.cpp utils.cpp
        LINKS ${PROJECT_NAME}_static
        DEPS ${PROJECT_NAME}_static
    )
endforeach()
