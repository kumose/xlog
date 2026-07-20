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

#include <cstdint>
#include <string>
#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/utility.h>

namespace {

    using ::testing::Eq;
    using ::testing::StartsWith;
    using ::xlog::hex_string;

    enum class Color : uint8_t { kRed = 0x0a };

    TEST(HexStringTest, Nullptr) {
        EXPECT_THAT(hex_string(nullptr), Eq("0x00"));
        const void *p = nullptr;
        EXPECT_THAT(hex_string(p), Eq("0x00"));
        int *ip = nullptr;
        EXPECT_THAT(hex_string(ip), Eq("0x00"));
    }

    TEST(HexStringTest, Pointer) {
        int x = 0;
        const std::string s = hex_string(&x);
        EXPECT_THAT(s, StartsWith("0x"));
        EXPECT_NE(s, "0x00");
    }

    TEST(HexStringTest, Integer) {
        EXPECT_THAT(hex_string(0), Eq("0x0"));
        EXPECT_THAT(hex_string(42u), Eq("0x2a"));
        EXPECT_THAT(hex_string(static_cast<uint8_t>(255)), Eq("0xff"));
        EXPECT_THAT(hex_string(true), Eq("0x1"));
        EXPECT_THAT(hex_string(false), Eq("0x0"));
    }

    TEST(HexStringTest, Enum) {
        EXPECT_THAT(hex_string(Color::kRed), Eq("0xa"));
    }

    TEST(HexStringTest, StringViewBytes) {
        EXPECT_THAT(hex_string(std::string_view("hi")), Eq("6869"));
        EXPECT_THAT(hex_string(std::string("AB")), Eq("4142"));
        EXPECT_THAT(hex_string(std::string_view{}), Eq(""));
    }

    TEST(HexStringTest, CharStarIsPointerNotDump) {
        const char *s = "hi";
        // Typed pointer overload → address, not "6869".
        EXPECT_THAT(hex_string(s), StartsWith("0x"));
        EXPECT_THAT(hex_string(std::string_view(s)), Eq("6869"));
    }

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
