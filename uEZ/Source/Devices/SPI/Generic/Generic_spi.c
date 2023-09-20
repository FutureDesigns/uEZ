/*-------------------------------------------------------------------------*
 * File:  lpc2478_spi.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Generic LPC2478 SPI Bus.
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
#include <Device/SPIBus.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include <HAL/SPI.h>
#include "Generic_SPI.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_SPI_BUS *iDevice;
    T_uezSemaphore iSem;
    HAL_SPI **iSPI;
    T_uezSemaphore iReady;
    TBool iBusy;
    T_spiCompleteCallback iCallback;
    void *iCallbackWorkspace;
    SPI_Request *iRequest;
} T_SPI_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a SPI Bus and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError SPI_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aW;

    // Create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    error = UEZSemaphoreCreateCounting(&p->iReady, 1, 0);
    if (error)
        return error;

    p->iBusy = EFalse;
    p->iCallback = 0;
    p->iCallbackWorkspace = 0;
    p->iRequest = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a SPI Bus to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError SPI_Generic_Configure(void *aW, const char *aSPIHALName)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aW;

    // Link to the SPI device or report an error when tried
    return HALInterfaceFind(aSPIHALName, (T_halWorkspace **)&p->iSPI);
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_TransferPolled
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a single SPI transfer using polling methods.  Will block until
 *      the SPI is completed.  Also protects multiple tasks from using
 *      the same resource.
 * Inputs:
 *      void *aW                    -- Workspace
 *      SPI_Request *aRequest       -- SPI request being used.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError SPI_Generic_TransferPolled(void *aWorkspace, SPI_Request *aRequest)
{
    T_uezError error;
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iSPI)->TransferPolled(p->iSPI, aRequest);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a piecewise transaction by starting a request, but doing
 *      no transfers.  The transaction MUST have a stop at some point.
 * Inputs:
 *      void *aW                    -- Workspace
 *      SPI_Request *aRequest       -- SPI request being used.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
void SPI_Generic_Start(void *aWorkspace, SPI_Request *aRequest)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;
    (*p->iSPI)->Start(p->iSPI, aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_TransferInOut
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a single segment of data out the SPI line and wait until done.
 * Inputs:
 *      void *aW                    -- Workspace
 *      SPI_Request *aRequest       -- SPI request being used.
 *      TUInt32 aSend               -- Data to send
 * Outputs:
 *      TUInt32                     -- Received data
 *---------------------------------------------------------------------------*/
TUInt32 SPI_Generic_TransferInOut(
    void *aWorkspace,
    SPI_Request *aRequest,
    TUInt32 aSend)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;

    return (*p->iSPI)->TransferInOut(p->iSPI, aRequest, aSend);
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_TransferInOutBytes
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a number of bytes out the SSP line and read back the
 *      response and wait until done.  Do not control the chip select.
 * Inputs:
 *      void *aW                    -- Workspace
 *      SSP_Request *aRequest       -- SSP request being used.
 * Outputs:
 *      T_uezError                  -- Error code (if any)
 *---------------------------------------------------------------------------*/
T_uezError SPI_Generic_TransferInOutBytes(
    void *aWorkspace,
    SPI_Request *aRequest)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;

    return (*p->iSPI)->TransferInOutBytes(p->iSPI, aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      Stop the SPI transaction and hold here.
 * Inputs:
 *      void *aW                    -- Workspace
 *      SPI_Request *aRequest       -- SPI request being used.
 *---------------------------------------------------------------------------*/
void SPI_Generic_Stop(void *aWorkspace, SPI_Request *aRequest)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;
    (*p->iSPI)->Stop(p->iSPI, aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a generic thread safe interface to the SPI HAL driver
 *      for handling SPI requests.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aSerialHALName -- Name of low level SPI HAL driver
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError SPI_Generic_Create(const char *aName, const char *aSPIHALName)
{
    T_uezDeviceWorkspace *p_ssp0;

    // Initialize the SPI bus 0 device
    // and link to the SPI0 HAL driver
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&SPIBus_Generic_Interface, 0, &p_ssp0);
    return SPI_Generic_Configure(p_ssp0, aSPIHALName);
}

static void ISPI_Generic_CompleteCallback(
    void *aWorkspace,
    SPI_Request *aRequest)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;

    // The transfer is complete, report we are done (still in the IRQ!)
    p->iBusy = EFalse;
    if (p->iCallback)
        p->iCallback(p->iCallbackWorkspace, p->iRequest);
}

T_uezError SPI_Generic_TransferNoBlock(
    void *aWorkspace,
    SPI_Request *aRequest,
    T_spiCompleteCallback aCallback,
    void *aCallbackWorkspace)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p->iBusy) {
        UEZSemaphoreRelease(p->iSem);
        return UEZ_ERROR_BUSY;
    }

    // Setup the transfer
    p->iBusy = ETrue;
    p->iCallback = aCallback;
    p->iCallbackWorkspace = aCallbackWorkspace;
    p->iRequest = aRequest;

    // Do the transfer
    (*p->iSPI)->TransferNoBlock(p->iSPI, aRequest,
        ISPI_Generic_CompleteCallback, aWorkspace);

    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

TBool SPI_Generic_IsBusy(void *aWorkspace)
{
    T_SPI_Generic_Workspace *p = (T_SPI_Generic_Workspace *)aWorkspace;
    return p->iBusy;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_SPI_BUS SPIBus_Generic_Interface = { {
// Common device interface
    "SPI:Generic",
    0x0100,
    SPI_Generic_InitializeWorkspace,
    sizeof(T_SPI_Generic_Workspace), },

// Functions
    SPI_Generic_TransferPolled,

    SPI_Generic_Start,
    SPI_Generic_TransferInOut,
    SPI_Generic_Stop,
    SPI_Generic_TransferInOutBytes,

    // uEZ v2.02
    SPI_Generic_TransferNoBlock,
    SPI_Generic_IsBusy };

/*-------------------------------------------------------------------------*
 * End of File:  lpc2478_spi.c
 *-------------------------------------------------------------------------*/
