/*-------------------------------------------------------------------------*
 * File:  lpc1788_USBDevice.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the NXP LPC17xx_40xx USBDevice Bus.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include <HAL/USBDeviceController.h>
#include <Types/USBDevice.h>
#include "LPC17xx_40xx_USBDevice.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_USB_DEVICE *iDevice;
    T_uezSemaphore iSem;
    T_uezSemaphore iReady;
    T_uezSemaphore iProcess;
    HAL_USBDeviceController **iUSBDeviceController;
    T_USBDevice iUSBDevice;
    TUInt8 iStandardBuffer[USB_ENDPOINT0_PACKET_SIZE];
    T_USBSetupPacket iSetupPacket;
    TUInt8 *iPacketData;
    TUInt16 iPacketLength;
    TUInt16 iPacketRemaining;
    TBool iNeedSetAddress;
    TUInt8 iUSBAddress;
    TUInt8 iSmallPacket[8];
} T_USBDevice_LPC17xx_40xx_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool NXP_LPC17xx_40xx_USBDevice_StandardDeviceRequest(
                void *aWorkspace,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData);
static void INXP_LPC17xx_40xx_StandardControlTransfer(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);
static void INXP_LPC17xx_40xx_USBDevice_EndpointReady(void *aCallbackWorkspace);

#define dprintf(...)

/*---------------------------------------------------------------------------*
 * Routine:  USBDevice_NXP_InitializeWorkspace_USBDevice
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a USBDevice Bus called USBDevice0 and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError USBDevice_NXP_InitializeWorkspace_USBDevice(void *aWorkspace)
{
    T_uezError error;

    T_USBDevice_LPC17xx_40xx_Workspace *p = (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;

    p->iUSBDeviceController = 0; // not setup yet

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;
    error = UEZSemaphoreCreateBinary(&p->iProcess);
    if (error)
        return error;
    error = UEZSemaphoreCreateBinary(&p->iReady);
    if (error)
        return error;

    memset(&p->iUSBDevice, 0, sizeof(p->iUSBDevice));
    p->iUSBDevice.iRequestTypeHandlers[USB_REQUEST_TYPE_STANDARD].iBuffer =
            p->iStandardBuffer;
    p->iUSBDevice.iRequestTypeHandlers[USB_REQUEST_TYPE_STANDARD].iHandler =
            NXP_LPC17xx_40xx_USBDevice_StandardDeviceRequest;
    p->iUSBDevice.iRequestTypeHandlers[USB_REQUEST_TYPE_STANDARD].iWorkspace =
            aWorkspace;
    p->iUSBDevice.iStatusEndpointReadyFunc = INXP_LPC17xx_40xx_USBDevice_EndpointReady;
    p->iUSBDevice.iStatusChangeFunc = 0; // undefined

    // These are ALWAYS set to the standard control transfer at first
    p->iUSBDevice.iEndpointFuncs[0].iIn = INXP_LPC17xx_40xx_StandardControlTransfer;
    p->iUSBDevice.iEndpointFuncs[0].iOut = INXP_LPC17xx_40xx_StandardControlTransfer;
    p->iUSBDevice.iCallbackWorkspace = p;

    // No address and no need to set it
    p->iNeedSetAddress = EFalse;
    p->iUSBAddress = 0;

    return error;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  IUSBCreateDataInPacket
 *-------------------------------------------------------------------------*
 * Description:
 *      Create a DATAIN packet on the USB bus by parsing the data
 *      into 64 byte blocks.
 * Inputs:
 *      T_USBDevice_LPC17xx_40xx_Workspace *p -- Workspace
 *-------------------------------------------------------------------------*/
static void IUSBCreateDataInPacket(T_USBDevice_LPC17xx_40xx_Workspace *p)
{
    TUInt16 numBytesToSend;

    // Decide how many bytes to send
    numBytesToSend = p->iPacketRemaining;
    if (numBytesToSend > 64)
        numBytesToSend = 64;

    // Send data
    ((*p->iUSBDeviceController)->Write)(
            p->iUSBDeviceController,
            ENDPOINT_IN(0),
            p->iPacketData,
            numBytesToSend);

    // Update pointer and remaining size
    p->iPacketRemaining -= numBytesToSend;
    p->iPacketData += numBytesToSend;
}


/*-------------------------------------------------------------------------*
 * Internal Function:  ICallSetupHandler
 *-------------------------------------------------------------------------*
 * Description:
 *      Intermediate function that looks up the type of the setup packet
 *      and the corresponding pointer to function to call.  The called
 *      function then handles the packet and returns ETrue.  Otherwise,
 *      it returns EFalse.
 * Inputs:
 *      T_USBDevice_LPC17xx_40xx_Workspace *p -- Workspace
 *      T_USBSetupPacket *aSetup  -- Pointer to setup packet to handle
 *      TUInt16 *aLength          -- Length of data attached to packet.
 *      TUInt8 **aData            -- Pointer to data attached to packet.
 *-------------------------------------------------------------------------*/
static TBool ICallSetupHandler(
                T_USBDevice_LPC17xx_40xx_Workspace *p,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData)
{
    T_USBDeviceRequestTypeHandlerStruct *p_handler;

dprintf("[H%d]", aSetup->iRequestType.iRecipientType);
    // Use the appropriate handler for the type of setup packet.
    // If it doesn't exist, return with an error.
    if (aSetup->iRequestType.iRecipientType <= USB_REQUEST_TYPE_RESERVED)  {
        p_handler = &p->iUSBDevice.iRequestTypeHandlers[
                        aSetup->iRequestType.iRecipientType];
        if (p_handler->iHandler) {
            return (p_handler->iHandler)(
                        p_handler->iWorkspace,
                        aSetup,
                        aLength,
                        aData);
        } else {
dprintf("[NoHand1]");
        }
    } else {
dprintf("[NoHand2]");
        // Unknown type we could not handle
        return EFalse;
    }
    return EFalse;
}

/*-------------------------------------------------------------------------*
 * Function:  INXP_LPC17xx_40xx_StandardControlTransfer
 *-------------------------------------------------------------------------*
 * Description:
 *      USB Endpoint 0 "Control" has a packet request.  See if it is
 *      a standard control or pass it on to the next level.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 *      TUInt8 aEndpoint          -- Endpoint 0 (in or out)
 *      T_USBEndpointStatus aStatus -- Status of the endpoint request
 *-------------------------------------------------------------------------*/
static void INXP_LPC17xx_40xx_StandardControlTransfer(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    int numRead;
    TUInt8 setupType;
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;

dprintf("{SC %d}", aEndpoint);
    // Is this device receiving a request from the host to send it data?
    if (aEndpoint == ENDPOINT_OUT(0)) {
        // Is the packet a setup packet?
        if (aStatus & USB_ENDPOINT_SELECT_SETUP_BIT) {
            // Reset the setup packet and put into memory
            ((*p->iUSBDeviceController)->Read)(
                p->iUSBDeviceController,
                0,
                (TUInt8 *)&p->iSetupPacket,
                sizeof(T_USBSetupPacket));
dprintf("[Setup $%02X $%02X %d %d %d]", p->iSetupPacket.iRequestType, p->iSetupPacket.iRequest, p->iSetupPacket.iValue, p->iSetupPacket.iIndex, p->iSetupPacket.iLength);

            // defaults for data pointer and residue
            setupType = p->iSetupPacket.iRequestType.iRecipientType;
            p->iPacketData = p->iUSBDevice.iRequestTypeHandlers[setupType].iBuffer;
            p->iPacketRemaining = p->iSetupPacket.iLength;
            p->iPacketLength = p->iSetupPacket.iLength;
dprintf("[Len %d]", p->iPacketLength);

            // Is this a zero length packet or a request from the host
            // to get something?
//            if ((p->iSetupPacket.iLength == 0) ||
//                    (p->iSetupPacket.iRequestType.iDirection ==
//                        USB_DIRECTION_TO_HOST)) {
//dprintf("[Zero]");
                if (p->iSetupPacket.iRequestType.iDirection !=
                        USB_DIRECTION_TO_HOST) {
                    ((*p->iUSBDeviceController)->Write)(
                            p->iUSBDeviceController,
                            ENDPOINT_IN(0),
                            0,
                            0);

                }

                // Call the appropriate function
                if (!ICallSetupHandler(
                        p,
                        &p->iSetupPacket,
                        &p->iPacketLength,
                        &p->iPacketData)) {
                    // For some reason, the packet could not be
                    // handled.  Stall the control pipe.
                    ((*p->iUSBDeviceController)->SetStallState)(
                            p->iUSBDeviceController,
                            aStatus,
                            ETrue);
                    return;
                } else {
                    // The remaining bytes are equal to the current length,
                    // trimmed to the amount requested by the host.
                    p->iPacketRemaining = p->iPacketLength;
                    if (p->iPacketRemaining > p->iSetupPacket.iLength)
                        p->iPacketRemaining = p->iSetupPacket.iLength;

                    // Send the start of the response
                    IUSBCreateDataInPacket(p);
                }
//            } else {
//dprintf("[NZ]");
//            }
        } else {
dprintf("[Data %d]", p->iPacketRemaining);
            // Is not a setup packet.  We are transferring data.
            // Are we expecting any more data?
            if (p->iPacketRemaining > 0) {
dprintf("[Remains]");
                // Read in the data and store
                numRead = ((*p->iUSBDeviceController)->Read)(
                                p->iUSBDeviceController,
                                0x00,
                                p->iPacketData,
                                p->iPacketRemaining);

                // Did we have a problem reading the data?
                if (numRead < 0)  {
                    // Just stall the data and stop here
                    ((*p->iUSBDeviceController)->SetStallState)(
                            p->iUSBDeviceController,
                            aStatus,
                            ETrue);
                    return;
                }

                // Next data location and subtract amount read
                p->iPacketData += numRead;
                p->iPacketRemaining -= numRead;

                // Any more data left to read?
                if (p->iPacketRemaining == 0) {
dprintf("[All]");
                    // All data has been read.
                    setupType = p->iSetupPacket.iRequestType.iRecipientType;
                    p->iPacketData = p->iUSBDevice.iRequestTypeHandlers[setupType].iBuffer;
                    // Now that all the data has been received from the
                    // the bus and hardware, process the data packet.
                    if (!ICallSetupHandler(
                                p,
                                &p->iSetupPacket,
                                &p->iPacketLength,
                                &p->iPacketData)) {
                        // For some reason, the packet could not be
                        // handled.  Stall the control pipe.
                        ((*p->iUSBDeviceController)->SetStallState)(
                            p->iUSBDeviceController,
                            aStatus,
                            ETrue);
                        return;
                    }
                    p->iPacketRemaining = p->iPacketLength;

                    // Tell host we got the data
                    IUSBCreateDataInPacket(p);
                }
            } else {
dprintf("[Clear]");
                // Do a read to clear the remaining data if we are
                // not expecting more data.
                ((*p->iUSBDeviceController)->Read)(
                    p->iUSBDeviceController,
                    0,
                    0,
                    0);
            }
        }
    } else if (aEndpoint == ENDPOINT_IN(0)) {
dprintf("[In]");
        // IN transfer
        // send more data if available (possibly a 0-length packet)
        if (p->iSetupPacket.iRequestType.iDirection == USB_DIRECTION_TO_HOST) {
dprintf("[2Host]");
            IUSBCreateDataInPacket(p);
        } else {
dprintf("[2Dev]");
            if (p->iNeedSetAddress) {
dprintf("[NeedAddr]");
                p->iNeedSetAddress = EFalse;
                (*p->iUSBDeviceController)->SetAddress(
                        p->iUSBDeviceController,
                        p->iUSBAddress);
            }
        }
    } else {
//        ASSERT(EFalse);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  INXP_LPC17xx_40xx_USBDevice_EndpointReady
 *---------------------------------------------------------------------------*
 * Description:
 *      Endpoint is ready.  Release the semaphore holding back requests
 *      to handle endpoint data.  ProcessEndpoints should be called soon.
 *      NOTE: This routine is a callback called during an interrupt.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void INXP_LPC17xx_40xx_USBDevice_EndpointReady(void *aCallbackWorkspace)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aCallbackWorkspace;

    // Let the main monitor task resume
    _isr_UEZSemaphoreRelease(p->iReady);
}

/*---------------------------------------------------------------------------*
 * Routine:  NXP_LPC17xx_40xx_USBDevice_ProcessEndpoints
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for USBDevice endpoints to become available and process
 *      if ready.
 * Inputs:
 *      void *aWorkspace                  -- Workspace
 *      TUInt32 aTimeout                  -- Timeout to wait for endpoint
 *                                          processing ready.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError NXP_LPC17xx_40xx_USBDevice_ProcessEndpoints(
        void *aWorkspace,
        TUInt32 aTimeout)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_USBDevice_LPC17xx_40xx_Workspace *p = (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;

    // Allow only one endpoint processing at a time
    error = UEZSemaphoreGrab(p->iProcess, aTimeout);
    if (error != UEZ_ERROR_NONE)
        return error;

    // Wait until it is ready (or timeout) which might occur
    // before we get to this line.
    error = UEZSemaphoreGrab(p->iReady, aTimeout);
    if (error == UEZ_ERROR_NONE) {
        // Got the semaphore, which means 1 or more endpoints are
        // ready to process.  Process all the endpoints.
        (*p->iUSBDeviceController)->ProcessEndpoints(p->iUSBDeviceController);

    }

    // At this point, either it occured, or we timed out.
    // In either case, go on.  We'll return the error to the
    // caller.

    // Done with this request, allow the next one to occur.
    UEZSemaphoreRelease(p->iProcess);

    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_Connect
 *-------------------------------------------------------------------------*
 * Description:
 *      Connect to the USB bus.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_Connect(void *aWorkspace)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->Connect(p->iUSBDeviceController);
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_Disconnect
 *-------------------------------------------------------------------------*
 * Description:
 *      Disconnect from the USB bus.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_Disconnect(void *aWorkspace)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->Disconnect(p->iUSBDeviceController);
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_SetAddress
 *-------------------------------------------------------------------------*
 * Description:
 *      Sets the USB address for this device.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aAddress             -- 7 bit address of this device.
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_SetAddress(void *aWorkspace, TUInt8 aAddress)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
//    (*p->iUSBDeviceController)->SetAddress(
//            p->iUSBDeviceController,
//            aAddress);
    p->iNeedSetAddress = ETrue;
    p->iUSBAddress = aAddress;
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_EndpointConfigure
 *-------------------------------------------------------------------------*
 * Description:
 *      Realize and enable an endpoint.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aEndpoint            -- Endpoint identifier
 *      TUInt16 aMaxPacketSize      -- Maximum size of packets
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_EndpointConfigure(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TUInt16 aMaxPacketSize)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->EndpointConfigure(
            p->iUSBDeviceController,
            aEndpoint,
            aMaxPacketSize);
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_Read
 *-------------------------------------------------------------------------*
 * Description:
 *      Read data from an endpoint
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aEndpoint            -- Endpoint with data
 *      TUInt8 *aData               -- Pointer to place to put data.
 *      TUInt16 iMaxLen             -- Maximum number of bytes for data.
 * Outputs:
 *      TInt16                      -- Number of bytes received, or -1
 *-------------------------------------------------------------------------*/
static TInt16 NXP_LPC17xx_40xx_USBDevice_Read(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TUInt8 *aData,
            TUInt16 iMaxLen)
{
    TInt16 numBytes;

    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    numBytes = (*p->iUSBDeviceController)->Read(
            p->iUSBDeviceController,
            aEndpoint,
            aData,
            iMaxLen);
    UEZSemaphoreRelease(p->iSem);

    return numBytes;
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_Write
 *-------------------------------------------------------------------------*
 * Description:
 *      Write data to an endpoint
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aEndpoint            -- Endpoint to receive data
 *      TUInt8 *aData               -- Pointer to data to send.
 *      TUInt16 aLength             -- Number of bytes to send
 * Outputs:
 *      TUInt16                     -- aLength
 *-------------------------------------------------------------------------*/
static TUInt16 NXP_LPC17xx_40xx_USBDevice_Write(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TUInt8 *aData,
            TUInt16 iMaxLen)
{
    TUInt16 length;

    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    length = (*p->iUSBDeviceController)->Write(
            p->iUSBDeviceController,
            aEndpoint,
            aData,
            iMaxLen);
    UEZSemaphoreRelease(p->iSem);

    return length;
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_SetStallState
 *-------------------------------------------------------------------------*
 * Description:
 *      Set the stall state of an endpoint
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aEndpoint            -- Endpoint to set to stalled or not
 *                                     stalled.
 *      TBool                       -- ETrue for stall, else EFalse for
 *                                     not stalled.
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_SetStallState(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TBool aStallState)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->SetStallState(
            p->iUSBDeviceController,
            aEndpoint,
            aStallState);
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_IsStalled
 *-------------------------------------------------------------------------*
 * Description:
 *      Determine if an endpoint is stalled.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aEndpoint            -- Endpoint to check if stalled
 * Outputs:
 *      TBool                       -- ETrue if stalled, else EFalse
 *-------------------------------------------------------------------------*/
static TBool NXP_LPC17xx_40xx_USBDevice_IsStalled(void *aWorkspace, TUInt8 aEndpoint)
{
    TBool result;

    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    result = (*p->iUSBDeviceController)->IsStalled(
            p->iUSBDeviceController,
            aEndpoint);
    UEZSemaphoreRelease(p->iSem);

    return result;
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_EndpointInterruptsEnable
 *-------------------------------------------------------------------------*
 * Description:
 *      Allow an endpoint to generate interrupts.
 * Inputs:
 *      T_LPC17xx_40xx_USBDev_Workspace *p -- workspace
 *      TUInt8 aEndpoint            -- Endpoint to trigger interrupts
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_EndpointInterruptsEnable(
                void *aWorkspace,
                TUInt8 aEndpoint)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->EndpointInterruptsEnable(
            p->iUSBDeviceController,
            aEndpoint);
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_EndpointInterruptsDisable
 *-------------------------------------------------------------------------*
 * Description:
 *      Disallow an endpoint to generate interrupts.
 * Inputs:
 *      T_LPC17xx_40xx_USBDev_Workspace *p -- workspace
 *      TUInt8 aEndpoint            -- Endpoint to trigger interrupts
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_EndpointInterruptsDisable(
                void *aWorkspace,
                TUInt8 aEndpoint)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->EndpointInterruptsDisable(
            p->iUSBDeviceController,
            aEndpoint);
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_ForceAddressAndConfiguration
 *-------------------------------------------------------------------------*
 * Description:
 *      Force the USB to go to a given address and configuration.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aAddress           -- Address of connection
 *      TUInt8 aConfigNum         -- Number of configuration to use
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_ForceAddressAndConfiguration(
            void *aWorkspace,
            TUInt8 aAddress,
            TUInt8 aConfigNum)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->ForceAddressAndConfigurationNum(
            p->iUSBDeviceController,
            aAddress,
            aConfigNum);
    UEZSemaphoreRelease(p->iSem);
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_Initialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the hardware and all the endpoints.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_Initialize(void *aWorkspace)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->Initialize(p->iUSBDeviceController);
    UEZSemaphoreRelease(p->iSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  NXP_LPC17xx_40xx_USBDevice_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the USBDev screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      T_USBDevSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError NXP_LPC17xx_40xx_USBDevice_Configure(
        void *aWorkspace,
        const TUInt8 *aDescriptorTable)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    T_USBDeviceControllerSettings settings;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Setup the descriptor table and usbdevice pointers
    settings.iUSBDescriptorTable = aDescriptorTable;
    settings.iUSBDevice = &p->iUSBDevice;

    (*p->iUSBDeviceController)->Configure(
            p->iUSBDeviceController,
            &settings);

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  NXP_LPC17xx_40xx_USBDevice_RegisterEndpointCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Register a endpoint callback routine that is called when data
 *      is ready/requested on the given endpoint and ProcessEndpoints is
 *      called.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aEndpoint          -- Endpoint to set callback.  Use
 *                                  macros ENDPOINT_IN or ENDPOINT_OUT
 *                                  to set if the endpoint is in or out.
 *      T_USBEndpointHandlerFunc aEndpointFunc -- Pointer to function to
 *                                  call when data is ready and
 *                                  ProcessEndpoints is called.
 * Outputs:
 *      T_uezError                 -- Error code UEZ_ERROR_NOT_AVAILABLE
 *                                      is returned if endpoint is already
 *                                      registered, or UEZ_ERROR_NONE if
 *                                      successful.
 *---------------------------------------------------------------------------*/
static T_uezError NXP_LPC17xx_40xx_USBDevice_RegisterEndpointCallback(
            void *aWorkspace,
            TUInt8 aEndpoint,
            T_USBEndpointHandlerFunc aEndpointFunc)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    T_USBEndpointHandler *p_handler =
        p->iUSBDevice.iEndpointFuncs + ENDPOINT_INDEX(aEndpoint);

    // Modify the callback table while no callbacks can occur
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Are we registering an in or out endpoint?
    if (ENDPOINT_IS_IN(aEndpoint))  {
        // Is in endpoint already in use?  If not, use it.
        if (p_handler->iIn)
            error = UEZ_ERROR_NOT_AVAILABLE;
        else
            p_handler->iIn = aEndpointFunc;
    } else {
        // Is in endpoint already in use?  If not, use it.
        if (p_handler->iOut)
            error = UEZ_ERROR_NOT_AVAILABLE;
        else
            p_handler->iOut = aEndpointFunc;
    }

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  NXP_LPC17xx_40xx_USBDevice_UnregisterEndpointCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Unregister an endpoint so the previously associated callback routine
 *      is no longer called when endpoint data was ready.
 * Inputs:
 *      void *aWorkspace          -- USB Device's workspace
 *      TUInt8 aEndpoint          -- Endpoint with callback.  Used macros
 *                                  ENDPOINT_IN or ENDPOINT_OUT for
 *                                  correct direction.
 * Outputs:
 *      T_uezError                 -- Error code UEZ_ERROR_NOT_FOUND
 *                                      is returned if endpoint is already
 *                                      unregistered, or UEZ_ERROR_NONE if
 *                                      successful.
 *---------------------------------------------------------------------------*/
static T_uezError NXP_LPC17xx_40xx_USBDevice_UnregisterEndpointCallback(
            void *aWorkspace,
            TUInt8 aEndpoint)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    T_USBEndpointHandler *p_handler =
        p->iUSBDevice.iEndpointFuncs + ENDPOINT_INDEX(aEndpoint);

    // Modify the callback table while no callbacks can occur
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Are we unregistering an in or out endpoint?
    if (ENDPOINT_IS_IN(aEndpoint))  {
        // Is in endpoint already freed?  If not, free it.
        if (!p_handler->iIn)
            error = UEZ_ERROR_NOT_FOUND;
        else
            p_handler->iIn = 0;
    } else {
        // Is in endpoint already freed?  If not, free it.
        if (!p_handler->iOut)
            error = UEZ_ERROR_NOT_FOUND;
        else
            p_handler->iOut = 0;
    }

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_StandardDeviceRequest
 *-------------------------------------------------------------------------*
 * Description:
 *      Handle a standard device request (as all USB devices should).
 * Inputs:
 *      void *aWorkspace          -- USB Device workspace
 *      T_USBSetupPacket *aSetup  -- Received setup packet
 *      TUInt16 *aLength          -- Place to store length of response data.
 *      TUInt8 **aData            -- Pointer to pointer to data.
 *-------------------------------------------------------------------------*/
static TBool NXP_LPC17xx_40xx_USBDevice_StandardDeviceRequest(
                void *aWorkspace,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    TUInt8 *p_data = *aData;
    const TUInt8 *p_desc;

dprintf("[SD %d]", aSetup->iRequest);
    switch (aSetup->iRequest)  {
        case USB_DEVICE_REQUEST_GET_STATUS:
            p_data[0] = 0;
            p_data[1] = 0;
            *aLength = 2;
            return ETrue;
        case USB_DEVICE_REQUEST_CLEAR_FEATURE:
            // TBD: Not used, really needs a callback function
            *aLength = 0;
            return ETrue;
        case USB_DEVICE_REQUEST_SET_FEATURE:
            // TBD: Not used, really needs a callback function
            *aLength = 0;
            return ETrue;
        case USB_DEVICE_REQUEST_SET_ADDRESS:
    p->iNeedSetAddress = ETrue;
    p->iUSBAddress = aSetup->iValue&0x7F;

//            ((*p->iUSBDeviceController)->SetAddress)(
//                        p->iUSBDeviceController,
//                        aSetup->iValue);
#if 0
            p->iPacketLength = 0;
            p->iPacketRemaining = 0;
            p->iPacketData = p->iSmallPacket;
            IUSBCreateDataInPacket(p);
#endif
            return ETrue;
        case USB_DEVICE_REQUEST_GET_DESCRIPTOR:
            {
                p_desc = ((*p->iUSBDeviceController)->GetDescriptor)(
                        p->iUSBDeviceController,
                        (aSetup->iValue)>>8,
                        (aSetup->iValue)&0xFF,
                        aLength);
                if (p_desc)  {
                    *aData = (TUInt8 *)p_desc;
                    return ETrue;
                }
            }
            return EFalse;
        case USB_DEVICE_REQUEST_SET_DESCRIPTOR:
            return EFalse;
        case USB_DEVICE_REQUEST_GET_CONFIGURATION:
            *aLength = 1;
            p_data[0] = ((*p->iUSBDeviceController)->GetConfiguration)(
                p->iUSBDeviceController);
            return ETrue;
        case USB_DEVICE_REQUEST_SET_CONFIGURATION:
            ((*p->iUSBDeviceController)->SetConfiguration)(
                p->iUSBDeviceController,
                (aSetup->iValue & 0xFF));
            return ETrue;
        case USB_DEVICE_REQUEST_GET_INTERFACE:
        case USB_DEVICE_REQUEST_SET_INTERFACE:
        case USB_DEVICE_REQUEST_SYNCH_FRAME:
            // These are not handled
        default:
            // Anything else is definitely non standard.
            return EFalse;
    }
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_RegisterRequestTypeCallback
 *-------------------------------------------------------------------------*
 * Description:
 *      Register a handler for different types of control 0 requests.
 * Inputs:
 *      void *aWorkspace        -- USB Device's workspace
 *      TUInt8 aType            -- Type of control 0 request.  Can be
 *                                  USB_REQUEST_TYPE_STANDARD,
 *                                  USB_REQUEST_TYPE_CLASS,
 *                                  USB_REQUEST_TYPE_VENDOR, or
 *                                  USB_REQUEST_TYPE_RESERVED
 *      void *aBuffer           -- Buffer to receive data (up to endpoint
 *                                  max sized packet).
 *      void *aCallbackWorkspace -- Pointer to callback's workspace
 *      T_USBRrequestHandlerFunc aRequestHandler -- Function called when
 *                                  control endpoint 0 receives data
 *                                  of this type.
 * Outputs:
 *      T_uezError              -- Returns UEZ_ERROR_NONE if successfully
 *                                  registered.  Returns
 *                                  UEZ_ERROR_NOT_AVAILABLE if slot already
 *                                  registered.  Returns
 *                                  UEZ_ERROR_ILLEGAL_OPERATION if incorrect
 *                                  type.
 *-------------------------------------------------------------------------*/
static T_uezError NXP_LPC17xx_40xx_USBDevice_RegisterRequestTypeCallback(
            void *aWorkspace,
            TUInt8 aType,
            void *aBuffer,
            void *aCallbackWorkspace,
            T_USBRequestHandlerFunc aRequestHandler)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    T_USBDeviceRequestTypeHandlerStruct *p_handler;

    // Modify the callback table while no callbacks can occur
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (aType <= USB_REQUEST_TYPE_RESERVED) {
        p_handler = &p->iUSBDevice.iRequestTypeHandlers[aType];
        if ((p_handler->iHandler == 0) ||
                ((p_handler->iHandler == NXP_LPC17xx_40xx_USBDevice_StandardDeviceRequest) &&
                    (aType == USB_REQUEST_TYPE_STANDARD)))  {
            p_handler->iHandler = aRequestHandler;
            p_handler->iBuffer = aBuffer;
            p_handler->iWorkspace = aCallbackWorkspace;
        } else {
            error = UEZ_ERROR_NOT_AVAILABLE;
        }
    } else {
        error = UEZ_ERROR_ILLEGAL_OPERATION;
    }

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_UnregisterRequestTypeCallback
 *-------------------------------------------------------------------------*
 * Description:
 *      Register a handler for different types of control 0 requests.
 * Inputs:
 *      void *aWorkspace        -- USB Device's workspace
 *      TUInt8 aType            -- Type of control 0 request.  Can be
 *                                  USB_REQUEST_TYPE_STANDARD,
 *                                  USB_REQUEST_TYPE_CLASS,
 *                                  USB_REQUEST_TYPE_VENDOR, or
 *                                  USB_REQUEST_TYPE_RESERVED
 * Outputs:
 *      T_uezError              -- Returns UEZ_ERROR_NONE if successfully
 *                                  unregistered.  Returns
 *                                  UEZ_ERROR_NOT_FOUND if slot already
 *                                  unregistered.  Returns
 *                                  UEZ_ERROR_ILLEGAL_OPERATION if incorrect
 *                                  type.
 *-------------------------------------------------------------------------*/
static T_uezError NXP_LPC17xx_40xx_USBDevice_UnregisterRequestTypeCallback(
            void *aWorkspace,
            TUInt8 aType)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    T_USBDeviceRequestTypeHandlerStruct *p_handler;

    // Modify the callback table while no callbacks can occur
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (aType <= USB_REQUEST_TYPE_RESERVED) {
        p_handler = &p->iUSBDevice.iRequestTypeHandlers[aType];
        if ((p_handler->iHandler == 0) ||
                ((p_handler->iHandler == NXP_LPC17xx_40xx_USBDevice_StandardDeviceRequest) &&
                    (aType == USB_REQUEST_TYPE_STANDARD)))  {
            // Already unregistered
            error = UEZ_ERROR_NOT_FOUND;
        } else {
            // Unregister the function
            if (aType == USB_REQUEST_TYPE_STANDARD)
                p_handler->iHandler = NXP_LPC17xx_40xx_USBDevice_StandardDeviceRequest;
            else
                p_handler->iHandler = 0;
            p_handler->iBuffer = 0;
            p_handler->iWorkspace = 0;
        }
    } else {
        error = UEZ_ERROR_ILLEGAL_OPERATION;
    }

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  NXP_LPC17xx_40xx_USBDevice_InterruptNakEnable
 *-------------------------------------------------------------------------*
 * Description:
 *      Declare which type of Naks generate interrupts.
 * Inputs:
 *      T_LPC17xx_40xx_USBDev_Workspace *p -- workspace
 *      TUInt8 aNakBits             -- Nak bits
 *-------------------------------------------------------------------------*/
static void NXP_LPC17xx_40xx_USBDevice_InterruptNakEnable(
                void *aWorkspace,
                TUInt8 aNakBits)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p =
        (T_USBDevice_LPC17xx_40xx_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    (*p->iUSBDeviceController)->InterruptNakEnable(
            p->iUSBDeviceController,
            aNakBits);
    UEZSemaphoreRelease(p->iSem);
}

void USBDevice_LPC17xx_40xx_Create(const char *aName, const char *aHALName)
{
    T_USBDevice_LPC17xx_40xx_Workspace *p;

    // Register USBDevice (which links it to the USBDeviceController)
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&USBDevice_NXP_LPC17xx_40xx_Interface,
            0,
            (T_uezDeviceWorkspace **)&p);

    // Access the USBDevice HAL and link to this device
    HALInterfaceFind(
            aHALName,
            (T_halWorkspace **)&p->iUSBDeviceController);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_USB_DEVICE USBDevice_NXP_LPC17xx_40xx_Interface = {
	{
	    // Common device interface
	    "USBDevice:NXP:LPC17xx_40xx",
	    0x0100,
	    USBDevice_NXP_InitializeWorkspace_USBDevice,
	    sizeof(T_USBDevice_LPC17xx_40xx_Workspace),
	},
	
    // Functions
    NXP_LPC17xx_40xx_USBDevice_Configure,
    NXP_LPC17xx_40xx_USBDevice_Initialize,
    NXP_LPC17xx_40xx_USBDevice_Connect,
    NXP_LPC17xx_40xx_USBDevice_Disconnect,
    NXP_LPC17xx_40xx_USBDevice_SetAddress,
    NXP_LPC17xx_40xx_USBDevice_EndpointConfigure,
    NXP_LPC17xx_40xx_USBDevice_Read,
    NXP_LPC17xx_40xx_USBDevice_Write,
    NXP_LPC17xx_40xx_USBDevice_SetStallState,
    NXP_LPC17xx_40xx_USBDevice_IsStalled,
    NXP_LPC17xx_40xx_USBDevice_EndpointInterruptsEnable,
    NXP_LPC17xx_40xx_USBDevice_EndpointInterruptsDisable,
    NXP_LPC17xx_40xx_USBDevice_ForceAddressAndConfiguration,
    NXP_LPC17xx_40xx_USBDevice_RegisterEndpointCallback,
    NXP_LPC17xx_40xx_USBDevice_UnregisterEndpointCallback,
    NXP_LPC17xx_40xx_USBDevice_RegisterRequestTypeCallback,
    NXP_LPC17xx_40xx_USBDevice_UnregisterRequestTypeCallback,
    NXP_LPC17xx_40xx_USBDevice_ProcessEndpoints,
    NXP_LPC17xx_40xx_USBDevice_InterruptNakEnable,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  lpc1788_USBDevice.c
 *-------------------------------------------------------------------------*/
