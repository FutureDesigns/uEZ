/*-------------------------------------------------------------------------*
* File:  uEZ.h
*--------------------------------------------------------------------------*
* Description:
*         Generic include file for all uEZ routines
*-------------------------------------------------------------------------*/

/**
 *	@file 	uEZ.h
 *  @brief 	uEZ System
 *
 *	Generic include file for all uEZ routines
 *
 * 	@mainpage
 *	@htmlinclude main.html
 */
#ifndef _UEZ_H_
#define _UEZ_H_

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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZErrors.h>
//#include <uEZMemory.h>
#include <uEZTickCounter.h>
#include <uEZRTOS.h>
//#include <uEZLCD.h>
#include <uEZTS.h>
#include <uEZCharDisplay.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Setup the memory, the handle table, and bring up the RTOS'
 *  multitasking system.
 *
 *	@return	UEZ_ERROR_NONE
 */
 T_uezError UEZSystemInit(void);

/**
 *	uEZ System Main Loop
 *
 *	@return	UEZ_ERROR_NONE
 */
T_uezError UEZSystemMainLoop(void);

/**
 *  Shows a failure message on the screen.
 *
 *  @return None
 */
void UEZFailureMsg(const char *aMsg);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZ.h
 *-------------------------------------------------------------------------*/
