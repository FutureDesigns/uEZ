/*-------------------------------------------------------------------------*
 * File:  StdInOut_Generic.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*
 * Routine:  putchar
 *---------------------------------------------------------------------------*
 * Description:
 *      Puts a character out to the standard console.  In this case, we
 *      use the standard out stream device (declared elsewhere).
 * Inputs:
 *      int aChar               -- Character to output
 * Outputs:
 *      int                     -- Character output or EOF if could not
 *                                  output.
 *---------------------------------------------------------------------------*/
int putchar(int aChar)
{
    char c = aChar;
    T_uezDevice stdout = StdOutGet();

    if (stdout) {
        if (aChar == '\n')
            putchar('\r');
        UEZStreamWrite(stdout, &c, 1, 0, UEZ_TIMEOUT_INFINITE);
    } else {
        return EOF;
    }
    return c;
}

/*---------------------------------------------------------------------------*
 * Routine:  getchar
 *---------------------------------------------------------------------------*
 * Description:
 *      Get a character from the standard input.  NOTE: blocks until
 *      a character is received.
 * Outputs:
 *      int                     -- Character received or EOF if could not
 *                                  get a character (no stdin).
 *---------------------------------------------------------------------------*/
int getchar(void)
{
    char c;
    T_uezDevice stdin = StdInGet();

    if (stdin) {
        if (UEZStreamRead(stdin, &c, 1, 0, UEZ_TIMEOUT_INFINITE)
                == UEZ_ERROR_NONE)
            return c;
        else
            return EOF;
    } else {
        return EOF;
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  StdInOut_Generic.c
 *-------------------------------------------------------------------------*/
