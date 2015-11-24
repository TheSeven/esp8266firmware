#pragma once

#include "global.h"
#include "lib/telnet/telnet.h"


#define BASE_TELNET_COMMANDS \
    { "help",       "[command]",                    "Shows help for a command", \
                    NULL, true,  CommandLine::Interface::helpHandler }, \
    { "status",     NULL,                           "Shows various status information", \
                    NULL, false, EasyWifi::cmd_status_handler }, \
    { "log",        "on|off",                       "Controls displaying of log messages on this console", \
                    NULL, true,  EasyWifi::cmd_log_handler }, \
    { "softap",     "on|off",                       "Enables/disables soft AP while connected to another AP", \
                    NULL, true,  EasyWifi::cmd_softap_handler }, \
    { "network",    "<essid> <password>",           "Configures the wireless network to connect to", \
                    NULL, true,  EasyWifi::cmd_network_handler }, \
    { "connect",    NULL,                           "Connects to the configured wireless network", \
                    NULL, false, EasyWifi::cmd_connect_handler }, \
    { "disconnect", NULL,                           "Disconnects from the configured wireless network", \
                    NULL, false, EasyWifi::cmd_disconnect_handler }, \
    { "ip",         "<ipaddr> <netmask> <gateway>", "Configures a static IP address for this node", \
                    NULL, true,  EasyWifi::cmd_ip_handler }, \
    { "dhcp",       NULL,                           "Configures a dynamic IP address for this node", \
                    NULL, false, EasyWifi::cmd_dhcp_handler }, \
    { "reset",      NULL,                           "Resets the node to its default configuration", \
                    NULL, false, EasyWifi::cmd_reset_handler }, \
    { "reboot",     NULL,                           "Reboots the node's operating system", \
                    NULL, false, EasyWifi::cmd_reboot_handler }, \
    { "exit",       NULL,                           "Closes the connection", \
                    NULL, false, EasyWifi::cmd_exit_handler }, \


extern const CommandLine::Command telnetCommands[];

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
    extern void cmd_exit_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_reboot_handler(CommandLine::Interface* interface, char* args);
    extern void cmd_reset_handler(CommandLine::Interface* interface, char* args);
    extern void initTelnet();
}
