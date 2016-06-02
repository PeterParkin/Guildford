#include "EnginePCH.h"
#include "../Engine/VirtualKey.h"

namespace Eng
{
	static const char* EVirtualKey_String[] =
	{
#pragma push_macro("V")
#define V(key) #key
#include "../Engine/VirtualKey.inl"
#pragma pop_macro("V")
	};

	static const char* GetString(EVirtualKey key)
	{
		return EVirtualKey_String[static_cast<uint8_t>(key)];
	}
}