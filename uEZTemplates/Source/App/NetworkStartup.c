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
#include <uEZPlatform.h>
#include "NetworkStartup.h"
//#include "NetworkDemos.h"
#include <NVSettings.h>
#include <Config_Build.h>
#include <sys/socket.h>

/*---------------------------------------------------------------------------*
 * Memory placement section:
 *---------------------------------------------------------------------------*/
//Allocate network memory (Not used today)
//UEZ_PUT_SECTION(".network", static TUInt8 _networkMemory [NETWORK_STACK_RAM_SIZE]);
//TUInt8 *_networkMemoryptr = _networkMemory;
   
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

#if (UEZ_ENABLE_TCPIP_STACK == 1)
TBool G_networkDown = EFalse;
extern T_uezTask G_lwipTask;

#include "Source/Library/Web/BasicWeb/BasicWEB.h"
//#include "AppHTTPServer.h"
//#include "ModbusTCPIPTask.h"
#include "Source/Library/Web/AWSIoTClient/AWSIoTClient.h"

#if (UEZ_HTTP_SERVER == 1)
extern T_uezTask G_HttpServerTask;
#endif
#if (UEZ_BASIC_WEB_SERVER == 1)
extern T_uezTask G_BasicWebTask;
#endif

#ifndef UEZ_AWS_IOT_CLIENT_DEMO
#define UEZ_AWS_IOT_CLIENT_DEMO  0
#endif

static uint32_t G_NetworkDevices[3] = {0};

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Wireless:
 *-------------------------------------------------------------------------*/
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
extern void INetworkConfigureWirelessConnection(T_uezDevice network);
extern void INetworkConfigureWirelessAccessPoint(T_uezDevice network);
extern void UEZPlatform_WirelessNetwork0_Require(void); // This should be in platform file header.
extern void UEZPlatform_WirelessNetwork1_Require(void); // This should be in platform file header.
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
        ETrue,

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
    UEZNetworkConfigureInfrastructure(network, &network_settings);
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

    UEZNetworkConfigureInfrastructure(network, &network_settings);
}
#endif

/*-------------------------------------------------------------------------*
 * Wired:
 *-------------------------------------------------------------------------*/
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
        ETrue, // Enable DHCP Client
        //EFalse, // Disable DHCP Client and use static IP

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
    memcpy(&network_settings.iMACAddress, &G_nonvolatileSettings.iMACAddr, 6);
    memcpy(&network_settings.iIPAddress.v4, &G_nonvolatileSettings.iIPAddr, 4);
    memcpy(&network_settings.iGatewayAddress.v4, &G_nonvolatileSettings.iIPGateway, 4);
    memcpy(&network_settings.iSubnetMask.v4, &G_nonvolatileSettings.iIPMask, 4);
#endif
    UEZNetworkConfigureInfrastructure(network, &network_settings);
}


/*-------------------------------------------------------------------------*
 * Common NetworkStartup:
 *-------------------------------------------------------------------------*/
TUInt32 NetworkStartup(T_uezTask aMyTask, void *aParams)
{
    PARAM_NOT_USED(aMyTask);
    PARAM_NOT_USED(aParams);
    TUInt8 Index = 0;
    UEZTaskDelay(5000); // allow for bootup logo/video to finish and reach main menu first.
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1) || (UEZ_ENABLE_WIRED_NETWORK == 1)
#if 1//LWIP_DHCP && UEZ_ENABLE_WIRELESS_NETWORK
    TBool wait = ETrue;
    T_uezNetworkStatus status;
#endif
    T_uezError error = UEZ_ERROR_NONE;
#endif

#if (UEZ_ENABLE_WIRED_NETWORK == 1)
    T_uezDevice wired_network;
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

    // At this point if using lwip the lwip task has been created along with a DHCP task.

    // If no problem bringing up the infrastructure, join the network
    if (!error) {
        error = UEZNetworkJoin(wired_network, "lwIP", 0, 5000);
    }

    // Let the last messages through (debug only)
    //UEZTaskDelay(1000);

    // Report the result
    if (error) {
        printf("Bringing up wired network: **FAILED** (error = %d)\n", error);
        if(error == 4) {
            printf("Increase Project Heap Memory\n");
        }
        return 1;
    } else {
        printf("Bringing up wired network: Done\n");
        G_NetworkDevices[Index++] = wired_network;
    }
#endif // if wired

#if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    int wirelessStarted = 0;
    T_uezDevice wireless_network;
    // ----------------------------------------------------------------------
    // Bring up the Wireless Network
    // ----------------------------------------------------------------------
    printf("Bringing up wireless network: Start\n");

    // First, get the Wireless Network connection
    UEZPlatform_WirelessNetwork1_Require();
    error = UEZNetworkOpen("WirelessNetwork1", &wireless_network);
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
        //UEZTaskDelay(1000);
    }

    // Report the result
    if (error) {
        printf("Bringing up wireless network: **FAILED** (error = %d)\n", error);
    } else {
        printf("Bringing up wireless network: Done\n");
        G_NetworkDevices[Index++] = wireless_network;
    }
#endif // if wireless
    
  // ----------------------------------------------------------------------
  // Now we can start web server, modbus, or do DHCP related things here.
  // ----------------------------------------------------------------------

#if (UEZ_ENABLE_TCPIP_STACK == 1) // TODO fix DHCP for runtime on or static settings
#if 1//LWIP_DHCP && UEZ_ENABLE_WIRELESS_NETWORK

    TBool G_DHCP_Task_Has_IP = EFalse;
    (void) G_DHCP_Task_Has_IP;
    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
    while(wait){
        UEZNetworkGetStatus(wired_network, &status);

        if(status.iIPAddr.v4[0] != 0){
            wait = EFalse;
            printf("IP Addr: %d.%d.%d.%d\n",
                    status.iIPAddr.v4[0],
                    status.iIPAddr.v4[1],
                    status.iIPAddr.v4[2],
                    status.iIPAddr.v4[3]);
        }
        UEZTaskDelay(1000);
    }
    #endif
    #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    while(wait){
        UEZNetworkGetStatus(wireless_network, &status);

        if(status.iIPAddr.v4[0] != 0){
            wait = EFalse;
            printf("IP Addr: %d.%d.%d.%d\n",
                    status.iIPAddr.v4[0],
                    status.iIPAddr.v4[1],
                    status.iIPAddr.v4[2],
                    status.iIPAddr.v4[3]);
        }
        UEZTaskDelay(1000);
    }
    #endif
#endif // if 1
#endif // if enable tcpip

//ModbusTCPIPTask_Start();

#if (UEZ_HTTP_SERVER == 1)
    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
      //App_HTTPServerStart(wired_network);
    #endif
    #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    if (wirelessStarted)
        App_HTTPServerStart(wireless_network);
    #endif
#endif
#if (UEZ_BASIC_WEB_SERVER == 1)
    printf("Webserver starting\n");
    error = BasicWebStart(wired_network);
    if (error) {
        printf("Problem starting BasicWeb! (Error=%d)\n", error);
    } else {
        printf("BasicWeb started\n");
    }
#endif

#if (UEZ_NETWORK_DEMOS == 1)
  #if (UEZ_ENABLE_WIRED_NETWORK == 1)
    Start_lwIP_Network_Demos(wired_network);
  #endif
  #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    Start_lwIP_Network_Demos(wireless_network);
  #endif
#endif

#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)
  #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    error = AWSIoTClientStart(wireless_network);
  #else
    #if (UEZ_ENABLE_WIRED_NETWORK == 1)      
      error = AWSIoTClientStart(wired_network);
    #endif
  #endif
#endif

  // --------------------------------------------------------------------------------------------------------
  // Now we can stay in a monitoring loop and restart/add/switch interfaces as needed.
  // For now the PHY timeout reset is only for the Ethernet PHY since the incoming peripheral clock can halt.
  // For any UART/SPI/SDIO Wi-Fi/Cell module, we shouldn't need to re-init them due to clock stoppage.
  // --------------------------------------------------------------------------------------------------------
#if (UEZ_ENABLE_WIRED_NETWORK == 1)
#if (EMAC_USE_INTERRUPT_TIMEOUT_DETECT == 1)
    HAL_EMAC **p_emac;
    HALInterfaceFind("EMAC", (T_halWorkspace **)&p_emac);
    TUInt16 *phyTimeoutCounterPtr = (*p_emac)->GetPhyTimeoutCounter();
#endif
#endif

    while(1) {
#if (UEZ_ENABLE_WIRED_NETWORK == 1)
#if (EMAC_USE_INTERRUPT_TIMEOUT_DETECT == 1)
        (*phyTimeoutCounterPtr)++;
#endif
#endif
        UEZTaskDelay(5000);
        
        // In this loop can do general monitoring of network tasks and could allow for dynamically turning DHCP on/off or similar.

#if 0
        if{network_settings.iEnableDHCP == ETrue) {
        } else { // DHCP enabled
        }
#endif

        //UEZNetworkGetStatus(wired_network, &status);
#if (UEZ_ENABLE_WIRED_NETWORK == 1)
#if (EMAC_USE_INTERRUPT_TIMEOUT_DETECT == 1)
        if((*phyTimeoutCounterPtr) > 596) {
          printf("Phy Isr Inactivity Count: %u\n", (uint32_t) (*phyTimeoutCounterPtr));
          // If 600 seconds go by with no interrupt the phy and clock must have died.
          if((*phyTimeoutCounterPtr) > 600) {
              G_networkDown = ETrue;
              NetworkStopApplications(wired_network);
          }
        }
#endif
#endif

        if(G_networkDown == ETrue) {
#if (UEZ_ENABLE_WIRED_NETWORK == 1)
         error = NetworkRestartPhy(wired_network);
#endif
       }
    }
    //return 0;
}

T_uezError NetworkRestartPhy(T_uezDevice network)
{
  T_uezError error = UEZNetworkInfrastructureRestart(network);

  // If no problem bringing up the infrastructure, join the network
  if (!error) {
      error = UEZNetworkJoin(network, "lwIP", 0, 5000);
  }

  // Report the result
  if (error) {
      printf("Restart wired network: **FAILED** (error = %d)\n", error);
      UEZTaskDelay(3000);
  } else {
      printf("Restart wired network: Done\n");
      G_networkDown = EFalse;
      NetworkReStartApplications(network);
  }
  return error;
}

void NetworkStopApplications(T_uezDevice network)
{
    PARAM_NOT_USED(network);
#if (UEZ_HTTP_SERVER == 1)
    if(G_HttpServerTask != NULL) { // May need to restart higher level tasks if stuck in queue.
      UEZTaskDelete(G_HttpServerTask);
    }
#endif

#if (UEZ_BASIC_WEB_SERVER == 1)
    if(G_BasicWebTask != NULL) { // May need to restart higher level tasks if stuck in queue.
      UEZTaskDelete(G_BasicWebTask);
    }
#endif

#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)    
      AWSIoTClientStop();
#endif

}

T_uezError NetworkReStartApplications(T_uezDevice network)
{
  T_uezError error = UEZ_ERROR_NONE;
     #if (UEZ_HTTP_SERVER == 1)
        #if (UEZ_ENABLE_WIRED_NETWORK == 1)
          App_HTTPServerStart(network);
        #endif
        #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
        //if (wirelessStarted)
            App_HTTPServerStart(network);
        #endif
    #endif
    #if (UEZ_BASIC_WEB_SERVER == 1)
        error = BasicWebStart(network);
        if (error) {
            printf("Problem starting BasicWeb! (Error=%d)\n", error);
        } else {
            printf("BasicWeb Restarted\n");
        }
    #endif

#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)    
     AWSIoTClientRestart();
#endif

    return error;
}

T_uezDevice NetworkGetActiveDevice(TUInt8 Index)
{
  return G_NetworkDevices[Index];
}

#endif

#if (UEZ_BASIC_WEB_SERVER == 1)
 void FuncTestPageHit(void)
 {
 }
 #endif
/*-------------------------------------------------------------------------*
 * End of File:  NetworkStartup.c
 *-------------------------------------------------------------------------*/
