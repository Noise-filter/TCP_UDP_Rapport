#ifndef SERVER_TCP_H
#define SERVER_TCP_H

#include "Connection.h"
#include "OysterByte.h"

class ServerTCP
{
public:
	ServerTCP();
	~ServerTCP();

	void Init(unsigned short port);

	int Accept();

	int Recv(Oyster::Network::OysterByte& byte);

	void Shutdown();

private:
	Oyster::Network::Connection connection;

};

#endif