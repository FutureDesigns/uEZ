/*-------------------------------------------------------------------------*
 * File:  Keypad.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Keypad Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Keypad_H_
#define _DEVICE_Keypad_H_

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
/**
 *  @file   /Include/Device/Keypad.h
 *  @brief  uEZ Keypad Device Interface
 *
 *  The uEZ Keypad Device Interface
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
#include <uEZDevice.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Scan)(void *aWorkspace, TUInt32 *aScanKey);
    T_uezError (*Register)(void *aWorkspace, T_uezQueue aQueue);
    T_uezError (*Unregister)(void *aWorkspace, T_uezQueue aQueue);
} DEVICE_Keypad;

#endif // _DEVICE_Keypad_H_
/*-------------------------------------------------------------------------*
 * End of File:  Keypad.h
 *-------------------------------------------------------------------------*/
