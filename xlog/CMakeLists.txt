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
###########################################################################
# below are examples, after initialization, modify it to your own.
###########################################################################
###kmcmake_example
kmcmake_cc_library(
        NAMESPACE ${PROJECT_NAME}
        NAME xlog
        SOURCES
        internal/fnmatch.cc
        internal/conditions.cc
        internal/check_op.cc
        internal/strerror.cc
        internal/fs_helper.cc
        internal/append_file.cc
        internal/log_message.cc
        sinks/default_sink.cc
        sinks/ansi_color_sink.cc
        sinks/log_filename.cc
        sinks/rotating_file_sink.cc
        sinks/daily_file_sink.cc
        sinks/hourly_file_sink.cc
        format.cc
        log_sink.cc
        log_sink_set.cc
        initialize.cc
        utility.cc
        setup.cc

        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        PLINKS
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)
