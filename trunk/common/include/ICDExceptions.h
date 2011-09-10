/**
* \file Custom exceptions that are used in the networking library
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _ICD_EXCEPTIONS_H
#define _ICD_EXCEPTIONS_H

#include <exception>

/**
* \class NullPointerException 
* An exception used when a NULL pointer is passed into a function.
*/
class NullPointerException : public std::exception
{
public:
	/**
	* Overridden function from the std::exception class. Gives you details about the exception
	*/
	virtual const char* what() const throw()
	{
		return "NULL pointer passed to us!";
	}
};

#endif
