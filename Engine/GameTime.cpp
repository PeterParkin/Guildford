#include "EnginePCH.h"
#include "GameTime.h"

namespace Eng
{
	static double Frequency = 0.0;
	GameTime::GameTime()
		: _PreviousHFTime(0)
		, _AppStartTime(0)
		, _FrameIndex(0)
		, _DeltaHFTimeSecs(0.0)
		, _LFTimeSecs(0.f)
	{
		LARGE_INTEGER li;
		if (Frequency == 0.0)
		{
			QueryPerformanceFrequency(&li);
			Frequency = (double)li.QuadPart;
		}
		QueryPerformanceCounter(&li);
		_PreviousHFTime = li.QuadPart;
		_AppStartTime = GetTickCount();
	}

	void GameTime::Update()
	{
		LARGE_INTEGER current_time;
		__int64 delta;
		++_FrameIndex;
		QueryPerformanceCounter(&current_time);
		delta = current_time.QuadPart - _PreviousHFTime;
		_PreviousHFTime = current_time.QuadPart;
		_DeltaHFTimeSecs = (float)(delta / Frequency);
		uint32_t ticks_since_start = GetTickCount() - _AppStartTime;
		static const uint32_t _24hours = 1000 * 60 * 60 * 24;
		if (ticks_since_start > _24hours )
		{
			_AppStartTime += _24hours;
			ticks_since_start -= _24hours;
		}
		_LFTimeSecs = ticks_since_start / 1000.f;
	}
}