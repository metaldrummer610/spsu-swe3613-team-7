/**
* \class BaseCode
* \brief A base class used for ICD codes.
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _BASE_CODE_H
#define _BASE_CODE_H

#include <string>
#include "ICDExceptions.h"

/**
* An enumeration that describes what type of code this object is
*/
typedef enum _code_types
{
	ICD9_t, /**< enum value for ICD9 */
	ICD10_t, /**< enum value for ICD10 */
	NOT_SET /**< enum value for a code whose value is not ICD9 or ICD10 */
} CodeType;

/**
* Converts a CodeType to an int so that it can be sent over the network
*/
int codeToInt(CodeType t)
{
	switch(t)
	{
		case ICD9_t:
			return 1;
		case ICD10_t:
			return 2;
		case NOT_SET:
			return -1;
		default:
			return 0;
	}
}

/**
* Converts an int to a CodeType so that it can be assigned to a BaseCode object
*/
CodeType intToCode(int i)
{
	switch(i)
	{
		case 1:
			return ICD9_t;
		case 2:
			return ICD10_t;
		default:
			return NOT_SET;
	}
}

/**
* This is the base class for all ICD9 and ICD10 codes. It provides functions for converting to and from void*'s for network communication.
*/
class BaseCode
{
public:
	BaseCode() : type(NOT_SET), code("") {}							//!< Default constructor
	BaseCode(CodeType t) : code("") { type = t; }					//!< Initializes with a specific CodeType
	BaseCode(CodeType t, char* c) : type(t), code(c) {}			//!< Initializes with a specific CodeType and code as a char*
	BaseCode(CodeType t, std::string c) : type(t), code(c) {}	//!< Initializes with a specific CodeType and code as a std::string

	virtual void* toBuffer() = 0;											//!< Converts this code to a buffer of bytes
	virtual void fromBuffer(void* buf) = 0;							//!< Converts a buffer of bytes to a code

	CodeType getType() { return type; }									//!< Returns the type of code
	std::string getCode() { return code; }								//!< Returns the code

	void setType(CodeType t) { type = t; }								//!< Sets the type of code
	void setCode(std::string s) { code = s; }							//!< Sets the code

	static BaseCode* createCodeFromBuffer(void* buf);				//!< Creates a generic BaseCode object from a buffer of bytes. A factory method.
protected:
	CodeType type;																//!< The type of code
	std::string code;															//!< The code
};

#endif
