/*-------------------------------------------------------------------------*
 * File:  Types/SPI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     SPI HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _SPI_TYPES_H_
#define _SPI_TYPES_H_

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
/**
 *  @file   /Include/Types/SPI.h
 *  @brief  uEZ SPI Types
 *
 *  The uEZ SPI Types
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
#include <HAL/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    /** Pointer to buffer of data being sent (Master/CPU -> Slave/Device)
     * Pass a value of 0 if 0xFF is to be sent for all data (all high bits) */
    const void *iDataMOSI;

    /** Pointer to buffer to receive data (Master/CPU <- Slave/Device)
     * Pass a value of 0 if no data is to be received */
    void *iDataMISO;

    /** Number of transfers to go both through data in and data out */
    TUInt32 iNumTransfers;

    /** 4-16 bits per transfer.  If greater than 8 bits, data MISO
     *  and data MOSI point to 16 bit words. */
    TUInt8 iBitsPerTransfer;

    /** Clock rate in kHz (e.g. 2500 for 2.5 MHz, recommend 1.0 MHz min) */
    TUInt32 iRate;

    /** Clock polarity - ETrue=high between frames, EFalse=low between frames */
    TBool iClockOutPolarity;

    /** Clock phase - ETrue=data on second clock transition, EFalse=data on
     * first clock transition. */
    TBool iClockOutPhase;

    /** Chip select configuration
     * GPIOPort and bit position (1<<n) */
    HAL_GPIOPort **iCSGPIOPort;

    /** Chip Select bit mask for the pin (1<<n) */
    TUInt32 iCSGPIOBit; 

    /** Chip select polarity - ETrue=high true, EFalse=low true */
    TBool iCSPolarity;

    /** Number of transfers outgoing completed [read only status] */
    TUInt32 iNumTransferredOut;

    /** Number of transfers incoming completed [read only status] */
    TUInt32 iNumTransferredIn;

    TUInt8 iFlags;
    	#define SPI_REQUEST_TOGGLING_CS  	(1<<0) // Toggling CS
} SPI_Request;

typedef void (*T_spiCompleteCallback)(void *aWorkspace, SPI_Request *aRequest);

#ifdef __cplusplus
}
#endif

#endif // _SPI_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/SPI.h
 *-------------------------------------------------------------------------*/
