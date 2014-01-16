#include "ClientTCP.h"

ClientTCP::ClientTCP()
{
	
}

ClientTCP::~ClientTCP()
{

}

void ClientTCP::InitNewClient(int socket)
{
	connection = new Oyster::Network::Connection(socket);
	connection->SetTCPNODELAY();
	//connection->InitiateClient();
	//connection->SetBlockingMode(false);
}

bool ClientTCP::Connect(char ip[], unsigned short port)
{
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

void ClientTCP::Send(Oyster::Network::OysterByte& byte)
{
	int result = connection->Send(byte);
}


int ClientTCP::Recv(Oyster::Network::OysterByte& byte)
{
	int result = connection->Recieve(byte);
	return result;
}

void ClientTCP::Shutdown()
{
	connection->Disconnect();
}