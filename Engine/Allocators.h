#pragma once

#include "../Engine/EngineCore.h" // ENGINE_USE_DEBUG_ALLOCATOR

namespace Eng
{
	// malloc/free
	struct DefaultAllocator
	{
#if ENGINE_USE_DEBUG_ALLOCATOR
		static void* Alloc(size_t size, const char* file, int line) { return _malloc_dbg(size, _NORMAL_BLOCK, file, line); }
		static void* Realloc(void* address, size_t size, const char* file, int line) { return _realloc_dbg(address, size, _NORMAL_BLOCK, file, line); }
		static void Free(void* address) { _free_dbg(address, _NORMAL_BLOCK); }
#else
		static void* Alloc(size_t size) { return malloc(size); }
		static void* Realloc(void* address, size_t size) { return realloc(address, size); }
		static void Free(void* address) { free(address); }
#endif
	};
}