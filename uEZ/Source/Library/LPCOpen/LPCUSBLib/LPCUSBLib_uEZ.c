/*-------------------------------------------------------------------------*
 * File:  LPCUSBLib_uEZ.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Interface between LPCUSBLib and uEZ.
 *
 * Implementation:
 *      LPCUSBLib used static linking with weak connections for most of their
 *      code.  We're going to with a more a dynamic solution and use
 *      callbacks so we can switch the device types on the fly.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include <Source/Library/LPCOpen/LPCUSBLib/Drivers/USB/USB.h>
#include "LPCUSBLib_uEZ.h"

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define IGrab()         UEZSemaphoreGrab(iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(iSem)

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
TUInt32 LPCUSBLIB_USBDeviceMonitor(T_uezTask aMyTask, void *aParameters);
TUInt32 LPCUSBLIB_USBHostMonitor(T_uezTask aMyTask, void *aParameters);
uint16_t NoGetDescriptor(uint8_t corenum,
                          const uint16_t wValue,
                          const uint8_t wIndex,
                          const void** const DescriptorAddress);


/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_uezSemaphore iSem = 0;
static TBool isInit = EFalse;
static T_LPCUSBLib_Device_Callbacks G_deviceCallbacks = {
        0, // Connected
        0, // Disconnected
        0, // ConfigurationChanged
        0, // ControlRequest
        0, // Suspend
        0, // WakeUp
        0, // Reset
        0, // Update
        NoGetDescriptor, // GetDescriptor
};
static T_LPCUSBLib_Host_Callbacks G_hostCallbacks[2] = {
        {
                0, // HostError
                0, // DeviceEnumerationComplete
                0, // DeviceEnumerationFailed
                0, // DeviceUnattached
                0, // DeviceAttached
                0, // Update
        },
        {
                0, // HostError
                0, // DeviceEnumerationComplete
                0, // DeviceEnumerationFailed
                0, // DeviceUnattached
                0, // DeviceAttached
                0, // Update
        }
};

// USB Device globals:
T_uezTask G_usbDevTask = 0;
static int32_t G_usbDevCorenum;
T_uezSemaphore G_usbDevWait = 0;

// USB Host Globals:
T_uezTask G_usbHostTask = 0;
T_uezSemaphore G_usbHostWait = 0;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
uint16_t NoGetDescriptor(uint8_t corenum,
                          const uint16_t wValue,
                          const uint8_t wIndex,
                          const void** const DescriptorAddress)
{
    return NO_DESCRIPTOR;
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
//    printf("USB Device CONNECTED");
    if (G_deviceCallbacks.Connected)
        G_deviceCallbacks.Connected();
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
//    printf("USB Device disconnected");
    if (G_deviceCallbacks.Disconnected)
        G_deviceCallbacks.Disconnected();
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
   //printf("USB Device Configuration Changed\n");
   if (G_deviceCallbacks.ConfigurationChanged)
       G_deviceCallbacks.ConfigurationChanged();
}

void EVENT_USB_Device_ControlRequest(void)
{
    if (G_deviceCallbacks.ControlRequest)
        G_deviceCallbacks.ControlRequest();

}

void EVENT_USB_Device_Suspend(void)
{
    if (G_deviceCallbacks.Suspend)
        G_deviceCallbacks.Suspend();
}

void EVENT_USB_Device_WakeUp(void)
{
    if (G_deviceCallbacks.WakeUp)
        G_deviceCallbacks.WakeUp();
}

void EVENT_USB_Device_Reset(void)
{
    if (G_deviceCallbacks.Reset)
        G_deviceCallbacks.Reset();
}

void EVENT_USB_Device_IRQActivity(void)
{
    _isr_LPCUSBLib_USBDeviceMonitor_ActivityDetected();
}

uint16_t CALLBACK_USB_GetDescriptor(uint8_t corenum,
                          const uint16_t wValue,
                          const uint8_t wIndex,
                          const void** const DescriptorAddress)
{
    return G_deviceCallbacks.GetDescriptor(corenum, wValue, wIndex, DescriptorAddress);
}

T_uezError UEZ_LPCUSBLib_Device_Require(int32_t aUnitAddress, T_LPCUSBLib_Device_Callbacks *aCallbacks, TUInt8 aForceFullspeed)
{
    T_uezError error;
    extern void LPCUSBLib_USB0_IRQHandler(void);
    extern void LPCUSBLib_USB1_IRQHandler(void);
    extern void Chip_USB0_Init(void);
    extern void Chip_USB1_Init(void);
    extern void HAL_USBForceFullSpeed(uint8_t corenum, uint32_t onoff);

    error = UEZ_LPCUSBLib_Require();

    if (aUnitAddress == 0) {
        // Ensure the interrupt is registers for USB0
#ifdef CORE_M4
        if (!InterruptIsRegistered(USB0_IRQn))
            InterruptRegister(USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB0");
#endif
#ifdef CORE_M0
        if (!InterruptIsRegistered(M0_USB0_IRQn))
            InterruptRegister(M0_USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB0");
#endif
#ifdef CORE_M0SUB
        if (!InterruptIsRegistered(M0S_USB0_IRQn))
            InterruptRegister(M0S_USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB0");
#endif
    } else {
        // Ensure the interrupt is registers for USB1
#ifdef CORE_M4
        if (!InterruptIsRegistered(USB1_IRQn))
            InterruptRegister(USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB1");
#endif
#ifdef CORE_M0
        if (!InterruptIsRegistered(M0_USB1_IRQn))
            InterruptRegister(M0_USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB1");
#endif
#ifdef CORE_M0SUB
        if (!InterruptIsRegistered(M0S_USB1_IRQn))
            InterruptRegister(M0S_USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB1");
#endif
    }

    // If we have callbacks, register them now
    if (aCallbacks) {
        // Copy over the callbacks
        G_deviceCallbacks = *aCallbacks;
    }

    G_usbDevCorenum = aUnitAddress;
    if (aUnitAddress == 0) {
        Chip_USB0_Init();
        USB_Init(0, USB_MODE_Device);
        
        // Other speed modes don't work on our HW.
        HAL_USBForceFullSpeed(aUnitAddress, 1); // Force the full speed
    } else {
        Chip_USB1_Init();
        USB_Init(1, USB_MODE_Device);

        // For now, force to Full Speed until we figure out how to do high speed
        if (aForceFullspeed == 1) {
            HAL_USBForceFullSpeed(aUnitAddress, 1);
        } else {
            HAL_USBForceFullSpeed(aUnitAddress, 0);
        }
    }

    if (G_usbDevTask == 0) {
        // Now create a task that constantly monitor the device
        error = UEZTaskCreate(
                    LPCUSBLIB_USBDeviceMonitor,
                    "USBDevice",
                    1024,
                    (void *)aUnitAddress,
                    UEZ_PRIORITY_HIGH,
                    &G_usbDevTask);
    }

    return error;
}

TUInt32 LPCUSBLIB_USBDeviceMonitor(T_uezTask aMyTask, void *aParameters)
{
    int32_t activity;
//static int32_t count = 0;
    if (G_usbDevWait == 0) {
        UEZSemaphoreCreateCounting(&G_usbDevWait, 1, 1);
    }
    while (1) {
        UEZ_LPCUSBLib_USBTask(G_usbDevCorenum, USB_MODE_Device);
        activity = 0;
        if (G_deviceCallbacks.Update)
            activity = G_deviceCallbacks.Update(G_usbDevCorenum);

        if (activity) {
            // Don't delay, go again
        } else {
            // Check if something has happend or wait up to 1000 ms before polling
            UEZSemaphoreGrab(G_usbDevWait, 1);
        }
//        count++;
//        if (count >= 100) {
//            UEZTaskDelay(1);
//            count = 0;
//        }
    }
//    return 0;
}

void _isr_LPCUSBLib_USBDeviceMonitor_ActivityDetected(void)
{
    if (G_usbDevWait)
        _isr_UEZSemaphoreRelease(G_usbDevWait);
}

void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode)
{
    if (G_hostCallbacks[corenum].HostError)
        G_hostCallbacks[corenum].HostError(corenum, ErrorCode);
}

void EVENT_USB_Host_DeviceAttached(const uint8_t corenum)
{
    if (G_hostCallbacks[corenum].DeviceAttached)
        G_hostCallbacks[corenum].DeviceAttached(corenum);
}

void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum)
{
    if (G_hostCallbacks[corenum].DeviceUnattached)
        G_hostCallbacks[corenum].DeviceUnattached(corenum);
}

void EVENT_USB_Host_DeviceEnumerationFailed(
        const uint8_t corenum,
        const uint8_t ErrorCode,
        const uint8_t SubErrorCode)
{
    if (G_hostCallbacks[corenum].DeviceEnumerationFailed)
        G_hostCallbacks[corenum].DeviceEnumerationFailed(corenum, ErrorCode, SubErrorCode);

}

void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
    if (G_hostCallbacks[corenum].DeviceEnumerationComplete)
        G_hostCallbacks[corenum].DeviceEnumerationComplete(corenum);
}

TUInt32 LPCUSBLIB_USBHostMonitor(T_uezTask aMyTask, void *aParameters)
{
    int32_t activity;
    if (G_usbHostWait == 0) {
        UEZSemaphoreCreateCounting(&G_usbHostWait, 1, 1);
    }
    UEZTaskDelay(1000);
    while (1) {
        activity = 0;
        // Check both USB0 and USB1
        for (int32_t i=0; i<2; i++) {
            if (G_hostCallbacks[i].Update) {
                UEZ_LPCUSBLib_USBTask(i, USB_MODE_Host);
                if (G_hostCallbacks[i].Update)
                    activity |= G_hostCallbacks[i].Update(i);
            }
        }

        if (activity) {
            // Don't delay, go again
        } else {
            // Check if something has happened or wait up to 1 ms before polling
            UEZSemaphoreGrab(G_usbHostWait, 1);
        }
    }
//    return 0;
}

T_uezError UEZ_LPCUSBLib_Host_Require(int32_t aUnitAddress, T_LPCUSBLib_Host_Callbacks *aCallbacks, TUInt8 aForceFullspeed)
{
    T_uezError error;
    extern void LPCUSBLib_USB0_IRQHandler(void);
    extern void LPCUSBLib_USB1_IRQHandler(void);
    extern void Chip_USB0_Init(void);
    extern void Chip_USB1_Init(void);
    extern void HAL_USBForceFullSpeed(uint8_t corenum, uint32_t onoff);

    error = UEZ_LPCUSBLib_Require();

    if (aUnitAddress == 0) {
#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1) // Allow USB1 only off board usb
#else
        // Ensure the interrupt is registers for USB0
#ifdef CORE_M4
        if (!InterruptIsRegistered(USB0_IRQn))
            InterruptRegister(USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB0");
#endif
#ifdef CORE_M0
        if (!InterruptIsRegistered(M0_USB0_IRQn))
            InterruptRegister(M0_USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB0");
#endif
#ifdef CORE_M0SUB
        if (!InterruptIsRegistered(M0S_USB0_IRQn))
            InterruptRegister(M0S_USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB0");
#endif
#endif
    } else {
//#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1) // Allow USB0 only onboard usb
//#else
        // Ensure the interrupt is registers for USB1
#ifdef CORE_M4
        if (!InterruptIsRegistered(USB1_IRQn))
            InterruptRegister(USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB1");
#endif
#ifdef CORE_M0
        if (!InterruptIsRegistered(M0_USB1_IRQn))
            InterruptRegister(M0_USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB1");
#endif
#ifdef CORE_M0SUB
        if (!InterruptIsRegistered(M0S_USB1_IRQn))
            InterruptRegister(M0S_USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
                INTERRUPT_PRIORITY_NORMAL, "USB1");
#endif
//#endif
    }

    // If we have callbacks, register them now
    if (aCallbacks) {
        // Copy over the callbacks
        G_hostCallbacks[aUnitAddress] = *aCallbacks;
    }

    // Note that we probably call this function multiple times when it should be called only once so we shouldn't shut off clocks here.
    if (aUnitAddress == 0) {
#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1) // Allow USB1 only off board USB
#else
        //USB_Disable(0, USB_MODE_Host);
        Chip_USB0_Init();
        USB_Init(0, USB_MODE_Host);
#endif
    } else {
//#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1)  // Allow USB0 only on board USB
//#else
        //USB_Disable(1, USB_MODE_Host);
        Chip_USB1_Init();
        USB_Init(1, USB_MODE_Host);
//#endif
    }

#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1)   // Allow USB1 only off board USB
    HAL_USBForceFullSpeed(aUnitAddress, 1); // Force the full speed USB1 port
#else
    
    if (aUnitAddress == 0) { // Other speed modes don't work on our HW.
       HAL_USBForceFullSpeed(aUnitAddress, 1); // Force the full speed
    }

    if (aUnitAddress == 1) { // This port only goes to one spot. With ULPI HW could do high speed mode someday. (maybe)
      // Force the full speed?
      if (aForceFullspeed == 1) {
          HAL_USBForceFullSpeed(aUnitAddress, 1);
      } else {
          HAL_USBForceFullSpeed(aUnitAddress, 0);
      }
    }
#endif

//#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1)   // Allow USB0 only onboard USB
    //HAL_USBForceFullSpeed(aUnitAddress, 0); // Force the full speed USB0 port
//#endif

    // Start up the host monitoring task
    if (G_usbHostTask == 0) {
        // Now create a task that constantly monitors the device
        error = UEZTaskCreate(
                    LPCUSBLIB_USBHostMonitor,
                    "USBHost",
                    1024,
                    (void *)0,
                    UEZ_PRIORITY_HIGH,
                    &G_usbHostTask);
    }

    return error;
}

T_uezError UEZ_LPCUSBLib_Require(void)
{
    if (isInit)
		return UEZ_ERROR_NONE;
    T_uezError error = UEZSemaphoreCreateBinary(&iSem);
    if (error)
    	return error;

    // Successfully added
    isInit = ETrue;

    return error;
}

void UEZ_LPCUSBLib_USBTask(uint8_t corenum, uint8_t mode)
{
	// Semaphore activities to avoid conflicts
	IGrab();
	USB_USBTask(corenum, mode);
	IRelease();
}


/*-------------------------------------------------------------------------*
 * File:  LPCUSBLib_uEZ.c
 *-------------------------------------------------------------------------*/
