#include "Timer.h"
#include <Windows.h>

Timer::Timer()
{
	ticksPerSecond = startTicks = currentTicks = 0;
	endTime = -1;
}

Timer::~Timer()
{
}

void Timer::Start()
{
	endTime = -1;
	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&startTicks);
}

double Timer::ElapsedSecounds()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks);
	endTime = (currentTicks- startTicks) / ticksPerSecond;
	return endTime;
}

double Timer::ElapsedMilliseconds()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks);
	endTime = ((double)((__int64)currentTicks - (__int64)startTicks) / ((__int64)ticksPerSecond) * 1000);
	return endTime;
}

double Timer::ElapsedMicroseconds()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks);
	endTime = ((double)((__int64)currentTicks - (__int64)startTicks) / ((__int64)ticksPerSecond) * 1000000);
	return endTime;
}

double Timer::GetEndTime()
{
	return endTime;
}