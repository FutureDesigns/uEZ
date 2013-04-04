/*-------------------------------------------------------------------------*
 * File:  StdInOut.c
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup StdInOut
 *  @{
 *  @brief     uEZ Std In Out Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    Standard IO used by getchar and putchar and its other files
 *      can be redirected to a new output.  These outputs can be to
 *      any stream active in the system.
 */
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
#include <uEZ.h>
#include "StdInOut.h"

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezDevice G_stdin;
static T_uezDevice G_stdout;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  StdinRedirect
 *---------------------------------------------------------------------------*/
/**
 *  Redirects standard in to given uEZ Device.
 *
 *  @param [in] aIn		uEZ Device to redirect input to
 */
/*---------------------------------------------------------------------------*/
void StdinRedirect(T_uezDevice aIn)
{
    G_stdin = aIn;
}
/*---------------------------------------------------------------------------*
 * Routine:  StdoutRedirect
 *---------------------------------------------------------------------------*/
/**
 *  Redirects standard out to given uEZ Device.
 *
 *  @param [in] aOut	uEZ Device to redirect output to
 */
/*---------------------------------------------------------------------------*/
void StdoutRedirect(T_uezDevice aOut)
{
    G_stdout = aOut;
}
/*---------------------------------------------------------------------------*
 * Routine:  Stdinet
 *---------------------------------------------------------------------------*/
/**
 *  Gets the standard input
 *
 *  @return     T_uezDevice standard in
 */
/*---------------------------------------------------------------------------*/
T_uezDevice StdinGet(void)
{
    return G_stdin;
}
/*---------------------------------------------------------------------------*
 * Routine:  StdoutGet
 *---------------------------------------------------------------------------*/
/**
 *  Gets the standard output
 *
 *  @return     T_uezDevice standard out
 */
/*---------------------------------------------------------------------------*/
T_uezDevice StdoutGet(void)
{
    return G_stdout;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  StdInOut.c
 *-------------------------------------------------------------------------*/
