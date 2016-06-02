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
			T Data;
			Node* Next;
			template<typename ...Args>
			Node(Node* next, Args... args) : Data(args...), Next(next) {}
		};

		AtomicStack() : Head(nullptr)
		{
		}
		
		~AtomicStack()
		{
			// TODO: Destruction.
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
			Node* node = pnew<Node>(address, Head.load(std::memory_order_relaxed), args...);
			while (!Head.compare_exchange_weak(node->Next, node, std::memory_order_release, std::memory_order_relaxed));
			return node->Next == nullptr;
		}
		bool Pop(T& out)
		{
			Node* popped_node = Head.load(std::memory_order_relaxed);
			do
			{
				if (popped_node == nullptr)
				{
					return false;
				}
			} while (!Head.compare_exchange_weak(popped_node, popped_node->Next, std::memory_order_release, std::memory_order_relaxed));
			out = popped_node->Data;
			popped_node->~Node();
			Free(popped_node);
			return true;
		}
		Node* PopAll()
		{
			return Head.exchange(nullptr, std::memory_order_relaxed);
		}
	private:
		std::atomic<Node*> Head;
	};
}