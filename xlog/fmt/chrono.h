//
// Copyright(c) 2016 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once
//
// include bundled or external copy of fmtlib's chrono support
//
#include <xlog/tweakme.h>

#if !defined(XLOG_USE_STD_FORMAT)
#if !defined(XLOG_FMT_EXTERNAL)
#include <xlog/fmt/bundled/chrono.h>
#else
#include <fmt/chrono.h>
#endif
#endif
