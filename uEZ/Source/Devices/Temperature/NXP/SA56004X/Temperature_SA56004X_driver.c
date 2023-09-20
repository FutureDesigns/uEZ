/*-------------------------------------------------------------------------*
 * File:  Temperature_SA56004X_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a Temperature sensor 
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "Temperature_SA56004X.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define SA56004X_I2C_SPEED               400 // kHz

#define SA56004X_RTHB                   0x01    // Remote high byte
#define SA56004X_RTLB                   0x10    // Remove low byte
#define SA56004X_LTHB                   0x00    // Local high byte
#define SA56004X_LTLB                   0x22    // Local low byte

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Temperature *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    TUInt8 iI2CAddr;
    TBool iIsRemote;
} T_Temp_NXP_SA56004X_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_SA56004X_InitializeWorkspaceLocal
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temp_NXP_SA56004X_InitializeWorkspace(void *aW, TBool aIsRemote)
{
    T_uezError error;

    T_Temp_NXP_SA56004X_Workspace *p = (T_Temp_NXP_SA56004X_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iIsRemote = aIsRemote;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_SA56004X_InitializeWorkspaceLocal
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temp_NXP_SA56004X_InitializeWorkspaceLocal(void *aW)
{
    return Temp_NXP_SA56004X_InitializeWorkspace(aW, EFalse);
}

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_SA56004X_InitializeWorkspaceRemote
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temp_NXP_SA56004X_InitializeWorkspaceRemote(void *aW)
{
    return Temp_NXP_SA56004X_InitializeWorkspace(aW, ETrue);
}

/*---------------------------------------------------------------------------*
 * Routine:  ITemp_NXP_SA56004X_ReadReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup an I2C register read and process
 * Inputs:
 *      T_Temp_NXP_SA56004X_Workspace *p -- Workspace
 *      TUInt8 aReg                 -- Register to read
 *      TUInt8 *aValue              -- Place to store result
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError ITemp_NXP_SA56004X_ReadReg(
            T_Temp_NXP_SA56004X_Workspace *p, 
            TUInt8 aReg,
            TUInt8 *aValue)
{
    I2C_Request r;

    // Setup the I2C request
    r.iAddr = p->iI2CAddr;
    r.iSpeed = SA56004X_I2C_SPEED;
    r.iWriteData = &aReg;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = aValue;
    r.iReadLength = 1;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;  // wait until bus ready

    // Do th request
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_SA56004X_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current SA56004X temperature reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temp_NXP_SA56004X_Read(void *aWorkspace, TInt32 *aTemperature)
{
    T_Temp_NXP_SA56004X_Workspace *p = (T_Temp_NXP_SA56004X_Workspace *)aWorkspace;
    T_uezError error;
    TUInt8 lowByte;
    TUInt8 highByte;
    TInt32 v;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // This is a fake loop so we can use 'break' to exit early.
    while (1) {
        error = ITemp_NXP_SA56004X_ReadReg(
                    p, 
                    p->iIsRemote?SA56004X_RTHB:SA56004X_LTHB,
                    &highByte);
        if (error)
            break;

        error = ITemp_NXP_SA56004X_ReadReg(
                    p, 
                    p->iIsRemote?SA56004X_RTLB:SA56004X_LTLB,
                    &lowByte);
        if (error)
            break;

        // Calculate the new Temperature
        // Put in the highest bits (masking off unused bits)
        v = (TInt32)((((highByte<<8)|((lowByte & 0xE0)<<0)) & 0xFFE0) << 16);
        // Now shift down to sign extend
        v >>= 8;
        *aTemperature = (TInt32)v;

        // Don't really loop
        break;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Temp_NXP_SA56004X_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the SA56004X's I2C setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void Temp_NXP_SA56004X_Configure(
        void *aWorkspace, 
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr)
{
    T_Temp_NXP_SA56004X_Workspace *p = (T_Temp_NXP_SA56004X_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
    p->iI2CAddr = aI2CAddr;
}

void Temp_NXP_SA56004X_Local_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr)
{
    T_uezDeviceWorkspace *p_temp0;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the temperature 0 device
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&Temperature_NXP_SA56004X_Local_Interface, 0,
        &p_temp0);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
    Temp_NXP_SA56004X_Configure(p_temp0, (DEVICE_I2C_BUS **)p_i2c, aI2CAddr);
}

void Temp_NXP_SA56004X_Remote_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr)
{
    T_uezDeviceWorkspace *p_temp0;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the temperature 0 device
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&Temperature_NXP_SA56004X_Remote_Interface, 0,
        &p_temp0);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
    Temp_NXP_SA56004X_Configure(p_temp0, (DEVICE_I2C_BUS **)p_i2c, aI2CAddr);
}


/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Temperature Temperature_NXP_SA56004X_Local_Interface = {
	{
	    // Common device interface
	    "Temperature:NXP:SA56004X:Local",
	    0x0100,
	    Temp_NXP_SA56004X_InitializeWorkspaceLocal,
	    sizeof(T_Temp_NXP_SA56004X_Workspace),
	},
	
    // Functions
    Temp_NXP_SA56004X_Read,
};

const DEVICE_Temperature Temperature_NXP_SA56004X_Remote_Interface = {
	{
	    // Common device interface
	    "Temperature:NXP:SA56004X:Remote",
	    0x0100,
	    Temp_NXP_SA56004X_InitializeWorkspaceRemote,
	    sizeof(T_Temp_NXP_SA56004X_Workspace),
	},
	
    // Functions
    Temp_NXP_SA56004X_Read,
};

/*-------------------------------------------------------------------------*
 * End of File:  Temperature_SA56004X_driver.c
 *-------------------------------------------------------------------------*/
