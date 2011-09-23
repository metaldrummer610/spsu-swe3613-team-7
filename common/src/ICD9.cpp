#include "ICD9.h"

ICD9::ICD9()
	: BaseCode(ICD9_t)
{
}

ICD9::ICD9(char* code, int len)
	: BaseCode(ICD9_t, code, len)
{
}

ICD9::ICD9(char* code, int len, char* desc, int descSize, char* flags, int flagsSize)
	: BaseCode(ICD9_t, code, len, desc, descSize, flags, flagsSize)
{
}
