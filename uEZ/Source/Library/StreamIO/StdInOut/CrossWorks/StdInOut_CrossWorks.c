/*-------------------------------------------------------------------------*
 * File:  StdInOut_CrossWorks.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Capture output to the stdout and redirect to the uEZ stream.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
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
#include <__cross_studio_io.h>
#include <stdint.h>
#include <stdio.h>
#include <uEZ.h>
#include "..\StdInOut.h"

//int getchar_a(void);
//#define getchar(void) _Generic(void: getchar_a)(void)

#define PARAMETER_NOT_USED(p) (void) ((p))

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
int __putchar(int aChar, __printf_tag_ptr unused)
{
    PARAMETER_NOT_USED(unused);
    char c = aChar;
    T_uezDevice stdout = StdoutGet();

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
// TODO: Replace getchar with appropriate function
/*int getchar_a(void) {
    return 0;
}*/

/*-------------------------------------------------------------------------*
 * End of File:  StdInOut_CrossWorks.c
 *-------------------------------------------------------------------------*/
