#ifndef _ICD_PACKET_H
#define _ICD_PACKET_H

class ICDPacket
{
public:
	ICDPacket() : type(0) {}
	ICDPacket(int t) : type(t) {}

	virtual void* toBuffer() = 0;
	static ICDPacket* createPacketFromBuffer(void* buf);

	int getType() { return type; }
	void setType(int i) { type = i; }
protected:
	int type;
};

#endif
