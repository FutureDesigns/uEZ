/*-------------------------------------------------------------------------*
* File:  uEZStream.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/

#ifndef _UEZ_STREAM_H_
#define _UEZ_STREAM_H_

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
 *  @file   uEZStream.h
 *  @brief  uEZ Stream
 *
 *  uEZ Stream
 */
#include "uEZTypes.h"
#include "uEZErrors.h"
#include <Device/Stream.h>

#ifdef __cplusplus
extern "C" {
#endif

T_uezError UEZStreamOpen(
        const char * const aName,
        T_uezDevice *aDevice);
T_uezError UEZStreamClose(T_uezDevice aDevice);
T_uezError UEZStreamWrite(
        T_uezDevice aDevice,
        void *aData,
        TUInt32 aNumBytes,
        TUInt32* aNumBytesWritten,
        TUInt32 aTimeout);
T_uezError UEZStreamRead(
        T_uezDevice aDevice,
        void *aData,
        TUInt32 aNumBytes,
        TUInt32* aNumBytesRead,
        TUInt32 aTimeout);
T_uezError UEZStreamControl(
        T_uezDevice aDevice,
        TUInt32 aControl,
        void *aControlData);
T_uezError UEZStreamFlush(T_uezDevice aDevice);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_STREAM_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZStream.h
 *-------------------------------------------------------------------------*/
