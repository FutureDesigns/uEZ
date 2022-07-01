/*-------------------------------------------------------------------------*
 * File:  uEZGenericBulkGeneric.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic stream interface to a GenericBulk device.
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
#include <uEZ.h>
#include <uEZRTOS.h>
#include <Device/Stream.h>
#include <Source/Library/USBDevice/Generic/Bulk/GenericBulk.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_STREAM *iDevice;
    T_uezSemaphore iSem;
    TBool iTxBusy;
    TUInt32 iNumOpen;
    T_uezSemaphore iSemEmpty;
    T_GenBulkCallbacks genericBulkCallbacks;
} T_GenericBulk_Stream_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a generic GenericBulk workspace.  Configure will setup the next
 *      step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_GenericBulk_Stream_Workspace *p = (T_GenericBulk_Stream_Workspace *)aW;
    p->iTxBusy = EFalse;
    p->iNumOpen = 0;
    UEZSemaphoreCreateBinary(&p->iSemEmpty);
    UEZSemaphoreGrab(p->iSemEmpty, 0);

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}
#if 0
/*---------------------------------------------------------------------------*
 * Routine:  IGenericBulkGenericCallbackReceivedByte
 *---------------------------------------------------------------------------*
 * Description:
 *      A single byte has been received -- queue it.
 * Inputs:
 *      void *aCallbackWorkspace    -- Pointer to T_GenericBulk_Stream_Workspace
 *      TUInt8 aByte                -- Byte received from GenericBulk driver
 *---------------------------------------------------------------------------*/
void IGenericBulkGenericCallbackReceivedByte(
        void *aCallbackWorkspace,
        TUInt8 aByte)
{
    T_GenericBulk_Stream_Workspace *p =
            (T_GenericBulk_Stream_Workspace *)aCallbackWorkspace;

    // A single byte is received
    _isr_UEZQueueSend(p->iQueueReceive, &aByte);
}

#endif

/*---------------------------------------------------------------------------*
 * Routine:  IGenericBulkGenericCallbackTransmitEmpty
 *---------------------------------------------------------------------------*
 * Description:
 *      The transmission buffer is empty (or needs more characters).  Pull
 *      a byte out of the waiting buffer and put into the GenericBulk hardware
 *      output by calling GenericBulkDevice->OutputByte.
 * Inputs:
 *      void *aCallbackWorkspace    -- Pointer to T_GenericBulk_Stream_Workspace
 *---------------------------------------------------------------------------*/
static void IOutputEmpty(void *aCallbackWorkspace)
{
    T_GenericBulk_Stream_Workspace *p =
            (T_GenericBulk_Stream_Workspace *)aCallbackWorkspace;

    // No more data to send, let the calling code know this
    UEZSemaphoreRelease(p->iSemEmpty);
    p->iTxBusy = EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the generic configuration with the given queue sizes and
 *      link to HAL GenericBulk device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      HAL_GenericBulk **aGenericBulk      -- GenericBulk Device to use
 *      TUInt32 aQueueSendSize    -- Size of the send queue
 *      TUInt32 aQueueReceiveSize   -- Size of the receive queue
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_Configure(T_uezDeviceWorkspace *aWorkspace)
{
    T_GenericBulk_Stream_Workspace *p =
            (T_GenericBulk_Stream_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    p->genericBulkCallbacks.iGenBulkEmptyOutput = IOutputEmpty;
    p->genericBulkCallbacks.iCallbackWorkspace = aWorkspace;
    GenericBulkInitialize(&p->genericBulkCallbacks);

    // Return last error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the GenericBulk port (activating it if first user).
 * Inputs:
 *      void *aWorkspace          -- This GenericBulk generic workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_Open(void *aWorkspace)
{
    T_GenericBulk_Stream_Workspace *p =
            (T_GenericBulk_Stream_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Note how many users are using the GenericBulk port
    p->iNumOpen++;

    // If the first user, activate the port
    //if (p->iNumOpen == 1)
    //    error = (*p->iGenericBulk)->Activate((T_halWorkspace *)p->iGenericBulk);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the GenericBulk port (deactivating it if last user to close).
 * Inputs:
 *      void *aWorkspace          -- This GenericBulk generic workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_Close(void *aWorkspace)
{
    // Decrement use count.  Are we done?
    T_GenericBulk_Stream_Workspace *p =
            (T_GenericBulk_Stream_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // If the last user, deactivate
    p->iNumOpen--;
    //if (p->iNumOpen == 0)
    //    error = (*p->iGenericBulk)->Deactivate((T_halWorkspace *)p->iGenericBulk);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read data from the GenericBulk port.  If an infinite amount of time is
 *      given, the GenericBulk port will wait until data arrives.  If a value
 *      of 0 is given for the timeout, then the GenericBulk data is read
 *      only if data is available.  The timeout is the timeout between
 *      successive characters, not the complete transfer.
 * Inputs:
 *      void *aWorkspace          -- This GenericBulk generic workspace
 *      TUInt8 *aData             -- Place to store data
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesRead    -- Number of bytes actually read.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_TIMEOUT is returned
 *                                   if timeout occurs trying to read
 *                                   the full amount and not enough data is
 *                                   provided.  UEZ_ERROR_NONE is reported if
 *                                   all data is returned.
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_Read(
        void *aWorkspace,
        TUInt8 *aData,
        TUInt32 aNumBytes,
        TUInt32 *aNumBytesRead,
        TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    //T_GenericBulk_Stream_Workspace *p =
    //        (T_GenericBulk_Stream_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;
    TInt32 c;
    PARAM_NOT_USED(aWorkspace);

    for (i = 0; i < aNumBytes; i++) {
        // Keep trying to receive bytes
        c = GenericBulkGet(aTimeout);
        if (c == -1) {
            error = UEZ_ERROR_TIMEOUT;
            break;
        }
        *(aData++) = (TUInt8)c;
    }

    // Report how many bytes we did get
    if (aNumBytesRead)
        *aNumBytesRead = i;

    // Report final error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the GenericBulk port or timeout trying.  If a timeout,
 *      the number of bytes written is reported.
 * Inputs:
 *      void *aWorkspace          -- This GenericBulk generic workspace
 *      TUInt8 *aData             -- Data to send
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesWritten -- Number of bytes actually written.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_TIMEOUT is returned
 *                                   if timeout occurs trying to write
 *                                   the full amount and not enough room was
 *                                   available.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_Write(
        void *aWorkspace,
        TUInt8 *aData,
        TUInt32 aNumBytes,
        TUInt32 *aNumBytesWritten,
        TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    T_GenericBulk_Stream_Workspace *p =
            (T_GenericBulk_Stream_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    for (i = 0; i < aNumBytes; i++) {
        // Send bytes one at a time
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

        p->iTxBusy = ETrue;

        // Semaphore is not going to be empty, be sure it is 'grabbed'
        UEZSemaphoreGrab(p->iSemEmpty, 0);

        // Send the bytes, one at a time
        if (!GenericBulkPut(*aData, aTimeout))
            error = UEZ_ERROR_TIMEOUT;

        UEZSemaphoreRelease(p->iSem);

        // Report any errors up to this point
        if (error)
            break;

        // Next byte
        aData++;
    }

    // Report how many bytes we did get
    if (aNumBytesWritten)
        *aNumBytesWritten = i;

    // Report final error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_Control
 *---------------------------------------------------------------------------*
 * Description:
 *      Command to configure the port or do handshaking via the stream
 *      interface.  For the uEZ Generic GenericBulk driver, only
 *      STREAM_CONTROL_SET_GenericBulk_SETTINGS and
 *      STREAM_CONTROL_GET_GenericBulk_SETTINGS are currently provided.
 * Inputs:
 *      void *aWorkspace          -- This GenericBulk generic workspace
 *      TUInt32 aControl          -- Command to execute
 *      void *aControlData        -- Data passed in with the command
 * Outputs:
 *      T_uezError                 -- Error code specific to command.
 *                                   If the command is unhandled, error code
 *                                   UEZ_ERROR_ILLEGAL_OPERATION is returned.
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_Control(
        void *aWorkspace,
        TUInt32 aControl,
        void *aControlData)
{
    //T_GenericBulk_Stream_Workspace *p = (T_GenericBulk_Stream_Workspace *)aWorkspace;

    // If we got here, the control was valid, but it could not be supported
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GenericBulk_Stream_Flush
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the GenericBulk port or timeout trying.  If a timeout,
 *      the number of bytes written is reported.
 * Inputs:
 *      void *aWorkspace          -- This GenericBulk generic workspace
 *      TUInt32 aTimeout          -- Timeout to wait for flush to finish
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_TIMEOUT is returned
 *                                   if timeout occurs trying to write
 *                                   the full amount and not enough room was
 *                                   available.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError GenericBulk_Stream_Flush(void *aWorkspace)
{
    T_GenericBulk_Stream_Workspace *p = (T_GenericBulk_Stream_Workspace *)aWorkspace;
#if 0
    // Decrement use count.  Are we done?
    T_uezError error = UEZ_ERROR_NONE;

    // Send bytes one at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Don't let interrupts occur while we do this one
    (*p->iGenericBulk)->Deactivate((T_halWorkspace *)p->iGenericBulk);

    // Are we currently busy sending data?
    if (p->iTxBusy) {
        (*p->iGenericBulk)->Activate((T_halWorkspace *)p->iGenericBulk);
        // Yes, busy, wait for it to flush
        error = UEZSemaphoreGrab(p->iSemEmpty, UEZ_TIMEOUT_INFINITE);
    } else {
        (*p->iGenericBulk)->Activate((T_halWorkspace *)p->iGenericBulk);
        // Transmit is not busy yet.  But we're about to be
    }

    UEZSemaphoreRelease(p->iSem);

    // Report final error
    return error;
#else
    // Are we currently busy sending data?
    if (p->iTxBusy) {
        // Yes, busy, wait for it to flush
        UEZSemaphoreGrab(p->iSemEmpty, 10);
    }

    return UEZ_ERROR_NONE;
#endif
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_STREAM GenericBulk_Stream_Interface = { {
// Generic bulk device stream interface
        "Stream:GenericBulk",
        0x0107,
        GenericBulk_Stream_InitializeWorkspace,
        sizeof(T_GenericBulk_Stream_Workspace), },

// Functions
        GenericBulk_Stream_Open,
        GenericBulk_Stream_Close,
        GenericBulk_Stream_Control,
        GenericBulk_Stream_Read,
        GenericBulk_Stream_Write,
        GenericBulk_Stream_Flush, };
/*-------------------------------------------------------------------------*
 * End of File:  GenericBulk.c
 *-------------------------------------------------------------------------*/
