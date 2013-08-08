/**
 *    @file     uEZDevice.h
 *  @brief     uEZ Device Interface
 *
 *    The uEZ interface which maps to lower level device drivers.
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
#ifndef _UEZ_DEVICE_H_
#define _UEZ_DEVICE_H_

#include "uEZTypes.h"
#include "uEZErrors.h"

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

#endif // _UEZ_DEVICE_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZDevice.h
 *-------------------------------------------------------------------------*/
