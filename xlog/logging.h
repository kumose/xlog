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
// Umbrella header for typical application use:
//
//   #include <xlog/logging.h>
//   xlog::initialize_log();
//   XLOG(INFO) << "hello";
//   XLOG(INFO).no_prefix() << "raw";
//   XPLOG(ERROR) << "open failed";
//   XVLOG(1) << "verbose";   // INFO if verbosity >= 1
//   XVLOG_EVERY_N(1, 100) << COUNTER;
//   TLOG(INFO, "x={}", 1);
//   ZLOG(INFO, "x=%d", 1);
//   XCHECK(ptr != nullptr);

#pragma once

#include <xlog/check.h>
#include <xlog/initialize.h>
#include <xlog/tlog.h>
#include <xlog/xlog.h>
#include <xlog/zlog.h>
