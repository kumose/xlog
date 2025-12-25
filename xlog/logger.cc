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

namespace xlog {

 LogMessage::~LogMessage() {
    do_log();
 }


 void LogMessage::do_log() {
    using namespace xlog;
    auto& f = Formatter;

    // styled icon
    std::string icon = std::string(DefaultStrings[level_index(level)]);
    auto l = level;
    auto fn = [l](std::string str) -> std::string {
        switch(l) {
            case LogLevel::LEVEL_DEBUG: return f.debug(str); 
            case LogLevel::LEVEL_INFO:  return f.info(str);
            case LogLevel::LEVEL_WARN:  return f.warn(str); 
            case LogLevel::LEVEL_ERROR: return f.error(str);
            case LogLevel::LEVEL_FATAL: return f.fatal(str);
            default: return std::string(str);
        }
    };
    
    std::string styled_icon = fn(icon);

    std::string base_prefix = styled_icon + " " + indent;
    std::string line_prefix = indent + fn("│") + "    ";
    std::string key_prefix = indent + fn("│") + "  ";
    std::string msg_prefix = indent + "    ";

    // print message lines
    std::istringstream msg_stream(message);
    std::string msg_line;
    bool first_msg_line = true;
    while (std::getline(msg_stream, msg_line)) {
        if (msg_line.empty()) continue;
        if (first_msg_line) {
            std::cout << base_prefix << msg_line;
            first_msg_line = false;
        } else {
            std::cout << "\n" << msg_prefix << msg_line;
        }
    }

    // print fields

     std::vector<std::pair<std::string, std::string>> mfields;

    for (auto& [key, value] : fields) {
        std::string styled_key = fn(key);
        

        if(value.find("\n") != std::string::npos) {
            mfields.emplace_back(key, value);
            continue;
        }
          std::cout <<"\t"<<styled_key << "=" << value;
    }

    for (auto& [key, value] : mfields) {
        std::string styled_key = fn(key);
        std::cout << "\n" << key_prefix << styled_key << "=";
        std::istringstream val_stream(value);
        std::string val_line;
        while (std::getline(val_stream, val_line)) {
            if (val_line.empty()) {
                std::cout << "\n";
                continue;
            }
            
            std::cout << "\n" << line_prefix << val_line;
        }
    }


    std::cout << "\n";
}



}  // namespace xlog
