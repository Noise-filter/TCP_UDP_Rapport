#ifndef CLIENT_UDP_H
#define CLIENT_UDP_H

#include "ConnectionUDP.h"

class ClientUDP
{
public:
	ClientUDP();
	~ClientUDP();

	void Connect(char ip[], unsigned short port, bool server);

	void Send(Oyster::Network::OysterByte& byte);
	int Recv(Oyster::Network::OysterByte& byte);

	void Shutdown();

private:
	Oyster::Network::ConnectionUDP connection;

};

#endif