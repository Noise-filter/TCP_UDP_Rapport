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
	//Pack size infront of the buffer.
	Pack(sendBuffer, sendBuffer.GetSize());

	//Send buffer and reset timer.
	con->Send(sendBuffer);
	sendBuffer.SetSize(4);
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

void Buffering::AddRecvMessage(Oyster::Network::OysterByte& byte)
{
	if(buffering)
	{
		//TODO
		//see if there is anything in the recvBuffer
			//If there is, add the new message to the recvBuffer and see if the size is enough now.
				//If the size isn't enough wait for next message.
				//If the size is enough get all the packages from the recvbuffer and add them to the queue.

		//Get first size from recvBuffer

		//Kolla om size == size of recvBuffer.
	}
	else
	{
		//Loop through all packages that was recieved and add them to the queue.
		int size = 0;
		int initSize = 0;

		Oyster::Network::OysterByte msg;

		//If there is part of a message in the buffer.
		if(recvBuffer.GetSize() > 0)
		{
			int temp = recvBuffer.GetSize();
			msg.ShallowCopy(recvBuffer);
			size = Unpacki(msg);
			initSize = size - msg.GetSize();
			msg.AddPartOfArray(byte, 0, initSize);
		}

		size = 0;

		for(int i = initSize; i < byte.GetSize(); i += size)
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
			//break;
		}
	}
}