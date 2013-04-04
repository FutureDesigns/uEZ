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
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZStream.h>
#include <stdio.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#if !defined(EOF)
#define EOF -1
#endif

#if !defined(putchar)
int putchar(int aChar);
#endif

#if !defined(getchar)
int getchar(void);
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

#endif // STDINOUT_H_
/*-------------------------------------------------------------------------*
 * End of File:  StdInOut.h
 *-------------------------------------------------------------------------*/

