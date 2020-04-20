#ifndef MIDI2_LR_OCPP_H_INCLUDED
#define MIDI2_LR_OCPP_H_INCLUDED
/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _WIN32
#include <string>
#include <unordered_map>
#include <utility>
using UniChar = uint16_t;
using pid_t = int;

namespace rsj {
   [[nodiscard]] std::string AppDataMac();
   [[nodiscard]] std::string AppLogMac();
   [[nodiscard]] std::unordered_map<UniChar, std::pair<size_t, bool>> GetKeyMap();
   void CheckPermission(pid_t pid);
} // namespace rsj
#endif
#endif
