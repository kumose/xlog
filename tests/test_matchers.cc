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

#include "test_matchers.h"

#include <string>

#include <gmock/gmock.h>

namespace xlog {
namespace test {
namespace {

    using ::testing::Field;
    using ::testing::MakeMatcher;
    using ::testing::Matcher;
    using ::testing::MatcherInterface;
    using ::testing::MatchResultListener;
    using ::testing::ResultOf;

    class AsStringImpl final : public MatcherInterface<std::string_view> {
    public:
        explicit AsStringImpl(const Matcher<const std::string &> &str_matcher)
            : str_matcher_(str_matcher) {}

        bool MatchAndExplain(std::string_view actual,
                             MatchResultListener *listener) const override {
            return str_matcher_.MatchAndExplain(std::string(actual), listener);
        }

        void DescribeTo(std::ostream *os) const override {
            str_matcher_.DescribeTo(os);
        }

        void DescribeNegationTo(std::ostream *os) const override {
            str_matcher_.DescribeNegationTo(os);
        }

    private:
        const Matcher<const std::string &> str_matcher_;
    };

    std::string_view Basename(std::string_view path) {
        const auto slash = path.find_last_of("/\\");
        return slash == std::string_view::npos ? path : path.substr(slash + 1);
    }

    std::string_view BufferView(const LogEntry &e) {
        return std::string_view(e.buffer.data(), e.buffer.size());
    }

    std::string_view FormatView(const LogEntry &e) {
        return std::string_view(e.format_buffer.data(), e.format_buffer.size());
    }

}  // namespace

    Matcher<std::string_view> AsString(
        const Matcher<const std::string &> &str_matcher) {
        return MakeMatcher(new AsStringImpl(str_matcher));
    }

    Matcher<const LogEntry &> SourceFilename(
        const Matcher<std::string_view> &source_filename) {
        return Field(&LogEntry::filename, source_filename);
    }

    Matcher<const LogEntry &> SourceBasename(
        const Matcher<std::string_view> &source_basename) {
        return ResultOf([](const LogEntry &e) { return Basename(e.filename); },
                        source_basename);
    }

    Matcher<const LogEntry &> SourceLine(const Matcher<int> &source_line) {
        return Field(&LogEntry::line, source_line);
    }

    Matcher<const LogEntry &> Severity(const Matcher<LogSeverity> &severity) {
        return Field(&LogEntry::log_severity, severity);
    }

    Matcher<const LogEntry &> TextMessage(
        const Matcher<std::string_view> &text_message) {
        return ResultOf(&BufferView, text_message);
    }

    Matcher<const LogEntry &> FormattedMessage(
        const Matcher<std::string_view> &formatted) {
        return ResultOf(&FormatView, formatted);
    }

    Matcher<const LogEntry &> Verbosity(
        const Matcher<uint32_t> &verbosity) {
        return Field(&LogEntry::verbose_level, verbosity);
    }

}  // namespace test
}  // namespace xlog
