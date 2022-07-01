/*-------------------------------------------------------------------------*
* File:  uEZDeviceTable.h
*--------------------------------------------------------------------------*
* Description:
*         Routines that manage a table of uEZ Devices
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZDeviceTable.h
 *  @brief 	uEZ Device Table routines
 *
 *	Routines that manage a table of uEZ Devices
 */
#ifndef _DEVICE_TABLE_H_
#define _DEVICE_TABLE_H_

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
#include <uEZDevice.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Register a device with the given interface and handle.  Creates
 *  a workspace for the device.
 *
 *	@param[in]	*aName					Unique identifier for device
 *	@param[in]	*aInterface			Interface to the device
 *	@param[in]	*aDeviceHandle	Pointer to recently created device's handle.
 *	@param[out]	**aWorkspace		Pointer to pointer to recently created workspace.
 *
 *	@return	T_uezError
 */
T_uezError UEZDeviceTableRegister(
                const char * const aName, 
                T_uezDeviceInterface *aInterface, 
                T_uezDevice *aDeviceHandle, 
                T_uezDeviceWorkspace **aWorkspace);
								
/**
 *	Retrieve a workspace from the given device handle
 *
 *	@param[in]	aDevice				Device with workspace to get
 *	@param[out]	**aWorkspace	Pointer to pointer to returned workspace.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZDeviceTableGetWorkspace(
                T_uezDevice aDevice, 
                T_uezDeviceWorkspace **aWorkspace);
								
/**
 *	Search device table for existing device and return device handle.
 *
 *	@param[in]	*aName		Pointer to device name to find.
 *  @param[out]	*aDevice	Pointer to device to return
 *
 *	@return	T_uezError
 */ 
T_uezError UEZDeviceTableFind(const char * const aName, T_uezDevice *aDevice);

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_TABLE_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZDevice.h
 *-------------------------------------------------------------------------*/
