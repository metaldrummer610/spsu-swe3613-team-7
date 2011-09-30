#include "ICD10.h"

ICD10::ICD10()
	: BaseCode(ICD10_t)
{
}

ICD10::ICD10(char* code, int len)
	: BaseCode(ICD10_t, code, len)
{
}

ICD10::ICD10(char* code, int len, char* desc, int descSize, char* flags, int flagsSize)
	: BaseCode(ICD10_t, code, len, desc, descSize, flags, flagsSize)
{
}