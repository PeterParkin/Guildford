#pragma once

// Copyright (c) 2016, Peter Parkin
// Publicly accessible from the Engine module.
// WARNING: Contains meaty variadic templates.  Don't put it in a precompiled header!

#include <atomic>
#include <vector>
#include <type_traits>
#include <tuple>
#include <stdint.h>

namespace Essay
{
	// Base class is non-template type with pure virtual Execute function.
	// Provides basic functionality of a taskgraph.
	class TaskBase
	{
	public:
		virtual void ExecuteVirtual() = 0;
		std::atomic<int> _DataSourcesRemaining;
		bool _CanBeExecuted;
		std::vector<TaskBase*> _TargetList;
		void Trigger()
		{
			if (1 == _DataSourcesRemaining.fetch_sub(1, std::memory_order_relaxed))
			{
				_CanBeExecuted = true;
			}
		}
		TaskBase(const int& num_data_sources)
			: _DataSourcesRemaining(num_data_sources)
			, _CanBeExecuted(num_data_sources == 0) 
		{}
		void Execute()
		{
			ExecuteVirtual();
			for (auto& task_base : _TargetList)
			{
				task_base->Trigger();
			}
		}
	};
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
		}
		void ExecuteVirtual() override
		{
			_Value = _Function();
		}
	};
	template<typename TValue_TaskBasePtrRef, typename... TValue_TaskBasePtrRefs>
	struct SetTargetList
	{
		static void Do(TaskBase* target, TValue_TaskBasePtrRef value_taskbase, TValue_TaskBasePtrRefs... others)
		{
			SetTargetList<TValue_TaskBasePtrRefs...>::Do(target, others...);
			value_taskbase->_TargetList.emplace_back(target);
		}
	};
	template<typename TValue_TaskBasePtrRef>
	struct SetTargetList<TValue_TaskBasePtrRef>
	{
		static void Do(TaskBase* target, TValue_TaskBasePtrRef value_taskbase)
		{
			value_taskbase->_TargetList.emplace_back(target);
		}
	};
	template<typename TFunction, typename... TParams>
	class Task_Value_TaskBase
		: public Value_TaskBase<typename std::result_of<TFunction(TParams...)>::type>
		, public std::tuple<TParams&...>
	{
	public:
		typedef typename std::result_of<TFunction(TParams...)>::type TReturnType;
		TFunction _Function;
		Task_Value_TaskBase(const TFunction& func, Value_TaskBase<TParams>*& ... args )
			: Value_TaskBase<TReturnType>(sizeof...(TParams))
			, std::tuple<TParams&...>(args->_Value ...)
			, _Function(func)
		{
			SetTargetList<Value_TaskBase<TParams>*& ...>::Do(static_cast<TaskBase*>(this), args...);
		}
		void ExecuteVirtual() override
		{
			return ExecuteVirtual(std::index_sequence_for<TParams...> {});
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
			return new Task_Value_TaskBase<TFunction, TParams...>(func, args...);
		}
	};
	template<>
	struct Task<>
	{
		template<typename TFunction, typename... TArgs>
		static Value_TaskBase<typename std::result_of<TFunction()>::type>* 
			Create(const TFunction& func)
		{
			return new TaskNoParams_Value_TaskBase<TFunction>(func);
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
} // namespace Essay
