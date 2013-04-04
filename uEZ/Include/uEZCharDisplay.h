/**
 *	@file 	uEZCharDisplay.h
 *  @brief 	uEZ CharDisplay Interface
 *
 *	The uEZ interface which maps to lower level text display drivers.
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_CHAR_DISPLAY_H_
#define _UEZ_CHAR_DISPLAY_H_

#include "uEZTypes.h"
#include "uEZErrors.h"

/**
 *	Open up access to the CharDisplay display (and turn on).
 *
 *	@param[in]	*aName			Pointer to name of CharDisplay display 
 *                      		(usually "CharDisplay")
 *	@param[out]	*aDevice		Pointer to device handle to be returned
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayOpen(
            const char * const aName, 
            T_uezDevice *aDevice);

/**
 *	End access to the CharDisplay display.  May turn off CharDisplay if 
 *  no one else is using the CharDisplay.
 *
 *	@param[in]	aDevice		Device handle of CharDisplay to close
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayClose(T_uezDevice aDevice);

/**
 *	Draw a single character on the character display.
 *
 *	@param [in]	aDevice		Handle to opened CharDisplay device.
 *	@param [in]	aRow			Row on display (top to bottom, 0 based)
 *	@param [in]	aColumn		Column on display (left to right, 0 based)
 *	@param [in]	aChar			Character to display	
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayDrawChar(
            T_uezDevice aDevice, 
            TUInt32 aRow,
            TUInt32 aColumn,
            TUInt32 aChar);

/**
 *	Draw a string of characters on the display
 *
 *	@param [in]	aDevice		Handle to opened CharDisplay device.
 *	@param [in]	aRow			Row on display (top to bottom, 0 based)
 *	@param [in]	aColumn		Column on display (left to right, 0 based)
 *	@param [in]	*aChar		Character string to display
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayDrawString(
            T_uezDevice aDevice, 
            TUInt32 aRow,
            TUInt32 aColumn,
            const char *aChar);

/**
 *	Draw a string of characters on the display
 *
 *	@param [in]	aDevice					Process command specific to the underlying device.
 *	@param [in]	aControl				Control being processed
 *	@param [in]	*aControlData		Data for control
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayControl(
            T_uezDevice aDevice,
            TUInt32 aControl,
            void *aControlData);

/**
 *	Clear a single row of text
 *
 *	@param [in]	aDevice			Handle to opened CharDisplay device.
 *	@param [in]	aRow				Row to be cleared
 *	@param [in]	aChar				Character to use
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayClearRow(
            T_uezDevice aDevice, 
            TUInt32 aRow,
            TUInt32 aChar);

/**
 *	Clear the display
 *
 *	@param [in]	aDevice			Handle to opened CharDisplay device.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayClearScreen(T_uezDevice aDevice);

/**
 *	Determine the size of the character display
 *
 *	@param [in]		aDevice				Handle to opened CharDisplay device.
 *	@param [out]	*aNumRows			Returned number of rows.
 *	@param [out]	*aNumColumns	Returned number of columns.
 *
 *	@return	T_uezError
 */ 
T_uezError UEZCharDisplayGetSize(
                T_uezDevice aDevice,
                TUInt32 *aNumRows,
                TUInt32 *aNumColumns);

#endif // _UEZ_CHAR_DISPLAY_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZCharDisplay.h
 *-------------------------------------------------------------------------*/
