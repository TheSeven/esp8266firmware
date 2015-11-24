#pragma once

#define ESP8266FLASH_OPTIMIZE __attribute__((section(".irom0.text")))
#define STORAGEPARTITION_OPTIMIZE __attribute__((section(".irom0.text")))
#define CONFIGSTORE_OPTIMIZE __attribute__((section(".irom0.text")))
#define CRC32_OPTIMIZE __attribute__((section(".irom0.text")))
#define PRINTF_OPTIMIZE __attribute__((section(".irom0.text")))
#define STRINGTOOLS_OPTIMIZE __attribute__((section(".irom0.text")))
#define COMMANDLINE_OPTIMIZE __attribute__((section(".irom0.text")))
#define TELNET_OPTIMIZE __attribute__((section(".irom0.text")))
