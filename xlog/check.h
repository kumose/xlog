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
// Fatal checks (XCHECK*). Always evaluate (except DXCHECK* under NDEBUG).
//
//   XCHECK(ptr != nullptr) << "need ptr";
//   XCHECK_EQ(a, b) << "mismatch";
//   XCHECK_STREQ(s1, s2);
//   auto* p = XCHECK_NOTNULL(MaybeNull());

#pragma once

#include <utility>

#include <xlog/internal/check_impl.h>
#include <xlog/internal/check_op.h>
#include <xlog/internal/log_message.h>

#define XCHECK(condition) \
    XLOG_INTERNAL_CHECK_IMPL((condition), #condition)

#define DXCHECK(condition) \
    XLOG_INTERNAL_DCHECK_IMPL((condition), #condition)

#define XCHECK_EQ(val1, val2) \
    XLOG_INTERNAL_CHECK_EQ_IMPL((val1), #val1, (val2), #val2)
#define XCHECK_NE(val1, val2) \
    XLOG_INTERNAL_CHECK_NE_IMPL((val1), #val1, (val2), #val2)
#define XCHECK_LE(val1, val2) \
    XLOG_INTERNAL_CHECK_LE_IMPL((val1), #val1, (val2), #val2)
#define XCHECK_LT(val1, val2) \
    XLOG_INTERNAL_CHECK_LT_IMPL((val1), #val1, (val2), #val2)
#define XCHECK_GE(val1, val2) \
    XLOG_INTERNAL_CHECK_GE_IMPL((val1), #val1, (val2), #val2)
#define XCHECK_GT(val1, val2) \
    XLOG_INTERNAL_CHECK_GT_IMPL((val1), #val1, (val2), #val2)

#define DXCHECK_EQ(val1, val2) \
    XLOG_INTERNAL_DCHECK_EQ_IMPL((val1), #val1, (val2), #val2)
#define DXCHECK_NE(val1, val2) \
    XLOG_INTERNAL_DCHECK_NE_IMPL((val1), #val1, (val2), #val2)
#define DXCHECK_LE(val1, val2) \
    XLOG_INTERNAL_DCHECK_LE_IMPL((val1), #val1, (val2), #val2)
#define DXCHECK_LT(val1, val2) \
    XLOG_INTERNAL_DCHECK_LT_IMPL((val1), #val1, (val2), #val2)
#define DXCHECK_GE(val1, val2) \
    XLOG_INTERNAL_DCHECK_GE_IMPL((val1), #val1, (val2), #val2)
#define DXCHECK_GT(val1, val2) \
    XLOG_INTERNAL_DCHECK_GT_IMPL((val1), #val1, (val2), #val2)

#define XCHECK_STREQ(s1, s2) \
    XLOG_INTERNAL_CHECK_STREQ_IMPL((s1), #s1, (s2), #s2)
#define XCHECK_STRNE(s1, s2) \
    XLOG_INTERNAL_CHECK_STRNE_IMPL((s1), #s1, (s2), #s2)
#define XCHECK_STRCASEEQ(s1, s2) \
    XLOG_INTERNAL_CHECK_STRCASEEQ_IMPL((s1), #s1, (s2), #s2)
#define XCHECK_STRCASENE(s1, s2) \
    XLOG_INTERNAL_CHECK_STRCASENE_IMPL((s1), #s1, (s2), #s2)

#define DXCHECK_STREQ(s1, s2) \
    XLOG_INTERNAL_DCHECK_STREQ_IMPL((s1), #s1, (s2), #s2)
#define DXCHECK_STRNE(s1, s2) \
    XLOG_INTERNAL_DCHECK_STRNE_IMPL((s1), #s1, (s2), #s2)
#define DXCHECK_STRCASEEQ(s1, s2) \
    XLOG_INTERNAL_DCHECK_STRCASEEQ_IMPL((s1), #s1, (s2), #s2)
#define DXCHECK_STRCASENE(s1, s2) \
    XLOG_INTERNAL_DCHECK_STRCASENE_IMPL((s1), #s1, (s2), #s2)

namespace xlog {
namespace log_internal {

    template <typename T>
    T CheckNotNull(const char *file, int line, const char *names, T &&t) {
        if (t == nullptr) {
            LogMessage(file, line, names);
        }
        return std::forward<T>(t);
    }

}  // namespace log_internal
}  // namespace xlog

#define XCHECK_NOTNULL(val)                                                  \
    ::xlog::log_internal::CheckNotNull(__FILE__, __LINE__,                   \
                                       "'" #val "' Must be non nullptr",     \
                                       (val))

#define XLOG_UNREACHABLE() XCHECK(false)
