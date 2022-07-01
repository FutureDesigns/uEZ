/*-------------------------------------------------------------------------*
* File:  uEZCharDisplay.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZCharDisplay.h
 *  @brief 	uEZ CharDisplay Interface
 *
 *	The uEZ interface which maps to lower level text display drivers.
 */
#ifndef _UEZ_CHAR_DISPLAY_H_
#define _UEZ_CHAR_DISPLAY_H_

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
#include "uEZTypes.h"
#include "uEZErrors.h"

#ifdef __cplusplus
extern "C" {
#endif

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
				
#ifdef __cplusplus
}
#endif

#endif // _UEZ_CHAR_DISPLAY_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZCharDisplay.h
 *-------------------------------------------------------------------------*/
