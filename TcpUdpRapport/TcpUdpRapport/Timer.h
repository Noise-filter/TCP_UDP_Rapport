#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	Timer();
	~Timer();

	void Start();

	double ElapsedSecounds();
	double ElapsedMilliseconds();
	double ElapsedMicroseconds();

	//Only use this to get the time after any of the Elapsed functions has been used.
	double GetEndTime();

private:
	unsigned __int64 ticksPerSecond;
	unsigned __int64 startTicks;
	unsigned __int64 currentTicks;
	double endTime;
};

#endif