/*-------------------------------------------------------------------------*
 * File:  ST1232TouchScreen.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the ST1232 Touchscreen
 *      Interface Chip.
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
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <Device/Touchscreen.h>
#include <HAL/HAL.h>
#include "uEZI2C.h"
#include <Device/I2CBus.h>
#include "ST1232TouchScreen.h"
#include <uEZGPIO.h>
#include <uEZProcessor.h>
#include <UEZPlatform.h>
#include <stdlib.h>
#include <stdio.h>
#include <Device/Stream.h>
#include <uEZStream.h>
#include <Types/Serial.h>
 
#ifndef TOUCHSCREEN_TEST
    #define TOUCHSCREEN_TEST 0
#endif


/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

#define ST1232_I2C_SPEED         400 //kHz
#define ST1232_I2C_ADDRESS       (0x55)

#define ST1232_DEVICE_MODE       0x00
#define ST1232_GUEST_ID          0x01
#define ST1232_TD_STATUS         0x02
#define ST1232_TOUCH1_XH         0x03
#define ST1232_TOUCH1_XL         0x04
#define ST1232_TOUCH1_YH         0x05
#define ST1232_TOUCH1_YL         0x06

#define DEVICE_MODE_NORMAL          (0)
#define DEVICE_MODE_TEST            (1<<6)
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {

    const DEVICE_TOUCHSCREEN *iDevice;
    int aNumOpen;

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

    TInt32 iLastX;
    TInt32 iLastY;
    T_uezTSFlags iLastTouch;

    TUInt32 iTDSLow;
    TUInt32 iTDSHigh;
    TBool iTDSWasPressed;

    char iI2CBus[5];
    T_uezGPIOPortPin iInteruptPin;
    T_uezGPIOPortPin iResetPin;

    T_uezSemaphore iSemWaitForTouch;

} T_ST1232Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_ST1232_Interface;

void TS_ST1232_InterruptISR(
        void *aInterruptHandlerWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_ST1232Workspace *p = (T_ST1232Workspace *)aInterruptHandlerWorkspace;
    _isr_UEZSemaphoreRelease(p->iSemWaitForTouch);
}


/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_ST1232_InitializeWorkspace(void *aW)
{
    T_ST1232Workspace *p = (T_ST1232Workspace *)aW;
    T_uezError error;

    p->aNumOpen = 0;
    p->iHaveCalibration = EFalse;
    p->iLastTouch = 0;
    p->iLastX = 0;
    p->iLastY = 0;
    error = UEZSemaphoreCreateBinary(&p->iSemWaitForTouch);
    if (error)
        return error;
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_ST1232_Open(void *aW)
{
    T_ST1232Workspace *p = (T_ST1232Workspace *)aW;

    if(p->aNumOpen == 0){
    }
    p->aNumOpen++;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_ST1232_Close(void *aW)
{
    T_ST1232Workspace *p = (T_ST1232Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_ST1232_Configure(T_uezDeviceWorkspace *aW)
{
    T_ST1232Workspace *p = (T_ST1232Workspace *)aW;

    UEZSemaphoreGrab(p->iSemWaitForTouch, 0);

    UEZGPIOSetMux(p->iResetPin, 0);//Set to GPIO
    UEZGPIOOutput(p->iResetPin);
    UEZGPIOClear(p->iResetPin);

    UEZBSPDelayMS(10); // Reset Pulse width of 10ms
    UEZGPIOConfigureInterruptCallback(
        UEZ_GPIO_PORT_FROM_PORT_PIN(p->iInteruptPin),
        TS_ST1232_InterruptISR,
        p);
    UEZGPIOEnableIRQ(p->iInteruptPin, GPIO_INTERRUPT_FALLING_EDGE);

    UEZGPIOSet(p->iResetPin);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_ST1232Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_ST1232_ApplyCalibration(
        T_ST1232Workspace *p,
        TUInt32 x,
        TUInt32 y,
        TUInt32 *newX,
        TUInt32 *newY)
{
#if 0
    TInt32 v;
	
    v = x - p->iPenOffsetX;
    v *= p->iPenXScaleTop;
    v /= p->iPenXScaleBottom;
    v += p->iPenBaseX;
#if 0
    if (v < 0)
        v = 0;
    if (v >= DISPLAY_WIDTH)
        v = DISPLAY_WIDTH-1;
#endif
    *newX = (TUInt32)v;

    v = y - p->iPenOffsetY;
    v *= p->iPenYScaleTop;
    v /= p->iPenYScaleBottom;
    v += p->iPenBaseY;
#if 0
    if (v < 0)
        v = 0;
    if (v >= DISPLAY_HEIGHT)
        v = DISPLAY_HEIGHT-1;
#endif
    *newY = (TUInt32)v;
#else
    *newX = (TUInt32)x;
    *newY = (TUInt32)y;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_ST1232_Poll(void *aWorkspace, T_uezTSReading *aReading)
{
    T_ST1232Workspace *p =
            (T_ST1232Workspace *) aWorkspace;
    T_uezError error;
    T_uezDevice i2c;
    TUInt8 dataIn[0x20];
    TUInt8 dataOut[5];
    TUInt32 Read = 0;
    TUInt32 x;
    TUInt32 y;
    
    error = UEZI2COpen(p->iI2CBus, &i2c);

    // Try to grab the semaphore -- do we have new data?
    if (UEZSemaphoreGrab(p->iSemWaitForTouch, 0) == UEZ_ERROR_NONE) {
        // Got new data!
        Read = 1;
    } else {
        Read = 0;
    }
    while (1){
        if(UEZGPIORead(p->iInteruptPin)){
            Read = 0;
        }
        if ( Read == 0)
        {
            aReading->iFlags = p->iLastTouch;
            aReading->iX = p->iLastX;
            aReading->iY = p->iLastY;

            TS_ST1232_ApplyCalibration(
                    p, (p->iLastX), (p->iLastY),
                    (TUInt32 *) &aReading->iX,
                    (TUInt32 *) &aReading->iY);
            UEZI2CClose(i2c);
            return UEZ_ERROR_NONE;
        }else{
            dataOut[0] = 0x12;
            error = UEZI2CWrite(i2c,
                    ST1232_I2C_ADDRESS,
                    ST1232_I2C_SPEED,
                    dataOut,
                    1,
                    50);

            error = UEZI2CRead(i2c,
                    ST1232_I2C_ADDRESS,
                    ST1232_I2C_SPEED,
                    dataIn,
                    0x07,
                    50);
            if(error != UEZ_ERROR_NONE){
                UEZGPIOClearIRQ(p->iInteruptPin);
                UEZGPIOClear(p->iResetPin);
                UEZBSPDelayMS(10); // Reset Pulse width of 10ms
                UEZGPIOSet(p->iResetPin);
                break;
            }

            x = (((dataIn[0x0] & 0x70)<<4) | dataIn[0x1]);

            y = (((dataIn[0x0] & 0x07)<<8) | dataIn[0x2]);

            y = (UEZ_LCD_DISPLAY_HEIGHT) - y;

            if((dataIn[0x0] & 0x80) == 0x80 ){//&& (z > 5) && (dataIn[0x10] != 0)){
                (aReading->iFlags) = (p->iLastTouch) = TSFLAG_PEN_DOWN;
                (aReading->iX) = (p->iLastX)= x;
                (aReading->iY) = (p->iLastY)= y;
                if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
                    // Convert X & Y coordinates
                    TS_ST1232_ApplyCalibration(
                            p, x, y,
                            (TUInt32 *) &aReading->iX,
                            (TUInt32 *) &aReading->iY);
#if TOUCHSCREEN_TEST          
                    T_uezDevice iDevice;
                    DEVICE_STREAM **iStream;
                    TUInt8 sendbuf[16];
                    T_uezError error;
                    TUInt32 num = 0;
                    error = UEZDeviceTableFind("Console", &iDevice);
                    if (!error)
                         error = UEZDeviceTableGetWorkspace(iDevice, (T_uezDeviceWorkspace **)&iStream);
                    if (error){
                    } else {         
                         sprintf((char*)sendbuf,"%03d,%03d,%03d,\n\r", aReading->iX, aReading->iY, aReading->iPressure);
                         error = (*iStream)->Write(iStream, sendbuf, 16, &num, 100);
                    }
#endif

                }
            }else {
                (aReading->iFlags) = (p->iLastTouch)= 0;
                (aReading->iX) = (p->iLastX);
                (aReading->iY) = (p->iLastY);

                TS_ST1232_ApplyCalibration(
                        p, (p->iLastX), (p->iLastY),
                        (TUInt32 *) &aReading->iX,
                        (TUInt32 *) &aReading->iY);
            }
            UEZGPIOClearIRQ(p->iInteruptPin);
        }
    }
    UEZI2CClose(i2c);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_ST1232_CalibrateStart(void *aWorkspace)
{
    T_ST1232Workspace *p = (T_ST1232Workspace *)aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_ST1232_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_ST1232Workspace *p = (T_ST1232Workspace *)aWorkspace;

    p->iIsCalibrating = EFalse;

    if (p->iNumCalibratePoints == NUM_CALIBRATE_POINTS_NEEDED)  {
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
    } else {
        return UEZ_ERROR_NOT_ENOUGH_DATA;
    }    
    return UEZ_ERROR_NONE;
}
/*---------------------------------------------------------------------------*
 * Routine:  TS_ST1232_CalibrateAdd
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
T_uezError TS_ST1232_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_ST1232Workspace *p = (T_ST1232Workspace *)aWorkspace;
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

T_uezError Touchscreen_ST1232_Create(const char *aName,
                                     const char *aI2CBus,
                                     T_uezGPIOPortPin aInteruptPin,
                                     T_uezGPIOPortPin aResetPin)
{
    T_uezError error;
    T_uezDevice device;
    T_uezDeviceWorkspace *workspace;

    T_ST1232Workspace *p;

    error = UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_ST1232_Interface,
            &device, &workspace);

    p = (T_ST1232Workspace *)workspace;

    strcpy(p->iI2CBus, aI2CBus);
    p->iResetPin = aResetPin;
    p->iInteruptPin = aInteruptPin;

    TS_ST1232_Configure((T_uezDeviceWorkspace*)p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_ST1232_Interface = {
        {
                // Common interface
                "Touchscreen:MC:ST1232",
                0x0100,
                TS_ST1232_InitializeWorkspace,
                sizeof(T_ST1232Workspace),
        },

        // Functions
        TS_ST1232_Open,
        TS_ST1232_Close,
        TS_ST1232_Poll,

        TS_ST1232_CalibrateStart,
        TS_ST1232_CalibrateAdd,
        TS_ST1232_CalibrateEnd,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  ST1232TouchScreen.c
 *-------------------------------------------------------------------------*/
