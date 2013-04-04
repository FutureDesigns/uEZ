/**
 *	@file 	uEZFlash.h
 *  @brief 	uEZ Flash Interface
 *
 *	The uEZ interface which maps to low level Flash drivers.
 */
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
#ifndef _UEZ_Flash_H_
#define _UEZ_Flash_H_

#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/Flash.h>

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

#endif // _UEZ_Flash_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZFlash.h
 *-------------------------------------------------------------------------*/
