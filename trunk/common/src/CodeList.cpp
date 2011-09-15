#include "CodeList.h"
#include "ICD9.h"
#include "ICD10.h"
#include <string.h>

void* codeListToBuffer(std::vector<BaseCode*> list)
{
	int bufferLen = 0;
	for(std::vector<BaseCode*>::iterator it = list.begin(); it != list.end(); it++)
	{
		bufferLen += BASE_CODE_BUFFER_SIZE((*it)->getCodeSize());
	}

	void* buf = new char[bufferLen + sizeof(int)];
	char* ptr = (char*)buf;

	int numElements = list.size();
	memcpy(ptr, &numElements, sizeof(int));
	ptr += sizeof(int);

	for(std::vector<BaseCode*>::iterator it = list.begin(); it != list.end(); it++)
	{
		BaseCode* code = (BaseCode*)(*it);
		
		void* codeBuf = code->toBuffer();
		int len = BASE_CODE_BUFFER_SIZE(code->getCodeSize());

		memcpy(ptr, codeBuf, len);
		ptr += len;
		delete (char*)codeBuf;
	}

	return buf;
}

std::vector<BaseCode*> bufferToCodeList(void* buf)
{
	if(buf == NULL)
		throw NullPointerException();

	std::vector<BaseCode*> ret;
	char* ptr = (char*)buf;

	int numCodes = 0;
	memcpy(&numCodes, ptr, sizeof(int));

	for(int i = 0; i < numCodes; i++)
	{
		BaseCode* code = new BaseCode();
		code->fromBuffer(ptr);

		int len = BASE_CODE_BUFFER_SIZE(code->getCodeSize());
		ptr += len;

		ret.push_back(code);
	}

	return ret;
}
