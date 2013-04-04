/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZGPIO.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZTemperature.h>
#include <uEZAccelerometer.h>
#include "NVSettings.h"
#include <stdio.h>

#include <Source/Library/Graphics/Glyph/glyph_api.h>
#include <Source/Library/Web/BasicWeb/BasicWeb.h>

#define CENTER_X(len, wid)  DISPLAY_WIDTH/2 - len*wid/2

/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/
T_uezDevice G_network;
T_uezNetworkStatus G_networkStatus;
T_glyphHandle G_glyphLCD;

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
	UEZGPIOOutput(GPIO_PD0);
	
    // Blink
    for (;;) {
        UEZGPIOSet(GPIO_PD0);
        UEZTaskDelay(250);
        UEZGPIOClear(GPIO_PD0);
        UEZTaskDelay(250);
    }

    return 0;
}

static void IDrawTitle(void)
{
    GlyphNormalScreen(G_glyphLCD);
    GlyphClearScreen(G_glyphLCD);
    GlyphSetFont(G_glyphLCD, GLYPH_FONT_5_BY_7);
    GlyphSetXY(G_glyphLCD, CENTER_X(20, 5u), DISPLAY_HEIGHT - 24);
    GlyphString(G_glyphLCD, "Future Designs, Inc.", 20);
    GlyphSetXY(G_glyphLCD, CENTER_X(12, 5u), DISPLAY_HEIGHT - 16);
    GlyphString(G_glyphLCD, "RX Solutions", 12);
    GlyphSetXY(G_glyphLCD, CENTER_X(15, 5u), DISPLAY_HEIGHT - 8);
    GlyphString(G_glyphLCD, "www.teamfdi.com", 15);
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
void SetupTasks(void)
{
    UEZTaskCreate(
            (T_uezTaskFunction)NetworkStartup,
            "NetStart",
            UEZ_TASK_STACK_BYTES(1024),
            (void *)0,
            UEZ_PRIORITY_NORMAL,
            0);
}

/*---------------------------------------------------------------------------*
 * Task:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      In the uEZ system, main() is a task.  Do not exit this task
 *      unless you want to the board to reset.  This function should
 *      setup the system and then run the main loop of the program.
 * Outputs:
 *      int                     -- Output error code
 *---------------------------------------------------------------------------*/
int MainTask(void)
{	
	T_uezDevice temp;
	T_uezDevice accel;
	T_uezError tempDeviceError;
	T_uezError accelDeviceError;
	TUInt8 displayString[100];
	TInt32 tempValue;
	float tempFloatValue;
	AccelerometerReading accelReading;
	
	printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner
	
	// Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);
	
    NVSettingsInit();
    // Load the settings from non-volatile memory
    if (NVSettingsLoad() == UEZ_ERROR_CHECKSUM_BAD) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }
	
	// Setup any additional misc. tasks (such as the heartbeat task)
    SetupTasks();

    GlyphOpen(&G_glyphLCD, 0);
	IDrawTitle();
	
	PlayAudio(523, 100);
    PlayAudio(659, 100);
    PlayAudio(783, 100);
    PlayAudio(1046, 100);
    PlayAudio(783, 100);
    PlayAudio(659, 100);
    PlayAudio(523, 100);	
	
	tempDeviceError = UEZTemperatureOpen("Temp0", &temp);
	if (tempDeviceError == UEZ_ERROR_NONE) {
		printf("MainTask: Temperature Device Open\n");
    } else {
        printf("MainTask: Failed to Open Temperature Device\n");
    }
	
	accelDeviceError = UEZAccelerometerOpen("Accel0", &accel);
	if (accelDeviceError == UEZ_ERROR_NONE) {
		printf("MainTask: Accelerometer Device Open\n");
    } else {
        printf("MainTask: Failed to Open Accelerometer Device\n");
    }
	
	while(1) {
		
		if ( tempDeviceError == UEZ_ERROR_NONE ) {
			if (UEZTemperatureRead(temp, &tempValue) == UEZ_ERROR_NONE) {
				tempFloatValue = tempValue/65536.0;
				sprintf((char *)displayString, "Temp: %.2f C", tempFloatValue);
				GlyphSetXY(G_glyphLCD, 0, 0);
    			GlyphString(G_glyphLCD, displayString, strlen(displayString));
            }
		}
		
		if ( accelDeviceError == UEZ_ERROR_NONE ) {
			if(UEZAccelerometerReadXYZ(accel, &accelReading, 10) == UEZ_ERROR_NONE )
			{				
				sprintf((char *)displayString, "ACCEL X: %d", accelReading.iX);
				GlyphSetXY(G_glyphLCD, 0, 16);
    			GlyphString(G_glyphLCD, displayString, strlen(displayString));
				
				sprintf((char *)displayString, "ACCEL Y: %d", accelReading.iY);
				GlyphSetXY(G_glyphLCD, 0, 24);
    			GlyphString(G_glyphLCD, displayString, strlen(displayString));
				
				sprintf((char *)displayString, "ACCEL Z: %d", accelReading.iZ);
				GlyphSetXY(G_glyphLCD, 0, 32);
    			GlyphString(G_glyphLCD, displayString, strlen(displayString));
			}
		}
		
		UEZTaskDelay(500);
	}

    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Network_Connect
 *---------------------------------------------------------------------------*
 * Description:
 *      When a system needs the network to be running, this routine is
 *      called.  Most of the work is already done in the UEZPlatform.
 *      This routine calls the correct connect routine and gets the
 *      network information.
 *---------------------------------------------------------------------------*/
T_uezError UEZPlatform_Network_Connect(void)
{
    T_uezError error = UEZ_ERROR_NOT_AVAILABLE;

    static TBool connected = EFalse;
    if (!connected) {
        connected = ETrue;
		error = UEZNetworkConnect(
            "WiredNetwork0",
            "lwIP",
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPAddr,
			(const T_uezNetworkAddr *)G_nonvolatileSettings.iIPMask,
			(const T_uezNetworkAddr *)G_nonvolatileSettings.iIPGateway,
            ETrue,
            &G_network,
            &G_networkStatus);
    }

    return error;
}

// Required Basic Web
void FuncTestPageHit(void)
{
}

T_uezError UEZPlatform_WiredNetwork0_Connect(
        T_uezDevice *aNetwork,
        T_uezNetworkStatus *aStatus)
{
    UEZPlatform_WiredNetwork0_Require();
    return UEZNetworkConnect(
            "WiredNetwork0",
            "lwIP",
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPAddr,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPMask,
            (const T_uezNetworkAddr *)G_nonvolatileSettings.iIPGateway,
            ETrue,
            aNetwork,
            aStatus);
}

/*---------------------------------------------------------------------------*
 * Routine:  uEZPlatformStartup
 *---------------------------------------------------------------------------*
 * Description:
 *      When uEZ starts, a special Startup task is created and called.
 *      This task brings up the all the hardware, reports any problems,
 *      starts the main task, and then exits.
 *---------------------------------------------------------------------------*/
TUInt32 uEZPlatformStartup(T_uezTask aMyTask, void *aParameters)
{
    extern T_uezTask G_mainTask;

	//UEZPlatform_Minimal_Require();
    //UEZPlatform_Standard_Require();  
    UEZPlatform_Full_Require();
	
	#if UEZ_ENABLE_USB_DEVICE_STACK
    // USB Flash drive needed?
    UEZPlatform_USBFlash_Drive_Require(0);
	#endif

	#if UEZ_ENABLE_USB_HOST_STACK
	    // USB Device needed?
	    UEZPlatform_USBDevice_Require();
	#endif

	#if UEZ_ENABLE_SDCARD
	    // SDCard needed?
	    UEZPlatform_SDCard_Drive_Require(1);
	#endif

	#if UEZ_ENABLE_TCPIP_STACK
	    // Network needed?
	    UEZPlatform_WiredNetwork0_Require();
	#endif

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}
