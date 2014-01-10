#ifndef CLIENT_UDP_H
#define CLIENT_UDP_H

//#include "ConnectionUDP.h"

class ClientUDP
{
public:
	ClientUDP();
	~ClientUDP();

	void Connect(char ip[], unsigned short port);

	void Send();

	void Shutdown();

private:


};

#endif