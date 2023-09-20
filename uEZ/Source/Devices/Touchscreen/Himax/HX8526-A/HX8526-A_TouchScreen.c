/*-------------------------------------------------------------------------*
 * File:  HX8526-A_TouchScreen.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the HX8526A Himax Touchscreen
 *      Interface Chip.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
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
#include "HX8526-A_TouchScreen.h"
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

#define HX8526A_I2C_SPEED         400 //kHz

#define HX8526A_I2C_ADDRESS       (0x94>>1)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {

        const DEVICE_TOUCHSCREEN *iDevice;
        int32_t aNumOpen;
        TUInt8 iDeviceID[3];

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

        char iI2CBus[24];
        T_uezGPIOPortPin iInteruptPin;
        T_uezGPIOPortPin iResetPin;

        T_uezSemaphore iSemWaitForTouch;

} T_HX8526AWorkspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_HX8526A_Interface;

void TS_HX8526A_InterruptISR(
        void *aInterruptHandlerWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aInterruptHandlerWorkspace;
    _isr_UEZSemaphoreRelease(p->iSemWaitForTouch);
}


/*---------------------------------------------------------------------------*
 * Routine:  TS_HX8526A_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_HX8526A_InitializeWorkspace(void *aW)
{
    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aW;
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
 * Routine:  TS_HX8526A_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_HX8526A_Open(void *aW)
{
    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aW;

    if(p->aNumOpen == 0){
    }
    p->aNumOpen++;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_HX8526A_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_HX8526A_Close(void *aW)
{
    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_HX8526A_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The I2C bus and GPIO device are being declared.

 // Reset active low, interrupt output is active low


 * Inputs:
 *      void *aW                    -- Particular TS workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_HX8526A_Configure(T_uezDeviceWorkspace *aW)
{
    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aW;
    T_uezError error;
    T_uezDevice i2c;
    TUInt8 dataIn[0x20];
    TUInt8 dataOut[5];

    UEZSemaphoreGrab(p->iSemWaitForTouch, 0);

    UEZGPIOSetMux(p->iResetPin, 0);//Set to GPIO
    UEZGPIOOutput(p->iResetPin);
    UEZGPIOClear(p->iResetPin); // start touchscreen reset by holding low

    UEZTaskDelay(20);
    UEZGPIOConfigureInterruptCallback(
            p->iInteruptPin,
            TS_HX8526A_InterruptISR,
            p);
    

    UEZGPIOSet(p->iResetPin); // touchscreen reset completed

    // 5ms delay?
    UEZTaskDelay(5);

    error = UEZI2COpen(p->iI2CBus, &i2c);
    
    // Send device ID Code command
    dataOut[0] = 0x31;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            1,
            50);
    
    // Read device ID code
    error = UEZI2CRead(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataIn,
            3,
            50);
    p->iDeviceID[0] = dataIn[0];
    p->iDeviceID[1] = dataIn[1];
    p->iDeviceID[2] = dataIn[2];
    
    // Verify first byte of device ID code is correct
    // The code is documented as 0x85, 0x20, 0x00 in the command list table.
    // At the time of coding, we are actually getting 0x85, 0x26, 0x01
    if(p->iDeviceID[0] != 0x85)
        UEZFailureMsg("Invalid Touchscreen Device ID Code");
    
    // Initialize sequence for the CTP controller
    // 1. Set Command Reload Disable = 1 (0x42H, PA1 = 0x02)
    dataOut[0] = 0x42;
    dataOut[1] = 0x02;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            2,
            50);
    
    // 2. Delay 1ms
    UEZTaskDelay(1);
    
    // 3. Set Command MICRO OFF (0x35H, PA1 = 0x02) to turn on internal MCU
    dataOut[0] = 0x35;
    dataOut[1] = 0x02;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            2,
            50);
    
    // 4. Delay 1ms
    UEZTaskDelay(1);

    // 5. Set command ROMRDY (0x36H, PA1 = 0x0FH, PA2 = 0x53H) to turn on  
    // internal flash.
    dataOut[0] = 0x36;
    dataOut[1] = 0x0F;
    dataOut[2] = 0x53;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            3,
            50);

    // 6. Delay 1ms
    UEZTaskDelay(1);

    // 7. Set command Cache Function (0xDDH, PA1 = 0x04H, PA2 = 0x02H)
    dataOut[0] = 0xDD;
    dataOut[1] = 0x06;//0x04;
    dataOut[2] = 0x02;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            3,
            50);

    // 8. Delay 1ms
    UEZTaskDelay(1);

    dataOut[0] = 0x76;
    dataOut[1] = 0x01;
    dataOut[2] = 0x2D;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            3,
            50);

    //Load config??

    // 9. Set command TSSON (0x83H) to starting touch sensing.
    dataOut[0] = 0x83;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            1,
            50);

    // 10. Delay 120ms
    UEZTaskDelay(120);

    // 11. Set command TSSLPOUT (0x81H) to turn on internal power.
    dataOut[0] = 0x81;
    error = UEZI2CWrite(i2c,
            HX8526A_I2C_ADDRESS,
            HX8526A_I2C_SPEED,
            dataOut,
            1,
            50);

    UEZGPIOEnableIRQ(p->iInteruptPin, GPIO_INTERRUPT_FALLING_EDGE);
    if (error != UEZ_ERROR_NONE)
    {
        return error;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_HX8526A_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_HX8526AWorkspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_HX8526A_ApplyCalibration(
        T_HX8526AWorkspace *p,
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
    if (v < 0)
        v = 0;
    if (v >= DISPLAY_WIDTH)
        v = DISPLAY_WIDTH-1;
#endif
    *newX = (TUInt32)x;

#if 0
    v = y - p->iPenOffsetY;
    v *= p->iPenYScaleTop;
    v /= p->iPenYScaleBottom;
    v += p->iPenBaseY;

    if (v < 0)
        v = 0;
    if (v >= DISPLAY_HEIGHT)
        v = DISPLAY_HEIGHT-1;
#endif
    *newY = (TUInt32)y;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_HX8526A_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_HX8526A_Poll(void *aWorkspace, T_uezTSReading *aReading)
{
    T_HX8526AWorkspace *p =
            (T_HX8526AWorkspace *) aWorkspace;
    T_uezError error;
    T_uezDevice i2c;
    TUInt8 dataIn[0x20];
    TUInt8 dataOut[5];
    TUInt32 Read = 0;
    TUInt32 x;
    TUInt32 y;
    static TUInt32 count = 0;

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
            //count++;
            if(count < 50){
                aReading->iFlags = p->iLastTouch;
                count = 0;
            } else {
                //If we have not received an interrupt in a while
                //assume the TS is not being touched
                aReading->iFlags = p->iLastTouch = 0;
            }
            aReading->iX = p->iLastX;
            aReading->iY = p->iLastY;

            TS_HX8526A_ApplyCalibration(
                    p, (p->iLastX), (p->iLastY),
                    (TUInt32 *) &aReading->iX,
                    (TUInt32 *) &aReading->iY);
            UEZI2CClose(i2c);
            return UEZ_ERROR_NONE;
        }else{
            count = 0;
            // receive 4 bytes
            dataOut[0] = 0x86; // Read Last Even
            error = UEZI2CWrite(i2c,
                    HX8526A_I2C_ADDRESS,
                    HX8526A_I2C_SPEED,
                    dataOut,
                    1,
                    50);
            if(error == UEZ_ERROR_NONE){
                    error = UEZI2CRead(i2c,
                    HX8526A_I2C_ADDRESS,
                    HX8526A_I2C_SPEED,
                    dataIn,
                    32,
                    50);
            }

            if(error != UEZ_ERROR_NONE){
                UEZGPIOClearIRQ(p->iInteruptPin);
                UEZGPIOClear(p->iResetPin);
                UEZTaskDelay(1);
                UEZGPIOSet(p->iResetPin);
                break;
            }

            x = (((dataIn[0x0])<<8) | dataIn[0x1]);

            y = (((dataIn[0x2])<<8) | dataIn[0x3]);
#if 0
            dataOut[0] = 0x88;//Clear unread events
            error = UEZI2CWrite(i2c,
                    HX8526A_I2C_ADDRESS,
                    HX8526A_I2C_SPEED,
                    dataOut,
                    1,
                    50);
            if(dataIn[29] > 1 && dataIn[29] <= 5){
              dataIn[3] = 0xff;
            }
#endif                   

            if(dataIn[0x0] != 0xFF){
                (aReading->iFlags) = (p->iLastTouch) = TSFLAG_PEN_DOWN;
                (aReading->iX) = (p->iLastX)= x;
                (aReading->iY) = (p->iLastY)= y;
                if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
                    // Convert X & Y coordinates
                    TS_HX8526A_ApplyCalibration(
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

                TS_HX8526A_ApplyCalibration(
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
 * Routine:  TS_HX8526A_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_HX8526A_CalibrateStart(void *aWorkspace)
{
    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_HX8526A_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_HX8526A_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aWorkspace;

    p->iIsCalibrating = EFalse;

    if (p->iNumCalibratePoints == NUM_CALIBRATE_POINTS_NEEDED)  {
        // Do the calibration algorithm here
        minIX = minOX = minIY = minOY = 0xFFFFFFFF;
        maxIX = maxOX = maxIY = maxOY = 0;

        for (i = 0; i < p->iNumCalibratePoints; i++) {
            // Find range of inputs
            if ((TUInt32) p->iCalibrateReadingsTaken[i].iX < minIX) {
                minIX = p->iCalibrateReadingsTaken[i].iX;
                minOX = p->iCalibrateReadingsExpected[i].iX;
            }
            if ((TUInt32) p->iCalibrateReadingsTaken[i].iX > maxIX) {
                maxIX = p->iCalibrateReadingsTaken[i].iX;
                maxOX = p->iCalibrateReadingsExpected[i].iX;
            }
            if ((TUInt32) p->iCalibrateReadingsTaken[i].iY < minIY) {
                minIY = p->iCalibrateReadingsTaken[i].iY;
                minOY = p->iCalibrateReadingsExpected[i].iY;
            }
            if ((TUInt32) p->iCalibrateReadingsTaken[i].iY > maxIY) {
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
 * Routine:  TS_HX8526A_CalibrateAdd
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
T_uezError TS_HX8526A_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_HX8526AWorkspace *p = (T_HX8526AWorkspace *)aWorkspace;
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

T_uezError Touchscreen_HX8526A_Create(const char *aName,
        const char *aI2CBus,
        T_uezGPIOPortPin aInteruptPin,
        T_uezGPIOPortPin aResetPin)
{
    T_uezError error;
    T_uezDevice device;
    T_uezDeviceWorkspace *workspace;

    T_HX8526AWorkspace *p;

    error = UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_HX8526A_Interface,
            &device, &workspace);

    p = (T_HX8526AWorkspace *)workspace;

    strcpy(p->iI2CBus, aI2CBus);
    p->iInteruptPin = aInteruptPin;
    p->iResetPin = aResetPin;

    TS_HX8526A_Configure((T_uezDeviceWorkspace*)p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_HX8526A_Interface = {
        {
                // Common interface
                "Touchscreen:Himax:HX8526A",
                0x0100,
                TS_HX8526A_InitializeWorkspace,
                sizeof(T_HX8526AWorkspace),
        },

        // Functions
        TS_HX8526A_Open,
        TS_HX8526A_Close,
        TS_HX8526A_Poll,

        TS_HX8526A_CalibrateStart,
        TS_HX8526A_CalibrateAdd,
        TS_HX8526A_CalibrateEnd,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  HX8526-A_TouchScreen.c
 *-------------------------------------------------------------------------*/
