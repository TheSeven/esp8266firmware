#include "global.h"
#include "lib/easywifi/easywifi.h"
#include "lib/esp8266-iot-sdk/flash.h"
#include "lib/telnet/telnet.h"
#include "lib/stringtools/stringtools.h"
#include "sys/util.h"


static espconn telnetListener;
static espconn* telnetConn;
static esp_tcp telnetTcp;

static const char* opmode_str[] =
{
    "NULL",
    "STA",
    "AP",
    "STA+AP",
};

static const char* phymode_str[] =
{
    "802.11b",
    "802.11g",
    "802.11n",
};

static const char* connstate_str[] =
{
    "IDLE",
    "CONNECTING",
    "WRONG_PASSWORD",
    "NO_AP_FOUND",
    "CONNECT_FAIL",
    "GOT_IP",
};


void ICACHE_FLASH_ATTR __attribute__((weak)) cmd_status_hook(CommandLine::Interface* interface)
{
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_status_handler(CommandLine::Interface* interface, char* args)
{
    interface->printf("CHIPID %08x\r\n", system_get_chip_id());
    interface->printf("VOLTAGE %dmV\r\n", (system_get_vdd33() * 1000) >> 10);
    interface->printf("MEMFREE %d\r\n", system_get_free_heap_size());
    system_print_meminfo();
    interface->printf("OPMODE %s\r\n", opmode_str[wifi_get_opmode()]);
    interface->printf("PHYMODE %s\r\n", phymode_str[wifi_get_phy_mode()]);
    uint8 macaddr[6];
    wifi_get_macaddr(STATION_IF, macaddr);
    interface->printf("CLIENT %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                   macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
    struct station_config sta_cfg;
    wifi_station_get_config(&sta_cfg);
    interface->printf("NETWORK CH%d %s %s %d %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                   wifi_get_channel(), sta_cfg.ssid, sta_cfg.password, sta_cfg.bssid_set,
                   sta_cfg.bssid[0], sta_cfg.bssid[1], sta_cfg.bssid[2],
                   sta_cfg.bssid[3], sta_cfg.bssid[4], sta_cfg.bssid[5]);
    interface->printf("CONNSTATE %s\r\n", connstate_str[wifi_station_get_connect_status()]);
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
    interface->printf("CLIENTIP %d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d\r\n", ip.ip.addr & 0xff,
                   (ip.ip.addr >> 8) & 0xff, (ip.ip.addr >> 16) & 0xff, ip.ip.addr >> 24,
                   ip.netmask.addr & 0xff, (ip.netmask.addr >> 8) & 0xff,
                   (ip.netmask.addr >> 16) & 0xff, ip.netmask.addr >> 24, ip.gw.addr & 0xff,
                   (ip.gw.addr >> 8) & 0xff, (ip.gw.addr >> 16) & 0xff, ip.gw.addr >> 24);
    interface->printf("DNSIP %d.%d.%d.%d %d.%d.%d.%d\r\n", sysCfg->dns1 & 0xff,
                   (sysCfg->dns1 >> 8) & 0xff, (sysCfg->dns1 >> 16) & 0xff, sysCfg->dns1 >> 24,
                   sysCfg->dns2 & 0xff, (sysCfg->dns2 >> 8) & 0xff,
                   (sysCfg->dns2 >> 16) & 0xff, sysCfg->dns2 >> 24);
    interface->printf("NTPIP %d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d\r\n", sysCfg->ntp1 & 0xff,
                   (sysCfg->ntp1 >> 8) & 0xff, (sysCfg->ntp1 >> 16) & 0xff, sysCfg->ntp1 >> 24,
                   sysCfg->ntp2 & 0xff, (sysCfg->ntp2 >> 8) & 0xff,
                   (sysCfg->ntp2 >> 16) & 0xff, sysCfg->ntp2 >> 24, sysCfg->ntp3 & 0xff,
                   (sysCfg->ntp3 >> 8) & 0xff, (sysCfg->ntp3 >> 16) & 0xff, sysCfg->ntp3 >> 24);
    interface->printf("TIMEZONE %d\r\n", sysCfg->timezone);
    wifi_get_macaddr(SOFTAP_IF, macaddr);
    interface->printf("SOFTAP %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                   macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
    wifi_get_ip_info(SOFTAP_IF, &ip);
    interface->printf("SERVERIP %d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d\r\n", ip.ip.addr & 0xff,
                   (ip.ip.addr >> 8) & 0xff, (ip.ip.addr >> 16) & 0xff, ip.ip.addr >> 24,
                   ip.netmask.addr & 0xff, (ip.netmask.addr >> 8) & 0xff,
                   (ip.netmask.addr >> 16) & 0xff, ip.netmask.addr >> 24, ip.gw.addr & 0xff,
                   (ip.gw.addr >> 8) & 0xff, (ip.gw.addr >> 16) & 0xff, ip.gw.addr >> 24);
    struct station_info* client = wifi_softap_get_station_info();
    while (client)
    {
        interface->printf("CLIENT %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\r\n",
                       client->bssid[0], client->bssid[1], client->bssid[2],
                       client->bssid[3], client->bssid[4], client->bssid[5],
                       client->ip.addr & 0xff, (client->ip.addr >> 8) & 0xff,
                       (client->ip.addr >> 16) & 0xff, client->ip.addr >> 24);
        client = client->next.stqe_next;
    }
    wifi_softap_free_station_info();
    cmd_status_hook(interface);
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_log_handler(CommandLine::Interface* interface, char* args)
{
    char* arg = StringTools::nextWord(&args);
    if (!interface->checkNoArgsLeft(&args)) return;
    if (!arg)
    {
        interface->printf("ERR Not enough arguments, please type \"log on\" or \"log off\"\r\n");
        return;
    }
    if (!strcmp(arg, "on")) ((Telnet::Server*)interface)->enableLogging = true;
    else if (!strcmp(arg, "off")) ((Telnet::Server*)interface)->enableLogging = false;
    else
    {
        interface->printf("ERR Bad argument, please type \"log on\" or \"log off\"\r\n");
        return;
    }
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_softap_handler(CommandLine::Interface* interface, char* args)
{
    char* arg = StringTools::nextWord(&args);
    if (!interface->checkNoArgsLeft(&args)) return;
    if (!arg)
    {
        interface->printf("ERR Not enough arguments, please type \"softap on\" or \"softap off\"\r\n");
        return;
    }
    if (!strcmp(arg, "on")) sysCfg->enableSoftAP = true;
    else if (!strcmp(arg, "off")) sysCfg->enableSoftAP = false;
    else
    {
        interface->printf("ERR Bad argument, please type \"softap on\" or \"softap off\"\r\n");
        return;
    }
    saveConfig();
    configureOpMode();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_network_handler(CommandLine::Interface* interface, char* args)
{
    char* essid = StringTools::nextWord(&args);
    char* password = StringTools::nextWord(&args);
    if (!interface->checkNoArgsLeft(&args)) return;
    if (!essid)
    {
        interface->printf("ERR Not enough arguments, please specify ESSID\r\n");
        return;
    }
    struct station_config stconf;
    memset(&stconf, 0, sizeof(stconf));
    os_strncpy((char*)stconf.ssid, essid, 32);
    if (password) os_strncpy((char*)stconf.password, password, 64);
    wifi_station_disconnect();
    wifi_station_set_config(&stconf);
    if (sysCfg->enableStation) wifi_station_connect();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_connect_handler(CommandLine::Interface* interface, char* args)
{
    sysCfg->enableStation = true;
    saveConfig();
    wifi_station_set_auto_connect(true);
    wifi_station_connect();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_disconnect_handler(CommandLine::Interface* interface, char* args)
{
    sysCfg->enableStation = false;
    saveConfig();
    wifi_station_set_auto_connect(false);
    wifi_station_disconnect();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_ip_handler(CommandLine::Interface* interface, char* args)
{
    char* ipaddr = StringTools::nextWord(&args);
    char* netmask = StringTools::nextWord(&args);
    char* gateway = StringTools::nextWord(&args);
    char* dns1 = StringTools::nextWord(&args);
    char* dns2 = StringTools::nextWord(&args);
    if (!interface->checkNoArgsLeft(&args)) return;
    if (!ipaddr || !netmask || !gateway)
    {
        interface->printf("ERR Not enough arguments\r\n");
        return;
    }
    uint32_t ip, mask, gw;
    if (!StringTools::parseIpAddr(&ip, ipaddr)
     || !StringTools::parseIpAddr(&mask, netmask)
     || !StringTools::parseIpAddr(&gw, gateway))
    {
        interface->printf("ERR Could not parse IP address\r\n");
        return;
    }
    sysCfg->enableDHCP = false;
    sysCfg->ipaddr = ip;
    sysCfg->netmask = mask;
    sysCfg->gateway = gw;
    if (dns1 && StringTools::parseIpAddr(&ip, dns1)) sysCfg->dns1 = ip;
    else sysCfg->dns1 = 0;
    if (dns2 && StringTools::parseIpAddr(&ip, dns2)) sysCfg->dns2 = ip;
    else sysCfg->dns2 = 0;
    saveConfig();
    configureDNS();
    configureOpMode();
    configureStation();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_dhcp_handler(CommandLine::Interface* interface, char* args)
{
    sysCfg->enableDHCP = true;
    saveConfig();
    configureStation();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_timezone_handler(CommandLine::Interface* interface, char* args)
{
    char* str = StringTools::nextWord(&args);
    if (!interface->checkNoArgsLeft(&args)) return;
    if (!str)
    {
        interface->printf("ERR Not enough arguments\r\n");
        return;
    }
    uint32_t offset;
    if (!StringTools::parseInt(&offset, str))
    {
        interface->printf("ERR Could not parse number\r\n");
        return;
    }
    sysCfg->timezone = *((char*)&offset);
    saveConfig();
    configureNTP();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_ntpserver_handler(CommandLine::Interface* interface, char* args)
{
    char* ip1 = StringTools::nextWord(&args);
    char* ip2 = StringTools::nextWord(&args);
    char* ip3 = StringTools::nextWord(&args);
    if (!interface->checkNoArgsLeft(&args)) return;
    if (!ip1)
    {
        interface->printf("ERR Not enough arguments\r\n");
        return;
    }
    uint32_t ip;
    if (!StringTools::parseIpAddr(&ip, ip1))
    {
        interface->printf("ERR Could not parse IP address\r\n");
        return;
    }
    sysCfg->ntp1 = ip;
    if (ip2 && StringTools::parseIpAddr(&ip, ip2)) sysCfg->ntp2 = ip;
    else sysCfg->ntp2 = 0;
    if (ip3 && StringTools::parseIpAddr(&ip, ip3)) sysCfg->ntp3 = ip;
    else sysCfg->ntp3 = 0;
    saveConfig();
    configureNTP();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_ntpsync_handler(CommandLine::Interface* interface, char* args)
{
    triggerNTP();
    interface->printf("OK\r\n");
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_exit_handler(CommandLine::Interface* interface, char* args)
{
    interface->printf("DBG Bye!\r\nOK\r\n");
    espconn_disconnect(telnetConn);
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_reboot_handler(CommandLine::Interface* interface, char* args)
{
    cmd_exit_handler(interface, NULL);
    reset();
}

void ICACHE_FLASH_ATTR EasyWifi::cmd_reset_handler(CommandLine::Interface* interface, char* args)
{
    interface->printf("DBG Erasing configuration section of flash...\r\n");
    ESP8266::Flash.erase(ESP8266::Flash.pageCount - ESP8266_FLASH_DATA_SIZE, ESP8266_FLASH_DATA_SIZE - 0x4000);
    ESP8266::Flash.erase(ESP8266::Flash.pageCount - 0x3000, 0x3000);
    cmd_reboot_handler(interface, NULL);
}

static class TelnetOut : public CommandLine::OutputHandler
{
public:
    void putString(const char* str, int len)
    {
        if (!telnetConn) return;
        espconn_send(telnetConn, (uint8_t*)(const_cast<char*>(str)), len);
     }
} telnetOut;

static Telnet::Server telnet(PROJECT_NAME, PROJECT_VERSION, telnetCommands, &telnetOut);

static void ICACHE_FLASH_ATTR telnetConnected(void* arg)
{
    if (telnetConn) espconn_disconnect(telnetConn);
    telnetConn = (espconn*)arg;
    espconn_set_opt(telnetConn, 0x0d);
    espconn_regist_time(telnetConn, 600, 1);
    telnet.newClient();
}

static void ICACHE_FLASH_ATTR telnetDisconnected(void* arg)
{
    telnetConn = NULL;
}

static void ICACHE_FLASH_ATTR telnetRecv(void* arg, char* data, uint16_t len)
{
    if (arg != telnetConn) return;
    if (telnet.handleData(data, len)) espconn_disconnect(telnetConn);
}

static void stdoutPutchar(char c)
{
    telnet.sendLogChar(c);
}

void ICACHE_FLASH_ATTR EasyWifi::initTelnet()
{
    tcpListen(&telnetListener, &telnetTcp, 23, telnetConnected, telnetDisconnected, telnetRecv);
    os_install_putc1((void*)stdoutPutchar);
}
