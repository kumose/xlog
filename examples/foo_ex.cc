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
// Minimal xlog usage: stream / fmt / printf / check.

#include <xlog/logging.h>

int main() {
    xlog::initialize_log();

    const int n = 42;
    XLOG(INFO) << "stream hello " << n;
    TLOG(WARNING, "fmt hello {}", n);
    ZLOG(ERROR, "printf hello %d", n);

    XCHECK_GT(n, 0) << "n must be positive";
    return 0;
}
