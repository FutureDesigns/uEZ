/*-------------------------------------------------------------------------*
 * File:  DEVICE/DAC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     DAC Device interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_DAC_H_
#define _DEVICE_DAC_H_

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
    T_uezDeviceInterface iInterface;

    // Functions
    T_uezError (*Read)(void *aWorkspace, TUInt32 *aValue);
    T_uezError (*ReadMilliVolts)(void *aWorkspace, TUInt32 *aMilliVolts);
    T_uezError (*Write)(void *aWorkspace, const TUInt32 value);
    T_uezError (*WriteMilliVolts)(void *aWorkspace, const TUInt32 aMilliVolts);
    T_uezError (*SetBias)(void *aWorkspace, TBool aBias);
    T_uezError (*SetVRef)(void *aWorkspace, const TUInt32 aVRef);
} DEVICE_DAC;

#endif /* _DEVICE_DAC_H_ */
