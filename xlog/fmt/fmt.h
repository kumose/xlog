//
// Copyright(c) 2016-2018 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

//
// Include a bundled header-only copy of fmtlib or an external one.
// By default spdlog include its own copy.
//
#include <xlog/tweakme.h>

#if defined(XLOG_USE_STD_FORMAT)  // XLOG_USE_STD_FORMAT is defined - use std::format
#include <format>
#elif !defined(XLOG_FMT_EXTERNAL)
#if !defined(XLOG_COMPILED_LIB) && !defined(FMT_HEADER_ONLY)
#define FMT_HEADER_ONLY
#endif
#ifndef FMT_USE_WINDOWS_H
#define FMT_USE_WINDOWS_H 0
#endif
#include <xlog/fmt/bundled/format.h>
#else  // XLOG_FMT_EXTERNAL is defined - use external fmtlib
#include <fmt/format.h>
#endif
