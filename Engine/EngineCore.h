#pragma once
//
// Copyright (c) 2016 by Peter Parkin. ALL RIGHTS RESERVED.
// Consult your license regarding permissions and restrictions.
//
// Include early in the precompiled header for any module that links to the engine.
//

// Replacement placement new, as new is redefined when using the debug allocator.
template<typename T, typename... TArgs>
T* pnew(void* address, TArgs... args)
{
	return new (address) T(args...);
}

#ifdef _DEBUG
# define ENGINE_USE_DEBUG_ALLOCATOR 1 // Set or unset the debug allocator when running in debug.
# define ENGINE_USE_DEBUG_DIRECTX 1
#else
# define ENGINE_USE_DEBUG_ALLOCATOR 0 // Never change this.
# define ENGINE_USE_DEBUG_DIRECTX 0
#endif
#if ENGINE_USE_DEBUG_DIRECTX
#define ENGINE_SET_DEBUG_OBJECT_NAME(device_child, name, ...) \
{ \
	char buffer[256]; \
	int num_chars = sprintf_s(buffer, name, __VA_ARGS__); \
	device_child->SetPrivateData( WKPDID_D3DDebugObjectName, num_chars, buffer); \
}
#else
#define ENGINE_SET_DEBUG_OBJECT_NAME(device_child, name, ...)
#endif

#define ENGINE_MAX_PROCESSORS 16 // Setting this to 1 will change atomic operations into regular ones (e.g. when building a single-threaded server)
static_assert(ENGINE_MAX_PROCESSORS > 0, "Don't be silly.");

#if ENGINE_USE_DEBUG_ALLOCATOR
# define _CRTDBG_MAP_ALLOC
# define DEBUG_NEW new( _CLIENT_BLOCK, __FILE__, __LINE__)
# define new DEBUG_NEW
# include <stdlib.h>
# include <crtdbg.h>
#else
# include <stdlib.h>
#endif

#ifndef ENGINE_API
# ifdef ENGINE_SHIPPING 
#  define ENGINE_API // Shipping means the Engine is compiled as a static library; dll otherwise.
# else
#  define ENGINE_API __declspec(dllimport)
# endif
#endif

// TODO: Set this on a per-architecture basis.
static const int _CacheLineSize = 64;

#define MIN(x,y) (x) < (y) ? (x) : (y)
#define MAX(x,y) (x) > (y) ? (x) : (y)
#define CLAMP(x, l, h) (x) < (l) ? (l) : ((x) > (h) ? (h) : (x))
#define SAFE_RELEASE(x) if( x ) { x->Release(); x = nullptr; }
#define SAFE_DELETE(x) if( x ) { delete x; x = nullptr; }
constexpr float Pi() { return 3.141592654f;  }
constexpr float Deg2Rad(float degrees) { return degrees / 180.f * Pi(); }
constexpr float Rad2Deg(float radians) { return radians / Pi() * 180.f; }
