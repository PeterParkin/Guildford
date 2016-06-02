#pragma once

// Copyright (c) 2016, Peter Parkin
// Publicly accessible from the Engine module.
// WARNING: Contains meaty variadic templates.  Don't put it in a precompiled header!

#include <atomic>
#include <vector>
#include <type_traits>
#include <tuple>
#include <stdint.h>
#include "../Engine/TaskGraph.h"
#include "../Engine/TaskBase.h"

namespace Eng
{
	template<typename TValue>
	class Value_TaskBase : public TaskBase
	{
	public:
		TValue _Value;
		void ExecuteVirtual() override {} 
		Value_TaskBase(const int& num_data_sources)
			: TaskBase(num_data_sources)
		{
		}
	};

	template<typename TFunction>
	class TaskNoParams_Value_TaskBase
		: public Value_TaskBase<typename std::result_of<TFunction()>::type>
	{
		typedef typename std::result_of<TFunction()>::type TReturn;
	public:
		TFunction _Function;
		TaskNoParams_Value_TaskBase(const TFunction& function)
			: Value_TaskBase<TReturn>(0)
			, _Function(function)
		{
			TaskGraph::QueueForImmediateExecution(this);
		}
		void ExecuteVirtual() override
		{
			_Value = _Function();
		}
	};
	template<typename TValue_TaskBasePtrRef, typename... TValue_TaskBasePtrRefs>
	struct SetTargetList
	{
		static void Do(int& num_sources_available, TaskBase* target, TValue_TaskBasePtrRef value_taskbase, TValue_TaskBasePtrRefs... others)
		{
			SetTargetList<TValue_TaskBasePtrRefs...>::Do(num_sources_available, target, others...);
			value_taskbase->AddTarget(num_sources_available, target);
		}
	};
	template<typename TValue_TaskBasePtrRef>
	struct SetTargetList<TValue_TaskBasePtrRef>
	{
		static void Do(int& num_sources_available, TaskBase* target, TValue_TaskBasePtrRef value_taskbase)
		{
			value_taskbase->AddTarget(num_sources_available, target);
		}
	};
	template<typename TFunction, typename... TParams>
	class Task_Value_TaskBase
		: public Value_TaskBase<typename std::result_of<TFunction(TParams...)>::type>
		, public std::tuple<const TParams&...>
	{
	public:
		typedef typename std::result_of<TFunction(TParams...)>::type TReturnType;
		TFunction _Function;
		Task_Value_TaskBase(const TFunction& func, Value_TaskBase<TParams>*& ... args )
			: Value_TaskBase<TReturnType>(sizeof...(TParams))
			, std::tuple<const TParams&...>(args->_Value ...)
			, _Function(func)
		{
			int num_sources_available = 0;
			SetTargetList<Value_TaskBase<TParams>*& ...>::Do(num_sources_available, static_cast<TaskBase*>(this), args...);
			if( _DataSourcesRemaining.fetch_sub(num_sources_available, std::memory_order_acquire) == num_sources_available)
			{
				TaskGraph::QueueForImmediateExecution(this);
			}
		}
		void ExecuteVirtual() override
		{
			ExecuteVirtual(std::index_sequence_for<TParams...> {});
		}
		template<std::size_t... I>
		void ExecuteVirtual(std::index_sequence<I...>)
		{
			_Value = _Function(std::get<I>(*this) ...);
		}
	};
	template<typename... TParams>
	struct Task
	{
		template<typename TFunction>
		static Value_TaskBase<typename std::result_of<TFunction(TParams...)>::type>*
			Create(const TFunction& func, Value_TaskBase<TParams>*& ... args)
		{
			TaskGraph::TaskCreated();
			void* address = TaskAllocator::Alloc(sizeof(Task_Value_TaskBase<TFunction, TParams...>));
			return pnew<Task_Value_TaskBase<TFunction, TParams...>>(address, func, args...);
		}
	};
	template<>
	struct Task<>
	{
		template<typename TFunction, typename... TArgs>
		static Value_TaskBase<typename std::result_of<TFunction()>::type>* 
			Create(const TFunction& func)
		{
			TaskGraph::TaskCreated();
			void* address = TaskAllocator::Alloc((unsigned)sizeof(TaskNoParams_Value_TaskBase<TFunction>));
			return pnew<TaskNoParams_Value_TaskBase<TFunction>>(address, func);
		}
	};
	struct Test
	{
		Test()
		{
			auto t1 = Task<>::Create([]()
			{
				return 1;
			});
			auto t2 = Task<>::Create([]()
			{
				return 2.f;
			});
			auto t3 = Task<int, float>::Create([](const int& i, const float& f)
			{
				return (double)(i + f);
			}, t1, t2);
		}
	};
} // namespace Eng
