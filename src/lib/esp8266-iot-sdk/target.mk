NAME := firmware
SDK ?= ../esp8266-iot-sdk
LDSCRIPT := src/lib/esp8266-iot-sdk/link.lds
$(TARGET): build/$(TARGET)/$(TYPE)/0x00000.bin
LISTINGS: build/$(TARGET)/$(TYPE)/$(NAME).elf.lst
CFLAGS_GENERAL += -I$(SDK)/include -I$(SDK)/examples/driver_lib/include -Isrc/lib/esp8266-iot-sdk
LDFLAGS_GENERAL += -L$(SDK)/lib -L../tools/xtensa-lx106-elf/lib -L../tools/lib/gcc/xtensa-lx106-elf/4.8.2
LIBS += -lmain -llwip -lnet80211 -lwpa -lpp -lphy -lhal -lc -lgcc
