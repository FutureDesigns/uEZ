/*-------------------------------------------------------------------------*
 * File:  HID_Generic.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <Device/USBHost.h>
#include <uEZDeviceTable.h>
#include "HID_Generic.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define HID_CLASS       0x03
#define HID_SUBCLASS    0x00
#define HID_PROTOCOL    0x00

#define HID_GET_REPORT      1
#define HID_GET_IDLE        2
#define HID_GET_PROTOCOL    3
#define HID_SET_REPORT      9
#define HID_SET_IDLE        10
#define HID_SET_PROTOCOL    11

#define HID_INDEFINITE_REPORT       0x0000

#define MAX_LENGTH_REPORT_DESCRIPTOR    2048

#define HID_TYPE_INPUT      1
#define HID_TYPE_OUTPUT     2
#define HID_TYPE_REPORT     3

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#define IGrab() UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease() UEZSemaphoreRelease(p->iSem)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_HID *iDevice;
    T_uezSemaphore iSem;
    TBool iNumOpen;
    TUInt16 iVendorID;
    TUInt16 iProductID;
    DEVICE_USBHost **iUSBHost;
    TBool iIsConnected;
    TBool iIsRegistered;
    TUInt8 iAddress;
    TUInt8 *iDescBuf;
    TUInt8 *iReportDesc;
    TUInt8 iEndpointInterruptIn;
    TUInt8 iEndpointInterruptOut;
    TUInt8 iInterval; // multiples of 4 ms (4 to 1020 ms intervals)

    T_usbHIDDescriptor iHIDDescriptor;
} T_Generic_HID_Workspace;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static T_uezError HID_Generic_ConnectedCheckMatch(
    void *aWorkspace,
    TUInt8 aAddress);
T_uezError HID_Generic_Disconnected(void *aWorkspace, TUInt8 aDeviceAddress);
extern TUInt16 ReadLE16U(volatile TUInt8 *pmem);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
const T_USBHostDeviceDriverAPI G_hidAPI = {
    HID_Generic_ConnectedCheckMatch,
    HID_Generic_Disconnected };

/*---------------------------------------------------------------------------*
 * Routine:  HID_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a generic HID workspace
 * Inputs:
 *      void *aWorkspace             -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError HID_Generic_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;

    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    p->iNumOpen = 0;
    p->iVendorID = 0;
    p->iProductID = 0;
    p->iUSBHost = 0;
    p->iIsRegistered = EFalse;
    p->iIsConnected = EFalse;
    p->iAddress = 0;
    p->iDescBuf = 0;
    p->iReportDesc = 0;
    p->iEndpointInterruptIn = 0;
    p->iEndpointInterruptOut = 0;
    p->iInterval = 0; // indefinite

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

static T_uezError HID_Generic_ConnectedCheckMatch(
    void *aWorkspace,
    TUInt8 aAddress)
{
    T_uezError error;
    TUInt32 descLen;
    TUInt8 *desc_ptr;
    TUInt8 hid_int_found;
    TUInt8 recv[10];
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;

    IGrab();

    // Try to do the initialization
    // Allocate some buffers
    p->iDescBuf = (*p->iUSBHost)->AllocBuffer(p->iUSBHost, 128);
    p->iReportDesc = (*p->iUSBHost)->AllocBuffer(p->iUSBHost, MAX_LENGTH_REPORT_DESCRIPTOR);
    p->iAddress = aAddress;

    // Get configuration information
    error = (*p->iUSBHost)->GetDescriptor(p->iUSBHost, aAddress,
        USB_DESCRIPTOR_TYPE_CONFIGURATION, 0, p->iDescBuf, 9, 5000);
    if (error) {
        IRelease();
        return error;
    }

    // Get type configuration descriptor
    descLen = ReadLE16U(&p->iDescBuf[2]);
    error = (*p->iUSBHost)->GetDescriptor(p->iUSBHost, aAddress,
        USB_DESCRIPTOR_TYPE_CONFIGURATION, 0, p->iDescBuf, descLen, 5000);
    if (error) {
        IRelease();
        return error;
    }

    desc_ptr = p->iDescBuf;
    hid_int_found = 0;

    // Make sure this is a type configuration
    if (desc_ptr[1] != USB_DESCRIPTOR_TYPE_CONFIGURATION) {
        IRelease();
        return UEZ_ERROR_UNKNOWN;
    }

    // Skip down to the data
    desc_ptr += desc_ptr[0];
    descLen = *((TUInt16 *)&p->iDescBuf[2]);

    // walk through the data's buffers until it gets to the end (complete length)
    while (desc_ptr < (p->iDescBuf + descLen)) {
        // What type of block is this?
        switch (desc_ptr[1]) {
            case USB_DESCRIPTOR_TYPE_INTERFACE:
                // Is this the right type of interface for HID device?
                if (desc_ptr[5] == HID_CLASS /*&& desc_ptr[6] == HID_SUBCLASS
                    && desc_ptr[7] == HID_PROTOCOL*/) {
                    hid_int_found = 1;
                }
                break;
            case USB_DESCRIPTOR_TYPE_ENDPOINT:
                // Found an endpoint descriptor
                // desc_ptr[1] = USB_DESCRIPTOR_TYPE_ENDPOINT
                // desc_ptr[2] = end point address (&0x7F) and if in (|0x80) or out (|0x00)
                // desc_ptr[3] = Transfer type (2=bulk, 3=interrupt)
                // desc_ptr[4] = max packet size (num bytes)
                // desc_ptr[5] = transactions per microframe if HS, usually 0x00 for HID
                // desc_ptr[6] = Interval (ms)
                // Is it an interrupt endpoint?
                // (We're only configuring interrupt right now, ignore rest)
                if ((desc_ptr[3] & 0x03) == USB_ENDPOINT_TYPE_INTERRUPT) {
                    if (desc_ptr[2] & 0x80) {
                        // IN endpoint
                        p->iEndpointInterruptIn = desc_ptr[2] & 0x7F;
                        p->iInterval = desc_ptr[6];
                    } else {
                        // OUT endpoint
                        p->iEndpointInterruptOut = desc_ptr[2] & 0x7F;
                    }

                    // Configure the endpoint (either in or out) with proper packet size limit
                    error = (*p->iUSBHost)->ConfigureEndpoint(p->iUSBHost,
                        aAddress, desc_ptr[2], ReadLE16U(&desc_ptr[4]));
                    if (error) {
                        return error;
                    }
                }
                break;
            case USB_DESCRIPTOR_TYPE_HID:
                // Found the HID descriptor too
                // Copy over the descriptor (for use later)
                p->iHIDDescriptor = *((T_usbHIDDescriptor *)desc_ptr);
                break;
            default:
                // All others, we ignore
                break;
        }

        // Move to next descriptor
        desc_ptr += desc_ptr[0];
    }
    if (hid_int_found) {
        printf("HID device connected\n");
    } else {
        printf("Not an HID device\n");
        IRelease();
        return UEZ_ERROR_NOT_FOUND;
    }

    // Set to the first configuration (to activate)
    error = (*p->iUSBHost)->SetConfiguration(p->iUSBHost, aAddress, 1);
    if (error) {
        IRelease();
        return error;
    }

    // Now do an indefinite period Set Idle command (we only need changes)
    (*p->iUSBHost)->Control(p->iUSBHost, aAddress, USB_REQ_TYPE_CLASS
        | USB_RECIPIENT_INTERFACE | USB_HOST_TO_DEVICE, HID_SET_IDLE,
        HID_INDEFINITE_REPORT, 0, 0, recv, 5000);

    // Get report descriptor (clip to the size we can handle)
    descLen = p->iHIDDescriptor.iDescriptorLength;
    if (descLen > MAX_LENGTH_REPORT_DESCRIPTOR)
        descLen = MAX_LENGTH_REPORT_DESCRIPTOR;
    (*p->iUSBHost)->Control(p->iUSBHost, aAddress, USB_REQ_TYPE_STANDARD
        | USB_RECIPIENT_INTERFACE | USB_DEVICE_TO_HOST, GET_DESCRIPTOR,
        ((p->iHIDDescriptor.iDescriptorType)<<8), 0, descLen,
        p->iReportDesc, 5000);
    p->iIsConnected = ETrue;

    IRelease();
    return error;
}

T_uezError HID_Generic_Disconnected(void *aWorkspace, TUInt8 aDeviceAddress)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    IGrab();
    p->iIsConnected = EFalse;
    IRelease();
    return UEZ_ERROR_NONE;
}

T_uezError HID_Generic_Open(
    void *aWorkspace,
    TUInt16 aVenderID,
    TUInt16 aProductID)
{
    T_uezError error;
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;

    p->iNumOpen++;
    if ((p->iNumOpen == 1) && (!p->iIsRegistered)) {
        IGrab();
        error = (*p->iUSBHost)->USBHostDeviceDriverRegister(p->iUSBHost,
            (T_uezDeviceWorkspace *)aWorkspace, &G_hidAPI);
        IRelease();
        return error;
    } else {
        return UEZ_ERROR_NONE;
    }
}

T_uezError HID_Generic_Close(void *aWorkspace)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;

    IGrab();
    p->iNumOpen--;
    IRelease();

    return UEZ_ERROR_NONE;
}

T_uezError HID_Generic_Read(
    void *aWorkspace,
    TUInt8 *aData,
    TUInt32 aLength,
    TUInt32 aTimeout)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    T_uezError error;

    if (p->iIsConnected) {
        IGrab();
        error = (*p->iUSBHost)->InterruptIn(p->iUSBHost, p->iAddress,
            p->iEndpointInterruptIn, aData, aLength, aTimeout);
        IRelease();
        return error;
    }
    return UEZ_ERROR_READ_WRITE_ERROR;
}

T_uezError HID_Generic_Write(
    void *aWorkspace,
    const TUInt8 *aData,
    TUInt32 aLength,
    TUInt32 *aNumWritten,
    TUInt32 aTimeout)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    T_uezError error;

    if (p->iIsConnected) {
        IGrab();
        error = (*p->iUSBHost)->ControlOut(
                p->iUSBHost,
                p->iAddress,
                USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_RECIPIENT_INTERFACE,
                HID_SET_REPORT,
                (0x00<<8)|0x00,
                0,
                aLength,
                (void *)aData,
                aTimeout);
        IRelease();
        return error;
    }
    return UEZ_ERROR_READ_WRITE_ERROR;

}

T_uezError HID_Generic_GetReportDescriptor(
    void *aWorkspace,
    TUInt8 *aData,
    TUInt32 aMaxLength)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    TUInt32 len;

    IGrab();
    len = aMaxLength;
    if (len > MAX_LENGTH_REPORT_DESCRIPTOR)
        len = MAX_LENGTH_REPORT_DESCRIPTOR;
    memcpy(aData, p->iReportDesc, len);
    IRelease();

    return UEZ_ERROR_NONE;
}

T_uezError HID_Generic_Configure(void *aWorkspace, const char *aUSBHostName)
{
    T_uezError error;
    T_uezDevice device;
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    
    // Link the USB Host to this mass storage device
    error = UEZDeviceTableFind(aUSBHostName, &device);
    if (error != UEZ_ERROR_NONE)
        return error;
    error = UEZDeviceTableGetWorkspace(device,
        (T_uezDeviceWorkspace **)&p->iUSBHost);
    if (error != UEZ_ERROR_NONE)
        return error;

    return error;
}

T_uezError HID_Generic_GetFeatureReport(
    void *aWorkspace,
    TUInt8 aReportID,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    T_uezError error;


    if (p->iIsConnected) {
        IGrab();
        error = (*p->iUSBHost)->Control(
                p->iUSBHost,
                p->iAddress,
                USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_RECIPIENT_INTERFACE,
                HID_GET_REPORT,
                (HID_TYPE_REPORT<<8)|aReportID,
                0,
                aMaxLength,
                aData,
                aTimeout);

        IRelease();
        return error;
    }
    return UEZ_ERROR_READ_WRITE_ERROR;
}

T_uezError HID_Generic_GetInputReport(
    void *aWorkspace,
    TUInt8 aReportID,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    T_uezError error;

    if (p->iIsConnected) {
        IGrab();
        error = (*p->iUSBHost)->Control(
                p->iUSBHost,
                p->iAddress,
                USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_RECIPIENT_INTERFACE,
                HID_GET_REPORT,
                (HID_TYPE_INPUT<<8)|aReportID,
                0,
                aMaxLength,
                aData,
                aTimeout);

        IRelease();
        return error;
    }
    return UEZ_ERROR_READ_WRITE_ERROR;
}

T_uezError HID_Generic_SetFeatureReport(
    void *aWorkspace,
    TUInt8 aReportID,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    T_uezError error;
    char buffer[64];
    TUInt32 len;

    if (p->iIsConnected) {
        len = aMaxLength+1;
        if (len > 64)
            len = 64;
        buffer[0] = aReportID;
        memcpy(buffer+1, aData, len-1);
        IGrab();
        error = (*p->iUSBHost)->ControlOut(
                p->iUSBHost,
                p->iAddress,
                USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_RECIPIENT_INTERFACE,
                HID_SET_REPORT,
                (HID_TYPE_REPORT<<8)|aReportID,
                0,
                len,
                buffer,
                aTimeout);

        IRelease();
        return error;
    }
    return UEZ_ERROR_READ_WRITE_ERROR;
}

T_uezError HID_Generic_SetOutputReport(
    void *aWorkspace,
    TUInt8 aReportID,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    T_uezError error;

    if (p->iIsConnected) {
        IGrab();
        error = (*p->iUSBHost)->ControlOut(
                p->iUSBHost,
                p->iAddress,
                USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_RECIPIENT_INTERFACE,
                HID_SET_REPORT,
                (HID_TYPE_OUTPUT<<8)|aReportID,
                0,
                aMaxLength,
                aData,
                aTimeout);

        IRelease();
        return error;
    }
    return UEZ_ERROR_READ_WRITE_ERROR;
}

T_uezError HID_Generic_GetString(
    void *aWorkspace,
    TUInt8 aIndex,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_Generic_HID_Workspace *p = (T_Generic_HID_Workspace *)aWorkspace;
    T_uezError error;


    if (p->iIsConnected) {
        IGrab();
        error = (*p->iUSBHost)->GetString(
                p->iUSBHost,
                p->iAddress,
                aIndex,
                aData,
                aMaxLength,
                aTimeout);

        IRelease();
        return error;
    }
    return UEZ_ERROR_READ_WRITE_ERROR;
}


/*---------------------------------------------------------------------------*
 * Routine:  HID_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a Human Input Device (HID) driver for doing data
 *      transfers over USB.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aHIDHALName -- Name of low level HID USB driver
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError HID_Generic_Create(const char *aName, const char *aHIDUSBName)
{
    T_uezDeviceWorkspace *p_HID;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&HID_Generic_Interface, 0, &p_HID);
    return HID_Generic_Configure(p_HID, aHIDUSBName);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_HID HID_Generic_Interface = { {
// Common device interface
    "HID:Generic",
    0x0100,
    HID_Generic_InitializeWorkspace,
    sizeof(T_Generic_HID_Workspace), },

// Functions
    HID_Generic_Open,
    HID_Generic_Close,
    HID_Generic_Read,
    HID_Generic_Write,
    HID_Generic_GetReportDescriptor,
    HID_Generic_GetFeatureReport,
    HID_Generic_SetFeatureReport,
    HID_Generic_GetString,
    HID_Generic_GetInputReport,
    HID_Generic_SetOutputReport,
};

/*-------------------------------------------------------------------------*
 * End of File:  HID_Generic.c
 *-------------------------------------------------------------------------*/
