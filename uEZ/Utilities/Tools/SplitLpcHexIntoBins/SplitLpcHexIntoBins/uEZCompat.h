/*-------------------------------------------------------------------------*
* File:  uEZCompat.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
#ifndef _UEZ_COMPAT_H_
#define _UEZ_COMPAT_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TUInt32
    typedef unsigned char TUInt8;
    typedef unsigned short TUInt16;
    typedef uint32_t TUInt32;
    typedef int32_t TBool;
    typedef char TInt8;
    typedef short TInt16;
    typedef int32_t TInt32;

#define EFalse 0
#define ETrue ~0

    typedef enum {
    UEZ_ERROR_NONE=0,
    UEZ_ERROR_OUT_OF_HANDLES,
    UEZ_ERROR_TIMEOUT,
    UEZ_ERROR_HANDLE_INVALID,
    UEZ_ERROR_OUT_OF_MEMORY,
    UEZ_ERROR_NOT_ENOUGH_DATA,
    UEZ_ERROR_OUT_OF_RANGE,
    UEZ_ERROR_NOT_SUPPORTED,
    UEZ_ERROR_ILLEGAL_OPERATION,
    UEZ_ERROR_NOT_FOUND,
    UEZ_ERROR_NOT_AVAILABLE,
    UEZ_ERROR_INTERNAL_ERROR,
    UEZ_ERROR_NAK,
    UEZ_ERROR_NOT_READY,
    UEZ_ERROR_INVALID_PARAMETER,
    UEZ_ERROR_ALREADY_EXISTS,
    UEZ_ERROR_READ_WRITE_ERROR,
    UEZ_ERROR_PROTECTED,
    UEZ_ERROR_NOT_ACTIVE,
    UEZ_ERROR_CANCELLED,
    UEZ_ERROR_OVERFLOW,
    UEZ_ERROR_STALL,
    UEZ_ERROR_CHECKSUM_BAD,
    UEZ_ERROR_INVALID,
    UEZ_ERROR_NOT_OPEN,
    UEZ_ERROR_BAD_ALIGNMENT,
    UEZ_ERROR_OUT_OF_DATA,
    UEZ_ERROR_OUT_OF_STACK_SPACE=1000,
    UEZ_ERROR_TOO_MANY_TASKS,
    UEZ_ERROR_DEVICE_NOT_FOUND=2000,
    UEZ_ERROR_UNKNOWN_DEVICE_CMD,
    UEZ_ERROR_ILLEGAL_PARAMETER,
    UEZ_ERROR_COULD_NOT_RESET,
    UEZ_ERROR_BUSY,
    UEZ_ERROR_UNKNOWN=(int32_t)0xFFFFFFFF
} T_uezError;


#endif

#ifdef __cplusplus
}
#endif

#endif // _UEZ_COMPAT_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZCompat.h
 *-------------------------------------------------------------------------*/
