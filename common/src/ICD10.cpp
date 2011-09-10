#include "ICD10.h"
#include <string.h>

ICD10::ICD10()
	: BaseCode(ICD10_t)
{
}

ICD10::ICD10(char* code)
	: BaseCode(ICD10_t, code)
{
}

ICD10::ICD10(std::string code)
	: BaseCode(ICD10_t, code)
{
}

void* ICD10::toBuffer()
{
	void* buf = new char[ICD10_BUF_LEN]; 
	char* ptr = (char*)buf;

	int tempCode = codeToInt(type);
	memcpy(ptr, &tempCode, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, code.c_str(), ICD10_STR_LEN);
	ptr += ICD10_STR_LEN;

	return buf;
}

void ICD10::fromBuffer(void* buf)
{
	if(buf == NULL)
		throw NullPointerException();

	char* ptr = (char*)buf;

	int temp = 0;
	memcpy(&temp, ptr, sizeof(int));
	ptr += sizeof(int);

	type = intToCode(temp);

	char tmpCode[ICD10_STR_LEN];
	memcpy(&tmpCode, ptr, ICD10_STR_LEN);
	ptr += ICD10_STR_LEN;

	code.clear();
	code.append((const char*)tmpCode);
}

