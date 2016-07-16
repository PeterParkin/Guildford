#pragma once
//
// Fast, but with limited protection.  Careful now.
//
#include <atomic>

namespace Eng
{
	class SpinLock
	{
	public:
		SpinLock() : _Value(0) {}
		void lock()
		{
			int expected = 0;
			while (_Value.compare_exchange_weak(expected, 1, std::memory_order_acquire, std::memory_order_relaxed))
			{
				expected = 0;
			}
		}
		void unlock()
		{
			int prev = _Value.exchange(0, std::memory_order_release);
			if (prev != 1)
			{
				throw Exception("Unlocked when not locked", __FILE__, __FUNCTION__, __LINE__);
			}
		}
		bool try_lock()
		{
			int expected = 0;
			return _Value.compare_exchange_strong(expected, 1, std::memory_order_acquire);
		}
	private:
		std::atomic<int> _Value;
	};
}