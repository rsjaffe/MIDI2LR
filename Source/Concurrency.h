#ifndef MIDI2LR_CONCURRENCY_H_INCLUDED
#define MIDI2LR_CONCURRENCY_H_INCLUDED
/*
==============================================================================

Concurrency.h

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
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <type_traits>

#ifdef _WIN32
#include <emmintrin.h>
#define CPU_RELAX _mm_pause()
#else
#define CPU_RELAX __builtin_ia32_pause()
#endif

namespace rsj {
   class RelaxTTasSpinLock {
    public:
      RelaxTTasSpinLock() noexcept = default;
      ~RelaxTTasSpinLock() = default;
      RelaxTTasSpinLock(const RelaxTTasSpinLock& other) = delete;
      RelaxTTasSpinLock(RelaxTTasSpinLock&& other) = delete;
      RelaxTTasSpinLock& operator=(const RelaxTTasSpinLock& other) = delete;
      RelaxTTasSpinLock& operator=(RelaxTTasSpinLock&& other) = delete;
      void lock() noexcept
      {
         do {
            while (flag_.load(std::memory_order_relaxed))
               CPU_RELAX; // spin without expensive exchange
         } while (flag_.exchange(true, std::memory_order_acquire));
      }
      bool try_lock() noexcept
      {
         if (flag_.load(std::memory_order_relaxed)) // avoid cache invalidation if lock
                                                    // appears to be unavailable
            return false;
         return !flag_.exchange(true, std::memory_order_acquire); // try to acquire lock
      }
      void unlock() noexcept
      {
         flag_.store(false, std::memory_order_release);
      }

    private:
      std::atomic<bool> flag_{false};
   };

   // all but blocking pops use scoped_lock. blocking pops use unique_lock
   template<typename T, class Container = std::deque<T>> class BlockingQueue {
    public:
      using container_type = Container;
      using value_type = typename Container::value_type;
      using size_type = typename Container::size_type;
      using reference = typename Container::reference;
      using const_reference = typename Container::const_reference;
      static_assert(std::is_same_v<T, value_type>, "container adaptors require consistent types");
      // Constructors: see https://en.cppreference.com/w/cpp/container/queue/queue
      // These are in same order and number as in cppreference
      /*1*/ BlockingQueue() noexcept(std::is_nothrow_default_constructible_v<Container>){};
      /*2*/ explicit BlockingQueue(const Container& cont) noexcept(
          std::is_nothrow_copy_constructible_v<Container>)
          : queue_{cont}
      {
      }
      /*3*/ explicit BlockingQueue(Container&& cont) noexcept(
          std::is_nothrow_move_constructible_v<Container>)
          : queue_{std::move(cont)}
      {
      }
      /*4*/ BlockingQueue(const BlockingQueue& other)
      {
         auto lock{std::scoped_lock(other.mutex_)};
         queue_ = other.queue_;
      }
      /*5*/ BlockingQueue(BlockingQueue&& other) noexcept(
          std::is_nothrow_move_constructible_v<BlockingQueue>)
      {
         auto lock{std::scoped_lock(other.mutex_)};
         queue_ = std::move(other.queue_);
      }
      /*6*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      explicit BlockingQueue(const Alloc& alloc) noexcept(
          std::is_nothrow_constructible_v<Container, const Alloc&>)
          : queue_{alloc}
      {
      }
      /*7*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      BlockingQueue(const Container& cont, const Alloc& alloc) : queue_{cont, alloc}
      {
      }
      /*8*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      BlockingQueue(Container&& cont, const Alloc& alloc) noexcept(
          std::is_nothrow_constructible_v<Container, Container, const Alloc&>)
          : queue_(std::move(cont), alloc)
      {
      }
      /*9*/ template<class Alloc, class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      BlockingQueue(const BlockingQueue& other, const Alloc& alloc) : queue_(alloc)
      {
         auto lock{std::scoped_lock(other.mutex_)};
         queue_ = other.queue_;
      }
      /*10*/ template<class Alloc,
          class = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
      BlockingQueue(BlockingQueue&& other, const Alloc& alloc) noexcept(
          std::is_nothrow_constructible_v<Container, Container, const Alloc&>)
          : queue_(alloc)
      {
         auto lock{std::scoped_lock(other.mutex_)};
         queue_ = std::move(other.queue_);
      }
      // operator=
      BlockingQueue& operator=(const BlockingQueue& other)
      {
         {
            auto lock{std::scoped_lock(mutex_, other.mutex_)};
            queue_ = other.queue_;
         }
         condition_.notify_all();
         return *this;
      }
      BlockingQueue& operator=(BlockingQueue&& other) noexcept(
          std::is_nothrow_move_assignable_v<Container>)
      {
         {
            auto lock{std::scoped_lock(mutex_, other.mutex_)};
            queue_ = std::move(other.queue_);
         }
         condition_.notify_all();
         return *this;
      }
      // destructor
      ~BlockingQueue() = default;
      // methods
      void push(const T& value)
      {
         {
            auto lock{std::scoped_lock(mutex_)};
            queue_.push_back(value);
         }
         condition_.notify_one();
      }
      void push(T&& value)
      {
         {
            auto lock{std::scoped_lock(mutex_)};
            queue_.push_back(std::move(value));
         }
         condition_.notify_one();
      }
      template<class... Args> void emplace(Args&&... args)
      {
         {
            auto lock{std::scoped_lock(mutex_)};
            queue_.emplace_back(std::forward<Args>(args)...);
         }
         condition_.notify_one();
      }
      T pop()
      {
         auto lock{std::unique_lock(mutex_)};
         condition_.wait(lock, [this]() noexcept(noexcept(std::declval<Container&>().empty())) {
            return !queue_.empty();
         });
         T rc{std::move(queue_.front())};
         queue_.pop_front();
         return rc;
      }
      [[nodiscard]] std::optional<T> try_pop()
      {
         auto lock{std::scoped_lock(mutex_)};
         if (queue_.empty())
            return std::nullopt;
         T rc{std::move(queue_.front())};
         queue_.pop_front();
         return rc;
      }
      void clear()
      {
         auto lock{std::scoped_lock(mutex_)};
         queue_.clear();
      }
      [[nodiscard]] auto size() const noexcept(noexcept(std::declval<Container&>().size()))
      {
         auto lock{std::scoped_lock(mutex_)};
         return queue_.size();
      }
      [[nodiscard]] auto clear_count()
      {
         auto lock{std::scoped_lock(mutex_)};
         auto ret = queue_.size();
         queue_.clear();
         return ret;
      }
      auto clear_count_push(const T& value)
      {
         size_type ret;
         {
            auto lock{std::scoped_lock(mutex_)};
            ret = queue_.size();
            queue_.clear();
            queue_.push_back(value);
         }
         condition_.notify_one();
         return ret;
      }
      auto clear_count_push(T&& value)
      {
         size_type ret;
         {
            auto lock{std::scoped_lock(mutex_)};
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
            auto lock{std::scoped_lock(mutex_)};
            ret = queue_.size();
            queue_.clear();
            queue_.emplace_back(std::forward<Args>(args)...);
         }
         condition_.notify_one();
         return ret;
      }
      [[nodiscard]] bool empty() const noexcept(noexcept(std::declval<Container&>().empty()))
      {
         auto lock{std::scoped_lock(mutex_)};
         return queue_.empty();
      }

    private:
      Container queue_{};
      mutable std::condition_variable condition_{};
      mutable std::mutex mutex_{};
   };
} // namespace rsj
#endif
