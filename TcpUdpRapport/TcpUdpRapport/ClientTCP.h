#ifndef CLIENT_TCP_H
#define CLIENT_TCP_H

//#include "Connection.h"

class ClientTCP
{
public:
	ClientTCP();
	~ClientTCP();

	void Connect(char ip[], unsigned short port);

	void Send();

	void Shutdown();

private:


};

#endif