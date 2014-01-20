#include "ClientTCP.h"

ClientTCP::ClientTCP()
{
	connection = NULL;
}

ClientTCP::~ClientTCP()
{
	delete connection;
	connection = NULL;
}

void ClientTCP::InitNewClient(int socket)
{
	if(connection)
	{
		delete connection;
		connection = NULL;
	}

	connection = new Oyster::Network::Connection(socket);
	connection->SetTCPNODELAY();
	//connection->SetBlockingMode(false);
}

bool ClientTCP::Connect(char ip[], unsigned short port)
{
	if(connection)
	{
		delete connection;
		connection = NULL;
	}

	connection = new Oyster::Network::Connection;
	int result = connection->InitiateClient();
	if(result)
		return false;

	result = connection->Connect(port, ip);
	if(result)
		return false;

	result = connection->SetBlockingMode(false);
	if(result)
		return false;

	connection->SetTCPNODELAY();

	return true;
}

void ClientTCP::UseBuffering(bool state)
{
	buffering.UseBuffering(state);
}

void ClientTCP::SetBufferTimeLimit(double limit)
{
	buffering.SetBufferTimeLimit(limit);
}

void ClientTCP::TrySendBuffer()
{
	buffering.Send(connection);
}

void ClientTCP::Send(Oyster::Network::OysterByte& byte)
{
	buffering.Buffer(byte, connection);
}
#include <iostream>
using namespace std;
int ClientTCP::Recv(Oyster::Network::OysterByte& byte)
{
	int result = connection->Recieve(byte);
	if(!result)
	{
		//cout << "ASD: " << byte.GetSize() << endl;
		buffering.AddRecvMessage(byte);
	}

	result = buffering.Recv(byte);

	return result;
}

void ClientTCP::PureSend(Oyster::Network::OysterByte& byte)
{
	connection->Send(byte);
}

int ClientTCP::PureRecv(Oyster::Network::OysterByte& byte)
{
	return connection->Recieve(byte);
}

void ClientTCP::Shutdown()
{
	connection->Disconnect();
}