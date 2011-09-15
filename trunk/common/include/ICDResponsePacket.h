#ifndef _ICD_RESPONSE_PACKET_H
#define _ICD_RESPONSE_PACKET_H

#include "ICDNetwork.h"

class ICDResponsePacket : public ICDPacket
{
public:
	ICDResponsePacket() : ICDPacket(ICD_PACKET_TYPE_RESPONSE), responseType(0), data(NULL), dataLen(0) {}
	ICDResponsePacket(int respType) : ICDPacket(ICD_PACKET_TYPE_RESPONSE), responseType(respType), data(NULL), dataLen(0) {}
	ICDResponsePacket(int respType, void* d, int dl) : ICDPacket(ICD_PACKET_TYPE_RESPONSE), responseType(respType), data(d), dataLen(dl) {}
	~ICDResponsePacket()
	{
		if(data != NULL)
			delete (char*)data;
	}

	int getResponseType() { return responseType; }
	void* getData() { return data; }
	int getDataLen() { return dataLen; }

	void setResponseType(int i) { responseType = i; }
	void setData(void* d, int l)
	{
		if(d == NULL)
			throw NullPointerException();

		data = d;
		dataLen = l;
	}

	virtual void* toBuffer();
protected:
	int responseType;
	void* data;
	int dataLen;
};

#endif
