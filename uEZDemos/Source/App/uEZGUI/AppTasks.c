/*-------------------------------------------------------------------------*
 * File:  AppTasks.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Various tasks to possibly run.  Examples if nothing else.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZProcessor.h>
#include "AppTasks.h"
#include <NVSettings.h>
#include "Source/Library/Web/BasicWeb/BasicWeb.h"
#include <HAL/GPIO.h>
#include <UEZPlatform.h>

#if ((UEZ_PROCESSOR == NXP_LPC2478) && (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_SHARP_LQ043T3DG01))
extern TUInt8 heartbeatLED; //for uEZGUI 43 2478 different for rev 2 and rev 3 boards.
#endif
/*---------------------------------------------------------------------------*
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink heartbeat LED
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
    HAL_GPIOPort **p_gpio;
#if ((UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_SEIKO_70WVW2T) ||     (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_TIANMA_TM070RBHG04) ||       (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1) ||         (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_SHARP_LQ043T1DG28))
    TUInt8 heartbeatLED = 13;
#elif ((UEZ_PROCESSOR == NXP_LPC1788) && (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_SHARP_LQ043T3DG01 || UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_SHARP_LQ043T1DG28))
    TUInt8 heartbeatLED = 13;
#elif ((UEZ_PROCESSOR == NXP_LPC1788) && (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1))
    TUInt8 heartbeatLED = 13;
#endif

    HALInterfaceFind("GPIO1", (T_halWorkspace **)&p_gpio);

    (*p_gpio)->SetOutputMode(p_gpio, 1 << heartbeatLED);
    (*p_gpio)->SetMux(p_gpio, heartbeatLED, 0); // set to GPIO
    // Blink
    for (;;) {
        (*p_gpio)->Set(p_gpio, 1 << heartbeatLED);
        UEZTaskDelay(250);
        (*p_gpio)->Clear(p_gpio, 1 << heartbeatLED);
        UEZTaskDelay(250);
    }
}

void INetworkConfigureWiredConnection(T_uezDevice network)
{
    T_uezNetworkSettings network_settings = {
        UEZ_NETWORK_TYPE_INFRASTRUCTURE,

    /* -------------- General Network configuration ---------------- */
    // MAC Address (if not hardware defined)
        { 0, 0, 0, 0, 0, 0 },

        // IP Address
        { 0, 0, 0, 0 },
        // Subnet mask
        { 255, 255, 255, 0 },
        // Gateway address
        { 0, 0, 0, 0 },

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
        0,

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
    T_uezError error = UEZ_ERROR_NONE;

#if UEZ_ENABLE_WIRED_NETWORK
    T_uezDevice wired_network;
#endif

#if UEZ_ENABLE_WIRED_NETWORK
    // ----------------------------------------------------------------------
    // Bring up the Wired Network
    // ----------------------------------------------------------------------
    printf("Bringing up wired network: Start\n");

    // First, get the Wireless Network connection
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

#if UEZ_ENABLE_TCPIP_STACK
    printf("Webserver starting\n");
    error = BasicWebStart(wired_network);
    if (error) {
        printf("Problem starting BasicWeb! (Error=%d)\n", error);
    } else {
        printf("BasicWeb started\n");
    }
#endif

    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup tasks that run besides main or the main menu.
 *---------------------------------------------------------------------------*/
T_uezError SetupTasks(void)
{
    T_uezError error = UEZ_ERROR_NONE;

#if APP_ENABLE_HEARTBEAT_LED_ON
    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);
#endif

    error = UEZTaskCreate(
                (T_uezTaskFunction)NetworkStartup,
                "NetStart",
                UEZ_TASK_STACK_BYTES(1024),
                (void *)0,
                UEZ_PRIORITY_NORMAL,
                0);

    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  AppTasks.c
 *-------------------------------------------------------------------------*/
