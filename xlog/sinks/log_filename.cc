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

#include <xlog/sinks/log_filename.h>
#include <xlog/format.h>

#include <filesystem>
#include <fmt/format.h>

namespace xlog {

    BaseFilename::BaseFilename(std::string_view filename) {
        const std::filesystem::path path{std::string(filename)};
        directory = path.has_parent_path() ? path.parent_path().string() : "";
        basename = path.stem().string();
        extension = path.extension().string();
    }

    namespace {
        std::string Join(const BaseFilename &base, std::string_view stem_suffix) {
            const std::string filename =
                fmt::format("{}{}{}", base.basename, stem_suffix, base.extension);
            if (base.directory.empty()) {
                return filename;
            }
            return (std::filesystem::path(base.directory) / filename).string();
        }
    }  // namespace

    std::string sequential_log_path(const BaseFilename &base, int64_t seq) {
        return Join(base, fmt::format("_{:04}", seq));
    }

    std::string daily_log_path(const BaseFilename &base,
                               std::chrono::system_clock::time_point tp,
                               bool utc) {
        const CivilDay c = to_civil_day(tp, utc);
        return Join(base, fmt::format("_{:04}-{:02}-{:02}", c.year, c.month,
                                      c.day));
    }

    std::string hourly_log_path(const BaseFilename &base,
                                std::chrono::system_clock::time_point tp,
                                bool utc) {
        const CivilDay c = to_civil_day(tp, utc);
        return Join(base, fmt::format("_{:04}-{:02}-{:02}-{:02}", c.year,
                                      c.month, c.day, c.hour));
    }

}  // namespace xlog
