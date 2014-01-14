#include "ServerTCP.h"

ServerTCP::ServerTCP()
{
}

ServerTCP::~ServerTCP()
{
}

void ServerTCP::Init(unsigned short port)
{
	int result = true;
	result = connection.InitiateServer(port);
	result = connection.SetBlockingMode(false);
}

int ServerTCP::Recv(Oyster::Network::OysterByte& byte)
{
	return connection.Recieve(byte);
}

int ServerTCP::Accept()
{
	return connection.Listen();
}

void ServerTCP::Shutdown()
{
	connection.Disconnect();
}