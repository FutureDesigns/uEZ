/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

/* This type MUST be 8 bit */
typedef unsigned char   BYTE;

/* These types MUST be 16 bit */
typedef short           SHORT;
typedef unsigned short  WORD;
typedef unsigned short  WCHAR;

/* These types MUST be 16 bit or 32 bit */
typedef int             INT;
typedef unsigned int    UINT;

/* These types MUST be 32 bit */
typedef long            LONG;
typedef unsigned long   DWORD;

/* Boolean type */
#ifndef FALSE
    typedef enum { FALSE = 0, TRUE } BOOL;
#else
    typedef int BOOL;
#endif

#endif
