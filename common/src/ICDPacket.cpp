#include "ICDPacket.h"
#include "ICDExceptions.h"
#include "ICDCommands.h"
#include "ICDCommandPacket.h"
#include "ICDResponsePacket.h"
#include "Utils.h"
#include <string.h>
#include <iostream>

ICDPacket* ICDPacket::createPacketFromBuffer(void* buf)
{
	if(buf == NULL)
		throw NullPointerException();
	
	ICDPacket* ret = NULL;
	char* ptr = (char*)buf;
	int size = 0;
	memcpy(&size, ptr, sizeof(int));
	ptr += sizeof(int);

//	dumpBuffer((const char*)buf, size + sizeof(int));

	if(size > 0)
	{
		int type = 0;
		memcpy(&type, ptr, sizeof(int));
		ptr += sizeof(int);

		switch(type)
		{
			case ICD_PACKET_TYPE_COMMAND:
				{
					int commandType = 0;
					memcpy(&commandType, ptr, sizeof(int));
					ptr += sizeof(int);

					int argLen = 0;
					memcpy(&argLen, ptr, sizeof(int));
					ptr += sizeof(int);

					void* args = new char[argLen];
					memcpy(args, ptr, argLen);
//					dumpBuffer((const char*)ptr, argLen);

					ret = new ICDCommandPacket(commandType, args, argLen);
				}
				break;
			case ICD_PACKET_TYPE_RESPONSE:
				{
					int responseType = 0;
					memcpy(&responseType, ptr, sizeof(int));
					ptr += sizeof(int);

					int dataLen = 0;
					memcpy(&dataLen, ptr, sizeof(int));
					ptr += sizeof(int);

					void* data = new char[dataLen];
					memcpy(data, ptr, dataLen);

					ret = new ICDResponsePacket(responseType, data, dataLen);
				}
				break;
			default:
				break;
		}
	}

	return ret;
}
