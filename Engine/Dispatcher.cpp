#include "EnginePCH.h"
#include "Dispatcher.h"

#include "AtomicStack.h"

namespace Eng
{
	alignas(_CacheLineSize)
	static AtomicStack<std::function<void()>> _Dispatcher;
	static uint8_t _DispatcherCachePad[_CacheLineSize - sizeof(_Dispatcher)];

	void Dispatcher::Dispatch(std::function<void()>& function)
	{
		_Dispatcher.Push(std::move(function));
	}
	void Execute(AtomicStack<std::function<void()>>::Node* node)
	{
		if (node)
		{
			Execute(node->_Next);
			node->_Data();
			delete node;
		}
	}
	void Dispatcher::ExecuteAll()
	{
		Execute(_Dispatcher.PopAll());
	}
	void Delete(AtomicStack<std::function<void()>>::Node* node)
	{
		if (node)
		{
			Delete(node->_Next);
			delete node;
		}
	}
	void Dispatcher::DeleteAll()
	{
		Delete(_Dispatcher.PopAll());
	}
}