#pragma once

#include "global.h"

#ifndef COMMANDLINE_OPTIMIZE
#define COMMANDLINE_OPTIMIZE
#endif
#ifndef COMMANDLINE_MAXLINE
#define COMMANDLINE_MAXLINE 128
#endif


namespace CommandLine
{
    
    class Interface;
    
    class __attribute__((packed,aligned(4))) Command
    {
    public:
        const char* command;
        const char* argList;
        const char* helpSummary;
        const char* helpDetail;
        bool hasArgs : 1;
        uint32_t : 31;
        void (*handler)(Interface* interface, char* args);
        
        constexpr COMMANDLINE_OPTIMIZE Command(const char* command, const char* argList,
                                               const char* helpSummary, const char* helpDetail,
                                               bool hasArgs, typeof(handler) handler)
            : command(command), argList(argList), helpSummary(helpSummary), helpDetail(helpDetail),
              hasArgs(hasArgs), handler(handler) {}
        constexpr COMMANDLINE_OPTIMIZE Command()
            : command(NULL), argList(NULL), helpSummary(NULL), helpDetail(NULL),
              hasArgs(false), handler(NULL) {}
    };
    
    
    class OutputHandler
    {
    public:
        virtual void putString(const char* str, int len) = 0;
    };


    class Interface
    {
    protected:
        const Command* commandHandlers;
        OutputHandler* outputHandler;
        uint32_t inLen;
        char inBuf[COMMANDLINE_MAXLINE];
        char outBuf[COMMANDLINE_MAXLINE];

    public:
        constexpr COMMANDLINE_OPTIMIZE Interface(const Command* commandHandlers,
                                                 OutputHandler* outputHandler)
            : commandHandlers(commandHandlers), outputHandler(outputHandler),
              inLen(0), inBuf{}, outBuf{} {}
        void handleChar(char c);
        void reset();
        int printf(const char* fmt, ...);
        void printMultiline(const char* prefix, const char* text);
        bool checkNoArgsLeft(char** data);
        static void helpHandler(Interface* interface, char* args);
        
    protected:
        static void showSummary(Interface* interface, const Command* command);
        void processLine();
    };

}
