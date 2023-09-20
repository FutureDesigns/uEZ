/*-------------------------------------------------------------------------*
 * File:  Flash.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Flash Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Flash_H_
#define _DEVICE_Flash_H_

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
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
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
#include <Types/Flash.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Read)(void *aWorkspace,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
    T_uezError (*Write)(void *aWorkspace,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
    T_uezError (*BlockErase)(void *aWorkspace,
            TUInt32 aOffset,
            TUInt32 aNumBytes);
    T_uezError (*ChipErase)(void *aWorkspace);
    T_uezError (*QueryReg)(void *aWorkspace,
            TUInt32 aReg,
            TUInt32 *aValue);
    T_uezError (*GetChipInfo)(void *aWorkspace,
            T_FlashChipInfo *aInfo);
    T_uezError (*GetBlockInfo)(void *aWorkspace,
            TUInt32 aOffset,
            T_FlashBlockInfo *aBlockInfo);

    // 2.13 for QSPI OTP region
    T_uezError (*ReadOtp)(void *aWorkspace,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
    T_uezError (*WriteOtp)(void *aWorkspace,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
    T_uezError (*LockOtp)(void *aWorkspace,
            TBool aLockOtp);
} DEVICE_Flash;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_Flash_H_
/*-------------------------------------------------------------------------*
 * End of File:  Flash.h
 *-------------------------------------------------------------------------*/

