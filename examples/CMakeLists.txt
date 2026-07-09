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

# Upstream demos (turbo/glog/absl) require external packages not currently available.
# Re-enable by installing the dependencies and uncommenting the section below.
# find_package(turbo QUIET)
# find_package(glog QUIET)
# find_package(absl QUIET CONFIG)
# ... (upstream demo targets follow)
