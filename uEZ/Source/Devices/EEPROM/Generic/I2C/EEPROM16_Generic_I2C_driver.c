/*-------------------------------------------------------------------------*
 * File:  EEPROM16_Generic_I2C_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the EEPROM for the I2C
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
#include "EEPROM16_Generic_I2C.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_EEPROM *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    TUInt8 iI2CAddr;
    TUInt8 iPage[EEPROM_MAX_PAGE_SIZE+4];
    T_EEPROMConfig iConfig;
} T_EEPROM16_Generic_I2C_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM16_Generic_I2C_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for I2C.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError EEPROM16_Generic_I2C_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_EEPROM16_Generic_I2C_Workspace *p = (T_EEPROM16_Generic_I2C_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    // Setup some standard settings in case they are not setup
    p->iConfig.iCommSpeed = 100;
    p->iConfig.iPageSize = 8;
    p->iConfig.iWriteTime = 25;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM16_Generic_I2C_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read bytes out of the EEPROM.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError EEPROM16_Generic_I2C_Read(
        void *aWorkspace,
        TUInt32 aAddress,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_EEPROM16_Generic_I2C_Workspace *p = (T_EEPROM16_Generic_I2C_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    I2C_Request r;
    TUInt32 size = aNumBytes;
    TUInt32 len;
    TUInt32 pageSize;
    TUInt8 *cmd = p->iPage;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    pageSize = p->iConfig.iPageSize;
    while (size) {
        // Determine number of bytes remaining in this page
        len = pageSize-((aAddress+pageSize)&(pageSize-1));
        // Clip to the remaining number of bytes
        if (len > size)
            len = size;

        // Create a command to write this data address
        cmd[0] = ((aAddress >> 8) & 0xFF);
        cmd[1] = (aAddress & 0xFF);

        // Update the part for the right blink reg
        r.iAddr = p->iI2CAddr|((aAddress>>16) & 0x3F);
        r.iSpeed = p->iConfig.iCommSpeed;

        // Send out the write to setup the address for
        // reading.
        r.iWriteData = cmd;
        r.iWriteLength = 2;
        r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

        // Read back the data into the buffer
        r.iReadData = aBuffer;
        r.iReadLength = len;
        r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

        // Do the command and stop on any errors
        error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
        if (error)
            break;

        aAddress += len;
        size -= len;
        aBuffer += len;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM16_Generic_I2C_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write bytes into the EEPROM
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to write
 *      TUInt8 *aBuffer             -- Pointer to buffer of data
 *      TUInt32 aNumBytes           -- Number of bytes to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError EEPROM16_Generic_I2C_Write(
        void *aWorkspace,
        TUInt32 aAddress,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_EEPROM16_Generic_I2C_Workspace *p = (T_EEPROM16_Generic_I2C_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    I2C_Request r;
    TUInt32 size = aNumBytes;
    TUInt32 len;
    TUInt32 pageSize;
    TUInt8 *cmd = p->iPage;
    TUInt32 i;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    pageSize = p->iConfig.iPageSize;
    while (size) {
        // Determine number of bytes remaining in this page
        len = pageSize-((aAddress+pageSize)&(pageSize-1));
        // Clip to the remaining number of bytes
        if (len > size)
            len = size;

        // Create a command to write this data address
        cmd[0] = ((aAddress>>8)&0xFF);
        cmd[1] = (aAddress&0xFF);
        for (i=2; i<=(1+len); i++)
            cmd[i] = *(aBuffer++);

        // Update the part for the right blink reg
        r.iAddr = p->iI2CAddr|((aAddress>>16) & 0x3F);
        r.iSpeed = p->iConfig.iCommSpeed;

        // Send out the write command
        r.iWriteData = cmd;
        r.iWriteLength = 2+len;
        r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

        // Read the nothing back
        r.iReadData = 0;
        r.iReadLength = 0;
        r.iReadTimeout = 0;

        // Do the command and stop on any errors
        error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
        if (error)
            break;

        // Wait the appropriate amount of time
        UEZTaskDelay(p->iConfig.iWriteTime);

        aAddress += len;
        size -= len;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM16_Generic_I2C_SetConfiguration
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the operating conditions of the I2C
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_EEPROMConfig *aConfig -- Operating parameters for this EEPROM
 *---------------------------------------------------------------------------*/
T_uezError EEPROM16_Generic_I2C_SetConfiguration(
        void *aWorkspace,
        const T_EEPROMConfig *aConfig)
{
    T_EEPROM16_Generic_I2C_Workspace *p = (T_EEPROM16_Generic_I2C_Workspace *)aWorkspace;
    if (aConfig->iPageSize > EEPROM_MAX_PAGE_SIZE)
        return UEZ_ERROR_INVALID_PARAMETER;
    p->iConfig = *aConfig;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM16_Generic_I2C_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the I2C's I2C setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void EEPROM16_Generic_I2C_Configure(
        void *aWorkspace,
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr)
{
    T_EEPROM16_Generic_I2C_Workspace *p = (T_EEPROM16_Generic_I2C_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
    p->iI2CAddr = aI2CAddr;
}

void EEPROM16_Generic_I2C_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr,
        const T_EEPROMConfig *aEEPROMConfig)
{
    T_uezDevice i2c;
    DEVICE_EEPROM **p_eeprom;
    T_uezDeviceWorkspace *p_i2c;

    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&EEPROM16_Generic_I2C_Interface,
            0,
            (T_uezDeviceWorkspace **)&p_eeprom);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, &p_i2c);
    EEPROM16_Generic_I2C_Configure(
            (void *)p_eeprom,
            (DEVICE_I2C_BUS **)p_i2c,
            aI2CAddr);
    (*p_eeprom)->SetConfiguration(p_eeprom, aEEPROMConfig);
}
/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_EEPROM EEPROM16_Generic_I2C_Interface = {
	{
		// Common device interface
	    "EEPROM:Generic:I2C",
	    0x0100,
	    EEPROM16_Generic_I2C_InitializeWorkspace,
	    sizeof(T_EEPROM16_Generic_I2C_Workspace),
	},
	
    // Functions
    EEPROM16_Generic_I2C_SetConfiguration,
    EEPROM16_Generic_I2C_Read,
    EEPROM16_Generic_I2C_Write,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  EEPROM16_Generic_I2C_driver.c
 *-------------------------------------------------------------------------*/
