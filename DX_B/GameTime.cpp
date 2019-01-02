#include "pch.h"
#include "GameTime.h"

unsigned __int32 clock_cycles_per_second = 0;
double seconds_per_frame = 0.0;

unsigned __int64 clock_start_time = 0L;

unsigned __int64 current_time_stamp = 0L;
unsigned __int64 delta_time = 0L;
unsigned __int64 delta_highest = 0L;
unsigned __int64 delta_lowest = 9999999999999L;

unsigned __int64 delta_frame_low = 0;
unsigned __int64 delta_frame_high = 0;

unsigned __int64 total_elapsed_time = 0L;
unsigned __int64 total_elapsed_frames = 0L;
unsigned __int64 total_estimated_frames = 0L;


void GameTime::Begin()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&clock_cycles_per_second);

	unsigned __int64 clock_cycles_per_frame = clock_cycles_per_second / 60;
	seconds_per_frame = 1.0 / clock_cycles_per_frame;

    QueryPerformanceCounter((LARGE_INTEGER*)&clock_start_time);
}

void GameTime::Tick()
{
	__int64 previous_time_stamp = current_time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&current_time_stamp);
	delta_time = current_time_stamp - previous_time_stamp;

	__int64 lastFrame = total_elapsed_frames;

	total_elapsed_time = current_time_stamp - clock_start_time;
	total_elapsed_frames = seconds_per_frame * total_elapsed_time;

	if (total_elapsed_frames > 120)
	{
		if ((total_elapsed_frames - lastFrame) == 0)
			delta_frame_low++;
		if ((total_elapsed_frames - lastFrame) > delta_frame_high)
			delta_frame_high = (total_elapsed_frames - lastFrame);
	}
}

__int64 GameTime::GetCurrentTimeCPUCycle()
{
	__int64 time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return time_stamp;
}

__int64 GameTime::GetCurrentTimeMicrosecond()
{
	__int64 time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return (__int64)(time_stamp * 1000000 * seconds_per_frame);
}

__int64 GameTime::GetCurrentTimeMilliseconds()
{
	__int64 time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return (__int64)(time_stamp * 1000 * seconds_per_frame);
}

__int64 GameTime::GetCurrentTimeSeconds()
{
	__int64 time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return (__int64)(time_stamp * seconds_per_frame);
}

__int64 GameTime::TotalElapsedSeconds()
{
	return total_elapsed_time / clock_cycles_per_second;
}

__int64 GameTime::TotalElapsedMilli()
{
	return (total_elapsed_time * 1000) / clock_cycles_per_second;
}

__int64 GameTime::TotalElapsedMicro()
{
	return (total_elapsed_time * 1000000) / clock_cycles_per_second;
}

__int64 GameTime::TotalElapsedCPUCycles()
{
	return total_elapsed_time;
}

__int64 GameTime::DeltaTime()
{
	return delta_time;
}

__int64 GameTime::DeltaHighest()
{
	return delta_highest;
}

__int64 GameTime::DeltaLowest()
{
	return delta_lowest;
}

__int64 GameTime::DeltaMajorLowFaults()
{
	return delta_frame_low;
}

__int64 GameTime::DeltaMajorHighFaults()
{
	return delta_frame_high;
}

__int64 GameTime::TotalElapsedFrames()
{
	return total_elapsed_frames;
}
