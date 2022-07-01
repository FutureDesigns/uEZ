/*-------------------------------------------------------------------------*
 * File:  Generic_GPDMA.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Generic GPDMA Driver.  This generic
 *      general purpose DMA driver provides RTOS synchronization of the
 *      DMA between multiple threads and provides functions to start,
 *      stop, and block on the completion of a transfer.  It depends on
 *      a lower level HAL driver to do the CPU dependent work.
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
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include <HAL/GPDMA.h>
#include "Generic_GPDMA.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_GPDMA *iDevice;
    T_uezSemaphore iSem;
    HAL_GPDMA **iGPDMA;
    T_uezSemaphore iPending;
    T_gpdmaState iLastState;
    TBool iPrepared;
    TBool iRunning;
} T_GPDMA_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_Complete
 *---------------------------------------------------------------------------*
 * Description:
 *      
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_gpdmaState aState         -- State
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void IGPDMA_Generic_Complete(void *aWorkspace, T_gpdmaState aState)
{
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    // Currently, don't use state other than saving it
    p->iLastState = aState;

    // Release the waiting caller
    _isr_UEZSemaphoreRelease(p->iPending);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a GPDMA Bank and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aW;

    p->iLastState = GPDMA_UNKNOWN;
    p->iPrepared = EFalse;
    p->iRunning = EFalse;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    // Create a binary semaphore that is released when data comes in
    error = UEZSemaphoreCreateBinary(&p->iPending);
    UEZSemaphoreGrab(p->iPending, 0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_ProcessRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a single GPDMA transfer blocking until complete.
 * Inputs:
 *      void *aW                    -- Workspace
 *      GPDMA_RequestSingle *aRequest -- GPDMA request being used.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_ProcessRequest(
        void *aWorkspace,
        const T_gpdmaRequest *aRequest)
{
    T_uezError error;
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Put in the request and when the ISR calls the callback, the
    // semaphore will be released.
    error = (*p->iGPDMA)->PrepareRequest(p->iGPDMA, aRequest,
            IGPDMA_Generic_Complete, aWorkspace);
    (*p->iGPDMA)->Start(p->iGPDMA);

    // Wait until it is released (if not already)
    UEZSemaphoreGrab(p->iPending, UEZ_TIMEOUT_INFINITE);

    // DMA is complete, release the GPDMA for another to use
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link the GPDMA Device Driver to a GPDMA HAL driver.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const char *aGPDMAHALName     -- HAL Device driver name
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_Configure(void *aW, const char *aGPDMAHALName)
{
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aW;
    T_uezError error;

    // Access the GPDMABank device
    error = HALInterfaceFind(aGPDMAHALName, (T_halWorkspace **)&p->iGPDMA);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_PrepareRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a request, but don't start it.  Blocks until the GPDMA is free.
 *      NOTE: This grabs the GPDMA with a semaphore!  If error, then
 *        not grabbed.
 * Inputs:
 *      void *aW                    -- Workspace
 *      GPDMA_RequestSingle *aRequest -- GPDMA request being used.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_PrepareRequest(
        void *aWorkspace,
        const T_gpdmaRequest *aRequest)
{
    T_uezError error;
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Put in the request and when the ISR calls the callback, the
    // semaphore will be released.
    error = (*p->iGPDMA)->PrepareRequest(p->iGPDMA, aRequest,
            IGPDMA_Generic_Complete, aWorkspace);

    if (error == UEZ_ERROR_NONE) {
        p->iPrepared = ETrue;
    } else {
        // Error, abort here
        UEZSemaphoreRelease(p->iSem);
        p->iPrepared = EFalse;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_RunPrepared
 *---------------------------------------------------------------------------*
 * Description:
 *      Call this routine after using PrepareRequest to run the GPDMA request.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code.  Returns
 *                                    UEZ_ERROR_NOT_READY if not prepared.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_RunPrepared(void *aWorkspace)
{
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    if (!p->iPrepared)
        return UEZ_ERROR_NOT_READY;

    p->iRunning = ETrue;

    (*p->iGPDMA)->Start(p->iGPDMA);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_WaitTilDone
 *---------------------------------------------------------------------------*
 * Description:
 *      Waits the given time until the the GPDMA finishes or times out.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aTimeout            -- Amount of time to wait.
 * Outputs:
 *      T_uezError                   -- Error code.  Returns
 *                                    UEZ_ERROR_TIMEOUT if not completed in time.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_WaitTilDone(void *aWorkspace, TUInt32 aTimeout)
{
    T_uezError error;
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    if (!p->iPrepared)
        return UEZ_ERROR_NOT_READY;
    if (!p->iRunning)
        return UEZ_ERROR_NOT_ACTIVE;

    // Wait until it is released (if not already)
    error = UEZSemaphoreGrab(p->iPending, aTimeout);

    // If we got it, then we're done, release everything
    if (error == UEZ_ERROR_NONE) {
        // Mark this as no longer running and prepared
        p->iPrepared = EFalse;
        p->iRunning = EFalse;

        error = (*p->iGPDMA)->Stop(p->iGPDMA);

        // DMA is complete, release the GPDMA for another to use
        UEZSemaphoreRelease(p->iSem);
    }

    // Return UEZ_ERROR_TIMEOUT or UEZ_ERROR_NONE
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      Forces the GPDMA to stop
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code.  Returns
 *                                    UEZ_ERROR_TIMEOUT if not completed in time.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_Stop(void *aWorkspace)
{
    T_uezError error;
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    // Check to see if we were running
    if (!p->iPrepared)
        return UEZ_ERROR_NOT_READY;
    if (!p->iRunning)
        return UEZ_ERROR_NOT_ACTIVE;

    // Force the GPDMA to stop
    error = (*p->iGPDMA)->Stop(p->iGPDMA);

    // Mark this as no longer running and prepared
    p->iPrepared = EFalse;
    p->iRunning = EFalse;

    // DMA is complete, release the GPDMA for another to use
    UEZSemaphoreRelease(p->iSem);

    // Return UEZ_ERROR_TIMEOUT or UEZ_ERROR_NONE
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_BurstRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a Burst Request for the given peripheral
 * Inputs:
 *      const char *aWorkspace -- Workspace
 *      T_gpdmaPeripheral -- Peripheral to trigger for in request
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_BurstRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    return (*p->iGPDMA)->BurstRequest(p, aPeripheral);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_SingleRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a Single Request for the given peripheral
 * Inputs:
 *      const char *aWorkspace -- Workspace
 *      T_gpdmaPeripheral -- Peripheral to trigger for in request
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_SingleRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    return (*p->iGPDMA)->SingleRequest(p, aPeripheral);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_LastBurstRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a Last Burst Request for the given peripheral
 * Inputs:
 *      const char *aWorkspace -- Workspace
 *      T_gpdmaPeripheral -- Peripheral to trigger for in request
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_LastBurstRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    return (*p->iGPDMA)->LastBurstRequest(p, aPeripheral);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_LastSingleRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a Last Single Request for the given peripheral
 * Inputs:
 *      const char *aWorkspace -- Workspace
 *      T_gpdmaPeripheral -- Peripheral to trigger for in request
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_LastSingleRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    T_GPDMA_Generic_Workspace *p = (T_GPDMA_Generic_Workspace *)aWorkspace;

    return (*p->iGPDMA)->LastSingleRequest(p, aPeripheral);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPDMA_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a Generic General Purpose DMA (GPDMA) driver for doing data
 *      transfers.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aGPDMAHALName -- Name of low level GPDMA HAL driver
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError GPDMA_Generic_Create(const char *aName, const char *aGPDMAHALName)
{
    T_uezDeviceWorkspace *p_gpdma;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&GPDMA_Generic_Interface, 0, &p_gpdma);
    return GPDMA_Generic_Configure(p_gpdma, aGPDMAHALName);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_GPDMA GPDMA_Generic_Interface = {
// Common device interface
        {
                "GPDMA:Generic",
                0x0100,
                GPDMA_Generic_InitializeWorkspace,
                sizeof(T_GPDMA_Generic_Workspace), },

        // Functions
        GPDMA_Generic_ProcessRequest,
        GPDMA_Generic_PrepareRequest,
        GPDMA_Generic_RunPrepared,
        GPDMA_Generic_WaitTilDone,
        GPDMA_Generic_Stop, 
        GPDMA_Generic_BurstRequest,
        GPDMA_Generic_SingleRequest,
        GPDMA_Generic_LastBurstRequest,
        GPDMA_Generic_LastSingleRequest };

/*-------------------------------------------------------------------------*
 * End of File:  Generic_GPDMA.c
 *-------------------------------------------------------------------------*/
