#pragma once
/*
==============================================================================

Misc.h

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
#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include <atomic>

#ifdef NDEBUG    // asserts disabled
static constexpr bool ndebug = true;
#else            // asserts enabled
static constexpr bool ndebug = false;
#endif

namespace RSJ {
    class spinlock {
        std::atomic_flag flag{ATOMIC_FLAG_INIT};
    public:
        void lock() noexcept
        {
            while (flag.test_and_set(std::memory_order_acquire))
                /*empty statement--spin until flag is cleared*/;
        }
        void unlock() noexcept
        {
            flag.clear(std::memory_order_release);
        }
    };
}

#endif  // MISC_H_INCLUDED
