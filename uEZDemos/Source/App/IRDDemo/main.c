/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Example program that tests the LCD, Queue, Semaphore, and
 *      touchscreen features.
 *
 * Implementation:
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <uEZ.h>
#include <HAL/EMAC.h>
#include <uEZI2C.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "NVSettings.h"
#include <uEZADC.h>
#include <uEZTemperature.h>
#include "Device\ToneGenerator.h"
#include <Types/InputEvent.h>
#include <uEZToneGenerator.h>
#include <uEZKeypad.h>
#include <Source\RTOS\FreeRTOS\include\task.h>
#include <uEZLEDBank.h>
#include <uEZPlatform.h>

#if UEZ_BASIC_WEB_SERVER
    #include "Source/Library/Web/BasicWeb/BasicWEB.h"
#endif
#if UEZ_HTTP_SERVER
    #include "Source/Library/Web/HTTPServer/HTTPServer.h"
#endif

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#define BOOTLOADER_TAG 0

/*---------------------------------------------------------------------------*
 * Additional Includes:
 *---------------------------------------------------------------------------*/
#if COMPILE_OPTION_GENERIC_BULK
#include <Source/Library/USBDevice/Generic/Bulk/GenericBulk.h>
#endif
#if COMPILE_OPTION_USB_KEYBOARD
#include <Source/Library/USBDevice/Generic/HID/GenericHID.h>
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#if BOOTLOADER_TAG
    #if (COMPILER_TYPE==IAR)
        #pragma segment=".tag"
    const T_programTag G_programTag  = {
    #endif
    #if (COMPILER_TYPE==RowleyARM)
    const T_programTag G_programTag __attribute__((section(".tag"))) = {
    #endif
        PROGRAM_TAG_SPECIAL_HEADER,
        PROJECT_NAME " " TAG_BUILD_DATE,
        VERSION_AS_TEXT,
        RELEASE_DATE, // official release date
        0,  // Size reserved
        0,  // Checksum reserved
        "", // Reserved field
        PROGRAM_TAG_SPECIAL_TAIL
    };
    #if (COMPILER_TYPE==IAR)
        #pragma segment=".text"
    #endif
#endif

T_uezDevice G_tg;
T_uezDevice G_leds;
char G_ipAddress[40];

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void ProbeI2CBus(void);

/*---------------------------------------------------------------------------*
 * Routine:  ProbeI2CBus
 *---------------------------------------------------------------------------*
 * Description:
 *      Probe all the I2C peripherals on I2C0 and output to the console
 *      which ones are found.
 *---------------------------------------------------------------------------*/
void ProbeI2CBus(void)
{
    T_uezDevice i2c0;
    T_uezError error;
    TUInt8 addr;
    TUInt8 data[10];

    error = UEZI2COpen("I2C0", &i2c0);
    if (error)
        return;

    // Probe all 127 addresses
    printf("\nProbing all 127 I2C addresses on I2C0:\n");
    for (addr=1; addr<=127; addr++)  {
        // Give it 1 second each (way too much time)
        error = UEZI2CRead(i2c0, addr, 100, data, 0, 1000);
        if (error == UEZ_ERROR_NONE)
            printf("  Device @ 0x%02X\n", addr<<1);
    }

     UEZI2CClose(i2c0);
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup tasks that run besides main or the main menu.
 *---------------------------------------------------------------------------*/
#if UEZ_ENABLE_TCPIP_STACK
#include <FreeRTOS.h>
#endif

T_uezDevice G_remoteTempDev;
T_uezDevice G_localTempDev;


TUInt32 MonLEDs(T_uezTask aMyTask, void *aParams);
extern T_uezQueue G_monLEDsQueue;

#if UEZ_HTTP_SERVER
static T_uezError IMainHTTPServerGetValue(
            void *aHTTPState,
            const char *aVarName)
{

    TInt32 temperature;
    char buffer[20];

    if (strcmp(aVarName, "RefreshRate") == 0) {
        HTTPServerSetVar(aHTTPState, aVarName, "2");
    } else if (strcmp(aVarName, "TEMPATTACHED") == 0) {
        UEZTemperatureRead(G_remoteTempDev, &temperature);
        if (TEMPERATURE_INTEGER(temperature) == 127)
            HTTPServerSetVar(aHTTPState, aVarName, "Not connected");
        else
            HTTPServerSetVar(aHTTPState, aVarName, "Connected");
    } else if (strcmp(aVarName, "LOCAL_TEMPERATURE") == 0) {
        UEZTemperatureRead(G_localTempDev, &temperature);
        sprintf(buffer, "%c%d.%02u",
            (temperature>=0)?'+':'-',
            TEMPERATURE_INTEGER(temperature),
            TEMPERATURE_FRAC100(temperature));
        HTTPServerSetVar(aHTTPState, aVarName, buffer);
    } else if (strcmp(aVarName, "REMOTE_TEMPERATURE") == 0) {
        UEZTemperatureRead(G_remoteTempDev, &temperature);
        sprintf(buffer, "%c%d.%02u",
            (temperature>=0)?'+':'-',
            TEMPERATURE_INTEGER(temperature),
            TEMPERATURE_FRAC100(temperature));
        if (TEMPERATURE_INTEGER(temperature) >= 127)
            HTTPServerSetVar(aHTTPState, aVarName, "---.--");
        else
            HTTPServerSetVar(aHTTPState, aVarName, buffer);
    } else {
        HTTPServerSetVar(aHTTPState, aVarName, "Test");
    }

    return UEZ_ERROR_NONE;
}

static T_uezError IMainHTTPServerSetValue(
            void *aHTTPState,
            const char *aVarName,
            const char *aValue)
{
    extern void MonLEDsStopAutoRunning(void);
    static TUInt32 piezoFreqHz = 1000;

    if (strcmp(aVarName, "PIEZOFREQHTML") == 0) {
        // Remember the last know frequency
        piezoFreqHz = atoi(aValue);
    } else if (strcmp(aVarName, "PIEZODURHTML") == 0) {
        // Now play it
        if (G_tg)
            UEZToneGeneratorPlayTone(G_tg,
                TONE_GENERATOR_HZ(piezoFreqHz),
                atoi(aValue) * 1000);
    } else if (strcmp(aVarName, "SERLOOPMSG")==0) {
        // Just output to the console
        printf("%s\r\n", aValue);
    } else if (strcmp(aVarName, "LED1")==0) {
        MonLEDEvent(atoi(aValue)?MON_LED_EVENT_ON:MON_LED_EVENT_OFF, 0);
    } else if (strcmp(aVarName, "LED2")==0) {
        MonLEDEvent(atoi(aValue)?MON_LED_EVENT_ON:MON_LED_EVENT_OFF, 1);
    } else if (strcmp(aVarName, "LED3")==0) {
        MonLEDEvent(atoi(aValue)?MON_LED_EVENT_ON:MON_LED_EVENT_OFF, 2);
    } else if (strcmp(aVarName, "LED4")==0) {
        MonLEDEvent(atoi(aValue)?MON_LED_EVENT_ON:MON_LED_EVENT_OFF, 3);
    }
    return UEZ_ERROR_NONE;
}
static T_httpServerParameters G_httpServerParams = {
    IMainHTTPServerGetValue,
    IMainHTTPServerSetValue,
};
#endif

void SetupTasks(void)
{
#if UEZ_ENABLE_TCPIP_STACK
    T_uezError error;

#if UEZ_BASIC_WEB_SERVER
    error = BasicWebStart();
    if (error) {
        printf("Problem starting BasicWeb! (Error=%d)\n", error);
    } else {
        printf("BasicWeb started\n");
    }
#endif
#if UEZ_HTTP_SERVER
    // If stack is enabled, turn on web server
    /* Create the lwIP task.  This uses the lwIP RTOS abstraction layer.*/
    #define HTTP_SERVER_STACK_SIZE      UEZ_TASK_STACK_BYTES(1400)

    error = UEZTaskCreate(
        HTTPServer,
        "HTTPSvr",
        HTTP_SERVER_STACK_SIZE,
        (void *)&G_httpServerParams,
        UEZ_PRIORITY_NORMAL,
        0);
    if (error) {
        printf("Problem starting HTTPServer! (Error=%d)\n", error);
    } else {
        printf("HTTPServer started\n");
    }
#endif
#endif

    MonLEDsStart();
}

void *UEZEMACGetMACAddr(TUInt32 aUnitNumber)
{
    return G_nonvolatileSettings.iMACAddr;
}

void PlayTune(void)
{
    if (G_tg) {
        UEZToneGeneratorPlayTone(G_tg, TONE_GENERATOR_HZ(523.251), 100);
        UEZToneGeneratorPlayTone(G_tg, TONE_GENERATOR_HZ(659.255), 100);
        UEZToneGeneratorPlayTone(G_tg, TONE_GENERATOR_HZ(783.991), 100);
        UEZToneGeneratorPlayTone(G_tg, TONE_GENERATOR_HZ(1046.500), 100);
        UEZToneGeneratorPlayTone(G_tg, TONE_GENERATOR_HZ(783.991), 100);
        UEZToneGeneratorPlayTone(G_tg, TONE_GENERATOR_HZ(659.255), 100);
        UEZToneGeneratorPlayTone(G_tg, TONE_GENERATOR_HZ(523.251), 100);
        UEZToneGeneratorClose(G_tg);
    }
}

void MainDrawCharDisplay(T_uezDevice aDisplay, TInt32 aTemperature, TBool aShowBottom)
{
    char temp[8];
    char buffer[22];
    UEZCharDisplayDrawString(aDisplay, 0, 0, "NXP Semiconductors");
    sprintf(
        temp,
        "%c%u.%02u",
        (aTemperature >= 0)?'+':'-',
        TEMPERATURE_INTEGER(aTemperature),
        TEMPERATURE_FRAC100(aTemperature));
    sprintf(buffer, "10/10/12  %8s%cC", temp, 0xDF);
    UEZCharDisplayDrawString(aDisplay, 1, 0, buffer);
    UEZCharDisplayDrawString(aDisplay, 2, 0, "INDUSTRIAL PLATFORM ");
    if (aShowBottom)
        UEZCharDisplayDrawString(aDisplay, 3, 0, "uEZ / IRD " VERSION_AS_TEXT);
}

static void ShowButtonPressedOnCharDisplay(
        T_uezDevice aDisplay,
        T_uezInputEvent event)
{
    typedef struct { TUInt16 iKey; const char *iText; } T_keymsg;
    const T_keymsg keysMsgs[] = {
        { KEY_F1, "F1" },
        { KEY_F2, "F2" },
        { KEY_F3, "F3" },
        { KEY_F4, "F4" },
        { KEY_ARROW_LEFT, "ARROW LEFT" },
        { KEY_ARROW_UP, "ARROW UP" },
        { KEY_ARROW_RIGHT, "ARROW RIGHT" },
        { KEY_ARROW_DOWN, "ARROW DOWN" },
        { KEY_MENU, "MENU" },
        { KEY_END_ON_OFF, "END" },
        { KEY_SEND, "SEND" },
        { KEY_OK, "OK" },
        { KEY_CLEAR, "CLEAR" },
        { KEY_0, "0" },
        { KEY_1, "1" },
        { KEY_2, "2" },
        { KEY_3, "3" },
        { KEY_4, "4" },
        { KEY_5, "5" },
        { KEY_6, "6" },
        { KEY_7, "7" },
        { KEY_8, "8" },
        { KEY_9, "9" },
        { KEY_ASTERISK, "*" },
        { KEY_POUND, "#" },
        { KEY_PLUS, "+" },
        { KEY_MINUS, "-" },
        { KEY_ENTER, "ENTER" },
        { KEY_BACKSPACE, "BACKSPACE" },
        { 0, 0}
    };
    const T_keymsg *p_msg;

    UEZCharDisplayClearRow(aDisplay, 3, ' ');
    for (p_msg=keysMsgs; p_msg->iKey; p_msg++) {
        if (p_msg->iKey == event.iEvent.iButton.iKey) {
            UEZCharDisplayDrawString(aDisplay, 3, 0, p_msg->iText);
            printf("\nKey: %s\n", p_msg->iText);
            break;
        }
    }
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
 * Routine:  UEZPlatform_Network_Connect
 *---------------------------------------------------------------------------*
 * Description:
 *      When a system needs the network to be running, this routine is
 *      called.  Most of the work is already done in the UEZPlatform.
 *      This routine calls the correct connect routine and gets the
 *      network information.
 *---------------------------------------------------------------------------*/
T_uezDevice G_network;
T_uezNetworkStatus G_networkStatus;
T_uezError UEZPlatform_Network_Connect(void)
{
    static TBool connected = EFalse;
    T_uezError error = UEZ_ERROR_NOT_AVAILABLE;

    if (!connected) {
        connected = ETrue;
        error = UEZPlatform_WiredNetwork0_Connect(&G_network, &G_networkStatus);
    }
    return error;
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
    T_uezError error;
    T_uezDevice adcDev;
    T_uezDevice lcd;
    ADC_RequestSingle adcRequest;
    TUInt32 value;
    char buffer[1024];
    T_uezDevice keypad;
    T_uezInputEvent event;
#if COMPILE_OPTION_USB_KEYBOARD && UEZ_ENABLE_USB_DEVICE_STACK
	static T_GenHIDCallbacks genHIDCallbacks = {
		0
	};
#endif
#if COMPILE_OPTION_GENERIC_BULK && UEZ_ENABLE_USB_DEVICE_STACK
	static T_GenBulkCallbacks genBulkCallbacks = {
	    0,
	    0
	};
#endif

    printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

    // Start up the heart beat of the LED
    error = UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);
    if (error)
        return error;

    NVSettingsInit();
    // Load the settings from non-volatile memory
    if (NVSettingsLoad() == UEZ_ERROR_CHECKSUM_BAD) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

    // Simple test to find devices on the I2C Bus
    ProbeI2CBus();

#if COMPILE_OPTION_USB_KEYBOARD && UEZ_ENABLE_USB_DEVICE_STACK
    genHIDCallbacks.iGHIDEmptyOutput = 0;
    GenericHIDInitialize(&genHIDCallbacks);
#endif
#if COMPILE_OPTION_GENERIC_BULK && UEZ_ENABLE_USB_DEVICE_STACK
    GenericBulkInitialize(&genBulkCallbacks);
#endif


    // Setup any additional misc. tasks (such as the heartbeat task)
    SetupTasks();

    if (UEZCharDisplayOpen("CharDisplay", &lcd) == UEZ_ERROR_NONE) {
        UEZCharDisplayClearScreen(lcd);
        MainDrawCharDisplay(lcd, 0, ETrue);
    } else {
        lcd = 0;
    }

    UEZToneGeneratorOpen("Speaker", &G_tg);
    UEZLEDBankOpen("LEDBank0", &G_leds);
    PlayTune();
    UEZTaskDelay(3000);

    printf("Directory:\n");
    Main_ShowDirectory();
    printf("HTTPRoot Directory:\n");
    Main_ShowHTTPRootDirectory();

    // Output it followed by spin:
	printf("\fTask          State  Priority  Stack	#\n************************************************\n" );
	/* ... Then the list of tasks and their status... */
	vTaskList( ( signed portCHAR * ) buffer);	
//    puts(buffer);
    printf("%s", buffer);

    UEZADCOpen("ADC0", &adcDev);
    UEZTemperatureOpen("TempRemote", &G_remoteTempDev);
    UEZTemperatureOpen("TempLocal", &G_localTempDev);
    UEZKeypadOpen("Keypad", &keypad);
    while (1) {
        static char spin[4] = "-\\|/";
        static int spinstate = 0;
        static int updateTemp = 0;

        adcRequest.iADCChannel = 2;
        adcRequest.iBitSampleSize = 10;
        adcRequest.iTrigger = ADC_TRIGGER_NOW;
        adcRequest.iCapturedData = &value; // blank pattern
        value = 0xCCCC;

        // Do the request
        UEZADCRequestSingle(adcDev, &adcRequest);

        printf("ADC: %04X %c\r", (unsigned int)value, spin[((spinstate++) & 0x03)]);
//        fflush(stdout);

        // Sit here and do, well, nothing
        UEZTaskDelay(100);

        updateTemp++;
        if (updateTemp == 5) {
            TInt32 temperature;
            TInt32 total=0;
            TUInt32 i;
            for (i=0; i<16; i++)  {
                UEZTemperatureRead(G_remoteTempDev, &temperature);
                total += temperature;
            }
            temperature = total/16;
            MainDrawCharDisplay(lcd, temperature, EFalse);
            updateTemp = 0;

#if COMPILE_OPTION_GENERIC_BULK && UEZ_ENABLE_USB_DEVICE_STACK
            GenericBulkPut('.', 0);
#endif
        }

        // See if there is anything pressed on the keypad
        while (UEZKeypadRead(keypad, &event, 0) == UEZ_ERROR_NONE) {
            // Got some event
            ShowButtonPressedOnCharDisplay(lcd, event);
        }
    }
#if (COMPILER_TYPE != IAR)
    // We should not exit main unless we want to reset the board
    return 0;
#endif    
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

    UEZPlatform_Standard_Require();

#if UEZ_ENABLE_USB_DEVICE_STACK
    UEZPlatform_USBDevice_Require();
#endif

#if UEZ_ENABLE_USB_HOST_STACK
    // USB Host is required
    UEZPlatform_USBFlash_Drive_Require(0);
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

void FuncTestPageHit(void)
{
}

void UEZPlatformWriteChar(TUInt8 aChar)
{
    putchar(aChar);
}

/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
