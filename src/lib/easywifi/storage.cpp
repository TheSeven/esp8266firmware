#include "global.h"
#include "lib/easywifi/easywifi.h"
#include "lib/esp8266-iot-sdk/flash.h"
#include "lib/configstore/configstore.h"
#include "sys/util.h"


static const SysConfig defaultSysConfig =
{
    version: CONFIG_VERSION,
    enableSoftAP: true,
    enableStation: true,
    enableDHCP: true,
	ipaddr: 0,
	netmask: 0,
	gateway: 0,
};

static StoragePartition configPart(&ESP8266::Flash,
                                   (ESP8266::Flash.pageCount - ESP8266_FLASH_DATA_SIZE) / ESP8266::Flash.eraseSize,
                                   (ESP8266_FLASH_DATA_SIZE - 0x4000) / ESP8266::Flash.eraseSize);
static ConfigData<EasyConfig, 256> config;
SysConfig* const sysCfg = &config.data.sysConfig;
AppConfig* const cfg = &config.data.appConfig;
static ConfigStore configStore(&config);
bool configChanged = false;


bool EasyWifi::initStorage()
{
    bool ok = true;
    Storage::Result result;
    result = configStore.init(&configPart);
    if (result != Storage::RESULT_OK) return false;
    configChanged = false;
    if (sysCfg->version != CONFIG_VERSION)
    {
        ok = false;
        configChanged = true;
        memcpy(sysCfg, &defaultSysConfig, sizeof(*sysCfg));
        memcpy(cfg, &defaultAppConfig, sizeof(*cfg));
        if (sizeof(config.padding)) memset(config.padding, 0, sizeof(config.padding));
    }
    return ok;
}

void saveConfig()
{
    Storage::Result result = configStore.save();
    if (result != Storage::RESULT_OK) return;
    configChanged = false;
}
