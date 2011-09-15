#ifndef _ICD_COMMAND_PACKET_H
#define _ICD_COMMAND_PACKET_H

#include "ICDNetwork.h"

class ICDCommandPacket : public ICDPacket
{
public:
	ICDCommandPacket() : ICDPacket(ICD_PACKET_TYPE_COMMAND), commandType(0), args(NULL), argLen(0) {}
	ICDCommandPacket(int cmdType) : ICDPacket(ICD_PACKET_TYPE_COMMAND), commandType(cmdType), args(NULL), argLen(0) {}
	ICDCommandPacket(int cmdType, void* a, int l) :ICDPacket(ICD_PACKET_TYPE_COMMAND), commandType(cmdType), args(a), argLen(l) {}
	~ICDCommandPacket()
	{
		if(args != NULL)
			delete (char*)args;
	}

	int getCommandType() { return commandType; }
	void* getArgs() { return args; }
	int getArgLen() { return argLen; }

	void setCommandType(int cmdType) { commandType = cmdType; }
	void setArgs(void* a, int l)
	{
		if(a == NULL)
			throw NullPointerException();

		args = a;
		argLen = l;
	}

	virtual void* toBuffer();
protected:
	int commandType;
	void* args;
	int argLen;
};

#endif
