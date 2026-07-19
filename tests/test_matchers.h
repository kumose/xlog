// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Minimal LogEntry matchers for xlog tests.

#pragma once

#include <cstdint>
#include <string_view>

#include <gmock/gmock.h>
#include <xlog/log_entry.h>
#include <xlog/log_severity.h>

namespace xlog {
namespace test {

    ::testing::Matcher<std::string_view> AsString(
        const ::testing::Matcher<const std::string &> &str_matcher);

    ::testing::Matcher<const LogEntry &> SourceFilename(
        const ::testing::Matcher<std::string_view> &source_filename);

    ::testing::Matcher<const LogEntry &> SourceBasename(
        const ::testing::Matcher<std::string_view> &source_basename);

    ::testing::Matcher<const LogEntry &> SourceLine(
        const ::testing::Matcher<int> &source_line);

    ::testing::Matcher<const LogEntry &> Severity(
        const ::testing::Matcher<LogSeverity> &severity);

    // Matches entry.buffer (payload before prefix formatting).
    ::testing::Matcher<const LogEntry &> TextMessage(
        const ::testing::Matcher<std::string_view> &text_message);

    // Matches entry.format_buffer (full formatted line).
    ::testing::Matcher<const LogEntry &> FormattedMessage(
        const ::testing::Matcher<std::string_view> &formatted);

    ::testing::Matcher<const LogEntry &> Verbosity(
        const ::testing::Matcher<uint32_t> &verbosity);

}  // namespace test
}  // namespace xlog
