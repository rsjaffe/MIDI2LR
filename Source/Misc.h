#pragma once
/*
==============================================================================

Misc.h

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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

#ifdef _WIN32
#include <emmintrin.h>
#define CPU_RELAX _mm_pause()
#else
#define CPU_RELAX __builtin_ia32_pause()
#endif

namespace RSJ {
    class RelaxTTasSpinLock {
    public:
        RelaxTTasSpinLock() = default;
        ~RelaxTTasSpinLock() = default;
        RelaxTTasSpinLock(const RelaxTTasSpinLock& other) = delete;
        RelaxTTasSpinLock(RelaxTTasSpinLock&& other) = delete;
        RelaxTTasSpinLock& operator=(const RelaxTTasSpinLock& other) = delete;
        RelaxTTasSpinLock& operator=(RelaxTTasSpinLock&& other) = delete;
        void lock() noexcept
        {
            do {
                while (flag.load(std::memory_order_relaxed))
                    CPU_RELAX;//spin without expensive exchange
            } while (flag.exchange(true, std::memory_order_acquire));
        }

        bool try_lock() noexcept
        {
            if (flag.load(std::memory_order_relaxed)) //avoid cache invalidation if lock unavailable
                return false;
            return !flag.exchange(true, std::memory_order_acquire); //try to acquire lock
        }

        void unlock() noexcept
        {
            flag.store(false, std::memory_order_release);
        }

    private:
        std::atomic<bool> flag{false};
    };
}
#endif  // MISC_H_INCLUDED
