// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

Utilities.cpp
This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
==============================================================================
*/
#include "Utilities.h"

std::string RSJ::trim(const std::string& str, const std::string& what) {
  auto front = str.find_first_not_of(what);
  if (front == std::string::npos)
    return std::string();
  auto back = str.find_last_not_of(what);
  return str.substr(front, back - front + 1);
}

std::string RSJ::ltrim(const std::string& str, const std::string& what) {
  auto front = str.find_first_not_of(what);
  if (front == std::string::npos)
    return std::string();
  return str.substr(front);
}

std::string RSJ::rtrim(const std::string& str, const std::string& what) {
  auto back = str.find_last_not_of(what);
  if (back == std::string::npos)
    return std::string();
  return str.substr(0, back + 1);
}