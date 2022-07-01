/*
 * LPCUSBLib_uEZ.h
 *
 *  Created on: May 1, 2016
 *      Author: lshields
 */

#ifndef LPCUSBLIB_UEZ_H_
#define LPCUSBLIB_UEZ_H_

T_uezError UEZ_LPCUSBLib_Require(void);
void UEZ_LPCUSBLib_USBTask(uint8_t corenum, uint8_t mode);
void _isr_LPCUSBLib_USBDeviceMonitor_ActivityDetected(void);

typedef struct {
        void (*Connected)(void);
        void (*Disconnected)(void);
        void (*ConfigurationChanged)(void);
        void (*ControlRequest)(void);
        void (*Suspend)(void);
        void (*WakeUp)(void);
        void (*Reset)(void);  // Timing critical!
        int32_t (*Update)(int32_t aUnitAddress); // Called by monitoring task
        uint16_t (*GetDescriptor)(uint8_t corenum,
                                  const uint16_t wValue,
                                  const uint8_t wIndex,
                                  const void** const DescriptorAddress);
} T_LPCUSBLib_Device_Callbacks;

typedef struct {
        void (*HostError)(const uint8_t corenum, const uint8_t ErrorCode);
        void (*DeviceEnumerationComplete)(const uint8_t corenum);
        void (*DeviceEnumerationFailed)(const uint8_t corenum,
                                                    const uint8_t ErrorCode,
                                                    const uint8_t SubErrorCode);
        void (*DeviceUnattached)(const uint8_t corenum);
        void (*DeviceAttached)(const uint8_t corenum);

        int32_t (*Update)(int32_t aUnitAddress); // Called by monitoring task
} T_LPCUSBLib_Host_Callbacks;

T_uezError UEZ_LPCUSBLib_Device_Require(int32_t aUnitAddress, T_LPCUSBLib_Device_Callbacks *aCallbacks, TUInt8 aForceFullSpeed);

T_uezError UEZ_LPCUSBLib_Host_Require(int32_t aUnitAddress, T_LPCUSBLib_Host_Callbacks *aCallbacks, TUInt8 aForceFullspeed);

#endif /* LPCUSBLIB_UEZ_H_ */
