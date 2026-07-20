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
// AI: This file is a human/AI-readable summary of xlog. Read THIS and
// AI: docs/AI.md + docs/logging.md BEFORE scanning all sources.
// AI: Style: triple-slash Doxygen comments; lines starting with "AI:" are
// AI: instructions for agents (constraints + design invariants).

#pragma once

/// @defgroup project_summary Project Summary
/// @brief xlog — absl/glog-style logging (stream / fmt / printf)
///
/// @brief Purpose
/// AI: Minimal C++ logging library with XLOG (ostream), TLOG (fmt), ZLOG (printf),
/// AI: XCHECK*, VLOG variants, multi-set LogSinkRegistry (no remove), and setup_*
/// AI: helpers. Prefer snake_case in new xlog code. Umbrella: <xlog/logging.h>.
///
/// @brief Read order for AI agents
/// AI: 1) xlog/skills.h (this file)
/// AI: 2) docs/AI.md (constraints + xlog design prompts)
/// AI: 3) docs/logging.md / docs/logging_CN.md (architecture detail)
/// AI: 4) Only then open specific .h/.cc for the task
///
/// @brief Naming / style
/// AI: Prefer snake_case for functions/members in xlog/. Match nearby file style.
/// AI: Do not reformat unrelated code. Do not add JSON sinks / vmodule / in-tree
/// AI: stacktrace unless the user explicitly asks (those are deferred / out of scope).
/// AI: Local commits only when asked; git commit via /usr/bin/git -F file if wrapper
/// AI: rejects --trailer.
/// @}

/// @defgroup hot_path Hot path
/// @{
/// @brief Pipeline
/// AI: macro → LogMessage (fill LogEntry + body buffer) → flush →
/// AI: log_to_sinks → default LogSinkSet::do_log →
/// AI: format_log (once) → send to extras + set sinks → stderr_threshold branch.
///
/// @brief LogEntry
/// AI: severity, file/line, timestamp, tid/pid, verbose_level, buffer (raw body),
/// AI: format_buffer (after format_log; what sinks write).
/// @}

/// @defgroup two_layers Two extension layers (CRITICAL)
/// @{
/// @brief LogSink — destination only
/// AI: Override send/flush. Consume already-formatted format_buffer.
/// AI: Built-ins: DefaultSink, NullSink, AnsiColorSink, RotatingFileSink,
/// AI: DailyFileSink, HourlyFileSink.
/// AI: NEVER put shared layout / prefix rewriting in every LogSink::send when
/// AI: multiple sinks should share one format — that is O(N) waste.
///
/// @brief LogSinkSet — format unlocked, then lock + dispatch
/// AI: do_log: format_log on caller thread (NO set lock) → lock → dispatch_locked
/// AI:   (sink send/flush + stderr_threshold). Built-in sinks have NO per-sink mutex.
/// AI: Re-entrant XLOG from send() uses TLS guard: no second lock, stderr only.
/// AI: Override format_log (layout) or dispatch_locked (fan-out). FormatOnlySinkSet
/// AI: skips I/O in dispatch_locked. Register custom sets via add_log_sink_set.
///
/// @brief Registry
/// AI: LogSinkRegistry: id → LogSinkSet, one default, NO remove.
/// AI: add_log_sink / add_log_sinks build a plain LogSinkSet today.
/// AI: Custom set subclasses need registry ownership of unique_ptr<LogSinkSet>
/// AI: (same immortal lifetime). set_default_sink(id) switches process default.
/// AI: Hot path may keep raw LogSinkSet* after set_default.
/// @}

/// @defgroup filters Filters (orthogonal)
/// @{
/// @brief min_log_level
/// AI: Applied in LogMessage::flush. Drops the entry before sinks.
/// AI: FATAL still aborts in flush even when filtered out.
///
/// @brief verbosity / VLOG
/// AI: XVLOG/TVLOG/ZVLOG are severity INFO with verbose_level; gate vs verbosity().
/// AI: No vmodule. Optional XLOG_MAX_VLOG_VERBOSITY compile-out.
/// AI: Prefix may mark Vn. EVERY_N / FIRST_N / ONCE / EVERY_POW_2 / EVERY_N_SEC shared.
///
/// @brief Compile-time
/// AI: XLOG_MIN_LOG_LEVEL, XLOG_STRIP_LOG (non-fatal → NullStream; FATAL/CHECK still die).
///
/// @brief stderr_threshold
/// AI: Applied in LogSinkSet::do_log AFTER sinks. Not the same as min_log_level.
/// AI: if (severity >= stderr_threshold) { write_to_stderr; if FATAL abort; }
/// AI: threshold always <= FATAL so FATAL always enters; no on_fatal_error.
/// AI: When a sink already owns stderr, set threshold to FATAL to avoid double-print.
/// AI: Defaults: initialize / DefaultSink path → FATAL; file-only setup_* → ERROR;
/// AI: setup_color_stdout → ERROR; setup_stderr / color_stderr / file+color → FATAL.
/// @}

/// @defgroup macros Public macros / headers
/// @{
/// @brief logging.h — umbrella (check, initialize, setup, tlog, utility, xlog, zlog)
/// @brief XLOG / DXLOG — operator<< ; DFATAL; .no_prefix(); XPLOG / .with_perror()
/// @brief TLOG / DTLOG — fmt::format via print
/// @brief ZLOG / DZLOG — printf via fmt::sprintf
/// @brief XVLOG / TVLOG / ZVLOG (+ D* and EVERY_* variants) — verbose INFO
/// @brief XCHECK* / DXCHECK* — fatal checks
/// @brief hex_string — utility.h helpers for << / {}
/// @}

/// @defgroup setup Setup helpers
/// @{
/// @brief setup.h
/// AI: initialize_log + register default set + set stderr_threshold appropriately.
/// AI: Prefer setup_* for apps; registry APIs for tests / multi-set (ScopedMockLog).
/// AI: make_default_log_filename(argv0) → logs/<basename>_log.txt
/// @}

/// @defgroup design_rules Design rules for agents
/// @{
/// @brief Invariants
/// AI: 1. Format at set scope; write at sink scope.
/// AI: 2. One format_log pass per entry for the whole set (+ extras).
/// AI: 3. stderr mirror is set policy (write_to_stderr), not a recursive registry hop.
/// AI: 4. FATAL terminates in do_log threshold branch or flush early-filter paths.
/// AI: 5. Registry append-only; switch with set_default_sink.
/// AI: 6. Default = ordered sync under one set mutex. Async is NOT built-in:
/// AI:    users subclass LogSinkSet (e.g. AsyncLogSinkSet): format unlocked →
/// AI:    enqueue; worker thread runs existing sink send/flush + stderr/FATAL.
/// AI:    Do not assume reordering is OK unless the app opts into async.
/// AI: 7. Stacktrace: out of scope for this repo (separate project later).
/// AI: 8. ref/ is reference / gitignored downloads — do not treat as production API.
/// AI: See docs/logging.md section "Sync by default, async when you need it".
/// @}

/// @defgroup source_map Source map (xlog/)
/// @{
/// @brief logging.h initialize.h setup.h — app entry / config / one-shot setup
/// @brief xlog.h tlog.h zlog.h check.h — macros
/// @brief log_entry.h log_severity.h log_sink.h log_sink_set.h — core types
/// @brief format.h / format.cc — default xlog_format
/// @brief internal/log_message.* — LogMessage / flush
/// @brief internal/conditions.* nullstream.h voidify.h strerror.* — macro support
/// @brief sinks/* — concrete LogSink implementations
/// AI: Default set (create_default): DefaultSink + AndroidLogSink (__ANDROID__)
/// AI:   + WindowsDebuggerLogSink (_WIN32). Platform sinks are siblings in the
/// AI:   default LogSinkSet, not inside DefaultSink::send.
/// @brief utility.h — write_to_stderr, hex_string, thread identify
/// @brief tests/ — gtest; many TUs provide their own main + LogTestEnvironment
/// @}

/// @defgroup kmcmake_build Build (kmcmake)
/// @{
/// @brief Build system
/// AI: kmcmake framework under kmcmake/ — DO NOT MODIFY unless task targets it.
/// AI: User CMake under cmake/ — MODIFY FREELY. Flags: KMCMAKE_CXX_OPTIONS.
/// AI: Deps via kmpkg (fmt, gtest). Build: cmake --preset / build dir + ctest.
/// @}

/// @defgroup version_header Generated version.h
/// @{
/// @brief XLOG_VERSION_* / XLOG_VERSION_STRING
/// @brief XLOG_SIMD_ENABLE_* / XLOG_GIT_* / XLOG_BUILD_*
/// @}

// AI: End of skills.h — prefer this + docs/AI.md + docs/logging.md over full tree scans.
