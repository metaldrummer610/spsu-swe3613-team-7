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

#include "ICDExceptions.h"
#include <string>

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
int codeToInt(CodeType t);

/**
* Converts an int to a CodeType so that it can be assigned to a BaseCode object
*/
CodeType intToCode(int i);

#define BASE_CODE_BUFFER_SIZE(x) sizeof(int) + sizeof(int) + x

/**
* This is the base class for all ICD9 and ICD10 codes. It provides functions for converting to and from void*'s for network communication.
*/
class BaseCode
{
public:
	BaseCode() : type(NOT_SET), code(NULL), codeSize(0) {}								//!< Default constructor
	BaseCode(CodeType t) : code(NULL), codeSize(0) { type = t; }						//!< Initializes with a specific CodeType
	BaseCode(CodeType t, char* c, int len) : type(t), code(c), codeSize(len) {}	//!< Initializes with a specific CodeType and code as a char*

	/**
	* Converts a code to a buffer of bytes. The format of that buffer is:
	*
	* (int)(int)(string)
	*	int 1 <- type of code
	*	int 2 <- size of code
	*	string <- actual code
	*/
	virtual void* toBuffer();
	virtual void fromBuffer(void* buf);									//!< Converts a buffer of bytes to a code

	CodeType getType() { return type; }									//!< Returns the type of code
	char* getCode() { return code; }										//!< Returns the code
	int getCodeSize() { return codeSize; }								//!< Returns the size of the code

	void setType(CodeType t) { type = t; }								//!< Sets the type of code
	void setCode(char* s) { code = s; }									//!< Sets the code
	void setCodeSize(int i) { codeSize = i; }							//!< Sets the size of the code

	static BaseCode* createCodeFromBuffer(void* buf);				//!< Creates a generic BaseCode object from a buffer of bytes. A factory method.
protected:
	CodeType type;																//!< The type of code
	char* code;																	//!< The code
	int codeSize;																//!< The size of the code
};

#endif
