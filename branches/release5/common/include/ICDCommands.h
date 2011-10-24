#ifndef _ICD_COMMANDS_H
#define _ICD_COMMANDS_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#include <vector>

class ICDCode;

/*
* structure of packets
* (int)(int)(data) <- 1st int tells us the total size of the packet, meaning everything after the size field. 2nd int tells us what type of packet this is
* 	if type == command
*		(int)(args...) <- int is the command type (see below). args... are a variable list of arguments that are determined by the command type
* 	else if type == response
* 		(int)(data) <- int is the response type. data is determined by the response type
*/

/**
* Define packet types
*/
#define ICD_PACKET_TYPE_COMMAND 	1
#define ICD_PACKET_TYPE_RESPONSE	2

class ICDCommand
{
public:
	ICDCommand() : commandType(-1) {}
	ICDCommand(int type) : commandType(type) {}

	virtual ~ICDCommand()
	{
	}

	int getCommandType() { return commandType; }
protected:
	int commandType;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		std::cout << "Calling ICDCommand serialize" << std::endl;
		ar & commandType;
	}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICDCommand)

class ICDResponse
{
public:
	ICDResponse() : responseType(-1) {}
	ICDResponse(int type) : responseType(type) {}

	virtual ~ICDResponse()
	{
	}

	int getResponseType() { return responseType; }
protected:
	int responseType;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & responseType;
	}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICDResponse)

/**
* Command that is sent when the client wants to convert from ICD9 to ICD10
* Example payload:
*		(size)(ICD_PACKET_TYPE_COMMAND)(ICD_COMMAND_COVERT_9_TO_10)(int)(code goes here) <- int before code is the length of the code
*
* ICD_RESPONSE_CONVERT_9_TO_10 is the response to this message
* @see ICD_RESPONSE_CONVERT_9_TO_10
*/
#define ICD_COMMAND_CONVERT_9_TO_10 1
class ICDCommandConvert9To10 : public ICDCommand
{
public:
	ICDCommandConvert9To10() : ICDCommand(ICD_COMMAND_CONVERT_9_TO_10), code("") {}
	ICDCommandConvert9To10(std::string str) : ICDCommand(ICD_COMMAND_CONVERT_9_TO_10), code(str) {}

	std::string getCode() { return code; }
	void setCode(std::string c) { code = c; }
private:
	std::string code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		std::cout << "Calling ICDCommandConvert9To10 serialize" << std::endl;
		ar & boost::serialization::base_object<ICDCommand>(*this);
		ar & code;
	}
};

/**
* Response to the convert 9 to 10 command.
* Example payload:
*		(size)(ICD_PACKET_TYPE_RESPONSE)(ICD_RESPONSE_CONVERT_9_TO_10)(list of codes)
*
* See the CodeList file for functions to convert between code lists and byte buffers
* @see CodeList
*/
#define ICD_RESPONSE_CONVERT_9_TO_10 1

class ICDResponseConvert9To10 : public ICDResponse
{
public:
	ICDResponseConvert9To10() : ICDResponse(ICD_RESPONSE_CONVERT_9_TO_10) {}
	ICDResponseConvert9To10(std::vector<ICDCode*> v) : ICDResponse(ICD_RESPONSE_CONVERT_9_TO_10), codes(v) {}

	std::vector<ICDCode*> getCodes() { return codes; }
	void setCodes(std::vector<ICDCode*> v) { codes = v; }
private:
	std::vector<ICDCode*> codes;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDResponse>(*this);
		ar & codes;
	}
};

//BOOST_CLASS_EXPORT(ICDCommand)
//BOOST_CLASS_EXPORT(ICDResponse)
//BOOST_CLASS_EXPORT(ICDCommandConvert9To10)
//BOOST_CLASS_EXPORT(ICDResponseConvert9To10)

#endif
