/*-------------------------------------------------------------------------*
 * File:  Types/I2S.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2C HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _I2C_TYPES_H_
#define _I2C_TYPES_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
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
#include <Include/uEZTypes.h>
#include <Include/HAL/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _I2S_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/I2S.h
 *-------------------------------------------------------------------------*/
