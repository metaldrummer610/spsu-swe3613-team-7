#include "ICDCommandPacket.h"
#include <string.h>

void* ICDCommandPacket::toBuffer()
{
	// This is: (type)(commandType)(argLength)(args)
	int size = sizeof(int) + sizeof(int) + sizeof(int) + argLen;
	// All that, plus the first int for the size of the entire thing
	void* ret = new char[size + sizeof(int)];
	char* ptr = (char*)ret;

	// Add the size of the packet
	memcpy(ptr, &size, sizeof(int));
	ptr += sizeof(int);

	// Add the packet type
	memcpy(ptr, &type, sizeof(int));
	ptr += sizeof(int);

	// Add the command type
	memcpy(ptr, &commandType, sizeof(int));
	ptr += sizeof(int);

	// Add the size of the args
	memcpy(ptr, &argLen, sizeof(int));
	ptr += sizeof(int);

	// Add the args themselves
	memcpy(ptr, args, argLen);

	return ret;
}
