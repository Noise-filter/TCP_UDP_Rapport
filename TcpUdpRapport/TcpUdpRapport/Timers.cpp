#include "Timers.h"

Timers::Timers()
{
	timers = 0;
	nrOfTimers = 0;
}

Timers::~Timers()
{
	if(timers)
	{
		delete[] timers;
		timers = 0;
	}
}

void Timers::InitTimers(int numberOfTimers)
{
	if(timers)
	{
		delete[] timers;
		timers = 0;
	}

	nrOfTimers = numberOfTimers;
	timers = new Timer[nrOfTimers];
}

void Timers::CalculateResultAndSave(char* filename)
{

}