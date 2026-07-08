#
# Copyright 2023 The titan-search Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME demo
        SOURCES
        demo.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${PROJECT_NAME}::xlog
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME demo_1
        SOURCES
        demo_1.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${PROJECT_NAME}::xlog
        ${KMCMAKE_DEPS_LINK}
        PUBLIC
)

find_package(turbo REQUIRED)
find_package(glog REQUIRED)
find_package(absl REQUIRED CONFIG)

set(XLOG_ABSL_USED_TARGETS
        absl::absl_check
        absl::absl_log
        absl::algorithm
        absl::base
        absl::bind_front
        absl::bits
        absl::btree
        absl::cleanup
        absl::cord
        absl::core_headers
        absl::debugging
        absl::die_if_null
        absl::dynamic_annotations
        absl::flags
        absl::flat_hash_map
        absl::flat_hash_set
        absl::function_ref
        absl::hash
        absl::layout
        absl::log_initialize
        absl::log_severity
        absl::memory
        absl::node_hash_map
        absl::node_hash_set
        absl::optional
        absl::span
        absl::status
        absl::statusor
        absl::strings
        absl::synchronization
        absl::time
        absl::type_traits
        absl::utility
        absl::variant
)


kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME demo_upstream_turbo
        SOURCES
        demo_upstream_turbo.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        turbo::turbo_static
        glog::glog
        ${XLOG_ABSL_USED_TARGETS}
        ${PROJECT_NAME}::xlog
        ${KMCMAKE_SYSTEM_DYLINK}
        PUBLIC
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME demo_upstream_glog
        SOURCES
        demo_upstream_glog.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${PROJECT_NAME}::xlog
        glog::glog
        ${KMCMAKE_SYSTEM_DYLINK}
        PUBLIC
)

kmcmake_cc_binary(
        NAMESPACE ${PROJECT_NAME}
        NAME demo_upstream_absl
        SOURCES
        demo_upstream_absl.cc
        CXXOPTS
        ${KMCMAKE_CXX_OPTIONS}
        LINKS
        ${PROJECT_NAME}::xlog
        ${XLOG_ABSL_USED_TARGETS}
        ${KMCMAKE_SYSTEM_DYLINK}
        PUBLIC
)
