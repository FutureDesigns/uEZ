/*-------------------------------------------------------------------------*
 * File:  FDICmd.h
 *-------------------------------------------------------------------------*
 * Description:
 *     FDI Command console
 *-------------------------------------------------------------------------*/
#ifndef _FDICMD_H_
#define _FDICMD_H_

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
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
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

#ifdef __cplusplus
}
#endif

#endif // _FDICMD_H_
/*-------------------------------------------------------------------------*
 * End of File:  FDICmd.h
 *-------------------------------------------------------------------------*/
