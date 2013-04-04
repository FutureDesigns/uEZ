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

#endif // _DEVICE_MASS_STORAGE_H_
/*-------------------------------------------------------------------------*
 * End of File:  Device/MassStorage.h
 *-------------------------------------------------------------------------*/

