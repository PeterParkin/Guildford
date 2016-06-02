#pragma once

#ifdef SHIPPING
# define ENGINE_API // Compiled as static library.
#else
# define ENGINE_API __declspec(dllexport) // Compiled as dll.
#endif

#include "EngineCore.h"
#include "Engine.h"

// Private to Engine.
static const int LocalBufferSize = 4096;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h> // standard fixed-sized types.
#include <stdio.h> // sprintf_s, etc.
#include <vector>
#include <atomic>
