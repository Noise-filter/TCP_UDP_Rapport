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
	byte.SetSize(4);
	Pack(&byte.GetByteArray()[0], id);
}

void UnpackProtocolID(OysterByte& byte, int& id)
{
	id = Unpacki(&byte.GetByteArray()[0]);
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
}

void UnpackProtocolPosition(OysterByte& byte, int& id)
{
	//One matrix
	UnpackProtocolID(byte, id);

	int size = sizeof(id);

	for(int i = 0; i < 16; i++)
	{
		float temp = 0.0f;
		temp = Unpackf(&byte.GetByteArray()[size]);
		size += 4;
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
}

void UnpackProtocolBigPosition(OysterByte& byte, int& id)
{
	//Several matrixes
	int numMatrixes = NUM_MATRIXES;
	UnpackProtocolID(byte, id);

	float temp = 0.0f;
	int size = sizeof(id);
	for(int i = 0; i < 16*numMatrixes; i++)
	{
		temp = Unpackf(&byte.GetByteArray()[size]);
		size += 4;
	}
}

#endif