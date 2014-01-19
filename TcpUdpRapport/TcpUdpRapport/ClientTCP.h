#ifndef CLIENT_TCP_H
#define CLIENT_TCP_H

#include "Connection.h"
#include "Buffering.h"

class ClientTCP
{
public:
	ClientTCP();
	~ClientTCP();

	void InitNewClient(int socket);
	bool Connect(char ip[], unsigned short port);

	//Sends the buffer if the timer has ended.
	void TrySendBuffer();

	//Queues the message if buffering is used, if not it sends it directly.
	void Send(Oyster::Network::OysterByte& byte);

	//Will recv messages from the internet, and unbuffers them to the queue. Then gets the next message from the queue.
	int Recv(Oyster::Network::OysterByte& byte);

	//Will only send and recv messages without any extra overhead.
	//Will only be used by the server to pass on the messages back
	// to the client.
	void PureSend(Oyster::Network::OysterByte& byte);
	int PureRecv(Oyster::Network::OysterByte& byte);

	void Shutdown();

private:
	Oyster::Network::Connection* connection;
	Buffering buffering;

};

#endif