#include "ICD9.h"
#include <string.h>

ICD9::ICD9()
	: BaseCode(ICD9_t)
{
}

ICD9::ICD9(char* code)
	: BaseCode(ICD9_t, code)
{
}

ICD9::ICD9(std::string code)
	: BaseCode(ICD9_t, code)
{
}

void* ICD9::toBuffer()
{
	void* buf = new char[ICD9_BUF_LEN]; 
	char* ptr = (char*)buf;

	int tempCode = codeToInt(type);
	memcpy(ptr, &tempCode, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, code.c_str(), ICD9_STR_LEN);
	ptr += ICD9_STR_LEN;

	return buf;
}

void ICD9::fromBuffer(void* buf)
{
	if(buf == NULL)
		throw NullPointerException();

	char* ptr = (char*)buf;

	int temp = 0;
	memcpy(&temp, ptr, sizeof(int));
	ptr += sizeof(int);

	type = intToCode(temp);

	char tmpCode[ICD9_STR_LEN];
	memcpy(&tmpCode, ptr, ICD9_STR_LEN);
	ptr += ICD9_STR_LEN;
	tmpCode[ICD9_STR_LEN] = '\0';

	code.clear();
	code += tmpCode;
}
