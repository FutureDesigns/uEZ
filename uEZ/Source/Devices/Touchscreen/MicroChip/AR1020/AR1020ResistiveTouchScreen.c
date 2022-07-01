/*-------------------------------------------------------------------------*
 * File:  AR1020ResistiveTouchScreen.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the MicroChip AR1020 Touchscreen
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
//#include <HAL/GPIO.h>
#include "uEZI2C.h"
#include <Device/I2CBus.h>
#include "AR1020ResistiveTouchScreen.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

#define AR1020_I2C_SPEED                100 //kHz
#define AR1020_I2C_ADDRESS              (0x9A>>1)


//AR1020 Registers              //Address Offset    //Values                   //Default Value
#define TouchThreshold          0x02    //          0-255                      0xC5   60
#define SensitivityFilter       0x03    //          0-255                      0x04
#define SamplingFast            0x04    //          1,2,4,8,16,32,64,128       0x04
#define SamplingSlow            0x05    //          1,2,4,8,16,32,64,128       0x10
#define AccuracyFilterFast      0x06    //          1-8                        0x02
#define AccuracyFilterSlow      0x07    //          1-8                        0x08
#define SpeedThreshold          0x08    //          0-255                      0x04
#define SleepDelay              0x0A    //          0-255                      0x64
#define PenUpDelay              0x0B    //          0-255                      0x80
#define TouchMode               0x0C    // PD2, PD1, PD0, PM1, PM0, PU2, PU1, PU0 0xB1
#define TouchOptions            0x0D    //          -,-,-,-,-,-,48W,CCE        0x00
#define CalibrationInset        0x0E    //          0-40                       0x19
#define PenStateReportDelay     0x0F    //          0-255                      0xC8
#define TouchReportDelay        0x11    //          0-255                      0x00

//AR1020 Commands
#define EnableTouch                     0x12
#define DisableTouch                    0x13
#define CalibrateMode                   0x14
#define RegisterRead                    0x20
#define RegisterWrite                   0x21
#define RegisterStartAddressRequest     0x22
#define RegistersWriteEEPROM            0x23
#define EEPROMRead                      0x28
#define EEPROMWrite                     0x29
#define EEPROMWriteToRegisters          0x2B


/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_MC_AR1020_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MC_AR1020_InitializeWorkspace(void *aW)
{
    T_MC_AR1020Workspace *p = (T_MC_AR1020Workspace *)aW;
    
    p->aNumOpen = 0;
    p->iHaveCalibration = EFalse;
    p->iLastTouch = 0;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MC_AR1020_Open(void *aW)
{
    T_MC_AR1020Workspace *p = (T_MC_AR1020Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;
    I2C_Request r;
    T_uezDevice i2c2;
    TUInt8 dataout[4] = {0x00, 0x55,0x01,EnableTouch};
    TUInt8 datain[4] = {0xff, 0xff, 0xff, 0xff};
    TUInt8 commandToSend[8] = {0x00, 0x55,0x05,RegisterWrite,0x00, 0x00, 0x01,0x60 };
    TUInt8 regstart[6];

    HAL_GPIOPort **p_gpio;
    const TUInt32 pin = 15;
    TUInt32 Read = 0;

    HALInterfaceFind("GPIO2", (T_halWorkspace **)&p_gpio);

    (*p_gpio)->SetInputMode(p_gpio, 1<<pin);
    (*p_gpio)->SetMux(p_gpio, pin, 0); // set to GPIO
    (*p_gpio)->Read(p_gpio, 1<<pin, &Read);

    if(p->aNumOpen == 0)
    {

      error = UEZI2COpen("I2C2", &i2c2);

            
      error = UEZI2CWrite(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, dataout, 4, UEZ_TIMEOUT_INFINITE);

      while(Read == 0)
      {
        (*p_gpio)->Read(p_gpio, 1<<pin, &Read);        
      }
      error = UEZI2CRead(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, datain, 5, UEZ_TIMEOUT_INFINITE);

      //send command to change TouchThreshold to 0x60
//      to write a register first send RegisterStartAddressRequest     0x22
//      calculate the address by adding the offset of the register to the start address TouchThreshold 0x02
//      issue the register write command RegisterWrite 0x21

      dataout[0] = 0;
      dataout[3] = RegisterStartAddressRequest;
//get start address
      error = UEZI2CWrite(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, dataout, 4, UEZ_TIMEOUT_INFINITE);

       while(Read == 0)
      {
        (*p_gpio)->Read(p_gpio, 1<<pin, &Read);        
      }
      error = UEZI2CRead(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, regstart, 6, UEZ_TIMEOUT_INFINITE);
//

      commandToSend[5] = regstart[4]+ TouchThreshold;
      error = UEZI2CWrite(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, commandToSend, 8, UEZ_TIMEOUT_INFINITE);

      while(Read == 0)
      {
        (*p_gpio)->Read(p_gpio, 1<<pin, &Read);        
      }
      error = UEZI2CRead(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, datain, 4, UEZ_TIMEOUT_INFINITE);

      commandToSend[5] = SensitivityFilter;
      commandToSend[6] = 1;
      commandToSend[7] = 10;
      Read = 0;

       error = UEZI2CWrite(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, commandToSend, 8, UEZ_TIMEOUT_INFINITE);

//      while(Read == 0)
//      {
//        (*p_gpio)->Read(p_gpio, 1<<pin, &Read);        
//      }
//      error = UEZI2CRead(i2c2, AR1020_I2C_ADDRESS, AR1020_I2C_SPEED, datain, 4, UEZ_TIMEOUT_INFINITE);

      UEZI2CClose(i2c2);    
    }
    p->aNumOpen++;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MC_AR1020_Close(void *aW)
{
    T_MC_AR1020Workspace *p = (T_MC_AR1020Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
/*T_uezError TS_MC_AR1020_Configure(
                T_uezDeviceWorkspace *aW,
                DEVICE_SPI_BUS **aSPIBus,
                HAL_GPIOPort **aGPIO,
                TUInt32 aGPIOBit)
{
    T_MC_AR1020Workspace *p = (T_MC_AR1020Workspace *)aW;
    p->iSPI = aSPIBus;
    p->iGPIO = aGPIO;
    p->iGPIOBit = aGPIOBit;

    return UEZ_ERROR_NONE;
}*/

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_MC_AR1020Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_MC_AR1020_ApplyCalibration(
                    T_MC_AR1020Workspace *p,
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
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MC_AR1020_Poll(void *aWorkspace, T_uezTSReading *aReading)
{
    T_MC_AR1020Workspace *p =
            (T_MC_AR1020Workspace *) aWorkspace;
    T_uezError error;
    I2C_Request r;
    T_uezDevice i2c2;
    TUInt8 data[5] = {0,0,0,0,0};
    HAL_GPIOPort **p_gpio;
    const TUInt32 pin = 15;
    TUInt32 Read = 0;
	TUInt32 x;
	TUInt32 y;

    HALInterfaceFind("GPIO2", (T_halWorkspace **)&p_gpio);

    (*p_gpio)->SetInputMode(p_gpio, 1<<pin);
    (*p_gpio)->SetMux(p_gpio, pin, 0); // set to GPIO
    (*p_gpio)->Read(p_gpio, 1<<pin, &Read);
    
    if ( Read == 0)
    {
      (aReading->iFlags) = (p->iLastTouch) ;
      return UEZ_ERROR_NONE;
    }
    else
    {
      r.iAddr = AR1020_I2C_ADDRESS;
      r.iSpeed = AR1020_I2C_SPEED;
      r.iWriteData = 0;
      r.iWriteLength = 0;
      r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
      r.iReadData = data;
      r.iReadLength = 5;
      r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

      error = UEZI2COpen("I2C2", &i2c2);

      UEZI2CTransaction(i2c2, &r);
      
      UEZI2CClose(i2c2);

      x = data[4];
      x = (x <<7);
      x = x | data[3];

      y = data[2];
      y = (y <<7);
      y = y | data[1];

      //if(p->iIsCalibrating)
      //for testing printf("%04x _ %04X\r\n", x, y);

      if((data[0] & 0x01)== 0x01)
      {
        (aReading->iFlags) = (p->iLastTouch) = TSFLAG_PEN_DOWN;
        (aReading->iX) = (p->iLastX)= x;
        (aReading->iY) = (p->iLastY)= y;
        if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
                // Convert X & Y coordinates
            TS_MC_AR1020_ApplyCalibration(
                    p, x, y,
                    (TUInt32 *) &aReading->iX,
                    (TUInt32 *) &aReading->iY);
        }
      }
      else
      {
        (aReading->iFlags) = (p->iLastTouch)= 0;
        (aReading->iX) = (p->iLastX);
        (aReading->iY) = (p->iLastY);

        TS_MC_AR1020_ApplyCalibration(
                    p, (p->iLastX), (p->iLastY),
                    (TUInt32 *) &aReading->iX,
                    (TUInt32 *) &aReading->iY);
      }

    return error;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MC_AR1020_CalibrateStart(void *aWorkspace)
{
    T_MC_AR1020Workspace *p = (T_MC_AR1020Workspace *)aWorkspace;
   
    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_MC_AR1020_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_MC_AR1020_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_MC_AR1020Workspace *p = (T_MC_AR1020Workspace *)aWorkspace;

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
 * Routine:  TS_MC_AR1020_CalibrateAdd
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
T_uezError TS_MC_AR1020_CalibrateAdd(
                    void *aWorkspace,
                    const T_uezTSReading *aReadingTaken,
                    const T_uezTSReading *aReadingExpected)
{
    T_MC_AR1020Workspace *p = (T_MC_AR1020Workspace *)aWorkspace;
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

T_uezError Touchscreen_MC_AR1020_Create(const char *aName)
{
    // Need to register touchscreen device
    return UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_MC_AR1020_Interface,
            0, 0);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_MC_AR1020_Interface = {
	{
	    // Common interface
	    "Touchscreen:MC:AR1020",
	    0x0100,
	    TS_MC_AR1020_InitializeWorkspace,
	    sizeof(T_MC_AR1020Workspace),
	},
	
    // Functions
    TS_MC_AR1020_Open,
    TS_MC_AR1020_Close,
    TS_MC_AR1020_Poll,

    TS_MC_AR1020_CalibrateStart,
    TS_MC_AR1020_CalibrateAdd,
    TS_MC_AR1020_CalibrateEnd,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  AR1020ResistiveTouchScreen.c
 *-------------------------------------------------------------------------*/
