/*-------------------------------------------------------------------------*
 * File:  FunctionalTest.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for ARM Carrier board with ARM7DIMM
 *
 * Implementation:
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

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <uEZTS.h>
#include <uEZFlash.h>
#include <uEZDeviceTable.h>
#include <Device/Stream.h>
#include <uEZStream.h>
#include <Types/Serial.h>
#include <Types/ADC.h>
#include <Types/LCD.h>
#include <Device/ADCBank.h>
#include <uEZFile.h>
#include <uEZRTOS.h>
#include "AppDemo.h"
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ProgramTag.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <HAL/GPIO.h>
#include <DEVICE/ADCBank.h>
#include <Device/Accelerometer.h>
#include <Device/ButtonBank.h>
#include <Device/Flash.h>
#include <Device/LEDBank.h>
#include <Device/Temperature.h>
#include <Device/RTC.h>
#include <Types/TimeDate.h>
#include <uEZProcessor.h>
#include <UEZPlatform.h>
#include <UEZLCD.h>
#include <NVSettings.h>

TBool G_webPageHit;

#if UEZ_ENABLE_LCD
void PlayWithBacklight(void);

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ICON_TEXT_COLOR         YELLOW
#define SELECT_ICON_COLOR       YELLOW

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
#define OPT_BUTTON_SKIP             0x0001
#define OPT_BUTTON_YES              0x0002
#define OPT_BUTTON_NO               0x0004
#define OPT_BUTTON_CANCEL           0x0008
#define OPT_BUTTON_OK               0x0010
#define OPT_INIT                    0x8000

#define TEST_RESULT_IN_PROGRESS          0x0000
#define TEST_RESULT_DONE                 0x0001
#define TEST_RESULT_PASS                 0x0002
#define TEST_RESULT_FAIL                 0x0003
#define TEST_RESULT_SKIP                 0x0004

typedef struct {
    SWIM_WINDOW_T iWin;
    TUInt32 iTestNum;
    TBool iNeedInit;
    TUInt16 iButtons;
    TUInt16 iResult;
    TUInt8 iData[100];
    TBool iGoNext;
    TBool iExit;
    TUInt16 iLine;
    TBool iAllPass;
    TBool iPauseComplete;
} T_testData;

typedef struct {
    void (*iAddButtons)(T_testData *aData, TUInt16 aButtonTypes);
    void (*iRemoveButtons)(T_testData *aData, TUInt16 aButtonTypes);
    void (*iTextLine)(T_testData *aData, TUInt16 aLine, const char *aText);
    void (*iShowResult)(T_testData *aData, TUInt16 aLine, TUInt16 aResult, TUInt32 aDelay);
    void (*iSetTestResult)(T_testData *aData, TUInt16 aResult);
    void (*iNextTest)(T_testData *aData);
} T_testAPI;

typedef struct {
    const char *iTitle;
    void (*iMonitorFunction)(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
    TUInt16 iLinesNeeded;
    TBool iPauseOnFail;
} T_testState;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void ITestPatternColors(T_testData *aData);
extern void Beep(void);
extern void BeepError(void);
extern void ButtonClick(void);
extern void PutPointRaw(TUInt8 aPage, int x, int y, TUInt16 pixel);
extern void DrawIcon(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
extern void IHidePage0(void);
extern void IShowPage0(void);
extern void PlayAudio(TUInt32 aHz, TUInt32 aMS);
extern void FunctionalTestScreen(T_testData *aData);

void IFuncTestSDRAM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestExternalRTC(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestInternalRTC(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestFlash(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestTemperature(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestSerial(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestCAN(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestEthernet(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestLCD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestUSBHost(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestMicroSDCard(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestAccelerometer(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestLEDAndButtons(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void IFuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestAccelerometerFast(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
#if COMPILE_OPTION_USB_KEYBOARD
void IFuncTestUSBDevice(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
#endif
void IFuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const T_testState G_testStates[] = {
#if UEZBSP_SDRAM
    { "SDRAM", IFuncTestSDRAM, 2, ETrue },            
#endif
    { "LCD", IFuncTestLCD, 10, EFalse },
    { "External RTC", IFuncTestExternalRTC, 1, ETrue },
    { "Internal RTC", IFuncTestInternalRTC, 1, ETrue },
    { "Flash", IFuncTestFlash, 1, ETrue },
    { "Temperature Sensor", IFuncTestTemperature, 1, ETrue },
    { "Serial", IFuncTestSerial, 1, ETrue },
	//{ "CAN", IFuncTestCAN, 1, ETrue },
    { "Micro SDCard", IFuncTestMicroSDCard, 3, ETrue },
    //{ "USB Host", IFuncTestUSBHost, 3, ETrue },
    //{ "Accelerometer", IFuncTestAccelerometer, 3, EFalse },
    { "Accelerometer", FuncTestAccelerometerFast, 1, EFalse },
    { "LED and Buttons", IFuncTestLEDAndButtons, 2, EFalse },
#if COMPILE_OPTION_USB_KEYBOARD
    //{ "USBDevice", IFuncTestUSBDevice, 2, EFalse },
#endif
    { "Speaker", IFuncTestSpeaker, 1, EFalse },
    { "Ethernet/HTTP", IFuncTestEthernet, 2, EFalse },
    { "Test Complete", IFuncTestComplete, 1000 },
    { 0 }
};
T_testData G_td;
TUInt8 G_results[50];

void IFuncTestExternalRTC(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        TUInt32 iStart;
        DEVICE_RTC **iRTC;
    } T_rtcTest;
    T_rtcTest *p = (T_rtcTest *)aData->iData;
    T_uezTimeDate td;

    if (aButton == OPT_INIT) {
        UEZDeviceTableFind("RTC", &p->iDevice);
        UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iRTC);
        td.iDate.iMonth = 1;
        td.iDate.iDay = 1;
        td.iDate.iYear = 2012;
        td.iTime.iHour = 8;
        td.iTime.iMinute = 0;
        td.iTime.iSecond = 0;
        // Set the time to 1/1/2009, 8:00:00
        (*p->iRTC)->Set(p->iRTC, &td);

        // Now wait a few seconds
        p->iStart = UEZTickCounterGet();

        aAPI->iTextLine(aData, 0, "Checking External RTC ...");
    } else if (aButton == 0) {
        // Show the current time
        // Has it been three seconds?
        if (UEZTickCounterGetDelta(p->iStart) >= 3000) {
            // Times up!
            // Read the RTC clock and make sure the time is what we
            // generally expected
            (*p->iRTC)->Get(p->iRTC, &td);
            if ((td.iDate.iMonth==1) &&
                    (td.iDate.iDay == 1) &&
                    (td.iDate.iYear == 2012) &&
                    (td.iTime.iHour == 8) &&
                    (td.iTime.iMinute == 0) &&
                    (td.iTime.iSecond > 1)) {
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            } else {
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            }
            aAPI->iNextTest(aData);
        }
    }
}

void IFuncTestInternalRTC(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        TUInt32 iStart;
        DEVICE_RTC **iRTC;
    } T_rtcTest;
    T_rtcTest *p = (T_rtcTest *)aData->iData;
    T_uezTimeDate td;

    if (aButton == OPT_INIT) {
        UEZDeviceTableFind("IRTC", &p->iDevice);
        UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iRTC);
        td.iDate.iMonth = 1;
        td.iDate.iDay = 1;
        td.iDate.iYear = 2012;
        td.iTime.iHour = 8;
        td.iTime.iMinute = 0;      
        td.iTime.iSecond = 0;
        // Set the time to 1/1/2012, 8:00:00
        (*p->iRTC)->Set(p->iRTC, &td);

        // Now wait a few seconds
        p->iStart = UEZTickCounterGet();    

        aAPI->iTextLine(aData, 0, "Checking Internal RTC ...");
    } else if (aButton == 0) {
        // Show the current time
        // Has it been three seconds?
        if (UEZTickCounterGetDelta(p->iStart) >= 3000) {
            // Times up!
            // Read the RTC clock and make sure the time is what we
            // generally expected
            (*p->iRTC)->Get(p->iRTC, &td);
            if ((td.iDate.iMonth==1) &&
                    (td.iDate.iDay == 1) &&
                    (td.iDate.iYear == 2012) &&
                    (td.iTime.iHour == 8) &&
                    (td.iTime.iMinute == 0) &&
                    (td.iTime.iSecond > 1)) {
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            } else {
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            }
            aAPI->iNextTest(aData);
        }
    }
}

void IFuncTestFlash(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
	TUInt8 readData[32];
    const TUInt32 flashOffset = 0x0;   
    T_uezDevice flashDevice;
    T_uezError error;

    static TUInt8 G_writeData[32] = {
        0xAA, 0xAA, 0x55, 0x55, 0x55, 0x01, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xAA, 0xAA,
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    };

    if (aButton == OPT_INIT) {
		
        aAPI->iTextLine(aData, 0, "Checking Data Flash ...");

		error = UEZFlashOpen("Flash0", &flashDevice);

		if (!error)
			error = UEZFlashChipErase(flashDevice);

		if (!error)
        	error = UEZFlashWrite(flashDevice, flashOffset, G_writeData, sizeof(G_writeData));
			
        if (error != UEZ_ERROR_NONE)  {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        } 
		
		UEZFlashClose(flashDevice);
		
    } else if (aButton == 0) {
		
		memset(readData, 0, sizeof(readData));
		
		error = UEZFlashOpen("Flash0", &flashDevice);

		if (!error)
        	error = UEZFlashRead(flashDevice, flashOffset, readData, sizeof(readData));
		
		if(memcmp(G_writeData, readData, 32)!=0) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        } else {
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        }

		UEZFlashClose(flashDevice);
		NVSettingsSave();
    }	
}

void IFuncTestTemperature(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_Temperature **iTemp;
        TUInt32 iCount;
        TInt32 iMinV;
        TInt32 iMaxV;
        char iLine[80];
    } T_tempTest;
    T_tempTest *p = (T_tempTest *)aData->iData;

    T_uezError error;
    TInt32 v;
    TUInt32 frac;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Temperature ...");

        error = UEZDeviceTableFind("Temp0", &p->iDevice);
        if (!error)
            error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iTemp);
        if (error) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }
        p->iMinV = 25<<16;
        p->iMaxV = 25<<16;
        p->iCount = 0;
    } else if (aButton == 0) {
        error = (*p->iTemp)->Read(p->iTemp, &v);
        if (error) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }
        if (v > p->iMaxV)
            p->iMaxV = v;
        if (v < p->iMinV)
            p->iMinV = v;
        // Get 3 bits of fractional part and multiply by 125 for
        // 0 --
        if (v < 0)
            frac = -v;
        else
            frac = v;
        frac = ((((TUInt32)(frac)) & 0xFFFF) >> 13) * 125;
        sprintf(p->iLine, "Checking Temperature ... %d.%03d C", v>>16, frac);
        aAPI->iTextLine(aData, 0, p->iLine);
        UEZTaskDelay(200);

        p->iCount++;
        if (p->iCount > 5) {
            // Done?  Is it in range
            if (p->iMinV < (15<<16)) {
                v = p->iMinV;
                if (v < 0)
                    frac = -v;
                else
                    frac = v;
                frac = ((((TUInt32)(frac)) & 0xFFFF) >> 13) * 125;
                sprintf(p->iLine, "Checking Temperature ... %d.%03d C (COLD!)", v>>16, frac);
                aAPI->iTextLine(aData, 0, p->iLine);

                // Too cold!
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            } else if (p->iMaxV > (35<<16)) {
                v = p->iMaxV;
                if (v < 0)
                    frac = -v;
                else
                    frac = v;
                frac = ((((TUInt32)(frac)) & 0xFFFF) >> 13) * 125;
                sprintf(p->iLine, "Checking Temperature ... %d.%03d C (HOT!)", v>>16, frac);
                aAPI->iTextLine(aData, 0, p->iLine);

                // Too hot!
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            } else {
                // Valid!
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            }
        }
    }
}

void IFuncTestSerial(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_STREAM **iStream;
        TUInt8 iReceived[5];
        TUInt32 iCount;
        TUInt32 iStart;
    } T_serialTest;
    T_serialTest *p = (T_serialTest *)aData->iData;
    T_uezError error;
    TUInt32 num;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Serial Port ...");

        error = UEZDeviceTableFind("Console", &p->iDevice);
        if (!error)
            error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iStream);
        if (error) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }
        p->iCount = 0;

        // Flush the input buffer
        while ((*p->iStream)->Read(p->iStream, p->iReceived, 5, &num, 500) != UEZ_ERROR_TIMEOUT) {

        }

        // Send out a 1 character string to the serial
        error = (*p->iStream)->Write(p->iStream, "|", 1, &num, 100);
        if (error) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }

        // Flush the input buffer
        while ((*p->iStream)->Read(p->iStream, p->iReceived, 5, &num, 500) != UEZ_ERROR_TIMEOUT) {

        }

        // Do again, send out a 1 character string to the serial
        error = (*p->iStream)->Write(p->iStream, "|", 1, &num, 100);
        if (error) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }

        p->iStart = UEZTickCounterGet();
    } else if (aButton == 0) {
        // Have we timed out?
        if (UEZTickCounterGetDelta(p->iStart) > 2000) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }
        num = 4 - p->iCount;
        (*p->iStream)->Read(p->iStream, p->iReceived + p->iCount, num, &num, 500);
        p->iCount += num;
        if (p->iCount >= 4) {
            // Should show the beginning of "J.Harris"
            if (memcmp(p->iReceived, "J.Ha", 4) == 0) {
                // Correct response
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                return;
            } else {
                // Incorrect response
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                return;
            }
        }
    }
}

void IFuncTestCAN(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_STREAM **iStream;
        TUInt32 iCount;
        TUInt32 iStart;
    } T_serialTest;
    T_serialTest *p = (T_serialTest *)aData->iData;
    unsigned int a, b;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking CAN Port ...");

        // Send a CAN message
        CANSend8(0x11223344, 0x55667788);

        // Setup a timeout period
        p->iStart = UEZTickCounterGet();
        p->iCount = 0;
    } else if (aButton == 0) {
        // Did we get a packet back?
        a = b = 0;
        if (CANReceive8(&a, &b)) {
            // Got something, is it right?
            if ((a==(0x11223344+0x01010101)) && (b==(0x55667788+0x01010101))) {
                // Matches!
                // We're done
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            }
        }

        // Have we timed out?
        if (UEZTickCounterGetDelta(p->iStart) > 2000) {
            p->iCount++;
            // Too many tries?
            if (p->iCount == 3) {
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                return;
            }
            // Send again
            CANSend8(0x11223344, 0x55667788);
            // Reset timeout
            p->iStart = UEZTickCounterGet();
        }
    }
}

#if 0
TUInt32 MemorySize(void)
{
    TUInt8 mem[100];
    TUInt8 prime;
    volatile TUInt8 *Base = (TUInt8 *)0xA0000000;
    volatile TUInt8 *Check = (TUInt8 *)0xA0010000;
    TUInt32 i;
    TUInt32 j;

    RTOS_ENTER_CRITICAL();

    while (1) {
        // Write a pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            mem[i] = Check[i];
            Check[i] = prime;
        }
        // Check the pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }
        // Changing base should not change the pattern here
        // (and thrash the cache)
        for (j=0; j<100; j++)  {
            for (i=0; i<1024; i++)
                Base[i] = 0xFF;
        }

        // Check the pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }

        // Replace the memory
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            Check[i] = mem[i];
        }
        Check += 0x10000;
    }
exit:
    RTOS_EXIT_CRITICAL();

    // Return size in bytes
    return Check-((TUInt8 *)0xA0000000);
}
#endif

#if UEZBSP_SDRAM
TUInt32 SDRAMMemoryTest(TUInt32 aSize)
{
    TUInt8 mem[100];
    TUInt8 prime;
    volatile TUInt8 *Base = (TUInt8 *)UEZBSP_SDRAM_BASE_ADDR;
    volatile TUInt8 *Check = (TUInt8 *)UEZBSP_SDRAM_BASE_ADDR+0x10000;
//volatile TUInt8 *Check = (TUInt8 *)UEZBSP_SDRAM_BASE_ADDR+0x400000;
    TUInt32 i;
    TUInt32 j;

    RTOS_ENTER_CRITICAL();

    while (Check < (Base + UEZBSP_SDRAM_SIZE)) {
        // Write a pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            mem[i] = Check[i];
            Check[i] = prime;
        }
        // Check the pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }
        // Changing base should not change the pattern here
        // (and thrash the cache)
        for (j=0; j<100; j++)  {
            for (i=0; i<1024; i++)
                Base[i] = 0xFF;
        }

        // Check the pattern
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }

        // Replace the memory
        for (i=0, prime=0x43; i<100; i++, prime+=7) {
            Check[i] = mem[i];
        }
        Check += 0x10000;
    }
exit:
    RTOS_EXIT_CRITICAL();

    // Return size in bytes
    return Check-Base;
}

void IFuncTestSDRAM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_sdramTest;
    T_sdramTest *p = (T_sdramTest *)aData->iData;
    TUInt32 size;
	T_uezDevice lcd;

    if (aButton == OPT_INIT) {
		
		UEZLCDOpen("LCD", &lcd);
		
        aAPI->iTextLine(aData, 0, "Checking SDRAM ...");

        // Just do it quickly and size it
		UEZLCDOff(lcd);   // Turn off LCD before test
        size = SDRAMMemoryTest(UEZBSP_SDRAM_SIZE);
		UEZLCDOn(lcd);	// Turn back on
		UEZLCDBacklight(lcd, 255);	// Turn up the LCD BL
        sprintf(p->iLine, "Checking SDRAM ... Size: %d bytes", size);
        aAPI->iTextLine(aData, 0, p->iLine);
//        aAPI->iTextLine(aData, 1, "  Is this the correct size?");
//        aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
        if (size == UEZBSP_SDRAM_SIZE) {
            aAPI->iTextLine(aData, 1, "  Correct size!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        } else {
            aAPI->iTextLine(aData, 1, "  **INCORRECT** size!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        }
#if 0
    } else if (aButton == OPT_BUTTON_YES) {
        aAPI->iTextLine(aData, 1, "  Correct size!");
        aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
    } else if (aButton == OPT_BUTTON_NO) {
        aAPI->iTextLine(aData, 1, "  **INCORRECT** size!");
        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
#endif
    }
}
#endif

void IFuncTestEthernet(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    char message[60];
    typedef struct {
        TUInt32 iStart;
    } T_serialTest;
    T_serialTest *p = (T_serialTest *)aData->iData;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Ethernet ...");
        sprintf(message, "Open web browser to http://%d.%d.%d.%d/",
                G_nonvolatileSettings.iIPAddr[0],
                G_nonvolatileSettings.iIPAddr[1],
                G_nonvolatileSettings.iIPAddr[2],
                G_nonvolatileSettings.iIPAddr[3]);
        aAPI->iTextLine(aData, 1, message);
        G_webPageHit = EFalse;
        p->iStart = UEZTickCounterGet();
    } else if (aButton == 0) {
        // Did we get hit?
        if (G_webPageHit) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            aAPI->iTextLine(aData, 1, "  Web browser page accessed!");
            return;
        }
    }
}

static void ITestPatternColors(T_testData *aData)
{
    int x, y;
    TUInt16 i=0;
    T_pixelColor c;

    for (y=1; y<32; y++)  {
        for (x=0; x<256; x++, i++)  {
            c = RGB(y*8-1, 0, 0);
            swim_set_pen_color(&aData->iWin, c /*(y<<10)+((x&128)?0x8000:0)*/);
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40);
        }
    }
    for (y=1; y<32; y++)  {
        c = RGB(0, y*8-1, 0);
        for (x=0; x<256; x++, i++)  {
            swim_set_pen_color(&aData->iWin, c /*(y<<5)+((x&128)?0x8000:0)*/);
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40+32);
        }
    }
    for (y=1; y<32; y++)  {
        c = RGB(0, 0, y*8-1);
        for (x=0; x<256; x++, i++)  {
            swim_set_pen_color(&aData->iWin, c /*(y<<0)+((x&128)?0x8000:0)*/);
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40+64);
        }
    }
}

void IFuncTestLCD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking LCD Colors ...");
        ITestPatternColors(aData);
        aAPI->iTextLine(aData, 9, "Do the red-green-blue appear in order and smooth?");
        aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
    } else if (aButton == OPT_BUTTON_YES) {
        aAPI->iShowResult(aData, 9, TEST_RESULT_PASS, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        return;
    } else if (aButton == OPT_BUTTON_NO) {
        aAPI->iShowResult(aData, 9, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        return;
    }
}

void IFuncTestUSBHost(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        TUInt32 iStart;
        T_uezFile iFile;
    } T_usbhostTest;
    T_usbhostTest *p = (T_usbhostTest *)aData->iData;
    T_uezError error;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking USB Host ... ");
        aAPI->iTextLine(aData, 1, "  Insert flash drive with TESTUSB.TXT file.");
        aAPI->iTextLine(aData, 2, "  Looking ...");

        // Read the tick counter
        p->iStart = UEZTickCounterGet();
    } else if (aButton == OPT_BUTTON_OK) {
        aAPI->iNextTest(aData);
    } else if (aButton == 0) {
        // Try once a second
        if (UEZTickCounterGetDelta(p->iStart) >= 1000) {
            p->iStart = UEZTickCounterGet();
            error = UEZFileOpen("0:TESTUSB.TXT", FILE_FLAG_READ_ONLY, &p->iFile);
            if (error == UEZ_ERROR_NONE) {
                aAPI->iTextLine(aData, 2, "  Looking ... Found!");
                UEZFileClose(p->iFile);
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            } else if (error == UEZ_ERROR_NOT_AVAILABLE)  {
                aAPI->iTextLine(aData, 2, "  Looking ... Not plugged in!");
            } else if (error == UEZ_ERROR_OUT_OF_MEMORY)  {
                aAPI->iTextLine(aData, 2, "  Looking ... Out of memory?!?");
            } else if (error == UEZ_ERROR_NOT_READY) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not ready!");
            } else if (error == UEZ_ERROR_NOT_FOUND) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not found!");
            } else if (error == UEZ_ERROR_NOT_AVAILABLE) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not available!");
            } else {
                aAPI->iTextLine(aData, 2, "  Looking ... Error!");
            }
        }
    }
}

void IFuncTestMicroSDCard(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        TUInt32 iStart;
        T_uezFile iFile;
    } T_usbhostTest;
    T_usbhostTest *p = (T_usbhostTest *)aData->iData;
    T_uezError error;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Micro SDCard ... ");
        aAPI->iTextLine(aData, 1, "  Insert Micro SDCard with TESTSDC.TXT file.");
        aAPI->iTextLine(aData, 2, "  Looking ...");

        // Read the tick counter
        p->iStart = UEZTickCounterGet();
    } else if (aButton == OPT_BUTTON_OK) {
        aAPI->iNextTest(aData);
    } else if (aButton == 0) {
        // Try once a second
        if (UEZTickCounterGetDelta(p->iStart) >= 1000) {
            p->iStart = UEZTickCounterGet();
            error = UEZFileOpen("1:TESTSDC.TXT", FILE_FLAG_READ_ONLY, &p->iFile);
            if (error == UEZ_ERROR_NONE) {
                aAPI->iTextLine(aData, 2, "  Looking ... Found!");
                UEZFileClose(p->iFile);
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            } else if (error == UEZ_ERROR_NOT_AVAILABLE)  {
                aAPI->iTextLine(aData, 2, "  Looking ... Not plugged in!");
            } else if (error == UEZ_ERROR_OUT_OF_MEMORY)  {
                aAPI->iTextLine(aData, 2, "  Looking ... Out of memory?!?");
            } else if (error == UEZ_ERROR_NOT_READY) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not ready!");
            } else if (error == UEZ_ERROR_NOT_FOUND) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not found!");
            } else if (error == UEZ_ERROR_NOT_AVAILABLE) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not available!");
            } else {
                aAPI->iTextLine(aData, 2, "  Looking ... Error!");
            }
        }
    }
}

void FuncTestAccelerometerFast(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_Accelerometer **iAccel;
        TUInt8 iSubstep;
        AccelerometerReading iReading;
        char iLine[80];
    } T_accelTest;
    T_accelTest *p = (T_accelTest *)aData->iData;
    T_uezError error;

    if (aButton == OPT_INIT) {
        p->iSubstep = 0;
        aAPI->iTextLine(aData, 0, "Checking Accelerometer ... ");
        error = UEZDeviceTableFind("Accel0", &p->iDevice);
        if (!error) {
            error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iAccel);
            if (!error) {
                error = (*p->iAccel)->ReadXYZ(p->iAccel, &p->iReading, 2500);
            }
        }
        if (error) {
            aAPI->iTextLine(aData, 0, "Checking Accelerometer ... Not found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        } else {
            aAPI->iTextLine(aData, 0, "Checking Accelerometer ... Found.");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        }
    }
}

void IFuncTestAccelerometer(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        T_uezDevice iDevice;
        DEVICE_Accelerometer **iAccel;
        TUInt8 iSubstep;
        AccelerometerReading iReading;
        char iLine[80];
    } T_accelTest;
    T_accelTest *p = (T_accelTest *)aData->iData;
    T_uezError error;
    TInt32 x;

    if (aButton == OPT_INIT) {
        p->iSubstep = 0;
        aAPI->iTextLine(aData, 0, "Checking Accelerometer ... ");
        error = UEZDeviceTableFind("Accel0", &p->iDevice);
        if (!error)
            error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iAccel);
        if (error) {
            aAPI->iTextLine(aData, 0, "Checking Accelerometer ... Not found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            return;
        }
    } else if (aButton == 0) {
        (*p->iAccel)->ReadXYZ(p->iAccel, &p->iReading, 250);
        x = p->iReading.iX>>10;
        if (p->iSubstep == 0) {
            sprintf(p->iLine, "  Tilt the board to the back (%d)", x);
            aAPI->iTextLine(aData, 1, p->iLine);
            // Looking for left tilt
            if (x < -6) {
                aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                p->iSubstep = 1;
            }
        } else {
            sprintf(p->iLine, "  Tilt the board to the front (%d)", x);
            aAPI->iTextLine(aData, 2, p->iLine);
            // Looking for right tilt
            if (x > 6) {
                aAPI->iShowResult(aData, 1, TEST_RESULT_PASS, 0);
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            }
        }
        UEZTaskDelay(20); // give it a chance to appear on the screen
    }
}

void IFuncTestLEDAndButtons(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        TUInt8 oldv;
        TUInt8 newv;
        TUInt8 bits;
        DEVICE_LEDBank **p_leds;
        T_uezDevice leds;
        DEVICE_ButtonBank **p_buttons;
        T_uezDevice buttons;
        TUInt8 pushed;
        TUInt8 iSubstep;
    } T_ledsButtonsTest;
    T_ledsButtonsTest *p = (T_ledsButtonsTest *)aData->iData;
    T_uezError error;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking LEDs and Buttons ... ");
        error = UEZDeviceTableFind("LEDBank0", &p->leds);
        if (!error)
            error = UEZDeviceTableGetWorkspace(p->leds, (T_uezDeviceWorkspace **)&p->p_leds);
        if (!error)
            error = UEZDeviceTableFind("ButtonBank0", &p->buttons);
        if (!error)
            error = UEZDeviceTableGetWorkspace(p->buttons, (T_uezDeviceWorkspace **)&p->p_buttons);
        if (error) {
            aAPI->iTextLine(aData, 0, "LEDs and Buttons ... Not Found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        }

        aAPI->iTextLine(aData, 1, "  Push all 4 buttons and turn on the LEDs");
        (*p->p_buttons)->SetActiveButtons(p->p_buttons, 0x0F);

        p->oldv = 0xFF;
        p->newv = 0xFF;
        p->pushed = 0;
        p->bits = 0;
        p->iSubstep = 0;
    } else if (aButton == 0) {
        TUInt32 butbits;
        TUInt8 v;

        (*p->p_leds)->On(p->p_leds, (p->bits & 0x0F)<<4);
        (*p->p_leds)->Off(p->p_leds, ((~p->bits) & 0x0F)<<4);
        (*p->p_buttons)->Read(p->p_buttons, &butbits);
        p->newv = butbits;
        v = ~p->newv & p->oldv;
        p->bits ^= v;
        p->oldv = p->newv;
        if (p->iSubstep == 0) {
            if ((p->bits & 0x0F) == 0x0F) {
                p->iSubstep = 1;
                aAPI->iTextLine(aData, 1, "  Are all 4 leds on?");
				aAPI->iTextLine(aData, 2, "  Is the heartbeat LED ON?");
                aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
            }
        }
    } else if (aButton == OPT_BUTTON_YES) {
        aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
    } else if (aButton == OPT_BUTTON_NO) {
        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
    }
}

void IFuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        TUInt8 iSubstep;
    } T_speakerTest;
    T_speakerTest *p = (T_speakerTest *)aData->iData;
    static const TUInt32 tones[8] = { 523, 659, 783, 1046, 783, 659, 523, 0 };

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Speaker ... ");
        aAPI->iTextLine(aData, 1, "  Do you hear the tones?");
        aAPI->iAddButtons(aData, OPT_BUTTON_YES|OPT_BUTTON_NO);
        p->iSubstep = 0;
    } else if (aButton == 0) {
        PlayAudio(tones[p->iSubstep], 100);
        p->iSubstep = (p->iSubstep+1)&7;
    } else if (aButton == OPT_BUTTON_YES) {
        aAPI->iTextLine(aData, 1, "  Tones are heard");
        aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
    } else if (aButton == OPT_BUTTON_NO) {
        aAPI->iTextLine(aData, 1, "  Tones are NOT heard");
        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
    }
}

#if COMPILE_OPTION_USB_KEYBOARD
void IFuncTestUSBDevice(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        TUInt8 iCaps;
    } T_usbdevTest;
    T_usbdevTest *p = (T_usbdevTest *)aData->iData;
    extern TBool G_capsOn;		

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking USB Device ... ");
        aAPI->iTextLine(aData, 1, "  Toggle caps on PC");
        p->iCaps = 0;
    } else if (aButton == 0) {
        extern void HIDKeyboardUpdate(void);
//        HIDKeyboardUpdate();
        if ((G_capsOn) && (p->iCaps == 0))  {
            p->iCaps = 1;
        } else if ((!G_capsOn) && (p->iCaps == 1))  {
            p->iCaps = 2;
        }
        if (p->iCaps == 2)  {
            aAPI->iTextLine(aData, 1, "  Caps toggled!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        }
    }
}
#endif

void IFuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    TUInt32 i=3;
    const T_testState *p;
    char *line = (char *)G_td.iData;
    TUInt32 c=0;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Functional Test is Complete");
        if (aData->iAllPass)
            aAPI->iTextLine(aData, 1, "  Final Result:  ^G** PASSES **^Y");
        else
            aAPI->iTextLine(aData, 1, "  Final Result:  ^R** FAILS **^Y");

        // Remove all buttons
        aAPI->iRemoveButtons(aData, 0xFFFF);
        // Put back on only the OK button
        aAPI->iAddButtons(aData, OPT_BUTTON_OK);

        if (!aData->iAllPass) {
            // Output failed
            aAPI->iTextLine(aData, i++, "Failed: ");
            strcpy(line, "    ");
            c=0;
            for (p=G_testStates; p->iMonitorFunction != IFuncTestComplete; p++) {
                if (G_results[p-G_testStates] == TEST_RESULT_FAIL) {
                    if (c)
                        strcat(line, ", ");
                    if (c == 4) {
                        aAPI->iTextLine(aData, i++, line);
                        strcpy(line, "    ");
                        c = 0;
                    }
                    strcat(line, p->iTitle);
                    c++;
                }
            }
            if (c) {
                aAPI->iTextLine(aData, i++, line);
                strcpy(line, "");
                c = 0;
            }

            // Output skipped
            aAPI->iTextLine(aData, i++, "Skipped: ");
            strcpy(line, "    ");
            c=0;
            for (p=G_testStates; p->iMonitorFunction != IFuncTestComplete; p++) {
                if (G_results[p-G_testStates] == TEST_RESULT_SKIP) {
                    if (c)
                        strcat(line, ", ");
                    if (c == 4) {
                        aAPI->iTextLine(aData, i++, line);
                        strcpy(line, "    ");
                        c = 0;
                    }
                    strcat(line, p->iTitle);
                    c++;
                }
            }
            if (c) {
                aAPI->iTextLine(aData, i++, line);
                strcpy(line, "");
                c = 0;
            }
        }
    } else if (aButton == OPT_BUTTON_OK) {
        aAPI->iNextTest(aData);
    }
}

static const struct { char *iText; T_pixelColor iColor; } G_textResults[] = {
    { "In Progress",    YELLOW, },
    { "Done",           GREEN,  },
    { "Pass",           GREEN,  },
    { "Fail",           RED,    },
    { "Skip",           YELLOW, },
};

#if UEZ_DEFAULT_LCD_RES_QVGA
    #define TEST_RESULT_LEFT_COLUMN         10
    #define TEST_RESULT_RIGHT_COLUMN        270
    #define TEST_RESULT_RIGHT_EDGE          310
    #define TEST_RESULT_LINE_HEIGHT         15
    #define TEST_RESULT_LINE_START          10
#endif

#if UEZ_DEFAULT_LCD_RES_VGA
    #define TEST_RESULT_LEFT_COLUMN         20
    #define TEST_RESULT_RIGHT_COLUMN        540
    #define TEST_RESULT_RIGHT_EDGE          620
    #define TEST_RESULT_LINE_HEIGHT         30
    #define TEST_RESULT_LINE_START          20
#endif

#if UEZ_DEFAULT_LCD_RES_WVGA
    #define TEST_RESULT_LEFT_COLUMN         20
    #define TEST_RESULT_RIGHT_COLUMN        540
    #define TEST_RESULT_RIGHT_EDGE          780
    #define TEST_RESULT_LINE_HEIGHT         30
    #define TEST_RESULT_LINE_START          20
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define TEST_RESULT_LEFT_COLUMN         10
    #define TEST_RESULT_RIGHT_COLUMN        270
    #define TEST_RESULT_RIGHT_EDGE          310
    #define TEST_RESULT_LINE_HEIGHT         15
    #define TEST_RESULT_LINE_START          10
#endif

#define TEST_COLOR_TEXT                 YELLOW
#define TEST_COLOR_BACKGROUND           BLACK
#define TEST_COLOR_BUTTON_EDGE          YELLOW
#define TEST_COLOR_BUTTON_CLICK         GREEN
#define TEST_RESULT_NUM_LINES           10

typedef struct {
    TUInt16 iBit;
    TUInt16 iX1, iY1, iX2, iY2;
    const char *iText;
} T_button;

#if UEZ_DEFAULT_LCD_RES_480x272
#define BUTTONS_LEFT_EDGE           10
#define BUTTONS_RIGHT_EDGE          305
#define BUTTONS_TOP                 170
#define BUTTONS_BOTTOM              210
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             10
#endif

#if UEZ_DEFAULT_LCD_RES_QVGA
#define BUTTONS_LEFT_EDGE           10
#define BUTTONS_RIGHT_EDGE          305
#define BUTTONS_TOP                 170
#define BUTTONS_BOTTOM              210
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             10
#endif

#if UEZ_DEFAULT_LCD_RES_VGA
#define BUTTONS_LEFT_EDGE           20
#define BUTTONS_RIGHT_EDGE          610
#define BUTTONS_TOP                 340
#define BUTTONS_BOTTOM              420
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             20
#endif

#if UEZ_DEFAULT_LCD_RES_WVGA
#define BUTTONS_LEFT_EDGE           20
#define BUTTONS_RIGHT_EDGE          770
#define BUTTONS_TOP                 340
#define BUTTONS_BOTTOM              420
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             20
#endif

#define BUTTON_WIDTH                ((BUTTONS_RIGHT_EDGE-BUTTONS_LEFT_EDGE-((BUTTONS_NUM-1)*BUTTONS_PADDING))/BUTTONS_NUM)
#define BUTTON_X_SPACING            (BUTTON_WIDTH+BUTTONS_PADDING)

#define BUTTON_LEFT(n)              ((BUTTON_X_SPACING*(n))+BUTTONS_LEFT_EDGE)
#define BUTTON_TOP(n)               (BUTTONS_TOP)
#define BUTTON_RIGHT(n)             (BUTTON_LEFT(n) + BUTTON_WIDTH - 1)
#define BUTTON_BOTTOM(n)            (BUTTONS_BOTTOM)

#define BUTTON_COORDS(n)            BUTTON_LEFT(n), BUTTON_TOP(n), BUTTON_RIGHT(n), BUTTON_BOTTOM(n)

static const T_button G_testButtons[] = {
    { OPT_BUTTON_CANCEL,    BUTTON_COORDS(0), "Cancel" },
    { OPT_BUTTON_YES,       BUTTON_COORDS(1), "Yes" },
    { OPT_BUTTON_NO,        BUTTON_COORDS(2), "No" },
    { OPT_BUTTON_SKIP,      BUTTON_COORDS(3), "Skip" },
    { OPT_BUTTON_OK,        BUTTON_COORDS(4), "OK" },
    { 0, /* ends here */ }
};

static TUInt16 ICalcY(TUInt16 aLine)
{
    return aLine * TEST_RESULT_LINE_HEIGHT + TEST_RESULT_LINE_START;
}

void TestDrawButtons(
        T_testData *aData,
        TUInt16 aButtons,
        T_pixelColor aBackground)
{
    const T_button *p_button;
    TUInt16 width, height;
    TUInt16 left, top, middle;
    const char *p;

    // Set the button colors
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BUTTON_EDGE);
    swim_set_fill_color(&aData->iWin, aBackground);
    swim_set_bkg_color(&aData->iWin, aBackground);

    // Draw each of the buttons that have appeared
    // Go through and remove buttons
    for (p_button=G_testButtons; p_button->iBit; p_button++)  {
        if (aButtons & p_button->iBit) {
            // Draw box button
            swim_set_fill_transparent(&G_td.iWin, 0);
            swim_put_box(
                &aData->iWin,
                p_button->iX1,
                p_button->iY1,
                p_button->iX2,
                p_button->iY2);

            // Now draw the text in it
            swim_set_fill_transparent(&aData->iWin, 1);

            // Calculate width of text
            width = 0;
            for (p=p_button->iText; *p; p++)
                width += swim_get_font_char_width(&aData->iWin, *p);

            // Cut width in half
            width >>= 1;

            // Now draw centered below.
            swim_set_pen_color(&aData->iWin, ICON_TEXT_COLOR);
            middle = (p_button->iX1 + p_button->iX2-1)>>1;
            left = middle-width;
            height = swim_get_font_height(&aData->iWin);
            top = ((p_button->iY1 + p_button->iY2-1)-height)>>1;
            swim_put_text_xy(&aData->iWin, p_button->iText, left, top);
        }
    }
}

void TestButtonClick(T_testData *aData, TUInt16 aButton)
{
    TestDrawButtons(aData, aButton, TEST_COLOR_BUTTON_CLICK);
    ButtonClick();
    UEZTaskDelay(100);
    TestDrawButtons(aData, aButton, TEST_COLOR_BACKGROUND);
}

void TestAddButtons(T_testData *aData, TUInt16 aButtonTypes)
{
    TUInt16 newButtons = aButtonTypes ^ aData->iButtons;

    TestDrawButtons(aData, newButtons, TEST_COLOR_BACKGROUND);

    aData->iButtons |= aButtonTypes;
}

void TestRemoveButtons(T_testData *aData, TUInt16 aButtonTypes)
{
    const T_button *p_button;
    TUInt16 buttons = aData->iButtons & aButtonTypes;

    // Erase the buttons
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_bkg_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_transparent(&G_td.iWin, 0);

    // Go through and remove buttons
    for (p_button=G_testButtons; p_button->iBit; p_button++)  {
        if (buttons & p_button->iBit) {
            swim_put_box(
                &aData->iWin,
                p_button->iX1,
                p_button->iY1,
                p_button->iX2,
                p_button->iY2);
        }
    }

    // Remove those buttons from the logic
    aData->iButtons &= ~aButtonTypes;
}

void TestTextLine(T_testData *aData, TUInt16 aLine, const char *aText)
{
    TUInt16 y = ICalcY(aLine+aData->iLine);
    const char *p_text;
    int mode = 0;

    // Draw a black rectangle over the area
    swim_set_pen_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_bkg_color(&aData->iWin, TEST_COLOR_BACKGROUND);
    swim_set_fill_transparent(&G_td.iWin, 0);

    // Draw the black box for the background
    swim_put_box(
        &aData->iWin,
        TEST_RESULT_LEFT_COLUMN,
        y,
        TEST_RESULT_RIGHT_EDGE-1,
        y+TEST_RESULT_LINE_HEIGHT-1);

    if (aText) {
        // Now draw the text that goes here
        swim_set_pen_color(&aData->iWin, TEST_COLOR_TEXT);
#if 0
        swim_put_text_xy(
            &aData->iWin,
            aText,
            TEST_RESULT_LEFT_COLUMN,
            y);
#else
        swim_set_xy(&aData->iWin, TEST_RESULT_LEFT_COLUMN, y);
        p_text = aText;
        while (*p_text) {
            if (mode == 0) {
                if (*p_text == '^')
                    mode = 1;
                else
                    swim_put_char(&aData->iWin, *p_text);
            } else {
                if (*p_text == 'G')
                    swim_set_pen_color(&aData->iWin, GREEN);
                else if (*p_text == 'R')
                    swim_set_pen_color(&aData->iWin, RED);
                else if (*p_text == 'Y')
                    swim_set_pen_color(&aData->iWin, YELLOW);
                else
                    swim_set_pen_color(&aData->iWin, TEST_COLOR_TEXT);
                mode = 0;
            }
            p_text++;
        }
#endif
    }
}

void TestShowResult(T_testData *aData, TUInt16 aLine, TUInt16 aResult, TUInt32 aDelay)
{
    // Draw the result on the screen
    swim_set_pen_color(&aData->iWin, G_textResults[aResult].iColor);
    swim_put_text_xy(
        &aData->iWin,
        G_textResults[aResult].iText,
        TEST_RESULT_RIGHT_COLUMN,
        ICalcY(aLine+aData->iLine));

    // Show for a moment, then go on
    UEZTaskDelay(aDelay);
}

void TestSetTestResult(T_testData *aData, TUInt16 aPass)
{
    aData->iResult = aPass;
    if ((aPass == TEST_RESULT_FAIL)||(aPass == TEST_RESULT_SKIP))  {
        // If any test is failed or skipped, not an 'all pass'
        G_td.iAllPass = EFalse;
    }
    if ((aPass == TEST_RESULT_PASS)||(aPass == TEST_RESULT_FAIL)||(aPass == TEST_RESULT_SKIP))
        aData->iGoNext = ETrue;
}

void TestNext(T_testData *aData)
{
    aData->iGoNext = ETrue;
}

const T_testAPI G_testAPI = {
    TestAddButtons,
    TestRemoveButtons,
    TestTextLine,
    TestShowResult,
    TestSetTestResult,
    TestNext,
};

#if(UEZ_PROCESSOR==RENESAS_RX63N)
extern TBool G_TriLED;
#endif

/*---------------------------------------------------------------------------*
 * Task:  FunctionalTest
 *---------------------------------------------------------------------------*
 * Description:
 *      This tasks draws a crosshairs and target each time the user
 *      touches the screen.
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 *---------------------------------------------------------------------------*/
void FunctionalTest(const T_choice *aChoice)
{
    T_uezTSReading reading;
    T_uezDevice ts;
    T_uezDevice lcd;
    TUInt32 lastX=0, lastY=0;
    T_pixelColor *pixels;
    T_uezQueue queue;
    INT_32 winX, winY;
    const T_button *p_button;
    const T_button *p_lastButton = 0;
    TUInt16 x, y;
    const T_testState *testState = &G_testStates[0];
    const T_testState *lastTestState = 0;
    TBool isCancelled = EFalse;
    TBool isPausing = EFalse;

#if(UEZ_PROCESSOR==RENESAS_RX63N)
	G_TriLED = ETrue;
#endif

    // Start with the first test
    testState = G_testStates;

    memset(&G_td, 0, sizeof(G_td));

    // All tests pass until otherwise noted
    G_td.iAllPass = ETrue;
    G_td.iPauseComplete = EFalse;

    // Setup queue to receive touchscreen events
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &queue) == UEZ_ERROR_NONE) {
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                UEZLCDGetFrame(lcd, 0, (void **)&pixels);

                // Clear the screen
//                IFillRect(pixels, 0, 0, RESOLUTION_X, RESOLUTION_Y, 0);
                FunctionalTestScreen(&G_td);

                for (;(testState->iTitle) && !isCancelled;) {
                    // New test state?
                    if (testState != lastTestState) {
                        // Remove all buttons but Skip and Cancel
                        TestRemoveButtons(&G_td, 0xFFFF&~(OPT_BUTTON_SKIP|OPT_BUTTON_CANCEL));
                        TestAddButtons(&G_td, OPT_BUTTON_SKIP|OPT_BUTTON_CANCEL);
                        testState->iMonitorFunction(&G_testAPI, &G_td, OPT_INIT);
                        lastTestState = testState;
                    }

                    // Wait forever until we receive a touchscreen event
                    // NOTE: UEZTSGetReading() can also be used, but it doesn't wait.
                    // Check every 50 ms
                    if (UEZQueueReceive(queue, &reading, 10)==UEZ_ERROR_NONE) {
                        winX = reading.iX;
                        winY = reading.iY;
                        swim_get_virtual_xy(&G_td.iWin, &winX, &winY);

                        // Is this a touching event?
                        if (reading.iFlags & TSFLAG_PEN_DOWN)  {
                            // We are touching the screen.
                            // Is this a different position than before?
                            if ((reading.iX != lastX) || (reading.iY != lastY))  {
                                x = reading.iX;
                                y = reading.iY;
                                // Determine which choice we are in
                                for (p_button=G_testButtons; p_button->iBit; p_button++)  {
                                    if ((x >= p_button->iX1) && (x <= p_button->iX2) &&
                                            (y >= p_button->iY1) && (y <= p_button->iY2)) {
                                        break;
                                    }
                                }
                                if (p_button != p_lastButton) {
                                    // See if we got a button
                                    if (p_button->iBit & G_td.iButtons) {
                                        // Click it
                                        TestButtonClick(&G_td, p_button->iBit);

                                        // Now process the command
                                        if (isPausing) {
                                            if (p_button->iBit & OPT_BUTTON_OK) {
                                                // Clicked OK, just go on, already have a failure
                                                G_td.iGoNext = ETrue;
                                                G_td.iPauseComplete = ETrue;
                                            }
                                        } else {
                                            testState->iMonitorFunction(&G_testAPI, &G_td, p_button->iBit);
                                            if (p_button->iBit & OPT_BUTTON_SKIP) {
                                                G_td.iGoNext = ETrue;
                                                G_testAPI.iShowResult(&G_td, 0, TEST_RESULT_SKIP, 250);
                                                G_testAPI.iSetTestResult(&G_td, TEST_RESULT_SKIP);

                                                // If any test is skipped, does not pass
                                                G_td.iAllPass = EFalse;
                                            }
                                        }
                                        if (p_button->iBit & OPT_BUTTON_CANCEL) {
                                            // If any test is cancelled, does not pass
                                            G_td.iAllPass = EFalse;
                                            isCancelled = ETrue;
                                        }
                                    }
                                }
                                p_lastButton = p_button;
                            }
                        } else {
                            p_lastButton = 0;
                        }
                    }

                    // Request to go on?
                    if (G_td.iGoNext) {
                        // Did we fail?  And do we need to pause?
                        if ((G_td.iResult == TEST_RESULT_FAIL) && (testState->iPauseOnFail)) {
                            // Did we pause?
                            if (!G_td.iPauseComplete) {
                                // Have not paused, put up the OK and Cancel buttons
                                TestRemoveButtons(&G_td, 0xFFFF&~(OPT_BUTTON_CANCEL));
                                TestAddButtons(&G_td, OPT_BUTTON_OK|OPT_BUTTON_CANCEL);

                                // We're not done yet
                                G_td.iGoNext = EFalse;

                                // And we're in pause mode
                                isPausing = ETrue;

                                // Play negative tone
                                BeepError();
                            } else {
                                // Done pausing, reset everything
                                G_td.iPauseComplete = EFalse;
                                isPausing = EFalse;
                            }
                        }
                    }
                    // Still going on?
                    if (G_td.iGoNext) {
                        // Record the result
                        G_results[testState-G_testStates] = G_td.iResult;

                        // Move to the next lines
                        G_td.iLine+=testState->iLinesNeeded;
                        testState++;
                        if ((G_td.iLine + testState->iLinesNeeded) >= TEST_RESULT_NUM_LINES) {
                            TUInt32 i;
                            // Back to the start
                            G_td.iLine = 0;
                            // Clear all the lines
                            for (i=0; i<TEST_RESULT_NUM_LINES; i++)
                                G_testAPI.iTextLine(&G_td, i, 0);
                        }
                        G_td.iGoNext = EFalse;
                    } else {
                        // Only monitor if we are not pausing
                        if (!isPausing)
                            testState->iMonitorFunction(&G_testAPI, &G_td, 0);
                    }
                }
                UEZLCDClose(lcd);
            }
            UEZTSClose(ts, queue);
        }
        UEZQueueDelete(queue);
    }
	
	#if(UEZ_PROCESSOR==RENESAS_RX63N)
	G_TriLED = EFalse;
	#endif

}

void FunctionalTestScreen(T_testData *aData)
{
    T_uezDevice lcd;
    T_pixelColor *pixels;

    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDOn(lcd);
        UEZLCDBacklight(lcd, 256);

        SUIHidePage0();

        UEZLCDGetFrame(lcd, 0, (void **)&pixels);
        swim_window_open(
            &aData->iWin,
            DISPLAY_WIDTH,
            DISPLAY_HEIGHT,
            pixels,
            0,
            0,
            DISPLAY_WIDTH-1,
            DISPLAY_HEIGHT-1,
            2,
            YELLOW,
            RGB(0, 0, 0),
            RED);
        swim_set_font(&aData->iWin, &APP_DEMO_DEFAULT_FONT);
        swim_set_title(&aData->iWin, "Functional Test", BLUE);

        SUIShowPage0();
    }
}
#endif

void FuncTestPageHit(void)
{
    G_webPageHit = ETrue;
}

/*-------------------------------------------------------------------------*
 * File:  FunctionalTest.c
 *-------------------------------------------------------------------------*/
