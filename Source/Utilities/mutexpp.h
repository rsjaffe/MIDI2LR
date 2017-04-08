#pragma once
/******************************************************************************/
// Mutex adaptors by Foster Brereton.
//
// Distributed under the MIT License. (See accompanying LICENSE.md or copy at
// https://opensource.org/licenses/MIT)
// defines and whitespace removed by Rory Jaffe
/******************************************************************************/

#ifndef MUTEXPP_HPP__
#define MUTEXPP_HPP__
#include <atomic>
#include <chrono>

namespace mutexpp {
    using clock_t = std::chrono::high_resolution_clock;
    using tp_t = clock_t::time_point;
    using diff_t = decltype((std::declval<tp_t>() - std::declval<tp_t>()).count());

    class spin_mutex {
    private:
        std::atomic_flag _lock = ATOMIC_FLAG_INIT;

    public:
        bool try_lock()
        {
            return !_lock.test_and_set(std::memory_order_acquire);
        }

        void lock()
        {
            std::size_t spin_count{0};
            while (!try_lock()) {
                ++spin_count;
            }
        }

        void unlock()
        {
            _lock.clear(std::memory_order_release);
        }
    };

    class adaptive_spin_mutex {
    private:
        std::atomic_flag         _lock = ATOMIC_FLAG_INIT;
        std::atomic<std::size_t> _spin_pred{0};

    public:
        bool try_lock()
        {
            return !_lock.test_and_set(std::memory_order_acquire);
        }

        void lock()
        {
            std::size_t spin_count{0};
            while (!try_lock()) {
                ++spin_count;
                if (spin_count < _spin_pred * 2)
                    continue;
                // REVISIT (fbrereto) : Iff sleep_for is guaranteed to block even
                // when the duration is 0, then std::chrono::nanoseconds(0) will
                // suffice here.
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            _spin_pred += (spin_count - _spin_pred) / 8;
        }

        void unlock()
        {
            _lock.clear(std::memory_order_release);
        }
    };

    class adaptive_block_mutex {
    private:
        std::atomic_flag    _lock = ATOMIC_FLAG_INIT;
        std::atomic<diff_t> _lock_pred{0};
        tp_t                _lock_start;

    public:
        bool try_lock()
        {
            return !_lock.test_and_set(std::memory_order_acquire);
        }

        void lock()
        {
            while (!try_lock()) {
                std::this_thread::sleep_for(tp_t::duration(_lock_pred));
            }
            _lock_start = clock_t::now();
        }

        void unlock()
        {
            _lock_pred += ((clock_t::now() - _lock_start).count() - _lock_pred) / 8;
            _lock.clear(std::memory_order_release);
        }
    };
} // namespace mutexpp
#endif // MUTEXPP_HPP__
