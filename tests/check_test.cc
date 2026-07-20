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
// XCHECK* coverage (adapted from turbo tests/log/check_test; no QCHECK/OK).

#include "test_helpers.h"

#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <xlog/check.h>
#include <xlog/initialize.h>

namespace {

    using ::testing::AllOf;
    using ::testing::HasSubstr;
    using ::testing::Not;

#if GTEST_HAS_DEATH_TEST
    using ::xlog::test::DiedOfFatal;
#endif

    class CheckTest : public ::testing::Test {
    protected:
        void SetUp() override {
            if (!xlog::is_initialized()) {
                xlog::initialize_log();
            }
        }
    };

#if GTEST_HAS_DEATH_TEST

    TEST_F(CheckTest, BasicValues) {
        XCHECK(true);

        EXPECT_DEATH(XCHECK(false), HasSubstr("Check failed: false"));

        int i = 2;
        XCHECK(i != 3);
    }

#endif

    TEST_F(CheckTest, LogicExpressions) {
        int i = 5;
        XCHECK(i > 0 && i < 10);
        XCHECK(i < 0 || i > 3);
    }

    TEST_F(CheckTest, PlacementsInCompoundStatements) {
        if (true) XCHECK(true);

        if (false)
            ;  // NOLINT
        else
            XCHECK(true);

        switch (0)
        case 0:
            XCHECK(true);
    }

    TEST_F(CheckTest, BoolConvertible) {
        struct Tester {
        } tester;
        XCHECK([&]() { return &tester; }());
    }

#if GTEST_HAS_DEATH_TEST

    TEST_F(CheckTest, ChecksWithSideEffects) {
        int var = 0;
        XCHECK([&var]() {
            ++var;
            return true;
        }());
        EXPECT_EQ(var, 1);

        EXPECT_DEATH(XCHECK([&var]() {
                         ++var;
                         return false;
                     }()) << var,
                     AllOf(HasSubstr("Check failed:"), HasSubstr("2")));
    }

#endif

    template <int a, int b>
    constexpr int sum() {
        return a + b;
    }
#define MACRO_ONE 1
#define TEMPLATE_SUM(a, b) sum<a, b>()
#define CONCAT(a, b) a b
#define IDENTITY(x) x

    TEST_F(CheckTest, PassingMacroExpansion) {
        XCHECK(IDENTITY(true));
        XCHECK_EQ(TEMPLATE_SUM(MACRO_ONE, 2), 3);
        XCHECK_STREQ(CONCAT("x", "y"), "xy");
    }

#if GTEST_HAS_DEATH_TEST

    TEST_F(CheckTest, MacroExpansionInMessage) {
        EXPECT_DEATH(XCHECK(IDENTITY(false)), HasSubstr("IDENTITY(false)"));
    }

    TEST_F(CheckTest, MacroExpansionCompare) {
        EXPECT_DEATH(XCHECK_EQ(IDENTITY(false), IDENTITY(true)),
                     HasSubstr("IDENTITY(false) == IDENTITY(true)"));
        EXPECT_DEATH(XCHECK_GT(IDENTITY(1), IDENTITY(2)),
                     HasSubstr("IDENTITY(1) > IDENTITY(2)"));
    }

    TEST_F(CheckTest, MacroExpansionStrCompare) {
        EXPECT_DEATH(XCHECK_STREQ(IDENTITY("x"), IDENTITY("y")),
                     HasSubstr("IDENTITY(\"x\") == IDENTITY(\"y\")"));
        EXPECT_DEATH(XCHECK_STRCASENE(IDENTITY("a"), IDENTITY("A")),
                     HasSubstr("IDENTITY(\"a\") != IDENTITY(\"A\")"));
    }

#endif

#undef TEMPLATE_SUM
#undef CONCAT
#undef MACRO_ONE
#undef IDENTITY

    TEST_F(CheckTest, BinaryChecksWithPrimitives) {
        XCHECK_EQ(1, 1);
        XCHECK_NE(1, 2);
        XCHECK_GE(1, 1);
        XCHECK_GE(2, 1);
        XCHECK_LE(1, 1);
        XCHECK_LE(1, 2);
        XCHECK_GT(2, 1);
        XCHECK_LT(1, 2);
    }

    enum { CASE_A, CASE_B };

    TEST_F(CheckTest, BinaryChecksWithEnumValues) {
        XCHECK_EQ(CASE_A, CASE_A);
        XCHECK_NE(CASE_A, CASE_B);
        XCHECK_GE(CASE_A, CASE_A);
        XCHECK_GE(CASE_B, CASE_A);
        XCHECK_LE(CASE_A, CASE_A);
        XCHECK_LE(CASE_A, CASE_B);
        XCHECK_GT(CASE_B, CASE_A);
        XCHECK_LT(CASE_A, CASE_B);
    }

    TEST_F(CheckTest, BinaryChecksWithNullptr) {
        const void *p_null = nullptr;
        const void *p_not_null = &p_null;
        XCHECK_EQ(p_null, nullptr);
        XCHECK_EQ(nullptr, p_null);
        XCHECK_NE(p_not_null, nullptr);
        XCHECK_NE(nullptr, p_not_null);
    }

#if GTEST_HAS_DEATH_TEST

    TEST_F(CheckTest, ComparingCharsValues) {
        {
            char a = ';';
            char b = 'b';
            EXPECT_DEATH(XCHECK_EQ(a, b),
                         HasSubstr("Check failed: a == b (';' vs. 'b')"));
            b = 1;
            EXPECT_DEATH(
                XCHECK_EQ(a, b),
                HasSubstr("Check failed: a == b (';' vs. char value 1)"));
        }
        {
            signed char a = ';';
            signed char b = 'b';
            EXPECT_DEATH(XCHECK_EQ(a, b),
                         HasSubstr("Check failed: a == b (';' vs. 'b')"));
            b = -128;
            EXPECT_DEATH(XCHECK_EQ(a, b),
                         HasSubstr("Check failed: a == b (';' vs. signed char "
                                   "value -128)"));
        }
        {
            unsigned char a = ';';
            unsigned char b = 'b';
            EXPECT_DEATH(XCHECK_EQ(a, b),
                         HasSubstr("Check failed: a == b (';' vs. 'b')"));
            b = 128;
            EXPECT_DEATH(XCHECK_EQ(a, b),
                         HasSubstr("Check failed: a == b (';' vs. unsigned "
                                   "char value 128)"));
        }
    }

    TEST_F(CheckTest, NullValuesAreReportedCleanly) {
        const char *a = nullptr;
        const char *b = nullptr;
        EXPECT_DEATH(XCHECK_NE(a, b),
                     HasSubstr("Check failed: a != b ((null) vs. (null))"));

        a = "xx";
        EXPECT_DEATH(XCHECK_EQ(a, b),
                     HasSubstr("Check failed: a == b (xx vs. (null))"));
        EXPECT_DEATH(XCHECK_EQ(b, a),
                     HasSubstr("Check failed: b == a ((null) vs. xx)"));
    }

#endif

    TEST_F(CheckTest, STREQ) {
        XCHECK_STREQ("this", "this");
        XCHECK_STREQ(nullptr, nullptr);
        XCHECK_STRCASEEQ("this", "tHiS");
        XCHECK_STRCASEEQ(nullptr, nullptr);
        XCHECK_STRNE("this", "tHiS");
        XCHECK_STRNE("this", nullptr);
        XCHECK_STRCASENE("this", "that");
        XCHECK_STRCASENE(nullptr, "that");
        XCHECK_STREQ((std::string("a") + "b").c_str(), "ab");
    }

    TEST_F(CheckTest, ComparisonPlacementsInCompoundStatements) {
        if (true) XCHECK_EQ(1, 1);
        if (true) XCHECK_STREQ("c", "c");

        if (false)
            ;  // NOLINT
        else
            XCHECK_LE(0, 1);

        if (false)
            ;  // NOLINT
        else
            XCHECK_STRNE("a", "b");

        switch (0)
        case 0:
            XCHECK_NE(1, 0);

        switch (0)
        case 0:
            XCHECK_STRCASEEQ("A", "a");
    }

    TEST_F(CheckTest, DCHECK) {
#ifdef NDEBUG
        DXCHECK(1 == 2) << "should be compiled away";
#endif
        DXCHECK(1 == 1);
        DXCHECK_EQ(1, 1);
        DXCHECK_NE(1, 2);
        DXCHECK_GE(1, 1);
        DXCHECK_LE(1, 2);
        DXCHECK_GT(2, 1);
        DXCHECK_LT(1, 2);
        DXCHECK_STREQ("a", "a");
    }

    TEST_F(CheckTest, NotNull) {
        int x = 0;
        int *p = XCHECK_NOTNULL(&x);
        EXPECT_EQ(p, &x);
    }

#if GTEST_HAS_DEATH_TEST

    TEST_F(CheckTest, NotNullDeath) {
        int *p = nullptr;
        EXPECT_DEATH(XCHECK_NOTNULL(p), HasSubstr("Must be non nullptr"));
    }

    TEST_F(CheckTest, Unreachable) {
        EXPECT_EXIT(XLOG_UNREACHABLE(), DiedOfFatal,
                    HasSubstr("Check failed: false"));
    }

#endif

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new xlog::test::LogTestEnvironment);
    return RUN_ALL_TESTS();
}
