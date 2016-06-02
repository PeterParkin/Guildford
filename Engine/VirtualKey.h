#pragma once

#include <stdint.h>

namespace Eng
{
	enum class EVirtualKey : uint8_t
	{
#pragma push_macro("V")
#define V(key) key
#include "../Engine/VirtualKey.inl"
	};
	static const char* GetString(EVirtualKey key);
#pragma pop_macro("V")
}
