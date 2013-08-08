/*-------------------------------------------------------------------------*
 * File:  FourWireTouchResist_TS.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface.
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
#include <uEZDeviceTable.h>
#include <uEZADC.h>
#include <uEZGPIO.h>
#include "FourWireTouchResist_TS.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#ifndef NUM_CALIBRATE_POINTS_NEEDED
    #define NUM_CALIBRATE_POINTS_NEEDED     3
#endif

#ifndef NUM_DUMMY_SAMPLES_PER_READING
    #define NUM_DUMMY_SAMPLES_PER_READING   0
#endif

#ifndef NUM_SAMPLES_PER_READING
    #define NUM_SAMPLES_PER_READING         16
#endif

#ifndef SCALE_READING_X
    #define SCALE_READING_X                 2
#endif

#ifndef SCALE_READING_Y
    #define SCALE_READING_Y                 2
#endif

#ifndef SCALE_READING_TOUCH
    #define SCALE_READING_TOUCH             2
#endif

#ifndef DEFAULT_TOUCHSCREEN_HIGH_LEVEL
    #define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        0x5000
#endif

#ifndef DEFAULT_TOUCHSCREEN_LOW_LEVEL
    #define DEFAULT_TOUCHSCREEN_LOW_LEVEL         0x1000
#endif

#define TS_GPIO_OUTPUT_MODE(gpio)   UEZGPIOOutput(p->iConfig.gpio.iGPIO)
#define TS_GPIO_INPUT_MODE(gpio)    UEZGPIOInput(p->iConfig.gpio.iGPIO)
#define TS_GPIO_CLEAR(gpio)         UEZGPIOClear(p->iConfig.gpio.iGPIO)
#define TS_GPIO_SET(gpio)           UEZGPIOSet(p->iConfig.gpio.iGPIO)
#define TS_GPIO_READ(gpio, v)       *v = UEZGPIORead(p->iConfig.gpio.iGPIO)
#define TS_GPIO_MUX_AS_GPIO(gpio)   \
    UEZGPIOSetMux(p->iConfig.gpio.iGPIO, p->iConfig.gpio.iGPIOMuxAsGPIO)
#define TS_GPIO_MUX_AS_ADC(gpio)    \
    UEZGPIOSetMux(p->iConfig.gpio.iGPIO, p->iConfig.gpio.iGPIOMuxAsADC)
#define TS_GPIO_SET_PULL(gpio, pull) \
    UEZGPIOSetPull(p->iConfig.gpio.iGPIO, pull)
#define TS_GPIO_ENABLE_INPUT_BUFFER(gpio) \
    UEZGPIOControl(p->iConfig.gpio.iGPIO, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0)
#define TS_GPIO_DISABLE_INPUT_BUFFER(gpio) \
    UEZGPIOControl(p->iConfig.gpio.iGPIO, GPIO_CONTROL_DISABLE_INPUT_BUFFER, 0)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_TOUCHSCREEN *iDevice;
    int aNumOpen;

    // Hardware linkage
    TS_FourWireTouchResist_Configuration iConfig;

    // Calibration information
    TBool iIsCalibrating;
    TUInt32 iNumCalibratePoints;
    T_uezTSReading iCalibrateReadingsTaken[NUM_CALIBRATE_POINTS_NEEDED];
    T_uezTSReading iCalibrateReadingsExpected[NUM_CALIBRATE_POINTS_NEEDED];

    TBool iHaveCalibration;

    TInt32 iPenOffsetX;
    TInt32 iPenBaseX;
    TInt32 iPenXScaleTop;
    TInt32 iPenXScaleBottom;

    TInt32 iPenOffsetY;
    TInt32 iPenBaseY;
    TInt32 iPenYScaleTop;
    TInt32 iPenYScaleBottom;

    T_uezSemaphore iSem;

    TUInt32 iTDSLow;
    TUInt32 iTDSHigh;
    TBool iTDSWasPressed;

    T_uezSemaphore iSemWaitForTouch;

    T_uezDevice iADCXPlus;
    T_uezDevice iADCYPlus;
} T_TS_FourWireTouchResist_Workspace;

TUInt32 G_readingsX[NUM_SAMPLES_PER_READING];
TUInt32 G_readingsY[NUM_SAMPLES_PER_READING];

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN TS_FourWireTouchResist_Interface;
static void IConfigureForTouchDetect(T_TS_FourWireTouchResist_Workspace *p);
static T_uezError TS_FourWireTouchResist_WaitForTouch(
        void *aWorkspace,
        TUInt32 aTimeout);

/*---------------------------------------------------------------------------*
 * Routine:  ISettleIO
 *---------------------------------------------------------------------------*
 * Description:
 *      Internal routine for waiting for IO to settle.
 *---------------------------------------------------------------------------*/
static void ISettleIO(T_TS_FourWireTouchResist_Workspace *p)
{
    UEZTaskDelay(1);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_FourWireTouchResist_InitializeWorkspace(void *aW)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aW;
    T_uezError error;

    p->aNumOpen = 0;
    p->iHaveCalibration = EFalse;
    p->iTDSWasPressed = EFalse;
    p->iTDSLow = (DEFAULT_TOUCHSCREEN_LOW_LEVEL >> 6) * NUM_SAMPLES_PER_READING;
    p->iTDSHigh = (DEFAULT_TOUCHSCREEN_HIGH_LEVEL >> 6)
        * NUM_SAMPLES_PER_READING;

    error = UEZSemaphoreCreateCounting(&p->iSemWaitForTouch, 1, 0);
    if (error)
        return error;
#if UEZ_REGISTER
    else {
        UEZSemaphoreSetName(p->iSemWaitForTouch, "WaitForTouch", "\0");
    }
#endif

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;
#if UEZ_REGISTER
    else
        UEZSemaphoreSetName(p->iSem, "ReadTouch", "\0");
#endif
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_Open(void *aW)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aW;
    p->aNumOpen++;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_Close(void *aW)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResistCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Interrupt routine called when a touch pin goes low
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      TUInt32 aPortPins           -- Which port pins were pressed
 *      T_gpioInterruptType         -- What condition caused the interrupt
 *---------------------------------------------------------------------------*/
void TS_FourWireTouchResistCallback(
        void *aW,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aW;

    // Flag the interrupt as occurring
    _isr_UEZSemaphoreRelease(p->iSemWaitForTouch);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_FourWireTouchResist_Configure(
        T_uezDeviceWorkspace *aW,
        const TS_FourWireTouchResist_Configuration *aConfig)
{
    T_uezError error;
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aW;
    p->iConfig = *aConfig;

    // Open up the ADC's
    error = UEZADCOpen(aConfig->iXPlus.iADC, &p->iADCXPlus);
    if (error)
        return error;
    error = UEZADCOpen(aConfig->iYPlus.iADC, &p->iADCYPlus);
    if (error)
        return error;

    // Setup IRQ handler for these lines
    UEZGPIOConfigureInterruptCallback(
            UEZ_GPIO_PORT_FROM_PORT_PIN(p->iConfig.iYMinus.iGPIO),
            TS_FourWireTouchResistCallback, p);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_ReadTouch
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_TS_FourWireTouchResist_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_ReadTouch(
        T_TS_FourWireTouchResist_Workspace *p,
        TBool *aValue)
{
    TUInt32 touchReading;
    ADC_RequestSingle adcRequestYPlus = {
        0, // Channel
        10, // 10 Bits
        ADC_TRIGGER_NOW,
        EFalse,
        0,
    };
    TUInt32 total;
    TUInt32 count;
    adcRequestYPlus.iADCChannel = p->iConfig.iYPlus.iADCChannel;
    adcRequestYPlus.iCapturedData = &touchReading;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Set XPlus to output low (GND)
    TS_GPIO_MUX_AS_GPIO(iXPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_ENABLE_INPUT_BUFFER(iXPlus);
    TS_GPIO_OUTPUT_MODE(iXPlus);
    TS_GPIO_CLEAR(iXPlus);
    TS_GPIO_SET_PULL(iXPlus, GPIO_PULL_DOWN);

    // Set XMinus to float (input mode with no pull up)
    TS_GPIO_INPUT_MODE(iXMinus);
    TS_GPIO_SET(iXMinus);
    TS_GPIO_SET_PULL(iXMinus, GPIO_PULL_NONE);

    // Set YPlus to input mode with a pull up
    TS_GPIO_MUX_AS_ADC(iYPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_DISABLE_INPUT_BUFFER(iYPlus);
    TS_GPIO_INPUT_MODE(iYPlus);
    TS_GPIO_SET_PULL(iYPlus, GPIO_PULL_NONE);

    // Set YMinus to float (input mode with no pull up)
    TS_GPIO_INPUT_MODE(iYMinus);
    TS_GPIO_SET_PULL(iYMinus, GPIO_PULL_UP);

    for (total = count = 0; count < NUM_SAMPLES_PER_READING; count++) {
        UEZADCRequestSingle(p->iADCYPlus, &adcRequestYPlus);
        total += touchReading * SCALE_READING_TOUCH;
    }
    touchReading = total;

    // Were we detected as pressed?
    if (p->iTDSWasPressed) {
        // We were detected as last pressed, therefore,
        // we only want to know if we are NOT pressed by
        // going from a low to a high.
        if (touchReading > p->iTDSHigh) {
            // We are no longer being pressed
            *aValue = EFalse;
        } else {
            // We are still being pressed
            *aValue = ETrue;
        }
    } else {
        // We currently not detected as being pressed, therefore,
        // check if we are going below the low point.
        if (touchReading > p->iTDSLow) {
            // Not below yet, so still not touched
            *aValue = EFalse;
        } else {
            // Below now, so being touched
            *aValue = ETrue;
        }
    }

    // Remember if we were being touched so the threshold
    // is being changed to detect the other side.
    p->iTDSWasPressed = *aValue;

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_ReadTouchGPIOOnly
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_TS_FourWireTouchResist_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
#if 0 // Not currently used, but saved for possible use later
static T_uezError TS_FourWireTouchResist_ReadTouch_GPIO(
        T_TS_FourWireTouchResist_Workspace *p, TBool *aValue)
{
    TUInt32 xPlus;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Set XPlus to output low (GND)
    TS_GPIO_INPUT_MODE(iXPlus);
    TS_GPIO_CLEAR(iXPlus);
    TS_GPIO_SET_PULL(iXPlus, GPIO_PULL_UP);

    // Set XMinus to output low (GND)
    TS_GPIO_INPUT_MODE(iXMinus);
    TS_GPIO_CLEAR(iXMinus);
    TS_GPIO_SET_PULL(iXMinus, GPIO_PULL_NONE);

    TS_GPIO_INPUT_MODE(iYPlus);
    TS_GPIO_SET_PULL(iYPlus, GPIO_PULL_NONE);

    // Set YMinus to float (input mode) with no pull ups
    TS_GPIO_OUTPUT_MODE(iYMinus);
    TS_GPIO_SET_PULL(iYMinus, GPIO_PULL_NONE);
    TS_GPIO_CLEAR(iYMinus);

    ISettleIO(p);

    TS_GPIO_READ(iXPlus, &xPlus);
    if (xPlus)
        *aValue = EFalse; // still high, not touched
    else
        *aValue = ETrue; // pulled down, touched

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_ReadX
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_TS_FourWireTouchResist_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_ReadX(
        T_TS_FourWireTouchResist_Workspace *p,
        TUInt32 *aValue)
{
    TUInt32 reading = 0;
    TUInt32 count;
    TUInt32 total;
    ADC_RequestSingle adcRequestYPlus = {
        0, // Channel
        10, // 10 Bits
        ADC_TRIGGER_NOW,
        EFalse,
        0,
    };
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    adcRequestYPlus.iCapturedData = &reading;

    // Setup iXPlus as GPIO
    TS_GPIO_MUX_AS_GPIO(iXPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_ENABLE_INPUT_BUFFER(iXPlus);
    // Set XPlus to output high
    TS_GPIO_OUTPUT_MODE(iXPlus);
    TS_GPIO_SET(iXPlus);
    TS_GPIO_SET_PULL(iXPlus, GPIO_PULL_NONE);

    // Setup iYPlus as ADC
    TS_GPIO_MUX_AS_ADC(iYPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_DISABLE_INPUT_BUFFER(iYPlus);

    // Set XMinus to output low
    TS_GPIO_OUTPUT_MODE(iXMinus);
    TS_GPIO_CLEAR(iXMinus);
    TS_GPIO_SET_PULL(iXMinus, GPIO_PULL_NONE);

    // Set YMinus to float (input mode) with no pull ups
    TS_GPIO_INPUT_MODE(iYMinus);
    TS_GPIO_SET_PULL(iYMinus, GPIO_PULL_NONE);

    ISettleIO(p);

    // Setup the request for the proper channel
    adcRequestYPlus.iADCChannel = p->iConfig.iYPlus.iADCChannel;

    for (total = count = 0; count < NUM_SAMPLES_PER_READING; count++) {
        UEZADCRequestSingle(p->iADCYPlus, &adcRequestYPlus);
        total += reading * SCALE_READING_X;
G_readingsX[count] = reading;
    }

    *aValue = total;
    //printf("ReadX: %d\n", *aValue);

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_ReadY
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the Y location being touched (if any)
 * Inputs:
 *      T_TS_FourWireTouchResist_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_ReadY(
        T_TS_FourWireTouchResist_Workspace *p,
        TUInt32 *aValue)
{
    TUInt32 yPlus = 0;
    ADC_RequestSingle adcReq = {
        1, // Channel
        10, // 10 Bits
        ADC_TRIGGER_NOW,
        EFalse,
        0,
    };
    TUInt32 count, total;

    adcReq.iCapturedData = &yPlus;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Setup iXPlus as ADC
    TS_GPIO_MUX_AS_ADC(iXPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_DISABLE_INPUT_BUFFER(iXPlus);

    // Setup iYPlus as GPIO
    TS_GPIO_MUX_AS_GPIO(iYPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_ENABLE_INPUT_BUFFER(iYPlus);
    // Set YPlus to output high
    TS_GPIO_OUTPUT_MODE(iYMinus);
    TS_GPIO_SET(iYMinus);
    TS_GPIO_SET_PULL(iYMinus, GPIO_PULL_NONE);

    // Set XMinus to float (input mode) with no pull ups
    TS_GPIO_INPUT_MODE(iXMinus);
    TS_GPIO_SET_PULL(iXMinus, GPIO_PULL_NONE);

    // Set YMinus to output low
    TS_GPIO_OUTPUT_MODE(iYPlus);
    TS_GPIO_CLEAR(iYPlus);
    TS_GPIO_SET_PULL(iYPlus, GPIO_PULL_NONE);

    ISettleIO(p);

    // Setup the request for the proper channel
    adcReq.iADCChannel = p->iConfig.iXPlus.iADCChannel;

    for (total = count = 0; count < NUM_SAMPLES_PER_READING; count++) {
        UEZADCRequestSingle(p->iADCXPlus, &adcReq);
        total += yPlus * SCALE_READING_X;
G_readingsY[count] = yPlus;
    }

    *aValue = total;

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_TS_FourWireTouchResist_Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_FourWireTouchResist_ApplyCalibration(
        T_TS_FourWireTouchResist_Workspace *p,
        TUInt32 x,
        TUInt32 y,
        TUInt32 *newX,
        TUInt32 *newY)
{
    TInt32 v;

    v = x - p->iPenOffsetX;
    v *= p->iPenXScaleTop;
    v /= p->iPenXScaleBottom;
    v += p->iPenBaseX;
    *newX = (TUInt32) v;

    v = y - p->iPenOffsetY;
    v *= p->iPenYScaleTop;
    v /= p->iPenYScaleBottom;
    v += p->iPenBaseY;
    *newY = (TUInt32) v;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_Poll(
        void *aWorkspace,
        T_uezTSReading *aReading)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aWorkspace;
    T_uezError error;
    TUInt32 x=0, y=0;
    TBool isTouch = EFalse;
    TUInt32 start;
    TUInt32 end;
    TUInt32 delta;
    static TUInt32 average = 0;

    error = TS_FourWireTouchResist_WaitForTouch(
                aWorkspace,
                10);

    start = UEZTickCounterGet();
    if (error != UEZ_ERROR_TIMEOUT) {
        while (1) {
            isTouch = EFalse;

#if 1
            // Is this touch the type we expect?
            error = TS_FourWireTouchResist_ReadTouch(p, &isTouch);
            if (error)
                return error;
            if (!isTouch)
                break;
#endif

            error = TS_FourWireTouchResist_ReadX(p, (TUInt32 *) &aReading->iX);
            if (error)
                    return error;

            error = TS_FourWireTouchResist_ReadY(p, (TUInt32 *) &aReading->iY);
            if (error)
                return error;

            // Make sure we are still touching
            // Notice that isTouch is EFalse unless we detect a touch at the end
            // (we only come into this code if there was a touch in the middle)
            error = TS_FourWireTouchResist_ReadTouch(p, &isTouch);
            if (error)
                return error;
            if (!isTouch)
                break;

#if 1
            UEZTaskDelay(5);

            // Second read to ensure we are still touching!
            error = TS_FourWireTouchResist_ReadTouch(p, &isTouch);
            if (error)
                return error;
            if (!isTouch)
                break;
#endif
            x = aReading->iX;
            y = aReading->iY;
            break;
        }
    } else {
        // No touching.
        isTouch = EFalse;
        error = UEZ_ERROR_NONE;
    }

    if (isTouch) {
        aReading->iFlags = TSFLAG_PEN_DOWN;

        if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
                // Convert X & Y coordinates
            TS_FourWireTouchResist_ApplyCalibration(
                    p, x, y,
                    (TUInt32 *) &aReading->iX,
                    (TUInt32 *) &aReading->iY);
        }
    } else {
        // Pen is not down
        aReading->iFlags = 0;
    }

    end = UEZTickCounterGet();
    delta = (end - start) * 1000000;
    average = (average * 99 + delta) / 100;
#if 0
    if (aReading->iFlags == TSFLAG_PEN_DOWN) {
for (i=0; i<NUM_SAMPLES_PER_READING; i++) {
    printf("%03X ", G_readingsX[i]);
}
printf("| ");
for (i=0; i<NUM_SAMPLES_PER_READING; i++) {
    printf("%03X ", G_readingsY[i]);
}
printf(".\n");
lineOut = ETrue;
} else {
if (lineOut) {
    printf("-\n");
    lineOut = EFalse;
}
}
#endif
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_CalibrateStart(void *aWorkspace)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *                                      Return ERROR_OUT_OF_RANGE if
 *                                      calibration is bad.  Returns
 *                                      UEZ_ERROR_NOT_ENOUGH_DATA if not
 *                                      enough data points were added.
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aWorkspace;

    p->iIsCalibrating = EFalse;

    if (p->iNumCalibratePoints == NUM_CALIBRATE_POINTS_NEEDED) {
        // Do the calibration algorithm here
        minIX = minOX = minIY = minOY = 0xFFFFFFFF;
        maxIX = maxOX = maxIY = maxOY = 0;

        for (i = 0; i < p->iNumCalibratePoints; i++) {
            // Find range of inputs
            if (p->iCalibrateReadingsTaken[i].iX < minIX) {
                minIX = p->iCalibrateReadingsTaken[i].iX;
                minOX = p->iCalibrateReadingsExpected[i].iX;
            }
            if (p->iCalibrateReadingsTaken[i].iX > maxIX) {
                maxIX = p->iCalibrateReadingsTaken[i].iX;
                maxOX = p->iCalibrateReadingsExpected[i].iX;
            }
            if (p->iCalibrateReadingsTaken[i].iY < minIY) {
                minIY = p->iCalibrateReadingsTaken[i].iY;
                minOY = p->iCalibrateReadingsExpected[i].iY;
            }
            if (p->iCalibrateReadingsTaken[i].iY > maxIY) {
                maxIY = p->iCalibrateReadingsTaken[i].iY;
                maxOY = p->iCalibrateReadingsExpected[i].iY;
            }
        }

        // Find scale factor of X
        p->iPenOffsetX = minIX;
        p->iPenBaseX = minOX;
        p->iPenXScaleTop = maxOX - minOX;
        p->iPenXScaleBottom = maxIX - minIX;

        // Find scale factor of Y
        p->iPenOffsetY = minIY;
        p->iPenBaseY = minOY;
        p->iPenYScaleTop = maxOY - minOY;
        p->iPenYScaleBottom = maxIY - minIY;

        p->iHaveCalibration = ETrue;

        // Check to see if this is valid calibration data
        // (should have a resolution of at least 5 bits per pixel output
        // using 10 bit ADCs).
        // This check doesn't catch everything but will help catch
        // most of the bogus readings (such as all the same points)
        if ((p->iPenXScaleBottom < p->iPenXScaleTop * 32)
                || (p->iPenYScaleBottom < p->iPenYScaleTop * 32)) {
            return UEZ_ERROR_OUT_OF_RANGE;
        }
    } else {
            return UEZ_ERROR_NOT_ENOUGH_DATA;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_CalibrateAdd
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a calibration point to the calibration data.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const T_uezTSReading *aReadingTaken -- Raw reading values
 *      const T_uezTSReading *aReadingExpected -- Expected output reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aWorkspace;
    TUInt32 num;

    num = p->iNumCalibratePoints;
    if (num < NUM_CALIBRATE_POINTS_NEEDED) {
        p->iCalibrateReadingsTaken[num] = *aReadingTaken;
        p->iCalibrateReadingsExpected[num] = *aReadingExpected;
        p->iNumCalibratePoints++;
    } else {
            return UEZ_ERROR_OVERFLOW;
    }

    // Report an error if there is not enough data
    if (p->iNumCalibratePoints < NUM_CALIBRATE_POINTS_NEEDED)
        return UEZ_ERROR_NOT_ENOUGH_DATA;

    // Got enough, good.
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_FourWireTouchResist_SetTouchDetectSensitivity
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets the touch sensitivity for detecting a touch on the screen.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt16 aLowLevel           -- 16-bit low level.  0xFFFF is full 100%
 *                                      sensitivity, 0 is no sensitivity.
 *      TUInt16 aHighLevel          -- 16-bit high level.  0xFFFF is no (0%)
 *                                      sensitivity, 0xFFFF is no sensitivity.
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_FourWireTouchResist_SetTouchDetectSensitivity(
        void *aWorkspace,
        TUInt16 aLowLevel,
        TUInt16 aHighLevel)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aWorkspace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    // Shift down the sensitivity to be a 10 bit reading
    // then multiply by the number of readings.
    p->iTDSLow = (aLowLevel >> 6) * NUM_SAMPLES_PER_READING;
    p->iTDSHigh = (aHighLevel >> 6) * NUM_SAMPLES_PER_READING;
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

static void IConfigureForTouchDetect(T_TS_FourWireTouchResist_Workspace *p)
{
    // Configure for a touch detect and IRQ
    // Set XPlus to output low (GND)
    TS_GPIO_MUX_AS_GPIO(iXPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_ENABLE_INPUT_BUFFER(iXPlus);
    TS_GPIO_OUTPUT_MODE(iXPlus);
    TS_GPIO_CLEAR(iXPlus);
    TS_GPIO_SET_PULL(iXPlus, GPIO_PULL_DOWN);

    // Set XMinus to float (input mode with no pull up)
    TS_GPIO_INPUT_MODE(iXMinus);
    TS_GPIO_SET(iXMinus);
    TS_GPIO_SET_PULL(iXMinus, GPIO_PULL_NONE);

    // Set YPlus to input mode with a pull up
    TS_GPIO_MUX_AS_GPIO(iYPlus);
    if (p->iConfig.iNeedInputBufferDisabled)
        TS_GPIO_ENABLE_INPUT_BUFFER(iYPlus);
    TS_GPIO_INPUT_MODE(iYPlus);
    TS_GPIO_SET_PULL(iYPlus, GPIO_PULL_UP);

    // Set YMinus to float (input mode with no pull up)
    TS_GPIO_INPUT_MODE(iYMinus);
    TS_GPIO_SET_PULL(iYMinus, GPIO_PULL_UP);

    ISettleIO(p);
}

static T_uezError TS_FourWireTouchResist_WaitForTouch(
        void *aWorkspace,
        TUInt32 aTimeout)
{
    T_TS_FourWireTouchResist_Workspace *p =
            (T_TS_FourWireTouchResist_Workspace *) aWorkspace;
    T_uezError error;
    TUInt32 sense;

    // Grab the touch screen
    error = UEZSemaphoreGrab(p->iSem, aTimeout);
    if (error)
            return error;

    IConfigureForTouchDetect(p);

    if (p->iConfig.iIRQBased) {
        // No, not already touching.
        // Setup and wait for interrupt up on edge or timeout
        UEZGPIOClearIRQ(p->iConfig.iYMinus.iGPIO);
        UEZGPIOEnableIRQ(p->iConfig.iYMinus.iGPIO, GPIO_INTERRUPT_FALLING_EDGE);
    }

    // Are we touching now? (no edge to detect)
    TS_GPIO_READ(iYMinus, &sense);
    if (sense) {
        if (p->iConfig.iIRQBased) {
            // Wait for interrupt on pin going low
            error = UEZSemaphoreGrab(p->iSemWaitForTouch, aTimeout);
            UEZGPIODisableIRQ(p->iConfig.iYMinus.iGPIO, GPIO_INTERRUPT_FALLING_EDGE);
        } else {
            // Wait for pin to go low
            while (1) {
                aTimeout--;
                if (aTimeout == 0) {
                    error = UEZ_ERROR_TIMEOUT;
                    break;
                }
                UEZTaskDelay(1);
                TS_GPIO_READ(iYMinus, &sense);
                if (!sense)
                    break;
            }
        }
    } else {
        if (p->iConfig.iIRQBased) {
            UEZGPIODisableIRQ(p->iConfig.iYMinus.iGPIO, GPIO_INTERRUPT_FALLING_EDGE);
        }
    }

    // Done waiting
    UEZSemaphoreRelease(p->iSem);

    // Return a timeout, error, or positive results
    return error;
}

void Touchscreen_FourWireTouchResist_Create(
        const char *aName,
        const TS_FourWireTouchResist_Configuration *aConfig)
{
    T_uezDeviceWorkspace *p_ts;

    // Need to register touchscreen device
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_FourWireTouchResist_Interface,
            0, &p_ts);

    // Configure it based on the given settings
    TS_FourWireTouchResist_Configure(
            p_ts,
            aConfig);
}
/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_FourWireTouchResist_Interface = {
    {
        // Common interface
        "Touchscreen:Generic:4Wire",
        0x0105,
        TS_FourWireTouchResist_InitializeWorkspace,
        sizeof(T_TS_FourWireTouchResist_Workspace),
    },

    // Functions
    TS_FourWireTouchResist_Open,
    TS_FourWireTouchResist_Close,
    TS_FourWireTouchResist_Poll,

    TS_FourWireTouchResist_CalibrateStart,
    TS_FourWireTouchResist_CalibrateAdd,
    TS_FourWireTouchResist_CalibrateEnd,

    TS_FourWireTouchResist_SetTouchDetectSensitivity,

    // v1.05 Functions
    TS_FourWireTouchResist_WaitForTouch,
};

/*-------------------------------------------------------------------------*
 * End of File:  FourWireTouchResist_TS.c
 *-------------------------------------------------------------------------*/
