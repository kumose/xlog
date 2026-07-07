# Copyright (C) Kumo inc. and its affiliates.
# Author: Jeff.li lijippy@163.com
# All rights reserved.
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
