/*-------------------------------------------------------------------------*
 * File:  uEZErrors.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Type definitions
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_ERRORS_H_
#define _UEZ_ERRORS_H_

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
    UEZ_ERROR_UNKNOWN=(int)0xFFFFFFFF
} T_uezError;

#endif // _UEZ_ERRORS_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZErrors.h
 *-------------------------------------------------------------------------*/
