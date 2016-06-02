#pragma once

#include "../Engine/EngineCore.h"
#include "../Engine/TaskGraph.h"

#include <atomic>

namespace Eng
{
	struct TaskAllocator
	{
		static void Initialize();
		static void Finalize();
		ENGINE_API static void* Alloc(unsigned size);
		static void Free(void* address) {}
		ENGINE_API static void FreeAll();
	};

	// Base class is non-template type with pure virtual Execute function.
	// Provides basic functionality of a taskgraph.
	class TaskBase
	{
	private:
		enum
		{
			WaitingOrProcessing = 0,
			WaitingOrProcessingLocked = 1,
			Complete = 2,
			CompleteLocked = 3,
			Force32 = 0xFFFFFFFF
		};
	public:
		virtual void ExecuteVirtual() = 0;
		std::atomic<uint32_t> _DataSourcesRemaining; // Min size for atomic increment.
		std::atomic<uint32_t> _State;
		TaskBase** _TargetList;
		int32_t _TargetNum;

		inline void AddTarget(TaskBase* target)
		{
			TaskBase** old_target_list = _TargetList;
			int32_t old_target_num = _TargetNum++;
			_TargetList = (TaskBase**)TaskAllocator::Alloc(_TargetNum * sizeof(TaskBase*));
			memcpy(_TargetList, old_target_list, old_target_num * sizeof(TaskBase*));
			// No need to free the old_target_list, as we're using a linear allocator.
			_TargetList[old_target_num] = target;
		}
		void AddTarget(int& num_sources_available, TaskBase* target)
		{
			// Lock the list.
			uint32_t state_prev = _State.fetch_add(1, std::memory_order_acquire);
			if (state_prev == WaitingOrProcessing)
			{
				AddTarget(target); // From this point on, we may be triggered!
			}
			else
			{
				++num_sources_available;
			}
			_State.fetch_sub(1, std::memory_order_release);
		}
		void Trigger()
		{
			if (1 == _DataSourcesRemaining.fetch_sub(1, std::memory_order_acquire))
			{
				TaskGraph::QueueForImmediateExecution(this);
			}
		}
		void TriggerImmediate()
		{
			if (1 == _DataSourcesRemaining.fetch_sub(1, std::memory_order_acquire))
			{
				Execute();
			}
		}
		TaskBase(const int& num_data_sources)
			: _DataSourcesRemaining(num_data_sources)
			, _TargetList(nullptr)
			, _TargetNum(0)
		{}
		void Execute()
		{
			try
			{
				ExecuteVirtual();
			}
			catch (...)
			{
				Engine::_ExceptionContainer += std::current_exception();
			}
			// Data is now available to other tasks.
			uint32_t expected;
			uint32_t requested = Complete;

			// It is possible for the main thread to be adding a dependency task,
			// at which point it will increment the _State.
			// Wait for it to release the atomic lock.
			do
			{
				expected = WaitingOrProcessing;
			} while (!_State.compare_exchange_weak(expected, requested, std::memory_order_acquire));
			// The _TargetList cannot be interfered with from this point.
			TaskGraph::TaskCompleted();
			uint16_t index = 0;
			for (; index < _TargetNum - 1; ++index)
			{
				_TargetList[index]->Trigger();
			}
			if (_TargetNum > 0)
			{
				_TargetList[index]->TriggerImmediate();
			}
		}
	};
}