#pragma once

//
// Publicly accessible from the Engine module.
// WARNING: Contains variadic templates, so don't put it in a precompiled header
// unless you want your compile times to go up.
//

namespace Eng
{
	class TaskBase;
	class IoReadRequest;
	class IoWriteRequest;
	class TaskGraph
	{
		// Restrict access to Queue to tasks.
//		template<typename TFunction>						friend class TaskNoParams_Value_TaskBase;
//		template<typename TFunction, typename... TParams>	friend class Task_Value_TaskBase;
	
	public:
		ENGINE_API static void QueueForImmediateExecution(TaskBase* task); // Creating a task automatically does this.
	public:
		static void Initialize(); // Prevent TaskGraph from being initialized/finalized
		static void Finalize(); //  outside the Engine module.
		static void AssociateWithRead(void* file_handle);
		static void AssociateWithWrite(void* file_handle);
		static void BeginRead(IoReadRequest* read_request);
		static void BeginWrite(IoWriteRequest* write_request);
		ENGINE_API static void Join();
		ENGINE_API static void TaskCreated();
		ENGINE_API static void TaskCompleted();
		ENGINE_API static void Async(std::function<void()>& function);
	};
}