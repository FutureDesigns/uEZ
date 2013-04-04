/*-------------------------------------------------------------------------*
 * File:  Flash.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Flash Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Flash_H_
#define _DEVICE_Flash_H_

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
#include <Types/Flash.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Read)(
            void *aWorkspace,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
    T_uezError (*Write)(
            void *aWorkspace,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
    T_uezError (*BlockErase)(
            void *aWorkspace,
            TUInt32 aOffset,
            TUInt32 aNumBytes);
    T_uezError (*ChipErase)(void *aWorkspace);
    T_uezError (*QueryReg)(
            void *aWorkspace,
            TUInt32 aReg,
            TUInt32 *aValue);
    T_uezError (*GetChipInfo)(
            void *aWorkspace,
            T_FlashChipInfo *aInfo);
    T_uezError (*GetBlockInfo)(
            void *aWorkspace,
            TUInt32 aOffset,
            T_FlashBlockInfo *aBlockInfo);
} DEVICE_Flash;

#endif // _DEVICE_Flash_H_
/*-------------------------------------------------------------------------*
 * End of File:  Flash.h
 *-------------------------------------------------------------------------*/

