#pragma once

class GameTime
{
	public:
		static void Begin();
		static void Tick();

		static __int64 GetCurrentTimeCPUCycle();
		static __int64 GetCurrentTimeMicrosecond();
		static __int64 GetCurrentTimeMilliseconds();
		static __int64 GetCurrentTimeSeconds();

		static __int64 TotalElapsedSeconds();
		static __int64 TotalElapsedMilli();
		static __int64 TotalElapsedMicro();
		static __int64 TotalElapsedCPUCycles();

		static __int64 DeltaTime();
		static __int64 DeltaHighest();
		static __int64 DeltaLowest();
		static __int64 DeltaMinorLowFaults();
		static __int64 DeltaMinorHighFaults();
		static __int64 DeltaMajorLowFaults();
		static __int64 DeltaMajorHighFaults();
		static __int64 TotalElapsedFrames();
};