#ifndef _ICD_PACKET_H
#define _ICD_PACKET_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

class ICDPacket
{
public:
	ICDPacket() : type(0) {}
	ICDPacket(int t) : type(t) {}

	virtual ~ICDPacket()
	{
	}

	int getType() { return type; }
	void setType(int i) { type = i; }
protected:
	int type;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		std::cout << "Calling ICDPacket serialize" << std::endl;
		ar & type;
	}
};

//BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICDPacket)
//BOOST_CLASS_EXPORT(ICDPacket)
#endif
