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

configure_file(${PROJECT_SOURCE_DIR}/benchmark/config.h.in ${PROJECT_SOURCE_DIR}/benchmark/config.h @ONLY)

find_package(benchmark REQUIRED)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME bench
        SOURCES bench.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS ${PROJECT_NAME}::xlog ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME async_bench
        SOURCES async_bench.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS ${PROJECT_NAME}::xlog ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

kmcmake_cc_benchmark(
        NAME latency
        MODULE ${PROJECT_NAME}
        SOURCES latency.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS ${PROJECT_NAME}::xlog benchmark::benchmark ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_benchmark(
        NAME formatter_bench
        MODULE ${PROJECT_NAME}
        SOURCES formatter-bench.cc
        CXXOPTS ${KMCMAKE_CXX_OPTIONS}
        LINKS ${PROJECT_NAME}::xlog benchmark::benchmark ${KMCMAKE_DEPS_LINK}
        SKIP
)
