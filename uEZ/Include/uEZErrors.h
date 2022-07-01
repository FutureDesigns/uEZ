/*-------------------------------------------------------------------------*
* File:  uEZErrors.h
*--------------------------------------------------------------------------*
* Description:
*         Common return type in uEZ
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZErrors.h
 *  @brief 	uEZError Type definition
 *
 *	Common return type in uEZ
 */
#ifndef _UEZ_ERRORS_H_
#define _UEZ_ERRORS_H_

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	T_uezError typedef
 */
typedef enum {
    UEZ_ERROR_NONE=0,
    UEZ_ERROR_OUT_OF_HANDLES=1,
    UEZ_ERROR_TIMEOUT=2,
    UEZ_ERROR_HANDLE_INVALID=3,
    UEZ_ERROR_OUT_OF_MEMORY=4,
    UEZ_ERROR_NOT_ENOUGH_DATA=5,
    UEZ_ERROR_OUT_OF_RANGE=6,
    UEZ_ERROR_NOT_SUPPORTED=7,
    UEZ_ERROR_ILLEGAL_OPERATION=8,
    UEZ_ERROR_NOT_FOUND=9,
    UEZ_ERROR_NOT_AVAILABLE=10,
    UEZ_ERROR_INTERNAL_ERROR=11,
    UEZ_ERROR_NAK=12,
    UEZ_ERROR_NOT_READY=13,
    UEZ_ERROR_INVALID_PARAMETER=14,
    UEZ_ERROR_ALREADY_EXISTS=15,
    UEZ_ERROR_READ_WRITE_ERROR=16,
    UEZ_ERROR_PROTECTED=17,
    UEZ_ERROR_NOT_ACTIVE=18,
    UEZ_ERROR_CANCELLED=19,
    UEZ_ERROR_OVERFLOW=20,
    UEZ_ERROR_STALL=21,
    UEZ_ERROR_CHECKSUM_BAD=22,
    UEZ_ERROR_INVALID=23,
    UEZ_ERROR_NOT_OPEN=24,
    UEZ_ERROR_BAD_ALIGNMENT=25,
    UEZ_ERROR_OUT_OF_DATA=26,
    UEZ_ERROR_ALREADY_OPEN=27,
    UEZ_ERROR_INCORRECT_TYPE=28,
    UEZ_ERROR_BUFFER_ERROR=29,
    UEZ_ERROR_ABORTED=30,
    UEZ_ERROR_RESETED=31,
    UEZ_ERROR_CLOSED=32,
    UEZ_ERROR_COULD_NOT_CONNECT=33,
    UEZ_ERROR_ILLEGAL_VALUE=34,
    UEZ_ERROR_ILLEGAL_ARGUMENT=35,
    UEZ_ERROR_OUT_OF_SPACE=36,
    UEZ_ERROR_READ_ONLY=37,
    UEZ_ERROR_DEADLOCK=38,
    UEZ_ERROR_NOT_EMPTY=39,
    UEZ_ERROR_ILLEGAL_SEQUENCE=40,
    UEZ_ERROR_BUSY=41,
    UEZ_ERROR_MISMATCH=42,
    UEZ_ERROR_INCOMPLETE_CONFIGURATION=43,
    UEZ_ERROR_INCOMPLETE=44,
    UEZ_ERROR_FAIL=45,
    UEZ_ERROR_COULD_NOT_JOIN=46,
    UEZ_ERROR_INVALID_SECURITY=47,
    UEZ_ERROR_EMPTY=48,
    UEZ_ERROR_TERMINATED=49,
    UEZ_ERROR_COULD_NOT_ERASE=50,
    UEZ_ERROR_ERASE_REQUIRED=51,
    UEZ_ERROR_DEVICE_STATUS=52,
    UEZ_ERROR_VERIFY_FAILED=53,
    UEZ_ERROR_UNKNOWN_DEVICE=54,
    UEZ_ERROR_OUT_OF_STACK_SPACE=55,//1000,
    UEZ_ERROR_TOO_MANY_TASKS=56,//1001,
    UEZ_ERROR_DEVICE_NOT_FOUND=57,//2000,
    UEZ_ERROR_UNKNOWN_DEVICE_CMD=58,//2001,
    UEZ_ERROR_ILLEGAL_PARAMETER=59,//2002,
    UEZ_ERROR_COULD_NOT_RESET=60,//2003,
    UEZ_ERROR_COULD_NOT_START=61,//2004,
    UEZ_ERROR_MUST_CLOSE_FIRST=62,//2005,
	UEZ_ERROR_RTOS_START=63,//3000,		/** Refer to RTOS specific errors. */
	UEZ_ERROR_RTOS_END=64,//4000,		/** Range reserved for RTOS errors. */
    UEZ_ERROR_UNKNOWN=0xFF,//(int32_t)0xFFFFFFFF
} T_uezError;

#ifdef __cplusplus
}
#endif

#endif // _UEZ_ERRORS_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZErrors.h
 *-------------------------------------------------------------------------*/
