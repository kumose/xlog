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


#include <xlog/format.h>
#include <ftxui/screen/color.hpp>
#include "ftxui/screen/screen.hpp"

namespace xlog {

LevelFormater DefaultTheme() {
    LevelFormater f;

    using namespace ftxui;

    f.debug = [](std::string msg) {
        return make_xlog_style(msg, bold, Color::RGB(94, 101, 85));
    };

    f.info = [](std::string msg) {
        return make_xlog_style(msg, bold, Color::RGB(4, 124, 140));
    };

    f.warn = [](std::string msg) {
        return make_xlog_style(msg, bold, Color::RGB(194, 156, 5));
    };

    f.error = [](std::string msg) {
        return make_xlog_style(msg, bold, Color::RGB(141, 35, 6));
    };

    f.fatal = [](std::string msg) {
        return make_xlog_style(msg, bold, Color::RGB(246, 60, 9));
    };

    return f;
}

    LevelFormater Formatter = DefaultTheme();
}  // namespace xlog
