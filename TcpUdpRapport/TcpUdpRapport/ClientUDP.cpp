#include "ClientUDP.h"

ClientUDP::ClientUDP()
{

}

ClientUDP::~ClientUDP()
{

}

void ClientUDP::Connect(char ip[], unsigned short port, bool server)
{
	if(server)
	{
		connection.InitiateServer(port);
		connection.Connect(port+1, ip);
	}
	else
	{
		connection.InitiateServer(port+1);
		connection.Connect(port, ip);
	}
	connection.SetBlockingMode(false);
}

void ClientUDP::Send(Oyster::Network::OysterByte& byte)
{
	connection.Send(byte);
}

int ClientUDP::Recv(Oyster::Network::OysterByte& byte)
{
	return connection.Recieve(byte);
}

void ClientUDP::Shutdown()
{
	connection.Disconnect();
}