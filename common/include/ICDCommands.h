#ifndef _ICD_COMMANDS_H
#define _ICD_COMMANDS_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#include <vector>

class ICDCode;

enum class CommandType
{
	NotSet = -1,
	Convert9To10 = 1,
	GetICD9Code,
	GetICD10Code,
	GetDXCode,
	CreateDXCode,
	GetDXCodes,
  DeleteDXCode
};

enum class ResponseType
{
	NotSet = -1,
	Convert9To10 = 1,
	GetICD9Code,
	GetICD10Code,
	GetDXCode,
	GetDXCodes
};

/**
* Define packet types
*/
class ICDCommand
{
public:
	ICDCommand() : commandType(CommandType::NotSet) {}
	ICDCommand(CommandType type) : commandType(type) {}

	virtual ~ICDCommand()
	{
	}

	CommandType getCommandType() { return commandType; }
protected:
	CommandType commandType;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & commandType;
	}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(ICDCommand)

class ICDResponse
{
public:
	ICDResponse() : responseType(ResponseType::NotSet) {}
	ICDResponse(ResponseType type) : responseType(type) {}

	virtual ~ICDResponse()
	{
	}

	ResponseType getResponseType() { return responseType; }
protected:
	ResponseType responseType;
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
class ICDCommandConvert9To10 : public ICDCommand
{
public:
	ICDCommandConvert9To10() : ICDCommand(CommandType::Convert9To10), code("") {}
	ICDCommandConvert9To10(std::string str) : ICDCommand(CommandType::Convert9To10), code(str) {}

	std::string getCode() { return code; }
	void setCode(std::string c) { code = c; }
private:
	std::string code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
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
class ICDResponseConvert9To10 : public ICDResponse
{
public:
	ICDResponseConvert9To10() : ICDResponse(ResponseType::Convert9To10) {}
	ICDResponseConvert9To10(std::vector<ICDCode*> v) : ICDResponse(ResponseType::Convert9To10), codes(v) {}

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

class ICDCommandGetICD9Code : public ICDCommand
{
public:
	ICDCommandGetICD9Code() : ICDCommand(CommandType::GetICD9Code), code("") {}
	ICDCommandGetICD9Code(std::string str) : ICDCommand(CommandType::GetICD9Code), code(str) {}

	std::string getCode() { return code; }
	void setCode(std::string c) { code = c; }
private:
	std::string code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDCommand>(*this);
		ar & code;
	}
};

class ICDResponseGetICD9Code : public ICDResponse
{
public:
	ICDResponseGetICD9Code() : ICDResponse(ResponseType::GetICD9Code), code(NULL) {}
	ICDResponseGetICD9Code(ICDCode* c) : ICDResponse(ResponseType::GetICD9Code), code(c) {}

	ICDCode* getCode() { return code; }
private:
	ICDCode* code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDResponse>(*this);
		ar & code;
	}
};

class ICDCommandGetICD10Code : public ICDCommand
{
public:
	ICDCommandGetICD10Code() : ICDCommand(CommandType::GetICD10Code), code("") {}
	ICDCommandGetICD10Code(std::string str) : ICDCommand(CommandType::GetICD10Code), code(str) {}

	std::string getCode() { return code; }
	void setCode(std::string c) { code = c; }
private:
	std::string code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDCommand>(*this);
		ar & code;
	}
};

class ICDResponseGetICD10Code : public ICDResponse
{
public:
	ICDResponseGetICD10Code() : ICDResponse(ResponseType::GetICD10Code), code(NULL) {}
	ICDResponseGetICD10Code(ICDCode* c) : ICDResponse(ResponseType::GetICD10Code), code(c) {}

	ICDCode* getCode() { return code; }
private:
	ICDCode* code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDResponse>(*this);
		ar & code;
	}
};

class ICDCommandGetDXCode : public ICDCommand
{
public:
	ICDCommandGetDXCode() : ICDCommand(CommandType::GetDXCode), code("") {}
	ICDCommandGetDXCode(std::string str) : ICDCommand(CommandType::GetDXCode), code(str) {}

	std::string getCode() { return code; }
	void setCode(std::string c) { code = c; }
private:
	std::string code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDCommand>(*this);
		ar & code;
	}
};

class ICDResponseGetDXCode : public ICDResponse
{
public:
	ICDResponseGetDXCode() : ICDResponse(ResponseType::GetDXCode), code(NULL) {}
	ICDResponseGetDXCode(DXCode* c) : ICDResponse(ResponseType::GetDXCode), code(c) {}

	DXCode* getCode() { return code; }
private:
	DXCode* code;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDResponse>(*this);
		ar & code;
	}
};

class ICDCommandCreateDXCode : public ICDCommand
{
public:
	ICDCommandCreateDXCode() : ICDCommand(CommandType::CreateDXCode), icd10Code(""), dxCode("") {}
	ICDCommandCreateDXCode(std::string str, std::string str2) : ICDCommand(CommandType::CreateDXCode), icd10Code(str), dxCode(str2) {}

	std::string getICD10Code() { return icd10Code; }
	void setICD10Code(std::string c) { icd10Code = c; }
	
	std::string getDXCode() { return dxCode; }
	void setDXCode(std::string c) { dxCode = c; }
private:
	std::string icd10Code;
	std::string dxCode;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDCommand>(*this);
		ar & icd10Code;
		ar & dxCode;
	}
};

class ICDCommandGetDXCodes : public ICDCommand
{
public:
	ICDCommandGetDXCodes() : ICDCommand(CommandType::GetDXCodes) {}
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDCommand>(*this);
	}
};

class ICDResponseGetDXCodes : public ICDResponse
{
public:
	ICDResponseGetDXCodes() : ICDResponse(ResponseType::GetDXCodes) {}
	ICDResponseGetDXCodes(std::vector<DXCode*> v) : ICDResponse(ResponseType::GetDXCodes), codes(v) {}

	std::vector<DXCode*> getCodes() { return codes; }
	void setCodes(std::vector<DXCode*> v) { codes = v; }
private:
	std::vector<DXCode*> codes;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDResponse>(*this);
		ar & codes;
	}
};

class ICDCommandDeleteDXCode : public ICDCommand
{
public:
	ICDCommandDeleteDXCode() : ICDCommand(CommandType::DeleteDXCode) : dxCode("") {}
  ICDCommandDeleteDXCode(std:;string code) : ICDCommand(CommandType::DeleteDXCode) : dxCode(code) {}
   
private:
  std::string dxCode;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ICDCommand>(*this);
    ar & dxCode;
	}
};

#endif
