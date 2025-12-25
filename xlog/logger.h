// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <xlog/format.h>

namespace xlog {

    enum class LogLevel {
        LEVEL_DEBUG,
        LEVEL_INFO,
        LEVEL_WARN,
        LEVEL_ERROR,
        LEVEL_FATAL,
        LevelCount
    };

    struct LogMessage {

        std::vector<std::pair<std::string, std::string>> fields;
        std::string message;
        std::string indent;
        LogLevel level;

        ~LogMessage();

    LogMessage& Debug(std::string_view msg) {
        message = msg;
        level = LogLevel::LEVEL_DEBUG;
        return *this;
    }

    LogMessage& Info(std::string_view msg) {
        message = msg;
        level = LogLevel::LEVEL_INFO;
        return *this;
    }

    LogMessage& Warn(std::string_view msg) {
        message = msg;
        level = LogLevel::LEVEL_WARN;
        return *this;
    }

    LogMessage& Error(std::string_view msg) {
        message = msg;
        level = LogLevel::LEVEL_ERROR;
        return *this;
    }

    LogMessage& Fatal(std::string_view msg) {
        message = msg;
        level = LogLevel::LEVEL_FATAL;
        return *this;
    }       

    LogMessage& WithField(std::string_view key, std::string_view value) {
        fields.emplace_back(key, value);
        return *this;
    }       
        
    private:
        void do_log();


    };

    constexpr std::array<std::string_view, static_cast<int>(xlog::LogLevel::LevelCount)> DefaultStrings = {
        "⚙",  // Debug
        "•",  // Info
        "⚠",  // Warn
        "⨯",  // Error
        "◼",  // Fatal
    };

    constexpr std::array<std::string_view, static_cast<int>(xlog::LogLevel::LevelCount)> CheckStrings = {
        "⚙",  // Debug
        "✓",  // Info
        "⚠",  // Warn
        "⨯",  // Error
        "◼",  // Fatal
    };

    constexpr std::array<std::string_view, static_cast<int>(xlog::LogLevel::LevelCount)> DefaultLevelStrings = {
        "DEBUG",  // Debug
        "INFO",  // Info
        "WARN",  // Warn
        "ERROR",  // Error
        "FATAL",  // Fatal
    };

    constexpr std::array<std::string_view, static_cast<int>(xlog::LogLevel::LevelCount)> ShortLevelStrings = {
        "D",  // Debug
        "I",  // Info
        "W",  // Warn
        "E",  // Error
        "F",  // Fatal
    };


    struct LogConfig {
        LogConfig();
        LevelFormater Format;
        FILE *File{nullptr};
        bool  LogLevelString{false};
        std::array<std::string_view, static_cast<int>(xlog::LogLevel::LevelCount)> Icons{DefaultStrings};
        std::array<std::string_view, static_cast<int>(xlog::LogLevel::LevelCount)> LevelString{DefaultLevelStrings};
    };


    void set_log_config(LogConfig conf);

    void enable_log_level_string();

    void disable_log_level_string();

    const LogConfig&get_log_config();
    

    inline int level_index(LogLevel l) {
        return static_cast<int>(l);
    }


}  // namespace xlog
