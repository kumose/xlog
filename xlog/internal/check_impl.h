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

#pragma once

#include <xlog/internal/check_op.h>
#include <xlog/internal/conditions.h>
#include <xlog/internal/strip.h>

#define XLOG_INTERNAL_CHECK_IMPL(condition, condition_text) \
    XLOG_INTERNAL_CONDITION_FATAL(STATELESS, XLOG_UNLIKELY(!(condition))) \
    XLOG_INTERNAL_CHECK(condition_text).internal_stream()

#ifndef NDEBUG
#define XLOG_INTERNAL_DCHECK_IMPL(condition, condition_text) \
    XLOG_INTERNAL_CHECK_IMPL(condition, condition_text)
#else
#define XLOG_INTERNAL_DCHECK_IMPL(condition, condition_text) \
    XLOG_INTERNAL_CHECK_IMPL(true || (condition), "true")
#endif

#define XLOG_INTERNAL_CHECK_EQ_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_OP(Check_EQ, ==, val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_CHECK_NE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_OP(Check_NE, !=, val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_CHECK_LE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_OP(Check_LE, <=, val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_CHECK_LT_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_OP(Check_LT, <, val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_CHECK_GE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_OP(Check_GE, >=, val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_CHECK_GT_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_OP(Check_GT, >, val1, val1_text, val2, val2_text)

#ifndef NDEBUG
#define XLOG_INTERNAL_DCHECK_EQ_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_EQ_IMPL(val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_DCHECK_NE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_NE_IMPL(val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_DCHECK_LE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_LE_IMPL(val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_DCHECK_LT_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_LT_IMPL(val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_DCHECK_GE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_GE_IMPL(val1, val1_text, val2, val2_text)
#define XLOG_INTERNAL_DCHECK_GT_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_CHECK_GT_IMPL(val1, val1_text, val2, val2_text)
#else
#define XLOG_INTERNAL_DCHECK_EQ_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_DCHECK_NOP(val1, val2)
#define XLOG_INTERNAL_DCHECK_NE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_DCHECK_NOP(val1, val2)
#define XLOG_INTERNAL_DCHECK_LE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_DCHECK_NOP(val1, val2)
#define XLOG_INTERNAL_DCHECK_LT_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_DCHECK_NOP(val1, val2)
#define XLOG_INTERNAL_DCHECK_GE_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_DCHECK_NOP(val1, val2)
#define XLOG_INTERNAL_DCHECK_GT_IMPL(val1, val1_text, val2, val2_text) \
    XLOG_INTERNAL_DCHECK_NOP(val1, val2)
#endif

#define XLOG_INTERNAL_CHECK_STREQ_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STROP(strcmp, ==, true, s1, s1_text, s2, s2_text)
#define XLOG_INTERNAL_CHECK_STRNE_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STROP(strcmp, !=, false, s1, s1_text, s2, s2_text)
#define XLOG_INTERNAL_CHECK_STRCASEEQ_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STROP(strcasecmp, ==, true, s1, s1_text, s2, s2_text)
#define XLOG_INTERNAL_CHECK_STRCASENE_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STROP(strcasecmp, !=, false, s1, s1_text, s2, s2_text)

#ifndef NDEBUG
#define XLOG_INTERNAL_DCHECK_STREQ_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STREQ_IMPL(s1, s1_text, s2, s2_text)
#define XLOG_INTERNAL_DCHECK_STRNE_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STRNE_IMPL(s1, s1_text, s2, s2_text)
#define XLOG_INTERNAL_DCHECK_STRCASEEQ_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STRCASEEQ_IMPL(s1, s1_text, s2, s2_text)
#define XLOG_INTERNAL_DCHECK_STRCASENE_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_CHECK_STRCASENE_IMPL(s1, s1_text, s2, s2_text)
#else
#define XLOG_INTERNAL_DCHECK_STREQ_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_DCHECK_NOP(s1, s2)
#define XLOG_INTERNAL_DCHECK_STRNE_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_DCHECK_NOP(s1, s2)
#define XLOG_INTERNAL_DCHECK_STRCASEEQ_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_DCHECK_NOP(s1, s2)
#define XLOG_INTERNAL_DCHECK_STRCASENE_IMPL(s1, s1_text, s2, s2_text) \
    XLOG_INTERNAL_DCHECK_NOP(s1, s2)
#endif
