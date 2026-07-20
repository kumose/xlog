# xlog — AI agent context

**Read first:** `xlog/skills.h` → this file → `docs/logging.md` (中文: `docs/logging_CN.md`).
Do not scan the whole tree before those.

The second half of this file is the kmcmake framework reference (build API).
Project-specific logging rules are in the sections below.

---

## xlog in one paragraph

absl/glog-style C++ logger: `XLOG` (stream) / `TLOG` (fmt) / `ZLOG` (printf),
`XCHECK*`, verbose INFO (`XVLOG`/`TVLOG`/`ZVLOG`), multi-set `LogSinkRegistry`
(no remove; switch with `set_default_sink`). Umbrella header: `<xlog/logging.h>`.
Prefer **snake_case** in new `xlog/` code.

## Hot path (do not invent another)

```text
macro → LogMessage → flush (min_log_level / verbosity; FATAL may abort here)
     → log_to_sinks → LogSinkSet::do_log
          → format_log ONCE → sink send (extras + set)
          → if severity >= stderr_threshold: write_to_stderr; if FATAL: abort
```

## Two layers — CRITICAL for any sink/format work

| Layer | Type | Responsibility | Extend by |
|-------|------|----------------|-----------|
| Sink | `LogSink` | One destination I/O | `send` / `flush` |
| Set | `LogSinkSet` | Format once, fan-out, stderr mirror + FATAL | `format_log` / `do_log` |

**AI must obey:**

1. **`do_log` = format unlocked → lock → `dispatch_locked`.** Custom layout =
   override `format_log`; custom fan-out = override `dispatch_locked`.
2. Sinks only do I/O on filled `format_buffer`; **no per-sink mutex** (set
   lock serializes). Do not re-format inside every `send`.
3. Re-entrant `XLOG` from `send` is TLS-guarded (no second set lock).
4. Registry: `add_log_sink(s)` / `add_log_sink_set` (immortal, no remove).
5. `to_sink_also` / `to_sink_only` extras still go through the default set’s
   format + lock.

Full write-up: [logging.md](./logging.md).

## Filters (do not conflate)

| Knob | Where | Meaning |
|------|--------|---------|
| `min_log_level` | `LogMessage::flush` | Drop entry before sinks; filtered FATAL still aborts |
| `verbosity` | flush + macros | VLOG families are **INFO** + `verbose_level` (no vmodule) |
| `stderr_threshold` | `LogSinkSet::do_log` after sinks | Extra stderr mirror; nested FATAL abort |
| `XLOG_MIN_LOG_LEVEL` / `XLOG_STRIP_LOG` / `XLOG_MAX_VLOG_VERBOSITY` | compile-time | Macro gating |

`stderr_threshold` contract:

```text
if (severity >= stderr_threshold) {   // always <= FATAL ⇒ FATAL enters
  write_to_stderr(...);
  if (FATAL) abort();
}
```

No `on_fatal_error`. When a sink already owns stderr, set threshold to **FATAL**
(avoid double-print). File-only `setup_*` uses **ERROR**. See `setup.cc` /
`initialize.cc`.

## Out of scope (unless user explicitly asks)

- JSON sinks / new heavy deps
- vmodule
- In-tree stacktrace (`ref/` downloads are reference only)
- Editing `kmcmake/` framework files
- Commits / push without explicit user request

## AI Constraints

**Before modifying any file:**
1. Read the file's full contents first
2. Propose the exact change (what + why) in text
3. Wait for user approval — do not edit until user says "go ahead" / "do it" / "改" / "干" / "执行"

**Before running any git mutating command** (commit, push, rebase, merge, reset, etc.):
1. State the exact command you intend to run
2. Wait for user approval

**Other restrictions:**
- Do not create new files unless the task explicitly requires it; prefer editing existing files
- Do not list directory trees (`ls`, `tree`) or glob files unless necessary for the task at hand
- Do not run `cmake --build` or `ctest` unless explicitly asked to verify a change
- Do not read files you already have context for — reuse what you've been given. Only re-read when the content has likely changed (e.g. after an edit you just made).
- Do not suggest adding new dependencies, libraries, or external tools without asking first
- Do not change `kmcmake/` framework files unless the task specifically targets them
- Do not reformat, restyle, or rename variables/functions as side work — stick to the task
- Batch independent operations (reads, searches, parallel commands) to reduce round-trips
- **Distinguish questions from tasks.** If the user asks "Is X appropriate?" / "合适吗" / "应该...吧", they are seeking an opinion, not requesting action. Answer concisely and stop. Do not propose changes, do not offer to implement, do not touch files — unless the user explicitly follows up with "do it" / "改" / "干" / "执行".

## AI Workflow

When the user gives a task, follow this sequence:

1. **Diagnose** — explain the problem and propose a solution
2. **Wait** — do not touch files until the user approves
3. **Execute** — only after the user says "go ahead" / "do it" / "改" / "干" / "执行"

---

# kmcmake AI Context

Build-system reference for this repo (kmcmake framework). Behavioral constraints
are in the xlog section above; do not re-read this half for logging design.

## Project Overview

kmcmake is a CMake-based C/C++ build system. The `template/` directory is the
skeleton used to bootstrap new projects. The generation flow is:

1. `cmake -S ./template -B build -DCHANGEME=myproject`
2. `cmake --install build --prefix /path/to/project`

During step 1, CMake processes `@VAR@` placeholders via `configure_file()`.
During step 2, the rendered files are copied to the target directory.

## Variable Reference

All placeholder variables used in `*.in` files:

| Variable | Source | Description |
|----------|--------|-------------|
| `@CHANGEME@` | `-DCHANGEME=` | Project name (e.g. `myproj`) |
| `@CHANGEME_UP@` | `string(TOUPPER)` | Uppercase project name |
| `@CHANGEME_LOW@` | `string(TOLOWER)` | Lowercase project name |
| `@TM_KMCMAKE_VERSION@` | `template/CMakeLists.txt` | kmcmake version |
| `@CURRENT_DATE@` | `template/CMakeLists.txt` | Generation date |
| `@PROJECT_NAME@` | CMake `project()` | CMake project name |
| `@PROJECT_NAME_UP@` | `string(TOUPPER)` | Upper-case project name |
| `@PROJECT_VERSION_MAJOR@` | user set | Version components |
| `@CMAKE_INSTALL_LIBDIR@` | GNUInstallDirs | Install lib directory |
| `@KMCMAKE_ARCH_OPTION@` | kmcmake_arch | SIMD compiler flags |
| `@KMCMAKE_CONFIG_PRIVATE_FIND_SNIPPETS@` | changeme_deps | Private find_package calls |
| `@UPPERCASE_BUILD_TYPE@` | `string(TOUPPER)` | Debug/Release/etc |
| `@LC_KMCMAKE_PRETTY_NAME@` | kmcmake_module | OS distro name (lower) |
| `@KMCMAKE_DISTRO_VERSION_ID@` | kmcmake_module | OS version |
| `@CMAKE_CXX_COMPILER_ID@` | CMake | Compiler ID |
| `@CMAKE_CXX_COMPILER_VERSION@` | CMake | Compiler version |
| `@CMAKE_CXX_COMPILER_FLAGS@` | CMake | Raw compiler flags |
| `@KMCMAKE_CXX_OPTIONS@` | changeme_cxx_config | Aggregated CXX flags |
| `@CMAKE_CXX_STANDARD@` | CMake | C++ standard |
| `@GIT_COMMIT_HASH@` | git_commit.cmake | Git commit info |
| `@KMCMAKE_ARCH_ENABLE_SSE@` (et al) | kmcmake_arch | SIMD enable (0/1) |
| `@KMCMAKE_RUNTIME_SIMD_LEVEL@` | kmcmake_option | Target SIMD level |
| `@PACKAGE_INIT@` | CMakePackageConfigHelpers | Package init snippet |

## Directory Layout

```
project/
├── CMakeLists.txt              # Entry: includes kmcmake_module + user cmake/
├── CMakePresets.json
├── docs/
│   └── AI.md                   # AI context (this file)
├── kmcmake/                    # Framework layer — DO NOT MODIFY
│   ├── kmcmake_module.cmake    # Module entry point
│   ├── kmcmake_option.cmake    # Global options (BUILD_SHARED, SIMD level, etc.)
│   ├── arch/
│   │   ├── kmcmake_arch.cmake  # SIMD: detect + level + BOOL→int conversion
│   │   ├── x86/...             # x86 SIMD detection + level
│   │   └── arm/...             # ARM SIMD detection + level
│   ├── tools/
│   │   ├── kmcmake_compiler_flags.cmake  # BASE_CXX_FLAGS, RANDEN_FLAGS
│   │   ├── default_setting.cmake         # print functions, install dirs
│   │   ├── kmcmake_cc_library.cmake      # kmcmake_cc_library()
│   │   ├── kmcmake_cc_test.cmake         # kmcmake_cc_test()
│   │   ├── kmcmake_cc_binary.cmake       # kmcmake_cc_binary()
│   │   ├── kmcmake_cc_interface.cmake    # kmcmake_cc_interface()
│   │   ├── kmcmake_cc_object.cmake       # kmcmake_cc_object()
│   │   ├── kmcmake_cc_benchmark.cmake    # kmcmake_cc_benchmark()
│   │   ├── kmcmake_cc_proto.cmake        # Protobuf support
│   │   ├── kmcmake_cc_proto_object.cmake # Protobuf object
│   │   └── git_commit.cmake              # Git version info
│   └── package/                          # CPack config
├── cmake/                      # User config layer — MODIFY FREELY
│   ├── myproj_user_option.cmake    # User overrides (loaded first)
│   ├── myproj_deps.cmake          # Dependencies
│   ├── myproj_cxx_config.cmake    # C++ flags aggregation (thin)
│   ├── myproj_cpack_config.cmake  # Packaging config
│   └── myproj_config.cmake.in     # Export template
├── myproj/                     # Source code
│   ├── CMakeLists.txt
│   ├── foo.h / foo.cc
│   ├── main.cc
│   └── version.h (generated from .in)
├── tests/
├── benchmark/
└── examples/
```

## Build Flow

1. `project()` sets project name and version
2. `include(kmcmake_module)` loads framework:
   - `kmcmake_option.cmake` — user-visible options
   - `default_setting.cmake` — utility functions + install dirs
   - all `kmcmake_cc_*.cmake` tools
3. `include(myproj_user_option OPTIONAL)` — user overrides (may set `KMCMAKE_RUNTIME_SIMD_LEVEL`, etc.)
4. `include(myproj_deps)` — `find_package` calls, system libs
5. `include(myproj_cxx_config)`:
   - `kmcmake_compiler_flags.cmake` → `KMCMAKE_BASE_CXX_FLAGS`
   - `kmcmake_arch.cmake` → `KMCMAKE_SIMD_CXX_FLAGS` + `KMCMAKE_ARCH_ENABLE_*`
   - aggregates → `KMCMAKE_CXX_OPTIONS`
6. `configure_file(version.h.in)` — generates version header with SIMD macros
7. `add_subdirectory(myproj)` — builds source targets
8. `add_subdirectory(tests)` — if testing enabled
9. `add_subdirectory(benchmark)` — if benchmark enabled
10. `add_subdirectory(examples)` — if examples enabled

## SIMD Architecture

The `arch/` subsystem is per-CPU-architecture:

- **Detection phase** (`kmcmake_arch_detect.cmake`): probes each feature at compile-time, exports `KMCMAKE_{ARCH}_HAS_{FEATURE}` (BOOL)
- **Level phase** (`kmcmake_arch_level.cmake`): reads `KMCMAKE_RUNTIME_SIMD_LEVEL`, enables features level-by-level, exports `KMCMAKE_ARCH_ENABLE_{FEATURE}` and `KMCMAKE_SIMD_CXX_FLAGS`

To add a new architecture:
1. Create `arch/<arch>/kmcmake_arch_detect.cmake` — define `KMCMAKE_<ARCH>_HAS_*` flags
2. Create `arch/<arch>/kmcmake_arch_level.cmake` — set `KMCMAKE_ARCH_ENABLE_*` + build flags
3. Add routing in `arch/kmcmake_arch.cmake`

## Key Conventions

- **DO NOT** modify files under `kmcmake/` — these are framework files that get replaced on upgrade
- **DO** modify files under `cmake/` — these are your project's user configuration
- To override SIMD features: set `KMCMAKE_ARCH_ENABLE_{FEATURE}` to `OFF` in `user_option.cmake`
- To override base flags: set `KMCMAKE_BASE_CXX_FLAGS` in `user_option.cmake`
- All public CMake functions use `kmcmake_` prefix

## skills.h Convention (xlog)

`xlog/skills.h` is the **primary** AI prompt for this library (logging design +
API map). Keep it in sync when architecture invariants change (two-layer sinks,
`stderr_threshold`, FATAL path, out-of-scope items).

**For AI agents:**
- Read `xlog/skills.h` first, then the xlog section at the top of this file, then
  `docs/logging.md` — that trio replaces scanning all sources for design intent
- Style: `///` Doxygen; lines with `AI:` are agent instructions
- For other kumose deps, look for `<dep>/skills.h` similarly

**For maintainers:**
- Update `skills.h` when changing hot path / sink layers / filter semantics
- Keep entries concise; link to `docs/logging.md` for long prose

## File Generation Rules

- Files with `.in` extension are processed by `configure_file(@ONLY)` — only `@VAR@` substitutions
- `@CHANGEME@` is replaced by the project name passed via `-DCHANGEME=`
- `@CHANGEME_UP@` is the uppercase variant
- Non-`.in` files are copied verbatim during `cmake --install`

---

# kmcmake API Reference

Quick usage demos for all `kmcmake_cc_*` functions. Use these as templates.

## kmcmake_cc_library

Builds static library from object sources. When `SHARE ON` is set, also builds
and installs a shared variant.

```cmake
# Minimal — public install, auto-named from folder
kmcmake_cc_library(
    PUBLIC
    NAME mylib
    SOURCES mylib.cc
    HEADERS mylib.h
)

# Full example with shared variant and dependencies
kmcmake_cc_library(
    PUBLIC
    NAME core
    NAMESPACE myproj
    SOURCES core.cc core.h
    HEADERS core.h
    SHARE                                                # build + install shared lib
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include         # public includes
    PINCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src            # private includes
    LINKS Threads::Threads                               # public link deps
    PLINKS myproj::util                                  # private link deps
    WLINKS archiver                                       # whole-archive link
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}                       # compiler flags
    DEFINES MYLIB_EXPORTS                                # compile definitions
    EXCLUDE_SYSTEM                                       # don't mark includes SYSTEM
    UNITY                                                # enable unity build
)
```

Creates targets:
- `<namespace>::<name>` (shared alias, only when `SHARE ON`)
- `<namespace>::<name>_static` (static alias)

## kmcmake_cc_interface

Header-only library (no compiled sources).

```cmake
kmcmake_cc_interface(
    PUBLIC
    NAME api
    NAMESPACE myproj
    HEADERS api.h api.hpp
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    LINKS myproj::core
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

## kmcmake_cc_binary

Creates an executable target.

```cmake
kmcmake_cc_binary(
    NAME mytool
    SOURCES main.cc tool.cc
    DEPS myproj::core
    LINKS ${KMCMAKE_DEPS_LINK}
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    LINKOPTS -static
    PUBLIC               # install the binary
)
```

## kmcmake_cc_object

Creates an object library (no linker step), useful for large source sets compiled once.

```cmake
kmcmake_cc_object(
    NAME bigobj
    SOURCES a.cc b.cc c.cc
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)
```

Then reference it in a library/binary via `OBJECTS`:

```cmake
kmcmake_cc_library(
    NAME combined
    OBJECTS bigobj
    # no SOURCES — uses precompiled objects
)
```

## kmcmake_cc_test

Create and register a CTest test case.

```cmake
# Basic test
kmcmake_cc_test(
    NAME my_test
    MODULE module_name              # prefix for test binary + ctest name
    SOURCES my_test.cc
    DEPS myproj::core gtest gtest_main
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)

# Extended test with custom command and expected output
kmcmake_cc_test_ext(
    NAME my_test
    MODULE module_name
    ALIAS custom_name                # appended to test name
    ARGS --flag value               # extra CLI args
    PASS_EXP "All tests passed"     # regex — test passes if output matches
    FAIL_EXP "ERROR|Failed"        # regex — test fails if output matches
    SKIP_EXP "SKIP|Skipped"        # regex — test is skipped
)

# Disabled test
kmcmake_cc_test(
    NAME disabled_test
    MODULE module
    SOURCES disabled_test.cc
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    DISABLED
)
```

Test naming pattern: `<module>_<name>` (e.g. `module_my_test`).

## kmcmake_cc_benchmark

Same API shape as test, under `KMCMAKE_BUILD_BENCHMARK`.

```cmake
kmcmake_cc_benchmark(
    NAME bm
    MODULE module
    SOURCES benchmark.cc
    DEPS benchmark::benchmark
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)
```

---

# Quick CMakeLists.txt Templates

## Module (subdirectory) CMakeLists.txt

```cmake
kmcmake_cc_library(
    PUBLIC
    NAME mysub
    NAMESPACE ${PROJECT_NAME}
    SOURCES mysub.cc
    HEADERS mysub.h
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    PLINKS ${KMCMAKE_DEPS_LINK}
)

kmcmake_cc_binary(
    NAME mysub_main
    SOURCES main.cc
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    DEPS ${PROJECT_NAME}::mysub
    LINKS ${KMCMAKE_DEPS_LINK}
)
```

## Test CMakeLists.txt

```cmake
kmcmake_cc_test(
    NAME unit_test
    MODULE module
    SOURCES unit_test.cc
    DEPS ${PROJECT_NAME}::mysub gtest gtest_main
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
)
```

## User Option Override (in cmake/*_user_option.cmake)

```cmake
# Force-disable AVX2 even if hardware supports it
set(KMCMAKE_ARCH_ENABLE_AVX2 OFF)

# Change SIMD target
set(KMCMAKE_RUNTIME_SIMD_LEVEL SSE4_2 CACHE STRING "" FORCE)

# Add extra compiler flags
list(APPEND KMCMAKE_CXX_OPTIONS "-fopenmp")
```