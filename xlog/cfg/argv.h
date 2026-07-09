// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <xlog/cfg/helpers.h>
#include <xlog/details/registry.h>

//
// Init log levels using each argv entry that starts with "XLOG_LEVEL="
//
// set all loggers to debug level:
// example.exe "XLOG_LEVEL=debug"

// set logger1 to trace level
// example.exe "XLOG_LEVEL=logger1=trace"

// turn off all logging except for logger1 and logger2:
// example.exe "XLOG_LEVEL=off,logger1=debug,logger2=info"

namespace xlog {
namespace cfg {

// search for XLOG_LEVEL= in the args and use it to init the levels
inline void load_argv_levels(int argc, const char **argv) {
    const std::string spdlog_level_prefix = "XLOG_LEVEL=";
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.find(spdlog_level_prefix) == 0) {
            auto levels_string = arg.substr(spdlog_level_prefix.size());
            helpers::load_levels(levels_string);
        }
    }
}

inline void load_argv_levels(int argc, char **argv) {
    load_argv_levels(argc, const_cast<const char **>(argv));
}

}  // namespace cfg
}  // namespace xlog
