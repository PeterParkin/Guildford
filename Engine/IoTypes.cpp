#include "EnginePCH.h"
#include "IoTypes.h"

namespace Eng
{
	IoReadData::IoReadData(unsigned size)
		: _Size(size)
	{
		_Address = size != 0 ? VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) : nullptr;
	}
	void IoReadData::Clear()
	{
		if (_Address != nullptr)
		{
			VirtualFree(_Address, 0, MEM_RELEASE);
			_Address = nullptr;
			_Size = 0;
		}
	}
	IoReadData::~IoReadData()
	{
		Clear();
	}
}