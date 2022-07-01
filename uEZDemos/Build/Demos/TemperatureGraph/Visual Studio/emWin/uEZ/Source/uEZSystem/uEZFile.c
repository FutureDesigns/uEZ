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
   

    return UEZ_ERROR_NONE;
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


    return UEZ_ERROR_NONE;
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


    return UEZ_ERROR_NONE;
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


    return UEZ_ERROR_NONE;
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


    return UEZ_ERROR_NONE;
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


    return UEZ_ERROR_NONE;
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
    

    return UEZ_ERROR_NONE;
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
    

    return UEZ_ERROR_NONE;
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
    

    return UEZ_ERROR_NONE;
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
   

    return UEZ_ERROR_NONE;
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
    

    return UEZ_ERROR_NONE;
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
    

    return UEZ_ERROR_NONE;
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
    

    return UEZ_ERROR_NONE;
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
   
    return UEZ_ERROR_NONE;
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
    

    return UEZ_ERROR_NONE;
}

/** @} */

/*-------------------------------------------------------------------------*
 * End of File:  uEZFile.c
 *-------------------------------------------------------------------------*/
