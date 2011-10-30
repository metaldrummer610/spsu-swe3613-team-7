#ifndef _ICD_PACKET_H
#define _ICD_PACKET_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

enum class PacketType
{
	NotSet = -1,
	Command = 1,
	Response
};

class ICDPacket
{
public:
	ICDPacket() : type(PacketType::NotSet) {}
	ICDPacket(PacketType t) : type(t) {}

	virtual ~ICDPacket()
	{
	}

	PacketType getType() { return type; }
	void setType(PacketType i) { type = i; }
protected:
	PacketType type;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & type;
	}
};

#endif
