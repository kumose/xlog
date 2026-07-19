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

#pragma once

#include <string_view>
#include <array>

namespace xlog {
    enum class LogSeverity {
        kSeverityTrace = 0,
        kSeverityDebug = 1,
        kSeverityInfo = 2,
        kSeverityWarning = 3,
        kSeverityError = 4,
        kSeverityFatal = 5,
    };

    static constexpr size_t kSeveritySize = 6;


    static constexpr std::array<std::string_view, kSeveritySize> kSeverityUpperNames = {
        "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
    };

    static constexpr std::array<std::string_view, kSeveritySize> kLogSeverityLowerNames = {
        "trace", "debug", "info", "warning", "error", "fatal"
    };

    static constexpr std::array<char, kSeveritySize> kLogSeverityShortUpperNames = {
        'T', 'D', 'I', 'W', 'E', 'F'
    };

    static constexpr std::array<char, kSeveritySize> kLogSeverityShortLowerNames = {
        't', 'd', 'i', 'w', 'e', 'f'
    };



    /// @brief Converts a severity level to its full upper‑case name.
    /// @param severity The level to convert.
    /// @return "TRACE", "DEBUG", etc., or "UNKNOWN" on error.
    inline constexpr std::string_view severity_to_upper_name(
        LogSeverity severity) noexcept {

        size_t index = static_cast<size_t>(severity);
        if (index >= kSeveritySize) {
            return "UNKNOWN";
        }
        return kSeverityUpperNames[index];
    }

    /// @brief Converts a severity level to its full lower‑case name.
    /// @param severity The level to convert.
    /// @return "trace", "debug", etc., or "unknown" on error.
    inline constexpr std::string_view severity_to_lower_name(
        LogSeverity severity) noexcept {

        size_t index = static_cast<size_t>(severity);
        if (index >= kSeveritySize) {
            return "unknown";
        }
        return kLogSeverityLowerNames[index];
    }

    /// @brief Converts a severity level to its upper‑case single character.
    /// @param severity The level to convert.
    /// @return 'T', 'D', etc., or '?' on error.
    inline constexpr char severity_to_upper_char(
        LogSeverity severity) noexcept {

        size_t index = static_cast<size_t>(severity);
        if (index >= kSeveritySize) {
            return '?';
        }
        return kLogSeverityShortUpperNames[index];
    }

    /// @brief Converts a severity level to its lower‑case single character.
    /// @param severity The level to convert.
    /// @return 't', 'd', etc., or '?' on error.
    inline constexpr char severity_to_lower_char(
        LogSeverity severity) noexcept {

        size_t index = static_cast<size_t>(severity);
        if (index >= kSeveritySize) {
            return '?';
        }
        return kLogSeverityShortLowerNames[index];
    }

    /// @brief Converts a severity level to its default (full, upper‑case) representation.
    /// @param severity The level to convert.
    /// @return "TRACE", "DEBUG", etc., or "UNKNOWN" on error.
    inline constexpr std::string_view severity_to_string(
        LogSeverity severity) noexcept {

        return severity_to_upper_name(severity);
    }


    /// @brief Equality comparison.
    inline constexpr bool operator==(LogSeverity lhs, LogSeverity rhs) noexcept {
        return static_cast<int>(lhs) == static_cast<int>(rhs);
    }

    /// @brief Inequality comparison.
    inline constexpr bool operator!=(LogSeverity lhs, LogSeverity rhs) noexcept {
        return static_cast<int>(lhs) != static_cast<int>(rhs);
    }

    /// @brief Less‑than comparison (based on the underlying integral order).
    inline constexpr bool operator<(LogSeverity lhs, LogSeverity rhs) noexcept {
        return static_cast<int>(lhs) < static_cast<int>(rhs);
    }

    /// @brief Less‑than‑or‑equal comparison.
    inline constexpr bool operator<=(LogSeverity lhs, LogSeverity rhs) noexcept {
        return static_cast<int>(lhs) <= static_cast<int>(rhs);
    }

    /// @brief Greater‑than comparison.
    inline constexpr bool operator>(LogSeverity lhs, LogSeverity rhs) noexcept {
        return static_cast<int>(lhs) > static_cast<int>(rhs);
    }

    /// @brief Greater‑than‑or‑equal comparison.
    inline constexpr bool operator>=(LogSeverity lhs, LogSeverity rhs) noexcept {
        return static_cast<int>(lhs) >= static_cast<int>(rhs);
    }


    // ============================================================================
    //  Utility functions
    // ============================================================================

    /// @brief Determines whether a message with the given severity should be logged
    ///        based on a threshold.
    /// @param current The severity of the message to log.
    /// @param threshold The minimum severity that is considered loggable.
    /// @return true if current >= threshold (i.e., current is at least as severe as threshold).
    /// @note This is a convenient wrapper around the >= operator.
    inline constexpr bool should_log(LogSeverity current, LogSeverity threshold) noexcept {
        return current >= threshold;   // uses the overloaded operator>= above
    }


    /// @brief Returns the underlying integral value of a severity level.
    /// @param severity The severity to query.
    /// @return The integer value (0–5).
    /// @note Mainly provided for symmetry and occasional numeric computations.
    inline constexpr int get_severity_value(LogSeverity severity) noexcept {
        return static_cast<int>(severity);
    }

    /// @brief Inserts a textual representation of a severity level into an output stream.
    /// @param os The output stream.
    /// @param severity The severity to write.
    /// @return A reference to the stream (for chaining).
    /// @note This overload uses the full, upper‑case name by default.
    inline std::ostream& operator<<(std::ostream& os, LogSeverity severity) {
        return os << severity_to_string(severity);
    }

} // namespace xlog
