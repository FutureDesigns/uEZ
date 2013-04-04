/**
 *	@file 	uEZSPI.h
 *  @brief 	uEZ SPI Interface
 *
 *	The uEZ interface which maps to low level SPI drivers.
 */
#ifndef _UEZ_SPI_H_
#define _UEZ_SPI_H_

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

#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/SPI.h>

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

#endif // _UEZ_SPI_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZSPI.h
 *-------------------------------------------------------------------------*/
