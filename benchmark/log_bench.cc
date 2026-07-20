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
// Compare XLOG / TLOG / ZLOG under:
//   * FormatOnlySinkSet  — format cost only (no I/O)
//   * RotatingFileSink   — format + append file
//
// Build with -DKMCMAKE_BUILD_BENCHMARK=ON, then:
//   ./benchmark/log_log_bench --benchmark_filter=FormatOnly
//   ./benchmark/log_log_bench --benchmark_filter=File

#include <benchmark/benchmark.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

#include <xlog/initialize.h>
#include <xlog/log_severity.h>
#include <xlog/log_sink_set.h>
#include <xlog/logging.h>
#include <xlog/sinks/format_only_sink_set.h>
#include <xlog/sinks/rotating_file_sink.h>

namespace {

    std::filesystem::path BenchDir() {
        return std::filesystem::temp_directory_path() / "xlog_log_bench";
    }

    void EnsureInit() {
        static const bool once = [] {
            xlog::initialize_log();
            xlog::set_min_log_level(xlog::LogSeverity::kSeverityInfo);
            xlog::set_log_with_prefix(true);
            return true;
        }();
        (void) once;
    }

    void UseFormatOnly() {
        EnsureInit();
        static const uint32_t id = [] {
            const uint32_t sid =
                xlog::add_log_sink_set(std::make_unique<xlog::FormatOnlySinkSet>());
            xlog::set_default_sink(sid);
            // No stderr mirror for INFO path.
            xlog::set_stderr_threshold(xlog::LogSeverity::kSeverityFatal);
            return sid;
        }();
        (void) id;
        xlog::set_default_sink(id);
        xlog::set_stderr_threshold(xlog::LogSeverity::kSeverityFatal);
    }

    void UseRotatingFile() {
        EnsureInit();
        static const uint32_t id = [] {
            std::error_code ec;
            std::filesystem::create_directories(BenchDir(), ec);
            const auto path = (BenchDir() / "bench_log.txt").string();
            constexpr size_t kMaxBytes = 512ull * 1024 * 1024;  // 512 MiB
            auto sink = std::make_unique<xlog::RotatingFileSink>(
                path, kMaxBytes, /*max_files=*/4, /*check_interval_s=*/3600);
            const uint32_t sid = xlog::add_log_sink(std::move(sink));
            xlog::set_default_sink(sid);
            xlog::set_stderr_threshold(xlog::LogSeverity::kSeverityFatal);
            return sid;
        }();
        (void) id;
        xlog::set_default_sink(id);
        xlog::set_stderr_threshold(xlog::LogSeverity::kSeverityFatal);
    }

    // -------------------------------------------------------------------------
    // Format only
    // -------------------------------------------------------------------------

    void BM_XLOG_FormatOnly(benchmark::State &state) {
        UseFormatOnly();
        int64_t i = 0;
        for (auto _ : state) {
            XLOG(INFO) << "bench xlog " << i << " val=" << (i * 3);
            ++i;
        }
        state.SetItemsProcessed(state.iterations());
    }

    void BM_TLOG_FormatOnly(benchmark::State &state) {
        UseFormatOnly();
        int64_t i = 0;
        for (auto _ : state) {
            TLOG(INFO, "bench tlog {} val={}", i, i * 3);
            ++i;
        }
        state.SetItemsProcessed(state.iterations());
    }

    void BM_ZLOG_FormatOnly(benchmark::State &state) {
        UseFormatOnly();
        int64_t i = 0;
        for (auto _ : state) {
            ZLOG(INFO, "bench zlog %lld val=%lld",
                 static_cast<long long>(i),
                 static_cast<long long>(i * 3));
            ++i;
        }
        state.SetItemsProcessed(state.iterations());
    }

    // -------------------------------------------------------------------------
    // Rotating file
    // -------------------------------------------------------------------------

    void BM_XLOG_File(benchmark::State &state) {
        UseRotatingFile();
        int64_t i = 0;
        for (auto _ : state) {
            XLOG(INFO) << "bench xlog " << i << " val=" << (i * 3);
            ++i;
        }
        state.SetItemsProcessed(state.iterations());
    }

    void BM_TLOG_File(benchmark::State &state) {
        UseRotatingFile();
        int64_t i = 0;
        for (auto _ : state) {
            TLOG(INFO, "bench tlog {} val={}", i, i * 3);
            ++i;
        }
        state.SetItemsProcessed(state.iterations());
    }

    void BM_ZLOG_File(benchmark::State &state) {
        UseRotatingFile();
        int64_t i = 0;
        for (auto _ : state) {
            ZLOG(INFO, "bench zlog %lld val=%lld",
                 static_cast<long long>(i),
                 static_cast<long long>(i * 3));
            ++i;
        }
        state.SetItemsProcessed(state.iterations());
    }

    BENCHMARK(BM_XLOG_FormatOnly)->Threads(1)->Threads(4);
    BENCHMARK(BM_TLOG_FormatOnly)->Threads(1)->Threads(4);
    BENCHMARK(BM_ZLOG_FormatOnly)->Threads(1)->Threads(4);

    BENCHMARK(BM_XLOG_File)->Threads(1)->Threads(4);
    BENCHMARK(BM_TLOG_File)->Threads(1)->Threads(4);
    BENCHMARK(BM_ZLOG_File)->Threads(1)->Threads(4);

}  // namespace

BENCHMARK_MAIN();
