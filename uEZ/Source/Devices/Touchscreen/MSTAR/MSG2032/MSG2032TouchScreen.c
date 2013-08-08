/*-------------------------------------------------------------------------*
 * File:  MSG2032TouchScreen.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the MSG2032 Touchscreen
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
#include "MSG2032TouchScreen.h"
#include <uEZGPIO.h>
#include <uEZProcessor.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

#define MSG2032_I2C_SPEED         100 //kHz
#define MSG2032_I2C_ADDRESS       (0x26<<1) //(0x62)

#define MSG2032_DEVICE_MODE       0x00
#define MSG2032_GUEST_ID          0x01
#define MSG2032_TD_STATUS         0x02
#define MSG2032_TOUCH1_XH         0x03
#define MSG2032_TOUCH1_XL         0x04
#define MSG2032_TOUCH1_YH         0x05
#define MSG2032_TOUCH1_YL         0x06

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

} T_MSG2032Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_MSG2032_Interface;

void TS_MSG2032_InterruptISR(
        void *aInterruptHandlerWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aInterruptHandlerWorkspace;
    _isr_UEZSemaphoreRelease(p->iSemWaitForTouch);
}


/*---------------------------------------------------------------------------*
 * Routine:  TS_MSG2032_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MSG2032_InitializeWorkspace(void *aW)
{
    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aW;
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
 * Routine:  TS_MSG2032_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MSG2032_Open(void *aW)
{
    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aW;

    if(p->aNumOpen == 0){
    }
    p->aNumOpen++;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MSG2032_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MSG2032_Close(void *aW)
{
    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MSG2032_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MSG2032_Configure(T_uezDeviceWorkspace *aW)
{
    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aW;

    UEZSemaphoreGrab(p->iSemWaitForTouch, 0);

    UEZGPIOSetMux(p->iResetPin, 0);//Set to GPIO
    UEZGPIOOutput(p->iResetPin);
    UEZGPIOClear(p->iResetPin);

    UEZTaskDelay(1);
    UEZGPIOConfigureInterruptCallback(
        UEZ_GPIO_PORT_FROM_PORT_PIN(p->iInteruptPin),
        TS_MSG2032_InterruptISR,
        p);
    UEZGPIOEnableIRQ(p->iInteruptPin, GPIO_INTERRUPT_FALLING_EDGE);

    UEZGPIOSet(p->iResetPin);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MSG2032_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_MSG2032Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_MSG2032_ApplyCalibration(
        T_MSG2032Workspace *p,
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
#if 0
    if (v < 0)
        v = 0;
    if (v >= DISPLAY_WIDTH)
        v = DISPLAY_WIDTH-1;
#endif
    *newX = (TUInt32)x;

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
    *newY = (TUInt32)y;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MSG2032_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MSG2032_Poll(void *aWorkspace, T_uezTSReading *aReading)
{
    T_MSG2032Workspace *p =
            (T_MSG2032Workspace *) aWorkspace;
    T_uezError error;
    I2C_Request r;
    T_uezDevice i2c0;
    TUInt8 dataIn[0x30];
    TUInt8 dataOut[5];
    TUInt32 Read = 0;
    TUInt32 x;
    TUInt32 y;
    
    error = UEZI2COpen(p->iI2CBus, &i2c0);

    // Try to grab the semaphore -- do we have new data?
    if (UEZSemaphoreGrab(p->iSemWaitForTouch, 0) == UEZ_ERROR_NONE) {
        // Got new data!
        Read = 1;
    } else {
        Read = 0;
    }

    if ( Read == 0)
    {
        aReading->iFlags = p->iLastTouch;
        aReading->iX = p->iLastX;
        aReading->iY = p->iLastY;

        TS_MSG2032_ApplyCalibration(
        p, (p->iLastX), (p->iLastY),
        (TUInt32 *) &aReading->iX,
        (TUInt32 *) &aReading->iY);
        return UEZ_ERROR_NONE;
    }
    else
    {
#if 1
        TUInt8 addr = 0x00;
        dataOut[0] = 0x00;
        TUInt8 addresses[50];
        TUInt8 index = 0;
        memset((void*)addresses, 0, sizeof(addresses));

        error = UEZ_ERROR_NAK;
        while (addr != 0x7F){
                      addr++;
            //if(addr == (0x34>>1) || addr == (0x3A>>1) || addr == (0xC0>>1) || addr  == 0x51 || addr == (0x92>>1)){
            //  addr++;
            //}
            error = UEZI2CWrite(i2c0,
                    addr,
                    MSG2032_I2C_SPEED,
                    dataOut,
                    1,
                    50);
            if(error == UEZ_ERROR_NONE){
              addresses[index++] = addr;
            }
        }
#endif
        dataOut[0] = 0x13;
        dataOut[1] = 0x00;
        error = UEZI2CWrite(i2c0,
                MSG2032_I2C_ADDRESS,
                MSG2032_I2C_SPEED,
                dataOut,
                2,
                50);

        error = UEZI2CRead(i2c0,
                MSG2032_I2C_ADDRESS,
                MSG2032_I2C_SPEED,
                dataIn,
                0x26,
                50);
        
        x = (((dataIn[0x12] & 0x70)<<4) | dataIn[0x13]);

        y = (((dataIn[0x12] & 0x07)<<8) | dataIn[0x14]);
        
        y = (UEZ_LCD_DISPLAY_HEIGHT) - y;

        if((dataIn[0x12] & 0x80) == 0x80)
        {
            (aReading->iFlags) = (p->iLastTouch) = TSFLAG_PEN_DOWN;
            (aReading->iX) = (p->iLastX)= x;
            (aReading->iY) = (p->iLastY)= y;
            if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
                // Convert X & Y coordinates
                TS_MSG2032_ApplyCalibration(
                        p, x, y,
                        (TUInt32 *) &aReading->iX,
                        (TUInt32 *) &aReading->iY);
            }
        }
        else //if ((dataIn[3] & 0xC0) == 0x40)
        {
            (aReading->iFlags) = (p->iLastTouch)= 0;
            (aReading->iX) = (p->iLastX);
            (aReading->iY) = (p->iLastY);

            TS_MSG2032_ApplyCalibration(
                    p, (p->iLastX), (p->iLastY),
                    (TUInt32 *) &aReading->iX,
                    (TUInt32 *) &aReading->iY);
        }

        UEZGPIOClearIRQ(GPIO_P0_22);
        return error;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MSG2032_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MSG2032_CalibrateStart(void *aWorkspace)
{
    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MSG2032_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MSG2032_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aWorkspace;

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
 * Routine:  TS_MSG2032_CalibrateAdd
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
T_uezError TS_MSG2032_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_MSG2032Workspace *p = (T_MSG2032Workspace *)aWorkspace;
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

T_uezError Touchscreen_MSG2032_Create(const char *aName,
                                     const char *aI2CBus,
                                     T_uezGPIOPortPin aInteruptPin,
                                     T_uezGPIOPortPin aResetPin)
{
    T_uezError error;
    T_uezDevice device;
    T_uezDeviceWorkspace *workspace;

    T_MSG2032Workspace *p;

    error = UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_MSG2032_Interface,
            &device, &workspace);

    p = (T_MSG2032Workspace *)workspace;

    strcpy(p->iI2CBus, aI2CBus);
    p->iResetPin = aResetPin;
    p->iInteruptPin = aInteruptPin;

    TS_MSG2032_Configure((T_uezDeviceWorkspace*)p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_MSG2032_Interface = {
        {
                // Common interface
                "Touchscreen:MC:MSG2032",
                0x0100,
                TS_MSG2032_InitializeWorkspace,
                sizeof(T_MSG2032Workspace),
        },

        // Functions
        TS_MSG2032_Open,
        TS_MSG2032_Close,
        TS_MSG2032_Poll,

        TS_MSG2032_CalibrateStart,
        TS_MSG2032_CalibrateAdd,
        TS_MSG2032_CalibrateEnd,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  MSG2032TouchScreen.c
 *-------------------------------------------------------------------------*/
