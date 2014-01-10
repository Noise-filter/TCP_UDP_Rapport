#include <iostream>
using namespace std;

#include "WinsockFunctions.h"

#include "ClientTCP.h"
#include "ClientUDP.h"
#include "ServerTCP.h"

void ClientUpdate();
void ServerUpdate();

bool RunServer = true;
bool UDP = true;

const char IP[] = "192.168.1.1";

ClientTCP clientTCP;
ClientUDP clientUDP;
ServerTCP serverTCP;

int main()
{
	InitWinSock();

	if(RunServer)
	{
		ServerUpdate();
	}
	else
	{
		ClientUpdate();
	}

	ShutdownWinSock();

	return 0;
}

void ClientUpdate()
{

}

void ServerUpdate()
{

}