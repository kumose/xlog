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

#include <xlog/logger.h>
#include <functional>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <string_view>
#include <vector>


namespace xlog {

    using StringFormater = std::function<std::string(std::string)>;

    struct LevelFormater {
        StringFormater debug;
        StringFormater info;
        StringFormater warn;
        StringFormater error;
        StringFormater fatal;
    };

    extern LevelFormater Formatter;
        
    inline std::string make_xlog_style(
        const std::string& msg,
        const ftxui::Decorator& deco,
        const ftxui::Color& col) {

        using namespace ftxui;

        auto element = text(msg) | deco | ftxui::color(col);

        auto screen = Screen::Create(Dimension::Fit(element));
        Render(screen, element);
        return screen.ToString();
    }
    
}  // namespace xlog
