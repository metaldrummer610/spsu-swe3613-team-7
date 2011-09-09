#ifndef _ICD_9_H
#define _ICD_9_H

#include <string>
#include <iostream>
#include "BaseCode.h"

#define ICD9_STR_LEN 4
#define ICD9_BUF_LEN sizeof(int) + ICD9_STR_LEN

class ICD9 : public BaseCode
{
public:
	ICD9();
	ICD9(char* code);
	ICD9(std::string code);

	virtual void* toBuffer();
	virtual void fromBuffer(void* buf);
};

#endif
