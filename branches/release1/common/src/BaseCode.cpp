#include "BaseCode.h"
#include "ICD9.h"
#include "ICD10.h"
#include <string.h>

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

BaseCode* BaseCode::createCodeFromBuffer(void* buf)
{
	if(buf == NULL)
		throw NullPointerException();

	int tmpCode = 0;
	memcpy(&tmpCode, buf, sizeof(int));

	CodeType type = intToCode(tmpCode);
	
	switch(type)
	{
		case ICD9_t:
		{
			ICD9* code = new ICD9();
			code->fromBuffer(buf);
			return code;
		}
		case ICD10_t:
		{
			ICD10* code = new ICD10();
			code->fromBuffer(buf);
			return code;
		}
		case NOT_SET:
			return NULL;
	}

	return NULL;
}
