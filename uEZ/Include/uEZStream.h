#ifndef _UEZ_STREAM_H_
#define _UEZ_STREAM_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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
 *  @file   uEZStream.h
 *  @brief  uEZ Stream
 *
 *  uEZ Stream
 */
#include "uEZTypes.h"
#include "uEZErrors.h"
#include <Device/Stream.h>

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

#endif // _UEZ_STREAM_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZStream.h
 *-------------------------------------------------------------------------*/
