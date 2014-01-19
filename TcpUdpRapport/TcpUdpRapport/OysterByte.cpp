#include "OysterByte.h"

using namespace Oyster::Network;

OysterByte::OysterByte()
{
	size = 0;
	capacity = 10;
	byteArray = new unsigned char[capacity];
}

OysterByte::OysterByte(int cap)
{
	size = 0;
	capacity = cap;
	byteArray = new unsigned char[capacity];
}

OysterByte::OysterByte(const OysterByte& obj)
{
	this->byteArray = new unsigned char[obj.capacity];

	for(int i = 0; i < (int)obj.size; i++)
	{
		this->byteArray[i] = obj.byteArray[i];
	}
	this->size = obj.size;
	this->capacity = obj.capacity;
}

OysterByte::~OysterByte()
{
	delete[] byteArray;
	byteArray = NULL;
}

void OysterByte::Clear()
{
	size = 0;
}

void OysterByte::Resize(unsigned int cap)
{
	if(capacity < cap)
	{
		delete[] byteArray;
		byteArray = new unsigned char[cap];
		capacity = cap;
	}
}

int OysterByte::GetSize()
{
	return size;
}

unsigned char* OysterByte::GetByteArray()
{
	return byteArray;
}

void OysterByte::AddSize(unsigned int size)
{
	unsigned int newCapacity = this->size + size;

	if(newCapacity >= capacity)
	{
		IncreaseCapacity(newCapacity);
	}

	this->size += size;
}

void OysterByte::SetBytes(unsigned char* bytes)
{
	delete[] byteArray;
	byteArray = bytes;
}

void OysterByte::SetSize(unsigned int size)
{
	this->size = size;
}

void OysterByte::ShallowCopy(OysterByte& copyFrom)
{
	delete[] byteArray;
	byteArray = copyFrom.byteArray;
	size = copyFrom.size;
	capacity = copyFrom.capacity;
	copyFrom.byteArray = new unsigned char[copyFrom.capacity];
	copyFrom.size = 0;
}

void OysterByte::CopyPartOfArray(OysterByte& copyFrom, unsigned int startIndex, unsigned int endIndex)
{
	if(startIndex < 0 && startIndex >= endIndex)
		return;
	if(endIndex > copyFrom.size)
		return;

	delete[] byteArray;
	capacity = copyFrom.capacity;
	size = endIndex-startIndex;
	byteArray = new unsigned char[capacity];

	for(unsigned int i = 0; i < size; i++)
	{
		byteArray[i] = copyFrom.byteArray[startIndex++];
	}
}

void OysterByte::AddPartOfArray(OysterByte& addFrom, unsigned int startIndex, unsigned int endIndex)
{
	if(startIndex < 0 && startIndex >= endIndex)
		return;
	if(endIndex > addFrom.size)
		return;

	int totalSize = endIndex - startIndex;
	totalSize += size;

	if(totalSize > capacity)
	{
		IncreaseCapacity(totalSize);
	}

	for(int i = size; i < totalSize; i++)
	{
		byteArray[i] = addFrom.byteArray[startIndex++];
	}

	size = totalSize;
}

OysterByte& OysterByte::operator =(const OysterByte& obj)
{
	delete[] this->byteArray;
	this->byteArray = new unsigned char[obj.capacity];

	for(int i = 0; i < (int)obj.size; i++)
	{
		this->byteArray[i] = obj.byteArray[i];
	}
	this->size = obj.size;
	this->capacity = obj.capacity;

	return *this;
}

OysterByte::operator char*()
{
	return (char*)byteArray;
}

OysterByte::operator const char*()
{
	return (const char*)byteArray;
}

OysterByte::operator unsigned char*()
{
	return byteArray;
}

OysterByte& OysterByte::operator +=(const OysterByte& obj)
{
	int newSize = this->size + obj.size;
	
	if(newSize >= (int)capacity)
	{
		IncreaseCapacity(newSize);
	}

	for(int i = size, j = 0; i < newSize; i++, j++)
	{
		this->byteArray[i] = obj.byteArray[j];
	}
	
	this->size = newSize;

	return *this;
}

/////////////
// Private //
/////////////

void OysterByte::IncreaseCapacity(unsigned int newCapacity)
{
	capacity = newCapacity * 2;
	unsigned char* temp = new unsigned char[capacity];
	
	for(int i = 0; i < (int)this->size; i++)
	{
		temp[i] = byteArray[i];
	}

	delete[] byteArray;
	byteArray = temp;
}