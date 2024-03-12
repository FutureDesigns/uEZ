/**
 *  @file   StdInOut.h
 *  @brief  uEZ Std In/Out
 *
 *  Standard IO used by getchar and putchar and its other files
 *      can be redirected to a new output.  These outputs can be to
 *      any stream active in the system.
 */
#ifndef STDINOUT_H_
#define STDINOUT_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZStream.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#if !defined(EOF)
#define EOF -1
#endif

#if !defined(putchar)
int32_t putchar(int32_t aChar);
#endif

#if !defined(getchar)
int32_t getchar(void);
#endif

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
	 
/**
 *	Redirect Standard in
 *
 *	@param [in] aOut	device to redirect too
 */
void StdinRedirect(T_uezDevice aIn);
/**
 *	Redirect Standard out
 *
 *	@param [in] aOut	device to redirect too
 *                          
 */
void StdoutRedirect(T_uezDevice aOut);
/**
 *	Get the standard in
 *
 *	@return		T_uezDevice	uEZ device type
 */
T_uezDevice StdinGet(void);
/**
 *	Get the standard out
 *
 *	@return		T_uezDevice	uEZ device type
 */
T_uezDevice StdoutGet(void);

#ifdef __cplusplus
}
#endif

#endif // STDINOUT_H_
/*-------------------------------------------------------------------------*
 * End of File:  StdInOut.h
 *-------------------------------------------------------------------------*/

