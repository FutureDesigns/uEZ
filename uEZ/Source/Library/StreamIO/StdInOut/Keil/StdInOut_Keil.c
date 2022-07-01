/*-------------------------------------------------------------------------*
 * File:  StdInOut_CrossWorks.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
//#include <uEZ.h>
#include <uEZTypes.h>
#include <uEZTickCounter.h>
#include <rt_sys.h>
#include <rt_misc.h>
#pragma import(__use_no_semihosting)

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
struct __FILE {
    int handle; /* Add whatever you need here */
};
#if !defined(FILE)
typedef struct __FILE FILE;
#endif
			
/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
FILE __stdout;
FILE __stdin;
FILE __stderr;

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
void _ttywrch(int c)
{
    static int wrap = 0;
    static char wrapBuffer[32];
    // Wrap in small buffer here
    wrapBuffer[wrap & 31] = c;
    wrap++;
}

int fputc(int aChar, FILE *f)
{
    char c = aChar;
    T_uezDevice stdout = StdoutGet();

    if (stdout) {
        if (aChar == '\n')
            putchar('\r');
        UEZStreamWrite(stdout, &c, 1, 0, UEZ_TIMEOUT_INFINITE);
        if (aChar == '\n')
            UEZStreamFlush(stdout);
    } else {
        _ttywrch(c);
        return c;
    }

    return c;
}

int fgetc(FILE *f)
{
    return (0);
}

void _sys_exit(int return_code)
{
    label: goto label;
    /* endless loop */
}

FILEHANDLE _sys_open(const char *name, int openmode)
{
    return 0;
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

/*-------------------------------------------------------------------------*
 * End of File:  StdInOut_CrossWorks.c
 *-------------------------------------------------------------------------*/
