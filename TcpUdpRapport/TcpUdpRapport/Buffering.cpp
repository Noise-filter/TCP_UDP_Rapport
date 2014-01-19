#include "Buffering.h"
#include "Packing.h"

using namespace Oyster::Network::Packing;

Buffering::Buffering()
{
	buffering = true;
	hasMessageToSend = false;
	hasRecievedMessage = false;
	sendBuffer.Resize(1000);
	recvBuffer.Resize(1000);
	sendBufferMaxSize = 1000;
	timerLimit = 0.1;
	sendBuffer.SetSize(4);
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

#include <iostream>
using namespace std;

void Buffering::AddRecvMessage(Oyster::Network::OysterByte& byte)
{
	if(buffering)
	{
		Oyster::Network::OysterByte msg;
		int size = 0;
		//see if there is anything in the recvBuffer
		if(recvBuffer.GetSize() > 0)
		{
			//If there is, add the new message to the recvBuffer and see if the size is enough now.
			size = Unpacki(recvBuffer);
			int totalSize = recvBuffer.GetSize() + byte.GetSize();
			if(totalSize < size)
			{
				recvBuffer += byte;
			}
			else if(totalSize == size)
			{
				recvBuffer += byte;
				msg.ShallowCopy(recvBuffer);
			}
			else //totalSize > size
			{

			}
		}
		else
		{
			size = Unpacki(byte);
			if(size == byte.GetSize())
			{
				size = 0;
				//Split a buffer message into several real messages.
				for(int i = 4; i < byte.GetSize(); i += size)
				{
					size = Unpacki(&byte.GetByteArray()[i]);
					msg.CopyPartOfArray(byte, i, i+size);
					recievedMessages.push(msg);
					hasRecievedMessage = true;
				}
			}
			else if(size < byte.GetSize())
			{
				int bufferSize = size;
				size = 0;

				//Go through all the buffer messages in this package.
				for(int j = 0; j < byte.GetSize(); j += bufferSize)
				{
					bufferSize = Unpacki(&byte.GetByteArray()[j]);

					//If a buffer package is not completely in the package.
					//Add it to the recievebuffer so it can be completed with the next recieved package.
					if(bufferSize + j > byte.GetSize())
					{
						int temp = j + bufferSize;
						if(temp > byte.GetSize())
							temp = byte.GetSize();
						recvBuffer.CopyPartOfArray(byte, j, temp);
					}
					else
					{
						//Split a buffer message into several real messages.
						for(int i = 4+j; i < bufferSize+j; i += size)
						{
							size = Unpacki(&byte.GetByteArray()[i]);
							msg.CopyPartOfArray(byte, i, i+size);
							recievedMessages.push(msg);
							hasRecievedMessage = true;
						}
					}
				}
			}
			else //size > byte.GetSize();
			{
				//Copy entire package into the recvBuffer.
				recvBuffer.CopyPartOfArray(byte, 0, byte.GetSize());
			}
		}

		//TODO
				//If the size isn't enough wait for next message.
				//If the size is enough get all the packages from the recvbuffer and add them to the queue.

		//Get first size from recvBuffer

		//Kolla om size == size of recvBuffer.
	}
	else
	{
		//if(recievedMessages.size() > 0)
		//	cout << "Queue size: " << recievedMessages.size() << endl;

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
		}
	}
}