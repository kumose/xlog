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

#include "test_helpers.h"

#include <filesystem>
#include <fstream>
#include <string>

#include <gtest/gtest.h>
#include <xlog/initialize.h>
#include <xlog/log_sink_set.h>
#include <xlog/logging.h>
#include <xlog/sinks/log_filename.h>
#include <xlog/sinks/null_sink.h>
#include <xlog/sinks/rotating_file_sink.h>

namespace fs = std::filesystem;

namespace {

    class LogSinkFileTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
            xlog::set_min_log_level(xlog::LogSeverity::kSeverityInfo);
            dir_ = fs::temp_directory_path() /
                   ("xlog_sink_" + std::to_string(
                                       reinterpret_cast<uintptr_t>(this)));
            fs::remove_all(dir_);
            fs::create_directories(dir_);
        }

        void TearDown() override { fs::remove_all(dir_); }

        std::string ReadFile(const fs::path &p) {
            std::ifstream in(p);
            return std::string((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
        }

        fs::path dir_;
    };

    TEST_F(LogSinkFileTest, MakeDefaultLogFilename) {
        EXPECT_EQ(xlog::make_default_log_filename("/usr/bin/foo"),
                  "logs/foo_log.txt");
        EXPECT_EQ(xlog::make_default_log_filename("bar"), "logs/bar_log.txt");
    }

    TEST_F(LogSinkFileTest, SequentialPath) {
        xlog::BaseFilename base((dir_ / "app.log").string());
        EXPECT_EQ(xlog::sequential_log_path(base, 1),
                  (dir_ / "app_0001.log").string());
    }

    TEST_F(LogSinkFileTest, RotatingFileWrites) {
        const auto path = dir_ / "rot.log";
        auto sink = std::make_unique<xlog::RotatingFileSink>(
            path.string(), /*max_size=*/1024 * 1024, /*max_files=*/3);
        const uint32_t id = xlog::add_log_sink(std::move(sink));
        ASSERT_TRUE(xlog::set_default_sink(id));

        XLOG(INFO) << "hello-file";
        for (auto *s :
             xlog::LogSinkRegistry::instance().default_sink_set()->sinks()) {
            s->flush();
        }

        ASSERT_TRUE(fs::exists(path));
        const std::string body = ReadFile(path);
        EXPECT_NE(body.find("hello-file"), std::string::npos);
    }

    TEST_F(LogSinkFileTest, NullSinkCompiles) {
        xlog::NullSink sink;
        xlog::LogEntry e;
        e.log_severity = xlog::LogSeverity::kSeverityInfo;
        sink.send(e);
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
