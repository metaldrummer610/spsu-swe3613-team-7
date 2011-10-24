#ifndef _ICD_RESPONSE_PACKET_H
#define _ICD_RESPONSE_PACKET_H

//#include "ICDNetwork.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

class ICDPacket;
class ICDResponse;

class ICDResponsePacket : public ICDPacket
{
public:
	ICDResponsePacket() : ICDPacket(ICD_PACKET_TYPE_RESPONSE), response(NULL) {}
	ICDResponsePacket(ICDResponse* resp) : ICDPacket(ICD_PACKET_TYPE_RESPONSE), response(resp) {}

	ICDResponse* getResponse() { return response; }
	void setResponse(ICDResponse* i) { response = i; }
protected:
	ICDResponse* response;
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDPacket>(*this);
		ar & response;
	}
};

//BOOST_CLASS_EXPORT(ICDResponsePacket)

#endif
