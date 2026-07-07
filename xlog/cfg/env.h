// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <xlog/cfg/helpers.h>
#include <xlog/details/os.h>
#include <xlog/details/registry.h>

//
// Init levels and patterns from env variables XLOG_LEVEL
// Inspired from Rust's "env_logger" crate (https://crates.io/crates/env_logger).
// Note - fallback to "info" level on unrecognized levels
//
// Examples:
//
// set global level to debug:
// export XLOG_LEVEL=debug
//
// turn off all logging except for logger1:
// export XLOG_LEVEL="*=off,logger1=debug"
//

// turn off all logging except for logger1 and logger2:
// export XLOG_LEVEL="off,logger1=debug,logger2=info"

namespace xlog {
namespace cfg {
inline void load_env_levels(const char* var = "XLOG_LEVEL") {
    auto env_val = details::os::getenv(var);
    if (!env_val.empty()) {
        helpers::load_levels(env_val);
    }
}

}  // namespace cfg
}  // namespace xlog
