/**
* \brief A set of functions used to convert std::vectors to and from byte buffers
* 
* \author Robbie Diaz
* \version 1.0
* \date 2011-09-09
*/

#ifndef _CODE_LIST_H
#define _CODE_LIST_H

#include <vector>
#include "BaseCode.h"

/**
* Converts a std::vector of BaseCode*'s to a buffer of bytes.
* @param list A vector of BaseCode*'s
* @return A buffer of bytes containing the converted list's members
*/
void* codeListToBuffer(std::vector<BaseCode*> list);

/**
* Converts a buffer of bytes into a std::vector of BaseCode*'s.
* @param buf A buffer of bytes
* @return A std::vector of populated BaseCode*'s
*/
std::vector<BaseCode*> bufferToCodeList(void* buf);

#endif
