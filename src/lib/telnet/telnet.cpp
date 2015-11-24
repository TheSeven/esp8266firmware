#include "global.h"
#include "lib/telnet/telnet.h"
#include "sys/util.h"


void TELNET_OPTIMIZE Telnet::Server::newClient()
{
    enableLogging = false;
    reset();
    printf("DBG %s v%s\r\nVERSION %s\r\n", projectName, version, version);
}

bool TELNET_OPTIMIZE Telnet::Server::handleData(void* data, int len)
{
    char* msg = (char*)data;
    if (len && *msg == 0xff)
    {
        if (len < 2) return false;
        if (msg[1] == 0xec)
        {
            printf("DBG Bye!\r\n");
            return true;
        }
        else if (msg[1] == 0xf4) printf("\r\n");
        else printf("DBG Ignoring telnet control packet\r\n");
        return false;
    }
    while (len--) handleChar(*msg++);
    return false;
}

void Telnet::Server::sendLogChar(char c)
{
    if (c == '\n' || logBufPos >= ARRAYLEN(logBuf) - 2)
    {
        logBuf[logBufPos] = '\r';
        logBuf[logBufPos + 1] = '\n';
        if (enableLogging) outputHandler->putString(logBuf, logBufPos + 2);
        logBufPos = 4;
    }
    if (c != '\n') logBuf[logBufPos++] = c;
}
