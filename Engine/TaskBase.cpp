#include "EnginePCH.h"
#include "TaskBase.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // VirtualAlloc
#include "Exception.h"

namespace Eng
{
	// 4MB virtual memory pool allocated in page-sized chunks.
	static const int _MaxVirtualAddressSpace = 4 * 1024 * 1024;
	__declspec(thread) bool _IsInThread = false;

	static unsigned _PageSize = 0;
	static uint8_t* _TBAddress = nullptr;
	static unsigned _TBReserved = 0;
	static unsigned _TBCommitted = 0;

	alignas(_CacheLineSize)
	struct CacheLineData
	{
		std::atomic<unsigned> _TBUsed = 0;
		std::atomic<unsigned> _Lock = 0;
		uint8_t _Pad[_CacheLineSize - sizeof(std::atomic<unsigned>) * 2];
	} static _CacheLineData;
	static_assert(sizeof(CacheLineData) == _CacheLineSize, "False sharing on the cacheline?");
	void TaskAllocator::Initialize()
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		_PageSize = si.dwPageSize;
		_IsInThread = true;
		_TBAddress = (uint8_t*)VirtualAlloc(nullptr, _MaxVirtualAddressSpace, MEM_RESERVE, PAGE_READWRITE);
		if (_TBAddress == nullptr)
		{
			ENGINE_LOG(Error, "Unable to allocate %d bytes of virtual address space", _MaxVirtualAddressSpace);
			return;
		}
		_TBReserved = _MaxVirtualAddressSpace;
	}
	void TaskAllocator::Finalize()
	{
		if (!_IsInThread)
		{
			ENGINE_LOG(Warning, "TaskAllocator::Finalize() called from a different thread than TaskAllocator::Initialize()");
		}
		VirtualFree(_TBAddress, 0, MEM_RELEASE);
		_TBAddress = nullptr;
		_TBReserved = 0;
		_TBCommitted = 0;
		_CacheLineData._TBUsed.store(0, std::memory_order_release);
	}
	void* TaskAllocator::Alloc(unsigned size)
	{
		unsigned old_used = _CacheLineData._TBUsed.fetch_add(size, std::memory_order_acquire);
		unsigned new_used = old_used + size;
		void* result = _TBAddress + old_used;
		if (new_used > _MaxVirtualAddressSpace)
		{
			ENGINE_LOG(Error, "TaskAllocator ran out of virtual address space");
			return nullptr;
		}
		else if (new_used > _TBCommitted)
		{
			unsigned previous_lock = _CacheLineData._Lock.fetch_add(1, std::memory_order_acquire);
			if ( previous_lock == 0)
			{
				// This thread is responsible for allocating the new page.
				if (!VirtualAlloc(_TBAddress + _TBCommitted, _PageSize, MEM_COMMIT, PAGE_READWRITE))
				{
					ENGINE_LOG(Error, "Unable to commit more physical memory");
					return nullptr;
				}
				_TBCommitted += _PageSize;
				_CacheLineData._Lock.fetch_sub(1, std::memory_order_release);
			}
			else
			{
				while (_CacheLineData._Lock.load(std::memory_order_relaxed) > previous_lock); // Spin until the lock is released.
				_CacheLineData._Lock.fetch_sub(1, std::memory_order_release);
			}
			return result;
		}
		return result;
	}

	void TaskAllocator::FreeAll()
	{
		if (!_IsInThread)
		{
			ENGINE_LOG(Warning, "TaskAllocator::FreeAll() called from a different thread than TaskAllocator::Initialize()");
		}
		_CacheLineData._TBUsed.store(0, std::memory_order_relaxed);
	}
}