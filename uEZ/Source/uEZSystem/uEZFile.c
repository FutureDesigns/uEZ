/*-------------------------------------------------------------------------*
 * File:  uEZFile.c
 *-------------------------------------------------------------------------*
 * Description:
 *     List of handles in the system.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *  @addtogroup uEZFile
 *  @{
 *  @brief     uEZ File Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ File interface.
 *
 * The uEZ system supplies a simple file based system for handling data. The
 * uEZ File system uses a mount based system with capability to support multiple
 * file system types.  Functionality is aimed at simple read/write access for
 * applications that need configuration or logging features.
 *
 * File systems can be mounted dynamically in the file hierarchy.
 *
 * @par Example code:
 * Example to create a file and write to it.
 * @par
 * @code
 * #include <uEZ.h>
 * #include "uEZFile.h"
 *
 * T_uezFile file;
 * TUInt8 data[9] = "Test Data";
 * TUInt32 numWritten;
 * if (UEZFileOpen("FILENAME.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *     // The file opened properly
 *     UEZFileWrite(file, data, 9, &numWritten);
 *     if (UEZFileClose(file) != UEZ_ERROR_NONE) {
 *         // Error closing file
 *     }
 * } else {
 *     // An error occurred opening the file
 * }
 * @endcode
 */
#include <string.h>
#include "Config.h"
#include <uEZ.h>
#include "uEZFile.h"
#include <uEZDevice.h>
#include <Device/FileSystem.h>
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
/** Mounted file system. */
typedef struct tag_mountedFS {
    /** Pointer to next mount point */
    struct tag_mountedFS *iNext;

    /** Pointer to file system's workspace */
    T_uezDeviceWorkspace *iFSDeviceWorkspace;

    /** character string for mount point in system (e.g., "/sdcard/") */
    char *iMountPoint;
} T_mountedFS;

/** Structure tracking actively open file */
typedef struct tag_openFile {
    /** Pointer to next open file */
    struct tag_openFile *iNext;

    /** Unique file handle within file system */
    TUInt32 iFileHandle;

    /** File system for this open file */
    T_mountedFS *iFS;
} T_openFile;

/** Structure holding state of a find file system */
typedef struct {
    /** File system the find file is being performed on. */
    T_mountedFS *iFS;
} T_findFileSession;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#ifdef NO_DYNAMIC_MEMORY_ALLOC
#define uezfileMAX_FILESYSTEMS          ( 2 )
#define uezfileMAX_MOUNT_STRLEN         ( 80 )
#define uezfileMAX_FILES                ( 5 )
#define uezfileMAX_FIND_SESSIONS        ( 2 )

static T_mountedFS xMountedFS[ uezfileMAX_FILESYSTEMS ];
static char xMountPointStrings[uezfileMAX_FILESYSTEMS][uezfileMAX_MOUNT_STRLEN];
static T_mountedFS *G_freemounts = 0;
static T_openFile xOpenFiles[uezfileMAX_FILES];
static T_openFile *G_freeFiles = 0;
static T_findFileSession xFileFindSessions[uezfileMAX_FIND_SESSIONS];
#endif
/** Pointer to linked list of mounted file systems */
static T_mountedFS *G_mounts = 0;

/** Semaphore for doing activities on the file system lists */
static T_uezSemaphore G_mfsSync = 0;

/** Pointer to list of open files. */
static T_openFile *G_openFiles = 0;

/*---------------------------------------------------------------------------*
 * Routine:  IGrab
 *---------------------------------------------------------------------------*/
/**
 *  Grab exclusive control of the File System.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *  if (UEZFileOpen("FILENAME.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      if (UEZFileClose(file) != UEZ_ERROR_NONE) {
 *          // Error closing file
 *      }
 *  } else {
 *      // An error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IGrab(void)
{
    if (G_mfsSync == 0)
        UEZSemaphoreCreateBinary(&G_mfsSync);
    UEZSemaphoreGrab(G_mfsSync, UEZ_TIMEOUT_INFINITE);
}

/*---------------------------------------------------------------------------*
 * Routine:  IRelease
 *---------------------------------------------------------------------------*/
/**
 *  Release exclusive control of the File System.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *
 *  if (UEZFileOpen("FILENAME.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      if (UEZFileClose(file) != UEZ_ERROR_NONE) {
 *          // Error closing file
 *      }
 *  } else {
 *      // An error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IRelease(void)
{
    UEZSemaphoreRelease(G_mfsSync);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileSystemInit
 *---------------------------------------------------------------------------*/
/**
 *  Initialise the file system data structures.
 *
 *  @return        T_uezError
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezDevice dev_fs;
 *
 *  FileSystem_FATFS_Create("FATFS");
 *  UEZDeviceTableFind("FATFS", &dev_fs);
 *  UEZFileSystemInit();
 *  UEZFileSystemMount(dev_fs, "/");
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileSystemInit(void)
{
#ifdef NO_DYNAMIC_MEMORY_ALLOC
    TUInt16 usLoop;

    /* Initialise the mount point list. */
    for (usLoop = 0; usLoop < (uezfileMAX_FILESYSTEMS - 1); usLoop++) {
        xMountedFS[usLoop].iNext = &xMountedFS[usLoop + 1];
        xMountedFS[usLoop].iMountPoint = &xMountPointStrings[usLoop][0];
    }
    xMountedFS[usLoop].iNext = NULL;
    xMountedFS[usLoop].iMountPoint = &xMountPointStrings[usLoop][0];
    G_freemounts = &xMountedFS[0];

    /* Initialise the file list. */
    for (usLoop = 0; usLoop < (uezfileMAX_FILES - 1); usLoop++) {
        xOpenFiles[usLoop].iNext = &xOpenFiles[usLoop + 1];
    }
    xOpenFiles[usLoop].iNext = NULL;
    G_freeFiles = &xOpenFiles[0];
#endif

    // Initialize the globals
    G_mounts = 0;
    G_mfsSync = 0;
    G_openFiles = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileSystemMount
 *---------------------------------------------------------------------------*/
/**
 *  Add a file system at a given drive location.
 *
 *  @param [in] aFileSystemDevice  File system device driver
 *
 *  @param [in] *aMountPoint        Point directory
 *
 *  @return     T_uezError         UEZ_ERROR_NONE if mounted, otherwise error.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezDevice dev_fs;
 *
 *  FileSystem_FATFS_Create("FATFS");
 *  UEZDeviceTableFind("FATFS", &dev_fs);
 *  UEZFileSystemInit();
 *  UEZFileSystemMount(dev_fs, "/");
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileSystemMount(
    T_uezDevice aFileSystemDevice,
    const char * const aMountPoint)
{
#ifdef NO_DYNAMIC_MEMORY_ALLOC
    T_mountedFS *p = G_freemounts;

    G_freemounts = G_freemounts->iNext;
    if (p == 0)
        return UEZ_ERROR_OUT_OF_MEMORY;
#else
    // Try allocating memory for the FS
    T_mountedFS *p = UEZMemAlloc(sizeof(T_mountedFS));
    if (p == 0)
        return UEZ_ERROR_OUT_OF_MEMORY;

    // Allocate emmory for the mount point or fail.
    p->iMountPoint = UEZMemAlloc(strlen(aMountPoint) + 1);
    if (p->iMountPoint == 0) {
        UEZMemFree(p);
        return UEZ_ERROR_OUT_OF_MEMORY;
    }
    strcpy(p->iMountPoint,aMountPoint);
#endif

    // Link in
    UEZDeviceTableGetWorkspace(aFileSystemDevice, &p->iFSDeviceWorkspace);
    p->iNext = G_mounts;
    IGrab();
    G_mounts = p;
    IRelease();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileSystemUnmount
 *---------------------------------------------------------------------------*/
/**
 *  Remove a file system at the given mount point.
 *
 *  @param [in]    *aMountPoint     Mount directory
 *
 *  @return        T_uezError      UEZ_ERROR_NONE if unmounted, otherwise error
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezDevice dev_fs;
 *
 *  FileSystem_FATFS_Create("FATFS");
 *  UEZDeviceTableFind("FATFS", &dev_fs);
 *  UEZFileSystemInit();
 *  UEZFileSystemMount(dev_fs, "/");
 *  // File operations here
 *  UEZFileSystemUnmount("/");
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileSystemUnmount(const char * const aMountPoint)
{
    // Unmounting the mount point
    T_mountedFS *fs = G_mounts;
    T_mountedFS *fs_prev = 0;

    while (fs) {
        // Go through the list and find a match
        if (strcmp(fs->iMountPoint, aMountPoint) == 0) {
            // Found a match, unlink it from the list
            IGrab();
            if (fs_prev)
                fs_prev->iNext = fs->iNext;
            else
                G_mounts = fs->iNext;
            IRelease();

            // Now that it is unlinked, free the memory
#ifdef NO_DYNAMIC_MEMORY_ALLOC
            fs->iNext = G_freemounts;
            G_freemounts = fs;
#else
            UEZMemFree(fs->iMountPoint);
            UEZMemFree(fs);
#endif
            return UEZ_ERROR_NONE;
        }
        // Keep looking
        fs_prev = fs;
        fs = fs->iNext;
    }

    // Count not unmount because could not find!
    return UEZ_ERROR_NOT_FOUND;
}
/*---------------------------------------------------------------------------*
 * Routine:  IFindMountFS
 *---------------------------------------------------------------------------*/
/**
 *  Convert the filename to a filesystem
 *
 *  @param [in]    *aName        Name of file to find
 *
 *  @param [in]    **p_fs         Returned file system internal struct
 *
 *  @return        T_uezError   UEZ_ERROR_NONE if found, else
 *                                  UEZ_ERROR_NOT_AVAILABLE
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  char *aName = "/NAME.TXT";
 *  T_mountedFS *p_fs;
 *
 *  // Convert the filename to a filesystem
 *  if (IFindMountFS(aName, &p_fs) != UEZ_ERROR_NONE) {
 *      // Not found
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IFindMountFS(const char * const aName, T_mountedFS **p_fs)
{
    // For this early version, we will only use the first mount
    if (!G_mounts)
        return UEZ_ERROR_NOT_AVAILABLE;

    *p_fs = G_mounts;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open a file for reading or writing.  Return an error if a problem.
 *
 *  @param [in]    *aName         Name of file to open
 *
 *  @param [in]    aFlags       FILE_FLAG_xxx telling open type
 *
 *  @param [in]    *aFile       File opened (or 0)
 *
 *  @return        T_uezError   UEZ_ERROR_NONE if file opened, else
 *                              UEZ_ERROR_NOT_FOUND if could not open
 *                              file, UEZ_ERROR_NOT_AVAILABLE if media
 *                              is missing.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *
 *  if (UEZFileOpen("Filename.txt", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // the file opened properly
 *  } else {
 *      // an error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileOpen(
    const char * const aName,
    TUInt32 aFlags,
    T_uezFile *aFile)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_openFile *p_of;
    TUInt32 fsFile;

    // Is there a mounted device?
    IGrab();

    // Convert the filename to a filesystem
    error = IFindMountFS(aName, &p_fs);
    if (error != UEZ_ERROR_NONE) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now get the file system and do the open command
    error = (*dfs)->Open(p_fs->iFSDeviceWorkspace, aName, aFlags, &fsFile);
    if (error == UEZ_ERROR_NONE) {
        // No errors?  Add file to the list of open files
#ifdef NO_DYNAMIC_MEMORY_ALLOC
        p_of = G_freeFiles;
        G_freeFiles = G_freeFiles->iNext;
#else
        p_of = UEZMemAlloc(sizeof(T_openFile));
#endif
        if (p_of == 0) {
            // Close the previously open file, we can't track it
            (*dfs)->Close(p_fs->iFSDeviceWorkspace, *aFile);

            // Note we have not enough memory
            IRelease();
            return UEZ_ERROR_OUT_OF_MEMORY;
        }

        // Add this file into the list
        p_of->iFileHandle = fsFile;
        p_of->iFS = p_fs;
        p_of->iNext = G_openFiles;
        G_openFiles = p_of;

        // Create a handle out of pointer
        *aFile = (T_uezFile)p_of;
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileClose
 *---------------------------------------------------------------------------*/
/**
 *  Close a previously opened file.
 *
 *  @param [in]    aFile          File that was opened
 *
 *  @return        T_uezError     UEZ_ERROR_NONE if closed, else error code.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *  TUInt32 numWritten;
 *
 *  if (UEZFileOpen("FILENAME.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      if (UEZFileClose(file) != UEZ_ERROR_NONE) {
 *          // Error closing file
 *      }
 *  } else {
 *      // An error occurred opening the file
 *  }
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileClose(T_uezFile aFile)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_openFile *p_prev_of;
    T_openFile *p_of;

    IGrab();

    // Get the open file link to this file
    p_of = (T_openFile *)aFile;
    p_fs = p_of->iFS;
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the close command
    error = (*dfs)->Close(p_fs->iFSDeviceWorkspace, p_of->iFileHandle);

    // Regardless of the error, remove it from the open file list
    p_prev_of = 0;
    p_of = G_openFiles;
    while (p_of) {
        // Search for the match
        if (p_of == (T_openFile *)aFile) {
            // Remove the open file
            if (p_prev_of == 0)
                G_openFiles = p_of->iNext;
            else
                p_prev_of->iNext = p_of->iNext;
            break;
        }
        p_prev_of = p_of;
        p_of = p_of->iNext;
    }
    IRelease();

    // Now that the file handle has been removed, delete it
#ifdef NO_DYNAMIC_MEMORY_ALLOC
    p_of->iNext = G_freeFiles;
    G_freeFiles = p_of;
#else
    UEZMemFree(p_of);
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileRead
 *---------------------------------------------------------------------------*/
/**
 *  Read bytes from the current file.
 *
 *  @param [in]    aFile            File to read bytes from
 *
 *  @param [in]    *aBuffer         Pointer to place to store bytes
 *
 *  @param [in]    aNumBytes        Number of bytes to read
 *
 *  @param [in]    *aNumBytesRead      Pointer to number of bytes read, or 0.
 *
 *  @return        T_uezError       UEZ_ERROR_NONE if read, else error code.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *  TUInt8 data[10];
 *  TUInt32 numRead;
 *
 *  if (UEZFileOpen("Filename.txt", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      UEZFileRead(file, data, 10, &numRead);
 *      if (UEZFileClose(file) != UEZ_ERROR_NONE) {
 *          // Error closing file
 *      }
 *  } else {
 *      // An error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileRead(
    T_uezFile aFile,
    void *aBuffer,
    TUInt32 aNumBytes,
    TUInt32 *aNumBytesRead)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_openFile *p_of;

    IGrab();

    // Get the open file link to this file
    p_of = (T_openFile *)aFile;
    p_fs = p_of->iFS;
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the read command
    error = (*dfs)->Read(p_fs->iFSDeviceWorkspace, p_of->iFileHandle, aBuffer,
        aNumBytes, aNumBytesRead);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write bytes to the current file.
 *
 *  @param [in]    aFile             File to read bytes from
 *
 *  @param [in]    *aBuffer          Pointer to place with bytes
 *
 *  @param [in]    aNumBytes            Number of bytes to write
 *
 *  @param [in]    *aNumBytesWritten Pointer to number of bytes written, or 0
 *
 *  @return        T_uezError        UEZ_ERROR_NONE if written, else error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *  TUInt8 data[9] = "Test Data";
 *  TUInt32 numWritten;
 *
 *  if (UEZFileOpen("FILENAME.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      UEZFileWrite(file, data, 9, &numWritten);
 *      if (UEZFileClose(file) != UEZ_ERROR_NONE) {
 *          // Error closing file
 *      }
 *  } else {
 *      // An error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileWrite(
    T_uezFile aFile,
    void *aBuffer,
    TUInt32 aNumBytes,
    TUInt32 *aNumBytesWritten)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_openFile *p_of;

    IGrab();

    // Get the open file link to this file
    p_of = (T_openFile *)aFile;
    p_fs = p_of->iFS;
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the write command
    error = (*dfs)->Write(p_fs->iFSDeviceWorkspace, p_of->iFileHandle, aBuffer,
        aNumBytes, aNumBytesWritten);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileDelete
 *---------------------------------------------------------------------------*/
/**
 *  Delete a file.
 *
 *  @param [in]    *aName            Name of file to delete
 *
 *  @return        T_uezError        UEZ_ERROR_NONE if deleted, else error code.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  if (UEZFileDelete("FILENAME.TXT") != UEZ_ERROR_NONE) {
 *      // Error deleting file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileDelete(const char * const aName)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;

    IGrab();

    // Convert the filename to a filesystem
    error = IFindMountFS(aName, &p_fs);
    if (error != UEZ_ERROR_NONE) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the delete command
    error = (*dfs)->Delete(p_fs->iFSDeviceWorkspace, aName);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileFindStart
 *---------------------------------------------------------------------------*/
/**
 *  Start looking for files in the current directory
 *
 *  @param [in]    *aDirectory     Name of directory to look within
 *
 *  @param [in]    *aEntry         Current file entry.
 *
 *  @return        T_uezError   UEZ_ERROR_NOT_AVAILABLE if the drive is not
 *                              mounted.  UEZ_ERROR_NONE if file found.
 *                              UEZ_ERROR_NOT_FOUND if reach the end and
 *                              no files found.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFileEntry entry;
 *
 *  if (UEZFileFindStart("1:/", &entry) == UEZ_ERROR_NONE) {
 *      do {
 *          printf("%c %-16.16s %02d/%02d/%04d %02d:%02d:%02d - %d bytes\n",
 *              (entry.iFileAttributes & UEZ_FILE_ENTRY_ATTR_DIRECTORY)?'D':'F',
 *              entry.iFilename,
 *              entry.iModifiedDate.iMonth,
 *              entry.iModifiedDate.iDay,
 *              entry.iModifiedDate.iYear,
 *              entry.iModifiedTime.iHour,
 *              entry.iModifiedTime.iMinutes,
 *              entry.iModifiedTime.iSeconds,
 *              entry.iFilesize);
 *      } while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
 *  } else {
 *      printf("No files found.\n");
 *  }
 *  UEZFileFindStop(&entry);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileFindStart(
    const char * const aDirectory,
    T_uezFileEntry *aEntry)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;

#ifdef NO_DYNAMIC_MEMORY_ALLOC
    TUInt16 usLoop = 0;
    T_findFileSession *p = NULL;

    while ((xFileFindSessions[usLoop].iFS != NULL)
        && (usLoop < uezfileMAX_FIND_SESSIONS)) {
        usLoop++;
    }
    if (usLoop < uezfileMAX_FIND_SESSIONS) {
        p = &xFileFindSessions[usLoop];
    }
#else
    T_findFileSession *p = UEZMemAlloc(sizeof(T_findFileSession));
#endif

    if (p == 0)
        return UEZ_ERROR_OUT_OF_MEMORY;

    IGrab();

    // Convert the directory to a filesystem
    error = IFindMountFS(aDirectory, &p_fs);
    if (error != UEZ_ERROR_NONE) {
        p->iFS = 0;
#ifndef NO_DYNAMIC_MEMORY_ALLOC
        UEZMemFree(p);
#endif
        aEntry->iFindFileSession = 0;
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iFS = p_fs;
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;
    aEntry->iFindFileSession = (TUInt32)p;

    // Now do the low level find command
    error = (*dfs)->FindStart(p_fs->iFSDeviceWorkspace, aDirectory, aEntry);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileFindNext
 *---------------------------------------------------------------------------*/
/**
 *  Seek the next file in the list, or return with an error code.
 *
 *  @param [in]    *aEntry         Current file entry.
 *
 *  @return        T_uezError      UEZ_ERROR_NOT_AVAILABLE if the drive is not
 *                                mounted. UEZ_ERROR_NONE if file found.
 *                              UEZ_ERROR_NOT_FOUND if reach the end and
 *                              no files found.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFileEntry entry;
 *
 *  if (UEZFileFindStart("1:/", &entry) == UEZ_ERROR_NONE) {
 *      do {
 *          printf("%c %-16.16s %02d/%02d/%04d %02d:%02d:%02d - %d bytes\n",
 *              (entry.iFileAttributes & UEZ_FILE_ENTRY_ATTR_DIRECTORY)?'D':'F',
 *              entry.iFilename,
 *              entry.iModifiedDate.iMonth,
 *              entry.iModifiedDate.iDay,
 *              entry.iModifiedDate.iYear,
 *              entry.iModifiedTime.iHour,
 *              entry.iModifiedTime.iMinutes,
 *              entry.iModifiedTime.iSeconds,
 *              entry.iFilesize);
 *      } while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
 *  } else {
 *      printf("No files found.\n");
 *  }
 *  UEZFileFindStop(&entry);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileFindNext(T_uezFileEntry *aEntry)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_findFileSession *p;

    IGrab();

    p = (T_findFileSession *)(aEntry->iFindFileSession);
    if (!p) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p_fs = p->iFS;
    if (p_fs) {
        dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

        // Now do find next command
        error = (*dfs)->FindNext(p_fs->iFSDeviceWorkspace, aEntry);
    } else {
        error = UEZ_ERROR_NOT_AVAILABLE;
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileFindStop
 *---------------------------------------------------------------------------*/
/**
 *  Stop looking for files and release related memory.
 *
 *  @param [in]    *aEntry             Current file entry.
 *
 *  @return        T_uezError       UEZ_ERROR_NOT_AVAILABLE if the drive
 *                                  is not mounted.  UEZ_ERROR_NONE if
 *                                  file found.  UEZ_ERROR_NOT_FOUND if
 *                                  reach the end and no files found.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFileEntry entry;
 *
 *  // Search the directory mounted as device "1:/"
 *  if (UEZFileFindStart("1:/", &entry) == UEZ_ERROR_NONE) {
 *      do {
 *          printf("%c %-16.16s %02d/%02d/%04d %02d:%02d:%02d - %d bytes\n",
 *              (entry.iFileAttributes & UEZ_FILE_ENTRY_ATTR_DIRECTORY)?'D':'F',
 *              entry.iFilename,
 *              entry.iModifiedDate.iMonth,
 *              entry.iModifiedDate.iDay,
 *              entry.iModifiedDate.iYear,
 *              entry.iModifiedTime.iHour,
 *              entry.iModifiedTime.iMinutes,
 *              entry.iModifiedTime.iSeconds,
 *              entry.iFilesize);
 *      } while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
 *  } else {
 *      printf("No files found.\n");
 *  }
 *  UEZFileFindStop(&entry);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileFindStop(T_uezFileEntry *aEntry)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_findFileSession *p;

    IGrab();

    p = (T_findFileSession *)(aEntry->iFindFileSession);
    if (!p) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p_fs = p->iFS;
    if (p_fs) {
        dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

        // Now do find next command
        error = (*dfs)->FindStop(p_fs->iFSDeviceWorkspace, aEntry);

        aEntry->iFindFileSession = 0;
    } else {
        error = UEZ_ERROR_NOT_AVAILABLE;
    }

    p->iFS = 0;
#ifndef NO_DYNAMIC_MEMORY_ALLOC
    UEZMemFree(p);
#endif

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileGetLength
 *---------------------------------------------------------------------------*/
/**
 *  Determine the length of an open file
 *
 *  @param [in]    aFile            File to read bytes from
 *
 *  @param [in]    *aLength         Length returned
 *
 *  @return        T_uezError       UEZ_ERROR_NONE if read, else error code.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *  TUInt32 size = 0;
 *
 *  if (UEZFileOpen("FILENAME.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      UEZFileGetLength(file, &size);
 *
 *      // Length of file now in size
 *  } else {
 *      // An error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileGetLength(T_uezFile aFile, TUInt32 *aLength)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_openFile *p_of;

    IGrab();

    // Get the open file link to this file
    p_of = (T_openFile *)aFile;
    p_fs = p_of->iFS;
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the read command
    error = (*dfs)->GetLength(p_fs->iFSDeviceWorkspace, p_of->iFileHandle,
        aLength);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileTellPosition
 *---------------------------------------------------------------------------*/
/**
 *  Look up the given file handle and tell what position the file
 *      is within the file.
 *
 *  @param [in]    aFile             File to read bytes from
 *
 *  @param [in]    *aPosition        Pointer to variable to receive position
 *
 *  @return        T_uezError        Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *  TUInt32 position = 0;
 *
 *  if (UEZFileOpen("Filename.txt", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      UEZFileTellPosition(file, &position);
 *
 *      // Position of the file is now in position
 *  } else {
 *      // An error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileTellPosition(T_uezFile aFile, TUInt32 *aPosition)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_openFile *p_of;

    IGrab();

    // Get the open file link to this file
    p_of = (T_openFile *)aFile;
    p_fs = p_of->iFS;
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the read command
    error = (*dfs)->TellPosition(p_fs->iFSDeviceWorkspace, p_of->iFileHandle,
        aPosition);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFileSeekPosition
 *---------------------------------------------------------------------------*/
/**
 *  Seek the position in the file at an offset from the beginning of the file.
 *
 *  @param [in]    aFile              File to read bytes from
 *
 *  @param [in]    aPosition        Position to jump to
 *
 *  @return        T_uezError          Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  T_uezFile file;
 *  TUInt32 position = 100;
 *
 *  if (UEZFileOpen("Filename.txt", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
 *      // The file opened properly
 *      UEZFileSeekPosition(file, position);
 *
 *      // Now at position 100
 *  } else {
 *      // An error occurred opening the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileSeekPosition(T_uezFile aFile, TUInt32 aPosition)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;
    T_openFile *p_of;

    IGrab();

    // Get the open file link to this file
    p_of = (T_openFile *)aFile;
    p_fs = p_of->iFS;
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the read command
    error = (*dfs)->SeekPosition(p_fs->iFSDeviceWorkspace, p_of->iFileHandle,
        aPosition);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZFileRename
*---------------------------------------------------------------------------*/
/**
 *  Rename an existing file.
 *
 *  @param [in]    *aOldFilename     File old name
 *
 *  @param [in]    *aNewFilename    File new name
 *
 *  @return        T_uezError        Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  if (UEZFileRename("OLDNAME.TXT", "NEWNAME.TXT") == UEZ_ERROR_NONE) {
 *      // The file renamed correctly
 *  } else {
 *      // An error occurred renaming the file
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileRename(
    const char * const aOldFilename,
    const char * const aNewFilename)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_mountedFS *p_fs2;
    DEVICE_FileSystem **dfs2;
    T_uezError error;
    const char* pOld = aOldFilename;
    const char* pNew = aNewFilename;

    IGrab();

    // Convert the filename to a filesystem
    error = IFindMountFS(aOldFilename, &p_fs);
    if (error != UEZ_ERROR_NONE) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Convert the new filename to a filesystem
    error = IFindMountFS(aNewFilename, &p_fs2);
    if (error != UEZ_ERROR_NONE) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    dfs2 = (DEVICE_FileSystem **)p_fs2->iFSDeviceWorkspace;

    // MUST be the same file system (otherwise we need to do a copy)
    if (dfs != dfs2) {
        IRelease();
        return UEZ_ERROR_MISMATCH;
    }

    if (pNew[1] == ':')
        pNew += 2; //move pointer beyond drive spec
    if (pNew[0] == '/')
        pNew += 1; //move pointer beyond '/'

    // Now do the Rename command
    error = (*dfs)->Rename(p_fs->iFSDeviceWorkspace, pOld,
        pNew);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZFileMakeDirectory
*---------------------------------------------------------------------------*/
/**
 *  Make a directory at the given location.  The parent directory
 *  must already exist.
 *
 *  @param [in]    *aDiretoryName     Dir new name
 *
 *  @return        T_uezError         Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFile.h>
 *
 *  if (UEZFileMakeDirectory("NewDirName") == UEZ_ERROR_NONE) {
 *      // Created new directory successfully
 *  } else {
 *      // An error occurred creating the new directory
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFileMakeDirectory(const char * const aDiretoryName)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;

    IGrab();

    // Convert the filename to a filesystem
    error = IFindMountFS(aDiretoryName, &p_fs);
    if (error != UEZ_ERROR_NONE) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the delete command
    error = (*dfs)->MakeDirectory(p_fs->iFSDeviceWorkspace, aDiretoryName);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZFileSystemSync
*---------------------------------------------------------------------------*
* Description:
*      Write the data that needs to be saved based on the given drive path.
* Inputs:
  *      const char * const aDrivePath -- Directory to drive to sync
* Outputs:
*      T_uezError               -- Error code, if any. UEZ_ERROR_OK if
 *                                  successful.
*---------------------------------------------------------------------------*/
T_uezError UEZFileSystemSync(const char * const aDrivePath)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;

    IGrab();

    // Convert the filename to a filesystem
    error = IFindMountFS(aDrivePath, &p_fs);
    if (error != UEZ_ERROR_NONE) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the sync command
    error = (*dfs)->Sync(p_fs->iFSDeviceWorkspace, aDrivePath);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZFileSystemGetVolumeInfo
*---------------------------------------------------------------------------*
* Description:
*      Get information about the drive volume at the given path.
* Inputs:
  *      const char * const aDrivePath -- Directory to drive to sync
  *      T_uezFileSystemVolumeInfo *aInfo -- Structure to receive volume
  *                             information
* Outputs:
*      T_uezError               -- Error code, if any. UEZ_ERROR_OK if
 *                                  successful.
*---------------------------------------------------------------------------*/
T_uezError UEZFileSystemGetVolumeInfo(
        const char * const aDrivePath,
        T_uezFileSystemVolumeInfo *aInfo)
{
    T_mountedFS *p_fs;
    DEVICE_FileSystem **dfs;
    T_uezError error;

    IGrab();

    // Convert the filename to a filesystem
    error = IFindMountFS(aDrivePath, &p_fs);
    if (error != UEZ_ERROR_NONE) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    dfs = (DEVICE_FileSystem **)p_fs->iFSDeviceWorkspace;

    // Now do the get info command
    error = (*dfs)->GetVolumeInfo(p_fs->iFSDeviceWorkspace, aDrivePath, aInfo);

    IRelease();

    return error;
}

/** @} */

/*-------------------------------------------------------------------------*
 * End of File:  uEZFile.c
 *-------------------------------------------------------------------------*/
