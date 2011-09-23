/**
* \class ICD10
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _ICD_10_H
#define _ICD_10_H

#include "BaseCode.h"
#include <string>

/**
* A class used to represent an ICD10 code.
*/
class ICD10 : public BaseCode
{
public:
	ICD10();																										//!< Default constructor
	ICD10(char* code, int len);																			//!< Initializes with a specific code as a char*
	ICD10(char* code, int len, char* desc, int descSize, char* flags, int flagsSize);	//!< Initialized with a code, description, and flags	
};

#endif
