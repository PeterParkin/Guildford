#pragma once

#include <vector>
#include <mutex>
#include <atomic>

#include "../Engine/Engine.h" // ENGINE_LOG

namespace Eng
{
	// Subclass must implement the void operator(TArgs... ) function.
	template<typename... TArgs> class Event;
	template<typename... TArgs>
	class Delegate
	{
		friend class Event<TArgs...>;
	public:
		Delegate() : _Event(nullptr) {}
		~Delegate() noexcept(false)
		{
			if (_Event != nullptr)
			{
				// Do not attempt to unregister from _Event, as it may be being destroyed.
				ENGINE_LOG(Warning, "Delegate not unregistered from event on destruction");
			}
		}
	protected:
		virtual void operator()(TArgs... args) = 0;
	private:
		Event<TArgs...>* _Event; // Access only after acquiring the event's mutex.
	};
	template<typename... TArgs>
	class Event
	{
		typedef Delegate<TArgs...> TDelegate;
		friend class TDelegate;
	public:
		// Trigger all delegates.
		void operator()(TArgs... args)
		{
			// Prevents any changes to the _DelegateArray.
			// Watch out for recursion deadlocks, i.e. as a delegate, don't try to unregister
			// as response to this callback.  And try to keep your processing short!
			std::lock_guard<std::mutex> lock(_Mutex);
			for (auto d : _DelegateArray)
			{
				(*d)(args...);
			}
		}

		// Add a delegate to the list to be triggered.
		void operator+=(TDelegate& d)
		{
			// TODO: Optimize so delegates can be added to an event already being triggered.
			// via addition to a secondary array that is concatenated with the _DelegateArray
			// once all delegates have been fired.

			// Make sure the delegate is not already attached to an event.
			std::lock_guard<std::mutex> lock(_Mutex);
			ENGINE_ASSERT(d._Event == nullptr);
			_DelegateArray.push_back(&d);
			d._Event = this; // Set directly - the lifetime of Delegate and Event is guaranteed.
		}
		// Remove a delegate from the list.
		void operator-=(TDelegate& d)
		{
			std::lock_guard<std::mutex> lock(_Mutex);
			ENGINE_ASSERT(d._Event == this);
			for( auto iter = _DelegateArray.begin(); iter != _DelegateArray.end(); ++iter)
			{
				if (*iter == &d)
				{
					_DelegateArray.erase(iter);
					d._Event = nullptr; // Set directly - the lifetime of Delegate and Event is guaranteed.
					return;
				}
			}
			ENGINE_LOG(Warning, "Attempt to remove a delegate from an event it is not registered to.");
		}
		~Event()
		{
			// A delegate will assert if being destroyed while still attached to an event.
			// However, if the event is destroyed first, the lifetime of all delegates in the 
			// array is guaranteed.
			std::lock_guard<std::mutex> lock(_Mutex);
			for (auto iter = _DelegateArray.begin(); iter != _DelegateArray.end(); ++iter)
			{
				(*iter)->_Event = nullptr;
			}
		}
	private:
		std::mutex _Mutex;
		std::vector<TDelegate*> _DelegateArray;
	};
}