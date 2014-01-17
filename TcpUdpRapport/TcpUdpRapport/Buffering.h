#ifndef BUFFERING_H
#define BUFFERING_H

#include <queue>
#include "Timer.h"
#include "OysterByte.h"

#include "IConnection.h"

class Buffering
{
public:
	Buffering();
	~Buffering();
	
	//Used to set if buffering should be used or not.
	void UseBuffering(bool state);
	
	//Set Buffer time limit in milliseconds
	void SetBufferTimeLimit(double limit);

	//Set Maximum send buffer size
	void SetSendBufferSizeLimit(int maxSize);

	//Sends the buffer if the timer has gone out.
	void Send(Oyster::Network::IConnection* con);

	//Add message to buffer. Will send the buffer if the message does not fit into the current buffer.
	void Buffer(Oyster::Network::OysterByte& byte, Oyster::Network::IConnection* con);

	//Will get the next recieved message
	//Return true if there is a new message in byte.
	bool Recv(Oyster::Network::OysterByte& byte);

	//Unbuffers messages and adds them to the queue.
	void AddRecvMessage(Oyster::Network::OysterByte& byte);

private:
	//Packs size infront of buffer, sends buffer, resets buffer, resets timer.
	void SendBuffer(Oyster::Network::IConnection* con);

private:
	bool buffering;
	bool hasMessageToSend;
	bool hasRecievedMessage;

	int sendBufferMaxSize;
	Oyster::Network::OysterByte sendBuffer;
	Oyster::Network::OysterByte recvBuffer;

	std::queue<Oyster::Network::OysterByte> recievedMessages;

	Timer timer;

	//milliseconds.
	double timerLimit;

};

#endif