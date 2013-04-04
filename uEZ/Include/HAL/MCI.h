/*-------------------------------------------------------------------------*
 * File:  MCI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef MCI_H_
#define MCI_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <HAL/HAL.h>
#include <Types/MCI.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_mciReceptionComplete)(void *aCallbackWorkspace);
typedef void (*T_mciTransmissionComplete)(void *aCallbackWorkspace);

typedef enum {
    UEZ_MCI_BUS_1BIT_WIDE,
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
} HAL_MCI;

#endif // MCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  MCI.h
 *-------------------------------------------------------------------------*/
