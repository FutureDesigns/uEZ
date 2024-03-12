/*-------------------------------------------------------------------------*
 * File:  SD_MMC.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef SD_MMC_H_
#define SD_MMC_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
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
#include <Types/SD_MMC.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_SD_MMCReceptionComplete)(void *aCallbackWorkspace);
typedef void (*T_SD_MMCTransmissionComplete)(void *aCallbackWorkspace);
typedef void (*T_SD_MMCError)(void *aCallbackWorkspace);

/*typedef enum {
    UEZ_SD_MMC_BUS_1BIT_WIDE = 0,
    UEZ_SD_MMC_BUS_4BIT_WIDE
} T_uezSD_MMCBus;*/

typedef enum {
    UEZ_MCI_BUS_1BIT_WIDE = 0,
    UEZ_MCI_BUS_4BIT_WIDE
} T_uezMCIBus;

// TODO: Comment this API!

// Note: This is different from the MCI.h HAL!

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
    TUInt32 (*ExecuteCommand)(
                void *aWorkspace,
                TUInt32 aCommand,
                TUInt32 aArgument,
                TUInt32 aWaitStatus,
                TUInt32 *aResponse);
    T_uezError (*ReadyReception)(
        void *aWorkspace,
        TUInt8 *aBuffer,
        TUInt32 aNumBlocks,
        TUInt32 aBlockSize,
        T_SD_MMCReceptionComplete aReceptionCallback,
        void *aCallbackWorkspace,
        TUInt32 *aNumBlocksReady);
    T_uezError (*SetRelativeCardAddress)(void *aWorkspace, TUInt32 aRCA);
    T_uezError (*SetClockRate)(void *aWorkspace, TUInt32 aHz);

    T_uezError (*ReadyTransmission)(
        void *aWorkspace,
        const TUInt8 *aBuffer,
        TUInt32 aNumBlocks,
        TUInt32 aBlockSize,
        TUInt32 *aNumWriting,
        T_SD_MMCTransmissionComplete aTransmissionCallback,
        void *aTransmissionCallbackWorkspace);
    void (*SetCardType)(void *aWorkspace, TUInt32 aType);
    void (*SetBlockSize)(void *aWorkspace, TUInt32 aBytesPerBlock);
    void (*PrepreExtCSDTransfer)(void *aWorkspace, TUInt8 *aAddress, TUInt32 aTransferSize);
    
    // 2.12
    void (*SetupErrorCallback) (void *aWorkspace,
         T_SD_MMCError aErrorCallback,
         void *aErrorCallbackWorkspace);

    // 2.14
    TBool (*IsCardInserted) (void *aWorkspace);
} HAL_SD_MMC;

#endif // SD_MMC_H_
/*-------------------------------------------------------------------------*
 * End of File:  SD_MMC.h
 *-------------------------------------------------------------------------*/
