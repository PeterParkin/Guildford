#include "EnginePCH.h"
#include "Config.h"
#include <string>

namespace Eng
{
	void Config::SetDataInternal(int num_params, ...)
	{
		ENGINE_ASSERT((num_params & 1) == 0);
		va_list vl;
		va_start(vl, num_params);
		for (int iter = 0; iter < num_params; iter+= 2)
		{
			const char* key = va_arg(vl, const char*);
			const char* value = va_arg(vl, const char*);
			unsigned key_hash = Engine::CreateHash32(key);
			ConfigKeyValuePair(key, value);
//			_MapHashToKeyValuePair.emplace(key_hash, ConfigKeyValuePair(key, value));
		}
		va_end(vl);
	}
	Config::~Config()
	{

	}
}