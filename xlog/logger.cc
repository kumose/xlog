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

#include <iostream>
#include <xlog/logger.h>
#include <xlog/format.h>
#include <sstream>

namespace xlog
{

    LogConfig::LogConfig():Format(DefaultTheme()), File(stderr) {

    }

    LogConfig defualt_config;


    void enable_log_level_string() {
        defualt_config.LogLevelString = true;
    }

    void disable_log_level_string() {
        defualt_config.LogLevelString = false;
    }

    void set_log_config(LogConfig conf)
    {
        defualt_config = defualt_config;
    }

    const LogConfig &get_log_config()
    {
        return defualt_config;
    }

    LogMessage::~LogMessage()
    {
        do_log();
    }

    void LogMessage::do_log()
    {
        using namespace xlog;
        auto &f = defualt_config.Format;

        // styled icon
        std::string icon = std::string(defualt_config.Icons[level_index(level)]);
        if (defualt_config.LogLevelString) {
            icon += " " + std::string(defualt_config.LevelString[level_index(level)]);
        }
        auto l = level;
        FormatStyle &fn = f.debug;
        switch (l)
        {
        case LogLevel::LEVEL_DEBUG:
            fn = f.debug;
            break;
        case LogLevel::LEVEL_INFO:
            fn = f.info;
            break;
        case LogLevel::LEVEL_WARN:
            fn = f.warn;
            break;
        case LogLevel::LEVEL_ERROR:
            fn = f.error;
            break;
        case LogLevel::LEVEL_FATAL:
            fn = f.fatal;
            break;
        default:
            fn = f.debug;
            break;
        }

        std::string styled_icon = fn.render(icon);

        std::string base_prefix = styled_icon + " " + indent;
        std::string line_prefix = indent + fn.render("│") + "    ";
        std::string key_prefix = indent + fn.render("│") + "  ";
        std::string msg_prefix = indent + "    ";

        // print message lines
        std::istringstream msg_stream(message);
        std::string msg_line;
        bool first_msg_line = true;
        while (std::getline(msg_stream, msg_line))
        {
            if (msg_line.empty())
                continue;
            if (first_msg_line)
            {
                fmt::print(defualt_config.File, "{}{}", base_prefix, msg_line);
                first_msg_line = false;
            } else {
                fmt::print(defualt_config.File, "\n{}{}", base_prefix, msg_line);
            }
        }

        // print fields

        std::vector<std::pair<std::string, std::string>> mfields;

        for (auto &[key, value] : fields)
        {
            std::string styled_key = fn.render(key);

            if (value.find("\n") != std::string::npos)
            {
                mfields.emplace_back(key, value);
                continue;
            }
            fmt::print(defualt_config.File,"\t{}={}",styled_key, value); 
        }

        for (auto &[key, value] : mfields)
        {
            std::string styled_key = fn.render(key);
            fmt::print(defualt_config.File,"\n{}{}=",key_prefix, styled_key); 
            std::istringstream val_stream(value);
            std::string val_line;
            while (std::getline(val_stream, val_line))
            {
                if (val_line.empty())
                {
                    fmt::print(defualt_config.File,"\n");
                    continue;
                }
                fmt::print(defualt_config.File,"\n{}{}=",line_prefix, val_line); 
            }
        }

        fmt::print(defualt_config.File,"\n");
    }

} // namespace xlog
