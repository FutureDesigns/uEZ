/*-------------------------------------------------------------------------*
 * File:  IntelHex.h
 *-------------------------------------------------------------------------*/
#ifndef INTELHEXPARSER_H_
#define INTELHEXPARSER_H_

/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2011 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 * 	-   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 * 	-   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 	-   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * 	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * 	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * 	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * 	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * 	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * 	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * 	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

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

#endif // INTELHEXPARSER_H_
/*-------------------------------------------------------------------------*
 * End of File:  IntelHexParser.h
 *-------------------------------------------------------------------------*/
