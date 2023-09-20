/*-------------------------------------------------------------------------*
 * File:  Temperature_Analog_Devices_ADT7420.c
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
#include <Device/Temperature.h>
#include <Device/I2CBus.h>
#include "Temperature_ADT7420.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ADT7420_I2C_SPEED               400 // kHz

#define ADT7420_ADDR                0x90
#define ADT7420_DEVICE_ID           0xC8    // bits 7:3 of mfg identification number

#define ADT7420_TEMP_MSB_REG        0x00
#define ADT7420_TEMP_LSB_REG        0x01
#define ADT7420_STATUS_REG          0x02
#define ADT7420_CONFIG_REG          0x03
#define ADT7420_T_HIGH_MSB_REG      0x04
#define ADT7420_T_HIGH_LSB_REG      0x05
#define ADT7420_T_LOW_MSB_REG       0x06
#define ADT7420_T_LOW_LSB_REG       0x07
#define ADT7420_T_CRIT_MSB_REG      0x08
#define ADT7420_T_CRIT_LSB_REG      0x09
#define ADT7420_HIST_REG            0x0A
#define ADT7420_ID_REG              0x0B
#define ADT7420_RESET_REG           0x2F

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Temperature *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    TUInt8 iI2CAddr;
    TBool iConfigured;
} T_Temperature_AnalogDevices_ADT7420_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  Temperature_AnalogDevices_ADT7420_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temperature_AnalogDevices_ADT7420_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Temperature_AnalogDevices_ADT7420_Workspace *p =
        (T_Temperature_AnalogDevices_ADT7420_Workspace *)aW;

    p->iConfigured = EFalse;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Temperature_AnalogDevices_ADT7420_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current ADT7420 temperature reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Temperature_AnalogDevices_ADT7420_Read(
    void *aWorkspace,
    TInt32 *aTemperature)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Temperature_AnalogDevices_ADT7420_Workspace *p =
        (T_Temperature_AnalogDevices_ADT7420_Workspace *)aWorkspace;
    I2C_Request r;
    TUInt8 data[2];
    TUInt8 reg0[2] = { 0x00, 0x00 };
    TInt32 v;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (!p->iConfigured) {
        r.iAddr = p->iI2CAddr;
        r.iSpeed = ADT7420_I2C_SPEED;
        r.iWriteData = reg0;
        r.iWriteLength = 2;
        r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
        r.iReadData = 0;
        r.iReadLength = 0;
        r.iReadTimeout = 0;
        p->iConfigured = ETrue;
        reg0[0] = ADT7420_CONFIG_REG;
        reg0[1] = 0;
        error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    }
    if (error) {
        UEZSemaphoreRelease(p->iSem);
        return error;
    }

    r.iAddr = p->iI2CAddr;
    r.iSpeed = ADT7420_I2C_SPEED;
    r.iWriteData = reg0;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = data;
    r.iReadLength = 2;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE; // wait until bus ready
    reg0[0] = ADT7420_TEMP_MSB_REG;

    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    if (!error) {
        // This temperature sensor is 11 bits up to about 125 C.
        // 3 bits are fraction, 7 bits are integer, and 1 sign bit

        // Put in the highest bits (masking off unused bits)
        v = (TInt32)((((data[0] << 8) | ((data[1]) << 0)) & 0xFFF8) << 16);

        // Now shift down to sign extend and get final result in 16.15 format
        v >>= 7;

        *aTemperature = (TInt32)v;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Temperature_AnalogDevices_ADT7420_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the ADT7420's I2C setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void Temperature_AnalogDevices_ADT7420_Configure(
    void *aWorkspace,
    DEVICE_I2C_BUS **aI2C,
    TUInt8 aI2CAddr)
{
    T_Temperature_AnalogDevices_ADT7420_Workspace *p =
        (T_Temperature_AnalogDevices_ADT7420_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
    p->iI2CAddr = aI2CAddr;
}

void Temp_AnalogDevices_ADT7420_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr)
{
    T_uezDeviceWorkspace *p_temp0;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup the temperature 0 device
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&Temperature_AnalogDevices_ADT7420_Interface,
        0, &p_temp0);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);
    Temperature_AnalogDevices_ADT7420_Configure(p_temp0,
        (DEVICE_I2C_BUS **)p_i2c, aI2CAddr);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Temperature Temperature_AnalogDevices_ADT7420_Interface = { {
// Common device interface
    "Temperature:AD:ADT7420",
    0x0100,
    Temperature_AnalogDevices_ADT7420_InitializeWorkspace,
    sizeof(T_Temperature_AnalogDevices_ADT7420_Workspace), },

// Functions
    Temperature_AnalogDevices_ADT7420_Read, };

/*-------------------------------------------------------------------------*
 * End of File:  Temperature_Analog_Devices_ADT7420.c
 *-------------------------------------------------------------------------*/
