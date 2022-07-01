/*-------------------------------------------------------------------------*
 * File:  NetworkStartup.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include <uEZPlatform.h>
#include <NVSettings.h>
#include "AppHTTPServer.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void INetworkConfigureWirelessConnection(T_uezDevice network)
{
    T_uezNetworkSettings network_settings = { UEZ_NETWORK_TYPE_INFRASTRUCTURE,

    /* -------------- General Network configuration ---------------- */
    // MAC Address (if not hardware defined)
        {{ 0, 0, 0, 0, 0, 0 }},

        // IP Address
        {{ 0, 0, 0, 0 }},
        // Subnet mask
        {{ 255, 255, 255, 0 }},
        // Gateway address
        {{ 0, 0, 0, 0 }},

        /* ------------- Wireless network specific settings -------------- */
        // Auto scan channel (0=Auto)
        0,

        // Transmit rate (0=Auto)
        0,

        // Transmit power (usually UEZ_NETWORK_TRANSMITTER_POWER_HIGH)
        UEZ_NETWORK_TRANSMITTER_POWER_HIGH,

        // DHCP Enabled?
        EFalse,

        // Security mode
        UEZ_NETWORK_SECURITY_MODE_WPA,

        // SSID (if ad-hoc)
        "uEZGUI",

        // DHCP Server is disabled
        EFalse,

        /** ------------- Network Type: IBSS (Peer to peer) ----------------*/
        // IBSS Channel
        0,

        /** If network type is UEZ_NETWORK_TYPE_IBSS (Peer to Peer),
         *  declare if this network is creating or joining. */
        UEZ_NETWORK_IBSS_ROLE_NONE, };

#if 0
    // Use settings from 0:CONFIG.INI file
    T_uezINISession ini;

    UEZINIOpen("0:CONFIG.INI", &ini);
    UEZINIGotoSection(ini, "Wireless");
    UEZINIGetString(ini, "ip", "192.168.10.2", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iIPAddress);
    UEZINIGetString(ini, "netmask", "255.255.255.0", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iSubnetMask);
    UEZINIGetString(ini, "gateway", "192.168.10.0", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iGatewayAddress);
    UEZINIClose(ini);
#elif 1
    UEZNetworkIPV4StringToAddr("192.168.10.2", &network_settings.iIPAddress);
    UEZNetworkIPV4StringToAddr("255.255.255.0", &network_settings.iSubnetMask);
    UEZNetworkIPV4StringToAddr("192.168.10.0", &network_settings.iGatewayAddress);
#else
    // Use non-volatile settings
    memcpy(&network_settings.iMACAddress, G_nonvolatileSettings.iMACAddr, 6);
    memcpy(&network_settings.iIPAddress.v4, G_nonvolatileSettings.iIPAddr, 4);
    memcpy(&network_settings.iGatewayAddress.v4, G_nonvolatileSettings.iIPGateway, 4);
    memcpy(&network_settings.iSubnetMask.v4, G_nonvolatileSettings.iIPMask, 4);
#endif
    UEZNetworkInfrastructureConfigure(network, &network_settings);
}

void INetworkConfigureWirelessAccessPoint(T_uezDevice network)
{
    T_uezNetworkSettings network_settings = {

        // Network type
        UEZ_NETWORK_TYPE_LIMITED_AP,

        /* -------------- General Network configuration ---------------- */
        // MAC Address (if not hardware defined)
        {{ 0, 0, 0, 0, 0, 0 }},

        // IP Address
        {{ 0, 0, 0, 0 }},

        // Subnet mask
        {{ 255, 255, 255, 0 }},

        // Gateway address
        {{ 0, 0, 0, 0 }},

        /* ------------- Wireless network specific settings -------------- */
        // Auto scan channel (0=Auto)
        0,

        // Transmit rate (0=Auto)
        0,

        // Transmit power (usually UEZ_NETWORK_TRANSMITTER_POWER_HIGH)
        UEZ_NETWORK_TRANSMITTER_POWER_HIGH,

        // DHCP Enabled?
        EFalse,

        // Security mode
        UEZ_NETWORK_SECURITY_MODE_WPA2, // UEZ_NETWORK_SECURITY_MODE_WPA,

        // SSID (for WAP)
        "uEZGUI",

        // DHCPServer Enable
        ETrue,

        /** ------------- Network Type: IBSS (Peer to peer) ----------------*/
        // IBSS Channel
        0,

        /** If network type is UEZ_NETWORK_TYPE_IBSS (Peer to Peer),
         *  declare if this network is creating or joining. */
        UEZ_NETWORK_IBSS_ROLE_CREATE, };

#if 0
    // Use settings from 0:CONFIG.INI file
    T_uezINISession ini;

    UEZINIOpen("0:CONFIG.INI", &ini);
    UEZINIGotoSection(ini, "WAP");
    UEZINIGetString(ini, "ip", "192.168.1.1", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iIPAddress);
    UEZINIGetString(ini, "netmask", "255.255.255.0", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iSubnetMask);
    UEZINIGetString(ini, "gateway", "192.168.1.0", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iGatewayAddress);
    UEZINIClose(ini);
#elif 1
    // Use hard coded settings
    UEZNetworkIPV4StringToAddr("192.168.1.1", &network_settings.iIPAddress);
    UEZNetworkIPV4StringToAddr("255.255.255.0", &network_settings.iSubnetMask);
    UEZNetworkIPV4StringToAddr("192.168.1.0", &network_settings.iGatewayAddress);
#elif 0
    // Use non-volatile settings
    memcpy(&network_settings.iMACAddress, G_nonvolatileSettings.iMACAddr, 6);
    memcpy(&network_settings.iIPAddress.v4, G_nonvolatileSettings.iIPAddr, 4);
    memcpy(&network_settings.iGatewayAddress.v4, G_nonvolatileSettings.iIPGateway, 4);
    memcpy(&network_settings.iSubnetMask.v4, G_nonvolatileSettings.iIPMask, 4);
#endif

    UEZNetworkInfrastructureConfigure(network, &network_settings);
}

void INetworkConfigureWiredConnection(T_uezDevice network)
{
    T_uezNetworkSettings network_settings = {
        UEZ_NETWORK_TYPE_INFRASTRUCTURE,

    /* -------------- General Network configuration ---------------- */
    // MAC Address (if not hardware defined)
        {{ 0, 0, 0, 0, 0, 0 }},

        // IP Address
        {{ 0, 0, 0, 0 }},
        // Subnet mask
        {{ 255, 255, 255, 0 }},
        // Gateway address
        {{ 0, 0, 0, 0 }},

        /* ------------- Wireless network specific settings -------------- */
        // Auto scan channel (0=Auto)
        0,

        // Transmit rate (0=Auto)
        0,

        // Transmit power (usually UEZ_NETWORK_TRANSMITTER_POWER_HIGH)
        UEZ_NETWORK_TRANSMITTER_POWER_HIGH,

        // DHCP Enabled?
        EFalse,

        // Security mode
        UEZ_NETWORK_SECURITY_MODE_OPEN,

        // SSID (if ad-hoc)
        {0},

        // DHCP Server is disabled
        EFalse,

        /** ------------- Network Type: IBSS (Peer to peer) ----------------*/
        // IBSS Channel
        0,

        /** If network type is UEZ_NETWORK_TYPE_IBSS (Peer to Peer),
         *  declare if this network is creating or joining. */
        UEZ_NETWORK_IBSS_ROLE_NONE,
    };

#if 0
    // Use settings from 0:CONFIG.INI file
    T_uezINISession ini;

    UEZINIOpen("0:CONFIG.INI", &ini);
    UEZINIGotoSection(ini, "Network");
    UEZINIGetString(ini, "ip", "192.168.10.20", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iIPAddress);
    UEZINIGetString(ini, "netmask", "255.255.255.0", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iSubnetMask);
    UEZINIGetString(ini, "gateway", "192.168.10.1", buffer, sizeof(buffer) - 1);
    UEZNetworkIPV4StringToAddr(buffer, &settings.iGatewayAddress);
    UEZINIClose(ini);
#elif 0
    UEZNetworkIPV4StringToAddr("192.168.10.2", &network_settings.iIPAddress);
    UEZNetworkIPV4StringToAddr("255.255.255.0", &network_settings.iSubnetMask);
    UEZNetworkIPV4StringToAddr("192.168.10.0", &network_settings.iGatewayAddress);
#else
    // Use non-volatile settings
    // Wired network uses the settings in NVSettings
    memcpy(&network_settings.iMACAddress, G_nonvolatileSettings.iMACAddr, 6);
    memcpy(&network_settings.iIPAddress.v4, G_nonvolatileSettings.iIPAddr, 4);
    memcpy(&network_settings.iGatewayAddress.v4, G_nonvolatileSettings.iIPGateway, 4);
    memcpy(&network_settings.iSubnetMask.v4, G_nonvolatileSettings.iIPMask, 4);
#endif
    UEZNetworkInfrastructureConfigure(network, &network_settings);
}

TUInt32 NetworkStartup(T_uezTask aMyTask, void *aParams)
{
#if UEZ_ENABLE_WIRELESS_NETWORK || UEZ_ENABLE_WIRED_NETWORK
    T_uezError error = UEZ_ERROR_NONE;
#endif

#if UEZ_ENABLE_WIRELESS_NETWORK
    int wirelessStarted = 0;
    T_uezDevice wireless_network;
    extern void UEZPlatform_WirelessNetwork0_Require(void);
#endif
#if UEZ_ENABLE_WIRED_NETWORK
    T_uezDevice wired_network;
    extern void INetworkConfigureWirelessConnection(T_uezDevice network);
    extern void INetworkConfigureWirelessAccessPoint(T_uezDevice network);
#endif

#if UEZ_ENABLE_WIRED_NETWORK
    // ----------------------------------------------------------------------
    // Bring up the Wired Network
    // ----------------------------------------------------------------------
    printf("Bringing up wired network: Start\n");

    // First, get the Wired Network connection
    UEZPlatform_WiredNetwork0_Require();
    error = UEZNetworkOpen("WiredNetwork0", &wired_network);
    if (error)
        UEZFailureMsg("NetworkStartup: Wired failed to start");

    // Configure the type of network desired
    INetworkConfigureWiredConnection(wired_network);

    // Bring up the infrastructure
    error = UEZNetworkInfrastructureBringUp(wired_network);

    // If no problem bringing up the infrastructure, join the network
    if (!error)
        error = UEZNetworkJoin(wired_network, "lwIP", 0, 5000);

    // Let the last messages through (debug only)
    UEZTaskDelay(1000);

    // Report the result
    if (error) {
        printf("Bringing up wired network: **FAILED** (error = %d)\n", error);
    } else {
        printf("Bringing up wired network: Done\n");
    }
#endif

#if UEZ_ENABLE_WIRELESS_NETWORK
    // ----------------------------------------------------------------------
    // Bring up the Wireless Network
    // ----------------------------------------------------------------------
    printf("Bringing up wireless network: Start\n");

    // First, get the Wireless Network connection
    UEZPlatform_WirelessNetwork0_Require();
    error = UEZNetworkOpen("WirelessNetwork0", &wireless_network);
    if (error) {
        // UEZFailureMsg("NetworkStartup: Wireless failed to start");
         wirelessStarted = 0;
    } else {
         wirelessStarted = 1;
    }

    if (wirelessStarted) {
        // Configure the type of network desired
        INetworkConfigureWirelessAccessPoint(wireless_network);
        //INetworkConfigureWirelessConnection(wireless_network);

        // Bring up the infrastructure
        error = UEZNetworkInfrastructureBringUp(wireless_network);

        // If no problem bringing up the infrastructure, join the network
        if (!error)
            error = UEZNetworkJoin(wireless_network, "uEZ", "fdifdifdiFDI", 5000);

        // Let the last messages through (debug only)
        UEZTaskDelay(1000);
    }

    // Report the result
    if (error) {
        printf("Bringing up wireless network: **FAILED** (error = %d)\n", error);
    } else {
        printf("Bringing up wireless network: Done\n");
    }
#endif

#if UEZ_BASIC_WEB_SERVER
    printf("Webserver starting\n");
    error = BasicWebStart(wired_network);
    if (error) {
        printf("Problem starting BasicWeb! (Error=%d)\n", error);
    } else {
        printf("BasicWeb started\n");
    }
#endif

#if UEZ_HTTP_SERVER
    #if UEZ_ENABLE_WIRED_NETWORK
      App_HTTPServerStart(wired_network);
    #endif
    #if UEZ_ENABLE_WIRELESS_NETWORK
    if (wirelessStarted)
        App_HTTPServerStart(wireless_network);
    #endif
#endif

    return 0;
}
/*-------------------------------------------------------------------------*
 * End of File:  NetworkStartup.c
 *-------------------------------------------------------------------------*/
