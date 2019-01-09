#include "pch.h"
#include "GameTime.h"

#include <iostream>
#include <windows.h>;

uint32_t clock_cycles_per_second = 0;

//Used For Calculating The Absolute Frame Index.
double seconds_per_frame = 0.0;

//Used For Calculating Elapsed Time Segments.
double seconds_per_cycle = 0.0;
double milis_per_cycle = 0.0;
double micros_per_cycle = 0.0;
double nanos_per_cycle = 0.0;

//The Application Begin Time In CPU Cycles.
int64_t clock_start_time = 0L;

//The Current CPU Cycles at start of Game Loop.
int64_t current_time_stamp = 0L;

//This is the delta time from last frame to current time.
int64_t delta_time = 0L;

//This is the elapsed time from the start time in cpu cycles.
int64_t total_elapsed_time = 0L;

//This is our desired actual frames passed over time. This allows perfect timing in event of stalls.
int64_t absolute_frame_ticks = 0L;

//This is our actual occured render loops used for proper syncing to figure out how far we fell out of sync.
int64_t gameloop_count = 0L;

uint64_t window_frametick_start = 0L;

void GameTime::Begin()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&clock_cycles_per_second);

	int64_t clock_cycles_per_frame = clock_cycles_per_second / 60;
	seconds_per_frame = 1.0 / clock_cycles_per_frame;

	seconds_per_cycle = 1.0 / clock_cycles_per_second;
	milis_per_cycle = seconds_per_cycle * 1000;
	micros_per_cycle = seconds_per_cycle * 1000000;
	nanos_per_cycle = seconds_per_cycle * 1000000000;

	QueryPerformanceCounter((LARGE_INTEGER*)&clock_start_time);
}

void GameTime::Tick()
{
	int64_t previous_time_stamp = current_time_stamp;

	QueryPerformanceCounter((LARGE_INTEGER*)&current_time_stamp);

	delta_time = current_time_stamp - previous_time_stamp;

	total_elapsed_time = current_time_stamp - clock_start_time;
	absolute_frame_ticks = seconds_per_frame * total_elapsed_time;

	++gameloop_count;
}

int64_t GameTime::CurrentTimeCPUCycles()
{
	int64_t time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return time_stamp;
}

double GameTime::CurrentTimeNanos()
{
	int64_t time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return time_stamp * nanos_per_cycle;
}

double GameTime::CurrentTimeMicros()
{
	int64_t time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return time_stamp * micros_per_cycle;
}

double GameTime::CurrentTimeMilis()
{
	int64_t time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return time_stamp * milis_per_cycle;
}

double GameTime::CurrentTimeSeconds()
{
	int64_t time_stamp;
	QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);
	return time_stamp * seconds_per_cycle;
}

double GameTime::ConvertCyclesToSeconds(int64_t cycles)
{
	return cycles * seconds_per_cycle;
}

double GameTime::ConvertCyclesToMilis(int64_t cycles)
{
	return cycles * milis_per_cycle;
}

double GameTime::ConvertCyclesToMicros(int64_t cycles)
{
	return cycles * micros_per_cycle;
}

double GameTime::ConvertCyclesToNanos(int64_t cycles)
{
	return cycles * nanos_per_cycle;
}

double GameTime::TotalElapsedSeconds()
{
	return total_elapsed_time * seconds_per_cycle;
}

double GameTime::TotalElapsedMilli()
{
	return total_elapsed_time * milis_per_cycle;
}

double GameTime::TotalElapsedMicro()
{
	return total_elapsed_time * micros_per_cycle;
}

double GameTime::TotalElapsedNanos()
{
	return total_elapsed_time * nanos_per_cycle;
}

int64_t GameTime::TotalElapsedCPUCycles()
{
	return total_elapsed_time;
}

int64_t GameTime::DeltaTime_CPUCycle()
{
	return delta_time;
}

double GameTime::DeltaTime_Nano()
{
	return delta_time * nanos_per_cycle;
}

double GameTime::DeltaTime_Micro()
{
	return delta_time * micros_per_cycle;
}

double GameTime::DeltaTime_Milli()
{
	return delta_time * milis_per_cycle;
}

double GameTime::DeltaTime_Seconds()
{
	return delta_time * seconds_per_cycle;
}

int64_t GameTime::AbsoluteFrameTicks()
{
	return absolute_frame_ticks;
}

int64_t GameTime::TotalGameLoops()
{
	return gameloop_count;
}

void GameTime::ResetWindowTimeStamp()
{
	window_frametick_start = absolute_frame_ticks;
}

int64_t GameTime::ElapsedWindowFrameTicks()
{
	return absolute_frame_ticks - window_frametick_start;
}


bool GameTime::Sleep()
{
	int64_t B = 0;
	while (B <= absolute_frame_ticks)
	{
		int64_t time_stamp;
		QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);

		int64_t A = time_stamp - clock_start_time;
		B = seconds_per_frame * A;
	}
	return true;
}

int64_t GameTime::Sleep(int pause, int checksum)
{
	int64_t pause_stamp = 0L;
	int64_t time_stamp = 0L;
	QueryPerformanceCounter((LARGE_INTEGER*)&pause_stamp);
	int64_t B = 0;
	while (((time_stamp - pause_stamp) < pause) && B <= checksum)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&time_stamp);

		int64_t A = time_stamp - clock_start_time;
		B = A - total_elapsed_time;
	}
	return B;
}

void GameTime::DisplayOutputMilliseconds(int64_t timespan, unsigned loops)
{
	float avg_cc = (float)timespan / loops;
	int millitime = (int)ConvertCyclesToMilis(timespan);
	int avg_nspl = (int)(avg_cc * 1000000000 * seconds_per_cycle);

	std::cout << "\nLoops: " << loops << ", Time Spent: " << millitime << "(Milli), Time Per Loop " << avg_nspl << "(Nano)\n";
}

void GameTime::DisplayOutputCPUCycles(int64_t timespan, unsigned loops)
{
	float avg_cc = (float)timespan / loops;
	int millitime = timespan;
	int avg_nspl = (int)(avg_cc * 1000000000 * seconds_per_cycle);

	std::cout << "\nLoops: " << loops << ", Time Spent: " << millitime << "(CPU Cycles), Time Per Loop " << avg_nspl << "(Nano)\n";

}