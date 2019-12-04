#pragma once

class GameTime
{
public:

	static    void Begin();
	static	  void Tick();

	static int64_t CurrentTimeCPUCycles();
	static  double CurrentTimeNanos();
	static  double CurrentTimeMicros();
	static  double CurrentTimeMilis();
	static  double CurrentTimeSeconds();

	static  double ConvertCyclesToNanos(int64_t cycles);
	static  double ConvertCyclesToMicros(int64_t cycles);
	static  double ConvertCyclesToMilis(int64_t cycles);
	static  double ConvertCyclesToSeconds(int64_t cycles);

	static int64_t TotalElapsedCPUCycles();
	static  double TotalElapsedNanos();
	static  double TotalElapsedMicro();
	static  double TotalElapsedMilli();
	static  double TotalElapsedSeconds();

	static int64_t DeltaTime_CPUCycle();
	static int64_t DeltaTime_Frames();
	static  double DeltaTime_Nano();
	static  double DeltaTime_Micro();
	static  double DeltaTime_Milli();
	static  double DeltaTime_Seconds();

	static int64_t AbsoluteFrameTicks();
	static int64_t TotalGameLoops();

	static    void ResetWindowTimeStamp();
	static int64_t ElapsedWindowFrameTicks();

	static    bool Sleep();
	static int64_t Sleep(int pause, int checksum);

	static    void DisplayOutputMilliseconds(int64_t timespan, unsigned loops);
	static    void DisplayOutputCPUCycles(int64_t timespan, unsigned loops);
};