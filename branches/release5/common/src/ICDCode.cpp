#include "ICDCode.h"
#include <string>
#include <sstream>
#include <iostream>

std::ostream& operator<<(std::ostream& out, ICDCode* code)
{
	std::string codeType;

	if(code->getType() == CodeType::ICD9)
		codeType = "ICD 9";
	else if(code->getType() == CodeType::ICD10)
		codeType = "ICD 10";
	else
		codeType = "Unknown!";

	std::stringstream ss;
	ss << codeType 
		<< "[code: " << code->getCode() 
		<< ", desc: " << code->getDesc() 
		<< ", flags: " << code->getFlags() 
		<< "]";

	out << ss.str();

	return out;
}
