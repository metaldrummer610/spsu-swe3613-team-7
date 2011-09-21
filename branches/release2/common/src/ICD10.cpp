#include "ICD10.h"

ICD10::ICD10()
	: BaseCode(ICD10_t)
{
}

ICD10::ICD10(char* code, int len)
	: BaseCode(ICD10_t, code, len)
{
}
