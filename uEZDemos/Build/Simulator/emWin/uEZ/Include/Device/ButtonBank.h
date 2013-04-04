/*-------------------------------------------------------------------------*
 * File:  ButtonBank.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ ButtonBank Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_ButtonBank_H_
#define _DEVICE_ButtonBank_H_

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
 *  @file   /Include/Device/ButtonBank.h
 *  @brief  uEZ ButtonBank Device Interface
 *
 *  The uEZ ButtonBank Device Interface
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
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*SetActiveButtons)(
            void *aWorkspace, 
            TUInt32 aActiveButtons);
    T_uezError (*Read)(
            void *aWorkspace,
            TUInt32 *aButtons);
} DEVICE_ButtonBank;

#endif // _DEVICE_ButtonBank_H_
/*-------------------------------------------------------------------------*
 * End of File:  ButtonBank.h
 *-------------------------------------------------------------------------*/

