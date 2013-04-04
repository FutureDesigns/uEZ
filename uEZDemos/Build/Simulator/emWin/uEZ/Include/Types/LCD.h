/*-------------------------------------------------------------------------*
 * File:  LCD.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Generic LCD types used by several systems
 *-------------------------------------------------------------------------*/
#ifndef _LCD_TYPES_H_
#define _LCD_TYPES_H_

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
 *  @file   /Include/Types/LCD.h
 *  @brief  uEZ LCD Types
 *
 *  The uEZ LCD Types
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
#define UEZLCD_COLOR_DEPTH_1_BIT    0
#define UEZLCD_COLOR_DEPTH_2_BIT    1
#define UEZLCD_COLOR_DEPTH_4_BIT    2
#define UEZLCD_COLOR_DEPTH_8_BIT    3
#define UEZLCD_COLOR_DEPTH_16_BIT   4
#define UEZLCD_COLOR_DEPTH_I15_BIT  5
#define UEZLCD_COLOR_DEPTH_24_BIT   6
typedef TUInt32 T_uezLCDColorDepth;

typedef enum {
    UEZLCD_ORDER_RGB,
    UEZLCD_ORDER_BGR,
} T_uezLCDPixelOrder;


#endif // _LCD_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Serial.h
 *-------------------------------------------------------------------------*/
