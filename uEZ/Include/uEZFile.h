/*-------------------------------------------------------------------------*
* File:  uEZFile.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to lower level file drivers.
*-------------------------------------------------------------------------*/
#ifndef _UEZ_FILE_H_
#define _UEZ_FILE_H_

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
#include "uEZTypes.h"
#include "uEZErrors.h"
#include "uEZRTOS.h"
#include <Types/File.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @typedef T_uezFile
 * uEZ File Type
 */
typedef TUInt32 T_uezFile;
#define UEZ_FILE_BAD         0

/**
 *	Initialise the file system data structures.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileSystemInit( void );

/**
 *	Add a file system at a given drive location.
 *
 *	@param[in]	aFileSystemDevice		File system device driver
 *	@param[in]	aMountPoint					Point directory
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileSystemMount(
            T_uezDevice aFileSystemDevice, 
            const char *const aMountPoint);
						
/**
 *	Remove a file system at the given mount point.
 *
 *	@param[in]	aMountPoint					Point directory
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileSystemUnmount(const char *const aMountPoint);

/**
 *	Open a file for reading or writing.
 *
 *	@param[in]	*aName			Name of file to open
 *	@param[in]	aFlags			FILE_FLAG_xxx telling open type
 *	@param[out]	*aFile			File opened (or 0)
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileOpen(
            const char * const aName, 
            TUInt32 aFlags,
            T_uezFile *aFile);
						
/**
 *	Close a previously opened file.
 *
 *	@param[in]	aFile		File to close
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileClose(T_uezFile aFile);

/**
 *	Read bytes from the current file.
 *
 *	@param[in]	aFile						File to read bytes from
 *	@param[out]	*aBuffer				Pointer to place to store bytes
 *	@param[in]	aNumBytes				Number of bytes to read
 *	@param[out]	*aNumBytesRead	Pointer to number of bytes read, or 0.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileRead(
            T_uezFile aFile, 
            void *aBuffer, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesRead);
						
/**
 *	Write bytes to the current file.
 *
 *	@param[in]	aFile						File to write bytes to
 *	@param[in]	*aBuffer				Pointer to place with bytes
 *	@param[in]	aNumBytes				Number of bytes to write
 *	@param[in]	*aNumBytesWritten	Pointer to number of bytes written, or 0.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileWrite(
            T_uezFile aFile, 
            void *aBuffer, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesWritten);

/**
 *	Delete a file.
 *
 *	@param[in]	aName						File to delete
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileDelete(const char * const aName);

/**
 *	Start looking for files in the current directory
 *
 *	@param[in]	*aDirectory		Name of directory to look within
 *	@param[in]	*aEntry				Current file entry.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileFindStart(const char * const aDirectory, T_uezFileEntry *aEntry);

/**
 *	Seek the next file in the list, or return with an error code.
 *
 *	@param[in]	*aEntry				Current file entry.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileFindNext(T_uezFileEntry *aEntry);

/**
 *	Stop looking for files and release related memory.
 *
 *	@param[in]	*aEntry				Current file entry.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileFindStop(T_uezFileEntry *aEntry);

/**
 *	Determine the length of an open file
 *
 *	@param[in]	aFile				File to read bytes from
 *	@param[out]	*aLength		Length returned
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileGetLength(T_uezFile aFile, TUInt32 *aLength);

/**
 *	Look up the given file handle and tell what position the file
 *  is within the file.
 *
 *	@param[in]	aFile				File to read bytes from
 *	@param[out]	*aPosition	Pointer to variable to receive position
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileTellPosition(T_uezFile aFile, TUInt32 *aPosition);

/**
 *	Seek the position in the file at an offset from the beginning of the
 *  file.
 *
 *	@param[in]	aFile				File to read bytes from
 *	@param[out]	*aPosition	Pointer to variable to receive position
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileSeekPosition(T_uezFile aFile, TUInt32 aPosition);

/**
 *	Rename the given file to a new filename in the same directory.
 *
 *	@param[in]	*aOldFilename		File old name
 *	@param[in]	*aNewFilename		File new name
 *
 *	@return	T_uezError
 */ 
T_uezError UEZFileRename(const char * const aOldFilename, const char * const aNewFilename);

/**
 *  Make a new directory at the given location.  The parent directory
 *  must already exist.
 *
 *  @param[in]  *aDiretoryName      Directory to be created
 *
 *  @return T_uezError
 */
T_uezError UEZFileMakeDirectory(const char * const aDiretoryName);

/**
 * Write the data that needs to be saved based on the given drive path.
 *
 * @param[in]   *aDrivePath         Directory to drive to sync
 *
 * @return T_uezError               Error code, if any. UEZ_ERROR_OK if
 *                                  successful.
 */
T_uezError UEZFileSystemSync(const char *const aDrivePath);

/**
 * Get information about the drive volume at the given path.
 *
 * @param[in]   *aDrivePath         Directory to drive to get info
 * @param[out]  *aInfo              Structure to receive volume information
 *
 * @return T_uezError               Error code, if any. UEZ_ERROR_OK if
 *                                  successful.
 */
T_uezError UEZFileSystemGetVolumeInfo(
        const char * const aDrivePath,
        T_uezFileSystemVolumeInfo *aInfo);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_FILE_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZFile.h
 *-------------------------------------------------------------------------*/
