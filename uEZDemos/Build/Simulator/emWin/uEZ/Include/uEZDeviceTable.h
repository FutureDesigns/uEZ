/**
 *    @file     uEZDeviceTable.h
 *  @brief     uEZ Device Table routines
 *
 *    Routines that manage a table of uEZ Devices
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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
 #ifndef _DEVICE_TABLE_H_
#define _DEVICE_TABLE_H_

#include <uEZDevice.h>

/**
 *    Register a device with the given interface and handle.  Creates
 *  a workspace for the device.
 *
 *    @param[in]    *aName                    Unique identifier for device
 *    @param[in]    *aInterface            Interface to the device
 *    @param[in]    *aDeviceHandle    Pointer to recently created device's handle.
 *    @param[out]    **aWorkspace        Pointer to pointer to recently created workspace.
 *
 *    @return    T_uezError
 */
T_uezError UEZDeviceTableRegister(
                const char * const aName, 
                T_uezDeviceInterface *aInterface, 
                T_uezDevice *aDeviceHandle, 
                T_uezDeviceWorkspace **aWorkspace);
                                
/**
 *    Retrieve a workspace from the given device handle
 *
 *    @param[in]    aDevice                Device with workspace to get
 *    @param[out]    **aWorkspace    Pointer to pointer to returned workspace.
 *
 *    @return    T_uezError
 */ 
T_uezError UEZDeviceTableGetWorkspace(
                T_uezDevice aDevice, 
                T_uezDeviceWorkspace **aWorkspace);
                                
/**
 *    Search device table for existing device and return device handle.
 *
 *    @param[in]    *aName        Pointer to device name to find.
 *  @param[out]    *aDevice    Pointer to device to return
 *
 *    @return    T_uezError
 */ 
T_uezError UEZDeviceTableFind(const char * const aName, T_uezDevice *aDevice);

#endif // _DEVICE_TABLE_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZDevice.h
 *-------------------------------------------------------------------------*/
