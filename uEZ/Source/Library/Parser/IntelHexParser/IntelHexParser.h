/*-------------------------------------------------------------------------*
 * File:  IntelHex.h
 *-------------------------------------------------------------------------*/
#ifndef INTELHEXPARSER_H_
#define INTELHEXPARSER_H_

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


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef INTEL_HEX_PARSER_MAX_DATA_LENGTH
    #define INTEL_HEX_PARSER_MAX_DATA_LENGTH        256
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        // Parsed internal state
        TUInt32 iSegment;
        TUInt32 iExtendedAddress;

        // Parsed output
        TUInt32 iDataAddress;
        TUInt16 iDataLength;
        TUInt8 iData[INTEL_HEX_PARSER_MAX_DATA_LENGTH];
        TBool iIsEnd;
} T_IntexHexParserWorkspace;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void IntelHexParserInit(T_IntexHexParserWorkspace *aWorkspace);

T_uezError IntelHexParserString(
                T_IntexHexParserWorkspace *aWorkspace,
                char *aString);

#ifdef __cplusplus
}
#endif

#endif // INTELHEXPARSER_H_
/*-------------------------------------------------------------------------*
 * End of File:  IntelHexParser.h
 *-------------------------------------------------------------------------*/
