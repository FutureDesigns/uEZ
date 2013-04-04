/**
 *	@file 	uEZEEPROM.h
 *  @brief 	uEZ EEPROM Interface
 *
 *	The uEZ EEPROM interface.
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
#ifndef _UEZ_EEPROM_H_
#define _UEZ_EEPROM_H_

#include <uEZTypes.h>
#include <uEZErrors.h>

/**
 *	Open up access to the EEPROM.
 *
 *	@param [in] 	*aName		Pointer to name of EEPROM(usually
 *                              "EEPROMx" where x is bus number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *	@return			T_uezError
 */
T_uezError UEZEEPROMOpen(const char * const aName, T_uezDevice *aDevice);

/**
 *	End access to the EEPROM.
 *
 *	@param [in]		aDevice			Device handle to close
 *	@return			T_uezError
 */
T_uezError UEZEEPROMClose(T_uezDevice aDevice);

/**
 *	Requests an EEPROM read with the given parameters.
 *
 *      @param [in]		aDevice         Handle to opened EEPROM device
 *		@param [out]	*aData			Place to store read data
 *      @param [in]     aAddress        Address of data to read
 *		@param [in]		aDataLength		Number of bytes to read
 *
 *		@return			T_uezError
 */
T_uezError UEZEEPROMRead(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aAddress,
    TUInt8 aDataLength);
	
/**
 *	Requests an EEPROM write with the given parameters.
 *
 *	@param [in]		aDevice     Handle to opened EEPROM device
 *	@param [in]		*aData		Pointer to data to write out
 *  @param [in]     aAddress    Address to write data
 *	@param [in]		aDataLength	Number of bytes to write
 *
 *	@return			T_uezError
 */
T_uezError UEZEEPROMWrite(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aAddress,
    TUInt8 aDataLength);
		
#endif // _UEZ_EEPROM_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZEEPROM.h
 *-------------------------------------------------------------------------*/
