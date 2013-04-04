/*-------------------------------------------------------------------------*
 * File:  FileSystem.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ FileSystem Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_FileSystem_H_
#define _DEVICE_FileSystem_H_

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
} DEVICE_FileSystem;

#endif // _DEVICE_FileSystem_H_
/*-------------------------------------------------------------------------*
 * End of File:  FileSystem.h
 *-------------------------------------------------------------------------*/
