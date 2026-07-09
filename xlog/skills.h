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
// AI: This file is a human/AI-readable summary of the project's public API and
// AI: design principles. AI agents should read this instead of scanning all
// AI: source files. For dependency libraries, read their skills.h similarly.
//
// AI: Style: triple-slash Doxygen comments for all entries.

#pragma once

/// @defgroup project_summary Project Summary
/// @brief xlog — C++ logging library with XLOG/TLOG/ZLOG macro families

/// @brief Project name
/// AI: xlog

/// @brief Description
/// AI: xlog is a C++ logging library with three macro families:
/// AI:   XLOG (stream <<), TLOG (fmt {}), ZLOG (printf %)
/// AI: Each family has _IF / _EVERY_N / _ONCE / _FIRST_N / _LEVEL / _EVERY_POW_2 / _EVERY_N_SEC variants,
/// AI: XCHECK/TCHECK/ZCHECK assertions, debug-only D* variants (release no-op),
/// AI: runtime level control, and optional upstream adapters for glog/turbo/abseil.

/// @brief Quick API (alias — use xlog/logging.h for macros, xlog/xlog.h for logger API)
/// AI: --- Log level ---
/// AI:   xlog::log_level()  = XLOG_LEVEL_TRACE | DEBUG | INFO | WARN | ERROR | FATAL | OFF;
/// AI:   xlog::vlog_level() = n;  // verbose at VXLOG/TVLOG/VZLOG when n >= 1
/// AI:
/// AI: --- Output file ---
/// AI:   auto logger = xlog::daily_logger_mt("app", "logs/daily.log");
/// AI:   (or rotating_logger_mt, basic_logger_sg, ... see xlog/sinks/)
/// AI:   xlog::set_default_logger(logger);
/// AI:
/// AI: --- Pattern ---
/// AI:   logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
/// AI:
/// AI: --- Sync / Async ---
/// AI:   // default is sync; for async wrap before use:
/// AI:   logger = xlog::create_async<xlog::sinks::daily_file_sink_mt>("logs/daily.log");
/// AI:
/// AI: --- Logging ---
/// AI:   XLOG(INFO) << "stream " << 42;
/// AI:   TLOG(INFO, "fmt {}", 42);
/// AI:   ZLOG(INFO, "printf %d", 42);
/// AI:   XPLOG(INFO) << "append errno";  // TPLOG/ZPLOG also
/// AI:   VXLOG(1) << "verbose when vlog_level >= 1";  // TVLOG/VZLOG also
/// AI:   DXLOG(INFO) << "debug only (no-op in release)";
/// AI:   DTLOG(INFO, "fmt") / DZLOG(INFO, "printf %d") — debug only, release no-op
/// AI:   XLOG_IF(INFO, cond) << "conditional";
/// AI:   TLOG_IF(INFO, cond, "fmt") / ZLOG_IF(INFO, cond, "printf %d")
/// AI:   XLOG_EVERY_N(INFO, 2) << "every 2nd call";
/// AI:   TLOG_EVERY_N(INFO, 2, "fmt") / ZLOG_EVERY_N(INFO, 2, "printf %d")
/// AI:   XCHECK_EQ(a, b) << "assert a == b";
/// AI:   TCHECK_EQ(a, b) / ZCHECK_EQ(a, b) — all three families
/// AI:
/// AI: --- Upstream adapters ---
/// AI:   #include <xlog/upstream/glog.h>
/// AI:   xlog::upstream::enable_glog(logger);  // LOG(INFO) → xlog
/// AI:   xlog::upstream::disable_glog();
/// AI:   (also turbo and absl variants)

/// @brief Writer layer
/// AI: spdlog-derived: logger, pattern_formatter, sinks (file/rotating/daily/null/color/systemd),
/// AI: async thread pool, registry, bundled fmt 12.1.0.

/// @brief Build system
/// AI: Uses kmcmake (https://github.com/kumose/kmcmake) as the CMake framework.
/// AI: Framework modules live under kmcmake/ — DO NOT MODIFY.
/// AI: User configuration lives under cmake/ — MODIFY FREELY.

/// @brief Directory layout
/// AI: .
/// AI: ├── CMakeLists.txt              # Entry point
/// AI: ├── cmake/                      # User configuration (modifiable)
/// AI: │   ├── <project>_user_option.cmake   # User overrides
/// AI: │   ├── <project>_deps.cmake          # Dependencies
/// AI: │   ├── <project>_cxx_config.cmake    # C++ flags aggregation
/// AI: │   ├── <project>_cpack_config.cmake  # Packaging config
/// AI: │   └── <project>_config.cmake.in     # CMake export template
/// AI: ├── kmcmake/                    # Framework (update-safe)
/// AI: │   ├── kmcmake_module.cmake    # Entry point
/// AI: │   ├── kmcmake_option.cmake    # Global options
/// AI: │   ├── arch/                   # Per-CPU SIMD detection + level
/// AI: │   └── tools/                  # Build functions (library, test, etc.)
/// AI: ├── <project>/                  # Source code
/// AI: │   ├── CMakeLists.txt
/// AI: │   ├── *.h / *.cc
/// AI: │   ├── version.h (generated)
/// AI: │   └── skills.h (this file)
/// AI: ├── tests/
/// AI: ├── benchmark/
/// AI: └── examples/

/// @brief Build flow
/// AI: 1. project() sets name + version
/// AI: 2. include(kmcmake_module) loads all framework modules
/// AI: 3. include(<project>_user_option OPTIONAL) — user overrides
/// AI: 4. include(<project>_deps) — find_package deps
/// AI: 5. include(<project>_cxx_config) — sets KMCMAKE_CXX_OPTIONS
/// AI: 6. configure_file(version.h.in) — generates version.h
/// AI: 7. add_subdirectory(<project>) — builds main sources
/// AI: 8. add_subdirectory(tests) / benchmark / examples — optional

/// @brief SIMD architecture
/// AI: Detection is per-CPU-architecture, each in arch/<arch>/:
/// AI:   - kmcmake_arch_detect.cmake: probes hardware, exports KMCMAKE_<ARCH>_HAS_<FEAT>
/// AI:   - kmcmake_arch_level.cmake: reads KMCMAKE_RUNTIME_SIMD_LEVEL, exports:
/// AI:       KMCMAKE_ARCH_ENABLE_<FEAT>  (0/1 for version.h)
/// AI:       KMCMAKE_SIMD_CXX_FLAGS       (compiler flags)
/// AI: To override: set KMCMAKE_ARCH_ENABLE_<FEAT> to OFF in user_option.cmake
/// @}

/// @defgroup build_api CMake Build API
/// AI: All target types are defined in kmcmake/tools/. See docs/AI.md for demos.
/// @{
/// @brief kmcmake_cc_library   — static + shared lib from same objects
/// @brief kmcmake_cc_interface — header-only library
/// @brief kmcmake_cc_binary    — executable
/// @brief kmcmake_cc_object    — object library (no link)
/// @brief kmcmake_cc_test      — test executable + ctest registration
/// @brief kmcmake_cc_benchmark — benchmark executable
/// @}

/// @defgroup config_vars Configuration Variables
/// AI: Output variables available after include(<project>_cxx_config)
/// @{
/// @brief KMCMAKE_BASE_CXX_FLAGS  — compiler flags determined by compiler ID
/// @brief KMCMAKE_SIMD_CXX_FLAGS  — SIMD flags from arch detection
/// @brief KMCMAKE_RANDEN_FLAGS    — AES/hardware random flags
/// @brief KMCMAKE_CXX_OPTIONS     — aggregated flags (BASE + SIMD + RANDEN)
/// @brief KMCMAKE_ARCH_ENABLE_*   — per-feature SIMD enable as 0/1 integers
/// @}

/// @defgroup version_header Generated version.h Macros
// The version.h file is generated from xlog/version.h.in
// and contains the following compile-time macros:
/// @{
/// @brief XLOG_VERSION_MAJOR / _MINOR / _PATCH  — version components
/// @brief XLOG_VERSION_STRING  — e.g. "0.0.5"
/// @brief XLOG_SIMD_LEVEL  — target SIMD level string
/// @brief XLOG_SIMD_ENABLE_SSE..AVX512F  — 0 or 1 per feature
/// @brief XLOG_CXX_COMPILER_ID     — compiler name
/// @brief XLOG_CXX_COMPILER_VERSION — compiler version
/// @brief XLOG_BUILD_TYPE_STRING       — Debug/Release/etc
/// @brief XLOG_BUILD_SYSTEM        — OS distro name
/// @brief XLOG_GIT_COMMIT_HASH     — full git commit
/// @brief XLOG_GIT_VERSION_STRING  — "tag-hash[-dirty]"
/// @}

// AI: End of skills.h — AI assistants should prioritize this file and docs/AI.md
// AI: over reading all source files. For third-party dependencies, look for their
// AI: skills.h at <dep>/include/<dep>/skills.h or similar location.
