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

#if defined(_WIN32) || defined(__CYGWIN__)
#  if defined(XLOG_EXPORTS)
#    define XLOG_EXPORT __declspec(dllexport)
#  else
#    define XLOG_EXPORT __declspec(dllimport)
#  endif
#  define XLOG_NO_EXPORT
#else
#  if defined(XLOG_EXPORTS)
#    define XLOG_EXPORT __attribute__((visibility("default")))
#  else
#    define XLOG_EXPORT
#  endif
#  define XLOG_NO_EXPORT __attribute__((visibility("hidden")))
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define XLOG_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#  define XLOG_DEPRECATED __declspec(deprecated)
#else
#  define XLOG_DEPRECATED
#endif
