/*-------------------------------------------------------------------------*
 * File:  USBMSDrive.h
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
 *-------------------------------------------------------------------------*/
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
#ifndef _USB_MS_DRIVE_H_
#define _USB_MS_DRIVE_H_

#include <Device/MassStorage.h>

typedef struct {
    void (*iUSBMSDriveActivity)(void *aWorkspace);
} T_USBMSDriveCallbacks;

// Main Virtual Comm Functions:
T_uezError USBMSDriveInitialize(
     T_USBMSDriveCallbacks *aCallbacks,
     void *aCallbackWorkspace,
     const char *aDeviceName);

#endif // _USB_MS_DRIVE_H_

/*-------------------------------------------------------------------------*
 * End of File:  USBMSDrive.h
 *-------------------------------------------------------------------------*/
