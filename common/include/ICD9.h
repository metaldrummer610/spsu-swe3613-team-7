/**
* \class ICD9
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _ICD_9_H
#define _ICD_9_H

#include <string>
#include <iostream>
#include "BaseCode.h"

#define ICD9_STR_LEN 4									//!< The length of an ICD9 code
#define ICD9_BUF_LEN sizeof(int) + ICD9_STR_LEN	//!< The length of the byte buffer produced

/**
* A class used to represent an ICD 9 code.
*/
class ICD9 : public BaseCode
{
public:
	ICD9();										//!< Default constructor
	ICD9(char* code);							//!< Initializes with a specific code as a char*
	ICD9(std::string code);					//!< Initializes with a specific code as an std::string

	virtual void* toBuffer();				//!< Inherited. See BaseCode::toBuffer
	virtual void fromBuffer(void* buf);	//!< Inherited. See BaseCode::fromBuffer
};

#endif
