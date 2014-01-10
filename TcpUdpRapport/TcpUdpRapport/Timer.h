#ifndef NETOWRK_DEPENDENCIES_TIMER_H
#define NETOWRK_DEPENDENCIES_TIMER_H

class Timer
{
public:
	Timer();
	~Timer();
	
	void update();
	void reset();

	float runtime();
	float elapsed();

	int framerate();
	double GetSeconds();
	double GetMilliSeconds();
	double GetMicroSeconds();

	int maxFramerate();
	int frameCount();

	void setFixedTimeStep(float step);

private:
	float delta;
	int framesPerSecond;
	int maxFramesPerSecond;
	int FrameCount;

	float fixedDelta;
	bool useFixedStep;

	unsigned __int64 ticksPerSecond;
	unsigned __int64 startupTicks;
	unsigned __int64 currentTicks;
	unsigned __int64 oneSecTicks;
	unsigned __int64 lastTicks;
};

#endif