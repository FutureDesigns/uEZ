/*-------------------------------------------------------------------------*
 * File:  ST_LIS3DH_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of an ST LIS3DH Accelerometer.
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "ST_LIS3DH_I2C.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Accelerometer *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    AccelerometerReading iLastReading;
    AccelerometerReadingFloat iLastReadingFloat;
} T_ST_Accelo_LIS3DH_I2C_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  IReadData
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the data from the I2C address or return an error
 * Inputs:
 *      T_ST_Accelo_LIS3DH_I2C_Workspace *p -- Workspace
 *      TUInt8 *aBuffer             -- Data buffer to hold data
 *      TUInt8 aStart               -- Start byte/register
 *      TUInt8 aNum                 -- Num bytes to read
 *      TUInt32 aTimeout            -- Timeout in making the attempt
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IReadData(
        T_ST_Accelo_LIS3DH_I2C_Workspace *p,
        TUInt8 *aBuffer,
        TUInt8 aStart,
        TUInt8 aNum,
        TUInt32 aTimeout)
{
     I2C_Request r;
     TUInt8 i = 0; 
     TUInt8 temp;

    for (i=0; i<aNum; i++, aStart++)
    {
    r.iAddr = LIS3DH_I2C_ADDR;
    // Write a command to go to the first register
    r.iWriteData = &aStart;
    r.iWriteLength = 1;
    r.iWriteTimeout = aTimeout;

    // Read the number of bytes
    r.iReadData = &temp;
    r.iReadLength = 1;
    r.iReadTimeout = aTimeout;
    r.iSpeed = LIS3DH_I2C_SPEED;

    // Process the I2C and return an error
    (*p->iI2C)->ProcessRequest(p->iI2C, &r);

    aBuffer[i] = temp;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelo_LIS3DH_I2C_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a SPI Bus called SPI0 and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelo_LIS3DH_I2C_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_ST_Accelo_LIS3DH_I2C_Workspace *p = 
        (T_ST_Accelo_LIS3DH_I2C_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iLastReading.iX = 0;
    p->iLastReading.iY = 0;
    p->iLastReading.iZ = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Bosch_Accelo_BMA150_I2C_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Temporary stub  
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelo_LIS3DH_I2C_GetInfo(
        void *aWorkspace,
        AccelerometerInfo *aInfo)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  ICalc10Bit8G
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the G's of the current movement based on the 8G
 *      configuration and reading of x, y, or z.
 * Inputs:
 *      T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p -- Workspace
 *      TUInt8 aMSB                 -- Most significant bits (bits 7..0)
 *      TUInt8 aLSB                 -- Least significant bits (bits 7..6)
 * Outputs:
 *      TInt32                      -- 15.16 signed result in g
 *---------------------------------------------------------------------------*/
// TODO If switching to 8g mode use this conversion function instead
/*static TInt32 ICalc10Bit8G(TUInt8 msb, TUInt8 lsb)
{
    TInt16 vi;
    TInt32 r;

    // Turn the MSB and LSB into a single 16-bit number
    vi = msb << 8;
    vi |= lsb;
    // Resulting number is left justified!

    // The 10-bit number is a [iiii.ffff ff00 0000] number (thus 0001.000000 is +1g)
    // in a 16-bit number.  iiii is 0-8 g, signed value

    // Extend the sign into a 32-bit signed number
    r = vi;

    // Now shift up the number
    // It now looks like this:
    // iiii iiii iiii iiii . ffff ff00 0000 0000
    // or a 15.16 signed number
    r <<= 4;

    return r;
}*/

/*---------------------------------------------------------------------------*
 * Routine:  ICalc10Bit2G
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the G's of the current movement based on the 2G
 *      configuration and reading of x, y, or z.
 * Inputs:
 *      T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p -- Workspace
 *      TUInt8 aMSB                 -- Most significant bits (bits 7..0)
 *      TUInt8 aLSB                 -- Least significant bits (bits 7..6)
 * Outputs:
 *      TInt32                      -- 15.16 signed result in g
 *---------------------------------------------------------------------------*/
static TInt32 ICalc10Bit2G(TUInt8 msb, TUInt8 lsb)
{
    TInt16 vi;
    TInt32 r;

    // Turn the MSB and LSB into a single 16-bit number
    vi = msb << 8;
    vi |= lsb;
    // Resulting number is left justified!
    
    // The 10-bit number is a [ii.ff ffff ff00 0000] number (thus 01.00000000 is +1g)
    // in a 16-bit number.  ii is 0-2 g, signed value
    
    // Extend the sign into a 32-bit signed number
    r = vi;

    // Now shift up the number
    // It now looks like this:
    // iiii iiii iiii iiii . ffff ffff 0000 0000
    // or a 15.16 signed number
    r <<= 2;

    return r;
}

/*---------------------------------------------------------------------------*
 * Routine:  ICalcG
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the G's of the current movement based on the current
 *      configuration and reading of x, y, or z.
 * Inputs:
 *      T_ST_Accelo_LIS3DH_I2C_Workspace *p -- Workspace
 *      TUInt8 aMSB                 -- Most significant bits (bits 7..0)
 *      TUInt8 aLSB                 -- Least significant bits (bits 7..6)
 * Outputs:
 *      float                      -- signed result in g
 *---------------------------------------------------------------------------*/
 // TODO add conversion for other modes such as 4g, 8, etc. This is 2G only!
static inline float ICalcG(
                TUInt8 msb, 
                TUInt8 lsb)
{    
    TInt16 vi; 
    vi = (int16_t) (msb << 8);
    vi = (int16_t) (lsb | vi);

    return (float)vi / (float)16384.0;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelo_LIS3DH_I2C_ReadXYZ
 *---------------------------------------------------------------------------*
 * Description:
 *      Try to get the XYZ reading of the accelerometer
 * Inputs:
 *      void *aW                    -- Workspace
 *      AccelerometerReading *aReading -- Place to store reading
 *      TUInt32 aTimeout            -- Time to wait until reading is ready
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelo_LIS3DH_I2C_ReadXYZ(
        void *aWorkspace, 
        AccelerometerReading *aReading,
        TUInt32 aTimeout)
{
    T_uezError error;
	T_ST_Accelo_LIS3DH_I2C_Workspace *p = (T_ST_Accelo_LIS3DH_I2C_Workspace *)aWorkspace;
	static TUInt8 accdata[18];
    TUInt8 status;

    aReading->iX = 0;
    aReading->iY = 0;
    aReading->iZ = 0;
	
    //return UEZ_ERROR_NOT_SUPPORTED;	 // TODO implement 15.16 signed

    // Allow only one transfer at a time
    error = UEZSemaphoreGrab(p->iSem, aTimeout);
    if (!error) {      	
      memset(accdata, 0xCC, sizeof(accdata));
      error = IReadData(p, accdata, 0x27, 7, aTimeout);
      status = accdata[0];
      if (status & (1<<3)) {
          p->iLastReading = *aReading;
          aReading->iX = ICalc10Bit2G(accdata[2], accdata[1]);
          aReading->iY = ICalc10Bit2G(accdata[4], accdata[3]);
          aReading->iZ = ICalc10Bit2G(accdata[6], accdata[5]);
      }		
      UEZSemaphoreRelease(p->iSem);
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelo_LIS3DH_I2C_ReadXYZ_Float
 *---------------------------------------------------------------------------*
 * Description:
 *      Get single XYZ reading of the accelerometer
 * Inputs:
 *      void *aW                    -- Workspace
 *      aReading                    -- Place to store reading
 *      TUInt32 aTimeout            -- Time to wait until reading is ready
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelo_LIS3DH_I2C_ReadXYZ_Float(
        void *aWorkspace, 
        AccelerometerReadingFloat *aReadingF,
        TUInt32 aTimeout)
{
    T_uezError error;
    T_ST_Accelo_LIS3DH_I2C_Workspace *p = (T_ST_Accelo_LIS3DH_I2C_Workspace *)aWorkspace;
    static TUInt8 accdata[18];
    TUInt8 status;

    aReadingF->iX = 0;
    aReadingF->iY = 0;
    aReadingF->iZ = 0;

    // Allow only one transfer at a time
    error = UEZSemaphoreGrab(p->iSem, aTimeout);
    if (!error) {      	
      memset(accdata, 0xCC, sizeof(accdata));
      error = IReadData(p, accdata, 0x27, 7, aTimeout);
/*
Array Contents After Read
accdata[0] - STATUS_REG (0x27)
accdata[1] - OUTX_L     (0x28)
accdata[2] - OUTX_H     (0x29)
accdata[3] - OUTY_L     (0x2A)
accdata[4] - OUTY_H     (0x2B)
accdata[5] - OUTZ_L     (0x2C)
accdata[6] - OUTZ_H     (0x2D)
*/
      status = accdata[0];
      if (status & (1<<3)) {
          p->iLastReadingFloat = *aReadingF;
          aReadingF->iX = ICalcG(accdata[2], accdata[1]);
          aReadingF->iY = ICalcG(accdata[4], accdata[3]);
          aReadingF->iZ = ICalcG(accdata[6], accdata[5]);
      }		
      UEZSemaphoreRelease(p->iSem);
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelo_LIS3DH_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Program a LIS3DH registers
 * Inputs:
 *      T_ST_Accelo_LIS3DH_I2C_Workspace *p - Workspace
 *      aI2C - i2c bus
 *      reg - register address
 *      setting - byte to program into register
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelo_LIS3DH_Configure(void *aWorkspace, DEVICE_I2C_BUS **aI2C, TUInt8 reg, TUInt8 setting)
{
    I2C_Request r;
    T_uezError error = UEZ_ERROR_NONE;
	T_ST_Accelo_LIS3DH_I2C_Workspace *p = 
          (T_ST_Accelo_LIS3DH_I2C_Workspace *)aWorkspace;
	
    TUInt8 data[2] = { reg, setting};

    p->iI2C = aI2C;
    
    r.iAddr = LIS3DH_I2C_ADDR;
    r.iWriteData = data;
    r.iWriteLength = 2;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = 0;
    r.iSpeed = LIS3DH_I2C_SPEED;

    // Process the I2C and return an error
    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelo_LIS3DH_I2C_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create an accelerometer driver for the Freescale MMA7455.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aI2CBusName -- Name of I2C bus device driver used by this
 *          accelerometer.
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelo_LIS3DH_I2C_Create(
        const char *aName,
        const char *aI2CBusName)
{
    T_uezDeviceWorkspace *p_accel;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the accelerator device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Accelerometer_ST_LIS3DH_via_I2C_Interface,
            0, &p_accel);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
	
	// Addr 0x20 Data 0x97 -- CTRL_REG1 set freq to 1.344KHz/enable all axes
        // We are setting normal mode (10-bit 2s compliment) in CTRL_REG1.
        // We are not setting CTRL_REG4 so we are in ±2 g mode.
    return ST_Accelo_LIS3DH_Configure(p_accel, // set default settings	
            (DEVICE_I2C_BUS **)p_i2c, LIS3DH_REGISTER_CTRL_REG1,
	LIS3DH_CTRL1_ODR_HR_NORM_1_344KHZ_LPM_5_375KHZ | LIS3DH_CTRL1_XYZ_AXIS_EN);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Accelerometer Accelerometer_ST_LIS3DH_via_I2C_Interface = {
    // Common device interface
    {
	    "Accelerometer:ST:LIS3DH:I2C",
	    0x0100,
	    ST_Accelo_LIS3DH_I2C_InitializeWorkspace,
	    sizeof(T_ST_Accelo_LIS3DH_I2C_Workspace),
	},
	
    // Functions
    ST_Accelo_LIS3DH_I2C_GetInfo,
    ST_Accelo_LIS3DH_I2C_ReadXYZ,
    ST_Accelo_LIS3DH_I2C_ReadXYZ_Float,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  ST_LIS3DH_driver.c
 *-------------------------------------------------------------------------*/
