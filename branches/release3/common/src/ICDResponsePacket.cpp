#include "ICDResponsePacket.h"
#include <string.h>

void* ICDResponsePacket::toBuffer()
{
	int size = sizeof(int) + sizeof(int) + sizeof(int) + dataLen;
	void* ret = new char[size + sizeof(int)];
	char* ptr = (char*)ret;

	memcpy(ptr, &size, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, &type, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, &responseType, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, &dataLen, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, data, dataLen);

	return ret;
}
