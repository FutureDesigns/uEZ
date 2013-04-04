/*-------------------------------------------------------------------------*
 * File:  SPIBus.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ SPI Bus Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_SPI_BUS_H_
#define _DEVICE_SPI_BUS_H_

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
 *  @file   /Include/Device/SPIBus.h
 *  @brief  uEZ SPI Bus Device Interface
 *
 *  The uEZ SPI Bus Device Interface
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
#include <uEZDevice.h>
#include <HAL/SPI.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

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
} DEVICE_SPI_BUS;

#endif // _DEVICE_SPI_BUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  SPIBus.h
 *-------------------------------------------------------------------------*/

