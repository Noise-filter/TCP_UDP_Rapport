#include "Timer.h"
#include <Windows.h>

Timer::Timer()
{
	framesPerSecond = 0;
	maxFramesPerSecond = 0;
	FrameCount = 0;

	delta = 0.0f;
	fixedDelta = 0.0f;
	useFixedStep = false;

	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
	QueryPerformanceFrequency((LARGE_INTEGER*)&currentTicks);
	startupTicks = currentTicks;
	oneSecTicks = currentTicks;
}

Timer::~Timer()
{
}

void Timer::update()
{
	lastTicks = currentTicks;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks);

	if(useFixedStep)
		delta = fixedDelta;
	else
		delta = (float)((__int64)currentTicks - (__int64)lastTicks) / (__int64)ticksPerSecond;

	if((float) ((__int64)currentTicks - (__int64)oneSecTicks) / (__int64)ticksPerSecond < 1.0f)
		FrameCount++;
	else
	{
		framesPerSecond = FrameCount;

		if(framesPerSecond > maxFramesPerSecond)
			maxFramesPerSecond = framesPerSecond;

		FrameCount = 0;
		oneSecTicks = currentTicks;
	}
}

void Timer::reset()
{
	framesPerSecond = 0;
	FrameCount = 0;
	delta = 0.0f;
}

float Timer::runtime()
{
	return ((float)((__int64)currentTicks - (__int64)startupTicks) / (__int64)ticksPerSecond);
}

double Timer::GetSeconds()
{
	return ((double)((__int64)currentTicks - (__int64)startupTicks) / (__int64)ticksPerSecond);
}

double Timer::GetMilliSeconds()
{
	return ((double)((__int64)currentTicks - (__int64)startupTicks) / ((__int64)ticksPerSecond) * 1000);
}

double Timer::GetMicroSeconds()
{
	return ((double)((__int64)currentTicks - (__int64)startupTicks) / ((__int64)ticksPerSecond) * 1000000);
}

float Timer::elapsed()
{
	return delta;
}

int Timer::framerate()
{
	return framesPerSecond;
}

int Timer::maxFramerate()
{
	return maxFramesPerSecond;
}

int Timer::frameCount()
{
	return FrameCount;
}

void Timer::setFixedTimeStep(float step)
{
	if(step <= 0.0f)
	{
		useFixedStep = false;
		fixedDelta = 0.0f;
	}
	else
	{
		useFixedStep = true;
		fixedDelta = step;
	}
}