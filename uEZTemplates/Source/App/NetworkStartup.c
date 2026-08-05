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
//#include "NetworkDemos.h" // can copy from uEZDemos.c example
#include <NVSettings.h>
#include <Config_Build.h>
#include <sys/socket.h>
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h> // rtt debug usage

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
extern sys_thread_t G_lwipTask;

#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include "AppHTTPServer.h"
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

#define MAX_NETWORK_INDEXES 3
static uint32_t G_NetworkDevices[MAX_NETWORK_INDEXES] = {0};
static TUInt8 G_Index = 0;
//#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)
uint8_t CurrentNetworkIndexUsedAsPrimaryInterface;
//#endif

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static TBool IScanGetNetworkName(
    void *aCallbackWorkspace,
    T_uezNetworkInfo *aNetworkInfo);

/*-------------------------------------------------------------------------*
 * Wireless:
 *-------------------------------------------------------------------------*/
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
extern void INetworkConfigureWirelessConnection(T_uezDevice network);
extern void INetworkConfigureWirelessAccessPoint(T_uezDevice network);
extern void UEZPlatform_WirelessNetwork0_Require(void); // This should be in platform file header.
extern void UEZPlatform_WirelessNetwork1_Require(void); // This should be in platform file header.
static TBool IScanGetNetworkName(void *aCallbackWorkspace, T_uezNetworkInfo *aNetworkInfo);
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
        UEZ_NETWORK_TYPE_WIRED,

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

void AddNetworkDevicehandle(uint32_t networkDevice)
{
    if(G_NetworkDevices[G_Index] == 0) {
      G_NetworkDevices[G_Index] = networkDevice;
      printf("Added Network Device Index %u\n", G_Index);
    }
    if(G_Index < MAX_NETWORK_INDEXES) {
      G_Index++;
    }
}

void RemoveNetworkDevicehandle(uint32_t networkDevice)
{
    for(uint8_t i = 0; i < MAX_NETWORK_INDEXES; i++) {
      if(G_NetworkDevices[i] == networkDevice) {
        G_NetworkDevices[i] = 0;        
        printf("Removed Network Device Index %u\n", i);
        break;
      }
    }
    //G_Index
}

/*-------------------------------------------------------------------------*
 * Common NetworkStartup:
 *-------------------------------------------------------------------------*/
TUInt32 NetworkStartup(T_uezTask aMyTask, void *aParams)
{
    PARAM_NOT_USED(aMyTask);
    PARAM_NOT_USED(aParams);
     (void)IScanGetNetworkName;

#if (UEZ_ENABLE_WIRED_NETWORK == 1)
    TBool gotWiredIP = EFalse;
#endif      
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    TBool gotWirelessIP = EFalse;
#endif

    //UEZTaskDelay(5000); // allow for bootup logo/video to finish and reach main menu first.
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1) || (UEZ_ENABLE_WIRED_NETWORK == 1)
#if 1//LWIP_DHCP && UEZ_ENABLE_WIRELESS_NETWORK
    TBool waitForeverForIP = EFalse;
    T_uezNetworkStatus status;
#endif
    T_uezError error = UEZ_ERROR_NONE;
    
#if (UEZ_ENABLE_WIRED_NETWORK == 1)
    T_uezError errorWired = UEZ_ERROR_NONE;
#endif
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    T_uezError errorWireless = UEZ_ERROR_NONE;
#endif
#endif

#if (UEZ_ENABLE_WIRED_NETWORK == 1)
    T_uezDevice wired_network;
    // ----------------------------------------------------------------------
    // Bring up the Wired Network
    // ----------------------------------------------------------------------
    printf("Bringing up wired network: Start\n");

    // First, get the Wired Network connection
    UEZPlatform_WiredNetwork0_Require();
    errorWired = UEZNetworkOpen("WiredNetwork0", &wired_network);
    if (errorWired)
        UEZFailureMsg("NetworkStartup: Wired failed to start");

    // Configure the type of network desired
    INetworkConfigureWiredConnection(wired_network);

    // Bring up the infrastructure
    errorWired = UEZNetworkInfrastructureBringUp(wired_network);

    // At this point if using lwip the lwip task has been created along with a DHCP task.

    // If no problem bringing up the infrastructure, join the network
    if (!errorWired) {
        errorWired = UEZNetworkJoin(wired_network, "lwIP", 0, 5000);
    }

    // Let the last messages through (debug only)
    //UEZTaskDelay(1000);

    // Report the result
    if (errorWired) {
        printf("Bringing up wired network: **FAILED** (error = %d)\n", error);
        if(errorWired == 4) {
            printf("Increase Project Heap Memory\n");
        }
        return 1;
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
    errorWireless = UEZNetworkOpen("WirelessNetwork1", &wireless_network);
    if (errorWireless) {
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
        errorWireless = UEZNetworkInfrastructureBringUp(wireless_network);

        // If no problem bringing up the infrastructure, join the network
        if (!errorWireless) {
            error = UEZNetworkJoin(wireless_network, "PutWiFiSsidHere", "PutWiFiPassphraseHere", 5000);
        }
    }
    if (errorWireless) {
        printf("Bringing up wireless network: **FAILED** (error = %d)\n", error);
    }
#endif // if wireless
    
    
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    if (!errorWireless) {
        printf("Bringing up wireless network: Done\n");
        AddNetworkDevicehandle(wireless_network);
    }
#endif // if wireless
#if (UEZ_ENABLE_WIRED_NETWORK == 1)
    if (!errorWired) {
        printf("Bringing up wired network: Done\n");
        AddNetworkDevicehandle(wired_network);
    }
#endif // if wired


  // ----------------------------------------------------------------------
  // Now we can start web server, modbus, or do DHCP related things here.
  // ----------------------------------------------------------------------

#if (UEZ_ENABLE_TCPIP_STACK == 1) // TODO fix DHCP for runtime on or static settings
    
    TBool G_DHCP_Task_Has_IP = EFalse; // Currently this DHCP task is only for lwip Ethernet
    (void) G_DHCP_Task_Has_IP;

    if(waitForeverForIP == ETrue) { // go through all interfaces one by one until they all have an IP
      #if (UEZ_ENABLE_WIRED_NETWORK == 1)
       while(gotWiredIP == EFalse) {
          UEZTaskDelay(500);
          UEZNetworkGetStatus(wired_network, &status);
          if(status.iIPAddr.v4[0] != 0) {
              printf("Wired IP Addr: %d.%d.%d.%d\n",
                      status.iIPAddr.v4[0],
                      status.iIPAddr.v4[1],
                      status.iIPAddr.v4[2],
                      status.iIPAddr.v4[3]);
              gotWiredIP = ETrue;
          }
      }
      #endif
      #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
       while(gotWirelessIP == EFalse) {
          UEZTaskDelay(500);
          UEZNetworkGetStatus(wireless_network, &status);
          if(status.iIPAddr.v4[0] != 0) {
              printf("Wireless IP Addr: %d.%d.%d.%d\n",
                      status.iIPAddr.v4[0],
                      status.iIPAddr.v4[1],
                      status.iIPAddr.v4[2],
                      status.iIPAddr.v4[3]);
              gotWirelessIP = ETrue;
          }
      }
      #endif
    } else { // 10 second timeout wait (not counting how long the getstatus blocks)
      for(uint8_t triesForIP = 0; triesForIP < 20; triesForIP++) {
      UEZTaskDelay(500);
      #if (UEZ_ENABLE_WIRED_NETWORK == 1)
          if(gotWiredIP == EFalse) {
                UEZNetworkGetStatus(wired_network, &status);
                if(status.iIPAddr.v4[0] != 0) {
                    printf("Wired IP Addr: %d.%d.%d.%d\n",
                            status.iIPAddr.v4[0],
                            status.iIPAddr.v4[1],
                            status.iIPAddr.v4[2],
                            status.iIPAddr.v4[3]);
                    gotWiredIP = ETrue;
                }
          }
      #endif
      #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
          if(gotWirelessIP == EFalse) {
              UEZNetworkGetStatus(wireless_network, &status);
              if(status.iIPAddr.v4[0] != 0) {
                  printf("Wireless IP Addr: %d.%d.%d.%d\n",
                          status.iIPAddr.v4[0],
                          status.iIPAddr.v4[1],
                          status.iIPAddr.v4[2],
                          status.iIPAddr.v4[3]);
                  gotWirelessIP = ETrue;
              }
          }
      #endif
      }


#if (UEZ_ENABLE_WIRED_NETWORK == 1)
      if(gotWiredIP == EFalse) {
        printf("No Wired IP address or cable unplugged.\n");
        //RemoveNetworkDevicehandle(wired_network); // Ethernet can be plugged in later and get an IP address
      }
#endif      
#if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
      if(gotWirelessIP == EFalse) {
        printf("No Wireless IP address.\n");
         // Will need to trigger AP connection again with new params or set static IP as it most likely won't magically start working later.
        RemoveNetworkDevicehandle(wireless_network);
      }
#endif

    } // end timeout wait
#endif // if enable tcpip

    for(uint8_t j = (MAX_NETWORK_INDEXES-1); j >=0; j--) {
      if(G_NetworkDevices[j] != 0) {
        CurrentNetworkIndexUsedAsPrimaryInterface = j;
        break;
      }
    }

    /* Can be used for testing */
    //error = UEZNetworkScan(wireless_network, 0, NULL, IScanGetNetworkName, NULL, 10000);
    
    //ModbusTCPIPTask_Start();

#if (UEZ_HTTP_SERVER == 1)
    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
      App_HTTPServerStart(wired_network);
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
        printf("BasicWeb started (default port 81)\n");
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
    printf("Starting MQTT Using Interface index %u\n", CurrentNetworkIndexUsedAsPrimaryInterface);
    error = AWSIoTClientStart(G_NetworkDevices[CurrentNetworkIndexUsedAsPrimaryInterface]);
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
#if (EMAC_USE_INTERRUPT_TIMEOUT_DETECT == 1) // we want to accurately count up to 600 seconds with no activity
        (*phyTimeoutCounterPtr)++; // make sure the task delay below, the RTOS tick rate, and the counter number all match together.
#endif
#endif
        UEZTaskDelay(1000);
        
        // In this loop can do general monitoring of network tasks and could allow for dynamically turning DHCP on/off or similar.

#if 0
        if(network_settings.iEnableDHCP == ETrue) {
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

#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)
    if(AWSIoTClientMonitorIsMQTTRunning() == EFalse) { // MQTT task was terminated, restart on diff network if exists
      uint8_t k;
      if(CurrentNetworkIndexUsedAsPrimaryInterface == 0) {
        k = (MAX_NETWORK_INDEXES-1);
      } else {
        k = CurrentNetworkIndexUsedAsPrimaryInterface-1;
      }
      
      for(int8_t i = k; i >=0; i--) {
        if(G_NetworkDevices[i] != 0) {
          CurrentNetworkIndexUsedAsPrimaryInterface = i; break;
        }
      }
      
      printf("Switching MQTT Interface to index %u\n", CurrentNetworkIndexUsedAsPrimaryInterface);
      UEZTaskDelay(1000);
      error = AWSIoTClientMqttRestart(G_NetworkDevices[CurrentNetworkIndexUsedAsPrimaryInterface]);
    }
#endif

        if(G_networkDown == ETrue) {
#if (UEZ_ENABLE_WIRED_NETWORK == 1)
         error = NetworkRestartPhy(wired_network);
#endif
       }
    }
    //return 0;
}

static TBool IScanGetNetworkName(
    void *aCallbackWorkspace,
    T_uezNetworkInfo *aNetworkInfo)
{
    char buffer[96];
    snprintf(buffer, 96, "%32s\t%02x:%02x:%02x:%02x:%02x:%02x\t%16s\t%10d\t%10u\n", aNetworkInfo->iName, 
    		(unsigned int) aNetworkInfo->iBSSID[0],
			(unsigned int) aNetworkInfo->iBSSID[1],
			(unsigned int) aNetworkInfo->iBSSID[2],
			(unsigned int) aNetworkInfo->iBSSID[3],
			(unsigned int) aNetworkInfo->iBSSID[4],
			(unsigned int) aNetworkInfo->iBSSID[5],
			(aNetworkInfo->iSecurityMode == UEZ_NETWORK_SECURITY_MODE_OPEN) ? "OPEN"
				: (aNetworkInfo->iSecurityMode == UEZ_NETWORK_SECURITY_MODE_WPA) ? "WPA"
				: (aNetworkInfo->iSecurityMode == UEZ_NETWORK_SECURITY_MODE_WPA2) ? "WPA2"
				: (aNetworkInfo->iSecurityMode == UEZ_NETWORK_SECURITY_MODE_WEP) ? "WEP"
				: (aNetworkInfo->iSecurityMode == UEZ_NETWORK_SECURITY_MODE_WPA_ENTERPRISE) ? "WPA ENTERPRISE"
				: (aNetworkInfo->iSecurityMode == UEZ_NETWORK_SECURITY_MODE_WPA2_ENTERPRISE) ? "WPA2 ENTERPRISE"
				: (aNetworkInfo->iSecurityMode == UEZ_NETWORK_SECURITY_MODE_UNKNOWN) ? "UNKNOWN"
				: "UNKNOWN",
			(int) aNetworkInfo->iRSSILevel,
			(unsigned int) aNetworkInfo->iChannel);
    DEBUG_RTT_Write(0, buffer, strlen(buffer));
    return ETrue;
}

T_uezError NetworkRestartPhy(T_uezDevice network)
{
  T_uezError error = UEZNetworkInfrastructureRestart(network);

  // If no problem bringing up the infrastructure, join the network
  if (!error) {
      //error = UEZNetworkJoin(network, "lwIP", 0, 5000); // not needed for ethernet, but may be needed for wifi someday
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
    if(G_HttpServerTask != (T_uezTask) NULL) { // restarting not supported on this task yet, only restart the PHY
      //UEZTaskDelete(G_HttpServerTask);
    }
#endif

#if (UEZ_BASIC_WEB_SERVER == 1)
    if(G_BasicWebTask != (T_uezTask) NULL) { // restarting not supported on this task yet, only restart the PHY
      //UEZTaskDelete(G_BasicWebTask);
    }
#endif

#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)    
      AWSIoTClientStop(); // AWS demo does support restarting, but for a PHY reset it shouldn't be needed to restart it.
#endif

}

T_uezError NetworkReStartApplications(T_uezDevice network)
{
  T_uezError error = UEZ_ERROR_NONE;
     #if (UEZ_HTTP_SERVER == 1)
        #if (UEZ_ENABLE_WIRED_NETWORK == 1)
          //App_HTTPServerStart(network); // restarting not supported on this task yet, only restart the PHY
        #endif
        #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
        //if (wirelessStarted)
            //App_HTTPServerStart(network); // restarting not supported on this task yet, only restart the PHY
        #endif
    #endif
    #if (UEZ_BASIC_WEB_SERVER == 1)
        //error = BasicWebStart(network); // restarting not supported on this task yet, only restart the PHY
        if (error) {
            //printf("Problem starting BasicWeb! (Error=%d)\n", error);
        } else {
            //printf("BasicWeb Restarted\n");
        }
    #endif

#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)    
     AWSIoTClientRestart();
#endif

    return error;
}


T_uezDevice NetworkGetPrimaryDevice(void) 
{
  return G_NetworkDevices[CurrentNetworkIndexUsedAsPrimaryInterface];
}

T_uezDevice NetworkGetActiveDevice(TUInt8 Index)
{
  return G_NetworkDevices[Index];
}

#else
T_uezDevice NetworkGetActiveDevice(TUInt8 Index)
{ 
  (void) Index;
  return 0;
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

