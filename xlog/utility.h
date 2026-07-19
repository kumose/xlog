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

#include <string_view>
#include <xlog/log_severity.h>

namespace xlog {

    // Unconditionally write a `message` to stderr. If `severity` exceeds kInfo
    // we also flush the stderr stream.
    void write_to_stderr(std::string_view message, LogSeverity severity);

    std::string_view get_thread_identify();

    void set_thread_identify(std::string_view thread_identify);

}  // namespace xlog

