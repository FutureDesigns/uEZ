/*-------------------------------------------------------------------------*
 * File:  lpc2478_I2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Generic LPC2478 I2C Bus.
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
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include <HAL/I2CBus.h>
#include "Generic_I2C.h"
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_I2C_BUS *iDevice;
    T_uezSemaphore iSem;
    T_uezSemaphore iDone;
    HAL_I2CBus **iI2C;

    // Slave parameters:
    T_I2CSlaveSetup iSlaveSetup;
    TUInt32 iSlaveReceiveIndex;
    TUInt32 iSlaveSendIndex;
    TBool iIsSending;
    TBool iNeedSlaveEnable;
} T_I2C_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a I2C Bus called I2C0 and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError II2C_Generic_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;

    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aWorkspace;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;
    error = UEZSemaphoreCreateBinary(&p->iDone);
    if (error)
        return error;
    p->iNeedSlaveEnable = ETrue;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the I2C device by linking to a HAL driver by name.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 *      const char *aI2CName                -- I2C HAL name
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Generic_Configure(void *aWorkspace, const char *aI2CHALName)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aWorkspace;

    // Access the I2C device and link in or return an error
    return HALInterfaceFind(aI2CHALName, (T_halWorkspace **)&p->iI2C);
}

/*---------------------------------------------------------------------------*
 * Routine:  II2C_Generic_Complete
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback routine from the I2C HAL driver to report that a request
 *      is completed (reguardless of positive or negative result).
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      I2C_Request *aRequest        -- I2C request being completed.
 *---------------------------------------------------------------------------*/
static void II2C_Generic_Complete(
    void *aCallbackWorkspace,
    I2C_Request *iRequest)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aCallbackWorkspace;
    //    PARAM_NOT_USED(iRequest);

    // Let the main task resume
    _isr_UEZSemaphoreRelease(p->iDone);
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Generic_ProcessRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Do two I2C requests (a write followed by a read) or only one
 *      (write or read) per the request.  Block until the request is
 *      processed or times out.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      I2C_Request *aRequest        -- I2C request being used.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Generic_ProcessRequest(void *aWorkspace, I2C_Request *aRequest)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aWorkspace;

    // Allow only one request at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Always do the write first (if we have a write)
    if (aRequest->iWriteData) {
        // Grab the semaphore for doing the task (so it is zero count
        // and another grab blocks until a release occurs).
        UEZSemaphoreGrab(p->iDone, 0);

        // Do the write request
        (*p->iI2C)->RequestWrite(p->iI2C, aRequest, aWorkspace,
            II2C_Generic_Complete);

        // Wait until it is ready (or timeout) which might occur
        // before we get to this line.
        error = UEZSemaphoreGrab(p->iDone, aRequest->iWriteTimeout);

        if (error == UEZ_ERROR_NONE) {
            if (aRequest->iStatus != I2C_OK)
                error = UEZ_ERROR_NAK;
        }
        // At this point, either it occured, or we timed out.
        // In either case, go on.  We'll return the error to the
        // caller.
    }

    // Let's do the read now (if any) and if there is no error
    if ((aRequest->iReadData) && (error == UEZ_ERROR_NONE)) {
        // Grab the semaphore for doing the task (so it is zero count
        // and another grab blocks until a release occurs).
        UEZSemaphoreGrab(p->iDone, 0);

        // Do the write request
        (*p->iI2C)->RequestRead(p->iI2C, aRequest, aWorkspace,
            II2C_Generic_Complete);

        // Wait until it is ready (or timeout) which might occur
        // before we get to this line.
        error = UEZSemaphoreGrab(p->iDone, aRequest->iReadTimeout);

        // At this point, either it occured, or we timed out.
        // In either case, go on.  We'll return the error to the
        // caller.
        if (error == UEZ_ERROR_NONE) {
            if (aRequest->iStatus != I2C_OK)
                error = UEZ_ERROR_NAK;
        }
    }

    // Done with this request, allow the next one to occur.
    UEZSemaphoreRelease(p->iSem);

    return error;
}

TBool I2C_Generic_SlaveIsLastReceiveByte(void *aCallbackWorkspace)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aCallbackWorkspace;
    p->iIsSending = EFalse;
    if (p->iSlaveReceiveIndex >= p->iSlaveSetup.iMaxReceiveLength)
        return ETrue;
    return EFalse;
}

void I2C_Generic_SlaveReceiveByte(void *aCallbackWorkspace, TUInt8 aByte)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aCallbackWorkspace;

    // Only store if there is room
    if (p->iSlaveReceiveIndex < p->iSlaveSetup.iMaxReceiveLength)
        p->iSlaveSetup.iReceiveBuffer[p->iSlaveReceiveIndex++] = aByte;
}
TBool I2C_Generic_SlaveIsLastTransmitByte(void *aCallbackWorkspace)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aCallbackWorkspace;
    p->iIsSending = ETrue;
    if (p->iSlaveSendIndex >= p->iSlaveSetup.iMaxSendLength)
        return ETrue;
    return EFalse;
}

TUInt8 I2C_Generic_GetTransmitByte(void *aCallbackWorkspace)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aCallbackWorkspace;

    // Send data if not at the end, otherwise send 0's.
    if (p->iSlaveSendIndex < p->iSlaveSetup.iMaxSendLength)
        return p->iSlaveSetup.iSendBuffer[p->iSlaveSendIndex++];
    return 0x00;
}

void I2C_Generic_TransferComplete(void *aCallbackWorkspace)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aCallbackWorkspace;

    // We are done, let the next layer handle this
    _isr_UEZSemaphoreRelease(p->iDone);
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Generic_SlaveStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Setups
 * Inputs:
 *      void *p -- I2C Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Generic_SlaveStart(
    void *aWorkspace,
    const T_I2CSlaveSetup *aSetup)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aWorkspace;
    static const T_I2CSlaveCallbacks callbacks = {
        I2C_Generic_SlaveIsLastReceiveByte,
        I2C_Generic_SlaveReceiveByte,
        I2C_Generic_SlaveIsLastTransmitByte,
        I2C_Generic_GetTransmitByte,
        I2C_Generic_TransferComplete };
		
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iSlaveSetup = *aSetup;

    // Reset the pointers
    p->iSlaveSendIndex = p->iSlaveReceiveIndex = 0;

    (*p->iI2C)->ConfigureSlave(p->iI2C, p->iSlaveSetup.iAddressLower7Bits,
        aWorkspace, &callbacks);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Generic_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enables the I2C channel.
 * Inputs:
 *      void *p -- I2C Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Generic_Enable(void *aWorkspace)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    error = (*p->iI2C)->Enable(p->iI2C);
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Generic_Disable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables the I2C channel.
 * Inputs:
 *      void *p -- I2C Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Generic_Disable(void *aWorkspace)
{
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    error = (*p->iI2C)->Disable(p->iI2C);
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError I2C_Generic_SlaveWaitForEvent(
    void *aWorkspace,
    T_I2CSlaveEvent *aEvent,
    TUInt32 *aLength,
    TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_I2C_Generic_Workspace *p = (T_I2C_Generic_Workspace *)aWorkspace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p->iNeedSlaveEnable) {
        // Ensure the flag is waiting
        UEZSemaphoreGrab(p->iDone, 0);

        // Go ahead and enable the I2C Slave to allow it to receive a transaction
        error = (*p->iI2C)->Enable(p->iI2C);

        p->iNeedSlaveEnable = EFalse;
    }

    // Wait on the complete flag
    error = UEZSemaphoreGrab(p->iDone, aTimeout);
    if (error == UEZ_ERROR_NONE) {
        if (p->iIsSending) {
            *aEvent = I2C_SLAVE_EVENT_SENT;
            *aLength = p->iSlaveSendIndex;
        } else {
            *aEvent = I2C_SLAVE_EVENT_RECEIVED;
            *aLength = p->iSlaveReceiveIndex;
        }

        // Stop handling commands
        error = (*p->iI2C)->Disable(p->iI2C);
        p->iNeedSlaveEnable = ETrue;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a generic I2C driver for doing data
 *      transfers over USB.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aI2CHALName -- Name of low level I2C HAL driver
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
void I2C_Generic_Create(const char *aName, const char *aI2CHALName)
{
    T_uezDeviceWorkspace *p_dev;

    // Setup a generic I2C driver
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&I2CBus_Generic_Interface, 0, &p_dev);

    // Link the device driver to the HAL driver
    I2C_Generic_Configure(p_dev, aI2CHALName);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_I2C_BUS I2CBus_Generic_Interface = { {
// Common device interface
    "I2C:Generic",
    0x0100,
    II2C_Generic_InitializeWorkspace,
    sizeof(T_I2C_Generic_Workspace), },

// Functions
    I2C_Generic_ProcessRequest,

    // uEZ v1.10 Functions:
    I2C_Generic_SlaveStart,
    I2C_Generic_SlaveWaitForEvent,
    I2C_Generic_Enable,
    I2C_Generic_Disable };

/*-------------------------------------------------------------------------*
 * End of File:  lpc2478_I2C.c
 *-------------------------------------------------------------------------*/
