/*-------------------------------------------------------------------------*
 * File:  Bosch_BMA150_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a Bosch BMA150 Accelerometer.
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "Bosch_BMA150_I2C.h"

// Note this driver has not been kept up to date or tested in recent memory.

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
} T_Bosch_Accelo_BMA150_I2C_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  IReadData
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the data from the I2C address or return an error
 * Inputs:
 *      T_Bosch_Accelo_BMA150_I2C_Workspace *p -- Workspace
 *      TUInt8 *aBuffer             -- Data buffer to hold data
 *      TUInt8 aStart               -- Start byte/register
 *      TUInt8 aNum                 -- Num bytes to read
 *      TUInt32 aTimeout            -- Timeout in making the attempt
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IReadData(
        T_Bosch_Accelo_BMA150_I2C_Workspace *p,
        TUInt8 *aBuffer,
        TUInt8 aStart,
        TUInt8 aNum,
        TUInt32 aTimeout)
{
    I2C_Request r;

    r.iAddr = BMA150_I2C_ADDR;
    // Write a command to go to the first register
    r.iWriteData = &aStart;
    r.iWriteLength = 1;
    r.iWriteTimeout = aTimeout;

    // Read the number of bytes
    r.iReadData = aBuffer;
    r.iReadLength = aNum;
    r.iReadTimeout = aTimeout;
    r.iSpeed = BMA150_I2C_SPEED;

    // Process the I2C and return an error
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Routine:  ICalcG
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the G's of the current movement based on the current
 *      configuration and reading of x, y, or z.
 * Inputs:
 *      T_Bosch_Accelo_BMA150_I2C_Workspace *p -- Workspace
 *      TUInt8 aMSB                 -- Most significant bits (bits 7..0)
 *      TUInt8 aLSB                 -- Least significant bits (bits 7..6)
 * Outputs:
 *      TInt32                      -- 15.16 signed result in g
 *---------------------------------------------------------------------------*/
static TInt32 ICalcG(
                TUInt8 msb, 
                TUInt8 lsb)
{
    TUInt32 vi;
    TInt32 v;

    vi = msb << 2;
    vi |= (lsb>>6)&0x03;
    vi <<= 22;
    v = vi;
    v >>= 16;

    v *= 2;  // 2 g extremes

    return v;
}

/*---------------------------------------------------------------------------*
 * Routine:  Bosch_Accelo_BMA150_I2C_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a SPI Bus called SPI0 and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Bosch_Accelo_BMA150_I2C_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Bosch_Accelo_BMA150_I2C_Workspace *p = 
        (T_Bosch_Accelo_BMA150_I2C_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Bosch_Accelo_BMA150_I2C_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get info about the unit
 * Inputs:
 *      void *aW                    -- Workspace
 *      AccelerometerReading *aReading -- Place to store reading
 *      TUInt32 aTimeout            -- Time to wait until reading is ready
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError Bosch_Accelo_BMA150_I2C_GetInfo(
        void *aWorkspace,
        AccelerometerInfo *aInfo)
{
    T_uezError error;
    T_Bosch_Accelo_BMA150_I2C_Workspace *p = 
        (T_Bosch_Accelo_BMA150_I2C_Workspace *)aWorkspace;
    TUInt8 data[2];

    // Allow only one transfer at a time
    error = UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (error)
        return error;

    // Read the I2C data or timeout trying
    error = IReadData(p, data, 0, 2, 100);
    if (error)
        return error;

    aInfo->iChipID = data[0] & 3;
    aInfo->iRevision = data[1] ;

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Bosch_Accelo_BMA150_I2C_ReadXYZ
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
T_uezError Bosch_Accelo_BMA150_I2C_ReadXYZ(
        void *aWorkspace, 
        AccelerometerReading *aReading,
        TUInt32 aTimeout)
{
    TUInt32 i;
    T_uezError error;
    T_Bosch_Accelo_BMA150_I2C_Workspace *p = 
        (T_Bosch_Accelo_BMA150_I2C_Workspace *)aWorkspace;
    static TUInt8 accdata[18];

    aReading->iX = 0;
    aReading->iY = 0;
    aReading->iZ = 0;

    // Allow only one transfer at a time
    error = UEZSemaphoreGrab(p->iSem, aTimeout);
    if (error)
        return error;

    for (i=0; i<10; i++) {
        error = IReadData(p, accdata, 0, 16, 100);
        if (accdata[2] & accdata[4] & accdata[6] & 1) {
            aReading->iX = ICalcG(accdata[3], accdata[2]);
            aReading->iY = ICalcG(accdata[5], accdata[4]);
            aReading->iZ = ICalcG(accdata[7], accdata[6]);
            break;
        }
        UEZTaskDelay(2);
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Bosch_Accelo_BMA150_I2C_ReadXYZ_Float(
void *aWorkspace, 
AccelerometerReadingFloat *aReading,
TUInt32 aTimeout)
{
    aReading->iX = 0;
    aReading->iY = 0;
    aReading->iZ = 0;
		
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError Bosch_Accelo_BMA150_Configure(
                void *aWorkspace, 
                DEVICE_I2C_BUS **aI2C)
{
    T_Bosch_Accelo_BMA150_I2C_Workspace *p = 
        (T_Bosch_Accelo_BMA150_I2C_Workspace *)aWorkspace;
    p->iI2C = aI2C;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Bosch_Accelo_BMA150_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create an accelerometer driver for the Bosch BMA150.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aI2CBusName -- Name of I2C bus device driver used by this
 *          accelerometer.
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError Bosch_Accelerometer_BMA150_Create(
        const char *aName,
        const char *aI2CBusName)
{
    T_uezDeviceWorkspace *p_accel;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the accelerator device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Accelerometer_Bosch_BMA150_via_I2C_Interface,
            0, &p_accel);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
    return Bosch_Accelo_BMA150_Configure(p_accel,
            (DEVICE_I2C_BUS **)p_i2c);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Accelerometer Accelerometer_Bosch_BMA150_via_I2C_Interface = {
    // Common device interface
    {
	    "Accelerometer:Bosch:BMA150:I2C",
	    0x0100,
	    Bosch_Accelo_BMA150_I2C_InitializeWorkspace,
	    sizeof(T_Bosch_Accelo_BMA150_I2C_Workspace),
	},
	
    // Functions
    Bosch_Accelo_BMA150_I2C_GetInfo,
    Bosch_Accelo_BMA150_I2C_ReadXYZ,
    Bosch_Accelo_BMA150_I2C_ReadXYZ_Float,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  Bosch_BMA150_driver.c
 *-------------------------------------------------------------------------*/
