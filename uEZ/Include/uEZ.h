/**
 *	@file 	uEZ.h
 *  @brief 	uEZ System
 *
 *	Generic include file for all uEZ routines
 *
 * 	@mainpage
 *	@htmlinclude main.html
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_H_
#define _UEZ_H_

#include <Config.h>
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <uEZMemory.h>
#include <uEZTickCounter.h>
#include <uEZRTOS.h>
//#include <uEZLCD.h>
#include <uEZTS.h>
#include <uEZCharDisplay.h>

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

#endif // _UEZ_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZ.h
 *-------------------------------------------------------------------------*/
