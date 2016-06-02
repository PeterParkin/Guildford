#pragma once

namespace Eng
{
	struct IoReadData
	{
		void* _Address;
		unsigned _Size;
		void Clear();
		ENGINE_API IoReadData(unsigned size);
		ENGINE_API ~IoReadData();
	};
}