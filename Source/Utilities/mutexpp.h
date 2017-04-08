#pragma once
/******************************************************************************/
// Mutex adaptors by Foster Brereton.
//
// Distributed under the MIT License. (See accompanying LICENSE.md or copy at
// https://opensource.org/licenses/MIT)
/******************************************************************************/

#ifndef MUTEXPP_HPP__
#define MUTEXPP_HPP__

/******************************************************************************/

#include <atomic>
#include <chrono>

/******************************************************************************/

namespace mutexpp {
    /******************************************************************************/

    using clock_t = std::chrono::high_resolution_clock;
    using tp_t = clock_t::time_point;
    using diff_t = decltype((std::declval<tp_t>() - std::declval<tp_t>()).count());

#if MUTEXPP_ENABLE_PROBE
    using duration_t = std::chrono::duration<double, std::micro>;
    using probe_t = void(*)(bool        did_block,
        std::size_t spin_count,
        duration_t  block_count);
#endif

    /******************************************************************************/

    class spin_mutex_t {
    private:
        std::atomic_flag _lock = ATOMIC_FLAG_INIT;

    public:
#if MUTEXPP_ENABLE_PROBE
        probe_t _probe{nullptr};
#endif

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

#if MUTEXPP_ENABLE_PROBE
            if (_probe) {
                static const duration_t zero_k{std::chrono::duration_cast<duration_t>(tp_t::duration(0))};

                _probe(false, spin_count, zero_k);
            }
#endif
        }

        void unlock()
        {
            _lock.clear(std::memory_order_release);
        }
    };

    /******************************************************************************/

    class adaptive_spin_mutex_t {
    private:
        std::atomic_flag         _lock = ATOMIC_FLAG_INIT;
        std::atomic<std::size_t> _spin_pred{0};

    public:
#if MUTEXPP_ENABLE_PROBE
        probe_t _probe{nullptr};
#endif

        bool try_lock()
        {
            return !_lock.test_and_set(std::memory_order_acquire);
        }

        void lock()
        {
#if MUTEXPP_ENABLE_PROBE
            bool   did_block{false};
#endif
            std::size_t spin_count{0};

            while (!try_lock()) {
                ++spin_count;

                if (spin_count < _spin_pred * 2)
                    continue;

                // REVISIT (fbrereto) : Iff sleep_for is guaranteed to block even
                // when the duration is 0, then std::chrono::nanoseconds(0) will
                // suffice here.
                std::this_thread::sleep_for(std::chrono::microseconds(1));

#if MUTEXPP_ENABLE_PROBE
                did_block = true;
#endif
            }

            _spin_pred += (spin_count - _spin_pred) / 8;

#if MUTEXPP_ENABLE_PROBE
            if (_probe) {
                static const duration_t zero_k{std::chrono::duration_cast<duration_t>(tp_t::duration(0))};
                _probe(did_block, _spin_pred, zero_k);
            }
#endif
        }

        void unlock()
        {
            _lock.clear(std::memory_order_release);
        }
    };

    /******************************************************************************/

    class adaptive_block_mutex_t {
    private:
        std::atomic_flag    _lock = ATOMIC_FLAG_INIT;
        std::atomic<diff_t> _lock_pred{0};
        tp_t                _lock_start;

    public:
#if MUTEXPP_ENABLE_PROBE
        probe_t _probe{nullptr};
#endif

        bool try_lock()
        {
            return !_lock.test_and_set(std::memory_order_acquire);
        }

        void lock()
        {
#if MUTEXPP_ENABLE_PROBE
            bool did_block{false};
#endif

            while (!try_lock()) {
                std::this_thread::sleep_for(tp_t::duration(_lock_pred));

#if MUTEXPP_ENABLE_PROBE
                did_block = true;
#endif
            }

#if MUTEXPP_ENABLE_PROBE
            if (_probe)
                _probe(did_block,
                    0,
                    std::chrono::duration_cast<duration_t>(tp_t::duration(_lock_pred)));
#endif

            _lock_start = clock_t::now();
        }

        void unlock()
        {
            _lock_pred += ((clock_t::now() - _lock_start).count() - _lock_pred) / 8;

            _lock.clear(std::memory_order_release);
        }
    };

    /******************************************************************************/
} // namespace mutexpp

  /******************************************************************************/

#endif // MUTEXPP_HPP__

  /******************************************************************************/