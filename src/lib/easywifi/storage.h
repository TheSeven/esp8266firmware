#pragma once

#include "global.h"
#include "lib/configstore/configstore.h"
#include "sys/util.h"
#include STRINGIFY(target/TARGET/appconfig.h)


struct __attribute__((packed,aligned(4))) SysConfig
{
    uint8_t version;
    bool enableSoftAP : 1;
    bool enableStation : 1;
    bool enableDHCP : 1;
    uint32_t : 13;
    int8_t timezone;
    uint32_t ipaddr;
    uint32_t netmask;
    uint32_t gateway;
    uint32_t dns1;
    uint32_t dns2;
    uint32_t ntp1;
    uint32_t ntp2;
    uint32_t ntp3;
    uint32_t : 32;
    uint32_t : 32;
    uint32_t : 32;
    uint32_t : 32;
    uint32_t : 32;
};

struct __attribute__((packed,aligned(4))) EasyConfig
{
    SysConfig sysConfig;
    AppConfig appConfig;
};

extern SysConfig* const sysCfg;
extern AppConfig* const cfg;
extern bool configChanged;
extern const AppConfig defaultAppConfig;

extern void saveConfig();

namespace EasyWifi
{
    extern bool initStorage();
}
