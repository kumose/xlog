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

#include <xlog/internal/fnmatch.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(FNMatchTest, Works) {
  using xlog::log_internal::fn_match;
  EXPECT_THAT(fn_match("foo", "foo"), IsTrue());
  EXPECT_THAT(fn_match("foo", "bar"), IsFalse());
  EXPECT_THAT(fn_match("foo", "fo"), IsFalse());
  EXPECT_THAT(fn_match("foo", "foo2"), IsFalse());
  EXPECT_THAT(fn_match("bar/foo.ext", "bar/foo.ext"), IsTrue());
  EXPECT_THAT(fn_match("*ba*r/fo*o.ext*", "bar/foo.ext"), IsTrue());
  EXPECT_THAT(fn_match("bar/foo.ext", "bar/baz.ext"), IsFalse());
  EXPECT_THAT(fn_match("bar/foo.ext", "bar/foo"), IsFalse());
  EXPECT_THAT(fn_match("bar/foo.ext", "bar/foo.ext.zip"), IsFalse());
  EXPECT_THAT(fn_match("ba?/*.ext", "bar/foo.ext"), IsTrue());
  EXPECT_THAT(fn_match("ba?/*.ext", "baZ/FOO.ext"), IsTrue());
  EXPECT_THAT(fn_match("ba?/*.ext", "barr/foo.ext"), IsFalse());
  EXPECT_THAT(fn_match("ba?/*.ext", "bar/foo.ext2"), IsFalse());
  EXPECT_THAT(fn_match("ba?/*", "bar/foo.ext2"), IsTrue());
  EXPECT_THAT(fn_match("ba?/*", "bar/"), IsTrue());
  EXPECT_THAT(fn_match("ba?/?", "bar/"), IsFalse());
  EXPECT_THAT(fn_match("ba?/*", "bar"), IsFalse());
  EXPECT_THAT(fn_match("?x", "zx"), IsTrue());
  EXPECT_THAT(fn_match("*b", "aab"), IsTrue());
  EXPECT_THAT(fn_match("a*b", "aXb"), IsTrue());
  EXPECT_THAT(fn_match("", ""), IsTrue());
  EXPECT_THAT(fn_match("", "a"), IsFalse());
  EXPECT_THAT(fn_match("ab*", "ab"), IsTrue());
  EXPECT_THAT(fn_match("ab**", "ab"), IsTrue());
  EXPECT_THAT(fn_match("ab*?", "ab"), IsFalse());
  EXPECT_THAT(fn_match("*", "bbb"), IsTrue());
  EXPECT_THAT(fn_match("*", ""), IsTrue());
  EXPECT_THAT(fn_match("?", ""), IsFalse());
  EXPECT_THAT(fn_match("***", "**p"), IsTrue());
  EXPECT_THAT(fn_match("**", "*"), IsTrue());
  EXPECT_THAT(fn_match("*?", "*"), IsTrue());
}

}  // namespace
