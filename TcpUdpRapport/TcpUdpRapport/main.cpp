#include <iostream>
#include <Windows.h>
#include <conio.h>

#include "WinsockFunctions.h"
#include "OysterByte.h"

#include "Timers.h"
#include "Config.h"

#include "ClientTCP.h"
#include "ClientUDP.h"
#include "ServerTCP.h"

#include "Protocols.h"

using namespace Oyster::Network;
using namespace std;

bool ClientUpdateTCP(int numPackages);
bool ServerUpdateTCP();
bool ClientUpdateUDP(int numPackages);
bool ServerUpdateUDP();

bool RunServer = false;
bool UDP = true;

string IP = "localhost";
unsigned short PortTCP = 6789;
unsigned short PortUDP = 9876;
unsigned short Port = 9876;

ClientTCP clientTCP;
ClientUDP clientUDP;
ServerTCP serverTCP;

OysterByte recvMsg;
OysterByte sendMsg;

Config configFile;

Timers timers;

int main()
{
	timers.InitTimers(1000000);
	recvMsg.Resize(1000);
	sendMsg.Resize(1000);

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
			clientUDP.Connect((char*)IP.c_str(), PortUDP, RunServer);
		else
			serverTCP.Init(PortTCP);
	}
	else
	{
		cout << "Client" << endl;
		if(UDP)
			clientUDP.Connect((char*)IP.c_str(), PortUDP, RunServer);
		else
			clientTCP.Connect((char*)IP.c_str(), PortTCP);
	}

	//Accept clients on the tcp server. stops when you hit 'space'.
	bool keepLooping = true;
	if(RunServer && !UDP)
	{
		while(keepLooping)
		{
			int client = serverTCP.Accept();
			if(client > 0)
			{
				clientTCP.InitNewClient(client);
				cout << client << " new client." << endl;
				client = -1;
			}
			
			if(_kbhit())
			{
				int key = _getch();
				cout << key << endl;
				if(key == 32)	//Space
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
				keepLooping = ServerUpdateTCP();
		}
		else
		{
			int val = 0;
			cout << "Menu" << endl;
			cout << "1. Test 1" << endl;
			cout << "2. Test 2" << endl;
			cout << "3. 10,000" << endl;
			cout << "0. Quit" << endl;
			cin >> val;

			switch(val)
			{
			case 1:
				if(UDP)
					keepLooping = ClientUpdateUDP(1000);
				else
					keepLooping = ClientUpdateTCP(1000);
				break;
			case 2:
				if(UDP)
					keepLooping = ClientUpdateUDP(2000);
				else
					keepLooping = ClientUpdateTCP(2000);
				break;
			case 3:
				if(UDP)
					keepLooping = ClientUpdateUDP(10000);
				else
					keepLooping = ClientUpdateTCP(10000);
				break;
			case 0:
				keepLooping = false;
				break;
			default:
				break;
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

bool ClientUpdateTCP(int numPackages)
{
	
	
	return true;
}

bool ServerUpdateTCP()
{
	if(!clientTCP.Recv(recvMsg))
	{
		cout << recvMsg.GetByteArray() << endl;
	}

	return true;
}

const int NUM_PACKAGES = 1000;

bool ClientUpdateUDP(int numPackages)
{
	int id = 0;
	int id2 = -1;

	Timer timer;
	timer.Start();
	for(int i = 0; id2 < numPackages-1; )
	{
		if(id < numPackages)
		{
			PackProtocolBigPosition(sendMsg, id++);
			clientUDP.Send(sendMsg);
			timers.timers[id-1].Start();
		}

		if(!clientUDP.Recv(recvMsg))
		{
			int temp = -1;
			UnpackProtocolBigPosition(recvMsg, temp);
			//cout << temp << ", " << recvMsg.GetSize() << endl;

			timers.timers[temp].ElapsedMicroseconds();

			id2 = temp;
		}

		//Sleep(1);
	}
	timer.ElapsedMilliseconds();

	double average = 0.0;
	double min = timers.timers[0].GetEndTime();
	double max = timers.timers[0].GetEndTime();
	int numLost = 0;
	for(int i = 0; i < numPackages; i++)
	{
		if(timers.timers[i].GetEndTime() < 0)
		{
			numLost++;
		}
		else
		{
			if(timers.timers[i].GetEndTime() > max)
				max = timers.timers[i].GetEndTime();
			
			if(timers.timers[i].GetEndTime() < min)
				min = timers.timers[i].GetEndTime();

			average += timers.timers[i].GetEndTime();
			//cout << timers.timers[i].GetEndTime() << endl;
		}
	}

	cout << endl;
	cout << "Total time: " << timer.GetEndTime() << " milliseconds." << endl;
	cout << "Min: " << min << " microseconds." << endl;
	cout << "Max: " << max << " microseconds." << endl;
	cout << "Average: " << average/(numPackages-numLost) << " microseconds." << endl;
	cout << "Lost packages: " << numLost << endl;
	cout << id << endl << endl;

	return true;
}

bool ServerUpdateUDP()
{
	static int id = -1;
	if(!clientUDP.Recv(recvMsg))
	{
		int temp = -1;
		UnpackProtocolBigPosition(recvMsg, temp);
		//cout << temp << ", " << recvMsg.GetSize() << endl;

		//PackProtocolBigPosition(sendMsg, temp);
		clientUDP.Send(recvMsg);
		/*
		if(temp == id+1)
		{
			id = temp;
		}
		else
		{
			cout << temp - id - 1 << endl;
			id = temp;
		}*/
	}
	//Sleep(1);

	return true;
}

/*
	Protocols:
		Every protocol will contain an int as an ID. from 0 to unlimited. So that we can identify the packages.
		
		Simple ID protocol: Only containing the ID.
		Position Protocol: Containing an 4x4 matrix.
		BigPosition Protocol: Containing x * 4x4 matrix.
		

*/