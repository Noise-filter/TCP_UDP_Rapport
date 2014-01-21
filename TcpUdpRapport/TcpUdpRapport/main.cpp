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

double packagesPerSec = 0;

string IP = "localhost";
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
	recvMsg.Resize(MAX_MESSAGE_SIZE);
	sendMsg.Resize(MAX_MESSAGE_SIZE);

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
			cout << "1. 1 Packet" << endl;
			cout << "2. 10 Packages" << endl;
			cout << "3. 100 Packages" << endl;
			cout << "4. 1000 Packages" << endl;
			cout << "5. 10000 Packages" << endl;
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
				runTest = true;
				break;
			case 2:
				numPackages = 10;
				runTest = true;
				break;
			case 3:
				numPackages = 100;
				runTest = true;
				break;
			case 4:
				numPackages = 1000;
				runTest = true;
				break;
			case 5:
				numPackages = 10000;
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
			packagesPerSec -= 0;
			cout << "Packages per sec: " << packagesPerSec << endl;
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

bool ClientUpdateTCP(int numPackages)
{
	int id = 0;
	int id2 = -1;

	Timer timer;
	timer.Start();
	double time = timer.ElapsedMilliseconds();
	for(int i = 0; id2 < numPackages-1; )
	{
		clientTCP.TrySendBuffer();
		if(id < numPackages)
		{
			PackProtocolBigPosition(sendMsg, id++);
			timers.timers[id-1].Start();
			clientTCP.Send(sendMsg);
		}

		if(clientTCP.Recv(recvMsg))
		{
			int temp = -1;
			int size = -1;
			UnpackProtocolBigPosition(recvMsg, size, temp);
			//if(recvMsg.GetSize() > 68)
				//cout << temp << ", " << recvMsg.GetSize() << endl;

			timers.timers[temp].ElapsedMilliseconds();
			
			time = timer.ElapsedMilliseconds();

			id2 = temp;
		}

		if(timer.ElapsedMilliseconds() - time > 2000)
			break;

		//Sleep(1);
	}
	timer.ElapsedMilliseconds();

	

	cout << endl;
	cout << "Total time: " << timer.GetEndTime() << " milliseconds." << endl;

	timers.CalculateResultAndSave(numPackages, UDP, buffering);
	timers.printValues();

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

bool ClientUpdateUDP(int numPackages)
{
	int id = 0;
	int id2 = -1;

	Timer timer, timer2;
	timer.Start();
	timer2.Start();
	double time = timer.ElapsedMilliseconds();
	int i = 0;
	for(; i < numPackages; )
	{
		clientUDP.TrySendBuffer();
		if(id < numPackages && timer2.ElapsedMilliseconds() > packagesPerSec)
		{
			PackProtocolBigPosition(sendMsg, id++);
			timers.timers[id-1].Start();
			clientUDP.Send(sendMsg);
			timer2.Start();
		}

		if(clientUDP.Recv(recvMsg))
		{
			int temp = -1;
			int size = -1;
			UnpackProtocolBigPosition(recvMsg, size, temp);
			//cout << temp << ", " << recvMsg.GetSize() << endl;

			timers.timers[temp].ElapsedMilliseconds();
			
			time = timer.ElapsedMilliseconds();

			id2 = temp;
			i++;
		}

		if(timer.ElapsedMilliseconds() - time > 2000)
			break;

		//Sleep(1);
	}
	timer.ElapsedMilliseconds();

	cout << endl;
	cout << "Total packages recieved: " << i << endl;
	cout << "Total time: " << timer.GetEndTime() << " milliseconds." << endl;

	timers.CalculateResultAndSave(numPackages, UDP, buffering);
	timers.printValues();

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