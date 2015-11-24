#pragma once

#include "global.h"
#include "lib/commandline/commandline.h"

extern "C"
{
#include "ip_addr.h"
#include "espconn.h"
}


#ifndef TELNET_OPTIMIZE
#define TELNET_OPTIMIZE
#endif
#ifndef TELNET_LOG_MAXLINE
#define TELNET_LOG_MAXLINE 128
#endif


namespace Telnet
{
    
    class Server : public CommandLine::Interface
    {
    private:
        const char* projectName;
        const char* version;
        char logBuf[TELNET_LOG_MAXLINE];
        uint16_t logBufPos;
        
    public:
        bool enableLogging : 1;
        uint16_t : 15;

        constexpr Server(const char* projectName, const char* version,
                         const CommandLine::Command* commandHandlers,
                         CommandLine::OutputHandler* outputHandler)
            : Interface(commandHandlers, outputHandler), projectName(projectName),
              version(version), logBuf{'L', 'O', 'G', ' '}, logBufPos(4), enableLogging(false) {}
        void newClient();
        bool handleData(void* data, int len);
        void sendLogChar(char c);
    };

}
