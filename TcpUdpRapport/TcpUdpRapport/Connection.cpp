#include "Connection.h"

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include "Defines.h"

using namespace Oyster::Network;

Connection::Connection()
{
	this->socket = -1;
	stillSending = false;
	closed = true;
}

Connection::Connection(int socket)
{
	this->socket = socket;
	stillSending = false;
	closed = true;
	SetTCPNODELAY();
}

Connection::~Connection()
{
	closesocket( this->socket );
}

int Connection::Connect(unsigned short port , const char serverName[])
{
	struct hostent *hostEnt;
	if((hostEnt = gethostbyname(serverName)) == NULL)
	{
		return SOCKET_ERROR;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = *(unsigned long*) hostEnt->h_addr;

	if(connect(this->socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	closed = false;
	stillSending = true;

	//connection succesfull!
	return 0;
}


int Connection::Disconnect()
{
	int result = closesocket(this->socket);

	if(result == SOCKET_ERROR)
	{
		return result;
	}

	return 0;
}

int Connection::InitiateServer(unsigned short port)
{
	int errorCode = 0;

	if((errorCode = InitiateSocket()) != 0)
	{
		return errorCode;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(this->socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		errorCode = GetLastError();
		errorCode = SOCKET_ERROR;
		closesocket(this->socket);
		return errorCode;
	}

	//not our Listen function! its trying to keep our socket open for connections
	if(listen(this->socket, 5) == SOCKET_ERROR)
	{
		errorCode = SOCKET_ERROR;
		closesocket(this->socket);
		return errorCode;
	}

	closed = false;
	stillSending = true;

	//Server started!
	return 0;
}

int Connection::InitiateClient()
{
	return InitiateSocket();
}
#include <iostream>
int Connection::Send(OysterByte &bytes)
{
	int nBytes;
	
	//std::cout << "Size: " << bytes.GetSize() << std::endl;
	int result;
	int count = 0;
	do
	{
		nBytes = send(this->socket, bytes, bytes.GetSize(), 0);
		if(nBytes == SOCKET_ERROR)
		{
			result = WSAGetLastError();
			if(result == 10035)
			{
				count++;
				if(count >= 2)
					std::cout << count << std::endl;
				continue;
			}
			else
				return result;
		}
	}while(0);

	return 0;
}

int Connection::Recieve(OysterByte &bytes)
{
	int nBytes;

	bytes.Resize(MAX_MESSAGE_SIZE);
	nBytes = recv(this->socket, bytes, MAX_MESSAGE_SIZE, 0);
	if(nBytes == SOCKET_ERROR)
	{
		bytes.SetSize(0);
		int result = WSAGetLastError();
		if(result == WSAECONNRESET)
			return 2;

		if(result != WSAEWOULDBLOCK)
			std::cout << result << std::endl;

		return result;
	}
	else
	{
		bytes.SetSize(nBytes);
	}
	if(nBytes == 0)	//Connection dropped
		return 2;

	return 0;
}

//Listen will only return the correct socket or -1 for failure.
int Connection::Listen()
{
	int clientSocket;
	if((clientSocket = (int)accept(this->socket, NULL, NULL)) == INVALID_SOCKET)
	{
		return (int)INVALID_SOCKET;
	}

	return clientSocket;
}

bool Connection::IsSending()
{
	return stillSending;
}

bool Connection::IsConnected()
{
	return !closed;
}

int Connection::SetBlockingMode(bool blocking)
{
	DWORD nonBlocking;

	if(blocking)
	{
		nonBlocking = 0;
	}
	else
	{
		nonBlocking = 1;
	}

	int result = ioctlsocket(this->socket, FIONBIO, &nonBlocking);
	if(result != 0)
	{
		return result;
	}

	//Success
	return 0;
}

int Connection::SetTCPNODELAY()
{
	int flag = 1;
	int result = setsockopt(this->socket,		/* socket affected */
							IPPROTO_TCP,		/* set option at TCP level */
							TCP_NODELAY,		/* name of option */
							(char *) &flag,		/* the cast is historical cruft */
							sizeof(int));		/* length of option value */
	if (result < 0)
		return -1;
	return 0;
}

///////////////////////////////////////
//Private functions
///////////////////////////////////////
int Connection::InitiateSocket()
{
	this->socket = (int)::socket(AF_INET, SOCK_STREAM, 0);
	if(this->socket == SOCKET_ERROR)
	{
		return socket;
	}

	if(SetTCPNODELAY())
	{
		return -1;
	}

	return 0;
}