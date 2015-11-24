#pragma once

#include "global.h"
#include "device/ws2811/ws2811.h"
#include "device/ws2811/esp8266/slc_register.h"


namespace WS2811
{

    class __attribute__((packed,aligned(4))) ESP8266Driver final : public Strip
    {
    private:
        static const uint16_t bitPatterns[16];
        struct sdio_queue i2sBufDescOut;
        struct sdio_queue i2sBufDescZeroes;
        uint8_t i2sZeroes[32];
        uint8_t i2sBlock[WS2811_BLOCKSIZE];
        
    public:
        ESP8266Driver();

        void sendFrame(void** data, int pixels);
        bool isSending();
        void cancelSending();

    private:
    };

}
