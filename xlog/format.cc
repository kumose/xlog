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

namespace xlog
{

    ///
    /// @brief Build default log theme with level-specific styles
    /// Keep consistent with original FTXUI version: bold + fixed RGB colors for each log level
    /// @return LevelFormater Configured formatter with default theme styles
    ///
    LevelFormater DefaultTheme()
    {
        LevelFormater f;

        // ------------------------------
        // DEBUG level style configuration
        // Style: bold + foreground RGB(94, 101, 85)
        // ------------------------------
        f.debug.bold(true)
            .foreground(94, 101, 85)
            .build(); // Match original FTXUI Color::RGB(94, 101, 85)

        // ------------------------------
        // INFO level style configuration
        // Style: bold + foreground RGB(4, 124, 140)
        // ------------------------------
        f.info.bold(true)
            .foreground(4, 124, 140)
            .build(); // Match original FTXUI Color::RGB(4, 124, 140)

        // ------------------------------
        // WARN level style configuration
        // Style: bold + foreground RGB(194, 156, 5)
        // ------------------------------
        f.warn.bold(true)
            .foreground(194, 156, 5)
            .build(); // Match original FTXUI Color::RGB(194, 156, 5)

        // ------------------------------
        // ERROR level style configuration
        // Style: bold + foreground RGB(141, 35, 6)
        // ------------------------------
        f.error.bold(true)
            .foreground(141, 35, 6)
            .build(); // Match original FTXUI Color::RGB(141, 35, 6)

        // ------------------------------
        // FATAL level style configuration
        // Style: bold + foreground RGB(246, 60, 9)
        // ------------------------------
        f.fatal.bold(true)
            .foreground(246, 60, 9)
            .build(); // Match original FTXUI Color::RGB(246, 60, 9)

        return f;
    }
} // namespace xlog
