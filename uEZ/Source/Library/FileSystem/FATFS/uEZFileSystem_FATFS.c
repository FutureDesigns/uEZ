/*-------------------------------------------------------------------------*
 * File:  uEZFileSystem_FATFS.c
 *-------------------------------------------------------------------------*/
/*
 *    @addtogroup uEZFileSystem_FATFS
 *  @{
 *  @brief     uEZ FAT FileSystem Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    Description:
 *      UEZlex File System to the NXP USB Host driver
 *
 * Implementation:
 *      Most all of this is calls to the USB Host driver.  It adds a
 *      file system to the uEZ system dynamically (specifically when
 *      a device is mounted)
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * TODO
 * @endcode
 */
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
#include <uEZ.h>
#include <uEZFile.h>
#include <uEZDeviceTable.h>
#include "uEZFileSystem_FATFS.h"
#include <string.h>

#define _INTEGER
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef unsigned char	BYTE;
typedef unsigned int	UINT;
#include "ff.h"

typedef struct {
    DIR iDir;
    FILINFO iInfo;
} FS_FATFS_FindSession;

#ifdef NO_DYNAMIC_MEMORY_ALLOC
#define uezFileSystemMAX_FIND_SESSIONS        ( 2 )

/* static find session. */
typedef struct {
    /* Pointer to find session */
    FS_FATFS_FindSession pFindSession;
	
	/* Availability Flag */
	TBool iInUse;
} T_staticFindSession;

static T_staticFindSession G_session[ uezFileSystemMAX_FIND_SESSIONS ];
#endif

#define MAX_FILES_OPEN  4
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    FIL iFile;
    TBool iInUse;
} T_FATFS_File;

typedef struct {
    const DEVICE_FileSystem *iDevice;
    T_uezSemaphore iSem;

    T_FATFS_File iFiles[MAX_FILES_OPEN];
} T_FATFS_FileSystem_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
T_uezError FS_FATFS_FindNext(
            void *aWorkspace,
            T_uezFileEntry *aEntry);

/*---------------------------------------------------------------------------*
 * Routine:  FATFS_FileSystem_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/*
 *  Setup the workspace
 *
 *  @param [in]    *aWorkspace  	Workspace
 *
 *  @return        T_uezError   	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FATFS_FileSystem_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;

	#ifdef NO_DYNAMIC_MEMORY_ALLOC
	/* Initialise the find session list. */
    for (i = 0; i < (uezFileSystemMAX_FIND_SESSIONS - 1); i++) {
        G_session[i].iInUse = EFalse;
    }
	#endif
	
    // Then create a semaphore to limit the number of accessors to 1 at a time
    error = UEZSemaphoreCreateBinary(&p->iSem);

    // Note that none of the file handles are in use
    for (i=0; i<MAX_FILES_OPEN; i++)
        p->iFiles[i].iInUse = EFalse;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGetFreeFile
 *---------------------------------------------------------------------------*/
/*
 *  Find a free file entry in the files list.  The index in the files list
 *  is returned or -1 is returned if the list is full.
 *
 *  @param [in]    *p               Workspace
 *
 *  @return        TInt16           Index to entry, or -1 for none.
 */
/*---------------------------------------------------------------------------*/
static TInt16 IGetFreeFile(T_FATFS_FileSystem_Workspace *p)
{
    TUInt32 i;

    for (i=0; i<MAX_FILES_OPEN; i++) {
        if (!p->iFiles[i].iInUse) {
            p->iFiles[i].iInUse = ETrue;
            return i;
        }
    }

    return -1;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGetDriveNumber
 *---------------------------------------------------------------------------*/
/*
 *  Given a path stripped of its mount point (if any), the path is converted
 *  to a drive number.  Therefore, a path of "1:/" returns 1, "0:/" returns
 *  0, and no path "/" returns 0.
 *
 *  @param [in]    *aPath           Path with embedded drive number
 *
 *  @return        TInt8            Drive number, 0 is the default.
 */
/*---------------------------------------------------------------------------*/
static TUInt8 IGetDriveNumber(const char *aPath)
{
    const char *p_dir = aPath;
    TUInt8 drive;

    while (*p_dir == ' ') p_dir++;      /* Strip leading spaces */
    drive = p_dir[0] - '0';           /* Is there a drive number? */
    if (drive <= 9 && p_dir[1] == ':')
        p_dir += 2;             /* Found a drive number, get and strip it */
    else
        drive = 0;            /* No drive number is given, use drive number 0 as default */

    return drive;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGrab
 *---------------------------------------------------------------------------*/
/*
 *  Grab semaphore to this file system object.
 *
 *  @param [in]    *p               Path with embedded drive number
 */
/*---------------------------------------------------------------------------*/
static void IGrab(T_FATFS_FileSystem_Workspace *p)
{
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
}

/*---------------------------------------------------------------------------*
 * Routine:  IRelease
 *---------------------------------------------------------------------------*/
/*
 *  Release semaphore to this file system object.
 *
 *  @param [in]    *p               Path with embedded drive number
 */
/*---------------------------------------------------------------------------*/
static void IRelease(T_FATFS_FileSystem_Workspace *p)
{
    UEZSemaphoreRelease(p->iSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  IFATFS_ConvertResultCodeToErrorCode
 *---------------------------------------------------------------------------*/
/*
 *  Convert from the FATFS error codes into UEZ_ERROR_... standard error
 *  codes.
 *
 *  @param [in]    FRESULT          Error code to convert
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
static T_uezError IFATFS_ConvertResultCodeToErrorCode(FRESULT res)
{
    switch (res) {
	case FR_OK:
            return UEZ_ERROR_NONE;
	case FR_NOT_READY:
            return UEZ_ERROR_NOT_READY;
        case FR_NO_FILE:
            return UEZ_ERROR_NOT_FOUND;
        case FR_NO_PATH:
            return UEZ_ERROR_NOT_FOUND;
        case FR_INVALID_NAME:
            return UEZ_ERROR_INVALID_PARAMETER;
        case FR_INVALID_DRIVE:
            return UEZ_ERROR_INVALID_PARAMETER;
        case FR_DENIED:
            return UEZ_ERROR_ILLEGAL_OPERATION;
        case FR_EXIST:
            return UEZ_ERROR_ALREADY_EXISTS;
	    case FR_RW_ERROR:
            return UEZ_ERROR_READ_WRITE_ERROR;
        case FR_WRITE_PROTECTED:
            return UEZ_ERROR_PROTECTED;
        case FR_NOT_ENABLED:
            return UEZ_ERROR_NOT_ACTIVE;
        case FR_NO_FILESYSTEM:
            return UEZ_ERROR_NOT_AVAILABLE;
        case FR_INVALID_OBJECT:
            return UEZ_ERROR_INVALID_PARAMETER;
        case FR_MKFS_ABORTED:
            return UEZ_ERROR_CANCELLED;
        default:
            return UEZ_ERROR_UNKNOWN;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_Open
 *---------------------------------------------------------------------------*/
/*
 *  Open the given filename for reading, writing, or appending.  Returns
 *  the file handle of the newly opened file or returns an error.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]     *aFilename      Pointer to filename
 *
 *  @param [in]     aFlags          File type flags (FILE_FLAG_...)
 *
 *  @param [out]    *aFileHandle    File handle to opened device
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_Open(
            void *aWorkspace,
            const char * const aFilename,
            TUInt32 aFlags,
            TUInt32 *aFileHandle)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    TInt32 fileno;
    T_FATFS_File *p_file;
    BYTE mode;
    FRESULT res;

    IGrab(p);

    fileno = IGetFreeFile(p);
    if (fileno < 0) {
        IRelease(p);
        return UEZ_ERROR_OUT_OF_HANDLES;
    }
    p_file = p->iFiles+fileno;
    switch (aFlags) {
        default:
        case FILE_FLAG_READ_ONLY:
            mode = FA_READ|FA_OPEN_EXISTING;
            break;
        case FILE_FLAG_WRITE:
            mode = FA_CREATE_ALWAYS | FA_WRITE;
            break;
        case FILE_FLAG_APPEND:
            mode = FA_OPEN_ALWAYS | FA_WRITE;
            break;
    }

    res = f_open(&p_file->iFile, aFilename, mode);
    if (res == FR_OK) {
        *aFileHandle = fileno;
        if (aFlags == FILE_FLAG_APPEND) {
            // Seek the end of the file
            f_lseek(&p_file->iFile, p_file->iFile.fsize);
        }
    } else {
        // Not good, release the handle immediately
        p->iFiles[fileno].iInUse = EFalse;
        *aFileHandle = 0;
    }

    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_OpenReadOnly
 *---------------------------------------------------------------------------*/
/*
 *  Open the given filename for reading, but not writing or appending.  Returns
 *  the file handle of the newly opened file or returns an error.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]     *aFilename      Pointer to filename
 *
 *  @param [in]     aFlags          File type flags (FILE_FLAG_...)
 *
 *  @param [out]    *aFileHandle    File handle to opened device
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_OpenReadOnly(
            void *aWorkspace,
            const char * const aFilename,
            TUInt32 aFlags,
            TUInt32 *aFileHandle)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    TInt32 fileno;
    T_FATFS_File *p_file;
    BYTE mode;
    FRESULT res;

    IGrab(p);

    fileno = IGetFreeFile(p);
    if (fileno < 0) {
        IRelease(p);
        return UEZ_ERROR_OUT_OF_HANDLES;
    }
    p_file = p->iFiles+fileno;
    switch (aFlags) {
        default:
        case FILE_FLAG_READ_ONLY:
            mode = FA_READ|FA_OPEN_EXISTING;
            break;
        case FILE_FLAG_WRITE:
            mode = FA_CREATE_ALWAYS | FA_WRITE;
            break;
        case FILE_FLAG_APPEND:
            mode = FA_OPEN_ALWAYS | FA_WRITE;
            break;
    }

    if (mode & FA_WRITE)  {
        res = FR_WRITE_PROTECTED; // assume we are write protected
    } else {
        res = f_open(&p_file->iFile, aFilename, mode);
    }
    if (res == FR_OK) {
        *aFileHandle = fileno;
        if (aFlags == FILE_FLAG_APPEND) {
            // Seek the end of the file
            f_lseek(&p_file->iFile, p_file->iFile.fsize);
        }
    } else {
        // Not good, release the handle immediately
        p->iFiles[fileno].iInUse = EFalse;
        *aFileHandle = 0;
    }

    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_Close
 *---------------------------------------------------------------------------*/
/*
 *  Close a previously opened file.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]    aFileHandle      Handle of previously opened file
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_Close(
            void *aWorkspace,
            TUInt32 aFileHandle)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    T_FATFS_File *p_file;
    FRESULT res;

    // Check for valid file handle
    if (aFileHandle >= MAX_FILES_OPEN)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine if valid file handle data
    p_file = p->iFiles + aFileHandle;
    if (!p_file->iInUse)
        return UEZ_ERROR_HANDLE_INVALID;

    IGrab(p);

    // Close it and mark it closed regardless of the result
    res = f_close(&p_file->iFile);
    p_file->iInUse = EFalse;

    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_Read
 *---------------------------------------------------------------------------*/
/*
 *  Read a block of data from an open file.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]    aFileHandle      Handle of opened file
 *
 *  @param [in]    *aBuffer         Buffer to received bytes of data
 *
 *  @param [in]    aNumBytes        Number of bytes to read into buffer
 *
 *  @param [out]   *aNumRead        Number of bytes actually read (end of file
 *                                      may read less)
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_Read(
            void *aWorkspace,
            TUInt32 aFileHandle,
            void *aBuffer,
            TUInt32 aNumBytes,
            TUInt32 *aNumRead)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    T_FATFS_File *p_file;
    FRESULT res;

    // Check for valid file handle
    if (aFileHandle >= MAX_FILES_OPEN)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine if valid file handle data
    p_file = p->iFiles + aFileHandle;
    if (!p_file->iInUse)
        return UEZ_ERROR_HANDLE_INVALID;

    IGrab(p);
    res = f_read(&p_file->iFile, aBuffer, aNumBytes, (UINT*)aNumRead);
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_Write
 *---------------------------------------------------------------------------*/
/*
 *  Write a block of data to an open file.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]    aFileHandle      Handle of opened file for write
 *
 *  @param [in]    *aBuffer         Buffer of bytes of data to write
 *
 *  @param [in]    aNumBytes        Number of bytes in buffer to write into buffer
 *
 *  @param [out]   *aNumWritten     Number of bytes actually written (for
 *                                      file systems with limits)
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_Write(
            void *aWorkspace,
            TUInt32 aFileHandle,
            void *aBuffer,
            TUInt32 aNumBytes,
            TUInt32 *aNumWritten)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    T_FATFS_File *p_file;
    FRESULT res;

    // Check for valid file handle
    if (aFileHandle >= MAX_FILES_OPEN)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine if valid file handle data
    p_file = p->iFiles + aFileHandle;
    if (!p_file->iInUse)
        return UEZ_ERROR_HANDLE_INVALID;

    IGrab(p);
    res = f_write(&p_file->iFile, aBuffer, aNumBytes, (UINT*)aNumWritten);
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_WriteReadOnly
 *---------------------------------------------------------------------------*/
/*
 *  Stub for attempting to write when read only file system selected.
 *  Writing to a read only file system will always return an error.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]    aFileHandle      Not used
 *
 *  @param [in]    *aBuffer         Not used
 *
 *  @param [in]    aNumBytes        Not used
 *
 *  @param [out]   *aNumWritten     Not used
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_WriteReadOnly(
            void *aWorkspace,
            TUInt32 aFileHandle,
            void *aBuffer,
            TUInt32 aNumBytes,
            TUInt32 *aNumWritten)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aFileHandle);
    PARAM_NOT_USED(aBuffer);
    PARAM_NOT_USED(aNumBytes);
    PARAM_NOT_USED(aNumWritten);

    // Don't even try, just return error
    return IFATFS_ConvertResultCodeToErrorCode(FR_WRITE_PROTECTED);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_Delete
 *---------------------------------------------------------------------------*/
/*
 *  Delete a file by its path name.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]    *aFilename       Path to filename
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_Delete(
            void *aWorkspace,
            const char * const aFilename)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    FRESULT res;

    IGrab(p);
    res = f_unlink(aFilename);
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_DeleteReadOnly
 *---------------------------------------------------------------------------*/
/*
 *  Deleting is not allowed on a read only file system.  Always return an
 *  error.
 *
 *  @param [in]    *aWorkspace      File System workspace
 *
 *  @param [in]    *aFilename       Not used.
 *
 *  @return        T_uezError       Resulting error code.
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_DeleteReadOnly(
            void *aWorkspace,
            const char * const aFilename)
{
    PARAM_NOT_USED(aWorkspace);

    // Don't even try, just return error
    return IFATFS_ConvertResultCodeToErrorCode(FR_WRITE_PROTECTED);
}

/*---------------------------------------------------------------------------*
 * Routine:  IFat16ToUEZDate
 *---------------------------------------------------------------------------*/
/*
 *  Convert a FAT16 date into a uEZ Date.
 *
 *  @param [out]   *aUEZDate        Structure to receive converted date
 *
 *  @param [in]    *aFat16Date      Fat structure to convert
 *
 *  @return        void
 */
/*---------------------------------------------------------------------------*/
static void IFat16ToUEZDate(T_uezFileDate *aUEZDate, TUInt16 aFat16Date)
{
    aUEZDate->iYear = 1980 + ((aFat16Date>>9)&0x3F);
    aUEZDate->iMonth = (aFat16Date>>5)&0x0F;
    aUEZDate->iDay = (aFat16Date>>0)&0x1F;
}

/*---------------------------------------------------------------------------*
 * Routine:  IFat16ToUEZTime
 *---------------------------------------------------------------------------*/
/*
 *  Convert a FAT16 time into a uEZ Time.
 *
 *  @param [out]   *aUEZTime        Structure to receive converted time
 *
 *  @param [in]    *aFat16Time      Fat structure to convert
 *
 *  @return        void
 */
/*---------------------------------------------------------------------------*/
static void IFat16ToUEZTime(T_uezFileTime *aUEZTime, TUInt16 aFat16Time)
{
    aUEZTime->iFineSeconds = 0;
    aUEZTime->iHour = (aFat16Time>>11)&0x1F;
    aUEZTime->iMinutes = (aFat16Time>>5)&0x3F;
    aUEZTime->iSeconds = (aFat16Time>>0)&0x1F;
}

/*---------------------------------------------------------------------------*
 * Routine:  ITranslateFileInfo
 *---------------------------------------------------------------------------*/
/*
 *  Convert a FATFS FILINFO structure into its corresponding T_uezFileEntry
 *  type.
 *
 *  @param [out]   *aEntry          Entry structure to fill with FILINFO
 *
 *  @param [in]    *finfo           FILINFO structure to convert from FATFS
 *
 *  @return        void
 */
/*---------------------------------------------------------------------------*/
static void ITranslateFileInfo(T_uezFileEntry *aEntry, FILINFO *finfo)
{
//    aEntry->iFileAttributes = (aFind->FileAttributes&0x10)?UEZ_FILE_ENTRY_ATTR_DIRECTORY:0;
    aEntry->iFileAttributes = 0;
    if (finfo->fattrib & AM_DIR)
        aEntry->iFileAttributes |= UEZ_FILE_ENTRY_ATTR_DIRECTORY;
    if (finfo->fattrib & AM_RDO)
        aEntry->iFileAttributes |= UEZ_FILE_ENTRY_ATTR_READ_ONLY;
    if (finfo->fattrib & AM_SYS)
        aEntry->iFileAttributes |= UEZ_FILE_ENTRY_ATTR_SYSTEM;
    if (finfo->fattrib & AM_HID)
        aEntry->iFileAttributes |= UEZ_FILE_ENTRY_ATTR_HIDDEN;
    if (finfo->fattrib & AM_ARC)
        aEntry->iFileAttributes |= UEZ_FILE_ENTRY_ATTR_ARCHIVE;

    IFat16ToUEZDate(&aEntry->iAccessDate, finfo->fdate);
    IFat16ToUEZTime(&aEntry->iAccessTime, finfo->ftime);

    IFat16ToUEZDate(&aEntry->iCreateDate, finfo->fdate);
    IFat16ToUEZTime(&aEntry->iCreateTime, finfo->ftime);
    IFat16ToUEZDate(&aEntry->iModifiedDate, finfo->fdate);
    IFat16ToUEZTime(&aEntry->iModifiedTime, finfo->ftime);
    strcpy(aEntry->iFilename, finfo->fname);
    aEntry->iFilesize = finfo->fsize;
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_FindFirst
 *---------------------------------------------------------------------------*/
/*
 *  Find the first file entry in a directory.  Use with FindNext to walk
 *  through the directory list.
 *
 *  @param [in]    *aWorkspace      File System Workspace
 *
 *  @param [in]    *aDirectory      Directory to find first
 *
 *  @param [out]    aEntry          Entry received.
 *
 *  @return        T_uezError       Error code
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_FindFirst(
            void *aWorkspace,
            const char * const aDirectory,
            T_uezFileEntry *aEntry)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    FRESULT res;
	FS_FATFS_FindSession *p_session = 0;
	
#ifdef NO_DYNAMIC_MEMORY_ALLOC
	TUInt32 i;
    IGrab(p);
	for(i=0; i<uezFileSystemMAX_FIND_SESSIONS; i++) {
		if(G_session[i].iInUse == EFalse) {
			p_session = &G_session[i].pFindSession;
			G_session[i].iInUse = ETrue;
            break;
		}
	}
    IRelease(p);
#else
    p_session = UEZMemAlloc(sizeof(FS_FATFS_FindSession));
#endif
    
    // Remember this block of data
    aEntry->iFindFileSessionDevice = (TUInt32)p_session;

	if (p_session == 0)
        return UEZ_ERROR_OUT_OF_MEMORY;

    // Open up the directory
    IGrab(p);
    res = f_opendir(&p_session->iDir, aDirectory);
    IRelease(p);

    // If we got an error, just ruturn that error
    if (res != FR_OK)
        return IFATFS_ConvertResultCodeToErrorCode(res);

    // Got here, still good, get the first entry
    return FS_FATFS_FindNext(aWorkspace, aEntry);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_FindNext
 *---------------------------------------------------------------------------*/
/*
 *  Find the next file entry after the given file entry.  Returns
 *  UEZ_ERROR_NOT_FOUND if no more file entries.  Use FileFirst if not
 *  started yet.
 *
 *  @param [in]    *aWorkspace      File System Workspace
 *
 *  @param [in/out] aEntry           Last entry received.
 *
 *  @return        T_uezError       Error code
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_FindNext(
            void *aWorkspace,
            T_uezFileEntry *aEntry)
{
    T_FATFS_FileSystem_Workspace *p =
            (T_FATFS_FileSystem_Workspace *)aWorkspace;
    FS_FATFS_FindSession *p_session =
            (FS_FATFS_FindSession *)aEntry->iFindFileSessionDevice;
    FRESULT res;
    FILINFO *p_info = &p_session->iInfo;

    // Make sure the session is valid
    if (!p_session)
         return UEZ_ERROR_INVALID_PARAMETER;

    // Grab the next entry in the list
    IGrab(p);
    p_info->fname[0] = '\0';
    res = f_readdir(&p_session->iDir, p_info);
    IRelease(p);

    // If the end (blank filename) or an error, return now
    p_info = &p_session->iInfo;
    if (res != FR_OK)
        return IFATFS_ConvertResultCodeToErrorCode(res);
    if (!p_info->fname[0])
        return UEZ_ERROR_NOT_FOUND;

    // Convert the information we have into something
    // that is common.
    ITranslateFileInfo(aEntry, &p_session->iInfo);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_FindStop
 *---------------------------------------------------------------------------*/
/*
 *  Stop looking for file entries releasing the file find session info
 *  stored in the aEntry structure.
 *
 *  @param [in]    *aWorkspace      File System Workspace
 *
 *  @param [in]    *aEntry          Last entry found.
 *
 *  @return        T_uezError       Error code
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_FindStop(
            void *aWorkspace,
            T_uezFileEntry *aEntry)
{
#ifdef NO_DYNAMIC_MEMORY_ALLOC
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
#endif
    FS_FATFS_FindSession *p_find = (FS_FATFS_FindSession *)aEntry->iFindFileSessionDevice;
#ifdef NO_DYNAMIC_MEMORY_ALLOC
	TUInt32 i;
#endif
	
    // Make sure the session is valid
    if (!p_find)
         return UEZ_ERROR_INVALID_PARAMETER;

#ifdef NO_DYNAMIC_MEMORY_ALLOC
    IGrab(p);
	for(i=0; i<uezFileSystemMAX_FIND_SESSIONS; i++) {
		if(p_find == &G_session[i].pFindSession) {
			G_session[i].iInUse = EFalse;
            break;
		}
	}
    IRelease(p);
#else
    // Release the memory
    UEZMemFree(p_find);
#endif
    aEntry->iFindFileSessionDevice = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_GetLength
 *---------------------------------------------------------------------------*/
/*
 *  Return the length of the currently open file.
 *
 *  @param [in]    *aWorkspace 			Workspace
 *
 *  @param [in]    aFileHandle    		File currently open
 *
 *  @param [in]    *aLength     		Length returned
 *
 *  @return        T_uezError  			Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_GetLength(
            void *aWorkspace,
            TUInt32 aFileHandle,
            TUInt32 *aLength)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    T_FATFS_File *p_file;
    FRESULT res=(FRESULT)0;

    // Check for valid file handle
    if (aFileHandle >= MAX_FILES_OPEN)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine if valid file handle data
    p_file = p->iFiles + aFileHandle;
    if (!p_file->iInUse)
        return UEZ_ERROR_HANDLE_INVALID;

    IGrab(p);
    *aLength = p_file->iFile.fsize;
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_SeekPosition
 *---------------------------------------------------------------------------*/
/*
 *  Seek the position in the file at an offset from the beginning of the
 *      file.
 *
 *  @param [in]    *aWorkspace     	Workspace
 *
 *  @param [in]    aFileHandle    	File currently open
 *
 *  @param [in]    aPosition      	Position to jump to
 *
 *  @return        T_uezError    	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_SeekPosition(
        void *aWorkspace,
        TUInt32 aFileHandle,
        TUInt32 aPosition)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    T_FATFS_File *p_file;
    FRESULT res=(FRESULT)0;

    // Check for valid file handle
    if (aFileHandle >= MAX_FILES_OPEN)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine if valid file handle data
    p_file = p->iFiles + aFileHandle;
    if (!p_file->iInUse)
        return UEZ_ERROR_HANDLE_INVALID;

    IGrab(p);
    res = f_lseek(&p_file->iFile, aPosition);
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_TellPosition
 *---------------------------------------------------------------------------*/
/*
 *  Look up the given file handle and tell what position the file
 *      is within the file.
 *
 *  @param [in]    *aWorkspace      Workspace
 *
 *  @param [in]    aFileHandle      File currently open
 *
 *  @param [in]    *aPosition       Pointer to variable to receive position
 *
 *  @return        T_uezError      	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_TellPosition(
        void *aWorkspace,
        TUInt32 aFileHandle,
        TUInt32 *aPosition)
{
    T_FATFS_FileSystem_Workspace *p = (T_FATFS_FileSystem_Workspace *)aWorkspace;
    T_FATFS_File *p_file;
    FRESULT res=(FRESULT)0;

    // Check for valid file handle
    if (aFileHandle >= MAX_FILES_OPEN)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine if valid file handle data
    p_file = p->iFiles + aFileHandle;
    if (!p_file->iInUse)
        return UEZ_ERROR_HANDLE_INVALID;

    IGrab(p);
    *aPosition = p_file->iFile.fptr;
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_Rename
 *---------------------------------------------------------------------------*/
/*
 *  Rename the given file to the new file name.
 *
 *  @param [in]    *aWorkspace          Workspace
 *
 *  @param [in]    *aOldFilename 		Name of old file to rename
 *
 *  @param [in]    *aNewFilename 		New filename (without full path)
 *
 *  @return        T_uezError       	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_Rename(
    void *aWorkspace,
    const char * const aOldFilename,
    const char * const aNewFilename)
{
    T_FATFS_FileSystem_Workspace *p =
        (T_FATFS_FileSystem_Workspace *)aWorkspace;
    FRESULT res;

    IGrab(p);
    res = f_rename(aOldFilename, aNewFilename);
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_RenameReadOnly
 *---------------------------------------------------------------------------*/
/*
 *  Renaming files is not allowed in read only file systems.
 *
 *  @param [in]    *aWorkspace          Workspace
 *
 *  @param [in]    *aOldFilename        Not used
 *
 *  @param [in]    *aNewFilename        Not used
 *
 *  @return        T_uezError           Error code
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_RenameReadOnly(
    void *aWorkspace,
    const char * const aOldFilename,
    const char * const aNewFilename)
{
    PARAM_NOT_USED(aWorkspace);

    // Don't even try, just return error
    return IFATFS_ConvertResultCodeToErrorCode(FR_WRITE_PROTECTED);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_MakeDirectory
 *---------------------------------------------------------------------------*/
/*
 *  Make a directory at the given location.  The parent directory
 *      must already exist.
 *
 *  @param [in]    *aWorkspace 				Workspace
 *
 *  @param [in]    *aDirectoryName 			Directory to create
 *
 *  @return        T_uezError  				Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_MakeDirectory(
    void *aWorkspace,
    const char * const aDirectoryName)
{
    T_FATFS_FileSystem_Workspace *p =
        (T_FATFS_FileSystem_Workspace *)aWorkspace;
    FRESULT res;

    IGrab(p);
    res = f_mkdir(aDirectoryName);
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FS_FATFS_MakeDirectoryReadOnly
 *---------------------------------------------------------------------------*/
/*
 *  Directories cannot be made on a read only file system.
 *
 *  @param [in]    *aWorkspace              Workspace
 *
 *  @param [in]    *aDirectoryName          Not used
 *
 *  @return        T_uezError               Error code
 */
/*---------------------------------------------------------------------------*/
T_uezError FS_FATFS_MakeDirectoryReadOnly(
    void *aWorkspace,
    const char * const aDirectoryName)
{
    PARAM_NOT_USED(aWorkspace);

    // Don't even try, just return error
    return IFATFS_ConvertResultCodeToErrorCode(FR_WRITE_PROTECTED);
}

/*---------------------------------------------------------------------------*
 * Routine:  FileSystem_FATFS_Create
 *---------------------------------------------------------------------------*/
/*
 *  Initialize FAT Filesystem driver
 *
 *  @param [in] 	*aName	Name of device
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  void UEZPlatform_FileSystem_Require(void)
 *  {
 *      T_uezDevice dev_fs;
 *
 *      DEVICE_CREATE_ONCE();
 *
 *      FileSystem_FATFS_Create("FATFS");
 *      UEZDeviceTableFind("FATFS", &dev_fs);
 *      UEZFileSystemInit();
 *      UEZFileSystemMount(dev_fs, "/");
 *  }
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void FileSystem_FATFS_Create(const char *aName)
{
    // Setup the FAT file system device
    UEZDeviceTableRegister(
            "FATFS",
            (T_uezDeviceInterface *)&FATFS_FileSystem_Interface,
            0,
            0);
}

/*---------------------------------------------------------------------------*
 * Routine:  FileSystem_FATFS_Create_ReadOnly
 *---------------------------------------------------------------------------*/
/*
 *  Initialize the FAT Filesystem driver, but make sure it is a read only
 *  version.
 *
 *  @param [in]     *aName  Name of device
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  void UEZPlatform_FileSystem_Require(void)
 *  {
 *      T_uezDevice dev_fs;
 *
 *      DEVICE_CREATE_ONCE();
 *
 *      FileSystem_FATFS_Create_ReadOnly("FATFS");
 *      UEZDeviceTableFind("FATFS", &dev_fs);
 *      UEZFileSystemInit();
 *      UEZFileSystemMount(dev_fs, "/");
 *  }
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void FileSystem_FATFS_Create_ReadOnly(const char *aName)
{
    // Setup the FAT file system device
    UEZDeviceTableRegister(
            "FATFS",
            (T_uezDeviceInterface *)&FATFS_FileSystem_Interface,
            0,
            0);
}

/*---------------------------------------------------------------------------*
 * Routine:  FileSystem_FATFS_Sync
 *---------------------------------------------------------------------------*/
/*
 *  Sync ensures any blocks waiting to be written are stored on the volume
 *
 *  @param [in]    *aWorkspace      Workspace
 *
 *  @param [in]    aPath            Path to volume to sync
 *
 *  @return        T_uezError       Error code
 */
/*---------------------------------------------------------------------------*/
T_uezError FileSystem_FATFS_Sync(void *aWorkspace, const char * const aPath)
{
    TUInt8 drive;
    T_FATFS_FileSystem_Workspace *p =
        (T_FATFS_FileSystem_Workspace *)aWorkspace;
    FRESULT res;

    drive = IGetDriveNumber(aPath);

    IGrab(p);
    res = f_drive_sync(drive);
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Routine:  FileSystem_FATFS_GetVolumeInfo
 *---------------------------------------------------------------------------*/
/*
 *  GetVolumeInfo returns information about the filesystem based on the
 *  given path into the file system (optional for some file systems)
 *
 *  @param [in]    *aWorkspace      Workspace
 *
 *  @param [in]    aPath            Path to volume to get info
 *
 *  @param [out]   *aInfo           Information retrieved
 *
 *  @return        T_uezError       Error code
 */
/*---------------------------------------------------------------------------*/
T_uezError FileSystem_FATFS_GetVolumeInfo(
        void *aWorkspace,
        const char * const aPath,
        T_uezFileSystemVolumeInfo *aInfo)
{
    T_FATFS_FileSystem_Workspace *p =
        (T_FATFS_FileSystem_Workspace *)aWorkspace;
    FRESULT res;
    FATFS *p_fs;

    IGrab(p);
    // Get the number of clusters free
    res = f_getfree(aPath, (DWORD *)&aInfo->iNumClustersFree, &p_fs);
    if (res == FR_OK) {
        // Get the number of clusters total
        aInfo->iNumClustersTotal = (p_fs->max_clust - 1);

        // Report the sectors per cluster, and the bytes per sector
        aInfo->iSectorsPerCluster = p_fs->csize;
        aInfo->iBytesPerSector = S_MAX_SIZ;
    }
    IRelease(p);

    return IFATFS_ConvertResultCodeToErrorCode(res);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_FileSystem FATFS_FileSystem_Interface = {
	{
	    // Generic device interface
	    "FS:FATFS",
	    0x0100,
	    FATFS_FileSystem_InitializeWorkspace,
	    sizeof(T_FATFS_FileSystem_Workspace),
	},
	
    // Functions
    FS_FATFS_Open,
    FS_FATFS_Close,
    FS_FATFS_Read,
    FS_FATFS_Write,
    FS_FATFS_Delete,
    FS_FATFS_FindFirst,
    FS_FATFS_FindNext,
    FS_FATFS_FindStop,
    FS_FATFS_GetLength,

    // v1.07 Functions
    FS_FATFS_SeekPosition,
    FS_FATFS_TellPosition,

    // v2.00 Functions
    FS_FATFS_Rename,
    FS_FATFS_MakeDirectory,

    // v2.04 Functions
    FileSystem_FATFS_Sync,
    FileSystem_FATFS_GetVolumeInfo
} ;

const DEVICE_FileSystem FATFS_FileSystem_ReadOnly_Interface = {
    {
        // Generic device interface
        "FS:FATFS",
        0x0100,
        FATFS_FileSystem_InitializeWorkspace,
        sizeof(T_FATFS_FileSystem_Workspace),
    },

    // Functions
    FS_FATFS_OpenReadOnly,
    FS_FATFS_Close,
    FS_FATFS_Read,
    FS_FATFS_WriteReadOnly,
    FS_FATFS_DeleteReadOnly,
    FS_FATFS_FindFirst,
    FS_FATFS_FindNext,
    FS_FATFS_FindStop,
    FS_FATFS_GetLength,

    // v1.07 Functions
    FS_FATFS_SeekPosition,
    FS_FATFS_TellPosition,

    // v2.00 Functions
    FS_FATFS_RenameReadOnly,
    FS_FATFS_MakeDirectoryReadOnly,

    // v2.04 Functions
    FileSystem_FATFS_Sync,
    FileSystem_FATFS_GetVolumeInfo
} ;
/* @} */

/*-------------------------------------------------------------------------*
 * End of File:  uEZFileSystem_FATFS.c
 *-------------------------------------------------------------------------*/
