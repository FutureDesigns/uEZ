/*-------------------------------------------------------------------------*
* File:  uEZDevice.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to lower level device drivers.
*-------------------------------------------------------------------------*/
/**
 *    @file     uEZDevice.h
 *  @brief     uEZ Device Interface
 *
 *    The uEZ interface which maps to lower level device drivers.
 */
#ifndef _UEZ_DEVICE_H_
#define _UEZ_DEVICE_H_

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
#include "uEZTypes.h"
#include "uEZErrors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *    @struct uEZDeviceInterface
 */
typedef struct {
    const char *iName;
    TUInt16 iVersion;   // Version is 0x103 for version 1.03
    T_uezError (*InitializeWorkspace)(void *aWorkspace);
    TUInt32 iWorkspaceSize;
} T_uezDeviceInterface;

/**
 *    @struct T_uezDeviceWorkspace
 */
typedef struct {
    T_uezDeviceInterface *iInterface;
} T_uezDeviceWorkspace;


/**
 *    Open up access to the Device bank.
 *
 *    @param [in]        *aDeviceName        Pointer to name of Device bank
 *    @param [out]    *aDevice    Pointer to device handle to be returned
 *
 *    @return    T_uezError
 */
T_uezError UEZDeviceOpen(
            const char *aDeviceName, 
            T_uezDevice *aDevice);

/**
 *    End access to the Device bank.
 *
 *    @param [in]    aDevice        Device handle of Device to close
 *
 *    @return    T_uezError
 */
T_uezError UEZDeviceClose(T_uezDevice aDevice);


#define DEVICE_CREATE_ONCE() \
    static TBool created = EFalse; \
    if (created) { return; }  \
        created=ETrue

#ifdef __cplusplus
}
#endif

#endif // _UEZ_DEVICE_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZDevice.h
 *-------------------------------------------------------------------------*/
