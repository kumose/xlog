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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#  define XLOG_OS_WINDOWS
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#  define XLOG_OS_CYGWIN
#elif defined(linux) || defined(__linux) || defined(__linux__)
#  define XLOG_OS_LINUX
#  if defined(__ANDROID__)
#    define XLOG_OS_ANDROID
#  endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#  define XLOG_OS_MACOSX
#elif defined(__FreeBSD__)
#  define XLOG_OS_FREEBSD
#elif defined(__NetBSD__)
#  define XLOG_OS_NETBSD
#elif defined(__OpenBSD__)
#  define XLOG_OS_OPENBSD
#elif defined(__EMSCRIPTEN__)
#  define XLOG_OS_EMSCRIPTEN
#else
// TODO(hamaji): Add other platforms.
#error Platform not supported by glog. Please consider to contribute platform information by submitting a pull request on Github.
#endif
