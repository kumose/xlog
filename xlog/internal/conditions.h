#pragma once

#include <atomic>
#include <cstdint>
#include <chrono>

namespace xlog {
namespace internal {

class Voidify {
public:
    Voidify() = default;
    template <typename T>
    void operator&&(const T &) {}
};

class LogEveryNState {
    std::atomic<uint32_t> counter_{0};
public:
    bool ShouldLog(int n) {
        auto counter = counter_.fetch_add(1, std::memory_order_relaxed);
        return counter % n == 0;
    }
    uint32_t counter() const {
        return counter_.load(std::memory_order_relaxed);
    }
};

class LogFirstNState {
    std::atomic<uint32_t> counter_{0};
public:
    bool ShouldLog(int n) {
        auto counter = counter_.fetch_add(1, std::memory_order_relaxed);
        return counter < static_cast<uint32_t>(n);
    }
    uint32_t counter() const {
        return counter_.load(std::memory_order_relaxed);
    }
};

class LogEveryPow2State {
    std::atomic<uint32_t> counter_{0};
public:
    bool ShouldLog() {
        auto counter = counter_.fetch_add(1, std::memory_order_relaxed);
        return counter == 0 || (counter & (counter - 1)) == 0;
    }
    uint32_t counter() const {
        return counter_.load(std::memory_order_relaxed);
    }
};

class LogEveryNSecState {
    std::atomic<uint32_t> counter_{0};
    std::atomic<int64_t> next_log_time_ms_{0};
public:
    bool ShouldLog(double seconds) {
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        auto next = next_log_time_ms_.load(std::memory_order_relaxed);
        counter_.fetch_add(1, std::memory_order_relaxed);
        if (now >= next) {
            auto delay = static_cast<int64_t>(seconds * 1000);
            if (delay < 1) delay = 1;
            next_log_time_ms_.store(now + delay, std::memory_order_relaxed);
            return true;
        }
        return false;
    }
    uint32_t counter() const {
        return counter_.load(std::memory_order_relaxed);
    }
};

} // namespace internal
} // namespace xlog

#define XLOG_INTERNAL_STATELESS_CONDITION(condition) \
    switch (0)                                       \
    case 0:                                          \
    default:                                         \
        !(condition) ? (void)0 : ::xlog::internal::Voidify()&&

#define XLOG_INTERNAL_STATEFUL_CONDITION(condition)                 \
    for (bool _xlog_do_log_ = (condition);                          \
         _xlog_do_log_;                                             \
         _xlog_do_log_ = false)                                     \
        XLOG_INTERNAL_STATEFUL_CONDITION_IMPL

#define XLOG_INTERNAL_STATEFUL_CONDITION_IMPL(kind, ...)                \
    for (static ::xlog::internal::Log##kind##State                      \
             _xlog_stateful_state_;                                     \
         _xlog_do_log_ &&                                               \
         _xlog_stateful_state_.ShouldLog(__VA_ARGS__);                  \
         _xlog_do_log_ = false)                                         \
        for (const uint32_t COUNTER =                                   \
                 _xlog_stateful_state_.counter();                       \
             _xlog_do_log_;                                             \
             _xlog_do_log_ = false)
