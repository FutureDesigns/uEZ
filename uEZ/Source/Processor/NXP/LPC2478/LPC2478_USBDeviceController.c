/*-------------------------------------------------------------------------*
 * File:  LPC2478_USBDev.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC2478 USBDev (Ethernet) Driver.
 * Implementation:
 *
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
#include "Config.h"
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_USBDeviceController.h>
// TBD:  not here!
#include <uEZRTOS.h>

/*-------------------------------------------------------------------------*
 * USB Registers:
 *-------------------------------------------------------------------------*/

#ifndef USBIntSt
// USB Interrupt Status
#define USBIntSt        CPUReg32(0xE01FC1C0)

// USB Device Interrupt Registers:
#define USBDevIntSt     CPUReg32(0xFFE0C200)
#define USBDevIntEn     CPUReg32(0xFFE0C204)
#define USBDevIntClr	CPUReg32(0xFFE0C208)
#define USBDevIntSet	CPUReg32(0xFFE0C20C)
#define USBDevIntPri	CPUReg32(0xFFE0C22C)
// Endpoint Interrupt Registers
#define USBEpIntSt      CPUReg32(0xFFE0C230)
#define USBEpIntEn      CPUReg32(0xFFE0C234)
#define USBEpIntClr     CPUReg32(0xFFE0C238)
#define USBEpIntSet     CPUReg32(0xFFE0C23C)
#define USBEpIntPri     CPUReg32(0xFFE0C240)

// Endpoint Realization Registers
#define USBReEp         CPUReg32(0xFFE0C244)
#define USBEpInd        CPUReg32(0xFFE0C248)
#define USBEpMaxPSize     CPUReg32(0xFFE0C24C)

// USB Transfer registers
#define USBRxData               CPUReg32(0xFFE0C218)
#define USBRxPLen               CPUReg32(0xFFE0C220)
#define USBTxData               CPUReg32(0xFFE0C21C)
#define USBTxPLen               CPUReg32(0xFFE0C224)
#define USBCtrl                 CPUReg32(0xFFE0C228)

// Command registers
#define USBCmdCode      CPUReg32(0xFFE0C210)
#define USBCmdData      CPUReg32(0xFFE0C214)
#endif

#define dprintf(...)

    // USB Device Interrupts Bits:
        // every 1 ms interrupt (for isochronous packet transfers)
        #define USB_FRAME                       (1<<0)

        // Fast interrupt transfer
        #define EP_FAST                     (1<<1)

        // Slow interrupt tranfer
        #define EP_SLOW                     (1<<2)

        // Device status changed
        #define DEV_STAT                    (1<<3)

        // Command code register is empty (new commands can be written)
        #define CCEMTY                      (1<<4)

        // Command data register is full (data can be read now)
        #define CDFULL                      (1<<5)

        // Current packet in FIFO has been transferred to the CPU
        #define RxENDPKT                    (1<<6)

        // The number of data bytes transferred to the FIFO equals
        // the number of bytes programmed in the TxPacket length register
        #define TxENDPKT                    (1<<7)

        // Endpoints realized.
        #define EP_RLZED                    (1<<8)

        // Error interrupt.
        #define ERR_INT                     (1<<9)


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
// Device commands:
#define USB_DEVICE_CMD_SET_ADDRESS                      0xD0
    #define USB_DEVICE_SET_ADDRESS_ADDRESS		    (1<<0) // 6 bits
    #define USB_DEVICE_SET_ADDRESS_ENABLE		    (1<<7)
#define USB_DEVICE_CMD_CONFIG                           0xD8
    #define USB_DEVICE_CONFIG                               (1<<0)
#define USB_DEVICE_CMD_SET_MODE                         0xF3
#define USB_DEVICE_CMD_READ_CUR_FRAME_NR                0xF5
#define USB_DEVICE_CMD_READ_TEST_REG                    0xFD
#define USB_DEVICE_CMD_STATUS                           0xFE
#define USB_DEVICE_CMD_GET_ERROR_CODE                   0xFF
#define USB_DEVICE_CMD_READ_ERROR_STATUS                0xFB

// Endpoint commands:
#define USB_ENDPOINT_CMD_SELECT                         0x00
#define USB_ENDPOINT_CMD_SELECT_CLEAR                   0x40
#define USB_ENDPOINT_CMD_SET_STATUS                     0x40
    #define ENDPOINT_STATUS_STALLED                         (1<<0)
    #define ENDPOINT_STATUS_DISABLED                        (1<<5)
    #define ENDPOINT_STATUS_RATE_FEEDBACK_MODE              (1<<6)
    #define ENDPOINT_STATUS_CONDITIONAL_STALL               (1<<7)
#define USB_ENDPOINT_CMD_CLEAR_BUFFER                   0xF2
#define USB_ENDPOINT_CMD_VALIDATE_BUFFER                0xFA

// Bits: USBCtrl
#define RD_EN                   (1<<0)
#define WR_EN                   (1<<1)
#define LOG_ENDPOINT            (1<<2)

// Bits: USBRxPLen
#define PKT_LNGTH               (1<<0) // 10 bits
    #define PKT_LNGTH_MASK          ((1<<10)-1)
#define DV                      (1<<10)
#define PKT_RDY                 (1<<11)


/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_USBDeviceController *iHAL;
    T_USBDeviceControllerSettings iSettings;
    TUInt8 iConfigurationNumber;

} T_LPC2478_USBDev_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_USBDeviceController LPC2478_USBDev_Interface;
T_LPC2478_USBDev_Workspace *G_LPC2478_USBDev_Workspace=0;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void LPC2478_USBDev_EndpointConfigure(
        void *aWorkspace,
        TUInt8 aEndpoint,
        TUInt16 aMaxPacketSize);

/*-------------------------------------------------------------------------*
 * Internal Function:  IEndpointToIndex
 *-------------------------------------------------------------------------*
 * Description:
 *      Convert a standard endpoint identifier to a LPC21xx USB endpoint
 *      index number.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint number
 * Outputs:
 *      TUInt32 aIndex           -- USB Hardware endpoint number
 *-------------------------------------------------------------------------*/
static INLINE TUInt32 IEndpointToIndex(TUInt8 aEndpoint)
{
    TUInt32 index = aEndpoint & 0x0F;
    index <<= 1;
    index |= ((aEndpoint & 0x80)>>7);
    return index;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  IIndexToEndpoint
 *-------------------------------------------------------------------------*
 * Description:
 *      Convert a LPC21xx USB endpoint index number to a standard
 *      endpoint identifier.
 * Inputs:
 *      TUInt32 aIndex           -- USB Hardware endpoint number
 * Outputs:
 *      TUInt8 aEndpoint         -- Endpoint number
 *-------------------------------------------------------------------------*/
static INLINE TUInt8 IIndexToEndpoint(TUInt32 aIndex)
{
    TUInt8 endpointNum = (TUInt8) (aIndex>>1);
    endpointNum |= (aIndex & 1) << 7;
    return endpointNum;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  IWaitForDeviceInterrupt
 *-------------------------------------------------------------------------*
 * Description:
 *      Wait for any of the given device interrupt flags to go from 1 to 0.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt32 aDevInterruptFlags -- Flag(s) to wait to go from 0 to 1
 *-------------------------------------------------------------------------*/
static void IWaitForDeviceInterrupt(
                T_LPC2478_USBDev_Workspace *p,
                TUInt32 aDevInterruptFlags)
{
    PARAM_NOT_USED(p);
    while ((USBDevIntSt & aDevInterruptFlags) != aDevInterruptFlags)
        {}
    USBDevIntClr = aDevInterruptFlags;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  ILPC2478_USBDev_Command
 *-------------------------------------------------------------------------*
 * Description:
 *      Put a command in the command buffer after clearing the command
 *      related device interrupt flags.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt8 aCommand           -- Command to send
 *-------------------------------------------------------------------------*/
static void ILPC2478_USBDev_Command(
            T_LPC2478_USBDev_Workspace *p,
            TUInt8 aCommand)
{
if (aCommand == USB_ENDPOINT_CMD_VALIDATE_BUFFER)
    dprintf("[Cmd V]");
else if (aCommand == USB_ENDPOINT_CMD_CLEAR_BUFFER)
    dprintf("[Cmd C]");
else if (aCommand <= 0x3F)
    dprintf("[Cmd S%d]", aCommand&0x3F);
else if ((aCommand & 0xC0)==0x40)
    dprintf("[Cmd SC%d]", aCommand&0x3F);
else if (aCommand == USB_DEVICE_CMD_SET_MODE)
    dprintf("[Cmd SM]");
else if (aCommand == USB_DEVICE_CMD_STATUS)
    ; // do nothing
else
    dprintf("[Cmd ?%02X]", aCommand);
    // Start by clearing the full and empty flags for the
    // command.
    USBDevIntClr = CDFULL | CCEMTY;

    // Write a command in the form of:
    //    0x00CC0500
    //    where CC = 8-bit command
    USBCmdCode = 0x00000500 | (aCommand << 16);

    // Wait for there to be room for another command
    IWaitForDeviceInterrupt(p, CCEMTY);
}

/*-------------------------------------------------------------------------*
 * Internal Function:  ILPC2478_USBDev_CommandAndWrite
 *-------------------------------------------------------------------------*
 * Description:
 *      Write a command and then write the data after it.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt8 aCommand           -- Command to send
 *      TUInt16 aData             -- Data to go after command
 *-------------------------------------------------------------------------*/
static void ILPC2478_USBDev_CommandAndWrite(
                T_LPC2478_USBDev_Workspace *p,
                TUInt8 aCommand,
                TUInt16 aData)
{
    // First, write the command
    ILPC2478_USBDev_Command(p, aCommand);

    // Then, write the data
    USBCmdCode = 0x00000100 | (aData << 16);

    // Wait for room to be made available for more data
    IWaitForDeviceInterrupt(p, CCEMTY);
}

/*-------------------------------------------------------------------------*
 * Internal Function:  ILPC2478_USBDev_CommandAndRead
 *-------------------------------------------------------------------------*
 * Description:
 *      Write a command and then read the data after it.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt8 aCommand           -- Command to send
 * Outputs:
 *      TUInt8                    -- Data received
 *-------------------------------------------------------------------------*/
static TUInt8 ILPC2478_USBDev_CommandAndRead(
                T_LPC2478_USBDev_Workspace *p,
                TUInt8 aCommand)
{
    // First, write a command
    ILPC2478_USBDev_Command(p, aCommand);

    // Second, read back a byte
    USBCmdCode = 0x00000200 | (aCommand << 16);

    // Wait until data is ready to be read
    IWaitForDeviceInterrupt(p, CDFULL);

    return (TUInt8)USBCmdData;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_InterruptNakEnable
 *-------------------------------------------------------------------------*
 * Description:
 *      Declares which NAK bits to produce interrupts on.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt8 aNakBits             -- Or'd list of NAK bits to interrupt on
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_InterruptNakEnable(
            void *aWorkspace,
            TUInt8 aNakBits)
{
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;

    ILPC2478_USBDev_CommandAndWrite(p, USB_DEVICE_CMD_SET_MODE, aNakBits);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_EndpointInterruptsEnable
 *-------------------------------------------------------------------------*
 * Description:
 *      Allow an endpoint to generate interrupts.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt8 aEndpoint            -- Endpoint to trigger interrupts
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_EndpointInterruptsEnable(
                void *aWorkspace,
                TUInt8 aEndpoint)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);

    // Enable interrupts for this endpoint
    // using regular "slow" method
    USBEpIntEn |= (1 << aIndex);
    USBDevIntEn |= EP_SLOW;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_EndpointInterruptsDisable
 *-------------------------------------------------------------------------*
 * Description:
 *      Allow an endpoint to generate interrupts.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt8 aEndpoint            -- Endpoint to trigger interrupts
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_EndpointInterruptsDisable(
                void *aWorkspace,
                TUInt8 aEndpoint)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);

    // Disable interrupts for this endpoint
    USBEpIntEn &= ~(1 << aIndex);
}


/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_USBDev_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this USBDev Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_USBDev_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;

    // Nothing is defined in this case
    p->iSettings.iUSBDevice = 0;
    p->iSettings.iUSBDescriptorTable = 0;

    // Remember this for the interrupt
    G_LPC2478_USBDev_Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_USBDev_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the USBDev screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      T_USBDevSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_USBDev_Configure(
        void *aWorkspace,
        T_USBDeviceControllerSettings *iSettings)
{
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;

    // Copy over the configuration
    p->iSettings = *iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC2478_USBDev_InterruptProcess
 *---------------------------------------------------------------------------*
 * Description:
 *      Process USB Device Interrupts
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *---------------------------------------------------------------------------*/
static void ILPC2478_USBDev_InterruptProcess(T_LPC2478_USBDev_Workspace *p)
{
    TUInt32 status;
    T_USBStatus usbStatus;
    T_USBDevice *p_dev = p->iSettings.iUSBDevice;

    status = USBDevIntSt;

    if (status & DEV_STAT) {
        // Clear the device status change flag
        USBDevIntClr = DEV_STAT;

        // Read the device status and run the callback (if it exists)
        usbStatus = (T_USBStatus)ILPC2478_USBDev_CommandAndRead(
                        p,
                        USB_DEVICE_CMD_STATUS);
        if (p_dev->iStatusChangeFunc)
            p_dev->iStatusChangeFunc(
                p_dev->iCallbackWorkspace,
                usbStatus);
    }

    // Handle Endpoints (SLOW)
    if (status & EP_SLOW) {

#if 0
        USBDevIntClr = EP_SLOW;
        // Handle endpoints with data on them
        for (i = 0; i < 32; i++) {
            if (USBEpIntSt & (1<<i)) {
                // Clear the interrupt flag
                USBEpIntClr = (1<<i);

                // Be sure wait for a command to be available
                IWaitForDeviceInterrupt(p, CDFULL);

                // Get the end point's status
                endpointStatus = USBCmdData;

                // Call the end point handler
                if (i & 1)  {
                    if (p_dev->iEndpointFuncs[i>>1].iIn) {
                        p_dev->iEndpointFuncs[i>>1].iIn(
                            p_dev->iCallbackWorkspace,
                            IIndexToEndpoint(i),
                            endpointStatus);
                    } else {
                        // TBD: Clear the endpoint?
                    }
                } else {
                    if (p_dev->iEndpointFuncs[i>>1].iOut) {
                        p_dev->iEndpointFuncs[i>>1].iOut(
                            p_dev->iCallbackWorkspace,
                            IIndexToEndpoint(i),
                            endpointStatus);
                    } else {
                        // TBD: Clear the endpoint?
                    }
                }
            }
        }
#else
        if (p_dev->iStatusEndpointReadyFunc) {
            InterruptDisable(INTERRUPT_CHANNEL_USB);
            p_dev->iStatusEndpointReadyFunc(
                p_dev->iCallbackWorkspace);
        }
#endif
    }

    // FRAME status?
    if (status & USB_FRAME) {
        // Clear FRAME flag
        USBDevIntClr = USB_FRAME;

        // Do nothing for now
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_USBDev_ProcessEndpoints
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if an endpoint is ready and if it is, call the function
 *      associated with that endpoint.  Clear any related interrupts.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *---------------------------------------------------------------------------*/
void LPC2478_USBDev_ProcessEndpoints(void *aWorkspace)
{
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;
    TUInt32 i;
    T_USBEndpointStatus endpointStatus;
    T_USBDevice *p_dev = p->iSettings.iUSBDevice;

    USBDevIntClr = EP_SLOW;

    // Handle endpoints with data on them
    for (i = 0; i < 32; i++) {
        if (USBEpIntSt & (1<<i)) {
            // Clear the interrupt flag
            USBEpIntClr = (1<<i);

            // Be sure wait for a command to be available
            IWaitForDeviceInterrupt(p, CDFULL);

            // Get the end point's status
            endpointStatus = USBCmdData;

            // Call the end point handler
            if (i & 1)  {
dprintf("\n[IN%d]", i>>1);
                if (p_dev->iEndpointFuncs[i>>1].iIn) {
                    p_dev->iEndpointFuncs[i>>1].iIn(
                        p_dev->iCallbackWorkspace,
                        IIndexToEndpoint(i),
                        endpointStatus);
                } else {
                    // TBD: Clear the endpoint?
                }
            } else {
dprintf("\n[OUT%d]", i>>1);
                if (p_dev->iEndpointFuncs[i>>1].iOut) {
                    p_dev->iEndpointFuncs[i>>1].iOut(
                        p_dev->iCallbackWorkspace,
                        IIndexToEndpoint(i),
                        endpointStatus);
                } else {
                    // TBD: Clear the endpoint?
                }
            }
        }
    }

    // Ensure the interrupts are back on
    InterruptEnable(INTERRUPT_CHANNEL_USB);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC2478_USBDev_InterruptVector
 *---------------------------------------------------------------------------*
 * Description:
 *      Process USB Device Interrupts
 *---------------------------------------------------------------------------*/
void LPC2478_InterruptProcess_USBDevice(void)
{
    if (G_LPC2478_USBDev_Workspace)
        ILPC2478_USBDev_InterruptProcess(G_LPC2478_USBDev_Workspace);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_Connect
 *-------------------------------------------------------------------------*
 * Description:
 *      Connect to the USB bus.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_Connect(void *aWorkspace)
{
    ILPC2478_USBDev_CommandAndWrite(
        (T_LPC2478_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_STATUS,
        USB_STATUS_CONNECT);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_Disconnect
 *-------------------------------------------------------------------------*
 * Description:
 *      Disconnect from the USB bus.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_Disconnect(void *aWorkspace)
{
    ILPC2478_USBDev_CommandAndWrite(
        (T_LPC2478_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_STATUS,
        0x00);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_SetAddress
 *-------------------------------------------------------------------------*
 * Description:
 *      Sets the USB address for this device.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aAddress             -- 7 bit address of this device.
 *-------------------------------------------------------------------------*/
void LPC2478_USBDev_SetAddress(void *aWorkspace, TUInt8 aAddress)
{
    ILPC2478_USBDev_CommandAndWrite(
        (T_LPC2478_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_SET_ADDRESS,
        USB_DEVICE_SET_ADDRESS_ENABLE | aAddress);
    ILPC2478_USBDev_CommandAndWrite(
        (T_LPC2478_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_SET_ADDRESS,
        USB_DEVICE_SET_ADDRESS_ENABLE | aAddress);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_DeviceIsConfigured
 *-------------------------------------------------------------------------*
 * Description:
 *      Declare the USB Device as configured.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *-------------------------------------------------------------------------*/
void LPC2478_USBDev_DeviceIsConfigured(void *aWorkspace)
{
    ILPC2478_USBDev_CommandAndWrite(
            (T_LPC2478_USBDev_Workspace *)aWorkspace,
            USB_DEVICE_CMD_CONFIG,
            USB_DEVICE_CONFIG);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_DeviceIsNotConfigured
 *-------------------------------------------------------------------------*
 * Description:
 *      Declare the USB Device as NOT configured.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *-------------------------------------------------------------------------*/
void LPC2478_USBDev_DeviceIsNotConfigured(void *aWorkspace)
{
    ILPC2478_USBDev_CommandAndWrite(
        (T_LPC2478_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_CONFIG,
        0);
}


/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_Read
 *-------------------------------------------------------------------------*
 * Description:
 *      Read data from an endpoint
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aEndpoint            -- Endpoint with data
 *      TUInt8 *aData               -- Pointer to place to put data.
 *      TUInt16 iMaxLen             -- Maximum number of bytes for data.
 * Outputs:
 *      TInt16                      -- Number of bytes received, or -1
 *-------------------------------------------------------------------------*/
TInt16 LPC2478_USBDev_Read(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TUInt8 *aData,
            TUInt16 iMaxLen)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);
    TUInt32 rxLength;
    TUInt32 rxData;
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;

dprintf("[R %d %d]", aEndpoint, iMaxLen);
    // Setup the endpoint for reading
    USBCtrl = RD_EN | ((aEndpoint & 0xF) << 2);

    // Wait for the packet to be ready (or an error)
    while (1)  {
        if ((rxLength = USBRxPLen) & PKT_RDY)
            break;
    }

    // Is data valid?
    if (!(rxLength & DV))
        return -1;

    // Data is valid.
    // Clean up and get the length.
    rxLength &= PKT_LNGTH_MASK;

    // Read data while it is available (32 bits at a time).
    while (USBCtrl & RD_EN) {
        // Get the 32 bits (or less)
        rxData = USBRxData;

        // Only store the data if there is a place to put it
        if (aData != 0) {
            // How much room is left?
            if (iMaxLen >= 4)  {
                // Do a group of 4 bytes quickly.
                // NOTE:  Compiler may optimize
                aData[0] = (rxData>>0) & 0xFF;
                aData[1] = (rxData>>8) & 0xFF;
                aData[2] = (rxData>>16) & 0xFF;
                aData[3] = (rxData>>24) & 0xFF;
                iMaxLen -= 4;
                aData += 4;
            } else {
                // Do a group of less than 4 bytes
                while (iMaxLen)  {
                    // One byte at a time
                    *(aData++) = rxData & 0xFF;
                    rxData >>= 8;
                    iMaxLen--;
                }
            }
        }
    }

    // Regardless of what happened above,
    // do a 'selection' and a 'clear' of the buffer
    ILPC2478_USBDev_Command(p, USB_ENDPOINT_CMD_SELECT | aIndex);
    ILPC2478_USBDev_Command(p, USB_ENDPOINT_CMD_CLEAR_BUFFER);

    return rxLength;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_Write
 *-------------------------------------------------------------------------*
 * Description:
 *      Write data to an endpoint
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aEndpoint            -- Endpoint to receive data
 *      TUInt8 *aData               -- Pointer to data to send.
 *      TUInt16 aLength             -- Number of bytes to send
 * Outputs:
 *      TUInt16                     -- aLength
 *-------------------------------------------------------------------------*/
TUInt16 LPC2478_USBDev_Write(
                void *aWorkspace,
                TUInt8 aEndpoint,
                TUInt8 *aData,
                TUInt16 aLength)
{
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);

dprintf("[W %d %d]", aEndpoint, aLength);
    // Enable writing to the end point and the number of bytes to be written
    USBCtrl = WR_EN | ((aEndpoint & 0xF) << 2);
    USBTxPLen = aLength;

    if (aLength == 0)  {
        while (USBCtrl & WR_EN)  {
            // Just force zeroes into without using
            // the pointer.
            USBTxData = 0;
        }
    } else {
        // Write data as long as it lets us
        while (USBCtrl & WR_EN)  {
            // Write in 32 bit blocks
            USBTxData =
                (aData[3] << 24) |
                (aData[2] << 16) |
                (aData[1] << 8) |
                aData[0];
            aData += 4;
        }
    }

    // Do a 'select' and 'validate' on the endpoint
    ILPC2478_USBDev_Command(p, USB_ENDPOINT_CMD_SELECT | aIndex);
    ILPC2478_USBDev_Command(p, USB_ENDPOINT_CMD_VALIDATE_BUFFER);

    return aLength;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_SetStallState
 *-------------------------------------------------------------------------*
 * Description:
 *      Set the stall state of an endpoint
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aEndpoint            -- Endpoint to set to stalled or not
 *                                     stalled.
 *      TBool                       -- ETrue for stall, else EFalse for
 *                                     not stalled.
 *-------------------------------------------------------------------------*/
void LPC2478_USBDev_SetStallState(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TBool aStallState)
{
dprintf("ST %d %d\n", aEndpoint, aStallState);
    // Write the stall bit of the end point.
    ILPC2478_USBDev_CommandAndWrite(
        (T_LPC2478_USBDev_Workspace *)aWorkspace,
        USB_ENDPOINT_CMD_SET_STATUS | IEndpointToIndex(aEndpoint),
        aStallState ? ENDPOINT_STATUS_STALLED : 0);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_IsStalled
 *-------------------------------------------------------------------------*
 * Description:
 *      Determine if an endpoint is stalled.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aEndpoint            -- Endpoint to check if stalled
 * Outputs:
 *      TBool                       -- ETrue if stalled, else EFalse
 *-------------------------------------------------------------------------*/
TBool LPC2478_USBDev_IsStalled(void *aWorkspace, TUInt8 aEndpoint)
{
    // Determine if the endpoint's stalled bit is set.
    return (ILPC2478_USBDev_CommandAndRead(
                (T_LPC2478_USBDev_Workspace *)aWorkspace,
                USB_ENDPOINT_CMD_SELECT |
                    IEndpointToIndex(aEndpoint)) &
                ENDPOINT_STATUS_STALLED);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_SetConfiguration
 *-------------------------------------------------------------------------*
 * Description:
 *      Go through the descriptor table and enable all endpoints for
 *      the given descriptor index and alternative setting.
 *      All corresponding endpoints are configured for matches.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aConfigurationIndex-- Descriptor index of config to use.
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_SetConfiguration(
                void *aWorkspace,
                TUInt8 aConfigurationIndex)
{
    T_LPC2478_USBDev_Workspace *p_workspace =
            (T_LPC2478_USBDev_Workspace *)aWorkspace;
    const TUInt8 *p = p_workspace->iSettings.iUSBDescriptorTable;
//    TUInt8 configuration = 0x80;

    T_USBDescriptorHeader *p_header = (T_USBDescriptorHeader *)p;

dprintf("\n\n---> [Cf %d]", aConfigurationIndex);

    // Index currently is 0 or non-zero.  If 0, it means
    // we are closing out the configuration.  Is it a zero?
    if (aConfigurationIndex == 0)  {
        // Yes, then kill the configuration
        LPC2478_USBDev_DeviceIsNotConfigured(aWorkspace);
    } else {
        // Otherwise, we are going to use the existing configuration
        while (p_header->iLength)  {
            if (p_header->iDescriptorType == USB_DESCRIPTOR_CONFIGURATION)  {
                // Found a configuration, the following
                // descriptors are now in that configuration
//                configuration =
//                    ((T_USBDescriptorConfiguration *)p)->iConfigurationValue;
            } else if (p_header->iDescriptorType == USB_DESCRIPTOR_ENDPOINT)  {
                // Only configure this endpoint if we have a matching
                // configuration number and alternate setting number.
                #define p_endpoint ((T_USBDescriptorEndpoint *)p)
                LPC2478_USBDev_EndpointConfigure(
                    aWorkspace,
                    p_endpoint->iAddress,
                    p_endpoint->iMaximumPacketSize);
            }
            // Next descriptor
            p += p_header->iLength;
            p_header = (T_USBDescriptorHeader *)p;
        }
        // Got to the end
        // Report that this device is configured.
        LPC2478_USBDev_DeviceIsConfigured((void *)p_workspace);
    }

    // Remember our configuration number
    p_workspace->iConfigurationNumber = aConfigurationIndex;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_GetConfiguration
 *-------------------------------------------------------------------------*
 * Description:
 *      Go through the descriptor table and enable all endpoints for
 *      the given descriptor index and alternative setting.
 *      All corresponding endpoints are configured for matches.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 * Outputs:
 *      TUInt8 aConfigurationIndex-- Descriptor index of config in use.
 *-------------------------------------------------------------------------*/
static TUInt8 LPC2478_USBDev_GetConfiguration(
                void *aWorkspace)
{
    T_LPC2478_USBDev_Workspace *p_workspace =
            (T_LPC2478_USBDev_Workspace *)aWorkspace;

    // Report our configuration
    return p_workspace->iConfigurationNumber;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_ForceAddressAndConfiguration
 *-------------------------------------------------------------------------*
 * Description:
 *      Force the USB to go to a given address and configuration.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aAddress           -- Address of connection
 *      TUInt8 aConfigNum         -- Number of configuration to use
 *-------------------------------------------------------------------------*/
void LPC2478_USBDev_ForceAddressAndConfiguration(
            void *aWorkspace,
            TUInt8 aAddress,
            TUInt8 aConfigNum)
{
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;

    LPC2478_USBDev_SetAddress(aWorkspace, aAddress);
    LPC2478_USBDev_SetConfiguration(aWorkspace, aConfigNum);
    p->iConfigurationNumber = aConfigNum;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  IEndpointEnable
 *-------------------------------------------------------------------------*
 * Description:
 *      Enable endpoint.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt32 aIndex            -- Index of endpoint
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_EndpointEnable(
                T_LPC2478_USBDev_Workspace *p,
                int aIndex)
{
    ILPC2478_USBDev_CommandAndWrite(
        p,
        USB_ENDPOINT_CMD_SET_STATUS | aIndex,
        0);
}

/*-------------------------------------------------------------------------*
 * Internal Function:  LPC2478_USBDev_EndpointDisable
 *-------------------------------------------------------------------------*
 * Description:
 *      Disable endpoint.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt32 aIndex            -- Index of endpoint
 *-------------------------------------------------------------------------*/
#if 0
static void LPC2478_USBDev_EndpointDisable(
                T_LPC2478_USBDev_Workspace *p,
                int aIndex)
{
    ILPC2478_USBDev_CommandAndWrite(
            p,
            USB_ENDPOINT_CMD_SET_STATUS | aIndex,
            ENDPOINT_STATUS_DISABLED);
}
#endif

/*-------------------------------------------------------------------------*
 * Internal Function:  LPC2478_USBDev_EndpointRealize
 *-------------------------------------------------------------------------*
 * Description:
 *      Realize an endpoint by telling the hardware to reserve space and
 *      to set the maximum packet size.
 * Inputs:
 *      T_LPC2478_USBDev_Workspace *p -- workspace
 *      TUInt32 aIndex            -- Index of endpoint
 *      TUInt16 aMaxPacketSize    -- Maximum size of packet (in bytes)
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_EndpointRealize(
                T_LPC2478_USBDev_Workspace *p,
                TUInt32 aIndex,
                TUInt16 aMaxPacketSize)
{
    // Declare which endpoint is going to be realized
    // using a bit position.
    USBReEp |= (1 << aIndex);
    USBEpInd = aIndex;

    // Declare size of maximum packets
    USBEpMaxPSize = aMaxPacketSize;

    // Wait for endpoint to be fully realized
    IWaitForDeviceInterrupt(p, EP_RLZED);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_EndpointConfigure
 *-------------------------------------------------------------------------*
 * Description:
 *      Realize and enable an endpoint.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aEndpoint            -- Endpoint identifier
 *      TUInt16 aMaxPacketSize      -- Maximum size of packets
 *-------------------------------------------------------------------------*/
static void LPC2478_USBDev_EndpointConfigure(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TUInt16 aMaxPacketSize)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;

    // realise EP
    LPC2478_USBDev_EndpointRealize(p, aIndex, aMaxPacketSize);

    // enable EP
    LPC2478_USBDev_EndpointEnable(
            p,
            aIndex);
}

/*-------------------------------------------------------------------------*
 * Function:  USBInitialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the hardware
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *-------------------------------------------------------------------------*/
void LPC2478_USBDev_Initialize(void *aWorkspace)
{
    extern void USBSharedInterruptSetup(void);
    TUInt8 i;
    T_LPC2478_USBDev_Workspace *p = (T_LPC2478_USBDev_Workspace *)aWorkspace;

    // Ensure power is on
    PCONP |= 0x80000000;

    // Make sure the clock is enabled
    OTG_CLK_CTRL = 0x0000001B;
    while ((OTG_CLK_STAT & 0x0000001B) != 0x1B) {
        ;
    }
    OTG_STAT_CTRL = 0x03;	/* This has to be set after OTG_CLK_CTRL configuration. */

    // Setup standard control endpoints
    LPC2478_USBDev_EndpointConfigure(
        aWorkspace,
        0x00,
        USB_ENDPOINT0_PACKET_SIZE);
    LPC2478_USBDev_EndpointConfigure(
        aWorkspace,
        0x80,
        USB_ENDPOINT0_PACKET_SIZE);

    // Only ACKs generate interrupts
//    LPC2478_USBDev_InterruptNakEnable(aWorkspace, 0);
    LPC2478_USBDev_InterruptNakEnable(aWorkspace, USB_DEVICE_SET_MODE_INAK_BI);

    // Setup interrupts for endpoints
    for (i=0; i<MAX_NUM_ENDPOINTS; i++)  {
        if (p->iSettings.iUSBDevice->iEndpointFuncs[i].iIn)
            LPC2478_USBDev_EndpointInterruptsEnable(aWorkspace,
                USB_ENDPOINT_ADDRESS(i, USB_ENDPOINT_ADDRESS_DIR_IN));
        if (p->iSettings.iUSBDevice->iEndpointFuncs[i].iOut)
            LPC2478_USBDev_EndpointInterruptsEnable(aWorkspace,
                USB_ENDPOINT_ADDRESS(i, USB_ENDPOINT_ADDRESS_DIR_OUT));
    }

    USBSharedInterruptSetup();
}

/*-------------------------------------------------------------------------*
 * Function:  LPC2478_USBDev_GetDescriptor
 *-------------------------------------------------------------------------*
 * Description:
 *      Search for a descriptor in the descriptor table based on the
 *      type and the index (of that type).  Return a pointer to the data
 *      and fill in the length of the data.
 * Inputs:
 *      void *aWorkspace          -- USBDev Controller's workspace
 *      TUInt8 aType              -- Type of descriptor to find
 *      TUInt16 aIndex            -- Index to descriptor (zero based)
 *      TUInt16 *aLength          -- Pointer to place to store length
 * Outputs:
 *      TUInt8 *                  -- Pointer to descriptor, or 0 for none.
 *-------------------------------------------------------------------------*/
const TUInt8 *LPC2478_USBDev_GetDescriptor(
            void *aWorkspace,
            TUInt8 aType,
            TUInt16 aIndex,
            TUInt16 *aLength)
{
    const TUInt8 *p = ((T_LPC2478_USBDev_Workspace *)aWorkspace)->
                    iSettings.iUSBDescriptorTable;
    T_USBDescriptorHeader *p_header = (T_USBDescriptorHeader *)p;

dprintf("[GetDesc %d %d]", aType, aIndex);
    while (p_header->iLength)  {
dprintf("<%p %d %d>", p_header, p_header->iDescriptorType, p_header->iLength);
        if (p_header->iDescriptorType == aType)  {
            if (aIndex==0)  {
                // Found the nth descriptor of the given type
                // Are we a configuration type?
                if (aType == USB_DESCRIPTOR_CONFIGURATION)  {
                    *aLength = ((T_USBDescriptorConfiguration *)p)->iTotalLength;
                } else {
                    *aLength = p_header->iLength;
                }
dprintf("[Found %p]\n", p);
                return p;
            } else {
                // Found the right type, but wrong index.
                // Keep looking.
                aIndex--;
            }
        }
        // Next descriptor
        p += p_header->iLength;
        p_header = (T_USBDescriptorHeader *)p;
    }
dprintf("[Not Found]\n");
    // If we got here, we never found it.
    return 0;
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_USBDevice_PortA_Require(
        const T_LPC2478_USBDevice_Settings *aSettings)
{
    static const T_LPC2478_PINSEL_ConfigList pinsVBUS[] = {
            {GPIO_P1_30, 2}, // USB_VBUS
    };
    static const T_LPC2478_PINSEL_ConfigList pinsDP1[] = {
            {GPIO_P0_29, 1}, // USB_D+1
    };
    static const T_LPC2478_PINSEL_ConfigList pinsDM1[] = {
            {GPIO_P0_30, 1}, // USB_D-1
    };
    static const T_LPC2478_PINSEL_ConfigList pinsCONNECT1[] = {
            {GPIO_P2_9,  1}, // USB_CONNECT1
    };
    static const T_LPC2478_PINSEL_ConfigList pinsUP_LED1[] = {
            {GPIO_P1_18, 1}, // USB_UP_LED1
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register USB Device
    HALInterfaceRegister("USBDeviceController",
            (T_halInterface *)&LPC2478_USBDev_Interface, 0, 0);

    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iVBUS, pinsVBUS, ARRAY_COUNT(pinsVBUS));
    LPC2478_PINSEL_ConfigPin(aSettings->iDP, pinsDP1, ARRAY_COUNT(pinsDP1));
    LPC2478_PINSEL_ConfigPin(aSettings->iDM, pinsDM1, ARRAY_COUNT(pinsDM1));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iCONNECT, pinsCONNECT1,
            ARRAY_COUNT(pinsCONNECT1));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iUP_LED, pinsUP_LED1,
            ARRAY_COUNT(pinsUP_LED1));
}

void LPC2478_USBDevice_PortB_Require(
        const T_LPC2478_USBDevice_Settings *aSettings)
{
    static const T_LPC2478_PINSEL_ConfigList pinsVBUS[] = {
            {GPIO_P1_30, 2}, // USB_VBUS
    };
    static const T_LPC2478_PINSEL_ConfigList pinsDP2[] = {
            {GPIO_P0_31, 1}, // USB_D+2
    };
    static const T_LPC2478_PINSEL_ConfigList pinsCONNECT2[] = {
            {GPIO_P0_14, 2}, // USB_CONNECT2
    };
    static const T_LPC2478_PINSEL_ConfigList pinsUP_LED2[] = {
            {GPIO_P0_13, 1}, // USB_UP_LED2
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register USB Device
    HALInterfaceRegister("USBDeviceController",
            (T_halInterface *)&LPC2478_USBDev_Interface, 0, 0);

    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iVBUS, pinsVBUS, ARRAY_COUNT(pinsVBUS));
    LPC2478_PINSEL_ConfigPin(aSettings->iDP, pinsDP2, ARRAY_COUNT(pinsDP2));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iDM, 0, 0);
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iCONNECT, pinsCONNECT2,
            ARRAY_COUNT(pinsCONNECT2));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iUP_LED, pinsUP_LED2,
            ARRAY_COUNT(pinsUP_LED2));
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_USBDeviceController LPC2478_USBDev_Interface = {
    "NXP:LPC2478:USB Device Controller",
    0x0100,
    LPC2478_USBDev_InitializeWorkspace,
    sizeof(T_LPC2478_USBDev_Workspace),

    LPC2478_USBDev_Configure,
    LPC2478_USBDev_Initialize,
    LPC2478_USBDev_Connect,
    LPC2478_USBDev_Disconnect,
    LPC2478_USBDev_SetAddress,
    LPC2478_USBDev_SetConfiguration,
    LPC2478_USBDev_GetConfiguration,
    LPC2478_USBDev_EndpointConfigure,
    LPC2478_USBDev_Read,
    LPC2478_USBDev_Write,
    LPC2478_USBDev_SetStallState,
    LPC2478_USBDev_IsStalled,
    LPC2478_USBDev_EndpointInterruptsEnable,
    LPC2478_USBDev_EndpointInterruptsDisable,
    LPC2478_USBDev_ForceAddressAndConfiguration,
    LPC2478_USBDev_ProcessEndpoints,
    LPC2478_USBDev_GetDescriptor,
    LPC2478_USBDev_InterruptNakEnable,
};

/*-------------------------------------------------------------------------*
 * End of File:  USBDev.c
 *-------------------------------------------------------------------------*/
