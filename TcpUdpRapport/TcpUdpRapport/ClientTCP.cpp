#include "ClientTCP.h"

ClientTCP::ClientTCP()
{
	
}

ClientTCP::~ClientTCP()
{

}

void ClientTCP::InitNewClient(int socket)
{
	connection = Oyster::Network::Connection(socket);
	connection.SetBlockingMode(false);
}

void ClientTCP::Connect(char ip[], unsigned short port)
{
	connection.InitiateClient();
	connection.Connect(port, ip);
	connection.SetBlockingMode(false);
}

void ClientTCP::Send(Oyster::Network::OysterByte& byte)
{
	connection.Send(byte);
}

int ClientTCP::Recv(Oyster::Network::OysterByte& byte)
{
	return connection.Recieve(byte);
}

void ClientTCP::Shutdown()
{
	connection.Disconnect();
}