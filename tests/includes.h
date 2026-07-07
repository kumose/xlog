#pragma once

#if defined(__GNUC__) && __GNUC__ == 12
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"  // Workaround for GCC 12
#endif
#include <catch2/catch_all.hpp>
#if defined(__GNUC__) && __GNUC__ == 12
#pragma GCC diagnostic pop
#endif

#include "utils.h"
#include <chrono>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <stdlib.h>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#undef SPDLOG_LEVEL_NAMES
#undef SPDLOG_SHORT_LEVEL_NAMES

#include <xlog/xlog.h>
#include <xlog/async.h>
#include <xlog/details/fmt_helper.h>
#include <xlog/details/os.h>

#ifndef SPDLOG_NO_TLS
#include <xlog/mdc.h>
#endif

#include <xlog/sinks/basic_file_sink.h>
#include <xlog/sinks/daily_file_sink.h>
#include <xlog/sinks/null_sink.h>
#include <xlog/sinks/ostream_sink.h>
#include <xlog/sinks/rotating_file_sink.h>
#include <xlog/sinks/stdout_color_sinks.h>
#include <xlog/sinks/msvc_sink.h>
#include <xlog/pattern_formatter.h>
