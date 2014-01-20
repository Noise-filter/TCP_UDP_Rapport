#include "Timers.h"
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

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

bool Timers::CalculateResultAndSave(int numPackages, bool UDP, bool buffering)
{
	average = 0.0;
	min = this->timers[0].GetEndTime();
	max = this->timers[0].GetEndTime();
	numLost = 0;
	this->numPackages = numPackages;
	for(int i = 0; i < numPackages; i++)
	{
		if(this->timers[i].GetEndTime() < 0)
		{
			numLost++;
		}
		else
		{
			if(this->timers[i].GetEndTime() > max)
				max = this->timers[i].GetEndTime();
			
			if(this->timers[i].GetEndTime() < min)
				min = this->timers[i].GetEndTime();

			average += this->timers[i].GetEndTime();
		}
	}
	average /= (numPackages-numLost);


	time_t t = time(0);   // get time now
	struct tm * now = new tm();
    localtime_s( now,  &t );

	char* fileName = new char[75];
	
	int packages = numPackages;
	int year = now->tm_year + 1900;
	int month = now->tm_mon + 1;
	int date = now->tm_mday;
	int hour = now->tm_hour + 1;
	int minutes = now->tm_min;
	int seconds = now->tm_sec;

	if(UDP)
	{
		if(buffering)
		{
			sprintf_s(fileName, 72  , "Research/UDP/Buffering/%d packages %d-%d-%d  %d.%d.%d.txt" , packages, year, month, date, hour, minutes, seconds);
		}

		else
		{
			sprintf_s(fileName, 75 , "Research/UDP/No Buffering/%d packages %d-%d-%d  %d.%d.%d.txt" , packages, year, month, date, hour, minutes, seconds);
		}

	}

	else
	{
		if(buffering)
		{
			sprintf_s(fileName, 72 , "Research/TCP/Buffering/%d packages %d-%d-%d  %d.%d.%d.txt" , packages, year, month, date, hour, minutes, seconds);
		}

		else
		{
			sprintf_s(fileName, 75 , "Research/TCP/No Buffering/%d packages %d-%d-%d  %d.%d.%d.txt" , packages, year, month, date, hour, minutes, seconds);
		}	
	}
  
	ofstream outFile(fileName);

	if(!outFile)
	{
		return false;
	}

	outFile << "Min: " << min << " milliseconds." << endl;
	outFile << "Max: " << max << " milliseconds." << endl;
	outFile << "Average: " << average << " milliseconds." << endl;
	outFile << "sent packages: " << numPackages << endl;
	outFile << "Lost packages: " << numLost << endl;
	outFile << endl;

	for(int i = 0; i < this->numPackages; i++)
	{
		outFile << this->timers[i].GetEndTime() << endl;
	}
	outFile.close();

	delete[] fileName;
	delete now;
	return true;
}

void Timers::printValues()
{
	/*
	for(int i = 0; i < this->numPackages; i++)
	{
		cout << this->timers[i].GetEndTime() << endl;
	}
	*/
	cout << endl;
	cout << "Min: " << min << " milliseconds." << endl;
	cout << "Max: " << max << " milliseconds." << endl;
	cout << "Average: " << average << " milliseconds." << endl;
	cout << "Lost packages: " << numLost << endl;
}