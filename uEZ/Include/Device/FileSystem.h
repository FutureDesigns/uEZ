/*-------------------------------------------------------------------------*
 * File:  FileSystem.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ FileSystem Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_FileSystem_H_
#define _DEVICE_FileSystem_H_

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
 *  @file   /Include/Device/FileSystem.h
 *  @brief  uEZ FileSystem Device Interface
 *
 *  The uEZ FileSystem Device Interface
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
#include <Types/File.h>
#include <Source/Library/FileSystem/FATFS/ff.h>

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
    T_uezError (*Open)(
            void *aWorkspace,
            const char * const aFilename,
            TUInt32 aFlags,
            TUInt32 *aFileHandle);
    T_uezError (*Close)(
            void *aWorkspace, 
            TUInt32 aFileHandle);
    T_uezError (*Read)(
            void *aWorkspace,
            TUInt32 aFileHandle, 
            void *aBuffer,
            TUInt32 aNumBytes,
            TUInt32 *aNumRead);
    T_uezError (*Write)(
            void *aWorkspace, 
            TUInt32 aFileHandle, 
            void *aBuffer,
            TUInt32 aNumBytes,
            TUInt32 *aNumWritten);
    T_uezError (*Delete)(
            void *aWorkspace, 
            const char * const aFilename);
    T_uezError (*FindStart)(
            void *aWorkspace,
            const char * const aDirectory,
            T_uezFileEntry *aEntry);
    T_uezError (*FindNext)(
            void *aWorkspace,
            T_uezFileEntry *aEntry);
    T_uezError (*FindStop)(
            void *aWorkspace,
            T_uezFileEntry *aEntry);
    T_uezError (*GetLength)(
            void *aWorkspace,
            TUInt32 aFileHandle,
            TUInt32 *aLength);


    /**
     * v1.07 Functions
     */
    T_uezError (*SeekPosition)(
            void *aWorkspace,
            TUInt32 aFileHandle,
            TUInt32 aPosition);
    T_uezError (*TellPosition)(
            void *aWorkspace,
            TUInt32 aFileHandle,
            TUInt32 *aPosition);


    /**
     * v1.13 Functions
     */
    T_uezError (*Rename)(
        void *aWorkspace,
        const char * const aOldFilename,
        const char * const aNewFilename);
    T_uezError (*MakeDirectory)(
        void *aWorkspace,
        const char * const aDirectoryName);


    /**
      * v1.13 Functions
      */
    /** Sync ensures any blocks waiting to be written are stored on the volume */
    T_uezError (*Sync)(void *aWorkspace, const char * const aPath);

    /** GetVolumeInfo returns information about the filesystem based on the */
    /* given path into the file system (optional for some file systems) */
    T_uezError (*GetVolumeInfo)(
            void *aWorkspace,
            const char * const aPath,
            T_uezFileSystemVolumeInfo *aInfo);

   /**
     * v2.12 Functions
     */
    // GetStorageInfo returns information about the physical storage device 
    T_uezError (*GetStorageInfo)(
            void *aWorkspace,
            const char aDriveNum,
            T_msSizeInfo *aInfo);

    // format the drive
    T_uezError (*MKFS)(
        void *aWorkspace,
        const TCHAR* path,
        const MKFS_PARM* opt);

   /**
     * v2.14 Functions
     */
    // set the buffer used to speed up seek operations
    T_uezError (*SetTableBuffer)(
        void *aWorkspace,
            TUInt32 aFileHandle,
            DWORD* bufPtr);

} DEVICE_FileSystem;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_FileSystem_H_
/*-------------------------------------------------------------------------*
 * End of File:  FileSystem.h
 *-------------------------------------------------------------------------*/
