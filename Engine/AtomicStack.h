#pragma once

#include "../Engine/Allocators.h" // DefaultAllocator

namespace Eng
{
	template<typename T, typename TNodeAllocator = DefaultAllocator>
	class AtomicStack : public TNodeAllocator
	{
	public:
		struct Node
		{
			T _Data;
			Node* _Next;
			template<typename ...Args>
			Node(Node* next, Args... args) : _Data(args...), _Next(next) {}
		};

		AtomicStack() : _Head(nullptr)
		{
		}
		
		~AtomicStack()
		{
			for (Node* head = PopAll(); head != nullptr; head = PopAll())
			{
				while (head != nullptr)
				{
					Node* next = head->_Next;
					delete head;
					head = next;
				}
			}
		}
		// Returns true if it's the first.
		template<typename ...Args>
		bool Push(Args... args)
		{
			void* address = Alloc(sizeof(T) 
#if ENGINE_USE_DEBUG_ALLOCATOR
				, __FILE__, __LINE__
#endif
			);
			Node* node = pnew<Node>(address, _Head.load(std::memory_order_relaxed), args...);
			while (!_Head.compare_exchange_weak(node->_Next, node, std::memory_order_release, std::memory_order_relaxed));
			return node->_Next == nullptr;
		}
		bool Pop(T& out)
		{
			Node* popped_node = _Head.load(std::memory_order_relaxed);
			do
			{
				if (popped_node == nullptr)
				{
					return false;
				}
			} while (!_Head.compare_exchange_weak(popped_node, popped_node->_Next, std::memory_order_release, std::memory_order_relaxed));
			out = popped_node->_Data;
			popped_node->~Node();
			Free(popped_node);
			return true;
		}
		Node* PopAll()
		{
			return _Head.exchange(nullptr, std::memory_order_relaxed);
		}
	private:
		std::atomic<Node*> _Head;
	};
}