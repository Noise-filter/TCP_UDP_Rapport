#include "Buffering.h"
#include "Packing.h"

using namespace Oyster::Network::Packing;

Buffering::Buffering()
{
	buffering = false;
	hasMessageToSend = false;
	hasRecievedMessage = false;
	sendBuffer.Resize(1000);
	recvBuffer.Resize(1000);
	sendBufferMaxSize = 1000;
	timerLimit = 0.1;
	sendBuffer.SetSize(0);
	timer.Start();
}

Buffering::~Buffering()
{
}

void Buffering::UseBuffering(bool state)
{
	buffering = state;
}

void Buffering::SetBufferTimeLimit(double limit)
{
	timerLimit = limit;
}

void Buffering::SetSendBufferSizeLimit(int maxSize)
{
	sendBufferMaxSize = maxSize;
}

void Buffering::Send(Oyster::Network::IConnection* con)
{
	if(buffering)
	{
		//Checks if there is something in the buffer and that the timer has ended.
		if(hasMessageToSend && timer.ElapsedMilliseconds() > timerLimit)
		{
			SendBuffer(con);
		}
	}
}

void Buffering::SendBuffer(Oyster::Network::IConnection* con)
{
	//Send buffer and reset timer.
	con->Send(sendBuffer);
	sendBuffer.SetSize(0);
	hasMessageToSend = false;
	timer.Start();
}

void Buffering::Buffer(Oyster::Network::OysterByte& byte, Oyster::Network::IConnection* con)
{
	if(buffering)
	{
		//If the sendBuffer + the new message is bigger than max buffer size.
		//Send the sendBuffer directly.
		if(sendBuffer.GetSize() + byte.GetSize() >= sendBufferMaxSize)
		{
			SendBuffer(con);
		}
		
		//Add new message to sendBuffer.
		if(byte.GetSize() <= sendBufferMaxSize)
		{
			sendBuffer += byte;
			hasMessageToSend = true;
		}
	}
	else //Send message directly
	{
		con->Send(byte);
	}

}

bool Buffering::Recv(Oyster::Network::OysterByte& byte)
{
	if(hasRecievedMessage)
	{
		byte.ShallowCopy(recievedMessages.front());
		recievedMessages.pop();
		if(recievedMessages.empty())
			hasRecievedMessage = false;

		return true;
	}
	return false;
}

#include <iostream>
using namespace std;

void Buffering::AddRecvMessage(Oyster::Network::OysterByte& byte)
{
	//Loop through all packages that was recieved and add them to the queue.
	int size = 0;

	Oyster::Network::OysterByte msg;

	//If there is part of a message in the buffer.
	if(recvBuffer.GetSize() > 0)
	{
		int temp = recvBuffer.GetSize();
		size = Unpacki(recvBuffer);
		if(temp + byte.GetSize() > size)
		{
			msg.ShallowCopy(recvBuffer);
			size -= msg.GetSize();
			msg.AddPartOfArray(byte, 0, size);
		}
		else
		{
			recvBuffer += byte;
		}
	}

 	for(int i = size; i < byte.GetSize(); i += size)
	{
		size = Unpacki(&byte.GetByteArray()[i]);
		if(i+size > byte.GetSize())
		{
			//Add it to the recvBuffer instead.
			recvBuffer.CopyPartOfArray(byte, i, byte.GetSize());
			break;
		}
		msg.CopyPartOfArray(byte, i, i+size);
		recievedMessages.push(msg);
		hasRecievedMessage = true;
	}
}