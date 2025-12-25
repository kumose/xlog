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

#include <functional>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <string_view>
#include <vector>

namespace xlog
{

    struct FormatStyle
    {
        std::string LeftPadding{0};
        std::string RightPadding{0};
        std::string Value{};
        std::vector<fmt::text_style> Elements;
        fmt::text_style Style = fmt::text_style();

        FormatStyle &left_padding(int n, char c = ' ')
        {
            LeftPadding = std::string(n, c);
            return *this;
        }
        FormatStyle &right_padding(int n, char c = ' ')
        {
            RightPadding = std::string(n, c);
            return *this;
        }
        FormatStyle &set_value(std::string_view msg)
        {
            Value = msg;
            return *this;
        }
        FormatStyle &bold(bool b)
        {
            if (b)
            {
                Elements.push_back(fmt::emphasis::bold);
            }
            return *this;
        }

        FormatStyle &foreground(fmt::color col)
        {
            Elements.push_back(fmt::fg(col));
            return *this;
        }

        FormatStyle &foreground(fmt::text_style d)
        {
            Elements.push_back(d);
            return *this;
        }

        FormatStyle &foreground(int r, int g, int b)
        {
            Elements.push_back(fmt::fg(fmt::rgb(r,g,b)));
            return *this;
        }
        FormatStyle &background(int r, int g, int b)
        {
            Elements.push_back(fmt::bg(fmt::rgb(r,g,b)));
            return *this;
        }

        FormatStyle &background(fmt::text_style d)
        {
            Elements.push_back(d);
            return *this;
        }

        FormatStyle &background(fmt::color col)
        {
            Elements.push_back(fmt::bg(col));
            return *this;
        }

        void build()
        {
            for (auto &decorator : Elements)
            {
                Style |= decorator;
            }
        }

        std::string render(const std::string &msg)
        {
            std::string content;
            if (!Value.empty())
            {
                content = LeftPadding + Value + " " + msg + RightPadding;
            }
            else
            {
                content = LeftPadding + msg + RightPadding;
            }

            return fmt::format(Style, "{}", content);
        }

        static std::string repeat(int n);
    };

    struct LevelFormater
    {
        FormatStyle debug;
        FormatStyle info;
        FormatStyle warn;
        FormatStyle error;
        FormatStyle fatal;
    };
    LevelFormater DefaultTheme();

    inline std::string make_xlog_style(
        const std::string &msg,
        fmt::text_style deco,
        fmt::color col)
    {
        return fmt::format(deco | fmt::fg(col), "{}", msg);
    }
} // namespace xlog
