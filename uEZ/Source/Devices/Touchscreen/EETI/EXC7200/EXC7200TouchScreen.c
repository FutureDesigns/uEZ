/*-------------------------------------------------------------------------*
 * File:  EXC7200TouchScreen.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the EXC7200 Touchscreen
 *      Interface Chip.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <Device/Touchscreen.h>
#include <HAL/HAL.h>
#include "uEZI2C.h"
#include <Device/I2CBus.h>
#include "EXC7200TouchScreen.h"
#include <uEZGPIO.h>
#include <uEZProcessor.h>
#include <string.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

#define EXC7200_I2C_SPEED         400 //kHz
#define EXC7200_I2C_ADDRESS       (0x04)//(0x70>>1)

#define EXC7200_DEVICE_MODE       0x00
#define EXC7200_GUEST_ID          0x01
#define EXC7200_TD_STATUS         0x02
#define EXC7200_TOUCH1_XH         0x03
#define EXC7200_TOUCH1_XL         0x04
#define EXC7200_TOUCH1_YH         0x05
#define EXC7200_TOUCH1_YL         0x06

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

} T_EXC7200Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_EXC7200_Interface;

void TS_EXC7200_InterruptISR(
        void *aInterruptHandlerWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aInterruptHandlerWorkspace;
    _isr_UEZSemaphoreRelease(p->iSemWaitForTouch);
}


/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_EXC7200_InitializeWorkspace(void *aW)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aW;
    T_uezError error;

    p->aNumOpen = 0;
    p->iHaveCalibration = EFalse;
    p->iLastTouch = 0;
    p->iLastX = 0;
    p->iLastY = 0;
    error = UEZSemaphoreCreateBinary(&p->iSemWaitForTouch);
    if (error)
        return error;
    UEZSemaphoreGrab(p->iSemWaitForTouch, 0);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_EXC7200_Open(void *aW)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aW;

    if(p->aNumOpen == 0)
    {
    }
    p->aNumOpen++;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_EXC7200_Close(void *aW)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_EXC7200_Configure(T_uezDeviceWorkspace *aW)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aW;

    UEZSemaphoreGrab(p->iSemWaitForTouch, 0);

    UEZGPIOSetMux(p->iResetPin, 0);//Set to GPIO
    UEZGPIOOutput(p->iResetPin);
    UEZGPIOClear(p->iResetPin);

    UEZTaskDelay(1);
    UEZGPIOConfigureInterruptCallback(
        UEZ_GPIO_PORT_FROM_PORT_PIN(p->iInteruptPin),
        TS_EXC7200_InterruptISR,
        p);
    UEZGPIOEnableIRQ(p->iInteruptPin, GPIO_INTERRUPT_FALLING_EDGE);

    UEZGPIOSet(p->iResetPin);
    
    T_uezDevice i2c;
    I2C_Request request;
    T_uezError error;
    TUInt8 dataIn[10];
    TUInt8 dataOut[10] = { 0x03, 0x06, 0x0A, 0x04, 0x36, 0x3f, 0x01, 9, 0, 0};

    request.iAddr = EXC7200_I2C_ADDRESS;
    request.iSpeed = EXC7200_I2C_SPEED;
    request.iReadData = dataIn;
    request.iReadLength = 10;
    request.iWriteData = dataOut;
    request.iWriteLength = 10;

    UEZI2COpen(p->iI2CBus, &i2c);
    error = UEZI2CTransaction(i2c, &request);
                      

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_EXC7200Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_EXC7200_ApplyCalibration(
        T_EXC7200Workspace *p,
        TUInt32 x,
        TUInt32 y,
        TUInt32 *newX,
        TUInt32 *newY)
{
    TInt32 v;

#if 0
    v = x - p->iPenOffsetX;
    v *= p->iPenXScaleTop;
    v /= p->iPenXScaleBottom;
    v += p->iPenBaseX;
    *newX = (TUInt32)v;

    v = y - p->iPenOffsetY;
    v *= p->iPenYScaleTop;
    v /= p->iPenYScaleBottom;
    v += p->iPenBaseY;
    *newY = (TUInt32)v;
#else
    *newX = x;
    *newY = y;
#endif

}

/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_EXC7200_Poll(void *aWorkspace, T_uezTSReading *aReading)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aWorkspace;
    T_uezError error;
    I2C_Request r;
    T_uezDevice i2c0;
    TUInt8 dataIn[11];
    TUInt8 dataOut[5];
    TUInt32 Read = 0;
    TUInt32 x;
    TUInt32 y;
    static TBool loop = EFalse;
    TUInt32 start = UEZTickCounterGet();
    TUInt8 i;

    error = UEZI2COpen(p->iI2CBus, &i2c0);

    //while (UEZTickCounterGetDelta(start) < 2) {
    for(i = 0; i < 10; i++){
        // Try to grab the semaphore -- do we have new data?
        if (UEZSemaphoreGrab(p->iSemWaitForTouch, 0) == UEZ_ERROR_NONE) {
            // Got new data!
            Read = 1;
        } else {
            Read = 0;
        }

        if (Read == 0 && !loop) {
            aReading->iFlags = p->iLastTouch;
            aReading->iX = p->iLastX;
            aReading->iY = p->iLastY;

            TS_EXC7200_ApplyCalibration(p, (p->iLastX), (p->iLastY),
                    (TUInt32 *)&aReading->iX, (TUInt32 *)&aReading->iY);
            return UEZ_ERROR_NONE;
        } else {
//            dataOut[0] = 0x00;
//            error = UEZI2CWrite(i2c0,
//                    EXC7200_I2C_ADDRESS,
//                    EXC7200_I2C_SPEED,
//                    dataOut,
//                    1,
//                    50);

            memset((void*)dataIn, 0, sizeof(dataIn));
            error = UEZI2CRead(i2c0, EXC7200_I2C_ADDRESS, EXC7200_I2C_SPEED,
                    dataIn, 0x0A, 50);

            x = (((dataIn[3] & 0xFF) << 8) | dataIn[2]) / 51;

            y = (((dataIn[5] & 0xFF) << 8) | dataIn[4]) / 70;

            if ((dataIn[1] & 0x81) == 0x81 && (dataIn[0] == 0x04) && ((dataIn[1] & 0x7C) == 0)) {
                loop = ETrue;
                (aReading->iFlags) = (p->iLastTouch) = TSFLAG_PEN_DOWN;
                (aReading->iX) = (p->iLastX) = x;
                (aReading->iY) = (p->iLastY) = y;
                if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
                    // Convert X & Y coordinates
                    TS_EXC7200_ApplyCalibration(p, x, y,
                            (TUInt32 *)&aReading->iX, (TUInt32 *)&aReading->iY);
                }
            } else if (((dataIn[1] & 0x81) == 0x80) && (dataIn[0] == 0x04) && ((dataIn[1] & 0x7C) == 0)) {
                (aReading->iFlags) = (p->iLastTouch) = 0;
                (aReading->iX) = (p->iLastX);
                (aReading->iY) = (p->iLastY);
                UEZGPIOClearIRQ(p->iInteruptPin);
            } else { //if ((dataIn[3] & 0xC0) == 0x40){
                (aReading->iFlags) = (p->iLastTouch);
                (aReading->iX) = (p->iLastX);
                (aReading->iY) = (p->iLastY);

                TS_EXC7200_ApplyCalibration(p, (p->iLastX), (p->iLastY),
                        (TUInt32 *)&aReading->iX, (TUInt32 *)&aReading->iY);
            }

            if (!UEZGPIORead(p->iInteruptPin)) {
                loop = ETrue;
            } else {
                loop = EFalse;
                break;
            }

            if (((dataIn[1] & 0x81) == 0x80) && (dataIn[0] == 0x04)) {
              i = i + 1;
              i = i -1;
                break;
            }
        }
#if 0
        TUInt32 count = 0;
        while (!UEZGPIORead(p->iInteruptPin)) {
            error = UEZI2CRead(i2c0,
                    EXC7200_I2C_ADDRESS,
                    EXC7200_I2C_SPEED,
                    dataIn,
                    0x0A,
                    50);
            count++;
        }
#endif

    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_EXC7200_CalibrateStart(void *aWorkspace)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_EXC7200_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_EXC7200_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aWorkspace;

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
 * Routine:  TS_EXC7200_CalibrateAdd
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
T_uezError TS_EXC7200_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_EXC7200Workspace *p = (T_EXC7200Workspace *)aWorkspace;
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

T_uezError Touchscreen_EXC7200_Create(const char *aName,
                                        const char *aI2CBus,
                                        T_uezGPIOPortPin aInteruptPin,
                                        T_uezGPIOPortPin aResetPin)
{
    T_uezDevice device;
    T_uezDeviceWorkspace *workspace;
    T_EXC7200Workspace *p;

    // Need to register touchscreen device
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_EXC7200_Interface,
            &device, &workspace);

    p = (T_EXC7200Workspace *)workspace;

    strcpy(p->iI2CBus, aI2CBus);
    p->iResetPin = aResetPin;
    p->iInteruptPin = aInteruptPin;

    TS_EXC7200_Configure((T_uezDeviceWorkspace*)p);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_EXC7200_Interface = {
        {
                // Common interface
                "Touchscreen:MC:EXC7200",
                0x0100,
                TS_EXC7200_InitializeWorkspace,
                sizeof(T_EXC7200Workspace),
        },

        // Functions
        TS_EXC7200_Open,
        TS_EXC7200_Close,
        TS_EXC7200_Poll,

        TS_EXC7200_CalibrateStart,
        TS_EXC7200_CalibrateAdd,
        TS_EXC7200_CalibrateEnd,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  EXC7200TouchScreen.c
 *-------------------------------------------------------------------------*/
