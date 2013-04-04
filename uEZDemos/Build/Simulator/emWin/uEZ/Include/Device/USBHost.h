/*-------------------------------------------------------------------------*
 * File:  DEVICE/USBHost.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USBHost DEVICE interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_USBHost_H_
#define _DEVICE_USBHost_H_

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
/**
 *  @file   /Include/Device/USBHost.h
 *  @brief  uEZ USB Host Device Interface
 *
 *  The uEZ USB Host Device Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZDevice.h>
#include <Types/USBHost.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define  USB_DESCRIPTOR_TYPE_DEVICE                     1
#define  USB_DESCRIPTOR_TYPE_CONFIGURATION              2
#define  USB_DESCRIPTOR_TYPE_INTERFACE                  4
#define  USB_DESCRIPTOR_TYPE_ENDPOINT                   5
#define  USB_DESCRIPTOR_TYPE_REPORT_DESCRIPTOR          0x2200

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezError (*ConnectedCheckMatch)(
        void *aDeviceDriverWorkspace,
        TUInt8 aDeviceAddress);
    T_uezError (*Disconnected)(
        void *aDeviceDriverWorkspace,
        TUInt8 aDeviceAddress);
} T_USBHostDeviceDriverAPI;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iInterface;

    // Functions
    T_uezError (*USBHostDeviceDriverRegister)(
        void *aWorkspace,
        T_uezDeviceWorkspace *iDeviceDriverWorkspace,
        const T_USBHostDeviceDriverAPI *aAPI);
    T_uezError (*USBHostDeviceDriverUnregister)(
        void *aWorkspace,
        T_uezDeviceWorkspace *iDeviceDriverWorkspace);
    T_uezError (*Init)(void *aWorkspace);
    void (*ResetPort)(void *aWorkspace);
    void * (*AllocBuffer)(void *aWorkspace, TUInt32 aSize);
    T_usbHostDeviceState (*GetState)(void *aWorkspace);

    T_uezError (*SetControlPacketSize)(
            void *aWorkspace, 
            TUInt8 aDeviceAddress,
            TUInt16 aPacketSize);
    T_uezError (*SetAddress)(
            void *aWorkspace, 
            TUInt8 aAddress);
    T_uezError (*ConfigureEndpoint)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpointAndInOut,   /**< bit 0x80 is set if IN */
            TUInt16 aMaxPacketSize);
    T_uezError (*SetConfiguration)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aConfiguration);

    T_uezError (*Control)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aBMRequestType,
            TUInt8 aRequest,
            TUInt16 aValue,
            TUInt16 aIndex,
            TUInt16 aLength,
            void *aBuffer,
            TUInt32 aTimeout);
    T_uezError (*GetDescriptor)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aType,
            TUInt8 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*GetString)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*BulkOut)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*BulkIn)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*InterruptOut)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            const TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*InterruptIn)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
} DEVICE_USBHost ;

#endif // _DEVICE_USBHost_H_
/*-------------------------------------------------------------------------*
 * End of File:  DEVICE/USBHost.h
 *-------------------------------------------------------------------------*/
