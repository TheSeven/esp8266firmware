#pragma once

#include "global.h"
#include "interface/storage/storage.h"


#ifndef ESP8266FLASH_OPTIMIZE
#define ESP8266FLASH_OPTIMIZE
#endif

namespace ESP8266
{
    class __attribute__((packed,aligned(4))) FlashDriver final : public Storage
    {
    public:
        constexpr FlashDriver() : Storage(ESP8266_FLASH_SIZE, 1, 0x1000, 1) {}
        virtual enum Result reset();
        virtual enum Result getStatus();
        virtual enum Result read(uint32_t page, uint32_t len, void* buf);
        virtual enum Result write(uint32_t page, uint32_t len, const void* buf);
        virtual enum Result erase(uint32_t page, uint32_t len);
    };

    extern FlashDriver Flash;
}
