/**
* \class ICD10
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _ICD_10_H
#define _ICD_10_H

#include <string>
#include <iostream>
#include "BaseCode.h"

#define ICD10_STR_LEN 7										//!< The length of an ICD10 code
#define ICD10_BUF_LEN sizeof(int) + ICD10_STR_LEN	//!< The length of the byte buffer produced

/**
* A class used to represent an ICD10 code.
*/
class ICD10 : public BaseCode
{
public:
	ICD10();											//!< Default constructor
	ICD10(char* code);							//!< Initializes with a specific code as a char*
	ICD10(std::string code);					//!< Initializes with a specific code as an std::string

	virtual void* toBuffer();					//!< Inherited. See BaseCode::toBuffer
	virtual void fromBuffer(void* buf);		//!< Inherited. See BaseCode::fromBuffer
};

#endif
