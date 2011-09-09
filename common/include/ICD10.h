#ifndef _ICD_10_H
#define _ICD_10_H

#include <string>
#include <iostream>
#include "BaseCode.h"

#define ICD10_STR_LEN 7
#define ICD10_BUF_LEN sizeof(int) + ICD10_STR_LEN

class ICD10 : public BaseCode
{
public:
	ICD10();
	ICD10(char* code);
	ICD10(std::string code);

	virtual void* toBuffer();
	virtual void fromBuffer(void* buf);
};

#endif
