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
	BaseCode() : type(NOT_SET), code(NULL), codeSize(0), desc(NULL), descSize(0), flags(NULL), flagsSize(0) {}												//!< Default constructor
	BaseCode(CodeType t) : code(NULL), codeSize(0), desc(NULL), descSize(0), flags(NULL), flagsSize(0) { type = t; }										//!< Initializes with a specific CodeType
	BaseCode(CodeType t, char* c, int len) : type(t), code(c), codeSize(len), desc(NULL), descSize(0), flags(NULL), flagsSize(0) {}					//!< Initializes with a specific CodeType and code as a char*
	BaseCode(CodeType t, char* c, int len, char* d, int dLen, char* f, int fLen) : 
		type(t), code(c), codeSize(len), desc(d), descSize(dLen), flags(f), flagsSize(fLen) {} 																	//!< Initializes with a specific CodeType, and code, description, and flags as char*
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
	char* getDesc() { return desc; }										//!< Returns the description
	char* getFlags() { return flags; }									//!< Returns the flags
	int getCodeSize() { return codeSize; }								//!< Returns the size of the code
	int getDescSize() { return descSize; }								//!< Returns the size of the description
	int getFlagsSize() { return flagsSize; }							//!< Returns the size of the flags

	void setType(CodeType t) { type = t; }								//!< Sets the type of code
	void setCode(char* s) { code = s; }									//!< Sets the code	
	void setDesc(char* s) { desc = s; }									//!< Sets the description
	void setFlags(char* s) { flags = s; }								//!< Sets the flags
	void setCodeSize(int i) { codeSize = i; }							//!< Sets the size of the code
	void setDescSize(int i) { descSize = i; }							//!< Sets the size of the description
	void setFlagsSize(int i) { flagsSize = i; }						//!< Sets the size of the flags

	static BaseCode* createCodeFromBuffer(void* buf);				//!< Creates a generic BaseCode object from a buffer of bytes. A factory method.
protected:
	CodeType type;																//!< The type of code
	char* code;																	//!< The code
	int codeSize;																//!< The size of the code
	char* desc;																	//!< The description
	int descSize;																//!< The size of the description
	char* flags;																//!< The flags	
	int flagsSize;																//!< The size of the flags
};

#endif
