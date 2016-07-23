#pragma once
/*
==============================================================================

Utilities.h
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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
#ifdef NDEBUG    // asserts disabled
static constexpr bool ndebug = true;
#else            // asserts enabled
static constexpr bool ndebug = false;
#endif

#include <atomic>
#include <cctype>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <utility>
namespace RSJ {
  template <typename T>
  struct counter {
    static std::atomic_int objects_created;
    static std::atomic_int objects_alive;

    counter() noexcept {
      ++objects_created;
      ++objects_alive;
    }

    counter(const counter&) noexcept {
      ++objects_created;
      ++objects_alive;
    }
  protected:
    ~counter() // objects should never be removed through pointers of this type
    {
      --objects_alive;
    }
  };
  template <typename T> std::atomic_int counter<T>::objects_created(0);
  template <typename T> std::atomic_int counter<T>::objects_alive(0);

/*
Usage:

class X :  RSJ::counter<X>
{
// ...
};

*/

  class spinlock {
    std::atomic_flag flag{ATOMIC_FLAG_INIT};
  public:
    inline void lock() noexcept {
      while (flag.test_and_set(std::memory_order_acquire))
        /*empty statement--spin until flag is cleared*/;
    }
    inline void unlock() noexcept {
      flag.clear(std::memory_order_release);
    }
  };

/* Usage
void foo()
{
static RSJ::spinlock lock;
lock_guard<RSJ::spinlock> guard(lock);
// do job
}
*/

  template<typename T>
  class threadsafe_queue {
  private:
    bool other_notification_{false};
    mutable std::mutex mut_;
    std::condition_variable data_cond_;
    std::queue<T> data_queue_;
  public:
    threadsafe_queue() {}
    void NotifyOther() {
      other_notification_ = true;
      data_cond_.notify_all();
    }
    void push(T new_value) {
      std::lock_guard<std::mutex> lk(mut_);
      data_queue_.push(std::move(new_value))
        ; data_cond_.notify_one();
    }
    void wait_and_pop(T& value) {
      std::unique_lock<std::mutex> lk(mut_);
      data_cond_.wait(lk, [this] {return !data_queue_.empty(); });
      value = std::move(data_queue_.front());
      data_queue_.pop();
    }
    std::shared_ptr<T> wait_and_pop() {
      std::unique_lock<std::mutex> lk(mut_);
      data_cond_.wait(lk, [this] {return !data_queue_.empty(); });
      std::shared_ptr<T> res(
        std::make_shared<T>(std::move(data_queue_.front())));
      data_queue_.pop();
      return res;
    }
    bool try_pop(T& value) {
      std::lock_guard<std::mutex> lk(mut_);
      if (data_queue_.empty())
        return false;
      value = std::move(data_queue_.front());
      data_queue_.pop();
      return true;
    }
    std::shared_ptr<T> try_pop() {
      std::lock_guard<std::mutex> lk(mut_);
      if (data_queue_.empty())
        return std::shared_ptr<T>();
      std::shared_ptr<T> res(
        std::make_shared<T>(std::move(data_queue_.front())));
      data_queue_.pop();
      return res;
    }
    bool empty() const {
      std::lock_guard<std::mutex> lk(mut_);
      return data_queue_.empty();
    }
  };

  static const std::string space = " \t\n\v\f\r";
  static const std::string blank = " \t";
  static const std::string digit = "0123456789";
  std::string trim(const std::string& str, const std::string& what = RSJ::space);
  std::string ltrim(const std::string& str, const std::string& what = RSJ::space);
  std::string rtrim(const std::string& str, const std::string& what = RSJ::space);
  inline bool contains(const std::string& str, const std::string& what = RSJ::space) {
    return str.find_first_of(what) != std::string::npos;
  }
}
