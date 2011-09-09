#ifndef _ICD_EXCEPTIONS_H
#define _ICD_EXCEPTIONS_H

#include <exception>

class NullBufferException : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "NULL buffer passed to us!";
	}
};

#endif
