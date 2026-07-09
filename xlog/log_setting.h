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

#include <xlog/common.h>

namespace xlog {
    inline int &log_level() {
        static int level = XLOG_LEVEL_INFO;
        return level;
    }

    inline int &vlog_level() {
        static int vlog = 0;
        return vlog;
    }

    // Returns true when verbose logging at the given level should be emitted.
    // VLOG macros log at TRACE severity; both global log_level and vlog_level
    // must permit the message.
    inline bool vlog_is_on(int verbose_level) {
        return log_level() <= XLOG_LEVEL_TRACE &&
               verbose_level <= vlog_level();
    }
} // namespace xlog

#define VXLOG_IS_ON(verbose_level) ::xlog::vlog_is_on(verbose_level)
#define TVLOG_IS_ON(verbose_level) ::xlog::vlog_is_on(verbose_level)
#define VZLOG_IS_ON(verbose_level) ::xlog::vlog_is_on(verbose_level)
