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


#include <xlog/xlog.h>

int main() {
    using namespace xlog;

    // Simple debug
    xlogger.Debug("Starting the process");

    xlogger.Info("Loading configuration");

    xlogger.Warn("Configuration deprecated")
           .WithField("file", "config.yaml")
           ;

    xlogger.IncreamIndent();
    // Error with multi-line message
    xlogger.Error(
        "Failed to load module\nCheck the logs for details"
    ).WithField("module", "network");

    xlogger.Error(
        "Failed"
    ).WithField("message", "Failed to load module\nCheck the logs for details");

    xlogger.Info(
        "success!"
    ).WithField("message", "Failed to load module\nCheck the logs for details")
    .WithField("message1", "Failed to load module1\nCheck the logs for details1").WithField("module", "network");
     xlogger.IncreamIndent();
    xlogger.Fatal("Fatal error encountered");

    // Using formatted message
    int count = 5;
    xlog::enable_log_level_string();
    xlogger.Infof("Processed {} files successfully", count);

    return 0;
}
