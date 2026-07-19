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

#include <string>
#include <cstdint>
#include <chrono>
#include <xlog/log_entry.h>

namespace xlog {

    /// One-shot civil time from system_clock (calendar + zone).
    struct CivilDay {
        uint16_t year{0};
        uint16_t month{0};
        uint8_t day{0};
        uint8_t hour{0};
        uint8_t minute{0};
        uint8_t second{0};
        uint64_t nanosecond{0};
        bool utc{false};
        int offset_seconds{0};  // east of UTC; 0 when utc
    };

    // Single localtime/gmtime; fills all fields including utc/offset.
    CivilDay to_civil_day(std::chrono::system_clock::time_point tp, bool utc = false);

    // With prefix:
    //   I2026-07-20T03:17:05.123456+08:00 13982(main) foo.cc:42] hello world
    //   W2026-07-20T03:17:05.123456+08:00 13982 bar.cc:108] disk almost full
    //   I2026-07-20T03:17:05.123456Z 13982 foo.cc:42] hello world   // utc
    //
    // Template:
    //   L{RFC3339} tid[(thread_identify)] file:line] message\n
    void xlog_format(LogEntry& entry);

}  // namespace xlog
