#ifndef TIMERS_H
#define TIMERS_H

#include "Timer.h"

class Timers
{
public:
	Timers();
	~Timers();

	void InitTimers(int numberOfTimers);

	void CalculateResultAndSave(char* filename);

	Timer* timers;

private:
	int nrOfTimers;
	

};

#endif