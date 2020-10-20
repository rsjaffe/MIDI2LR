#ifndef MIDI2LR_CONCURRENCY_H_INCLUDED
#define MIDI2LR_CONCURRENCY_H_INCLUDED
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
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <random>
#include <thread>
#include <type_traits>

extern "C" {
extern void _mm_pause();
}

namespace rsj {
   /* from http://prng.di.unimi.it/splitmix64.c, made state atomically updated and added methods to
    * satisfy concept std::uniform_random_bit_generator */

   class PRNG {
    public:
      using result_type = uint64_t;
      static result_type NextRandom() noexcept
      {
         result_type z {
             state_.fetch_add(0x9e3779b97f4a7c15, std::memory_order_relaxed) + 0x9e3779b97f4a7c15};
         z = (z ^ z >> 30) * 0xbf58476d1ce4e5b9;
         z = (z ^ z >> 27) * 0x94d049bb133111eb;
         return z ^ z >> 31;
         static_assert(std::is_unsigned_v<decltype(z)>, "Avoid sign extension");
      }
      [[nodiscard]] constexpr result_type min() const noexcept
      {
         return std::numeric_limits<result_type>::min();
      }
      [[nodiscard]] constexpr result_type max() const noexcept
      {
         return std::numeric_limits<result_type>::max();
      }
      result_type operator()() noexcept
      {
         return NextRandom();
      }

    private:
      alignas(128) static std::atomic<result_type> state_;
   };
   /* have to separate declaration from definition due to MSVC bug when std:c++latest
    * https://developercommunity.visualstudio.com/content/problem/1079261/alignas-not-accepted-when-applied-to-inline-static.html
    */
   alignas(128) inline std::atomic<PRNG::result_type> PRNG::state_ {[] {
      static_assert(sizeof(std::random_device::result_type) * 2 == sizeof(result_type)
                    && sizeof(std::random_device::result_type) == sizeof(uint32_t));
      auto rd {std::random_device {}};
      return static_cast<result_type>(rd()) << 32 | static_cast<result_type>(rd());
   }()};

   class SpinLock {
    public:
      SpinLock() noexcept = default;
      ~SpinLock() = default;
      SpinLock(const SpinLock& other) = delete;
      SpinLock(SpinLock&& other) = delete;
      SpinLock& operator=(const SpinLock& other) = delete;
      SpinLock& operator=(SpinLock&& other) = delete;
      void lock() noexcept
      { /* if uncontested, don't bother with exponential back-off */
         if (!flag_.load(std::memory_order_relaxed)
             && !flag_.exchange(true, std::memory_order_acquire))
            return;
         using LoopT = std::uint64_t;
         const auto [bo1, bo2, bo3] {[]() noexcept -> std::tuple<LoopT, LoopT, LoopT> {
            static_assert(std::is_unsigned_v<
                              PRNG::result_type> && sizeof(PRNG::result_type) >= sizeof(uint16_t));
            const auto rn {PRNG::NextRandom()};
            return {1 + (rn & 0b11), 12 + (rn >> 2 & 0b111), 56 + (rn >> 5 & 0b1111)};
         }()};
         do {
            /* avoid cache invalidation if lock appears to be unavailable */
            for (LoopT k {0}; flag_.load(std::memory_order_relaxed); ++k) {
               using namespace std::chrono_literals;
               if (k < bo1)
                  continue;
               if (k < bo2)
                  _mm_pause();
               else if (k < bo3)
                  std::this_thread::yield();
               else
                  std::this_thread::sleep_for(1ms);
            }
         } while (flag_.exchange(true, std::memory_order_acquire));
      }
      bool try_lock() noexcept
      {
         /* avoid cache invalidation if lock appears to be unavailable */
         if (flag_.load(std::memory_order_relaxed))
            return false;
         return !flag_.exchange(true, std::memory_order_acquire); /* try to acquire lock */
      }
      void unlock() noexcept
      {
         flag_.store(false, std::memory_order_release);
      }

    private: /* see https://stackoverflow.com/a/52158819/5699329 for 128 value */
      alignas(128) std::atomic<bool> flag_ {false};
      static_assert(sizeof(bool) == 1, "Padding assumes size of bool is 1.");
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
      [[maybe_unused]] alignas(1) std::byte padding_[127];
   };

   /* all but blocking pops use scoped_lock. blocking pops use unique_lock */
   template<typename T, class Container = std::deque<T>, class Mutex = std::mutex>
   class ConcurrentQueue {
    public:
      using container_type = Container;
      using value_type = typename Container::value_type;
      using size_type = typename Container::size_type;
      using reference = typename Container::reference;
      using const_reference = typename Container::const_reference;
      static_assert(std::is_same_v<T, value_type>, "container adapters require consistent types");
      /* Constructors: see https://en.cppreference.com/w/cpp/container/queue/queue. These are in
       * same order and number as in cppreference */
      /*1*/ ConcurrentQueue() noexcept(std::is_nothrow_default_constructible_v<Container>) {}
      /*2*/ explicit ConcurrentQueue(const Container& cont) noexcept(
          std::is_nothrow_copy_constructible_v<Container>)
          : queue_ {cont}
      {
      }
      /*3*/ explicit ConcurrentQueue(Container&& cont) noexcept(
          std::is_nothrow_move_constructible_v<Container>)
          : queue_ {std::exchange(cont, {})}
      {
      }
      /*4*/ ConcurrentQueue(const ConcurrentQueue& other)
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = other.queue_;
      }
      /*5*/ ConcurrentQueue(ConcurrentQueue&& other) noexcept(
          std::is_nothrow_move_constructible_v<Container>&& noexcept(
              std::scoped_lock(std::declval<Mutex>())))
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = std::exchange(other.queue_, {});
      }
      /*6*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      explicit ConcurrentQueue(const Alloc& alloc) noexcept(
          std::is_nothrow_constructible_v<Container, const Alloc&>)
          : queue_ {alloc}
      {
      }
      /*7*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      ConcurrentQueue(const Container& cont, const Alloc& alloc) : queue_ {cont, alloc}
      {
      }
      /*8*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      ConcurrentQueue(Container&& cont, const Alloc& alloc) noexcept(
          std::is_nothrow_constructible_v<Container, Container, const Alloc&>)
          : queue_(std::exchange(cont, {}), alloc)
      {
      }
      /*9*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      ConcurrentQueue(const ConcurrentQueue& other, const Alloc& alloc) : queue_(alloc)
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = Container(other.queue_, alloc);
      }
      /*10*/ template<class Alloc,
          class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      ConcurrentQueue(ConcurrentQueue&& other, const Alloc& alloc) noexcept(
          std::is_nothrow_constructible_v<Container, Container, const Alloc&>&& noexcept(
              std::scoped_lock(std::declval<Mutex>())))
          : queue_(alloc)
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = Container(std::exchange(other.queue_, {}), alloc);
      }
      /* operator= */
      ConcurrentQueue& operator=(const ConcurrentQueue& other)
      {
         {
            auto lock {std::scoped_lock(mutex_, other.mutex_)};
            queue_ = other.queue_;
         }
         condition_.notify_all();
         return *this;
      }
      ConcurrentQueue& operator=(ConcurrentQueue&& other) noexcept(
          std::is_nothrow_move_assignable_v<Container>&& noexcept(
              std::scoped_lock(std::declval<Mutex>())))
      {
         {
            auto lock {std::scoped_lock(mutex_, other.mutex_)};
            queue_ = std::exchange(other.queue_, {});
         }
         condition_.notify_all();
         return *this;
      }
      /* destructor */
      ~ConcurrentQueue() = default;
      /* methods */
      [[nodiscard]] auto empty() const noexcept(noexcept(
          std::declval<Container>().empty()) && noexcept(std::scoped_lock(std::declval<Mutex>())))
      {
         auto lock {std::scoped_lock(mutex_)};
         return queue_.empty();
      }
      [[nodiscard]] size_type size() const noexcept(noexcept(
          std::declval<Container>().size()) && noexcept(std::scoped_lock(std::declval<Mutex>())))
      {
         auto lock {std::scoped_lock(mutex_)};
         return queue_.size();
      }
      [[nodiscard]] size_type max_size() const
          noexcept(noexcept(std::declval<Container>().max_size()) && noexcept(
              std::scoped_lock(std::declval<Mutex>())))
      {
         auto lock {std::scoped_lock(mutex_)};
         return queue_.max_size();
      }
      void push(const T& value)
      {
         {
            auto lock {std::scoped_lock(mutex_)};
            queue_.push_back(value);
         }
         condition_.notify_one();
      }
      void push(T&& value)
      {
         {
            auto lock {std::scoped_lock(mutex_)};
            queue_.push_back(std::move(value));
         }
         condition_.notify_one();
      }
      template<class... Args> void emplace(Args&&... args)
      {
         {
            auto lock {std::scoped_lock(mutex_)};
            queue_.emplace_back(std::forward<Args>(args)...);
         }
         condition_.notify_one();
      }
      T pop()
      {
         auto lock {std::unique_lock(mutex_)};
         while (queue_.empty())
            condition_.wait(lock);
         T rc {std::move(queue_.front())};
         queue_.pop_front();
         return rc;
      }
      std::optional<T> try_pop()
      {
         auto lock {std::scoped_lock(mutex_)};
         if (queue_.empty())
            return std::nullopt;
         T rc {std::move(queue_.front())};
         queue_.pop_front();
         return rc;
      }
      void swap(ConcurrentQueue& other) noexcept(std::is_nothrow_swappable_v<Container>&& noexcept(
          std::scoped_lock(std::declval<Mutex>())))
      {
         {
            auto lock {std::scoped_lock(mutex_, other.mutex_)};
            queue_.swap(other.queue_);
         }
         condition_.notify_all();
         other.condition_.notify_all();
      }
      void resize(size_type count)
      {
         auto lock {std::scoped_lock(mutex_)};
         queue_.resize(count);
      }
      void resize(size_type count, const value_type& value)
      {
         {
            auto lock {std::scoped_lock(mutex_)};
            queue_.resize(count, value);
         }
         condition_.notify_all();
      }
      void clear() noexcept(noexcept(std::declval<Container>().clear()) && noexcept(
          std::scoped_lock(std::declval<Mutex>())))
      {
         auto lock {std::scoped_lock(mutex_)};
         queue_.clear();
      }
      [[nodiscard]] size_type
      clear_count() noexcept(noexcept(std::declval<Container>().clear()) && noexcept(
          std::declval<Container>().size()) && noexcept(std::scoped_lock(std::declval<Mutex>())))
      {
         auto lock {std::scoped_lock(mutex_)};
         const auto ret {queue_.size()};
         queue_.clear();
         return ret;
      }
      size_type clear_count_push(const T& value)
      {
         size_type ret;
         {
            auto lock {std::scoped_lock(mutex_)};
            ret = queue_.size();
            queue_.clear();
            queue_.push_back(value);
         }
         condition_.notify_one();
         return ret;
      }
      size_type clear_count_push(T&& value)
      {
         size_type ret;
         {
            auto lock {std::scoped_lock(mutex_)};
            ret = queue_.size();
            queue_.clear();
            queue_.push_back(std::move(value));
         }
         condition_.notify_one();
         return ret;
      }
      template<class... Args> auto clear_count_emplace(Args&&... args)
      {
         size_type ret;
         {
            auto lock {std::scoped_lock(mutex_)};
            ret = queue_.size();
            queue_.clear();
            queue_.emplace_back(std::forward<Args>(args)...);
         }
         condition_.notify_one();
         return ret;
      }

    private:
      Container queue_ {};
      mutable std::conditional_t<std::is_same_v<Mutex, std::mutex>, std::condition_variable,
          std::condition_variable_any>
          condition_ {};
      mutable Mutex mutex_ {};
   };
} // namespace rsj
#endif
