#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <vld.h>

#include "WinsockFunctions.h"
#include "OysterByte.h"

#include "Timers.h"
#include "Config.h"

#include "ClientTCP.h"
#include "ClientUDP.h"
#include "ServerTCP.h"

#include "Protocols.h"

#include "Defines.h"

using namespace Oyster::Network;
using namespace std;

bool ClientUpdateTCP(int numPackages);
bool ServerUpdateTCP();
bool ClientUpdateUDP(int numPackages);
bool ServerUpdateUDP();

bool RunServer = false;
bool UDP = true;

bool buffering = false;
double bufferTimeLimit = 0.02;

double pps[4];
const int NUM_DIFFERENT_PACKAGES = 4;
const int PACKAGE_SIZES[NUM_DIFFERENT_PACKAGES] = {
	8,
	64,
	400,
	1000,
};

string IP = "localhost";
unsigned short Port = 9876;

ClientTCP clientTCP;
ClientUDP clientUDP;
ServerTCP serverTCP;

OysterByte recvMsg;
OysterByte sendMsg;

Config configFile;

Timers timers[NUM_DIFFERENT_PACKAGES];

int main()
{
	timers[0].InitTimers(1000000);
	timers[1].InitTimers(1000000);
	timers[2].InitTimers(1000000);
	timers[3].InitTimers(1000000);
	recvMsg.Resize(MAX_MESSAGE_SIZE);
	sendMsg.Resize(MAX_MESSAGE_SIZE);

	pps[0] = (double)1/(double)4000 * 1000;
	pps[1] = (double)1/(double)4000 * 1000;
	pps[2] = (double)1/(double)2000 * 1000;
	pps[3] = (double)1/(double)1000 * 1000;

	InitWinSock();

	if(!configFile.LoadFile("Config.txt"))
	{
		cout << "Could not load or create a default config file." << endl;
	}
	else
	{
		IP = configFile.ip;
		Port = configFile.port;
		UDP = configFile.udp;
		RunServer = configFile.server;
	}

	//Initiate clients and servers based on global variables 'RunServer' and 'UDP'
	if(RunServer)
	{
		cout << "Server" << endl;
		if(UDP)
			clientUDP.Connect((char*)IP.c_str(), Port, RunServer);
		else
			serverTCP.Init(Port);
	}
	else
	{
		cout << "Client" << endl;
		if(UDP)
			clientUDP.Connect((char*)IP.c_str(), Port, RunServer);
		else
			if(!clientTCP.Connect((char*)IP.c_str(), Port))
			{
				return 0;
			}
	}

	if(UDP)
		cout << "UDP" << endl;
	else
		cout << "TCP" << endl;

	//Accept clients on the tcp server. stops when you hit 'space'.
	bool keepLooping = true;
	if(RunServer && !UDP)
	{
connections:
		cout << "Listening for new client." << endl;
		while(keepLooping)
		{
			int client = serverTCP.Accept();
			if(client > 0)
			{
				clientTCP.InitNewClient(client);
				cout << client << " new client." << endl;
				client = -1;
				break;
			}

			Sleep(1);
		}
	}

	cout << "Start test" << endl;

	keepLooping = true;
	while(keepLooping)
	{
		if(RunServer)
		{
			if(UDP)
				keepLooping = ServerUpdateUDP();
			else
				if(!ServerUpdateTCP())
				{
					goto connections;
				}
		}
		else
		{
			int numPackages = 0;
			bool runTest = false;
			int val = 5;
			cout << "Menu" << endl;
			cout << "1. 1 seconds" << endl;
			cout << "2. 2 seconds" << endl;
			cout << "3. 3 seconds" << endl;
			cout << "4. 4 seconds" << endl;
			cout << "5. 5 seconds" << endl;
			if(buffering)
				cout << "9. Turn buffering off" << endl;
			else
				cout << "9. Turn buffering on" << endl;
			cout << "0. Quit" << endl;
			cin >> val;

			switch(val)
			{
			case 1:
				numPackages = 1;
				cout << "How long? ";
				cin >> numPackages;
				runTest = true;
				break;
			case 2:
				numPackages = 2;
				runTest = true;
				break;
			case 3:
				numPackages = 3;
				runTest = true;
				break;
			case 4:
				numPackages = 4;
				runTest = true;
				break;
			case 5:
				numPackages = 5;
				runTest = true;
				break;
			case 9:
				buffering = !buffering;
				clientUDP.UseBuffering(buffering);
				clientUDP.SetBufferTimeLimit(bufferTimeLimit);
				clientTCP.UseBuffering(buffering);
				clientTCP.SetBufferTimeLimit(bufferTimeLimit);
				break;
			case 0:
				keepLooping = false;
				break;
			default:
				break;
			}

			if(runTest)
			{
				if(UDP)
					keepLooping = ClientUpdateUDP(numPackages);
				else
					keepLooping = ClientUpdateTCP(numPackages);
			}
		}
	}

	//Close down all clients and servers started.
	if(RunServer)
	{
		if(UDP)
			clientUDP.Shutdown();
		else
		{
			serverTCP.Shutdown();
			clientTCP.Shutdown();
		}
	}
	else
	{
		if(UDP)
			clientUDP.Shutdown();
		else
			clientTCP.Shutdown();
		system("PAUSE");
	}

	ShutdownWinSock();

	return 0;
}

bool ClientUpdateTCP(int seconds)
{
	int id[NUM_DIFFERENT_PACKAGES];
	int id2[NUM_DIFFERENT_PACKAGES];
	Timer ppsTimers[NUM_DIFFERENT_PACKAGES];

	Timer timer, totalTestTimer;
	timer.Start();
	totalTestTimer.Start();

	for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
	{
		ppsTimers[i].Start();
		id[i] = id2[i] = 0;
	}

	bool done = false;
	double time = timer.ElapsedMilliseconds();
	while(!done)
	{
		clientTCP.TrySendBuffer();
		if(totalTestTimer.ElapsedSecounds() < seconds)
		{
			for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
			{
				if(ppsTimers[i].ElapsedMilliseconds() > pps[i])
				{
					packStuff(sendMsg, PACKAGE_SIZES[i], id[i]++);
					timers[i].timers[id[i]-1].Start();
					clientTCP.Send(sendMsg);
					//cout << "Size: " << sendMsg.GetSize() << endl;
					ppsTimers[i].Start();
				}
			}
		}
		else
		{
			/*done = true;
			for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
			{
				if(id2[i] < id[i])
				{
					done = false;
					break;
				}
			}*/
		}

		if(clientTCP.Recv(recvMsg))
		{
			int temp = -1;
			int size = -1;
			unpackStuff(recvMsg, size, temp);
			//if(recvMsg.GetSize() > 68)
				//cout << temp << ", " << recvMsg.GetSize() << endl;

			for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
			{
				if(size == PACKAGE_SIZES[i])
				{
					timers[i].timers[temp].ElapsedMilliseconds();
					id2[i]++;
				}
			}

			time = timer.ElapsedMilliseconds();
		}

		if(timer.ElapsedMilliseconds() - time > 2000)
			break;
	}

	cout << endl;
	cout << "Total time: " << timer.GetEndTime() << " milliseconds." << endl;

	for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
	{
		timers[i].CalculateResultAndSave(id[i], UDP, buffering, i);
		timers[i].printValues();
	}

	return true;
}

bool ServerUpdateTCP()
{
	int result = clientTCP.PureRecv(recvMsg);
	if(!result)
	{
		//cout << recvMsg.GetSize() << endl;	//Print out the size of the recieved message

		clientTCP.PureSend(recvMsg);
	}
	else if(result == 2)	//Client dropped
	{
		return false;
	}

	return true;
}

bool ClientUpdateUDP(int seconds)
{
	int id[NUM_DIFFERENT_PACKAGES];
	int id2[NUM_DIFFERENT_PACKAGES];
	Timer ppsTimers[NUM_DIFFERENT_PACKAGES];

	Timer timer, totalTestTimer;
	timer.Start();
	totalTestTimer.Start();

	for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
	{
		ppsTimers[i].Start();
		id[i] = id2[i] = 0;
	}

	bool done = false;
	double time = timer.ElapsedMilliseconds();
	while(!done)
	{
		clientUDP.TrySendBuffer();
		if(totalTestTimer.ElapsedSecounds() < seconds)
		{
			for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
			{
				if(ppsTimers[i].ElapsedMilliseconds() > pps[i])
				{
					packStuff(sendMsg, PACKAGE_SIZES[i], id[i]++);
					timers[i].timers[id[i]-1].Start();
					clientUDP.Send(sendMsg);
					//cout << "Size: " << sendMsg.GetSize() << endl;
					ppsTimers[i].Start();
				}
			}
		}
		else
		{
			/*done = true;
			for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
			{
				if(id2[i] < id[i])
				{
					done = false;
					break;
				}
			}*/
		}

		if(clientUDP.Recv(recvMsg))
		{
			int temp = -1;
			int size = -1;
			unpackStuff(recvMsg, size, temp);
			//if(recvMsg.GetSize() > 68)
				//cout << temp << ", " << recvMsg.GetSize() << endl;

			for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
			{
				if(size == PACKAGE_SIZES[i])
				{
					timers[i].timers[temp].ElapsedMilliseconds();
					id2[i]++;
				}
			}

			time = timer.ElapsedMilliseconds();
		}

		if(timer.ElapsedMilliseconds() - time > 2000)
			break;
	}

	cout << endl;
	cout << "Total time: " << timer.GetEndTime() << " milliseconds." << endl;

	for(int i = 0; i < NUM_DIFFERENT_PACKAGES; i++)
	{
		timers[i].CalculateResultAndSave(id[i], UDP, buffering, i);
		timers[i].printValues();
	}

	return true;
}

bool ServerUpdateUDP()
{
	if(!clientUDP.PureRecv(recvMsg))
	{
		//cout << recvMsg.GetSize() << endl;	//Print out the size of the recieved message

		clientUDP.PureSend(recvMsg);
	}

	return true;
}

/*
	Protocols:
		Every protocol will contain an int as an ID. from 0 to unlimited. So that we can identify the packages.
		
		Simple ID protocol: Only containing the ID.
		Position Protocol: Containing an 4x4 matrix.
		BigPosition Protocol: Containing x * 4x4 matrix.

*/