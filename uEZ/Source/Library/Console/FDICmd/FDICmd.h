/*-------------------------------------------------------------------------*
 * File:  FDICmd.h
 *-------------------------------------------------------------------------*
 * Description:
 *     FDI Command console
 *-------------------------------------------------------------------------*/
#ifndef _FDICMD_H_
#define _FDICMD_H_

/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2008 Future Designs, Inc. All Rights Reserved.
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
#ifndef FDICMD_MAX_LINE_LENGTH
    #define FDICMD_MAX_LINE_LENGTH 78
#endif
#ifndef FDICMD_PRINTF_BUFFER_SIZE
    #define FDICMD_PRINTF_BUFFER_SIZE 512 // bytes in stack
#endif
#ifndef FDICMD_MAX_PARAMETERS
    #define FDICMD_MAX_PARAMETERS  10
#endif


/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef int (*T_consoleCmd)(void *aWorkspace, int argc, char *argv[]);
typedef struct {
    char *iName;    // 0 = last entry in array
    T_consoleCmd iFunction;
} T_consoleCmdEntry;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError FDICmdStart(
    T_uezDevice aStream,
    void **aWorkspace,
    TUInt32 aStackSizeBytes,
    const T_consoleCmdEntry *aCommandTable);
T_uezError FDICmdStop(void *aWorkspace);
T_uezError FDICmdPrintf(void *aWorkspace, const char *aFormat, ...);
T_uezError FDICmdSendString(void *aWorkspace, const char *aString);
int FDICmdProcessCmd(void *aWorkspace, const char *aCmd);
T_uezError FDICmdReadLine(
        void *aWorkspace,
        char *aLine,
        TUInt32 aMaxLength,
        TUInt32 aTimeout,
        TBool aEcho);

TUInt32 FDICmdUValue(const char *aArg);

#endif // _FDICMD_H_
/*-------------------------------------------------------------------------*
 * End of File:  FDICmd.h
 *-------------------------------------------------------------------------*/
