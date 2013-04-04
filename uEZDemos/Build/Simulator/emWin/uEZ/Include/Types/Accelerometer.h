/*-------------------------------------------------------------------------*
 * File:  Types/Accelerometer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Accelerometer types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _ACCELEROMETER_TYPES_H_
#define _ACCELEROMETER_TYPES_H_

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

/** Three signed readings from accelerometer (x, y, and z).
 * Readings are in g's with signed 15.16 fixed notation
 */
typedef struct {
    TInt32 iX;
    TInt32 iY;
    TInt32 iZ;
} AccelerometerReading;

#endif // _ACCELEROMETER_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/Accelerometer.h
 *-------------------------------------------------------------------------*/
