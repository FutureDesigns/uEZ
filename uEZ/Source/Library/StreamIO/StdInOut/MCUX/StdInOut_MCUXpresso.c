/*-------------------------------------------------------------------------*
 * File:  StdInOut_MCUXpresso.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Capture output to the stdout and redirect to the uEZ stream.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2024 Future Designs, Inc.
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
// board specific/platform include here
#include <stdio.h>
#include <stdint.h>
#include <uEZ.h>
#include "../StdInOut.h"

//int32_t __getchar(void);
//#define getchar(void) _Generic(void: getchar_a)(void)

#define PARAMETER_NOT_USED(p) (void) ((p))

#ifdef __MCUXPRESSO

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
    T_uezDevice stdout2 = StdoutGet();

    if (stdout2 != (T_uezDevice)NULL) {
        if (__c == '\n') {
          char ch_ret = '\r'; //__putchar('\r');
          UEZStreamWrite(stdout2, &ch_ret, 1, 0, 1000);
        }
        UEZStreamWrite(stdout2, &ch, 1, 0, 1000);
    } else {
        return EOF;
    }
    return ch;
}

int __putchar(int aChar)
{
    PARAMETER_NOT_USED(aChar);
    return putchar_uEZ(aChar);
}

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
/*int32_t __getchar(void) {
    return 0;
}*/

#if 0
int puts_char(const char *s){
  return putchar_uEZ((int)*s);
};


int _printf_char(const char *fmt, ...)
{
  int r = 0;
  //va_list ParamList;

  //va_start(ParamList, fmt);
  //r = SEGGER_RTT_vprintf(1, sFormat, &ParamList);
  //va_end(ParamList);
  return r;
}
int printf_char(const char *fmt, ...)
{
  int r = 0;
//    va_list ParamList;

  //  va_start(ParamList, fmt);
    //r = SEGGER_RTT_vprintf(1, sFormat, &ParamList);
//    va_end(ParamList);
    return r;
}
#endif

#if 1
size_t _write(int handle, const unsigned char *buffer, size_t size)
{
#if defined(DEBUG_ENABLE)
        size_t nChars = 0;

        if (buffer == 0) {
                /*
                   This means that we should flush internal buffers.  Since we
                   don't we just return.  (Remember, "handle" == -1 means that all
                   handles should be flushed.)
                 */
                return 0;
        }

        /* This template only writes to "standard out" and "standard err",
           for all other file handles it returns failure. */
        if (( handle != _LLIO_STDOUT) && ( handle != _LLIO_STDERR) ) {
                return _LLIO_ERROR;
        }

        for ( /* Empty */; size != 0; --size) {
        	putchar_uEZ(*buffer++);
            ++nChars;
        }
        return nChars;
#else
        return size;
#endif /* defined(DEBUG_ENABLE) */
}
#endif

#if 0
/*********************************************************************
*
*       _write_r()
*
* Function description
*   Low-level reentrant write function.
*   libc subroutines will use this system routine for output to all files,
*   including stdout.
*   Write data via RTT.
*/
size_t _write_r(void *r, int file, const void *ptr, size_t len) {
  (void) file;  /* Not used, avoid warning */
  (void) r;     /* Not used, avoid warning */
  return len;
}
#endif

#else
#endif

#if 1 // TODO remove newlib re-ent --enable-newlib-reent-small
int _fstat_r (void * ptr, int fd, void * pstat) {
  return 0;
}

void * _sbrk_r(void * ptr, int incr) {
  return 0;
}

size_t _read_r(void * ptr, int* fd, int buf, size_t cnt) {
    return cnt;
}

int _isatty_r (ptr, fd) struct _reent *ptr;     int fd;
{
  return 0;
}

int _lseek_r(void * ptr, int fd, int , int whence) {
  return 0;
}

int _close_r (ptr, fd)     struct _reent *ptr;     int fd;
{
  (void) ptr;
  (void) fd;
  return 0;
}
#endif

/*-------------------------------------------------------------------------*
 * End of File:  StdInOut_MCUXpresso.c
 *-------------------------------------------------------------------------*/
