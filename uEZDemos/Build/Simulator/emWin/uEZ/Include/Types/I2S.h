/*-------------------------------------------------------------------------*
 * File:  Types/I2S.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2C HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _I2C_TYPES_H_
#define _I2C_TYPES_H_

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
 *  @file   /Include/Types/I2S.h
 *  @brief  uEZ I2S Types
 *
 *  The uEZ I2S Types
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
#include <HAL/GPIO.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iSampleRate;
    TUInt8 iFormat; /** 1 is mono, 0 is stereo */
        #define I2S_SETTING_SAMPLE_FORMAT_STEREO    0
        #define I2S_SETTING_SAMPLE_FORMAT_MONO      1
    TUInt8 iSampleSize;
        #define I2S_SETTING_SAMPLE_SIZE_8BIT        0
        #define I2S_SETTING_SAMPLE_SIZE_16BIT       1
        #define I2S_SETTING_SAMPLE_SIZE_32BIT       3
    TUInt8 iLowLevel;
} T_I2S_Settings;

#endif // _I2S_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/I2S.h
 *-------------------------------------------------------------------------*/
