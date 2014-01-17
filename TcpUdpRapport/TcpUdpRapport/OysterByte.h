#ifndef NETWORK_DEPENDENCIES_OYSTER_BYTE_H
#define NETWORK_DEPENDENCIES_OYSTER_BYTE_H

/////////////////////////////////////
// Created by Pontus Fransson 2013 //
/////////////////////////////////////

#include <string.h>

namespace Oyster
{
	namespace Network
	{
		class OysterByte
		{
		public:
			OysterByte();
			OysterByte(int cap);
			OysterByte(const OysterByte& obj);
			virtual ~OysterByte();

			//Resets size to 0
			void Clear();

			//Resizes the array, it does not keep anything in it.
			void Resize(unsigned int cap);

			int GetSize();
			unsigned char* GetByteArray();

			void AddSize(unsigned int size);
			void SetBytes(unsigned char* bytes);

			//Only sets the private variable 'size'
			void SetSize(unsigned int size);

			//Copies over the pointer, size and capacity. It recreats the copyFrom array and sets size = 0.
			void ShallowCopy(OysterByte& copyFrom);

			//Copies over a part of the copyFrom array. NOT shallow copy.
			void CopyPartOfArray(OysterByte& copyFrom, unsigned int startIndex, unsigned int endIndex);

			//Copies over a part of the addFrom array and adds it to the end of this array.
			void AddPartOfArray(OysterByte& addFrom, unsigned int startIndex, unsigned int endIndex);

			OysterByte& operator =(const OysterByte& obj);

			operator char*();
			operator const char*();
			operator unsigned char*();

			OysterByte& operator +=(const OysterByte& obj);

		private:
			//Expands the byteArray
			void IncreaseCapacity(unsigned int cap);


		private:
			unsigned char* byteArray;
			unsigned int size;
			unsigned int capacity;

		};
	}
}

#endif