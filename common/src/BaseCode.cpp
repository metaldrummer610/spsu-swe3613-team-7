#include "BaseCode.h"
#include "ICD9.h"
#include "ICD10.h"

static BaseCode* BaseCode::createCodeFromBuffer(void* buf)
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
	}

	return NULL;
}
