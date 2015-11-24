#pragma once

#include "global.h"


#ifndef STRINGTOOLS_OPTIMIZE
#define STRINGTOOLS_OPTIMIZE
#endif


namespace StringTools
{
    char* nextWord(char** data);
    bool parseInt(uint32_t* out, char* str);
    bool parseIpAddr(uint32_t* ip, char* str);
}
