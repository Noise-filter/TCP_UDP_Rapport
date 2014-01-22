#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include <iostream>
using std::cout;
using std::endl;

#include "Packing.h"
#include "OysterByte.h"
using namespace Oyster::Network::Packing;
using Oyster::Network::OysterByte;

void PackProtocolID(OysterByte& byte, int id)
{
	//One id
	byte.SetSize(8);
	Pack(&byte.GetByteArray()[0], 8);
	Pack(&byte.GetByteArray()[4], id);
}

void UnpackProtocolID(OysterByte& byte, int& size, int& id)
{
	size = Unpacki(&byte.GetByteArray()[0]);
	id = Unpacki(&byte.GetByteArray()[4]);
}

void PackProtocolPosition(OysterByte& byte, int id)
{
	//One matrix
	PackProtocolID(byte, id);

	float temp = 1.23f;
	int size = byte.GetSize();
	for(int i = 0; i < 16; i++)
	{
		Pack(&byte.GetByteArray()[size], temp);
		temp++;
		size += 4;
	}
	byte.SetSize(size);
	Pack(&byte.GetByteArray()[0], size);
}

void UnpackProtocolPosition(OysterByte& byte, int& size, int& id)
{
	//One matrix
	UnpackProtocolID(byte, size, id);
	
	float temp = 0.0f;
	int index = sizeof(id) + sizeof(size);
	for(int i = 0; i < 16; i++)
	{
		temp = Unpackf(&byte.GetByteArray()[index]);
		index += 4;
	}
}

//Change this to increase/decrease the number of matrixes that is sent.
const int NUM_MATRIXES = 1;
void PackProtocolBigPosition(OysterByte& byte, int id)
{
	//Several matrixes
	int numMatrixes = NUM_MATRIXES;
	PackProtocolID(byte, id);

	float temp = 1.23f;
	int size = byte.GetSize();
	for(int i = 0; i < 16*numMatrixes; i++)
	{
		Pack(&byte.GetByteArray()[size], temp);
		temp++;
		size += 4;
	}
	byte.SetSize(size);
	Pack(&byte.GetByteArray()[0], size);
}

void UnpackProtocolBigPosition(OysterByte& byte, int& size, int& id)
{
	//Several matrixes
	int numMatrixes = NUM_MATRIXES;
	UnpackProtocolID(byte, size, id);

	float temp = 0.0f;
	int index = sizeof(id) + sizeof(size);
	for(int i = 0; i < 16*numMatrixes; i++)
	{
		temp = Unpackf(&byte.GetByteArray()[index]);
		index += 4;
	}
}

void packStuff(OysterByte& byte, int size, int id)
{
	if(size > 7)
	{
		PackProtocolID(byte, id);
		char temp = 1;
		for(int i = 8; i < size; i++)
		{
			Pack(&byte.GetByteArray()[i], temp);
		}

		byte.SetSize(size);
		Pack(&byte.GetByteArray()[0], size);
	}
}

void unpackStuff(OysterByte& byte, int &size, int &id)
{
	UnpackProtocolID(byte, size, id);

	float temp = 0.0f;
	for(int i = 8; i < byte.GetSize(); i++)
	{
		temp = Unpackf(&byte.GetByteArray()[i]);
	}
}

#endif