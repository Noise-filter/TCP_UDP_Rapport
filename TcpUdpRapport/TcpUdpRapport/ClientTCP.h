#ifndef CLIENT_TCP_H
#define CLIENT_TCP_H

#include "Connection.h"

class ClientTCP
{
public:
	ClientTCP();
	~ClientTCP();

	void InitNewClient(int socket);
	void Connect(char ip[], unsigned short port);

	void Send(Oyster::Network::OysterByte& byte);
	int Recv(Oyster::Network::OysterByte& byte);

	void Shutdown();

private:
	Oyster::Network::Connection connection;

};

#endif