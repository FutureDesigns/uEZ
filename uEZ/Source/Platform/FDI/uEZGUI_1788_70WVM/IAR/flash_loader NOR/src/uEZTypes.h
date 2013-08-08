/*-------------------------------------------------------------------------*
 * File:  uEZTypes.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Type definitions
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_TYPES_H_
#define _UEZ_TYPES_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
 * Types:
 *-------------------------------------------------------------------------*/
#if (COMPILER_TYPE==CodeRed)
    typedef unsigned int TUInt32;
    typedef unsigned short TUInt16;
    typedef unsigned char TUInt8;
    typedef int TInt32;
    typedef short TInt16;
    typedef char TInt8;
    typedef void (*TFPtr)(void);
    typedef int TBool;
    #define EFalse          0
    #define ETrue           (~0)
#elif (COMPILER_TYPE==IAR)
    // IAR is based on the stdint library
    #include <stdint.h>
    typedef uint32_t TUInt32;
    typedef uint16_t TUInt16;
    typedef uint8_t TUInt8;
    typedef int32_t TInt32;
    typedef int16_t TInt16;
    typedef int8_t TInt8;
    typedef void (*TFPtr)(void);
    typedef int TBool;
    #define EFalse          0
    #define ETrue           (~0)
#else
    // Default types
    typedef unsigned long TUInt32;
    typedef unsigned short TUInt16;
    typedef unsigned char TUInt8;
    typedef long TInt32;
    typedef short TInt16;
    typedef char TInt8;
    typedef void (*TFPtr)(void);
    typedef int TBool;
    #define EFalse          0
    #define ETrue           (~0)
#endif

// Volatile extensions
typedef volatile TUInt32 TVUInt32;
typedef volatile TInt32 TVInt32;
typedef volatile TUInt16 TVUInt16;
typedef volatile TInt16 TVInt16;
typedef volatile TUInt8 TVUInt8;
typedef volatile TInt8 TVInt8;

/*-------------------------------------------------------------------------*
 * Handles:
 *-------------------------------------------------------------------------*/
typedef TUInt32 T_uezHandle;
typedef T_uezHandle T_uezDevice;

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#define ARRAY_COUNT(array)  (sizeof(array)/sizeof(array[0]))
#define CPUReg32(addr)      *((TVUInt32 *)(addr))
#define CPUReg16(addr)      *((TVUInt16 *)(addr))
#define CPUReg8(addr)       *((TVUInt8 *)(addr))
#define PARAM_NOT_USED(p)

#endif // _UEZ_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZTypes.h
 *-------------------------------------------------------------------------*/
