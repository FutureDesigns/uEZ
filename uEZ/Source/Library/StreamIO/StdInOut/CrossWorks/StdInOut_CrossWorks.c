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
#include <debugio.h>
#include <__cross_studio_io.h>
#include <stdio.h>
#include <stdint.h>
#include <uEZ.h>
#include "../StdInOut.h"

//int32_t getchar_a(void);
//#define getchar(void) _Generic(void: getchar_a)(void)

#define PARAMETER_NOT_USED(p) (void) ((p))

//#undef __putchar
//#undef debug_putchar
//#undef putchar

/*---------------------------------------------------------------------------*
 * Routine:  putchar
 *---------------------------------------------------------------------------*
 * Description:
 *      Puts a character out to the standard console.  In this case, we
 *      use the standard out stream device (declared elsewhere).
 * Inputs:
 *      int32_t aChar               -- Character to output
 * Outputs:
 *      int32_t                     -- Character output or EOF if could not
 *                                  output.
 *---------------------------------------------------------------------------*/
int putchar_uEZ(int __c)
{
    char ch = __c;
    T_uezDevice stdout = StdoutGet();

    if (stdout != NULL) {
        if (__c == '\n')
            putchar('\r');
        UEZStreamWrite(stdout, &ch, 1, 0, 1000);
    } else {
        return EOF;
    }
    return ch;
}

#if 1
int __putchar(int aChar, __printf_tag_ptr unused)
{
    PARAMETER_NOT_USED(unused);
    return putchar_uEZ(aChar);
}
#else
int __putchar(int ch)
{
    return putchar_uEZ(c);
}
#endif

#if 0
int debug_putchar(int c)
{
    return putchar_uEZ(c);
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  getchar
 *---------------------------------------------------------------------------*
 * Description:
 *      Get a character from the standard input.  NOTE: blocks until
 *      a character is received.
 * Outputs:
 *      int32_t                     -- Character received or EOF if could not
 *                                  get a character (no stdin).
 *---------------------------------------------------------------------------*/
// TODO: Replace getchar with appropriate function
/*int32_t getchar_a(void) {
    return 0;
}*/

/*-------------------------------------------------------------------------*
 * End of File:  StdInOut_CrossWorks.c
 *-------------------------------------------------------------------------*/
