#include "global.h"

extern "C"
{
    #include "c_types.h"
    #include "spi_flash.h"
}

#include "lib/esp8266-iot-sdk/flash.h"
#include "sys/util.h"


namespace ESP8266
{
    enum Storage::Result ESP8266FLASH_OPTIMIZE FlashDriver::reset()
    {
        return RESULT_OK;
    }

    enum Storage::Result ESP8266FLASH_OPTIMIZE FlashDriver::getStatus()
    {
        return RESULT_OK;
    }

    enum Storage::Result ESP8266FLASH_OPTIMIZE FlashDriver::read(uint32_t page, uint32_t len, void* buf)
    {
        if (page >= pageCount || len > pageCount - page) return RESULT_INVALID_ARGUMENT;
        if (!len) return RESULT_OK;
        if (spi_flash_read(page, (uint32*)buf, len) != SPI_FLASH_RESULT_OK) return RESULT_UNKNOWN_ERROR;
        return RESULT_OK;
    }

    enum Storage::Result ESP8266FLASH_OPTIMIZE FlashDriver::write(uint32_t page, uint32_t len, const void* buf)
    {
        if (page >= pageCount || len > pageCount - page || page & (programSize - 1) || len & (programSize - 1))
            return RESULT_INVALID_ARGUMENT;
        if (spi_flash_write(page, (uint32*)buf, len) != SPI_FLASH_RESULT_OK) return RESULT_UNKNOWN_ERROR;
        return RESULT_OK;
    }

    enum Storage::Result ESP8266FLASH_OPTIMIZE FlashDriver::erase(uint32_t page, uint32_t len)
    {
        if (page >= pageCount || len > pageCount - page || page & (eraseSize - 1) || len & (eraseSize - 1))
            return RESULT_INVALID_ARGUMENT;
        if (spi_flash_erase_sector(page / eraseSize) != SPI_FLASH_RESULT_OK) return RESULT_UNKNOWN_ERROR;
        return RESULT_OK;
    }

    FlashDriver Flash{};
}
