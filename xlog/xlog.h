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

#include <vector>
#include <string>
#include <iostream>
#include <xlog/logger.h>
#include <fmt/format.h>

namespace xlog {

    
    struct Xlogger {
        std::string indent;

        void IncreamIndent() {
            indent += "\t";
        }
        
        void DecreamIndent() {
            if (!indent.empty()) {
                indent.pop_back();
            }
        }

        void ResetIndent() {
            indent.clear();
        }

        LogMessage Debug(std::string_view msg) {
            LogMessage m;
            m.indent = indent;
            m.Debug(msg);
            return m;
        }

        LogMessage Info(std::string_view msg) {
        LogMessage m;
        m.indent = indent;
        m.Info(msg);
        return m;
    }

    LogMessage Warn(std::string_view msg) {
        LogMessage m;
        m.indent = indent;
        m.Warn(msg);
        return m;
    }

    LogMessage Error(std::string_view msg) {
        LogMessage m;
        m.indent = indent;
        m.Error(msg);
        return m;
    }

    LogMessage Fatal(std::string_view msg) {
        LogMessage m;
        m.indent = indent;
        m.Fatal(msg);
        return m;
    }

    LogMessage WithField(std::string_view key, std::string_view value) {
        LogMessage m;
        m.indent = indent;
        m.WithField(key, value);
        return m;
    }

    template<typename... Args>
    LogMessage Debugf(const char* fmt, Args&&... args) {
        return Debug(fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    LogMessage Infof(const char* fmt, Args&&... args) {
        return Info(fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    LogMessage Warnf(const char* fmt, Args&&... args) {
        return Warn(fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    LogMessage Errorf(const char* fmt, Args&&... args) {
        return Error(fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    LogMessage Fatalf(const char* fmt, Args&&... args) {
        return Fatal(fmt::format(fmt, std::forward<Args>(args)...));
    }

    };

    extern Xlogger xlogger;
        
}  // namespace xlog
