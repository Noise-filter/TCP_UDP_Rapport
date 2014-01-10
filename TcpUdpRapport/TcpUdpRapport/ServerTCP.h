#ifndef SERVER_TCP_H
#define SERVER_TCP_H

//#include "Connection.h"

class ServerTCP
{
public:
	ServerTCP();
	~ServerTCP();

	void Init(unsigned short port);

	int Accept();

	void Shutdown();

private:


};

#endif