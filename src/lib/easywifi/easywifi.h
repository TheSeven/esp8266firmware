#pragma once

#include "global.h"

extern "C"
{
#include "ets_sys.h"
#include "osapi.h"
#include "ip_addr.h"
#include "espconn.h"
#include "gpio.h"
#include "os_type.h"
#include "sntp.h"
#include "time.h"
}

#include "espmissingincludes.h"

#include "lib/easywifi/telnet.h"
#include "lib/easywifi/storage.h"
#include "sys/util.h"
#include "sys/time.h"


extern void appInit();

namespace EasyWifi
{
    extern void tcpListen(struct espconn* conn, esp_tcp* tcp, int port,
                          void (*connectCallback)(void* conn), void (*disconnectCallback)(void* conn),
                          void (*receiveCallback)(void* conn, char* data, uint16_t len));
    extern void configureOpMode();
    extern void configureStation();
    extern void configureDNS();
    extern void configureNTP();
    extern void triggerNTP();
}
