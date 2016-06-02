#pragma once

namespace Eng
{
	// All GameTime wraps around after 24 hours.
	class GameTime
	{
	private:
		int64_t _PreviousHFTime;
		uint32_t _AppStartTime;
		uint32_t _FrameIndex;
		float _DeltaHFTimeSecs;
		float _LFTimeSecs;

	public:
		ENGINE_API GameTime();
		ENGINE_API void Update();

		float GetTime() const
		{
			return _LFTimeSecs;
		}
		float GetDeltaTime() const
		{
			return _DeltaHFTimeSecs;
		}
		uint32_t GetFrameIndex() const
		{
			return _FrameIndex;
		}
	};
}