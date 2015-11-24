#include "global.h"
#include "lib/easywifi/easywifi.h"
#include "lib/stringtools/stringtools.h"
#include "device/ws2811/esp8266/ws2811_esp8266.h"


const AppConfig defaultAppConfig =
{
};

static WS2811::ESP8266Driver ledStrip;
static uint8_t patternData[333][3];
static struct espconn ledConn;
static esp_udp ledUDP;

static void ICACHE_FLASH_ATTR ledRecv(void* arg, char* buf, unsigned short len)
{
    void* ptr = (void*)buf;
    ledStrip.sendFrame(&ptr, len / 3);
}

static void ICACHE_FLASH_ATTR sendPattern()
{
    void* ptr = *patternData;
    ledStrip.sendFrame(&ptr, ARRAYLEN(patternData));
}

static void ICACHE_FLASH_ATTR cmd_pattern_handler(CommandLine::Interface* interface, char* args)
{
    char* brightness = StringTools::nextWord(&args);
    char* pattern = StringTools::nextWord(&args);
    if (!interface->checkNoArgsLeft(&args)) return;
    if (!brightness || !pattern)
    {
        interface->printf("ERR Not enough arguments\r\n");
        return;
    }
    uint32_t b;
    if (!StringTools::parseInt(&b, brightness) || b > 255)
    {
        interface->printf("ERR Invalid brightness value\r\n");
        return;
    }
    char* pptr = pattern;
    uint8_t* optr = *patternData;
    while (optr < *patternData + sizeof(patternData))
    {
        uint32_t data = 0;
        if (!*pptr) pptr = pattern;
        char c = *pptr++;
        if (c == 'r') data = 0x000100;
        else if (c == 'g') data = 0x000001;
        else if (c == 'b') data = 0x010000;
        else if (c == 'c') data = 0x010001;
        else if (c == 'm') data = 0x010100;
        else if (c == 'y') data = 0x000101;
        else if (c == 'w') data = 0x010101;
        data *= b;
        *optr++ = data;
        *optr++ = data >> 8;
        *optr++ = data >> 16;
    }
    sendPattern();
    interface->printf("OK\r\n");
}

const CommandLine::Command telnetCommands[] =
{
    BASE_TELNET_COMMANDS
    { "pattern", "<brightness> <colors>", "Sends a test pattern to the LED strip",
                 "<brightness> is a value from 0 to 255\r\n<colors> is a sequence of 0,r,g,b,c,m,y,w",
                 true, cmd_pattern_handler },
    { /* End of list */ }
};

void ICACHE_FLASH_ATTR appInit()
{
    memset(patternData, 0, sizeof(patternData));
    sendPattern();

    ledConn.type = ESPCONN_UDP;
    ledConn.state = ESPCONN_NONE;
    ledConn.proto.udp = &ledUDP;
    ledUDP.local_port = LED_UDP_PORT;
    ledUDP.remote_port = LED_UDP_PORT;
    memset(ledUDP.remote_ip, 0xff, 4);
    espconn_regist_recvcb(&ledConn, ledRecv);
    espconn_create(&ledConn); 
}
