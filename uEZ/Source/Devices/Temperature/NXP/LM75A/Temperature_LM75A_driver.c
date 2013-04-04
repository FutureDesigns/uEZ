/*-------------------------------------------------------------------------*
 * File:  Temperature_LM75A_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a Temperature sensor 
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

#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "Temperature_LM75A.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define LM75A_I2C_SPEED               400 // kHz
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Temperature *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    TUInt8 iI2CAddr;
} T_Temp_NXP_LM75A_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_LM75A_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temp_NXP_LM75A_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Temp_NXP_LM75A_Workspace *p = (T_Temp_NXP_LM75A_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_LM75A_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current LM75A temperature reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temp_NXP_LM75A_Read(void *aWorkspace, TInt32 *aTemperature)
{
    T_uezError error;
    T_Temp_NXP_LM75A_Workspace *p = (T_Temp_NXP_LM75A_Workspace *)aWorkspace;
    I2C_Request r;
    TUInt8 data[2];
    TUInt8 reg0[1] = { 0x00 };
    TInt32 v;

    r.iAddr = p->iI2CAddr;
    r.iSpeed = LM75A_I2C_SPEED;
    r.iWriteData = reg0;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = data;
    r.iReadLength = 2;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE; // wait until bus ready

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    if (!error) {
        // This temperature sensor is 11 bits up to about 125 C.
        // 3 bits are fraction, 7 bits are integer, and 1 sign bit

        // Put in the highest bits (masking off unused bits)
        v = (TInt32)((((data[0] << 8) | ((data[1]) << 0)) & 0xFFE0) << 16);

        // Now shift down to sign extend
        v >>= 8;

        *aTemperature = (TInt32)v;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_LM75A_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the LM75A's I2C setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void Temp_NXP_LM75A_Configure(
        void *aWorkspace,
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr)
{
    T_Temp_NXP_LM75A_Workspace *p = (T_Temp_NXP_LM75A_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
    p->iI2CAddr = aI2CAddr;
}

void Temp_NXP_LM75A_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr)
{
    T_uezDeviceWorkspace *p_temp0;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the temperature 0 device
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Temperature_NXP_LM75A_Interface, 0,
            &p_temp0);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
    Temp_NXP_LM75A_Configure(p_temp0, (DEVICE_I2C_BUS **)p_i2c, aI2CAddr);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Temperature Temperature_NXP_LM75A_Interface = { {
// Common device interface
        "Temperature:NXP:LM75A",
        0x0100,
        Temp_NXP_LM75A_InitializeWorkspace,
        sizeof(T_Temp_NXP_LM75A_Workspace), },

// Functions
        Temp_NXP_LM75A_Read, };

/*-------------------------------------------------------------------------*
 * End of File:  Temperature_LM75A_driver.c
 *-------------------------------------------------------------------------*/
