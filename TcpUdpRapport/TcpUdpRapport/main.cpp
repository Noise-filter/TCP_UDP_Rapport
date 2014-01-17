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

bool buffering = false;

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
			int numPackages = 0;
			bool runTest = false;
			int val = 0;
			cout << "Menu" << endl;
			cout << "1. 1 Packet" << endl;
			cout << "2. 10 Packages" << endl;
			cout << "3. 100 Packages" << endl;
			cout << "4. 1000 Packages" << endl;
			cout << "5. 10000 Packages" << endl;
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

bool ClientUpdateTCP(int numPackages)
{
	int id = 0;
	int id2 = -1;

	Timer timer;
	timer.Start();
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

			id2 = temp;
		}

		//Sleep(1);
	}
	timer.ElapsedMilliseconds();

	

	cout << endl;
	cout << "Total time: " << timer.GetEndTime() << " milliseconds." << endl;

	timers.CalculateResultAndSave(numPackages);
	timers.printValues();
	
	return true;
}

bool ServerUpdateTCP()
{
	static int id = -1;
	if(clientTCP.Recv(recvMsg))
	{
		int temp = -1;
		int size = -1;
		UnpackProtocolBigPosition(recvMsg, size, temp);
		//if(recvMsg.GetSize() > 68)
		//cout << temp << ", " << recvMsg.GetSize() << ", " << size << endl;

		//PackProtocolBigPosition(sendMsg, temp);
		clientTCP.Send(recvMsg);
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

bool ClientUpdateUDP(int numPackages)
{
	int id = 0;
	int id2 = -1;

	Timer timer;
	timer.Start();
	for(int i = 0; id2 < numPackages-1; )
	{
		clientUDP.TrySendBuffer();
		if(id < numPackages)
		{
			PackProtocolBigPosition(sendMsg, id++);
			timers.timers[id-1].Start();
			clientUDP.Send(sendMsg);
		}

		if(clientUDP.Recv(recvMsg))
		{
			int temp = -1;
			int size = -1;
			UnpackProtocolBigPosition(recvMsg, size, temp);
			//cout << temp << ", " << recvMsg.GetSize() << endl;

			timers.timers[temp].ElapsedMilliseconds();

			id2 = temp;
		}

		//Sleep(1);
	}
	timer.ElapsedMilliseconds();

	cout << endl;
	cout << "Total time: " << timer.GetEndTime() << " milliseconds." << endl;

	timers.CalculateResultAndSave(numPackages);
	timers.printValues();

	return true;
}

bool ServerUpdateUDP()
{
	static int id = -1;
	if(clientUDP.Recv(recvMsg))
	{
		int temp = -1;
		int size = -1;
		UnpackProtocolBigPosition(recvMsg, size, temp);
		//cout << temp << ", " << recvMsg.GetSize() << ", " << size << endl;

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