/*-------------------------------------------------------------------------*
 * File:  USBHost.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USBHost HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_USBHost_H_
#define _HAL_USBHost_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   /Include/HAL/USBHost.h
 *  @brief  uEZ USB Host HAL Interface
 *
 *  The uEZ USB Host HAL Interface
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
#include <HAL/HAL.h>
#include <Types/USBHost.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct hcEd {                        /**< ----------- HostController EndPoint Descriptor ------------- */
    volatile  TUInt32  Control;              /**< Endpoint descriptor control */
    volatile  TUInt32  TailTd;               /**< Physical address of tail in Transfer descriptor list */
    volatile  TUInt32  HeadTd;               /**< Physcial address of head in Transfer descriptor list */
    volatile  TUInt32  Next;                 /**< Physical address of next Endpoint descriptor */
} HCED;

typedef struct hcTd {                        /**< ------------ HostController Transfer Descriptor ------------ */
    volatile  TUInt32  Control;              /**< Transfer descriptor control */
    volatile  TUInt32  CurrBufPtr;           /**< Physical address of current buffer pointer */
    volatile  TUInt32  Next;                 /**< Physical pointer to next Transfer Descriptor */
    volatile  TUInt32  BufEnd;               /**< Physical address of end of buffer */
} HCTD;

typedef struct hcca {                        /**< ----------- Host Controller Communication Area ------------ */
    volatile  TUInt32  IntTable[32];         /**< Interrupt Table */
    volatile  TUInt32  FrameNumber;          /**< Frame Number */
    volatile  TUInt32  DoneHead;             /**< Done Head */
    volatile  TUInt8  Reserved[116];         /**< Reserved for future use */
    volatile  TUInt8  Unknown[4];            /**< Unused */
} HCCA;

typedef struct {
    void (*Connected)(void *aCallbackWorkspace);
    void (*Disconnected)(void *aCallbackWorkspace);
    T_uezError (*WaitForIRQEvent)(void *aCallbackWorkspace, TUInt32 aTimeout);
    void (*FlagIRQEvent)(void *aCallbackWorkspace);
} T_USBHostCallbacks;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define  HOST_GET_DESCRIPTOR(workspace, descType, descIndex, length) \
            (*((HAL_USBHost **)workspace)->Host_CtrlRecv)( \
                workspace, \
                USB_DEVICE_TO_HOST | USB_RECIPIENT_DEVICE, \
                GET_DESCRIPTOR,    \
                (descType << 8)|(descIndex), 0, length)

#define  HOST_SET_ADDRESS(workspace, new_addr) \
            (*((HAL_USBHost **)workspace)->Host_CtrlSend)(\
                workspace, \
                USB_HOST_TO_DEVICE | USB_RECIPIENT_DEVICE, SET_ADDRESS, \
                new_addr, 0, 0)

#define  USBH_SET_CONFIGURATION(workspace, configNum)                                           \
            (*((HAL_USBHost **)workspace)->Host_CtrlSend)( \
                workspace,  \
                USB_HOST_TO_DEVICE | USB_RECIPIENT_DEVICE, \
                SET_CONFIGURATION, \
                configNum, 0, 0)

#define  USBH_SET_INTERFACE(workspace, ifNum, altNum)                                           \
            (*((HAL_USBHost **)workspace)->Host_CtrlSend)( \
                workspace, \
                USB_HOST_TO_DEVICE | USB_RECIPIENT_INTERFACE, \
                SET_INTERFACE,  \
                altNum, ifNum, 0)

typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*SetCallbacks)(
        void *aWorkspace, 
        void *aCallbackWorkspace,
        const T_USBHostCallbacks *aCallbackAPI);
    T_uezError (*Init)(void *aWorkspace);
    void (*ResetPort)(void *aWorkspace);
    void (*ResetBuffers)(void *aWorkspace);
    void * (*AllocBuffer)(void *aWorkspace, TUInt32 aSize);
    T_uezError (*ProcessTD)(
        void *aWorkspace,
        volatile HCED *aEndDesc,
        TUInt32 aToken,
        volatile TUInt8 *buffer,
        TUInt32 buffer_len,
        TUInt32 aTimeout);
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
} HAL_USBHost ;

#ifdef __cplusplus
}
#endif

#endif // _HAL_USBHost_H_
/*-------------------------------------------------------------------------*
 * End of File:  HAL/USBHost.h
 *-------------------------------------------------------------------------*/
