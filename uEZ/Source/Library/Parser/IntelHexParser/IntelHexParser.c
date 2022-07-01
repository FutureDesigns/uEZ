/*-------------------------------------------------------------------------*
 * File:  IntelHexParser.c
 *-------------------------------------------------------------------------*/
/*
 *  @{
 *  @brief     uEZ Intel Hex Parser
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    uEZ Intel Hex Parser
 */
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include "IntelHexParser.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Routine:  IntelHexParserString
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]    *aString		Pointer to string to process
 *
 *  @param [in]    *aLength		Length of string
 *
 *  @return        TUInt32		return value
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
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
 *---------------------------------------------------------------------------*/
/**
 *  Initialize the Intel hex parser workspace
 *
 *  @param [in]    *aWorkspace	Workspace with state and output information.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
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
 *---------------------------------------------------------------------------*/
/**
 *  Take the given string and parse the data, storing the parsed
 *      data back into the workspace.  String must start with ':'.
 *
 *  @param [in]    *aWorkspace	Pointer to workspace with state and
 *                                  output information.
 *
 *  @param [in]    *aString		Pointer to string to process
 *
 *  @return        T_uezError	return UEZ_ERROR_NONE if ok
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError IntelHexParserString(
                T_IntexHexParserWorkspace *aWorkspace,
                char *aString)
{
    TUInt16 address;
    TUInt8 record;
    TUInt16 i;
    TUInt8 checksum;
    char *pc;

    T_IntexHexParserWorkspace *p = aWorkspace;
    p->iDataLength = 0;

    // Must start with a ':' to parse
    if (*aString != ':')
        return UEZ_ERROR_INVALID;

    p->iDataLength = (TUInt16)IHexValue(aString+1, 2);
    address = (TUInt16)IHexValue(aString+3, 4);
    record = (TUInt8)IHexValue(aString+7, 2);
    checksum = p->iDataLength + (address & 0xFF) + (address >> 8) + record;
    pc = aString+9;

    switch (record)  {
        case 0:
            // Data record
            p->iDataAddress = (p->iSegment << 4) + (p->iExtendedAddress
                    << 16) + address;
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
 * End of File:  IntelHexParser.c
 *-------------------------------------------------------------------------*/
