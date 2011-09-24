#include "BaseCode.h"
#include "ICD9.h"
#include "ICD10.h"
#include <string.h>
#include <string>
#include <iostream>

int codeToInt(CodeType t)
{
	switch(t)
	{
		case ICD9_t:
			return 1;
		case ICD10_t:
			return 2;
		case NOT_SET:
			return -1;
		default:
			return 0;
	}
}

CodeType intToCode(int i)
{
	switch(i)
	{
		case 1:
			return ICD9_t;
		case 2:
			return ICD10_t;
		default:
			return NOT_SET;
	}
}

int BaseCode::getSizeInBytes()
{
	return sizeof(int) + sizeof(int) + codeSize + sizeof(int) + descSize + sizeof(int) + flagsSize;
}

void* BaseCode::toBuffer()
{
	int codeType = codeToInt(type);
	void* ret = new char[this->getSizeInBytes()];
	char* ptr = (char*)ret;

	memcpy(ptr, &codeType, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, &codeSize, sizeof(int));
	ptr += sizeof(int);

	strncpy(ptr, code, codeSize);
	ptr += codeSize;

	memcpy(ptr, &descSize, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, desc, descSize);
	ptr += descSize;

	memcpy(ptr, &flagsSize, sizeof(int));
	ptr += sizeof(int);

	memcpy(ptr, flags, flagsSize);
	return ret;
}

void BaseCode::fromBuffer(void* buf)
{
	if(buf == NULL)
		throw NullPointerException();

	char* ptr = (char*)buf;
	
	int codeTypeInt = 0;
	memcpy(&codeTypeInt, ptr, sizeof(int));
	ptr += sizeof(int);
	type = intToCode(codeTypeInt);
	std::cout << "Parsed type: " << type << std::endl;

	memcpy(&codeSize, ptr, sizeof(int));
	ptr += sizeof(int);
	std::cout << "Parsed code size: " << codeSize << std::endl;

	code = new char[codeSize];
	memset(code, 0, codeSize);
	strncpy(code, ptr, codeSize);
	ptr += codeSize;
	std::cout << "Parsed code: " << code << "." << std::endl;

	memcpy(&descSize, ptr, sizeof(int));
	ptr += sizeof(int);
	std::cout << "Parsed descSize: " << descSize << std::endl;

	desc = new char[descSize];
	memset(desc, 0, descSize);
	strncpy(desc, ptr, descSize);
	ptr += descSize;
	std::cout << "Parsed desc: " << desc << "." << std::endl;

	memcpy(&flagsSize, ptr, sizeof(int));
	ptr += sizeof(int);
	std::cout << "Parsed flagsSize: " << flagsSize << std::endl;

	flags = new char[flagsSize];
	memset(flags, 0, flagsSize);
	strncpy(flags, ptr, flagsSize);
	std::cout << "Parsed flags: " << flags << "." << std::endl;
}

std::ostream& operator<<(std::ostream& out, BaseCode* code)
{
	char* buffer = new char[code->getCodeSize() + code->getDescSize() + code->getFlagsSize() + 30];

	sprintf(buffer, "%s Code: %s. Desc: %s. Flags: %s.", code->getType() == ICD9_t ? "ICD9:" : "ICD10:", code->getCode(), code->getDesc(), code->getFlags());

	std::string tmp;
	tmp.append(buffer);
	out << tmp;

	delete buffer;
	return out;
}
