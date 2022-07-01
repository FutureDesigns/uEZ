/*-------------------------------------------------------------------------*
 * File:  IntelHex.c
 *-------------------------------------------------------------------------*
 * Description:
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "uEZCompat.h"
#include "IntelHexParser.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static TUInt32 IHexValue(char *aString, TUInt8 aLength)
{
    char c;
    TUInt32 value = 0;
    while (aLength--) {
        c = *(aString++);
        value = (value << 4) |
                (((c & 0x40) ? 9 /*letter*/: 0 /*digit*/) + (c & 0x0F));
    }
    return value;
}

/*---------------------------------------------------------------------------*
 * Routine:  IntelHexParserInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the Intel hex parser workspace
 * Inputs:
 *      T_IntelHexParserWorkspace *aWorkspace -- Workspace with state and
 *                                  output information.
 *---------------------------------------------------------------------------*/
void IntelHexParserInit(T_IntexHexParserWorkspace *aWorkspace)
{
    T_IntexHexParserWorkspace *p = aWorkspace;

    p->iSegment = 0;
    p->iExtendedAddress = 0;
    p->iDataLength = 0;
    p->iIsEnd = EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  IntelHexParserString
 *---------------------------------------------------------------------------*
 * Description:
 *      Take the given string and parse the data, storing the parsed
 *      data back into the workspace.  String must start with ':'.
 * Inputs:
 *      T_IntelHexParserWorkspace *aWorkspace -- Workspace with state and
 *                                  output information.
 *      char *aString -- String to process
 *---------------------------------------------------------------------------*/
T_uezError IntelHexParserString(
                T_IntexHexParserWorkspace *aWorkspace,
                char *aString)
{
    TUInt16 address;
    TUInt8 record;
    TUInt16 i;
    TUInt8 checksum;
    char *pc;
    TUInt16 dataLength;

    T_IntexHexParserWorkspace *p = aWorkspace;
    p->iDataLength = 0;

    // Must start with a ':' to parse
    if (*aString != ':')
        return UEZ_ERROR_INVALID;

    // We have no data until we get a data record
    p->iDataLength = 0;
    dataLength = (TUInt16)IHexValue(aString+1, 2);
    address = (TUInt16)IHexValue(aString+3, 4);
    record = (TUInt8)IHexValue(aString+7, 2);
    checksum = dataLength + (address & 0xFF) + (address >> 8) + record;
    pc = aString+9;

    switch (record)  {
        case 0:
            // Data record
            p->iDataAddress = (p->iSegment << 4) + (p->iExtendedAddress
                    << 16) + address;
            p->iDataLength = dataLength;
            for (i = 0; i < p->iDataLength; i++, pc += 2) {
                // Data get cut off?
                if ((pc[0] == '\0') || (pc[1] == '\0'))
                    return UEZ_ERROR_OUT_OF_DATA;

                // Store the byte for that entry
                checksum += (p->iData[i] = (TUInt8)IHexValue(pc, 2));
            }

            // Get last byte, the 2's compliment checksum that should make the
            // total be zero.
            if ((pc[0] == '\0') || (pc[1] == '\0'))
                return UEZ_ERROR_OUT_OF_DATA;
            checksum += (TUInt8)IHexValue(pc, 2);
            if (checksum != 0)
                return UEZ_ERROR_CHECKSUM_BAD;
            break;
        case 1:
            // End of hex file
            p->iIsEnd = ETrue;
            break;
        case 2:
            // Change the current segment
            p->iSegment = IHexValue(pc, 4);
            break;
        case 4:
            // Get the extended address
            p->iExtendedAddress = IHexValue(pc, 4);
            break;
    }
    return UEZ_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
 * End of File:  IntelHex.c
 *-------------------------------------------------------------------------*/
