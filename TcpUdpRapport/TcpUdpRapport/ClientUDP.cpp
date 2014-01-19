#include "ClientUDP.h"

ClientUDP::ClientUDP()
{
	connection = NULL;
}

ClientUDP::~ClientUDP()
{
	delete connection;
	connection = NULL;
}

void ClientUDP::Connect(char ip[], unsigned short port, bool server)
{
	if(connection)
	{
		delete connection;
		connection = NULL;
	}

	connection = new Oyster::Network::ConnectionUDP;

	if(server)
	{
		connection->InitiateServer(port);
		connection->Connect(port+1, ip);
	}
	else
	{
		connection->InitiateServer(port+1);
		connection->Connect(port, ip);
	}
	connection->SetBlockingMode(false);
}

void ClientUDP::TrySendBuffer()
{
	buffering.Send(connection);
}

void ClientUDP::Send(Oyster::Network::OysterByte& byte)
{
	buffering.Buffer(byte, connection);
}

int ClientUDP::Recv(Oyster::Network::OysterByte& byte)
{
	int result = connection->Recieve(byte);

	if(!result)
		buffering.AddRecvMessage(byte);

	result = buffering.Recv(byte);

	return result;
}

void ClientUDP::PureSend(Oyster::Network::OysterByte& byte)
{
	connection->Send(byte);
}

int ClientUDP::PureRecv(Oyster::Network::OysterByte& byte)
{
	return connection->Recieve(byte);
}

void ClientUDP::Shutdown()
{
	connection->Disconnect();
}