/*-------------------------------------------------------------------------*
 * File:  Device/MassStorage.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Mass Storage Device Interface
 * Implementation:
 *      Mass storage device with block level control
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_MASS_STORAGE_H_
#define _DEVICE_MASS_STORAGE_H_

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
/**
 *  @file   /Include/Device/MassStorage.h
 *  @brief  uEZ Mass Storage Device Interface
 *
 *  The uEZ Mass Storage Device Interface
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
#include <Types/TimeDate.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef TUInt32 T_msStatus;
    #define MASS_STORAGE_STATUS_NEED_INIT       (1<<0)
    #define MASS_STORAGE_STATUS_NO_MEDIUM       (1<<1)
    #define MASS_STORAGE_STATUS_WRITE_PROTECTED (1<<2)

typedef struct {
    TUInt32 iNumSectors;
    TUInt32 iSectorSize;
    TUInt32 iBlockSize; /**< num sectors per block */
} T_msSizeInfo;

typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Init)(void *aWorkspace, TUInt32 aUnitAddress);
    T_uezError (*GetStatus)(void *aWorkspace, T_msStatus *aStatus);
    T_uezError (*Read)(
            void *aWorkspace, 
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            void *aBuffer);
    T_uezError (*Write)(
            void *aWorkspace, 
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            const void *aBuffer);
    T_uezError (*Sync)(void *aWorkspace);
    T_uezError (*GetSizeInfo)(void *aWorkspace, T_msSizeInfo *aInfo);
    T_uezError (*SetPower)(void *aWorkspace, TBool aOn);
    T_uezError (*SetLock)(void *aWorkspace, TBool aLock);
    T_uezError (*SetSoftwareWriteProtect)(void *aWorkspace, TBool aSWWriteProtect);
    T_uezError (*Eject)(void *aWorkspace);
    T_uezError (*MiscControl)(
            void *aWorkspace,
            TUInt32 aControlCode,
            void *aBuffer);
} DEVICE_MassStorage;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_MASS_STORAGE_H_
/*-------------------------------------------------------------------------*
 * End of File:  Device/MassStorage.h
 *-------------------------------------------------------------------------*/

