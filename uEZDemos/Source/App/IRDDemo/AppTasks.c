/*-------------------------------------------------------------------------*
 * File:  Tasks.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Various tasks to possibly run.  Examples if nothing else.
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
#include <stdio.h>
#include <uEZ.h>
#include <uEZFile.h>
#include <uEZDeviceTable.h>
#include <Device/Accelerometer.h>
#include <Device/Temperature.h>
#include <Device/LEDBank.h>
#include <Device/ButtonBank.h>
#include <Types/TimeDate.h>
#include <Device/EEPROM.h>
#include <Device/ADCBank.h>
#include <HAL/GPIO.h>
#include "AppTasks.h"
#include "AppDemo.h"
#include <uEZMemory.h>
#include <Source/Library/USBDevice/Generic/Bulk/GenericBulk.h>

T_uezError Main_ShowDirectory(void)
{
    T_uezFileEntry entry;

    if (UEZFileFindStart("/", &entry) == UEZ_ERROR_NONE) {
        do {
            printf("%c %-16.16s %02u/%02u/%04u %02u:%02u:%02u - %u bytes\n",
                (entry.iFileAttributes & UEZ_FILE_ENTRY_ATTR_DIRECTORY)?'D':'F',
                entry.iFilename,
                entry.iModifiedDate.iMonth,
                entry.iModifiedDate.iDay,
                entry.iModifiedDate.iYear,
                entry.iModifiedTime.iHour,
                entry.iModifiedTime.iMinutes,
                entry.iModifiedTime.iSeconds,
                entry.iFilesize);
        } while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
    } else {
        printf("No files found.\n");
    }
    UEZFileFindStop(&entry);

    return UEZ_ERROR_NONE;
}

T_uezError Main_ShowHTTPRootDirectory(void)
{
    T_uezFileEntry entry;

    if (UEZFileFindStart("/HTTPROOT", &entry) == UEZ_ERROR_NONE) {
        do {
            printf("%c %-16.16s %02u/%02u/%04u %02u:%02u:%02u - %u bytes\n",
                (entry.iFileAttributes & UEZ_FILE_ENTRY_ATTR_DIRECTORY)?'D':'F',
                entry.iFilename,
                entry.iModifiedDate.iMonth,
                entry.iModifiedDate.iDay,
                entry.iModifiedDate.iYear,
                entry.iModifiedTime.iHour,
                entry.iModifiedTime.iMinutes,
                entry.iModifiedTime.iSeconds,
                entry.iFilesize);
        } while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
    } else {
        printf("No files found.\n");
    }
    UEZFileFindStop(&entry);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Task:  MonDir
 *---------------------------------------------------------------------------*
 * Description:
 *      Output the directory once every 5 seconds.
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 MonDir(T_uezTask aMyTask, void *aParams)
{
    T_uezFile file;

    // Constantly fill the queue
    for (;;) {
        Main_ShowDirectory();
        if (UEZFileOpen("TESTOUT1.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
            UEZFileWrite(file, "Hello, World", 12, 0);
            UEZFileClose(file);
        }
        UEZTaskDelay(5000);
    }
}

/*---------------------------------------------------------------------------*
 * Task:  MonAccl
 *---------------------------------------------------------------------------*
 * Description:
 *      Output all 16 registers of the acceleromameter
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 MonAccl(T_uezTask aMyTask, void *aParams)
{
    // Constantly fill the queue
    T_uezDevice accel0;
    AccelerometerReading reading;
    DEVICE_Accelerometer **aws;

    if (UEZDeviceTableFind("Accel0", &accel0) == UEZ_ERROR_NONE) {
        UEZDeviceTableGetWorkspace(accel0, (T_uezDeviceWorkspace **)&aws);
        while (1) {
            if ((*aws)->ReadXYZ(aws, &reading, 1000) == UEZ_ERROR_TIMEOUT) {
                printf("Accelerometer reading timeout!");
            } else {
                printf("Accel0: %d %d %d\n", reading.iX, reading.iY, reading.iZ);
            }
            UEZTaskDelay(200);
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Task:  MonTemp
 *---------------------------------------------------------------------------*
 * Description:
 *      Monitor temperature registers
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 MonTemp(T_uezTask aMyTask, void *aParams)
{
    DEVICE_Temperature **p_temp;
    T_uezDevice temp0;
    T_uezError error;
    TInt32 v;
    TUInt32 frac;

    error = UEZDeviceTableFind("Temp0", &temp0);
    if (error)
        return (TUInt32)error;

    error = UEZDeviceTableGetWorkspace(temp0, (T_uezDeviceWorkspace **)&p_temp);
    if (error)
        return (TUInt32)error;

    for (;;) {
        error = (*p_temp)->Read(p_temp, &v);
        printf("Temp: ");
        if (error) {
            printf("Error %d\n", error);
        } else {
            // Get 3 bits of fractional part and multiply by 125 for
            // 0 --
            if (v < 0)
                frac = -v;
            else
                frac = v;
            frac = ((((TUInt32)(frac)) & 0xFFFF) >> 13) * 125;
            printf("%d.%03u C", v>>16, frac);
        }
        printf("\n");
        UEZTaskDelay(200);
    }
}

// G_LEDSAutoRun is a simple flag that animates the LEDs until it is stopped
// The MonLEDs checks this simple flag to see if the LEDs should be animated.
static TBool G_LEDSAutoRun = ETrue;
T_uezQueue G_monLEDsQueue;

/*---------------------------------------------------------------------------*
 * Routine:  MonLEDsStopAutoRunning
 *---------------------------------------------------------------------------*
 * Description:
 *      Simple routine to create a monitoring led event and put in the
 *      event queue.
 *---------------------------------------------------------------------------*/
T_uezError MonLEDEvent(TUInt8 aEvent, TUInt8 aLEDIndex)
{
#if 0
    T_monLedsMsg *p_msg;
    T_uezError error = UEZ_ERROR_NONE;

    p_msg = UEZMemAlloc(sizeof(T_monLedsMsg));
    if (p_msg) {
        p_msg->iEvent = aEvent;
        p_msg->iLEDIndex = aLEDIndex;
        if (UEZQueueSend(G_monLEDsQueue, p_msg, 0) == UEZ_ERROR_TIMEOUT) {
            // Can't queue, it's full.  Abort.
            free(p_msg);
            error = UEZ_ERROR_OVERFLOW;
        }
    } else {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }
    return error;
#else
    T_uezError error = UEZ_ERROR_NONE;
    T_monLedsMsg msg;

    msg.iEvent = aEvent;
    msg.iLEDIndex = aLEDIndex;
    if (UEZQueueSend(G_monLEDsQueue, &msg, 0) == UEZ_ERROR_TIMEOUT) {
        error = UEZ_ERROR_OVERFLOW;
    }
    return error;
#endif
}

/*---------------------------------------------------------------------------*
 * Task:  MonLEDs
 *---------------------------------------------------------------------------*
 * Description:
 *      Monitor the leds and buttons (flip them when they are pressed)
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 MonLEDs(T_uezTask aMyTask, void *aParams)
{
    // Constantly fill the queue
    T_uezError error;
    TUInt8 bits = 0x00;
    TUInt8 v;
    TUInt8 oldv;
    TUInt8 newv;
    TUInt32 butbits;
    DEVICE_LEDBank **p_leds;
    T_uezDevice leds;
    DEVICE_ButtonBank **p_buttons;
    T_uezDevice buttons;
    TUInt8 count = 5;
    TUInt8 showBit = 0x01;
    T_monLedsMsg msg;

    error = UEZDeviceTableFind("LEDBank0", &leds);
    if (error)
        return error;
    error = UEZDeviceTableGetWorkspace(leds, (T_uezDeviceWorkspace **)&p_leds);
    if (error)
        return error;

    error = UEZDeviceTableFind("ButtonBank0", &buttons);
    if (error)
        return error;
    error = UEZDeviceTableGetWorkspace(buttons, (T_uezDeviceWorkspace **)&p_buttons);
    if (error)
        return error;

    (*p_leds)->SetBlinkRegister(p_leds, 0, 500, 128);
    (*p_leds)->SetBlinkRegister(p_leds, 1, 250, 64);

    (*p_buttons)->SetActiveButtons(p_buttons, 0x0F);

    oldv = 0xFF;
    newv = 0xFF;
    for (;;) {
        // Look for a task or wait 50 ms
        if (UEZQueueReceive(G_monLEDsQueue, &msg, 50) != UEZ_ERROR_TIMEOUT) {
            // Got an event.  Stop running.
            G_LEDSAutoRun = EFalse;
            if (msg.iEvent == MON_LED_EVENT_OFF) {
                bits &= ~(1<<msg.iLEDIndex);
            } else if (msg.iEvent == MON_LED_EVENT_ON) {
                bits |= (1<<msg.iLEDIndex);
            }
        }
        if (!G_LEDSAutoRun) {
            (*p_leds)->On(p_leds, (bits & 0x0F)<<4);
            (*p_leds)->Off(p_leds, ((~bits) & 0x0F)<<4);
        }

        (*p_buttons)->Read(p_buttons, &butbits);
        newv = butbits;
        v = ~newv & oldv;
        if (v & 0x0F) {
            bits ^= v;
            G_LEDSAutoRun = EFalse;
        }
        oldv = newv;

        if (G_LEDSAutoRun) {
            count--;
            if (count == 0) {
                count = 5;
                (*p_leds)->On(p_leds, (showBit & 0x0F)<<4);
                (*p_leds)->Off(p_leds, ((~showBit) & 0x0F)<<4);

                // Roll bit and possibly roll around
                showBit <<= 1;
                if (showBit == 0x10)
                    showBit = 0x01;
            }
        }
    }
}

void MonLEDsStart(void)
{
    UEZQueueCreate(8, sizeof(T_monLedsMsg), &G_monLEDsQueue);
    UEZTaskCreate(MonLEDs, "MonLEDs", 128, (void *)0, UEZ_PRIORITY_NORMAL, 0);
}


/*---------------------------------------------------------------------------*
 * Task:  WatchGenericBulk
 *---------------------------------------------------------------------------*
 * Description:
 *      Monitor the USB port for data going back and forth and output
 *      to the console while sending a '.' character once a second.
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
#if COMPILE_OPTION_GENERIC_BULK
TUInt32 WatchGenericBulk(T_uezTask aMyTask, void *aParams)
{
    PARAM_NOT_USED(aMyTask);
    PARAM_NOT_USED(aParams);
    TInt32 c;

    // Sit and read characters out the virtual com and output a period once a second
    // or per received character.
    while (1) {
        c = GenericBulkGet(1000);
        if (c != -1)  {
            GenericBulkPut(c, 0);
            printf("%c", c);
        } else {
            GenericBulkPut('.', 0);
            printf(".");
        }
    }
}
#endif


/*---------------------------------------------------------------------------*
 * Task:  ReadButtons
 *---------------------------------------------------------------------------*
 * Description:
 *      Monitor the leds and buttons (flip them when they are pressed)
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Return
 *---------------------------------------------------------------------------*/
T_uezError ReadButtons(TUInt32 *aButtons)
{
    // Constantly fill the queue
    T_uezError error;
    DEVICE_ButtonBank **p_buttons;
    T_uezDevice buttons;

    // Look for the device
    error = UEZDeviceTableFind("ButtonBank0", &buttons);
    if (error)
        return error;
    error = UEZDeviceTableGetWorkspace(buttons, (T_uezDeviceWorkspace **)&p_buttons);
    if (error)
        return error;

    // Make sure the 4 buttons are active
    (*p_buttons)->SetActiveButtons(p_buttons, 0xFF);

    // Read those four buttons
    (*p_buttons)->Read(p_buttons, aButtons);

    return UEZ_ERROR_NONE;
}

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
#include <uEZProcessor.h>
#include <CMSIS/LPC17xx.h>
#include <uEZToneGenerator.h>
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
    // Blink
    for (;;) {
      LPC_GPIO0->FIOSET = (1<<7);
      UEZTaskDelay(250);
      LPC_GPIO0->FIOCLR = (1<<7);
      UEZTaskDelay(250);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  Tasks.c
 *-------------------------------------------------------------------------*/
