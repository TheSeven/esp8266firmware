#pragma once

#include "global.h"
#include "lib/telnet/telnet.h"


#define BASE_TELNET_COMMANDS \
    { "help",       "[command]", \
                    "Help for a command", \
                    NULL, true,  CommandLine::Interface::helpHandler }, \
    { "status",     NULL, \
                    "Status information", \
                    NULL, false, EasyWifi::cmd_status_handler }, \
    { "log",        "on|off", \
                    "Enable log messages on this console", \
                    NULL, true,  EasyWifi::cmd_log_handler }, \
    { "softap",     "on|off", \
                    "Use softAP while connected to another AP", \
                    NULL, true,  EasyWifi::cmd_softap_handler }, \
    { "network",    "<essid> <password>", \
                    "Wireless network to connect to", \
                    NULL, true,  EasyWifi::cmd_network_handler }, \
    { "connect",    NULL, \
                    "Connect to the network", \
                    NULL, false, EasyWifi::cmd_connect_handler }, \
    { "disconnect", NULL, \
                    "Disconnect from the network", \
                    NULL, false, EasyWifi::cmd_disconnect_handler }, \
    { "ip",         "<ipaddr> <netmask> <gateway> [dns1] [dns2]", \
                    "Use static IP address", \
                    NULL, true,  EasyWifi::cmd_ip_handler }, \
    { "dhcp",       NULL, \
                    "Get IP address from DHCP", \
                    NULL, false, EasyWifi::cmd_dhcp_handler }, \
    { "timezone",   "<offset>", \
                    "UTC offset in hours", \
                    NULL, true,  EasyWifi::cmd_timezone_handler }, \
    { "ntpserver",  "<ip> [ip2] [ip3]", \
                    "NTP servers to use", \
                    NULL, true,  EasyWifi::cmd_ntpserver_handler }, \
    { "ntpsync",    NULL, \
                    "Trigger NTP time sync", \
                    NULL, false,  EasyWifi::cmd_ntpsync_handler }, \
    { "reset",      NULL, \
                    "Reset to default configuration", \
                    NULL, false, EasyWifi::cmd_reset_handler }, \
    { "reboot",     NULL, \
                    "Reboot device", \
                    NULL, false, EasyWifi::cmd_reboot_handler }, \
    { "exit",       NULL, \
                    "Close this connection", \
                    NULL, false, EasyWifi::cmd_exit_handler }, \


extern const CommandLine::Command telnetCommands[];
extern void cmd_status_hook(CommandLine::Interface* interface);

namespace EasyWifi
{
    extern void cmd_status_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_log_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_softap_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_network_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_connect_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_disconnect_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_ip_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_dhcp_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_timezone_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_ntpserver_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_ntpsync_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_exit_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_reboot_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_reset_handler(CommandLine::Interface* interface, char* args);
    extern void initTelnet();
}
