/*-------------------------------------------------------------------------*
 * File:  ProgramTag.h
 *-------------------------------------------------------------------------*
 | Description:
 |      Tag for firmware that boot loader will use
 *-------------------------------------------------------------------------*/
#ifndef _PROGRAM_TAG_H_
#define _PROGRAM_TAG_H_

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

#ifdef __cplusplus
extern "C" {
#endif

#define PROGRAM_TAG_SPECIAL_HEADER  0x50524768  //"PRGh"
#define PROGRAM_TAG_SPECIAL_TAIL    0x74505247  //"tPRG"

#define PROGRAM_TAG_SIZE_UPPER_LIMIT        131072 // 128k
#define PROGRAM_TAG_SIZE_LOWER_LIMIT        256

// Total size of tag is 256 bytes:

#if (COMPILER_TYPE==IAR)
#pragma pack(push, 1)
typedef struct {
    TUInt32 iSpecialHeader;
    char iName[64];
    char iVersion[16];
    char iDate[64];
    // Size must be:
    //   PROGRAM_TAG_SIZE_LOWER_LIMIT < sz <= PROGRAM_TAG_SIZE_UPPER_LIMIT
    TUInt32 iSize;
    TUInt32 iChecksum;
    char iReserved[96];
    TUInt32 iSpecialTail;
} T_programTag ;
extern const T_programTag G_programTag;
#pragma pack(pop)
#endif

#if (COMPILER_TYPE==RowleyARM)
typedef struct __attribute__ ((__packed__)) {
    TUInt32 iSpecialHeader;
    char iName[64];
    char iVersion[16];
    char iDate[64];
    // Size must be:
    //   PROGRAM_TAG_SIZE_LOWER_LIMIT < sz <= PROGRAM_TAG_SIZE_UPPER_LIMIT
    TUInt32 iSize;
    TUInt32 iChecksum;
    char iReserved[96];
    TUInt32 iSpecialTail;
} T_programTag ;
extern const T_programTag G_programTag __attribute__((section("PROGRAM_TAG")));
#endif

//extern const T_programTag G_programTag;

#ifdef __cplusplus
}
#endif

#endif //_PROGRAM_TAG_H_

/*-------------------------------------------------------------------------*
 * File:  ProgramTag.h
 *-------------------------------------------------------------------------*/

