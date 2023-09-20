/*-------------------------------------------------------------------------*
 * File:  Freescale_MMA7455_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of an ST MMA7455 Accelerometer.
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
#include <stdio.h>
#include "Freescale_MMA7455.h"
#include <uEZDeviceTable.h>
#ifndef FREESCALE_MMA7455_G_MODE
//#define FREESCALE_MMA7455_G_MODE 2
#define FREESCALE_MMA7455_G_MODE 8
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

// Internal registers
#define Freescale_MMA7455_REG_XOUT10_LSB   0x00
#define Freescale_MMA7455_REG_XOUT10_MSB   0x01
#define Freescale_MMA7455_REG_YOUT10_LSB   0x02
#define Freescale_MMA7455_REG_YOUT10_MSB   0x03
#define Freescale_MMA7455_REG_ZOUT10_LSB   0x04
#define Freescale_MMA7455_REG_ZOUT10_MSB   0x05
#define Freescale_MMA7455_REG_XOUT8        0x06
#define Freescale_MMA7455_REG_YOUT8        0x07
#define Freescale_MMA7455_REG_ZOUT8        0x08
#define Freescale_MMA7455_REG_STATUS       0x09
#define   Freescale_MMA7455_STATUS_DRDY      (1<<0)  // non-zero = ready
#define   Freescale_MMA7455_STATUS_DOVR      (1<<1)  // non-zero = data is over written
#define   Freescale_MMA7455_STATUS_PERR      (1<<2)  // non-zero = parity error in trim data (only self test)
#define Freescale_MMA7455_REG_DRIFT_X_LSB  0x10
#define Freescale_MMA7455_REG_DRIFT_X_MSB  0x11
#define Freescale_MMA7455_REG_DRIFT_Y_LSB  0x12
#define Freescale_MMA7455_REG_DRIFT_Y_MSB  0x13
#define Freescale_MMA7455_REG_DRIFT_Z_LSB  0x14
#define Freescale_MMA7455_REG_DRIFT_Z_MSB  0x15
#define Freescale_MMA7455_REG_MODE         0x16
#define   Freescale_MMA7455_MODE_MODE_BITS            (3<<0)
#define   Freescale_MMA7455_MODE_MODE_STANDBY         (0<<0)
#define   Freescale_MMA7455_MODE_MODE_MEASUREMENT     (1<<0)
#define   Freescale_MMA7455_MODE_MODE_LEVEL_DETECTION (2<<0)
#define   Freescale_MMA7455_MODE_MODE_PULSE_DETECTION (3<<0)
#define   Freescale_MMA7455_MODE_GLVL_2G              (1<<2)
#define   Freescale_MMA7455_MODE_GLVL_4G              (2<<2)
#define   Freescale_MMA7455_MODE_GLVL_8G              (0<<2)
#define   Freescale_MMA7455_MODE_STON                 (1<<4)  // non-zero = self test
#define   Freescale_MMA7455_MODE_SPI3W_4WIRE          (0<<5)
#define   Freescale_MMA7455_MODE_SPI3W_3WIRE          (1<<5)
#define   Freescale_MMA7455_MODE_DRPD_OUTPUT          (0<<6)
#define   Freescale_MMA7455_MODE_DRPD_NOT_OUTPUT      (1<<6)
#define Freescale_MMA7455_REG_CONTROL1     0x18
#define Freescale_MMA7455_REG_CONTROL2     0x19
#define Freescale_MMA7455_REG_PULSE_DURATION            0x1C
#define Freescale_MMA7455_REG_LATENCY_TIME              0x1D
#define Freescale_MMA7455_REG_TIME_WINDOW_2ND_PULSE     0x1E

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Accelerometer *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    AccelerometerReading iLastReading;
    TUInt8 iI2CAddress;
} T_Accelerometer_Freescale_MMA7455_I2C_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  IReadData
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the data from the I2C address or return an error
 * Inputs:
 *      T_Accelerometer_Freescale_MMA7455_Workspace *p -- Workspace
 *      TUInt8 *aBuffer             -- Data buffer to hold data
 *      TUInt8 aStart               -- Start byte/register
 *      TUInt8 aNum                 -- Num bytes to read
 *      TUInt32 aTimeout            -- Timeout in making the attempt
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IReadData(
        T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p,
        TUInt8 *aBuffer,
        TUInt8 aStart,
        TUInt8 aNum,
        TUInt32 aTimeout)
{
    I2C_Request r;
    //TUInt8 i = 0;
    //TUInt8 temp;
    T_uezError error = UEZ_ERROR_NONE;

#if 1
    //    for (i=0; i<aNum; i++, aStart++)
    //    {
    r.iAddr = p->iI2CAddress;
    // Write a command to go to the first register
    r.iWriteData = &aStart;
    r.iWriteLength = 1;
    r.iWriteTimeout = aTimeout;

    // Read the number of bytes
    r.iReadData = aBuffer; //&temp;
    r.iReadLength = aNum; // 1;
    r.iReadTimeout = aTimeout;
    r.iSpeed = MMA7455_I2C_SPEED;

    // Process the I2C and return an error
    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    //        if (error)
    //            break;

    //        aBuffer[i] = temp;
    //    }
#else
    for (i=0; i<aNum; i++, aStart++)
    {
        r.iAddr = p->iI2CAddress;
        // Write a command to go to the first register
        r.iWriteData = &aStart;
        r.iWriteLength = 1;
        r.iWriteTimeout = aTimeout;

        // Read the number of bytes
        r.iReadData = aBuffer; //&temp;
        r.iReadLength = aNum; // 1;
        r.iReadTimeout = aTimeout;
        r.iSpeed = MMA7455_I2C_SPEED;

        // Process the I2C and return an error
        error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
        if (error)
        break;

        aBuffer[i] = temp;
    }
#endif
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ICalc10Bit8G
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the G's of the current movement based on the current
 *      configuration and reading of x, y, or z.
 * Inputs:
 *      T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p -- Workspace
 *      TUInt8 aMSB                 -- Most significant bits (bits 7..0)
 *      TUInt8 aLSB                 -- Least significant bits (bits 7..6)
 * Outputs:
 *      TInt32                      -- 15.16 signed result in g
 *---------------------------------------------------------------------------*/
static TInt32 ICalc10Bit8G(TUInt8 msb, TUInt8 lsb)
{
    TInt16 vi;
    TInt32 r;

    // Turn the MSB and LSB into a single 16-bit number
    vi = msb << 8;
    vi |= lsb;

    // The 10-bit number is a [0000 00ii ii.ff ffff] number (thus 0001.000000 is +1g)
    // in a 16-bit number.  iiii is 0-8 g, signed value
    vi <<= 6;

    // Now the number is a [iiii.ffff ff00 0000] number
    // Extend the sign into a 32-bit signed number
    r = vi;

    // Now shift up the number
    // It now looks like this:
    // iiii iiii iiii iiii . ffff ff00 0000 0000
    // or a 15.16 signed number
    r <<= 4;

    return r;
}

/*---------------------------------------------------------------------------*
 * Routine:  ICalc8Bit2G
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the G's of the current movement based on the current
 *      configuration and reading of x, y, or z.
 * Inputs:
 *      T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p -- Workspace
 *      TUInt8 aMSB                 -- 8-bit value
 * Outputs:
 *      TInt32                      -- 15.16 signed result in g
 *---------------------------------------------------------------------------*/
#if (FREESCALE_MMA7455_G_MODE==2)
static TInt32 ICalc8Bit2G(TUInt8 value)
{
    TInt32 r;

    // Turn the value into a signed 32-bit number
    // 0x40 = 1g, 0xC0 = -1g
    // 0100000    0x11000000
    // Or, in the format:
    //  [siffffff] where s = sign bit, i = integer, fffff = fractional amount
    // After 32 bit conversion:
    //  [siffffff 00000000 00000000 00000000]
    r = (value << 24);

    // Just shift down and extend the sign
    //  [ssssssss sssssssi ffffff00 00000000]
    r >>= 14;

    return r;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  Accelerometer_Freescale_MMA7455_I2C_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a SPI Bus called SPI0 and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Accelerometer_Freescale_MMA7455_I2C_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p =
            (T_Accelerometer_Freescale_MMA7455_I2C_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
#if UEZ_REGISTER
    UEZSemaphoreSetName(p->iSem, "Accelerometer", "\0");
#endif

    p->iLastReading.iX = 0;
    p->iLastReading.iY = 0;
    p->iLastReading.iZ = 0;
    p->iI2CAddress = MMA7455_I2C_ADDR;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Bosch_Accelo_BMA150_I2C_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Temporary stub  
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/

T_uezError Accelerometer_Freescale_MMA7455_I2C_GetInfo(
        void *aWorkspace,
        AccelerometerInfo *aInfo)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Accelerometer_Freescale_MMA7455_I2C_ReadXYZ
 *---------------------------------------------------------------------------*
 * Description:
 *      Try to get the XYZ reading of the accelerometer. Returns a single reading.
 * Inputs:
 *      void *aW                    -- Workspace
 *      AccelerometerReading *aReading -- Place to store reading
 *      TUInt32 aTimeout            -- Time to wait until reading is ready
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError Accelerometer_Freescale_MMA7455_I2C_ReadXYZ(
        void *aWorkspace,
        AccelerometerReading *aReading,
        TUInt32 aTimeout)
{
    TUInt8 status;
    T_uezError error;
    T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p =
            (T_Accelerometer_Freescale_MMA7455_I2C_Workspace *)aWorkspace;
    TUInt8 accdata[0x20];
    TUInt32 start;
    TBool success = EFalse;

    aReading->iX = 0;
    aReading->iY = 0;
    aReading->iZ = 0;

    // Allow only one transfer at a time
    error = UEZSemaphoreGrab(p->iSem, aTimeout);
    if (error)
        return error;

    // A reading is available every 4 ms
    start = UEZTickCounterGet();
    while (UEZTickCounterGetDelta(start) < 10) { // try up to 10 ms
        memset(accdata, 0xCC, sizeof(accdata));
        error = IReadData(p, accdata + Freescale_MMA7455_REG_STATUS, 0x00, 1,
                100);
        if (!error) {
            status = accdata[Freescale_MMA7455_REG_STATUS];
            if (status & Freescale_MMA7455_STATUS_DRDY) {
                error = IReadData(p, accdata, 0x00, 9, 100);
                if (!error) {
#if (FREESCALE_MMA7455_G_MODE==8)
#if 0
                    printf("x:%02X%02X y:%02X%02X z:%02X%02X\r\n",
                            accdata[Freescale_MMA7455_REG_XOUT10_MSB],
                            accdata[Freescale_MMA7455_REG_XOUT10_LSB],
                            accdata[Freescale_MMA7455_REG_YOUT10_MSB],
                            accdata[Freescale_MMA7455_REG_YOUT10_LSB],
                            accdata[Freescale_MMA7455_REG_ZOUT10_MSB],
                            accdata[Freescale_MMA7455_REG_ZOUT10_LSB]);
#endif												
                    aReading->iX = ICalc10Bit8G(
                            accdata[Freescale_MMA7455_REG_XOUT10_MSB],
                            accdata[Freescale_MMA7455_REG_XOUT10_LSB]);
                    aReading->iY = ICalc10Bit8G(
                            accdata[Freescale_MMA7455_REG_YOUT10_MSB],
                            accdata[Freescale_MMA7455_REG_YOUT10_LSB]);
                    aReading->iZ = ICalc10Bit8G(
                            accdata[Freescale_MMA7455_REG_ZOUT10_MSB],
                            accdata[Freescale_MMA7455_REG_ZOUT10_LSB]);
#elif (FREESCALE_MMA7455_G_MODE==2)
                    aReading->iX = ICalc8Bit2G(
                            accdata[Freescale_MMA7455_REG_XOUT8]);
                    aReading->iY = ICalc8Bit2G(
                            accdata[Freescale_MMA7455_REG_YOUT8]);
                    aReading->iZ = ICalc8Bit2G(
                            accdata[Freescale_MMA7455_REG_ZOUT8]);
#endif
                    p->iLastReading = *aReading;
                    success = ETrue;
                    break; // break if successful
                }
            }
        }
        // Let another task run awhile as we wait for a response
        UEZTaskDelay(1);
    }
    if (!success) {
        // Go with the last reading
        *aReading = p->iLastReading;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}
/*---------------------------------------------------------------------------*
 * Routine:  Accelerometer_Freescale_MMA7455_I2C_ReadXYZ_Float
 *---------------------------------------------------------------------------*
 * Description:
 *      Try to get the XYZ reading of the accelerometer in float. Returns a single reading.
 * Inputs:
 *      void *aW                    -- Workspace
 *      AccelerometerReading *aReading -- Place to store reading
 *      TUInt32 aTimeout            -- Time to wait until reading is ready
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError Accelerometer_Freescale_MMA7455_I2C_ReadXYZ_Float(
        void *aWorkspace, 
        AccelerometerReadingFloat *aReading,
        TUInt32 aTimeout)
{
    /*T_uezError error;
	T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p =
        (T_Accelerometer_Freescale_MMA7455_I2C_Workspace *)aWorkspace;*/
	
    aReading->iX = 0;
    aReading->iY = 0;
    aReading->iZ = 0;
    // Insert floating point version here if needed. But this part is EOL now.
/*
    // Allow only one transfer at a time
    error = UEZSemaphoreGrab(p->iSem, aTimeout);
    if (!error) {

	UEZSemaphoreRelease(p->iSem);
    }
    return error;*/
    
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Accelerometer_Freescale_MMA7455_I2C_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup MMA7455 control registers
 * Inputs:
 *      T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p -- Workspace
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_TIMEOUT if no
 *                                      reading.
 *---------------------------------------------------------------------------*/
T_uezError Accelerometer_Freescale_MMA7455_I2C_Configure(
        void *aWorkspace,
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr)
{
    // List of registers to write
    T_Accelerometer_Freescale_MMA7455_I2C_Workspace *p =
            (T_Accelerometer_Freescale_MMA7455_I2C_Workspace *)aWorkspace;
    const TUInt8 reg[][2] = { //{regaddr, data}
            { Freescale_MMA7455_REG_CONTROL1, 0x80 }, {
                    Freescale_MMA7455_REG_CONTROL2,
                    0x00 }, {
                    Freescale_MMA7455_REG_MODE,
                    (Freescale_MMA7455_MODE_MODE_MEASUREMENT
#if (FREESCALE_MMA7455_G_MODE==8)
                            | Freescale_MMA7455_MODE_GLVL_8G
#elif (FREESCALE_MMA7455_G_MODE==2)
                            |Freescale_MMA7455_MODE_GLVL_2G
#endif
                            | Freescale_MMA7455_MODE_DRPD_OUTPUT) }, {
                    Freescale_MMA7455_REG_DRIFT_X_LSB,
                    0x00 }, { Freescale_MMA7455_REG_DRIFT_X_MSB, 0x00 }, {
                    Freescale_MMA7455_REG_DRIFT_Y_LSB,
                    0x00 }, { Freescale_MMA7455_REG_DRIFT_Y_MSB, 0x00 }, {
                    Freescale_MMA7455_REG_DRIFT_Z_LSB,
                    0x00 }, { Freescale_MMA7455_REG_DRIFT_Z_MSB, 0x00 }, {
                    Freescale_MMA7455_REG_PULSE_DURATION,
                    0x00 }, { Freescale_MMA7455_REG_LATENCY_TIME, 0x00 }, {
                    Freescale_MMA7455_REG_TIME_WINDOW_2ND_PULSE,
                    0x00 }, { 0, 0 }, };
    TUInt8 i = 0;
    I2C_Request r;
    T_uezError error = UEZ_ERROR_NONE;

    p->iI2CAddress = aI2CAddr;
    p->iI2C = aI2C;
    r.iAddr = p->iI2CAddress;
    r.iWriteLength = 2;
    r.iWriteTimeout = 1000;
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = 0;
    r.iSpeed = MMA7455_I2C_SPEED;
    while (reg[i][0]) {
        r.iWriteData = reg[i]; // change data location and process second set of bytes
        error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
        if (error)
            break;
            //UEZFailureMsg("Accelerometer could not be configured!");
        i++;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Accelerometer_Freescale_MMA7455_I2C_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create an accelerometer driver for the Freescale MMA7455.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aI2CBusName -- Name of I2C bus device driver used by this
 *          accelerometer.
 *      TUInt8 aI2CAddr -- 7-bit address (lower 7 bits) of I2C address of
 *          accelerometer.
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError Accelerometer_Freescale_MMA7455_I2C_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr)
{
    T_uezDeviceWorkspace *p_accel;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the accelerator device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Accelerometer_Freescale_MMA7455_I2C_Interface,
            0, &p_accel);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
    return Accelerometer_Freescale_MMA7455_I2C_Configure(p_accel,
            (DEVICE_I2C_BUS **)p_i2c, aI2CAddr);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Accelerometer Accelerometer_Freescale_MMA7455_I2C_Interface = {
// Common device interface
        {
                "Accelerometer:Freescale:MMA7455:I2C",
                0x0100,
                Accelerometer_Freescale_MMA7455_I2C_InitializeWorkspace,
                sizeof(T_Accelerometer_Freescale_MMA7455_I2C_Workspace), },

        // Functions
        Accelerometer_Freescale_MMA7455_I2C_GetInfo,
        Accelerometer_Freescale_MMA7455_I2C_ReadXYZ,
        Accelerometer_Freescale_MMA7455_I2C_ReadXYZ_Float,
};

/*-------------------------------------------------------------------------*
 * End of File:  Freescale_MMA7455_driver.c
 *-------------------------------------------------------------------------*/
