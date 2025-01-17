#ifndef _ICD_COMMAND_PACKET_H
#define _ICD_COMMAND_PACKET_H

//#include "ICDPacket.h"
//#include "ICDCommands.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

class ICDPacket;
class ICDCommand;

class ICDCommandPacket : public ICDPacket
{
public:
	ICDCommandPacket() : ICDPacket(PacketType::Command), command(NULL) {}
	ICDCommandPacket(ICDCommand* cmd) : ICDPacket(PacketType::Command), command(cmd) {}

	ICDCommand* getCommand() { return command; }
	void setCommand(ICDCommand* cmd) { command = cmd; }
protected:
	ICDCommand* command;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDPacket>(*this);
		ar & command;
	}
};

//BOOST_CLASS_EXPORT(ICDCommandPacket)

#endif
