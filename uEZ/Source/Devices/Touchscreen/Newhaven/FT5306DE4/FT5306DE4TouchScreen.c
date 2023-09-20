/*-------------------------------------------------------------------------*
* File:  FT5306DE4TouchScreen.c
*-------------------------------------------------------------------------*
* Description:
*      Device implementation of the FT5306DE4 Touchscreen
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
#include <uEZPlatform.h>
#include "FT5306DE4TouchScreen.h"
#include <uEZGPIO.h>
#include <uEZProcessor.h>

/*---------------------------------------------------------------------------*
* Constants:
*---------------------------------------------------------------------------*/

#define FT5306DE4_I2C_SPEED         400 //kHz
#define FT5306DE4_I2C_ADDRESS       (0x70>>1)

#define FT5306DE4_DEVICE_MODE       0x00
#define FT5306DE4_GUEST_ID          0x01
#define FT5306DE4_TD_STATUS         0x02
#define FT5306DE4_TOUCH1_XH         0x03
#define FT5306DE4_TOUCH1_XL         0x04
#define FT5306DE4_TOUCH1_YH         0x05
#define FT5306DE4_TOUCH1_YL         0x06

#define DEVICE_MODE_NORMAL          (0)
#define DEVICE_MODE_TEST            (1<<6)
/*---------------------------------------------------------------------------*
* Types:
*---------------------------------------------------------------------------*/
typedef struct {
  
  const DEVICE_TOUCHSCREEN *iDevice;
  int32_t aNumOpen;
  
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
  
} T_FT5306DE4Workspace;

/*---------------------------------------------------------------------------*
* Prototypes:
*---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_FT5306DE4_Interface;

void TS_FT5306DE4_InterruptISR(
                               void *aInterruptHandlerWorkspace,
                               TUInt32 aPortPins,
                               T_gpioInterruptType aType)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aInterruptHandlerWorkspace;
  _isr_UEZSemaphoreRelease(p->iSemWaitForTouch);
}


/*---------------------------------------------------------------------------*
* Routine:  TS_FT5306DE4_InitializeWorkspace
*---------------------------------------------------------------------------*
* Description:
*      Setup workspace for the TI TSC2046.
* Inputs:
*      void *aW                    -- Particular SPI workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError TS_FT5306DE4_InitializeWorkspace(void *aW)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aW;
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
* Routine:  TS_FT5306DE4_Open
*---------------------------------------------------------------------------*
* Description:
*      The TI TSC2046 is being opened.
* Inputs:
*      void *aW                    -- Particular SPI workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError TS_FT5306DE4_Open(void *aW)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aW;
  
  if(p->aNumOpen == 0)
  {
  }
  p->aNumOpen++;
  
  return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  TS_FT5306DE4_Close
*---------------------------------------------------------------------------*
* Description:
*      The TI TSC2046 is being closed.
* Inputs:
*      void *aW                    -- Particular SPI workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError TS_FT5306DE4_Close(void *aW)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aW;
  p->aNumOpen--;
  
  return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  TS_FT5306DE4_Configure
*---------------------------------------------------------------------------*
* Description:
*      The SPI bus and GPIO device are being declared.
* Inputs:
*      void *aW                    -- Particular SPI workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError TS_FT5306DE4_Configure(T_uezDeviceWorkspace *aW)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aW;
  T_uezDevice i2c;  
  TUInt8 dataIn[0xA];
  TUInt8 dataOut[2] = {0};  
  
  UEZBSPDelayMS(200); // Time between power on and reset must be at least 1ms (Tvdr), 
  // due to differences with compiler task switching set this higher than needed.
  
  UEZSemaphoreGrab(p->iSemWaitForTouch, 0);
  UEZGPIOSetMux(p->iResetPin, 0);//Set to GPIO
  UEZGPIOOutput(p->iResetPin);
  UEZGPIOClear(p->iResetPin);
  UEZBSPDelayMS(10); // Reset Pulse width of 10ms to satisfy 5 ms requirement
  UEZGPIOSet(p->iResetPin);
  UEZBSPDelayMS(300); // Time between reset and init must be at least 300ms
  
  UEZGPIOConfigureInterruptCallback(
                                    p->iInteruptPin,
                                    TS_FT5306DE4_InterruptISR,
                                    p);
  UEZGPIOEnableIRQ(p->iInteruptPin, GPIO_INTERRUPT_FALLING_EDGE);
  
  // Read/setup registers  
  UEZI2COpen(p->iI2CBus, &i2c);
    
  dataOut[0] = 0xA0; // read revision settings register bank
  UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataOut,1,50);    
  UEZI2CRead(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataIn, 0xA,50);
    
  // Now we can check based on Firmware ID or Chip/Vender ID to see what settings to use
  if(dataIn[6] == 0x30){ // firmware of newer 4.3J models?    
    TUInt8 Config80[0xB] = {0x80, // address
      0x28, // Valid touch detect               // Change only this value from default 0x19
      0x50, // Valid touch peak detect
      0xDB, // Touch focus threshold
      0x00, // Surface water threshold
      0x06, // Temperature compensation threshold
      0xA0, // Touch difference threshold
      0x01, // Power Control Mode - Auto Jump
      0x0A, // Monitor status entry delay
      0x06, // Active status period
      0x28, // Idle timeout when in monitor mode
    };
    
    // Only change valid touch detect
    UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,Config80,0x2,50); 
  
    // Uncomment to setup all touch threshold settings in one go
    //UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,Config80,0xB,50); 
    
    dataOut[0] = 0x80; // read settings back to confirm
    UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataOut,1,50);    
    UEZI2CRead(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataIn, 0xA,50);
    
  } else if (dataIn[6] == 0x08){ // firmware of 4.3M models on high brightness model    
  TUInt8 Config80[0xB] = {0x80, // address
      0x28, // Valid touch detect               // Change only this value from default 0x19
      0x3C, // Valid touch peak detect
      0xD5, // Touch focus threshold
      0x01, // Surface water threshold
      0x01, // Temperature compensation threshold
      0xA0, // Touch difference threshold
      0x01, // Power Control Mode - Auto Jump
      0x0A, // Monitor status entry delay
      0x06, // Active status period
      0x28, // Idle timeout when in monitor mode
    };
    // Only change valid touch detect
    UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,Config80,0x2,50); 
  
    // Uncomment to setup all touch threshold settings in one go
    //UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,Config80,0xB,50); 
  
    dataOut[0] = 0x80; // read settings back to confirm
    UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataOut,1,50);    
    UEZI2CRead(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataIn, 0xA,50);
      
  } else if (dataIn[6] == 0x011){ // original model firmware ID, don't change any settings and use as is
    // Settings below are from original version touchscreen
    /*TUInt8 Config80[0xB] = {0x80, // address
      0x19, // Valid touch detect
      0x3C, // Valid touch peak detect
      0x6E, // Touch focus threshold
      0x01, // Surface water threshold
      0x01, // Temperature compensation threshold
      0xA0, // Touch difference threshold
      0x01, // Power Control Mode - Auto Jump
      0x0A, // Monitor status entry delay
      0x06, // Active status period
      0x28, // Idle timeout when in monitor mode
  };*/
    // Uncomment to only change valid touch detect
    //UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,Config80,0x2,50); 
  
    // Uncomment to setup all touch threshold settings in one go
    //UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,Config80,0xB,50); 

    dataOut[0] = 0x80; // read settings back to confirm
    UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataOut,1,50);    
    UEZI2CRead(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataIn, 0xA,50);
  
  } else { // if we see any other IDs we get here
    //Program only touch detect threshold 
    TUInt8 ConfigTD[0x02] = {0x80, // address
      0x28, // Valid touch detect
    }; 
    
    dataOut[0] = 0x80; // address 80 mutual cap touch threshold
    UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataOut,1,50);
    UEZI2CRead(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataIn, 0xA,50);
    if(dataIn[0] < 40) { // raise threshold to 40 if less
      UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,ConfigTD,0x2,50); 

      dataOut[0] = 0x80; // read settings back to confirm
    
      UEZI2CWrite(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataOut,1,50);
      UEZI2CRead(i2c,FT5306DE4_I2C_ADDRESS,FT5306DE4_I2C_SPEED,dataIn, 0xA,50);
    }
  }
  
  UEZI2CClose(i2c);
  return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  TS_FT5306DE4_ApplyCalibration
*---------------------------------------------------------------------------*
* Description:
*      Given the x, y raw coordinates, calculate the proper x, y
*      calibrated coordinates.
* Inputs:
*      T_FT5306DE4Workspace *p    -- Workspace
*      TUInt32 x, y                -- Raw x, y values
*      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
static void TS_FT5306DE4_ApplyCalibration(
                                          T_FT5306DE4Workspace *p,
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
* Routine:  TS_FT5306DE4_Poll
*---------------------------------------------------------------------------*
* Description:
*      Take a reading from the TSC2406 and put in reading structure.
* Inputs:
*      void *aW                    -- Workspace
*      T_uezTSReading *aReading     -- Pointer to final reading
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError TS_FT5306DE4_Poll(void *aWorkspace, T_uezTSReading *aReading)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *) aWorkspace;
  T_uezError error;
  T_uezDevice i2c;
  TUInt8 dataIn[0x30];
  TUInt8 dataOut[2] = {0};
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
  
  if ( Read == 0)
  {
    aReading->iFlags = p->iLastTouch;
    aReading->iX = p->iLastX;
    aReading->iY = p->iLastY;
    
    TS_FT5306DE4_ApplyCalibration(
                                  p, (p->iLastX), (p->iLastY),
                                  (TUInt32 *) &aReading->iX,
                                  (TUInt32 *) &aReading->iY);
    UEZI2CClose(i2c);
    return UEZ_ERROR_NONE;
  }
  else
  {
    dataOut[0] = 0x00;
    error = UEZI2CWrite(i2c,
                        FT5306DE4_I2C_ADDRESS,
                        FT5306DE4_I2C_SPEED,
                        dataOut,
                        1,
                        50);
    
    error = UEZI2CRead(i2c,
                       FT5306DE4_I2C_ADDRESS,
                       FT5306DE4_I2C_SPEED,
                       dataIn,
                       0xC, // only read first 2 touches
                       50);
    
    x = (((dataIn[3] & 0x0F)<< 8) | dataIn[4]); // grab first touch coordinates    
    y = (((dataIn[5] & 0x0F)<< 8) | dataIn[6]);
    
    //Now decide if pen down, holding, or pen up based on driver flag output
    if((dataIn[3] & 0xC0) == 0x80) { // pen hold, counts as pen down in uEZ until hold is supported
      (aReading->iFlags) = (p->iLastTouch) = TSFLAG_PEN_DOWN;
      (aReading->iX) = (p->iLastX)= x;
      (aReading->iY) = (p->iLastY)= y;
      if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
        TS_FT5306DE4_ApplyCalibration(p, x, y,
                                      (TUInt32 *) &aReading->iX,
                                      (TUInt32 *) &aReading->iY);
      }
    } else if((dataIn[3] & 0xC0) == 0x40) { // pen up always
      (aReading->iFlags) = (p->iLastTouch)= 0;
      (aReading->iX) = (p->iLastX);
      (aReading->iY) = (p->iLastY);
      if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {      
        TS_FT5306DE4_ApplyCalibration(p, (p->iLastX), (p->iLastY),
                                      (TUInt32 *) &aReading->iX,
                                      (TUInt32 *) &aReading->iY);
      }
    } else { // pen down 
      if(dataIn[2] >= 0x1){ // at pen down      
        (aReading->iFlags) = (p->iLastTouch) = TSFLAG_PEN_DOWN;
        (aReading->iX) = (p->iLastX)= x;
        (aReading->iY) = (p->iLastY)= y;
        if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {      
          TS_FT5306DE4_ApplyCalibration(p, x, y,
                                        (TUInt32 *) &aReading->iX,
                                        (TUInt32 *) &aReading->iY);
        }
      }
    }
    
    UEZGPIOClearIRQ(p->iInteruptPin);
    UEZI2CClose(i2c);
    return error;
  }
}

/*---------------------------------------------------------------------------*
* Routine:  TS_FT5306DE4_CalibrateStart
*---------------------------------------------------------------------------*
* Description:
*      Start calibration mode.  All readings will now report raw data.
* Inputs:
*      void *aW                    -- Workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError TS_FT5306DE4_CalibrateStart(void *aWorkspace)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aWorkspace;
  
  p->iIsCalibrating = ETrue;
  p->iNumCalibratePoints = 0;
  
  return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  TS_FT5306DE4_CalibrateEnd
*---------------------------------------------------------------------------*
* Description:
*      Calibration is complete.  Compute the calibration matrix and use.
* Inputs:
*      void *aW                    -- Workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError TS_FT5306DE4_CalibrateEnd(void *aWorkspace)
{
  TUInt32 minIX, maxIX, minOX, maxOX;
  TUInt32 minIY, maxIY, minOY, maxOY;
  TUInt32 i;
  
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aWorkspace;
  
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
* Routine:  TS_FT5306DE4_CalibrateAdd
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
T_uezError TS_FT5306DE4_CalibrateAdd(
                                     void *aWorkspace,
                                     const T_uezTSReading *aReadingTaken,
                                     const T_uezTSReading *aReadingExpected)
{
  T_FT5306DE4Workspace *p = (T_FT5306DE4Workspace *)aWorkspace;
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

T_uezError Touchscreen_FT5306DE4_Create(const char *aName,
                                        const char *aI2CBus,
                                        T_uezGPIOPortPin aInteruptPin,
                                        T_uezGPIOPortPin aResetPin)
{
  T_uezError error;
  T_uezDevice device;
  T_uezDeviceWorkspace *workspace;
  
  T_FT5306DE4Workspace *p;
  
  error = UEZDeviceTableRegister(aName,
                                 (T_uezDeviceInterface *)&Touchscreen_FT5306DE4_Interface,
                                 &device, &workspace);
  
  p = (T_FT5306DE4Workspace *)workspace;
  
  strcpy(p->iI2CBus, aI2CBus);
  p->iResetPin = aResetPin;
  p->iInteruptPin = aInteruptPin;
  
  TS_FT5306DE4_Configure((void *)p);
  
  return error;
}

/*---------------------------------------------------------------------------*
* Device Interface table:
*---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_FT5306DE4_Interface = {
  {
    // Common interface
    "Touchscreen:MC:FT5306DE4",
    0x0100,
    TS_FT5306DE4_InitializeWorkspace,
    sizeof(T_FT5306DE4Workspace),
  },
  
  // Functions
  TS_FT5306DE4_Open,
  TS_FT5306DE4_Close,
  TS_FT5306DE4_Poll,
  
  TS_FT5306DE4_CalibrateStart,
  TS_FT5306DE4_CalibrateAdd,
  TS_FT5306DE4_CalibrateEnd,
} ;

/*-------------------------------------------------------------------------*
* End of File:  FT5306DE4TouchScreen.c
*-------------------------------------------------------------------------*/
