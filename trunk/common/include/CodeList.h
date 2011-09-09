#ifndef _CODE_LIST_H
#define _CODE_LIST_H

#include <vector>
#include "BaseCode.h"

void* codeListToBuffer(std::vector<BaseCode*> list);
std::vector<BaseCode*> bufferToCodeList(void* buf);

#endif
