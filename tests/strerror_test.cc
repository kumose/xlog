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

#include <cerrno>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/internal/strerror.h>

namespace {

    using ::testing::AnyOf;
    using ::testing::Eq;
    using ::testing::HasSubstr;
    using ::testing::StartsWith;
    using ::xlog::log_internal::StrError;

    TEST(StrErrorTest, MatchesKnownErrno) {
        errno = ERANGE;
        EXPECT_THAT(StrError(ENOENT), Eq(std::strerror(ENOENT)));
        EXPECT_EQ(errno, ERANGE);  // must not clobber errno
    }

    TEST(StrErrorTest, InvalidErrno) {
        errno = ERANGE;
        const std::string s = StrError(-1);
        EXPECT_THAT(s, AnyOf(Eq("No error information"),  // musl
                             StartsWith("Unknown error"),
                             HasSubstr("Unknown")));
        EXPECT_EQ(errno, ERANGE);
    }

    TEST(StrErrorTest, Concurrent) {
        constexpr int kCodes = 64;
        std::vector<std::string> expected(kCodes);
        for (int i = 0; i < kCodes; ++i) {
            expected[static_cast<size_t>(i)] = std::strerror(i);
        }

        auto worker = [&]() {
            for (int i = 0; i < kCodes; ++i) {
                errno = EDOM;
                const std::string got = StrError(i);
                const int saved = errno;
                EXPECT_EQ(saved, EDOM);
                // Platform "unknown" strings need not match glibc strerror.
                if (got.rfind("Unknown error", 0) != 0 &&
                    got != "No error information") {
                    EXPECT_EQ(got, expected[static_cast<size_t>(i)]);
                }
            }
        };

        std::vector<std::thread> threads;
        for (int t = 0; t < 8; ++t) {
            threads.emplace_back(worker);
        }
        for (auto &th : threads) {
            th.join();
        }
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
