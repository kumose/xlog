// Copyright (C) 2026 Kumo Inc. and its affiliates. All Rights Reserved.
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

#include <xlog/internal/fnmatch.h>

#include <gtest/gtest.h>

namespace {

TEST(FNMatchTest, Works) {
  using xlog::log_internal::fn_match;
  EXPECT_TRUE(fn_match("foo", "foo"));
  EXPECT_FALSE(fn_match("foo", "bar"));
  EXPECT_FALSE(fn_match("foo", "fo"));
  EXPECT_FALSE(fn_match("foo", "foo2"));
  EXPECT_TRUE(fn_match("bar/foo.ext", "bar/foo.ext"));
  EXPECT_TRUE(fn_match("*ba*r/fo*o.ext*", "bar/foo.ext"));
  EXPECT_FALSE(fn_match("bar/foo.ext", "bar/baz.ext"));
  EXPECT_FALSE(fn_match("bar/foo.ext", "bar/foo"));
  EXPECT_FALSE(fn_match("bar/foo.ext", "bar/foo.ext.zip"));
  EXPECT_TRUE(fn_match("ba?/*.ext", "bar/foo.ext"));
  EXPECT_TRUE(fn_match("ba?/*.ext", "baZ/FOO.ext"));
  EXPECT_FALSE(fn_match("ba?/*.ext", "barr/foo.ext"));
  EXPECT_FALSE(fn_match("ba?/*.ext", "bar/foo.ext2"));
  EXPECT_TRUE(fn_match("ba?/*", "bar/foo.ext2"));
  EXPECT_TRUE(fn_match("ba?/*", "bar/"));
  EXPECT_FALSE(fn_match("ba?/?", "bar/"));
  EXPECT_FALSE(fn_match("ba?/*", "bar"));
  EXPECT_TRUE(fn_match("?x", "zx"));
  EXPECT_TRUE(fn_match("*b", "aab"));
  EXPECT_TRUE(fn_match("a*b", "aXb"));
  EXPECT_TRUE(fn_match("", ""));
  EXPECT_FALSE(fn_match("", "a"));
  EXPECT_TRUE(fn_match("ab*", "ab"));
  EXPECT_TRUE(fn_match("ab**", "ab"));
  EXPECT_FALSE(fn_match("ab*?", "ab"));
  EXPECT_TRUE(fn_match("*", "bbb"));
  EXPECT_TRUE(fn_match("*", ""));
  EXPECT_FALSE(fn_match("?", ""));
  EXPECT_TRUE(fn_match("***", "**p"));
  EXPECT_TRUE(fn_match("**", "*"));
  EXPECT_TRUE(fn_match("*?", "*"));
}

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
