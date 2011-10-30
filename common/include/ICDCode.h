/**
* \class ICDCode
* \brief A base class used for ICD codes.
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _BASE_CODE_H
#define _BASE_CODE_H

#include <string>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/**
* An enumeration that describes what type of code this object is
*/
enum class CodeType
{	
	NOT_SET = 0, 	/**< enum value for a code whose value is not ICD9 or ICD10 */
	ICD9 = 1,		/**< enum value for ICD9 */
	ICD10, 			/**< enum value for ICD10 */
	DX 				/**< enum value for a DX code*/
};

/**
* This is the base class for all ICD9 and ICD10 codes. It provides functions for converting to and from void*'s for network communication.
*/
class ICDCode
{
public:
	ICDCode() : type(CodeType::NOT_SET), code(""), desc(""), flags("") {}													//!< Default constructor
	ICDCode(CodeType t) : type(t), code(""), desc(""), flags("") {}															//!< Initializes with a specific CodeType
	ICDCode(CodeType t, std::string c) : type(t), code(c), desc(""), flags("") {}										//!< Initializes with a specific CodeType and code
	ICDCode(CodeType t, std::string c, std::string d, std::string f) : type(t), code(c), desc(d), flags(f) {}	//!< Initializes with a specific CodeType, code, description, and flags

	CodeType getType() { return type; }									//!< Returns the type of code
	std::string getCode() { return code; }								//!< Returns the code
	std::string getDesc() { return desc; }								//!< Returns the description
	std::string getFlags() { return flags; }							//!< Returns the flags

	void setType(CodeType t) { type = t; }								//!< Sets the type of code
	void setCode(std::string s) { code = s; }							//!< Sets the code	
	void setDesc(std::string s) { desc = s; }							//!< Sets the description
	void setFlags(std::string s) { flags = s; }						//!< Sets the flags
protected:
	CodeType type;																//!< The type of code
	std::string code;															//!< The code
	std::string desc;															//!< The description
	std::string flags;														//!< The flags

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & type;
		ar & code;
		ar & desc;
		ar & flags;
	}
};

std::ostream& operator<<(std::ostream& out, ICDCode* code);

#endif
