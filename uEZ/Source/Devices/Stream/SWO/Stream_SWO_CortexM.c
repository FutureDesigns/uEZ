/*-------------------------------------------------------------------------*
 * File:  Stream_SWO_CortexM.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Stream interface for the Segger SWO Cortex M debugger terminal.
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
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include "Stream_SWO_CortexM.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_STREAM *iDevice;
} T_Stream_SWO_CortexM_Workspace;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
#define ITM_STIM_U32 (*(volatile uint32_t*)0xE0000000)    // Stimulus Port Register word acces
#define ITM_STIM_U8  (*(volatile         char*)0xE0000000)    // Stimulus Port Register byte acces
#define ITM_ENA      (*(volatile uint32_t*)0xE0000E00)    // Control Register
#define ITM_TCR      (*(volatile uint32_t*)0xE0000E80)    // Trace control register
#define DHCSR        (*(volatile uint32_t*)0xE000EDF0)    // Debug Halting Control Status Register
#define DEMCR        (*(volatile uint32_t*)0xE000EDFC)    // Debug Exception Monitor Control Register

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a generic serial workspace.  Configure will setup the next
 *      step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_InitializeWorkspace(void *aW)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the generic configuration with the given queue sizes and
 *      link to HAL Serial device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      HAL_Serial **aSerial      -- Serial Device to use
 *      TUInt32 aQueueSendSize    -- Size of the send queue
 *      TUInt32 aQueueReceiveSize   -- Size of the receive queue
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_Configure(T_uezDeviceWorkspace *aWorkspace)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the stream
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_Open(void *aWorkspace)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the device.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_Close(void *aWorkspace)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Reading characters from the debug terminal is not possible. This
 *      is an output only stream. Therefore, we simulate a timeout to make
 *      this stream behave like a normal stream device.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 *      TUInt8 *aData             -- Place to store data
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesRead    -- Number of bytes actually read.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_Read(
            void *aWorkspace, 
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesRead,
            TUInt32 aTimeout)
{
    *aNumBytesRead = 0;
    UEZTaskDelay(aTimeout);
    return UEZ_ERROR_TIMEOUT;
}


/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_WriteChar
 *---------------------------------------------------------------------------*
 * Description:
 *      Writes a single character to the SWO debug terminal.
 * Inputs:
 *      TInt8 c                   -- Character to write to the stream
 *      TUInt32 aTimeout          -- Timeout in milliseconds
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_WriteChar(TInt8 c, TUInt32 aTimeout) {
    TUInt32 tickStart;

    // Check if SWO is set up. If it is not, return to avoid that a program
    // hangs if no debugger is connected.

    // Check if DEBUGEN in DHCSR is set
    if ((DHCSR & 1)!= 1) {
        return UEZ_ERROR_NOT_ACTIVE;
    }

    // Check if TRACENA in DEMCR is set
    if ((DEMCR & (1 << 24)) == 0) {
        return UEZ_ERROR_NOT_ACTIVE;
    }

    // Check if ITM_TRC is enabled
    if ((ITM_TCR & (1 << 22)) == 1) {
        return UEZ_ERROR_NOT_ACTIVE;
    }

    // Check if stimulus port 0 is enabled
    if ((ITM_ENA & 1) == 0) {
        return UEZ_ERROR_NOT_ACTIVE;
    }

    tickStart = UEZTickCounterGet();
    // Wait until STIMx is ready to accept at least 1 word
    while ((ITM_STIM_U8 & 1) == 0) {
        if((PROCESSOR_OSCILLATOR_FREQUENCY / (UEZTickCounterGet()-tickStart)) > aTimeout) {
            return UEZ_ERROR_TIMEOUT;
        }
    }
    
    // send the character
    ITM_STIM_U8 = c;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the segger SWO debug terminal.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
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
T_uezError Stream_SWO_CortexM_Write(
            void *aWorkspace, 
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesWritten,
            TUInt32 aTimeout)
{
    T_uezError error;
    TUInt32 i;
    for(i=0; i<aNumBytes; i++) {
        // Print out character per character
        error = Stream_SWO_CortexM_WriteChar(*aData++, aTimeout);
        if(error != UEZ_ERROR_NONE)
            return error;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_Control
 *---------------------------------------------------------------------------*
 * Description:
 *      Not supported by this device
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt32 aControl          -- Command to execute
 *      void *aControlData        -- Data passed in with the command
 * Outputs:
 *      T_uezError                 -- Error code. UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_Control(
            void *aWorkspace, 
            TUInt32 aControl, 
            void *aControlData)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_Flush
 *---------------------------------------------------------------------------*
 * Description:
 *      Not supported by this device.
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt32 aTimeout          -- Timeout to wait for flush to finish
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_Flush(void *aWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_SWO_CortexM_FullDuplex_Stream_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a full duplex serial stream to a HAL serial device and create
 *      the read and write buffers.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aSerialHALName -- Name of low level Serial HAL driver
 *      TUInt32 aWriteBufferSize -- Size of the write buffer size in bytes
 *      TUInt32 aReadBufferSize -- Size of the read buffer size in bytes
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError Stream_SWO_CortexM_Create(const char *aName)
{
    T_uezDeviceWorkspace *p_stream;

    // Create serial stream device and link to HAL_Serial driver
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Stream_SWO_CortexM_Interface, 0,
            &p_stream);
    return Stream_SWO_CortexM_Configure(p_stream);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_STREAM Stream_SWO_CortexM_Interface = {
	{
	    // Generic device interface
	    "Serial:Generic",
	    0x0100,
	    Stream_SWO_CortexM_InitializeWorkspace,
	    sizeof(T_Stream_SWO_CortexM_Workspace),
	},
	
    // Functions
    Stream_SWO_CortexM_Open,
    Stream_SWO_CortexM_Close,
    Stream_SWO_CortexM_Control,
    Stream_SWO_CortexM_Read,
    Stream_SWO_CortexM_Write,
    Stream_SWO_CortexM_Flush,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  uEZSerialGeneric.c
 *-------------------------------------------------------------------------*/
