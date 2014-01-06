/**
 *	@file 	uEZDeviceTable.h
 *  @brief 	uEZ Device Table routines
 *
 *	Routines that manage a table of uEZ Devices
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


T_uezError UEZDeviceTableRegister(
                const char * const aName, 
                T_uezDeviceInterface *aInterface, 
                T_uezDevice *aDeviceHandle, 
                T_uezDeviceWorkspace **aWorkspace);
T_uezError UEZDeviceTableGetWorkspace(
                T_uezDevice aDevice, 
                T_uezDeviceWorkspace **aWorkspace);
T_uezError UEZDeviceTableFind(
                const char * const aName,
                T_uezDevice *aDevice);
T_uezError UEZDeviceTableRegisterAlias(
                const char * const aExistingName,
                const char * const aAliasName);
TBool UEZDeviceTableIsRegistered(const char * const aName);

#endif // _DEVICE_TABLE_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZDevice.h
 *-------------------------------------------------------------------------*/
