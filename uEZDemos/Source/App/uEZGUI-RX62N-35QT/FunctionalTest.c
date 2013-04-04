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
#include <uEZTS.h>
#include <uEZDeviceTable.h>
#include <Device/Stream.h>
#include <uEZStream.h>
#include <Types/Serial.h>
#include <Types/ADC.h>
#include <Types/LCD.h>
#include <Device/ADCBank.h>
//#include <uEZSSP.h>
#include <uEZFile.h>
#include <uEZRTOS.h>
#include <uEZFlash.h>
#include <uEZADC.h>
#include "FunctionalTest.h"
#include "AppDemo.h"
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include "FreeRTOS.h"
#include "task.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
//#include <uEZ_GUI.h>

#include <HAL/GPIO.h>
#include <DEVICE/ADCBank.h>
#include <Device/Accelerometer.h>
#include <Device/ButtonBank.h>
#include <Device/EEPROM.h>
#include <Device/LEDBank.h>
#include <Device/Temperature.h>
#include <Device/RTC.h>
#include <Types/TimeDate.h>

#include <uEZPlatform.h>
#include <uEZProcessor.h>

#if UEZ_ENABLE_LCD
void PlayWithBacklight(void);

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_8_BIT)
    #define RGB(r, g, b) \
        ((((r>>5)&7)<<5)| \
        (((g>>5)&7)<<2)| \
        (((b>>6)&3)<<0))
#endif
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_16_BIT)
    #define RGB(r, g, b)      \
        ( (((r>>3)&0x1F)<<11)| \
          (((g>>2)&0x3F)<<5)| \
          (((b>>3)&0x1F)<<0) )
#endif
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_I15_BIT)
    #define RGB(r, g, b)      \
        ( (((r>>3)&0x1F)<<10)| \
          (((g>>3)&0x1F)<<5)| \
          (((b>>3)&0x1F)<<0) )
#endif
#define ICON_TEXT_COLOR         YELLOW
#define SELECT_ICON_COLOR       YELLOW

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

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
extern TBool UEZGUIIsLoopbackBoardConnected();

//extern T_uezError USBHost_SwitchPort(void *aWorkspace, TUInt32 aPort);

void FuncTestSDRAM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestEEPROM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestSerial(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestLCD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestBacklightMonitor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestVoltageMonitor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestGPIOs(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestMicroSDCard(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const T_testState G_testStates[] = {
	//{ "GPIOs", FuncTestGPIOs, 3, ETrue, EFalse, ETrue },
    { "Speaker", FuncTestSpeaker, 1, EFalse, EFalse, EFalse },
    { "LCD", FuncTestLCD, 10, EFalse, EFalse, EFalse },
    { "SDRAM", FuncTestSDRAM, 2, ETrue, EFalse, EFalse },
    { "Micro SDCard", FuncTestMicroSDCard, 3, ETrue, EFalse, EFalse },
    //{ "Serial", FuncTestSerial, 1, ETrue },
    { "Test Complete", FuncTestComplete, 1000, EFalse, EFalse },
    { 0 }
};
T_testData G_td;
TUInt8 G_results[50];

void FuncTestMicroSDCard(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        TUInt32 iStart;
        T_uezFile iFile;
        TUInt32 iNumRetries;
    } T_usbhostTest;
    T_usbhostTest *p = (T_usbhostTest *)aData->iData;
    T_uezError error;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking Micro SDCard ... ");
        aAPI->iTextLine(aData, 1, "  Insert Micro SDCard with SLIDES/SLIDES.TXT file.");
        aAPI->iTextLine(aData, 2, "  Looking ...");

        // Read the tick counter
        p->iStart = UEZTickCounterGet();
        p->iNumRetries = 0;
    } else if (aButton == OPT_BUTTON_OK) {
        aAPI->iNextTest(aData);
    } else if (aButton == 0) {
        // Try once a second
        if (UEZTickCounterGetDelta(p->iStart) >= 1000) {
            p->iStart = UEZTickCounterGet();
            error = UEZFileOpen("1:SLIDES/SLIDES.TXT", FILE_FLAG_READ_ONLY, &p->iFile);
            if (error == UEZ_ERROR_NONE) {
                aAPI->iTextLine(aData, 2, "  Looking ... Found!");
                UEZFileClose(p->iFile);
                aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                sprintf(aData->iResultValue, "OK");
            } else if (error == UEZ_ERROR_NOT_AVAILABLE)  {
                aAPI->iTextLine(aData, 2, "  Looking ... Not plugged in!");
                sprintf(aData->iResultValue, "Not plugged in");
            } else if (error == UEZ_ERROR_OUT_OF_MEMORY)  {
                aAPI->iTextLine(aData, 2, "  Looking ... Out of memory?!?");
                sprintf(aData->iResultValue, "Internal Error");
            } else if (error == UEZ_ERROR_NOT_READY) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not ready!");
                sprintf(aData->iResultValue, "Not Ready");
            } else if (error == UEZ_ERROR_NOT_FOUND) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not found!");
                sprintf(aData->iResultValue, "Not Found");
            } else if (error == UEZ_ERROR_NOT_AVAILABLE) {
                aAPI->iTextLine(aData, 2, "  Looking ... Not available!");
                sprintf(aData->iResultValue, "Not available");
            } else if (error == UEZ_ERROR_READ_WRITE_ERROR) {
                aAPI->iTextLine(aData, 2, "  Looking ... Error!");
                sprintf(aData->iResultValue, "Read/Write Error");
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            } else {
                aAPI->iTextLine(aData, 2, "  Looking ... Error!");
                sprintf(aData->iResultValue, "No SLIDES.TXT");
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            }
            if (error != UEZ_ERROR_NONE) {
                p->iNumRetries++;
                if (p->iNumRetries >= 15) {
                    aAPI->iTextLine(aData, 2, "  Looking ... Timeout!");
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                    sprintf(aData->iResultValue, "Timeout");
                }
            }
        }
    }
}

void FuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
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
        sprintf(aData->iResultValue, "OK");
    } else if (aButton == OPT_BUTTON_NO) {
        aAPI->iTextLine(aData, 1, "  Tones are NOT heard");
        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        sprintf(aData->iResultValue, "Not heard");
    }
}

void FuncTestSerial(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
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

        p->iReceived[0] = '.';

        // Do again, send out a 4 character string to the serial
        error = (*p->iStream)->Write(p->iStream, "Test", 4, &num, 100);
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
            if (memcmp(p->iReceived, "Test", 4) == 0) {
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
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
            mem[i] = Check[i];
            Check[i] = prime;
        }
        // Check the pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
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
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
            if (Check[i] != prime)
                goto exit;
        }

        // Replace the memory
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<100; i++, prime+=7) {
            Check[i] = mem[i];
        }
        Check += 0x10000;
    }
exit:
    RTOS_EXIT_CRITICAL();

    // Return size in bytes
    return Check-((TUInt8 *)0xA0000000);
}

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

// This version messes up the screen to ensure uniqueness across the whole memory
TUInt32 SDRAMMemoryTestExtensive(TUInt32 aSize)
{
    TUInt32 prime;
    volatile TUInt32 *Base = (TUInt32 *)UEZBSP_SDRAM_BASE_ADDR;
    volatile TUInt32 *Check = (TUInt32 *)UEZBSP_SDRAM_BASE_ADDR+0x10000;
    TUInt32 i;
    TUInt32 j;
    const TUInt32 checkSize = 0x10000/4;

    RTOS_ENTER_CRITICAL();

     // Fill the memory with a mostly unique pattern
    Check = (TUInt32 *)(UEZBSP_SDRAM_BASE_ADDR+0x10000);
    while (Check < (Base + UEZBSP_SDRAM_SIZE/4)) {
        // Write a pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<checkSize; i++, prime+=7) {
            Check[i] = (TUInt32)(Check+i);
        }
        Check += 0x10000/4;
    }
    // Changing base should not change the pattern here
    // (and thrash the cache)
    for (j=0; j<10; j++)  {
        for (i=0; i<checkSize; i++)
            Base[i] = 0xFFFFFFFF;
    }

    Check = (TUInt32 *)(UEZBSP_SDRAM_BASE_ADDR+0x10000);
    while (Check < (Base + UEZBSP_SDRAM_SIZE/4)) {
        // Check the pattern
        for (i=0, prime=0x43+(((TUInt32)Check)>>16); i<checkSize; i++, prime+=7) {
            if (Check[i] != ((TUInt32)(Check+i)))
                goto exit;
        }
        Check += 0x10000/4;
    }

exit:
    RTOS_EXIT_CRITICAL();

    // Return size in bytes
    return Check-Base;
}

void FuncTestSDRAM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_sdramTest;
    T_sdramTest *p = (T_sdramTest *)aData->iData;
    TUInt32 size;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking SDRAM ...");

        // Just do it quickly and size it
		if(aData->iParams[0] == 0)
			size = SDRAMMemoryTest(UEZBSP_SDRAM_SIZE);
		else if(aData->iParams[0] == 1)
			size = SDRAMMemoryTestExtensive(UEZBSP_SDRAM_SIZE);
		else
			printf("FAIL: First parameter must be 0 (fast) or 1 (extensive)\n");

        sprintf(p->iLine, "Checking SDRAM ... Size: %d bytes", size);
        aAPI->iTextLine(aData, 0, p->iLine);

        if (size == UEZBSP_SDRAM_SIZE) {
            aAPI->iTextLine(aData, 1, "  Correct size!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
            sprintf(aData->iResultValue, "0x%08X", size);
        } else {
            aAPI->iTextLine(aData, 1, "  **INCORRECT** size!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "0x%08X", size);
        }
    }
}  

static void ITestPatternColors(T_testData *aData)
{
    int x, y;
    TUInt16 i=0;

    for (y=1; y<32; y++)  {
        swim_set_pen_color(&aData->iWin, (y<<11));
	    for (x=0; x<256; x++, i++)  {
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40);
        }
    }
    for (y=1; y<32; y++)  {
        swim_set_pen_color(&aData->iWin, ((y*2)<<5));
	    for (x=0; x<256; x++, i++)  {
            swim_put_pixel(&aData->iWin,
                    x+20,
                    y+40+32);
        }
    }
    for (y=1; y<32; y++)  {
        swim_set_pen_color(&aData->iWin, (y<<0));
	    for (x=0; x<256; x++, i++)  {
            swim_put_pixel(&aData->iWin, 
                    x+20,
                    y+40+64);
        }
    }
}

void FuncTestLCD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
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

void FuncTestBacklightMonitor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_backlightTest;
    T_backlightTest *p = (T_backlightTest *)aData->iData;
    T_uezDevice adc;
    T_uezError error;
    ADC_RequestSingle r;
    TUInt32 reading;
    TUInt32 percent1;
    TUInt32 percent2;
    TUInt32 percent;
    T_uezDevice lcd;
    TUInt32 i;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Backlight Monitor ...");
    UEZLCDOpen("LCD", &lcd);

        // Just do it quickly
        error = UEZADCOpen("ADC0", &adc);
        if (error) {
            aAPI->iTextLine(aData, 0, "Backlight Monitor ADC not Found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Backlight Monitor ADC not Found!");
        } else {
            // Take a reading
            r.iADCChannel = 0;
            r.iBitSampleSize = 10;
            r.iTrigger = ADC_TRIGGER_NOW;
            r.iCapturedData = &reading;
            error = UEZADCRequestSingle(adc, &r);
            if (error) {
                aAPI->iTextLine(aData, 0, "Backlight Monitor ADC bad reading!");
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "No Reading");
            } else {
                // TBD: Is 0x180 the mid point of the expected range?
                percent = reading * 100 / 0x180;
                percent1 = percent;
                percent2 = 0;
                sprintf(p->iLine, "Backlight Monitor (on: %d %%, off: %d %%)", percent1, percent2);
                aAPI->iTextLine(aData, 0, p->iLine);
                sprintf(aData->iResultValue, "%d%% %d%%", percent1, percent2);

                // We'll be generous and allow +/- 20%
                if ((percent < 80) || (percent > 120)) {
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                    sprintf(aData->iResultValue, "%d%% (NOT ON)", percent1);
                } else {
                    // Now turn off the LCD
                    UEZLCDOff(lcd);
                    UEZLCDBacklight(lcd, 0);
                    // Settling time
                    UEZTaskDelay(100);
                    UEZADCRequestSingle(adc, &r);
                    percent = reading * 100 / 0x180;
                    UEZLCDOn(lcd);
                    for (i=0; i<256; i+=2) {
                        UEZLCDBacklight(lcd, i);
                        UEZTaskDelay(10);
                    }

                    percent2 = percent;
                    sprintf(p->iLine, "Backlight Monitor (on: %d %%, off: %d %%)", percent1, percent2);
                    aAPI->iTextLine(aData, 0, p->iLine);
                    sprintf(aData->iResultValue, "%d%% %d%%", percent1, percent2);

                    if (percent > 50) {
                        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                        sprintf(aData->iResultValue, "%d%% (NOT OFF)", percent2);
                    } else {
                        aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                        sprintf(aData->iResultValue, "%d%% %d%%", percent1, percent2);
                    }
                }
            }
        }
        UEZADCClose(adc);
        UEZLCDClose(lcd);
    }
}

void FuncTestVoltageMonitor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_backlightTest;
    T_backlightTest *p = (T_backlightTest *)aData->iData;
    T_uezDevice adc;
    T_uezError error;
    ADC_RequestSingle r;
    TUInt32 reading;
    TUInt32 percent;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Voltage Monitor ...");

        // Just do it quickly
        error = UEZADCOpen("ADC_S12AD", &adc);
        if (error) {
            aAPI->iTextLine(aData, 0, "Voltage Monitor ADC not Found!");
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
            sprintf(aData->iResultValue, "Not Found");
        } else {
            // Take a reading
            r.iADCChannel = 0;
            r.iBitSampleSize = 12;
            r.iTrigger = ADC_TRIGGER_NOW;
            r.iCapturedData = &reading;
            error = UEZADCRequestSingle(adc, &r);
            if (error) {
                aAPI->iTextLine(aData, 0, "Voltage Monitor ADC bad reading!");
                aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                sprintf(aData->iResultValue, "No Reading");
            } else {
                // Generate percentage based on perfect 5V input
                // divided by 2, but compared to a A/D value of 0 - 0x3FFF
                // with a reference voltage of 3.3V
                //   reading = (voltage / 5 V) * 0x7FF
                //   percent = 100 * (reading / (0x7FF * 3.3V/5V))
                percent = reading * 100 / 0xC1C;
                sprintf(p->iLine, "Voltage Monitor (%d %%)", percent);
                aAPI->iTextLine(aData, 0, p->iLine);
                // We'll be generous and allow +/- 20%
                if ((percent < 80) || (percent > 120)) {
                    aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                } else {
                    aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
                    aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
                }
            }
        }
        UEZADCClose(adc);
    }
}

typedef unsigned long long TUInt64;

typedef struct {
    TUInt64 iPinsDrive;
    TUInt64 iPinsChange;
    const char *iMsg;
} T_gpioTestEntry;

typedef struct {
    TUInt8 iMapPort;
    TUInt32 iMapPin;
    const char *iText;
} T_gpioMapping;

static const T_gpioMapping G_gpioMapping[] = {
    #define EXP_J3_38            (1<<0)	// 38 - ENET_TXD0	
    { 8,    1, "J3.38" },        
    #define EXP_J3_37            (1<<1)	// 37 - ENET_TXD1
    { 8,    2, "J3.37" },        
    #define EXP_J3_36           (1<<2)	// 36 - ENET_TXEN
    { 8,    0, "J3.36" },        
    #define EXP_J3_35           (1<<3)	// 35 - ENET_CRSDV
    { 8,    3, "J3.35" },        
    #define EXP_J3_34			 (1<<4)	// 34 - ENET_RXD0
    { 7,    5, "J3.34" },        
    #define EXP_J3_33	         (1<<5)	// 33 - ENET_RXD1
    { 7,   4, "J3.33" },        
    #define EXP_J3_31			 (1<<6)	// 31 - ENET_RX_ER
    { 7,   7, "J3.31" },        
    #define EXP_J3_30           (1<<7)	// 30 - ENET_REFCLK
    { 7,   6, "J3.30" },        
    #define EXP_J3_28           (1<<8)	// 28 - ENET_MDC
    { 7,   2, "J3.28" },        
    #define EXP_J3_27           (1<<9)	// 27 - ENET_MDIO
    { 7,   1, "J3.27" },        
    #define EXP_J3_26           (1<<10)	// 26 - AN1
    { 4,   1, "J3.26" },        
    #define EXP_J3_25           (1<<11)	// 25 - DA1
    { 0,   5, "J3.25" },
	#define EXP_J3_21           (1<<12)	// 21 - P42_IRQ10-B_AN2
    { 4,   2, "J3.21" },
	#define EXP_J3_20           (1<<13)	// 20 - P43_IRQ11-B_AN3
    { 4,   3, "J3.20" },
    #define EXP_J3_15           (1<<14)	// 15 - P16/USB0_VBUS/USB0_VBUSEN-B/TIOC3C-A/TMO2/IRQ6-B
    { 1,   6, "J3.15" },        
    #define EXP_J3_14           (1<<15)	// 14 - P14/USB0_OVRCURA/USB0_DPUPE-B/TMRI2/IRQ4-B
    { 1,   4, "J3.14" },        
    #define EXP_J3_13           (1<<16)	// 13 - P07_IRQ15-A_ADTRG0-A
    { 0,   7, "J3.13" },                    
    #define EXP_J3_05           (1<<17)	// 5 - SDA0_TXD2A
    { 1,   3, "J3.05" },        	
    #define EXP_J3_04           (1<<18)	// 4 - SCL0_RXD2A
    { 1,   2, "J3.04" },
    #define EXP_J3_03           (1<<19)	// 3 - P32/TIOC0C/RTCOUT/CTX0/TXD6B/IRQ2A
    { 3,   2, "J3.03" },
	#define EXP_J3_02           (1<<20)	// 2 - P33/TIOC0D/CRX0/RXD6B/IRQ3A
    { 3,   3, "J3.02" },
};

const T_gpioTestEntry G_gpioTestArray[] = {
    { EXP_J3_38,          	EXP_J3_04,               	"J3.38 -> J3.04" },
    { EXP_J3_37,          	EXP_J3_05,               	"J3.37 -> J3.05" },
	{ EXP_J3_36,         	EXP_J3_27, 					"J3.36 -> J3.27" },
	{ EXP_J3_35,          	EXP_J3_34,       			"J3.35 -> J3.34" },
    { EXP_J3_33,          	EXP_J3_25,               	"J3.33 -> J3.25" },
    { EXP_J3_31,         	EXP_J3_30,               	"J3.31 -> J3.30" },
    { EXP_J3_26,         	EXP_J3_15,               	"J3.26 -> J3.15" },
	{ EXP_J3_21,          	EXP_J3_20,        			"J3.21 -> J3.20" },

	{ EXP_J3_04,          	EXP_J3_38,               	"J3.04 -> J3.38" },
    { EXP_J3_05,          	EXP_J3_37,               	"J3.05 -> J3.37" },
	{ EXP_J3_27,         	EXP_J3_36,					"J3.27 -> J3.36" },
	{ EXP_J3_34,          	EXP_J3_35,       			"J3.34 -> J3.35" },
    { EXP_J3_25,          	EXP_J3_33,               	"J3.25 -> J3.33" },
    { EXP_J3_30,         	EXP_J3_31,               	"J3.30 -> J3.31" },
    { EXP_J3_15,         	EXP_J3_26,               	"J3.15 -> J3.26" },
	{ EXP_J3_20,          	EXP_J3_21,        			"J3.20 -> J3.21" },
	/*
	{ EXP_J3_28,         	EXP_J3_03|EXP_J3_13,         "J3.28 -> J3.03|J3.13" },
	{ EXP_J3_03,         	EXP_J3_28|EXP_J3_13,         "J3.03 -> J3.28|J3.13" },
	{ EXP_J3_13,         	EXP_J3_03|EXP_J3_28,         "J3.02 -> J3.03|J3.28" },
	*/
};

HAL_GPIOPort **PortNumToPort(TUInt8 aPort)
{
    HAL_GPIOPort **p=0;

    if (aPort == 0) {
        HALInterfaceFind("GPIO0", (T_halWorkspace **)&p);
    } else if (aPort == 1) {
        HALInterfaceFind("GPIO1", (T_halWorkspace **)&p);
    } else if (aPort == 2) {
        HALInterfaceFind("GPIO2", (T_halWorkspace **)&p);
    } else if (aPort == 3) {
        HALInterfaceFind("GPIO3", (T_halWorkspace **)&p);
    } else if (aPort == 4) {
        HALInterfaceFind("GPIO4", (T_halWorkspace **)&p);
    } else if (aPort == 5) {
        HALInterfaceFind("GPIO5", (T_halWorkspace **)&p);
    } else if (aPort == 6) {
        HALInterfaceFind("GPIO6", (T_halWorkspace **)&p);
    } else if (aPort == 7) {
        HALInterfaceFind("GPIO7", (T_halWorkspace **)&p);
    } else if (aPort == 8) {
        HALInterfaceFind("GPIO8", (T_halWorkspace **)&p);
    } else if (aPort == 9) {
        HALInterfaceFind("GPIO9", (T_halWorkspace **)&p);
    }

    return p;
}

void FuncTestGPIOs(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
        TUInt32 iState;
        TUInt32 iStart;
    } T_gpioTest;
    T_gpioTest *p = (T_gpioTest *)aData->iData;
    TUInt32 reading;
    const T_gpioTestEntry *p_test;
    HAL_GPIOPort **p_port;
    const T_gpioMapping *p_pin;
    TUInt64 pre;
    TUInt64 post;
    TUInt64 diff;
    TUInt64 pin;

    if (aButton == OPT_INIT) {
        aAPI->iTextLine(aData, 0, "Checking GPIOs ... ");

        // Read the tick counter
//        p->iStart = UEZTickCounterGet();

        // Setup all the port pins to be MUX'd for GPIO and input only
        for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {

            p_pin = G_gpioMapping+pin;
            sprintf(p->iLine, "  Prep %s", p_pin->iText);
            aAPI->iTextLine(aData, 1, p->iLine);
            p_port = PortNumToPort(p_pin->iMapPort);
            //(*p_port)->SetPull(p_port, p_pin->iMapPin, GPIO_PULL_UP); // set to Pull up
            (*p_port)->SetInputMode(p_port, 1<<p_pin->iMapPin); // set to Input
            (*p_port)->SetMux(p_port, p_pin->iMapPin, 0); // set to GPIO
        }

        p->iState = 0;
    } else if (aButton == 0) {
        // Try one each time through this loop
        if (p->iState < ARRAY_COUNT(G_gpioTestArray)) {
            p_test = &G_gpioTestArray[p->iState++];
            sprintf(p->iLine, "  Testing %s", p_test->iMsg);
            aAPI->iTextLine(aData, 1, p->iLine);
            sprintf(aData->iResultValue, "%s", p_test->iMsg);

            // Go through and setup the pins for GPIO output on the drive pin
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
                if (p_test->iPinsDrive & (((TUInt64)1)<<pin)) {
                    // Drive 1 on this pin
                    (*p_port)->Set(p_port, 1<<p_pin->iMapPin); // set to 1
                    (*p_port)->SetOutputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                } else {
                    (*p_port)->SetInputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                    //(*p_port)->SetPull(p_port, 1<<p_pin->iMapPin, GPIO_PULL_UP);
                }
            }

            UEZTaskDelay(10);

            // Now, read all the pins and create a single word showing if pins are
            // high or low
            pre = 0;
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
                (*p_port)->Read(p_port, 1<<p_pin->iMapPin, &reading);
                if (reading)
                    pre |= (((TUInt64)1)<<pin);
            }

            // Now drive it to a 0
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
                if (p_test->iPinsDrive & (((TUInt64)1)<<pin)) {
                    // Drive 0 on this pin
                    (*p_port)->Clear(p_port, 1<<p_pin->iMapPin); // set to 1
                    (*p_port)->SetOutputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                } else {
                    (*p_port)->SetInputMode(p_port, 1<<p_pin->iMapPin); // set to Output
                    //(*p_port)->SetPull(p_port, 1<<p_pin->iMapPin, GPIO_PULL_UP);
                }
            }

            UEZTaskDelay(10);

            // Now, read all the pins and create a single word showing if pins are
            // high or low
            post = 0;
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
                p_port = PortNumToPort(p_pin->iMapPort);
				UEZTaskDelay(1);
                (*p_port)->Read(p_port, 1<<p_pin->iMapPin, &reading);
                if (reading)
                    post |= (((TUInt64)1)<<pin);
            }

            // What pins changed?
            diff = pre ^ post;

            // Which pins were we testing (both driving and receiving were tested,
            // all other should not have changed!)
            for (pin=0; pin<ARRAY_COUNT(G_gpioMapping); pin++) {
                p_pin = G_gpioMapping+pin;
				UEZTaskDelay(1);
                if ((p_test->iPinsDrive & (((TUInt64)1)<<pin)) || (p_test->iPinsChange & (((TUInt64)1)<<pin))) {
                    // This pin should have changed
                    if (!(diff & (((TUInt64)1)<<pin))) {
                        // This pin did not change when it should have
                        sprintf(p->iLine, "  %s stuck %s", p_pin->iText, (pre & (((TUInt64)1)<<pin))?"HIGH":"LOW");
                        aAPI->iTextLine(aData, 2, p->iLine);
                        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                        break;
                    }
					
                }
				// Removed the toggle check because these ports do not have software pull up's
				#if 0  
				else {
                    // This pin should NOT have changed
                    if (diff & (((TUInt64)1)<<pin)) {
                        // This pin changed when it should not have
                        sprintf(p->iLine, "  %s toggled!", p_pin->iText);
                        aAPI->iTextLine(aData, 2, p->iLine);
                        aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
                        aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
                        break;
                    }
                }
				#endif
            }
        } else {
            // Got to the end, must have passed
            aAPI->iTextLine(aData, 1, "  GPIO Testing Complete.");
            aAPI->iTextLine(aData, 2, "  All GPIOs lines passed.");
            sprintf(aData->iResultValue, "OK");
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        }
    }
}

/* Test that each page of the flash memory can be written to/read from */
TUInt32 NORFlashFullMemoryTest(
        const T_testAPI *aAPI,
        T_testData *aData,
        T_uezDevice dev,
        T_FlashChipInfo info)
{
    typedef struct {
        char iLine[80];
    } T_norFlashTest;
    T_uezError error;
    TUInt32 reg;
    TUInt8 flip;
    T_norFlashTest *p = (T_norFlashTest *)aData->iData;
    TUInt32 passNumber;
    TUInt32 region;
    TUInt32 blockInRegion;
    TUInt32 offset;
    TUInt32 byteOffset;

    aAPI->iTextLine(aData, 1, "  Checking QRY");

    // Start with determining if there is a 'QRY' set of characters
    // If the following fails, report "Not Found" error
    sprintf(aData->iResultValue, "Not Found");
    error = UEZFlashQueryReg(dev, 0x10, &reg);
    if (error || reg != 'Q')
        return 1001;
    error = UEZFlashQueryReg(dev, 0x11, &reg);
    if (error || reg != 'R')
        return 1001;
    error = UEZFlashQueryReg(dev, 0x12, &reg);
    if (error || reg != 'Y')
        return 1001;

    for (flip=0; flip<2; flip++)
    {
		int blockSize = 0;
		unsigned char *pBlockMemory = 0;
		
        //NOTE: Probably better to erase on a per-block basis to test the block erase
        //      is working correctly

         sprintf(p->iLine, "  flip %d)", flip);
         aAPI->iTextLine(aData, 1, p->iLine);
#if 0
        // Start with a good ol' fashion chip erase
         sprintf(p->iLine, "  Chip Erase (flip %d)", flip);
         aAPI->iTextLine(aData, 1, p->iLine);
        error = UEZFlashChipErase(dev);
        if (error)
            return 1009;
#endif

        // passNumber == 0 write pass
        // passNumber == 1 read pass
        for (passNumber = 0; passNumber < 2; passNumber++)
        {
            // flash address
            int address = 0;
            int overallBlock = 0;

            sprintf(p->iLine, "  passNumber %d", passNumber);
            aAPI->iTextLine(aData, 1, p->iLine);

            for (region = 0; region < info.iNumRegions; region++)
            {
			   int actualBlockSize;
// Temporarily override this
//info.iRegions[region].iNumEraseBlocks = 128-2;
               sprintf(p->iLine, "  region %d", region);
               aAPI->iTextLine(aData, 1, p->iLine);

               // ensure that our memory is the correct size
                actualBlockSize = info.iRegions[region].iSizeEraseBlock;
                if(blockSize != actualBlockSize)
                {
                    blockSize = actualBlockSize;
                    pBlockMemory = (unsigned char*)0xA0400000;

                    sprintf(p->iLine, "  pBlockMemory %p, blockSize %d", pBlockMemory, blockSize);
                    aAPI->iTextLine(aData, 3, p->iLine);
                }

                for (blockInRegion = 0; blockInRegion < info.iRegions[region].iNumEraseBlocks; blockInRegion++)
                {
                    int expectedValue;
                    if(flip == 0)
                    {
                        expectedValue = overallBlock;
                    } else
                    {
                        // invert bits
                        expectedValue = overallBlock ^ 0xFF;
                    }

                    // perform the write if passNumber == 0 or the read/compare if passNumber == 1
                    if(passNumber == 0)
                    {
                        sprintf(p->iLine, "  passNumber %d, overallBlock %d (WRITING)", passNumber, overallBlock);
                        aAPI->iTextLine(aData, 1, p->iLine);
                        sprintf(aData->iResultValue, "0x%08X (Writing)", address);

                        // fill the memory with the appropriate value
                        for (offset = 0; offset < blockSize; offset++)
                        {
                            pBlockMemory[offset] = expectedValue;
                        }

                        // erase the block
                        if (UEZFlashBlockErase(dev, address, blockSize))
                            return 10056;

                        // write the block
                        if (UEZFlashWrite(dev, address, pBlockMemory, blockSize))
                            return 10057;
                    } else
                    {
                        sprintf(p->iLine, "  passNumber %d, overallBlock %d (READING)", passNumber, overallBlock);
                        aAPI->iTextLine(aData, 1, p->iLine);
                        sprintf(aData->iResultValue, "0x%08X (Reading)", address);

                        // read the block into the block memory buffer
                        UEZFlashRead(dev, address, pBlockMemory, blockSize);

                        // compare bytes
                        for (byteOffset = 0; byteOffset < blockSize; byteOffset++)
                        {
                            if(pBlockMemory[byteOffset] != expectedValue)
                            {
                                sprintf(p->iLine, "  overallBlock %d, byteOffset %d expected 0x%x, got 0x%x",
                                        overallBlock, byteOffset, expectedValue, pBlockMemory[byteOffset]);
                                aAPI->iTextLine(aData, 2, p->iLine);
                                return 1025;
                            }
                        }
                    }

                    overallBlock++;

                    // advance the address
                    address += blockSize;

                } // foreach(blockInRegion)
            } // foreach(region)
        } // foreach(passNumber)
    } // foreach(flip)

    // Got here with no errors, good!
    sprintf(aData->iResultValue, "OK");
    return 0;
}

/* Full flash memory test */
void FuncTestFullFlash0(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
{
    typedef struct {
        char iLine[80];
    } T_sdramTest;
    T_sdramTest *p = (T_sdramTest *)aData->iData;
    TUInt32 errorCode;
    T_uezDevice dev;
    T_FlashChipInfo info;

    if (aButton == OPT_INIT) {
        // Just do it quickly and size it
        aAPI->iTextLine(aData, 0, "Full check of NOR Flash Memory ...");
        if (UEZFlashOpen("Flash0", &dev)) {
            errorCode = 1000;
        } else {
            UEZFlashGetChipInfo(dev, &info);
            errorCode = NORFlashFullMemoryTest(aAPI, aData, dev, info);
            UEZFlashClose(dev);
        }

        if (errorCode==0) {
            aAPI->iShowResult(aData, 0, TEST_RESULT_PASS, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_PASS);
        } else {
            sprintf(p->iLine, "  Error Code %d!", errorCode & 0x7FFF);
            aAPI->iTextLine(aData, 2, p->iLine);
            aAPI->iShowResult(aData, 0, TEST_RESULT_FAIL, 0);
            aAPI->iSetTestResult(aData, TEST_RESULT_FAIL);
        }
    }
}

void FuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton)
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
            for (p=G_testStates; p->iMonitorFunction != FuncTestComplete; p++) {
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
            for (p=G_testStates; p->iMonitorFunction != FuncTestComplete; p++) {
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
    { "Ignored",        YELLOW, },
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

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
  #define BUTTONS_LEFT_EDGE           20
  #define BUTTONS_RIGHT_EDGE          460
  #define BUTTONS_TOP                 160
  #define BUTTONS_BOTTOM              240
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
    int mode = 0;
    const char *p_text;
    TUInt16 y = ICalcY(aLine+aData->iLine);

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
    TBool haveLoopback = UEZGUIIsLoopbackBoardConnected();

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

                    if ((testState->iIgnoreIfNoLoopbackBoard) && (!haveLoopback)) {
                        G_results[testState-G_testStates] = TEST_RESULT_IGNORE;
                        testState++;
                        continue;
                    }

                    // New test state?
                    if (testState != lastTestState) {
                        // Check that we have room for these lines
                        if ((G_td.iLine + testState->iLinesNeeded) >= TEST_RESULT_NUM_LINES) {
                            TUInt32 i;
                            // Back to the start
                            G_td.iLine = 0;
                            // Clear all the lines
                            for (i=0; i<TEST_RESULT_NUM_LINES; i++)
                                G_testAPI.iTextLine(&G_td, i, 0);
                        }
    
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

void FuncTestPageHit(void)
{
}

#endif

/*-------------------------------------------------------------------------*
 * File:  FunctionalTest.c
 *-------------------------------------------------------------------------*/
