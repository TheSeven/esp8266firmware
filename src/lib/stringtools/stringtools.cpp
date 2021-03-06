#include "global.h"
#include "lib/stringtools/stringtools.h"


char* STRINGTOOLS_OPTIMIZE StringTools::nextWord(char** data)
{
    if (!data || !*data || !**data) return NULL;
    char c = *(*data)++;
    while (c == ' ' || c == '\t') c = *(*data)++;
    if (!c) return NULL;
    char* word = *data - 1;
    while (c && c != ' ' && c != '\t') c = *(*data)++;
    *(*data - 1) = 0;
    if (!c) (*data)--;
    return word;
}

bool STRINGTOOLS_OPTIMIZE StringTools::parseInt(uint32_t* out, char* str)
{
    *out = 0;
    bool negate = false;
    if (*str == '-')
    {
        str++;
        negate = true;
    }
    if (*str == '0' && (*++str & ~0x20) == 'X')
    {
        str++;
        while (true)
        {
            char c = *str++;
            char x = c & ~0x20;
            if (c >= '0' && c <= '9') *out = (*out << 4) | (c - '0');
            else if (x >= 'A' && x <= 'F') *out = (*out << 4) | (x - 'A' + 10);
            else if (!c) break;
            else return false;
        }
    }
    else
    {
        while (true)
        {
            char c = *str++;
            if (c >= '0' && c <= '9') *out = *out * 10 + c - '0';
            else if (!c) break;
            else return false;
        }
    }
    if (negate) *out = (uint32_t)(-((int)*out));
    return true;
}

bool STRINGTOOLS_OPTIMIZE StringTools::parseIpAddr(uint32_t* ip, char* str)
{
    *ip = 0;
    for (int i = 0; i < 4; i++)
    {
        int byte = 0;
        while (true)
        {
            char c = *str++;
            if (c >= '0' && c <= '9') byte = 10 * byte + c - '0';
            else if (c == (i < 3 ? '.' : 0)) break;
            else return false;
        }
        if (byte > 0xff) return false;
        *ip |= byte << (8 * i);
    }
    return true;
}

int STRINGTOOLS_OPTIMIZE StringTools::parseHexStr(uint8_t* out, char* str)
{
    int len = 0;
    while (true)
    {
        uint8_t byte;
        char c = *str++;
        char x = c & ~0x20;
        if (!c) break;
        else if (c >= '0' && c <= '9') byte = (c - '0') << 4;
        else if (x >= 'A' && x <= 'F') byte = (x - 'A' + 10) << 4;
        else return false;
        c = *str++;
        x = c & ~0x20;
        if (c >= '0' && c <= '9') byte |= c - '0';
        else if (x >= 'A' && x <= 'F') byte |= x - 'A' + 10;
        else return false;
        *out++ = byte;
        len++;
    }
    return len;
}
