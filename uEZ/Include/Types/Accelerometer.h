/*-------------------------------------------------------------------------*
 * File:  Types/Accelerometer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Accelerometer types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _ACCELEROMETER_TYPES_H_
#define _ACCELEROMETER_TYPES_H_

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

/**
 *  @file   /Include/Types/Accelerometer.h
 *  @brief  uEZ Accelerometer Types
 *
 *  The uEZ Accelerometer Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    /** Unique identifier for this accelerometer */
    TUInt32 iChipID;

    /** Version of accelerometer in 16.16 fixed format
     * (e.g., version 1.00 is 0x00010000)
     */
    TUInt32 iRevision;
} AccelerometerInfo;

// Three signed readings from accelerometer (x, y, and z).
// Readings are in g's with signed 15.16 fixed notation
typedef struct {
    TInt32 iX;
    TInt32 iY;
    TInt32 iZ;
} AccelerometerReading;

// Three signed readings from accelerometer (x, y, and z).
// Readings are in floating point notation in G's
typedef struct {
    float iX;
    float iY;
    float iZ;
} AccelerometerReadingFloat;

#ifdef __cplusplus
}
#endif

#endif // _ACCELEROMETER_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/Accelerometer.h
 *-------------------------------------------------------------------------*/
