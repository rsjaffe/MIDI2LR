#pragma once
#pragma warning(push)
#pragma warning( disable : 4324 )
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
#include <vector>
#include <mutex>
#include <cstddef>
#include <stack>

#ifdef NDEBUG    // asserts disabled
static constexpr bool ndebug = true;
#else            // asserts enabled
static constexpr bool ndebug = false;
#endif

namespace RSJ {
	constexpr std::size_t kCacheLineSize = 64;
	class UniqueInstanceNumber {
		std::atomic<size_t> number;

	public:
		std::size_t takeNumber() {
			return number++;
		}

		void returnNumber(std::size_t) noexcept {
			//do nothing;
		}

		void returnNumberNoLock(std::size_t) noexcept {
			//do nothing;
		}

	};

	template<class Mutex = std::mutex>
	class UniqueInstanceNumberRecycling {
		Mutex mtx;
		std::stack<std::size_t, std::vector<std::size_t>> freedNumbers{};
		std::size_t next{ 0 };

	public:
		std::size_t takeNumber() {
			std::lock_guard<decltype(mtx)> lock(mtx);
			if (freedNumbers.empty()) {
				return next++;
			}
			else {
				auto myNumber = freedNumbers.top();
				freedNumbers.pop();
				return myNumber;
			}
		}
		//used during destructor, so should guarantee no exceptions
		void returnNumber(std::size_t number) noexcept {
			try {
				std::lock_guard<decltype(mtx)> lock(mtx);
				freedNumbers.push(number);
			}
			catch (...) {
				//noexcept guarantee
			}
		}

		//use when destructor holds a lock
		void returnNumberNoLock(std::size_t number) noexcept {
			try {
				freedNumbers.push(number);
			}
			catch (...) {
				//noexcept guarantee
			}
		}
	};

	inline static void CpuRelax() noexcept
	{
#ifdef _WIN32
		_mm_pause();
#else
		asm("pause");//check to see whether OSX also uses _mm_pause
#endif
	}

	class spinlock {
		std::atomic_flag flag{ ATOMIC_FLAG_INIT };
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


	class RelaxTTasSpinLock {
	public:
		void lock() noexcept
		{
			do {
				while (flag.load(std::memory_order_relaxed))
					CpuRelax();//spin without expensive exchange
			} while (flag.exchange(true, std::memory_order_acquire));
		}

		bool try_lock() noexcept
		{
			return !flag.exchange(true, std::memory_order_acquire);
		}

		void unlock() noexcept
		{
			flag.store(false, std::memory_order_release);
		}

	private:
		std::atomic_bool flag = { false };
	};

	template<class InstanceNumberProvider = UniqueInstanceNumber>
	class PropBoTicketSpinLock {
	public:
		PropBoTicketSpinLock() : instanceNumber_{ instanceNumberProvider_.takeNumber() } {};
		~PropBoTicketSpinLock() {
			if (savedTicketIsValid[instanceNumber_]) {//must use a saved ticket to avoid deadlock
				const auto myTicketNo = savedTicketNumber[instanceNumber_];
				savedTicketIsValid[instanceNumber_] = false; //in case number is recycled
				while (true) {
					const auto servingTicketNo = ServingTicketNo.load(std::memory_order_acquire);
					if (servingTicketNo == myTicketNo)
						break;
					const std::size_t waitIters = kBackoffBase * (myTicketNo - servingTicketNo);
					for (std::size_t i = 0; i < waitIters; i++)
						CpuRelax();
				}//after acquisition, update ServingTicketNo to simulate unlock
				instanceNumberProvider_.returnNumberNoLock(instanceNumber_);
				const auto newNo = ServingTicketNo.load(std::memory_order_relaxed) + 1;
				ServingTicketNo.store(newNo, std::memory_order_release);
			}
			else {
				instanceNumberProvider_.returnNumber(instanceNumber_);
			}
		}

		void lock() noexcept
		{
			std::size_t myTicketNo = 0;
			if (!savedTicketIsValid[instanceNumber_]) {
				myTicketNo = NextTicketNo.fetch_add(1, std::memory_order_relaxed);
			} //don't need to save ticket number as will not exit without using it
			else {
				myTicketNo = savedTicketNumber[instanceNumber_];
				savedTicketIsValid[instanceNumber_] = false;
			}
			while (true) {
				const auto servingTicketNo = ServingTicketNo.load(std::memory_order_acquire);
				if (servingTicketNo == myTicketNo)
					break;
				const std::size_t waitIters = kBackoffBase*(myTicketNo - servingTicketNo);
				for (std::size_t i = 0; i < waitIters; i++)
					CpuRelax();
			}
		}

		bool try_lock() noexcept
		{
			std::size_t myTicketNo = 0;
			if (!savedTicketIsValid[instanceNumber_]) {
				myTicketNo = NextTicketNo.fetch_add(1, std::memory_order_relaxed);
			}
			else {
				myTicketNo = savedTicketNumber[instanceNumber_];
			}
			if (ServingTicketNo.load(std::memory_order_acquire) == myTicketNo) {
				return true;
			}
			else { //no lock, save ticket
				savedTicketIsValid[instanceNumber_] = true;
				savedTicketNumber[instanceNumber_] = myTicketNo;
				return false;
			}
		}

		void unlock() noexcept
		{
			savedTicketIsValid[instanceNumber_] = false;
			const auto newNo = ServingTicketNo.load(std::memory_order_relaxed) + 1;
			ServingTicketNo.store(newNo, std::memory_order_release);
		}

	private:
		static constexpr std::size_t kBackoffBase = 10;
		alignas(kCacheLineSize)std::atomic_size_t ServingTicketNo = { 0 };
		alignas(kCacheLineSize)std::atomic_size_t NextTicketNo = { 0 };
		static InstanceNumberProvider instanceNumberProvider_;
		static thread_local std::vector<std::size_t> savedTicketNumber;
		static thread_local std::vector<bool> savedTicketIsValid;
		const std::size_t instanceNumber_{ 0 };
	};
}
#pragma warning(pop)
#endif  // MISC_H_INCLUDED
