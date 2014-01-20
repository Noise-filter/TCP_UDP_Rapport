#ifndef TIMERS_H
#define TIMERS_H

#include "Timer.h"

class Timers
{
public:
	Timers();
	~Timers();

	void InitTimers(int numberOfTimers); //, bool UDP, bool buffering);

	bool CalculateResultAndSave(int numPackages);
	void printValues();

	Timer* timers;

private:
	int nrOfTimers;
	double average;
	double min;
	double max;
	int numLost;
	int numPackages;
	
	

};

#endif