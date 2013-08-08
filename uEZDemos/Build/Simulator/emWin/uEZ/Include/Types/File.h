/*-------------------------------------------------------------------------*
 * File:  Types/File.h
 *-------------------------------------------------------------------------*
 * Description:
 *     File HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _File_TYPES_H_
#define _File_TYPES_H_

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
    TUInt8 iHour;           /** 0 - 23 */
    TUInt8 iMinutes;        /** 0 - 59 */
    TUInt8 iSeconds;        /** 0 - 59 */
    TUInt8 iFineSeconds;    /** 0 - 100 multiples of 10 ms */
} T_uezFileTime;

typedef struct {
    TUInt16 iYear;          /** 0 - 9999 */
    TUInt8 iMonth;          /** 1 - 12 */
    TUInt8 iDay;            /** 1 - 31 */
} T_uezFileDate;

typedef struct {
    char iFilename[256+1];     /** Complete filename */
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

#endif // _File_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/File.h
 *-------------------------------------------------------------------------*/
