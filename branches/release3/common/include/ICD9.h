/**
* \class ICD9
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _ICD_9_H
#define _ICD_9_H

#include "BaseCode.h"
#include <string>

/**
* A class used to represent an ICD 9 code.
*/
class ICD9 : public BaseCode
{
public:
	ICD9();										//!< Default constructor
	ICD9(char* code, int len);				//!< Initializes with a specific code as a char*
	ICD9(char* code, int len, char* desc, int descSize, char* flags, int flagsSize); //!< Initializes with a specific code, description, and flags
};

#endif
