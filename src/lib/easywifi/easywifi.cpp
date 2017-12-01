#include "global.h"
#include "lib/easywifi/easywifi.h"
#include "app/main.h"
#include "sys/util.h"
#include "lib/printf/printf.h"


static const char softap_password[] = SOFTAP_PASSWORD;


void ICACHE_FLASH_ATTR EasyWifi::tcpListen(struct espconn* conn, esp_tcp* tcp, int port,
                                           void (*connectCallback)(void* conn),
                                           void (*disconnectCallback)(void* conn),
                                           void (*receiveCallback)(void* conn, char* data, uint16_t len))
{
    conn->type = ESPCONN_TCP;
    conn->state = ESPCONN_NONE;
    tcp->local_port = port;
    conn->proto.tcp = tcp;
    espconn_regist_connectcb(conn, connectCallback);
    espconn_regist_recvcb(conn, receiveCallback);
    espconn_regist_disconcb(conn, disconnectCallback);
    espconn_accept(conn);
}

void ICACHE_FLASH_ATTR EasyWifi::configureOpMode()
{
    bool softap = sysCfg->enableSoftAP;
    bool station = sysCfg->enableStation;
    if (wifi_station_get_connect_status() != STATION_GOT_IP) softap = true;
    bool softapConnected = wifi_softap_get_station_num();
    softap |= softapConnected;
    softap |= read_usec_timer64() < 60000000;
    wifi_station_set_reconnect_policy(!softapConnected);
    wifi_set_opmode_current(station | (softap << 1));
}

void ICACHE_FLASH_ATTR EasyWifi::configureStation()
{
    if (sysCfg->enableDHCP) wifi_station_dhcpc_start();
	else
	{
        wifi_station_dhcpc_stop();
        struct ip_info info;
        info.ip.addr = sysCfg->ipaddr;
        info.netmask.addr = sysCfg->netmask;
        info.gw.addr = sysCfg->gateway;
        wifi_set_ip_info(STATION_IF, &info);
    }
}

void ICACHE_FLASH_ATTR EasyWifi::configureDNS()
{
    espconn_dns_setserver(0, (ip_addr_t*)&sysCfg->dns1);
    espconn_dns_setserver(1, (ip_addr_t*)&sysCfg->dns2);
}

void ICACHE_FLASH_ATTR EasyWifi::configureNTP()
{
    sntp_stop();
    sntp_set_timezone(sysCfg->timezone);
    sntp_setserver(0, (ip_addr_t*)&sysCfg->ntp1);
    sntp_setserver(1, (ip_addr_t*)&sysCfg->ntp2);
    sntp_setserver(2, (ip_addr_t*)&sysCfg->ntp3);
    sntp_init();
}

void ICACHE_FLASH_ATTR EasyWifi::triggerNTP()
{
    sntp_stop();
    sntp_init();
}

static void ICACHE_FLASH_ATTR wifiEventHandler(System_Event_t* evt)
{
    if (evt->event >= EVENT_SOFTAPMODE_PROBEREQRECVED) return;
    EasyWifi::configureOpMode();
}

int ICACHE_FLASH_ATTR main()
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);
    ets_wdt_init(1000000);

    EasyWifi::initStorage();

    uint8 macaddr[6];
    wifi_get_macaddr(STATION_IF, macaddr);

    struct softap_config softap;
    memset(&softap, 0, sizeof(softap));
    softap.ssid_len = snprintf((char*)softap.ssid, sizeof(softap.ssid),
                               SOFTAP_PREFIX "_%02x%02x%02x%02x%02x%02x",
                               macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
    softap.authmode = AUTH_WPA_WPA2_PSK;
    os_memcpy(softap.password, softap_password, sizeof(softap_password));
    softap.channel = 1 + (macaddr[5] & 3) * 4;
    softap.max_connection = 10;
    wifi_softap_set_config_current(&softap);
    wifi_station_ap_number_set(1);
    wifi_set_opmode(STATIONAP_MODE);
    wifi_station_set_hostname((char*)softap.ssid);
    wifi_set_broadcast_if(3);

    wifi_softap_dhcps_stop();
    struct ip_info info;
    IP4_ADDR(&info.ip, 192, 168, 255, 1);
    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
    IP4_ADDR(&info.gw, 192, 168, 255, 254);
    wifi_set_ip_info(SOFTAP_IF, &info);
    fixed_dhcps_lease dhcp;
    IP4_ADDR(&dhcp.start_ip, 192, 168, 255, 100);
    IP4_ADDR(&dhcp.end_ip, 192, 168, 255, 199);
    wifi_softap_set_dhcps_lease((dhcps_lease*)&dhcp);
    wifi_softap_dhcps_start();
    
    wifi_station_ap_number_set(1);
    EasyWifi::configureStation();
    wifi_station_set_auto_connect(sysCfg->enableStation);
    wifi_station_set_reconnect_policy(1);
    wifi_set_event_handler_cb(wifiEventHandler);
    EasyWifi::configureOpMode();
    EasyWifi::configureDNS();
    EasyWifi::configureNTP();
    
    EasyWifi::initTelnet();

    appInit();

    return 0;
}
