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
#include "SendKeys.h"

namespace rsj {
   ActiveModifiers ActiveModifiers::FromWindows(const int from) noexcept
   {
      /* shift coded as follows: 1: shift, 2: ctrl, 4: alt, 8: hankaku */
      ActiveModifiers am {};
      am.alt_opt = from & 4;
      am.control = from & 2;
      am.hankaku = from & 8;
      am.shift = from & 1;
      return am;
   }

   ActiveModifiers ActiveModifiers::FromMidi2LR(const int from) noexcept
   {
      ActiveModifiers am {};
      am.alt_opt = from & 1;
      am.command = from & 8;
      am.control = from & 2;
      am.shift = from & 4;
      return am;
   }
} // namespace rsj