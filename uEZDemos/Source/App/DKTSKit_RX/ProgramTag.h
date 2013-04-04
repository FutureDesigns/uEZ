/*-------------------------------------------------------------------------*
 * File:  ProgramTag.h
 *-------------------------------------------------------------------------*
 | Description:
 |      Tag for firmware that boot loader will use
 *-------------------------------------------------------------------------*/
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

#ifndef _PROGRAM_TAG_H_
#define _PROGRAM_TAG_H_

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

#endif //_PROGRAM_TAG_H_

/*-------------------------------------------------------------------------*
 * File:  ProgramTag.h
 *-------------------------------------------------------------------------*/

