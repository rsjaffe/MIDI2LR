#pragma once
/*
==============================================================================

Utilities.h
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

#ifndef MIDI2LR_UTILITIES_H_INCLUDED
#define MIDI2LR_UTILITIES_H_INCLUDED

#include <atomic>
#include <cctype>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <utility>
namespace rsj {
    template <typename T>
    struct Counter {
        static std::atomic_int objects_created;
        static std::atomic_int objects_alive;

        Counter() noexcept
        { //constructor
            ++objects_created;
            ++objects_alive;
        }

        Counter(const Counter&) noexcept
        { //copy constructor
            ++objects_created;
            ++objects_alive;
        }

        Counter& operator=(Counter) noexcept
        {
            return *this;
        } //not a constructor, so no inc

        Counter(Counter&&) noexcept
        {} //don't increment on move

        Counter& operator=(Counter&&) noexcept
        {
            return *this;
        } //don't increment on move

    protected:
        ~Counter() // objects should never be removed through pointers of this type
        {
            --objects_alive;
        }
    };
    template <typename T> std::atomic_int Counter<T>::objects_created(0);
    template <typename T> std::atomic_int Counter<T>::objects_alive(0);

    /*
    Usage:

    class X :  rsj::counter<X>
    {
    // ...
    };

    */

    template<typename T>
    class ThreadsafeQueue {
    private:
        bool other_notification_{false};
        mutable std::mutex mut_;
        std::condition_variable data_cond_;
        std::queue<T> data_queue_;
    public:
        ThreadsafeQueue()
        {}
        void NotifyOther()
        {
            other_notification_ = true;
            data_cond_.notify_all();
        }
        void Push(T new_value)
        {
            std::lock_guard<std::mutex> lk(mut_);
            data_queue_.Push(std::move(new_value))
                ; data_cond_.notify_one();
        }
        void WaitAndPop(T& value)
        {
            std::unique_lock<std::mutex> lk(mut_);
            data_cond_.wait(lk, [this] {return !data_queue_.Empty(); });
            value = std::move(data_queue_.front());
            data_queue_.pop();
        }
        std::shared_ptr<T> WaitAndPop()
        {
            std::unique_lock<std::mutex> lk(mut_);
            data_cond_.wait(lk, [this] {return !data_queue_.Empty(); });
            std::shared_ptr<T> res(
                std::make_shared<T>(std::move(data_queue_.front())));
            data_queue_.pop();
            return res;
        }
        bool TryPop(T& value)
        {
            std::lock_guard<std::mutex> lk(mut_);
            if (data_queue_.Empty())
                return false;
            value = std::move(data_queue_.front());
            data_queue_.pop();
            return true;
        }
        std::shared_ptr<T> TryPop()
        {
            std::lock_guard<std::mutex> lk(mut_);
            if (data_queue_.Empty())
                return std::shared_ptr<T>();
            std::shared_ptr<T> res(
                std::make_shared<T>(std::move(data_queue_.front())));
            data_queue_.pop();
            return res;
        }
        bool Empty() const
        {
            std::lock_guard<std::mutex> lk(mut_);
            return data_queue_.Empty();
        }
    };

    static const std::string kSpace = " \t\n\v\f\r";
    static const std::string kBlank = " \t";
    static const std::string kDigit = "0123456789";
    std::string Trim(const std::string& str, const std::string& what = rsj::kSpace);
    std::string LTrim(const std::string& str, const std::string& what = rsj::kSpace);
    std::string RTrim(const std::string& str, const std::string& what = rsj::kSpace);
    inline bool Contains(const std::string& str, const std::string& what = rsj::kSpace)
    {
        return str.find_first_of(what) != std::string::npos;
    }
}

#endif