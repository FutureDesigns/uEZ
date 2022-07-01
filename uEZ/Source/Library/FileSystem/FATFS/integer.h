/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

#include <stdint.h> /* for int types */

/* These types MUST be 16-bit or 32-bit */
//typedef int32_t		int32_t; // already defined
typedef uint32_t	UINT;

/* This type MUST be 8-bit */
typedef unsigned char	BYTE;

/* These types MUST be 16-bit */
typedef short			SHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types MUST be 32-bit */
typedef long			LONG;
typedef unsigned long	DWORD;

/* This type MUST be 64-bit (Remove this for C89 compatibility) */
typedef unsigned long long QWORD;

/* Boolean type */
#ifndef FALSE
    typedef enum { FALSE = 0, TRUE } BOOL;
#else
    typedef int32_t BOOL;
#endif

#endif
