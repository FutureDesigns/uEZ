/*-------------------------------------------------------------------------*
* File:  uEZSPI.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level SPI drivers.
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZSPI.h
 *  @brief 	uEZ SPI Interface
 *
 *	The uEZ interface which maps to low level SPI drivers.
 */
#ifndef _UEZ_SPI_H_
#define _UEZ_SPI_H_

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
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/SPI.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Open up access to the SPI bank.
 *
 *	@param [in] 	*aName		Pointer to name of SPI bus (usually
 *                          "SPIx" where x is bus number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZSPIOpen(
            const char *const aName, 
            T_uezDevice *aDevice);
						
/**
 *	End access to the SPI bank.
 *
 *	@param [in]	aDevice		SPI Device handle to close
 *
 *	@return		T_uezError
 */
T_uezError UEZSPIClose(T_uezDevice aDevice);

/**
 *	Requests a SPI polled transfer (both read and write).
 *
 *	@param [in]			aDevice			SPI Device handle to close
 *	@param [in,out]	*aRequest		Structure of read and/or write SPI
 *                          		transaction information.
 *
 *	@return		T_uezError
 */
T_uezError UEZSPITransferPolled(T_uezDevice aDevice, SPI_Request *aRequest);

// TODO: Comments!
T_uezError UEZSPITransferNoBlock(
        T_uezDevice aDevice,
        SPI_Request *aRequest,
        T_spiCompleteCallback aCallback,
        void *aCallbackWorkspace);
TBool UEZSPIIsBusy(T_uezDevice aDevice);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_SPI_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZSPI.h
 *-------------------------------------------------------------------------*/
