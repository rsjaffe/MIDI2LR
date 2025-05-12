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
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <type_traits>
#include <utility>

namespace rsj {
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
      /*1*/ ConcurrentQueue()
          noexcept(std::is_nothrow_default_constructible_v<Container>) = default;
      /*2*/ explicit ConcurrentQueue(const Container& cont)
          noexcept(std::is_nothrow_copy_constructible_v<Container>)
          : queue_ {cont}
      {
      }

      /*3*/ explicit ConcurrentQueue(Container&& cont)
          noexcept(std::is_nothrow_move_constructible_v<Container>)
          : queue_ {std::exchange(cont, {})}
      {
      }

      /*4*/ ConcurrentQueue(const ConcurrentQueue& other)
          noexcept(std::is_nothrow_copy_constructible_v<Container>
                   && noexcept(std::scoped_lock(mutex_)))
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = other.queue_;
      }

      /*5*/ ConcurrentQueue(ConcurrentQueue&& other)
          noexcept(std::is_nothrow_move_constructible_v<Container>
                   && noexcept(std::scoped_lock(mutex_)))
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = std::exchange(other.queue_, {});
      }

      /*6*/ template<class Alloc>
      explicit ConcurrentQueue(const Alloc& alloc)
          noexcept(std::is_nothrow_constructible_v<Container, const Alloc&>)
         requires(std::uses_allocator_v<Container, Alloc>)
          : queue_ {alloc}
      {
      }

      /*7*/ template<class Alloc>
      ConcurrentQueue(const Container& cont, const Alloc& alloc)
         requires(std::uses_allocator_v<Container, Alloc>)
          : queue_ {cont, alloc}
      {
      }

      /*8*/ template<class Alloc>
      ConcurrentQueue(Container&& cont, const Alloc& alloc)
          noexcept(std::is_nothrow_constructible_v<Container, Container, const Alloc&>)
         requires(std::uses_allocator_v<Container, Alloc>)
          : queue_(std::exchange(cont, {}), alloc)
      {
      }

      /*9*/ template<class Alloc>
      ConcurrentQueue(const ConcurrentQueue& other, const Alloc& alloc)
         requires(std::uses_allocator_v<Container, Alloc>)
          : queue_(alloc)
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = Container(other.queue_, alloc);
      }

      /*10*/ template<class Alloc>
      ConcurrentQueue(ConcurrentQueue&& other, const Alloc& alloc)
          noexcept(std::is_nothrow_constructible_v<Container, Container, const Alloc&>
                   && noexcept(std::scoped_lock(mutex_)))
         requires(std::uses_allocator_v<Container, Alloc>)
          : queue_(alloc)
      {
         auto lock {std::scoped_lock(other.mutex_)};
         queue_ = Container(std::exchange(other.queue_, {}), alloc);
      }

      /* operator= */
      ConcurrentQueue& operator=(const ConcurrentQueue& other)
          noexcept(std::is_nothrow_copy_assignable_v<Container>
                   && noexcept(std::scoped_lock(mutex_)))
      {
         {
            auto lock {std::scoped_lock(mutex_, other.mutex_)};
            queue_ = other.queue_;
         }
         condition_.notify_all();
         return *this;
      }

      ConcurrentQueue& operator=(ConcurrentQueue&& other)
          noexcept(std::is_nothrow_move_assignable_v<Container>
                   && noexcept(std::scoped_lock(mutex_)))
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
      [[nodiscard]] auto empty() const
          noexcept(noexcept(queue_.empty()) && noexcept(std::scoped_lock(mutex_)))
      {
         auto lock {std::scoped_lock(mutex_)};
         return queue_.empty();
      }

      [[nodiscard]] size_type size() const
          noexcept(noexcept(queue_.size()) && noexcept(std::scoped_lock(mutex_)))
      {
         auto lock {std::scoped_lock(mutex_)};
         return queue_.size();
      }

      [[nodiscard]] size_type max_size() const
          noexcept(noexcept(queue_.max_size()) && noexcept(std::scoped_lock(mutex_)))
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
            T new_item {std::forward<Args>(args)...};
            auto lock {std::scoped_lock(mutex_)};
            queue_.push_back(std::move(new_item));
         }
         condition_.notify_one();
      }

      T pop()
      {
         auto lock {std::unique_lock(mutex_)};
         while (queue_.empty()) { condition_.wait(lock); }
         T rc {std::move(queue_.front())};
         queue_.pop_front();
         return rc;
      }

      std::optional<T> try_pop()
      {
         auto lock {std::scoped_lock(mutex_)};
         if (queue_.empty()) { return std::nullopt; }
         T rc {std::move(queue_.front())};
         queue_.pop_front();
         return rc;
      }

      void swap(ConcurrentQueue& other)
          noexcept(std::is_nothrow_swappable_v<Container> && noexcept(std::scoped_lock(mutex_)))
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

      void clear()
          noexcept(std::is_nothrow_default_constructible_v<Container>
                   && std::is_nothrow_destructible_v<Container>
                   && std::is_nothrow_swappable_v<Container> && noexcept(std::scoped_lock(mutex_)))
      { /*https://devblogs.microsoft.com/oldnewthing/20201112-00/?p=104444 */
         Container trash {};
         auto lock {std::scoped_lock(mutex_)};
         std::swap(trash, queue_);
      }

      [[nodiscard]] size_type clear_count()
          noexcept(std::is_nothrow_default_constructible_v<Container>
                   && std::is_nothrow_destructible_v<Container>
                   && std::is_nothrow_swappable_v<Container> && noexcept(queue_.size())
                   && noexcept(std::scoped_lock(mutex_)))
      {
         Container trash {};
         {
            auto lock {std::scoped_lock(mutex_)};
            std::swap(trash, queue_);
         }
         return trash.size();
      }

      size_type clear_count_push(const T& value)
      {
         Container trash {};
         {
            auto lock {std::scoped_lock(mutex_)};
            std::swap(trash, queue_);
            queue_.push_back(value);
         }
         condition_.notify_one();
         return trash.size();
      }

      size_type clear_count_push(T&& value)
      {
         Container trash {};
         {
            auto lock {std::scoped_lock(mutex_)};
            std::swap(trash, queue_);
            queue_.push_back(std::move(value));
         }
         condition_.notify_one();
         return trash.size();
      }

      template<class... Args> auto clear_count_emplace(Args&&... args)
      {
         Container trash {};
         {
            T new_item {std::forward<Args>(args)...};
            auto lock {std::scoped_lock(mutex_)};
            std::swap(trash, queue_);
            queue_.push_back(std::move(new_item));
         }
         condition_.notify_one();
         return trash.size();
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
