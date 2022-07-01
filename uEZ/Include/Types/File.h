/*-------------------------------------------------------------------------*
 * File:  Types/File.h
 *-------------------------------------------------------------------------*
 * Description:
 *     File HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _File_TYPES_H_
#define _File_TYPES_H_

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
 *  @file   /Include/Types/File.h
 *  @brief  uEZ File Types
 *
 *  The uEZ File Types
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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define FILE_FLAG_READ_ONLY             1
#define FILE_FLAG_APPEND                2
#define FILE_FLAG_WRITE                 3

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
#define UEZ_FILE_ENTRY_ATTR_DIRECTORY       (1<<0)
#define UEZ_FILE_ENTRY_ATTR_READ_ONLY       (1<<1)
#define UEZ_FILE_ENTRY_ATTR_SYSTEM          (1<<2)
#define UEZ_FILE_ENTRY_ATTR_HIDDEN          (1<<3)
#define UEZ_FILE_ENTRY_ATTR_ARCHIVE         (1<<4)

typedef struct {
    TUInt8 iHour;       	/** 0 - 23 */
    TUInt8 iMinutes;    	/** 0 - 59 */
    TUInt8 iSeconds;    	/** 0 - 59 */
    TUInt8 iFineSeconds;    /** 0 - 100 multiples of 10 ms */
} T_uezFileTime;

typedef struct {
    TUInt16 iYear;      	/** 0 - 9999 */
    TUInt8 iMonth;      	/** 1 - 12 */
    TUInt8 iDay;        	/** 1 - 31 */
} T_uezFileDate;

typedef struct {
    char iFilename[256+1];  /** Complete filename */
    TUInt8 iFileAttributes;
    TUInt32 iFilesize;
    T_uezFileDate iCreateDate;
    T_uezFileTime iCreateTime;
    T_uezFileDate iAccessDate;
    T_uezFileTime iAccessTime;
    T_uezFileDate iModifiedDate;
    T_uezFileTime iModifiedTime;
    TUInt32 iFindFileSession;
    TUInt32 iFindFileSessionDevice;
} T_uezFileEntry;

typedef struct {
    /** Size in bytes of a single sector in the volume */
    TUInt32 iBytesPerSector;

    /** Size of a cluster in number of sectors */
    TUInt32 iSectorsPerCluster;

    /** Number of clusters (of sectors) total */
    TUInt32 iNumClustersTotal;

    /** Number of clusters (of sectors) free */
    TUInt32 iNumClustersFree;
} T_uezFileSystemVolumeInfo;

#ifdef __cplusplus
}
#endif

#endif // _File_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/File.h
 *-------------------------------------------------------------------------*/
