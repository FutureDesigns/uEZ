/*-------------------------------------------------------------------------*
 * File:  MCI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef MCI_H_
#define MCI_H_

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
#include <uEZTypes.h>
#include <HAL/HAL.h>
#include <Types/MCI.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_mciReceptionComplete)(void *aCallbackWorkspace);
typedef void (*T_mciTransmissionComplete)(void *aCallbackWorkspace);
typedef void (*T_mciError)(void *aCallbackWorkspace);

typedef enum {
    UEZ_MCI_BUS_1BIT_WIDE = 0,
    UEZ_MCI_BUS_4BIT_WIDE
} T_uezMCIBus;

// TODO: Comment this API!

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Common header
    T_halInterface iInterface;

    // Functions (uEZ v2.04)
    T_uezError (*PowerOn)(void *aWorkspace);
    T_uezError (*PowerOff)(void *aWorkspace);
    T_uezError (*Reset)(void *aWorkspace);
    TBool (*SendCommand)(
        void *aWorkspace,
        TUInt16 aCommand,
        TUInt32 aArgument,
        T_uezMCIResponseType aResponse,
        TUInt32 *buffer);
    T_uezError (*ReadyReception)(
        void *aWorkspace,
        TUInt32 aNumBlocks,
        TUInt32 aBlockSize,
        T_mciReceptionComplete aReceptionCallback,
        void *aCallbackWorkspace,
        TUInt32 *aNumBlocksReady);
    T_uezError (*StopTransfer)(void *aWorkspace);
    T_uezError (*Read)(void *aWorkspace, void *aBuffer, TUInt32 aNumBytes);
    T_uezError (*SetRelativeCardAddress)(void *aWorkspace, TUInt32 aRCA);
    T_uezError (*SetClockRate)(void *aWorkspace, TUInt32 aHz, TBool aPowerSave);
    T_uezError (*SetDataBus)(void *aWorkspace, T_uezMCIBus aBus);

    T_uezError (*ReadyTransmission)(
        void *aWorkspace,
        TUInt32 aNumBlocks,
        TUInt32 aBlockSize,
        TUInt32 *aNumWriting);
    T_uezError (*StartTransmission)(
        void *aWorkspace,
        T_mciTransmissionComplete aTransmissionCallback,
        void *aCallbackWorkspace);
    T_uezError (*Write)(
        void *aWorkspace,
        const TUInt8 *aBuffer,
        TUInt32 aNumBytes);
    TBool (*IsWriteAvailable)(void *aWorkspace);
    TBool (*IsWriteEmpty)(void *aWorkspace);

    // uEZ 2.11.1
    T_uezError (*SetErrorCallback)(
        void *aWorkspace,
        T_mciError aErrorCallback,
        void *aCallbackWorkspace);
} HAL_MCI;

#ifdef __cplusplus
}
#endif

#endif // MCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  MCI.h
 *-------------------------------------------------------------------------*/
