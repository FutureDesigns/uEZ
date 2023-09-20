/*-------------------------------------------------------------------------*
* File:  uEZFlash.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level Flash drivers.
*-------------------------------------------------------------------------*/
#ifndef _UEZ_Flash_H_
#define _UEZ_Flash_H_

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
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/Flash.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Open up access to the Flash bank.
 *
 *	@param [in] 	*aName		Pointer to name of Flash bus (usually
 *                          "Flashx" where x is bus number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashOpen(
            const char *const aName,
            T_uezDevice *aDevice);
						
/**
 *	End access to the Flash bank.
 *
 *	@param [in]	aDevice		Device handle to close
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashClose(T_uezDevice aDevice);

/**
 *	Read bytes out of the Flash.
 *
 *	@param [in]		aDevice			Flash Device handle
 *	@param [in]		aOffset			Byte Offset address into flash to read
 *	@param [out]	*aBuffer		Pointer to buffer to receive data
 *	@param [in]		aNumBytes		Number of bytes to read
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashRead(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
						
/**
 *	Write bytes into the Flash. NOTE: You must erase the flash before
 *  writing to it.
 *
 *	@param [in]	aDevice			Flash Device handle
 *	@param [in]	aOffset			Byte Offset address into flash to read
 *	@param [in]	*aBuffer		Pointer to buffer of data
 *	@param [in]	aNumBytes		Number of bytes to write
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashWrite(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
						
/**
 *	Erase one ore more blocks in the given location
 *
 *	@param [in]	aDevice			Flash Device handle
 *	@param [in]	aOffset			Byte Offset address into flash to start erase
 *	@param [in]	aNumBytes		Number of bytes to erase
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashBlockErase(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            TUInt32 aNumBytes);
						
/**
 *	Erase the complete chip
 *
 *	@param [in]	aDevice			Flash Device handle
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashChipErase(T_uezDevice aDevice);

/**
 *	Query the chip for the given register.
 *
 *	@param [in]		aDevice			Flash Device handle
 *	@param [in]		aReg				Register to query
 *	@param [out]	*aValue			Place to store value returned
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashQueryReg(
            T_uezDevice aDevice,
            TUInt32 aReg,
            TUInt32 *aValue);
						
/**
 *	Get information about the chip size
 *
 *	@param [in]		aDevice		Flash Device handle
 *	@param [out]	*aInfo		Pointer to structure to receive info
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashGetChipInfo(
            T_uezDevice aDevice,
            T_FlashChipInfo *aInfo);
						
/**
 *	Get information about the block
 *
 *	@param [in]	aDevice				Flash Device handle
 *	@param [in]	aOffset				Byte offset into flash
 *	@param [in]	*aBlockInfo		Pointer to structure to receive info
 *
 *	@return		T_uezError
 */
T_uezError UEZFlashGetBlockInfo(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            T_FlashBlockInfo *aBlockInfo);
			
#ifdef __cplusplus
}
#endif

#endif // _UEZ_Flash_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZFlash.h
 *-------------------------------------------------------------------------*/
