/*-------------------------------------------------------------------------*
 * File:  Generic_USBHost.c
 *-------------------------------------------------------------------------*
 * Description:
 *      DEVICE implementation of the USBHost Interface.  Allows devices
 *      to register as USBHost devices to get called when a device
 *      is connected or disconnected.
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

/*---------------------------------------------------------------------------*
 * Include Files:
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "Generic_USBHost.h"

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef UEZ_GENERIC_USBHOST_REPORT_CONNECT_DISCONNECT
#define UEZ_GENERIC_USBHOST_REPORT_CONNECT_DISCONNECT 0
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define USBHOST_SIZE_EVENT_QUEUE        5

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)
#define IGrabList()     UEZSemaphoreGrab(p->iListSem, UEZ_TIMEOUT_INFINITE)
#define IReleaseList()  UEZSemaphoreRelease(p->iListSem)
//#define DebugConMsg(x)  printf(x)
#define DebugConMsg(x)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct _T_usbhostRegistration {
    struct _T_usbhostRegistration *iNext;
    T_uezDeviceWorkspace *iDeviceDriver;
    const T_USBHostDeviceDriverAPI *iAPI;
    TUInt8 iAddress;
    TBool iIsConnected;
} T_usbhostRegistration;

typedef enum {
    USBHOST_EVENT_TYPE_CONNECT,
    USBHOST_EVENT_TYPE_DISCONNECT,
    USBHOST_EVENT_TYPE_UNKNOWN
} T_usbhostEventType;

typedef struct {
    T_usbhostEventType iType;
} T_usbhostEvent;

typedef struct {
    // Required device id
    const DEVICE_USBHost *iDevice;

    // Semaphore to ensure no overlapping commands
    HAL_USBHost **iUSBHost;
    T_uezSemaphore iSem;
    T_uezSemaphore iListSem;

    // Registrations of device drivers
    T_usbhostRegistration *iRegistrations;

    // Monitor Task to watch IRQ and process events
    T_uezTask iMonitorTask;

    // Queue to receive events
    T_uezSemaphore iStatusChangeSem;
    T_uezSemaphore iWaitForIRQEvent;

    //! Speed of this USBHost
    T_usbHostSpeed iSpeed;
} T_Generic_USBHost_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void IGenericUSBHostConnected(void *aWorkspace);
static void IGenericUSBHostDisconnected(void *aWorkspace);
static T_uezError IGenericUSBHostWaitForIRQEvent(void *aWorkspace, TUInt32 aTimeout);
static void IGenericUSBHostFlagIRQEvent(void *aWorkspace);
static T_uezError Generic_USBHost_Init(void *aWorkspace);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const T_USBHostCallbacks G_GenericUSBHostCallbacks = {
    IGenericUSBHostConnected,
    IGenericUSBHostDisconnected,
    IGenericUSBHostWaitForIRQEvent,
    IGenericUSBHostFlagIRQEvent,
};

/*---------------------------------------------------------------------------*
 * Routine:  Generic_USBHost_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 * Outputs:
 *      T_uezError                  -- Any error if failed
 *---------------------------------------------------------------------------*/
static T_uezError Generic_USBHost_InitializeWorkspace(void *aWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    p->iRegistrations = 0;

    error = UEZSemaphoreCreateBinary(&p->iStatusChangeSem);
    if (error)
        return error;
    UEZSemaphoreGrab(p->iStatusChangeSem, 0);

    error = UEZSemaphoreCreateBinary(&p->iWaitForIRQEvent);
    if (error)
        return error;
    UEZSemaphoreGrab(p->iWaitForIRQEvent, 0);

    error = UEZSemaphoreCreateBinary(&p->iListSem);
    if (error)
        return error;

    return UEZSemaphoreCreateBinary(&p->iSem);
}

static T_uezError Generic_USBHost_Register(
        void *aWorkspace,
        T_uezDeviceWorkspace *aDeviceDriverWorkspace,
        const T_USBHostDeviceDriverAPI *aAPI)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_usbhostRegistration *p_reg;

    // Allocate the memory for the registration
    p_reg = UEZMemAlloc(sizeof(T_usbhostRegistration));
    if (p_reg == 0)
        return UEZ_ERROR_OUT_OF_MEMORY;

    // Fill the structure with the information
    p_reg->iAPI = aAPI;
    p_reg->iDeviceDriver = aDeviceDriverWorkspace;

    // Add to the list of registrations (safely)
    IGrabList();
    p_reg->iNext = p->iRegistrations;
    p->iRegistrations = p_reg;
    IReleaseList();

    return UEZ_ERROR_NONE;
}

static T_uezError Generic_USBHost_Unregister(
        void *aWorkspace,
        T_uezDeviceWorkspace *aDeviceDriverWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_usbhostRegistration *p_reg;
    T_usbhostRegistration *p_prev = 0;

    IGrabList();
    // Walk the list look for a match
    p_reg = p->iRegistrations;
    while (p_reg) {
        // Is this the same device driver?
        if (p_reg->iDeviceDriver == aDeviceDriverWorkspace)
            break;

        // Not this one, go to the next
        p_prev = p_reg;
        p_reg = p_reg->iNext;
    }
    if (p_reg) {
        // Found a match, unhook it from the list
        if (p_prev) {
            p_prev->iNext = p_reg->iNext;
        } else {
            p->iRegistrations = p_reg->iNext;
        }

        // Free the registration memory
        p_reg->iNext = 0;
        UEZMemFree(p_reg);
    }
    IReleaseList();

    return UEZ_ERROR_NONE;
}

static void IGenericUSBHostConnected(void *aWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    if (p->iStatusChangeSem)
        _isr_UEZSemaphoreRelease(p->iStatusChangeSem);
}

static void IGenericUSBHostDisconnected(void *aWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    if (p->iStatusChangeSem)
        _isr_UEZSemaphoreRelease(p->iStatusChangeSem);
}

static T_uezError IGenericUSBHostWaitForIRQEvent(void *aWorkspace, TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NOT_READY;

    if (p->iWaitForIRQEvent)
        error = UEZSemaphoreGrab(p->iWaitForIRQEvent, aTimeout);

    return error;
}
static void IGenericUSBHostFlagIRQEvent(void *aWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    if (p->iWaitForIRQEvent)
        _isr_UEZSemaphoreRelease(p->iWaitForIRQEvent);
}

static TUInt32 Generic_USBHost_Monitor(T_uezTask aMyTask, void *aParameters)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aParameters;
    T_usbhostEvent event = {USBHOST_EVENT_TYPE_UNKNOWN};
    T_usbhostRegistration *p_reg;
    T_uezError error;
    TUInt8 retry;
    HAL_USBHost **p_usbHost = p->iUSBHost;
    TUInt8 *descBuf;
    TBool needReset;
    TBool isConnected = EFalse;
    const TUInt8 devAddress = 1;

    (*p_usbHost)->Init(p_usbHost);
    (*p_usbHost)->ResetPort(p_usbHost);

    // Wait for at least one registration (otherwise this monitor is really useless)
    // (Waiting also ensures the usually one device needed is already configured)
    // NOTE: This should be improved in the future so all registrations of device
    // drivers is done BEFORE we start connecting/disconnecting.  Reason: the
    // device may be already connected at power up (yielding a race condition)!
    while (1) {
        if (p->iRegistrations)
            break;

        // Check every half second
        UEZTaskDelay(500);
    }
    while (1) {
        // Wait for the event to be received (or kill thread if an error)
        if (UEZSemaphoreGrab(
                p->iStatusChangeSem,
                UEZ_TIMEOUT_INFINITE) != UEZ_ERROR_NONE)
            break;
        
        // Debounce and confirm we are truly connected or disconnected
        retry = 5;

        while (retry--) {
            UEZTaskDelay(1);
            // Determine the state we are in
            if ((*p_usbHost)->GetState(p_usbHost) == USB_HOST_STATE_CONNECTED) {
                event.iType = USBHOST_EVENT_TYPE_CONNECT;
                DebugConMsg("O");
            } else {
                event.iType = USBHOST_EVENT_TYPE_DISCONNECT;
                DebugConMsg("x");
            }
        }

        // Handle the type of event
        if ((event.iType == USBHOST_EVENT_TYPE_CONNECT) && (isConnected == EFalse)) {
            // Initialize it
//                (*p_usbHost)->Init(p_usbHost);
                (*p_usbHost)->ResetPort(p_usbHost);
            // Waiting for connect, or abort
            retry = 3;

            while (retry--) {
                UEZTaskDelay(1);
                // Determine the state we are in
                if ((*p_usbHost)->GetState(p_usbHost) == USB_HOST_STATE_CONNECTED) {
                    event.iType = USBHOST_EVENT_TYPE_CONNECT;
                    DebugConMsg("O");
                } else {
                    event.iType = USBHOST_EVENT_TYPE_DISCONNECT;
                    DebugConMsg("x");
                }
            }
            if (event.iType == USBHOST_EVENT_TYPE_DISCONNECT) {
                // Failed?  Connect must be interrupted
                DebugConMsg("_F0_");
                (*p_usbHost)->Init(p_usbHost);
                (*p_usbHost)->ResetPort(p_usbHost);
                continue;
            }

            (*p_usbHost)->ResetBuffers(p_usbHost);
            descBuf = (*p_usbHost)->AllocBuffer(p_usbHost, 128);

            // What speed are we now?
            p->iSpeed = (*p_usbHost)->GetRootPortDetectedSpeed(p_usbHost);

            // Configure the root port to match
            (*p_usbHost)->SetRootPortSpeed(p_usbHost, p->iSpeed);

            // Reset the port (and the allocated buffers)
//                (*p_usbHost)->ResetPort(p_usbHost);

            // Determine the control packet size
            error = (*p_usbHost)->GetDescriptor(
                        p_usbHost,
                        0,
                        USB_DESCRIPTOR_TYPE_DEVICE,
                        0,
                        descBuf,
                        8,
                        5000);
            if (error) {
                // Failed?  Connect must be interrupted
                DebugConMsg("_F1_");
                (*p_usbHost)->Init(p_usbHost);
                (*p_usbHost)->ResetPort(p_usbHost);
                continue;
            }

            // Set the control packet size
            error = (*p_usbHost)->SetControlPacketSize(
                        p_usbHost,
                        0,
                        descBuf[7]);
            if (error) {
                // Failed?  Connect must be interrupted
                DebugConMsg("_F2_");
                (*p_usbHost)->Init(p_usbHost);
                (*p_usbHost)->ResetPort(p_usbHost);
                continue;
            }

            // Force the device address to 1
            (*p_usbHost)->SetAddress(p_usbHost, devAddress);

            // Walk through all the registrations and use the first match
            IGrabList();
            p_reg = p->iRegistrations;
            while (p_reg) {
                // Reset buffers again
                (*p_usbHost)->ResetBuffers(p_usbHost);
                // See if we can match
                error = p_reg->iAPI->ConnectedCheckMatch(
                            p_reg->iDeviceDriver,
                            devAddress);
                // No more matches, ok, done
                if (error == UEZ_ERROR_NONE) {
                    p_reg->iIsConnected = ETrue;
                    p_reg->iAddress = devAddress;
                    break;
                }
                p_reg = p_reg->iNext;
            }
            IReleaseList();
            if (error == UEZ_ERROR_NONE) {
#if (UEZ_GENERIC_USBHOST_REPORT_CONNECT_DISCONNECT == 1)
                printf("USBHost: Device connected ... %s found\n", p_reg->iDeviceDriver->iInterface->iName);
#endif                
                isConnected = ETrue;
            } else {
                DebugConMsg("_F3_");
                // Failed?  Connect must be interrupted
                (*p_usbHost)->Init(p_usbHost);
                (*p_usbHost)->ResetPort(p_usbHost);
            }
        } else if ((event.iType == USBHOST_EVENT_TYPE_DISCONNECT) && (isConnected == ETrue)) {
            // Walk through all the registrations and make sure all are disconnected
            IGrabList();
            p_reg = p->iRegistrations;
            needReset = EFalse;
            while (p_reg) {
                // See if we can match
                if (p_reg->iIsConnected == ETrue) {
                    p_reg->iAPI->Disconnected(
                                p_reg->iDeviceDriver,
                                p_reg->iAddress);
                    // Mark this usage as disconnected
                    p_reg->iIsConnected = EFalse;
                    p_reg->iAddress = 0;
                    needReset = ETrue;
                    break;
                }
                p_reg = p_reg->iNext;
            }
            IReleaseList();

            if (needReset) {
                (*p_usbHost)->Init(p_usbHost);
                (*p_usbHost)->ResetPort(p_usbHost);
            }
#if (UEZ_GENERIC_USBHOST_REPORT_CONNECT_DISCONNECT == 1)
            printf("USBHost: %s disconnected\n", p_reg->iDeviceDriver->iInterface->iName);
#endif
            isConnected = EFalse;
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  Generic_USBHost_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link the USB Host Device driver to a USB Host Driver
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 * Outputs:
 *      T_uezError                  -- Any error if failed
 *---------------------------------------------------------------------------*/
T_uezError Generic_USBHost_Configure(
                void *aWorkspace,
                HAL_USBHost **aUSBHost)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    p->iUSBHost = aUSBHost;
    // Setup callbacks to receive interrupts
    (*p->iUSBHost)->SetCallbacks(
        p->iUSBHost,
        aWorkspace,
        &G_GenericUSBHostCallbacks);
    error = UEZTaskCreate(
                Generic_USBHost_Monitor,
                "USBHost",
                UEZ_TASK_STACK_BYTES( 2048 ),
                aWorkspace,
                UEZ_PRIORITY_NORMAL,
                &p->iMonitorTask);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Generic_USBHost_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initializes the LPC247x host controller with buffers
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *---------------------------------------------------------------------------*/
static T_uezError Generic_USBHost_Init(void *aWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->Init(p->iUSBHost);
    IRelease();

    return error;
}

static void Generic_USBHost_ResetPort(void *aWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;

    IGrab();
    (*p->iUSBHost)->ResetPort(p->iUSBHost);
    IRelease();
}

static void *Generic_USBHost_AllocBuffer(void *aWorkspace, TUInt32 aSize)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    void *ptr;

    IGrab();
    ptr = (*p->iUSBHost)->AllocBuffer(p->iUSBHost, aSize);
    IRelease();

    return ptr;
}

static T_usbHostDeviceState Generic_USBHost_GetState(void *aWorkspace)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_usbHostDeviceState state;

    IGrab();
    state = (*p->iUSBHost)->GetState(p->iUSBHost);
    IRelease();

    return state;
}

static T_uezError Generic_USBHost_SetControlPacketSize(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt16 aPacketSize)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->SetControlPacketSize(p->iUSBHost, aDeviceAddress, aPacketSize);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_SetAddress(
            void *aWorkspace,
            TUInt8 aAddress)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->SetAddress(p->iUSBHost, aAddress);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_ConfigureEndpoint(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpointAndInOut,   // bit 0x80 is set if IN
            TUInt16 aMaxPacketSize)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->ConfigureEndpoint(
            p->iUSBHost,
            aDeviceAddress,
            aEndpointAndInOut,   // bit 0x80 is set if IN
            aMaxPacketSize);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_SetConfiguration(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aConfiguration)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->SetConfiguration(
            p->iUSBHost,
            aDeviceAddress,
            aConfiguration);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_Control(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aBMRequestType,
            TUInt8 aRequest,
            TUInt16 aValue,
            TUInt16 aIndex,
            TUInt16 aLength,
            void *aBuffer,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->Control(
            p->iUSBHost,
            aDeviceAddress,
            aBMRequestType,
            aRequest,
            aValue,
            aIndex,
            aLength,
            aBuffer,
            aTimeout);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_GetDescriptor(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aType,
            TUInt8 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->GetDescriptor(
            p->iUSBHost,
            aDeviceAddress,
            aType,
            aIndex,
            aBuffer,
            aSize,
            aTimeout);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_GetString(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->GetString(
            p->iUSBHost,
            aDeviceAddress,
            aIndex,
            aBuffer,
            aSize,
            aTimeout);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_BulkOut(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->BulkOut(
            p->iUSBHost,
            aDeviceAddress,
            aEndpoint,
            aBuffer,
            aSize,
            aTimeout);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_BulkIn(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->BulkIn(
            p->iUSBHost,
            aDeviceAddress,
            aEndpoint,
            aBuffer,
            aSize,
            aTimeout);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_InterruptOut(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            const TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->InterruptOut(
            p->iUSBHost,
            aDeviceAddress,
            aEndpoint,
            aBuffer,
            aSize,
            aTimeout);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_InterruptIn(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->InterruptIn(
            p->iUSBHost,
            aDeviceAddress,
            aEndpoint,
            aBuffer,
            aSize,
            aTimeout);
    IRelease();

    return error;
}

static T_uezError Generic_USBHost_ControlOut(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aBMRequestType,
            TUInt8 aRequest,
            TUInt16 aValue,
            TUInt16 aIndex,
            TUInt16 aLength,
            void *aBuffer,
            TUInt32 aTimeout)
{
    T_Generic_USBHost_Workspace *p = (T_Generic_USBHost_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iUSBHost)->ControlOut(
            p->iUSBHost,
            aDeviceAddress,
            aBMRequestType,
            aRequest,
            aValue,
            aIndex,
            aLength,
            aBuffer,
            aTimeout);
    IRelease();

    return error;
}

void USBHost_Generic_Create(const char *aName, const char *aHALUSBHostName)
{
    T_halWorkspace *p_halUSBHost;
    T_uezDeviceWorkspace *p_usbhost;

    // Register the USB Host device driver
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&G_Generic_USBHost_Interface,
            0,
            (T_uezDeviceWorkspace **)&p_usbhost);
    // Link the Generic USB host to the HAL USB host
    HALInterfaceFind(aHALUSBHostName, (T_halWorkspace **)&p_halUSBHost);
    Generic_USBHost_Configure(p_usbhost, (HAL_USBHost **)p_halUSBHost);
}

const DEVICE_USBHost G_Generic_USBHost_Interface = {
    // T_deviceInterface iInterface
    {
        "Generic:USBHost",
        0x0100,
        Generic_USBHost_InitializeWorkspace,
        sizeof(T_Generic_USBHost_Workspace),
    },

    Generic_USBHost_Register,
    Generic_USBHost_Unregister,
    Generic_USBHost_Init,
    Generic_USBHost_ResetPort,
    Generic_USBHost_AllocBuffer,
    Generic_USBHost_GetState,

    Generic_USBHost_SetControlPacketSize,
    Generic_USBHost_SetAddress,
    Generic_USBHost_ConfigureEndpoint,
    Generic_USBHost_SetConfiguration,

    Generic_USBHost_Control,
    Generic_USBHost_GetDescriptor,
    Generic_USBHost_GetString,
    Generic_USBHost_BulkOut,
    Generic_USBHost_BulkIn,
    Generic_USBHost_InterruptOut,
    Generic_USBHost_InterruptIn,

    Generic_USBHost_ControlOut,
};


/*-------------------------------------------------------------------------*
 * End of File:  Generic_USBHost.c
 *-------------------------------------------------------------------------*/
