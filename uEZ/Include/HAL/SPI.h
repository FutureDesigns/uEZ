/*-------------------------------------------------------------------------*
 * File:  SPI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     SPI HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
 *  @file   /Include/HAL/SPI.h
 *  @brief  uEZ SPI HAL Interface
 *
 *  The uEZ SPI HAL Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <HAL/HAL.h>
#include <Types/SPI.h>

/*-------------------------------------------------------------------------*
 * HAL Interface:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*TransferPolled)(void *aWorkspace, SPI_Request *aRequest);

    void (*Start)(void *aWorkspace, SPI_Request *aRequest);
    TUInt32 (*TransferInOut)(
                    void *aWorkspace, 
                    SPI_Request *aRequest,
                    TUInt32 aNum);    
    void (*Stop)(void *aWorkspace, SPI_Request *aRequest);
    T_uezError (*TransferInOutBytes)(void *aWorkspace, SPI_Request *aRequest);

    // uEZ v2.02
    T_uezError (*TransferNoBlock)(
        void *aWorkspace,
        SPI_Request *aRequest,
        T_spiCompleteCallback aCallback,
        void *aCallbackWorkspace);
    TBool (*IsBusy)(void *aWorkspace);
} HAL_SPI;

#endif // _HAL_SPI_H_
/*-------------------------------------------------------------------------*
 * End of File:  SPI.h
 *-------------------------------------------------------------------------*/
