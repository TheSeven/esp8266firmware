#include "global.h"
#include "lib/commandline/commandline.h"
#include "lib/stringtools/stringtools.h"
#include "lib/printf/printf.h"
#include "sys/util.h"


void COMMANDLINE_OPTIMIZE CommandLine::Interface::handleChar(char c)
{
    if (c == 8)
    {
        if (inLen) inLen--;
    }
    else if (!inLen && (!c || c == '\r' || c == '\n' || c == ' ' || c == '\t'));
    else if (!c || c == '\r' || c == '\n')
    {
        if (inLen >= ARRAYLEN(inBuf)) printf("ERR Line is too long\r\n");
        else processLine();
    }
    else if (inLen < ARRAYLEN(inBuf)) inBuf[inLen++] = c;
}

void COMMANDLINE_OPTIMIZE CommandLine::Interface::reset()
{
    inLen = 0;
}

void COMMANDLINE_OPTIMIZE CommandLine::Interface::processLine()
{
    inBuf[inLen] = 0;
    char* data = inBuf;
    char* cmd = StringTools::nextWord(&data);
    if (cmd)
    {
        for (const Command* command = commandHandlers; true; command++)
        {
            if (!command->command)
            {
                printf("ERR Unknown command \"%s\"\r\n", cmd);
                break;
            }
            if (strcmp(cmd, command->command)) continue;
            if (!command->hasArgs && !checkNoArgsLeft(&data)) break;
            command->handler(this, data);
            break;
        }
    }
    inLen = 0;
}

int COMMANDLINE_OPTIMIZE CommandLine::Interface::printf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(outBuf, sizeof(outBuf), fmt, ap);
    va_end(ap);
    outputHandler->putString(outBuf, len);
    return len;
}

void COMMANDLINE_OPTIMIZE CommandLine::Interface::printMultiline(const char* prefix, const char* text)
{
    if (!text || !prefix) return;
    int prefixlen = strlen(prefix);
    while (*text)
    {
        outputHandler->putString(prefix, prefixlen);
        int len = 0;
        while (*(text + len) && *(text + len++) != '\n');
        outputHandler->putString(text, len);
        text += len;
    }
    outputHandler->putString("\r\n", 2);
}

bool COMMANDLINE_OPTIMIZE CommandLine::Interface::checkNoArgsLeft(char** data)
{
    int result = StringTools::nextWord(data) == NULL;
    if (!result) printf("ERR Too many arguments\r\n");
    return result;
}

void COMMANDLINE_OPTIMIZE CommandLine::Interface::helpHandler(Interface* interface, char* args)
{
    char* cmd = StringTools::nextWord(&args);
    if (cmd)
    {
        if (!interface->checkNoArgsLeft(&args)) return;
        for (const Command* command = interface->commandHandlers; true; command++)
        {
            if (!command->command)
            {
                interface->printf("ERR Unknown command \"%s\"\r\n", cmd);
                return;
            }
            if (strcmp(cmd, command->command)) continue;
            interface->printf("DBG Command help:\r\n");
            showSummary(interface, command);
            interface->printMultiline("DBG ", command->helpDetail);
            break;
        }
    }
    else
    {
        interface->printf("DBG List of commands:\r\n");
        for (const Command* command = interface->commandHandlers; command->command; command++)
            showSummary(interface, command);
    }
    interface->printf("OK\r\n");
}

void COMMANDLINE_OPTIMIZE CommandLine::Interface::showSummary(Interface* interface,
                                                              const Command* command)
{
    int len = interface->printf("DBG     %s %s", command->command,
                                command->hasArgs ? command->argList : "");
    if (len > 24)
    {
        interface->printf("\r\nDBG");
        len = 3;
    }
    interface->outputHandler->putString("                     ", 24 - len);
    interface->printf(" - %s\r\n", command->helpSummary);
}
