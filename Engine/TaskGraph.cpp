#include "EnginePCH.h"
#include "Engine.h"
#include "TaskGraph.h"
#include "AtomicStack.h"
#include "TaskBase.h"
#include "IoReadRequest.h"
#include "Dispatcher.h"

#include <thread>

namespace Eng
{
	enum class Key : ULONG_PTR
	{
		Exit, Task, Async, BeginRead, EndRead, BeginWrite, EndWrite
	};
	alignas(_CacheLineSize) static AtomicStack<TaskBase*, TaskAllocator> _TaskQueue;
	static uint8_t _HeadCachePad[_CacheLineSize - sizeof(_TaskQueue)];

	static int _NumThreads = 0;
	static HANDLE _IoCompletionPort = INVALID_HANDLE_VALUE;
	static std::vector<std::thread> _ThreadArray;
	static int _NumTasks = 0;
	static std::atomic<int> _NumTasksComplete = 0;

	inline static int GetNumWorkerThreads()
	{
		int concurrency = (int)std::thread::hardware_concurrency();
		return CLAMP(concurrency, 1, ENGINE_MAX_PROCESSORS - 1);
	}
	inline static bool DoWork(DWORD duration = INFINITE)
	{
		DWORD number_of_bytes = 0;
		ULONG_PTR key;
		OVERLAPPED* ovl;
		BOOL result = GetQueuedCompletionStatus(_IoCompletionPort, &number_of_bytes, &key, &ovl, duration);
		if (result == FALSE)
		{
			return true; // Keep going, we haven't received an exit request yet.
		}
		const Key& _key = *reinterpret_cast<const Key*>(&key);

		switch (_key)
		{
			case Key::Exit:
				return false;
			case Key::Task:
				// try/catch inside Execute.
				reinterpret_cast<TaskBase*>(ovl)->Execute();
				break;
			case Key::Async:
			{
				std::function<void()>* function = reinterpret_cast<std::function<void()>*>(ovl);
				try
				{
					(*function)();
				}
				catch (...)
				{
					Engine::_ExceptionContainer += std::current_exception();
				}
				delete function;
			}
			break;
			case Key::BeginRead:
			{
				auto read_request = reinterpret_cast<IoReadRequest*>(ovl);
				try
				{
					read_request->Begin();
				}
				catch (...)
				{
					Engine::_ExceptionContainer += std::current_exception();
					read_request->End(false); // Deletes the read request.
				}
			}
			break;
			case Key::EndRead:
			{
				auto read_request = static_cast<IoReadRequest*>(ovl);
				try
				{
					read_request->End(number_of_bytes != 0);
				}
				catch (...)
				{
					Engine::_ExceptionContainer += std::current_exception();
					read_request->PostEnd();
					delete read_request;
				}
			}
			break;
		}
		return true;
	}
	static void ThreadMain(int index)
	{
		{
			char buffer[] = "TaskGraph00";
			sprintf_s(buffer, "TaskGraph%02d", index);
			Engine::SetThreadName(buffer, EThreadType::TaskGraphThread);
		}
		while (DoWork());
	}

	void TaskGraph::Initialize()
	{
		_NumThreads = GetNumWorkerThreads();
		_IoCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, _NumThreads);
		_ThreadArray.reserve(_NumThreads);
		for (int iter = 0; iter < _NumThreads; ++iter)
		{
			_ThreadArray.emplace_back(ThreadMain, iter);
		}
	}
	void TaskGraph::Finalize()
	{
		for (int iter = 0; iter < _NumThreads; ++iter)
		{
			// Send the shutdown requests.
			PostQueuedCompletionStatus(_IoCompletionPort, 0, static_cast<ULONG_PTR>(Key::Exit), nullptr);
		}
		for (auto& t : _ThreadArray)
		{
			t.join();
		}
		_ThreadArray.resize(0);
		_ThreadArray.shrink_to_fit();
		CloseHandle(_IoCompletionPort);
		_IoCompletionPort = INVALID_HANDLE_VALUE;
	}

	void TaskGraph::AssociateWithRead(void* file_handle)
	{
		HANDLE result = CreateIoCompletionPort(file_handle, _IoCompletionPort, static_cast<ULONG_PTR>(Key::EndRead), 0);
		if (result == NULL)
		{
			ENGINE_LOG(Error, "Unable to associate file handle with IoCompletionPort");
		}
	}

	void TaskGraph::QueueForImmediateExecution(TaskBase* task)
	{
		PostQueuedCompletionStatus(_IoCompletionPort, 0, static_cast<ULONG_PTR>(Key::Task), reinterpret_cast<LPOVERLAPPED>(task));
	}

	void TaskGraph::Join()
	{
		// Appear as if running from the TaskGraph.
		extern __declspec(thread) EThreadType _ThreadType;
		if (_ThreadType != EThreadType::MainThread)
		{
			ENGINE_LOG(Error, "TaskGraph joined outside of the MainThread");
			return;
		}
		_ThreadType = EThreadType::TaskGraphThread;
		while (_NumTasksComplete.load(std::memory_order_acquire) != _NumTasks)
		{
			// TODO: Fix energy wasted by spinning.
			DoWork(0);
		}
		// Now appear as the MainThread once again.
		_ThreadType = EThreadType::MainThread;

		_NumTasks = 0;
		_NumTasksComplete.store(0, std::memory_order_relaxed);
		TaskAllocator::FreeAll();
		Dispatcher::ExecuteAll();
	}

	void TaskGraph::TaskCreated()
	{
		if (Engine::GetThreadType() != EThreadType::MainThread)
		{
			ENGINE_LOG(Error, "You may only create tasks on the MainThread.");
			return;
		}
		++_NumTasks; // No need to be atomic - only the main thread can create tasks.
	}
	void TaskGraph::TaskCompleted()
	{
		_NumTasksComplete.fetch_add(1, std::memory_order_release);
	}

	void TaskGraph::Async(std::function<void()>& function)
	{
		auto new_function = new std::function<void()>(std::move(function));
		PostQueuedCompletionStatus(_IoCompletionPort, 0, static_cast<ULONG_PTR>(Key::Async), reinterpret_cast<LPOVERLAPPED>(new_function));
	}

	void TaskGraph::BeginRead(IoReadRequest* read_request)
	{
		if( FALSE == PostQueuedCompletionStatus(_IoCompletionPort, 0, static_cast<ULONG_PTR>(Key::BeginRead), reinterpret_cast<LPOVERLAPPED>(read_request)))
		{
			ENGINE_LOG(Error, "Unable to BeginRead");
		}
	}
}