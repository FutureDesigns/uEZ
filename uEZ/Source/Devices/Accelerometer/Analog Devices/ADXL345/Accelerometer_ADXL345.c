/*-------------------------------------------------------------------------*
 * File:  Accelerometer_ADXL345.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of an ST ADXL345 Accelerometer.
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
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "Accelerometer_ADXL345.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

#define ADXL345_ID_REG              0x00
#define ADXL345_THRESH_TAP_REG      0x1D
#define ADXL345_OFSX_REG            0x1E
#define ADXL345_OFSY_REG            0x1F
#define ADXL345_OFSZ_REG            0x20
#define ADXL345_DUR_REG             0x21
#define ADXL345_LATENT_REG          0x22
#define ADXL345_WINDOW_REG          0x23
#define ADXL345_THRESH_ACT_REG      0x24
#define ADXL345_THRESH_INACT_REG    0x25
#define ADXL345_TIME_INACT_REG      0x26
#define ADXL345_ACT_INACT_CTL_REG   0x27
#define ADXL345_THRESH_FF_REG       0x28
#define ADXL345_TIME_FF_REG         0x29
#define ADXL345_TAP_AXES_REG        0x2A
#define ADXL345_ACT_TAP_STATUS_REG  0x2B
#define ADXL345_BW_RATE_REG         0x2C
#define ADXL345_POWER_CTL_REG       0x2D
#define ADXL345_INT_ENABLE_REG      0x2E
#define ADXL345_INT_MAP_REG         0x2F
#define ADXL345_INT_SOURCE_REG      0x30
#define ADXL345_DATA_FORMAT_REG     0x31
#define ADXL345_DATAX0_REG          0x32
#define ADXL345_DATAX1_REG          0x33
#define ADXL345_DATAY0_REG          0x34
#define ADXL345_DATAY1_REG          0x35
#define ADXL345_DATAZ0_REG          0x36
#define ADXL345_DATAZ1_REG          0x37
#define ADXL345_FIFO_CTL_REG        0x38
#define ADXL345_FIFO_STATUS_REG     0x39

#define SCALE_X(x)                (x) // (113*(x))/64    // 1.77
#define SCALE_Y(y)                (y) // (113*(y))/64    // 1.77
#define SCALE_Z(z)                (-(z)) // ( 94*(z))/64    // 1.47

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Accelerometer *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    AccelerometerReading iLastReading;
    TInt32 iZeroX;
    TInt32 iZeroY;
    TInt32 iZeroZ;
    TBool iInit;
} T_Accelerometer_ADXL345_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TInt32 IAxisRead(T_Accelerometer_ADXL345_Workspace *p, TUInt8 aAxisReg);

/*---------------------------------------------------------------------------*
 * Routine:  IReadData
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the data from the I2C address or return an error
 * Inputs:
 *      T_Accelerometer_ADXL345_Workspace *p -- Workspace
 *      TUInt8 *aBuffer             -- Data buffer to hold data
 *      TUInt8 aStart               -- Start byte/register
 *      TUInt8 aNum                 -- Num bytes to read
 *      TUInt32 aTimeout            -- Timeout in making the attempt
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IReadData(
    T_Accelerometer_ADXL345_Workspace *p,
    TUInt8 *aBuffer,
    TUInt8 aStart,
    TUInt8 aNum)
{
    I2C_Request r;
    TUInt8 i = 0;
    TUInt8 temp;

    for (i = 0; i < aNum; i++, aStart++) {
        r.iAddr = ADXL345_ADDR;
        // Write a command to go to the first register
        r.iWriteData = &aStart;
        r.iWriteLength = 1;
        r.iWriteTimeout = 1000;

        // Read the number of bytes
        r.iReadData = &temp;
        r.iReadLength = 1;
        r.iReadTimeout = 1000;
        r.iSpeed = ADXL345_SPEED;

        // Process the I2C and return an error
        (*p->iI2C)->ProcessRequest(p->iI2C, &r);

        aBuffer[i] = temp;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IWriteData
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the data to the I2C address or return an error
 * Inputs:
 *      T_Accelerometer_ADXL345_Workspace *p -- Workspace
 *      TUInt8 *aBuffer             -- Data buffer to hold data
 *      TUInt8 aNum                 -- Num bytes to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IWriteData(
    T_Accelerometer_ADXL345_Workspace *p,
    TUInt8 *aBuffer,
    TUInt8 aNum)
{
    I2C_Request r;

    r.iAddr = ADXL345_ADDR;
    // Write out the data
    r.iWriteData = aBuffer;
    r.iWriteLength = aNum;
    r.iWriteTimeout = 1000;

    // Read no bytes
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = 0;
    r.iSpeed = ADXL345_SPEED;

    // Process the I2C and return an error
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

static TInt32 IAxisRead(T_Accelerometer_ADXL345_Workspace *p, TUInt8 aAxisReg)
{
    TUInt8 buffer[2];
    TInt32 v = 0;
    if (IReadData(p, buffer, aAxisReg, 2) == UEZ_ERROR_NONE) {
        v = (buffer[1] << 8) | buffer[0];
    }
    // Convert from signed 14 bits (10 bits signficant) to 15.16 signed
    v <<= 22;
    v >>= 14;

    return v;
}


static void IInitAccel(T_Accelerometer_ADXL345_Workspace *p)
{
    TUInt8 cmdFormat[2] = { ADXL345_DATA_FORMAT_REG, 0 }; // +/- 2g
    TUInt8 cmdFIFO[2] = { ADXL345_FIFO_CTL_REG, 0 };
    TUInt8 cmdPower[2] = { ADXL345_POWER_CTL_REG, 8 };
    int i;

    IWriteData(p, cmdFormat, 2);
    IWriteData(p, cmdFIFO, 2);
    IWriteData(p, cmdPower, 2);
    p->iZeroX = p->iZeroY = p->iZeroZ = 0;
    for (i=0; i<8; i++) {
        p->iZeroX += IAxisRead(p, ADXL345_DATAX0_REG);
        p->iZeroY += IAxisRead(p, ADXL345_DATAY0_REG);
        p->iZeroZ += IAxisRead(p, ADXL345_DATAZ0_REG);
    }
    p->iZeroX /= 8;
    p->iZeroY /= 8;
    p->iZeroZ /= 8;
    p->iZeroZ -= 65536;  // TODO: This assumes Z is positive down when starts
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelerometer_ADXL345_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a SPI Bus called SPI0 and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelerometer_ADXL345_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Accelerometer_ADXL345_Workspace *p =
        (T_Accelerometer_ADXL345_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iLastReading.iX = 0;
    p->iLastReading.iY = 0;
    p->iLastReading.iZ = 0;
    p->iZeroX = 0;
    p->iZeroY = 0;
    p->iZeroZ = 0;
    p->iInit = EFalse;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Bosch_Accelerometer_BMA150_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Temporary stub
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/

T_uezError ST_Accelerometer_ADXL345_GetInfo(
    void *aWorkspace,
    AccelerometerInfo *aInfo)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelerometer_ADXL345_ReadXYZ
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
T_uezError ST_Accelerometer_ADXL345_ReadXYZ(
    void *aWorkspace,
    AccelerometerReading *aReading,
    TUInt32 aTimeout)
{
    T_Accelerometer_ADXL345_Workspace *p =
        (T_Accelerometer_ADXL345_Workspace *)aWorkspace;

    aReading->iX = 0;
    aReading->iY = 0;
    aReading->iZ = 0;

    if (!p->iInit) {
        IInitAccel(p);
        p->iInit = ETrue;
    }
    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, aTimeout);

    aReading->iX = SCALE_X(IAxisRead(p, ADXL345_DATAX0_REG) - p->iZeroX);
    aReading->iY = SCALE_Y(IAxisRead(p, ADXL345_DATAY0_REG) - p->iZeroY);
    aReading->iZ = SCALE_Z(IAxisRead(p, ADXL345_DATAZ0_REG) - p->iZeroZ);

    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelerometer_ADXL345_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup ADXL345 control registers
 * Inputs:
 *      T_Accelerometer_ADXL345_Workspace *p -- Workspace
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelerometer_ADXL345_Configure(
    void *aWorkspace,
    DEVICE_I2C_BUS **aI2C)
{
    T_Accelerometer_ADXL345_Workspace *p =
        (T_Accelerometer_ADXL345_Workspace *)aWorkspace;

    p->iI2C = aI2C;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ST_Accelerometer_ADXL345_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create an accelerometer driver for the ST ADXL345.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aI2CBusName -- Name of I2C bus device driver used by this
 *          accelerometer.
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError ST_Accelerometer_ADXL345_Create(
        const char *aName,
        const char *aI2CBusName)
{
    T_uezDeviceWorkspace *p_accel;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the accelerator device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Accelerometer_ADXL345_Interface,
            0, &p_accel);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
    return ST_Accelerometer_ADXL345_Configure(p_accel,
            (DEVICE_I2C_BUS **)p_i2c);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Accelerometer Accelerometer_ADXL345_Interface = {
// Common device interface
    {
        "Accelerometer:AnalogDevices:ADXL345",
        0x0100,
        ST_Accelerometer_ADXL345_InitializeWorkspace,
        sizeof(T_Accelerometer_ADXL345_Workspace), },

    // Functions
    ST_Accelerometer_ADXL345_GetInfo,
    ST_Accelerometer_ADXL345_ReadXYZ, };

/*-------------------------------------------------------------------------*
 * End of File:  Accelerometer_ADXL345.c
 *-------------------------------------------------------------------------*/
