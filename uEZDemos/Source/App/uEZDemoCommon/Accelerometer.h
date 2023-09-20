/*-------------------------------------------------------------------------*
 * File:  uEZDemoCommon.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Common library features of all FDI uEZ Demos
 *-------------------------------------------------------------------------*/
 
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
#include <uEZ.h>

#ifdef __cplusplus
extern "C" {
#endif


#define DISPLAY_WIDTH       UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT      UEZ_LCD_DISPLAY_HEIGHT

void AccelDemoMode(const T_choice *aChoice);
// Accelerometer Demo Options:
#ifndef ACCEL_DEMO_SWAP_XY
    #define ACCEL_DEMO_SWAP_XY          0
#endif
#ifndef ACCEL_DEMO_FLIP_X
    #define ACCEL_DEMO_FLIP_X           0
#endif
#ifndef ACCEL_DEMO_FLIP_Y
    #define ACCEL_DEMO_FLIP_Y           0
#endif
#ifndef ACCEL_DEMO_G_TO_PIXELS
    #define ACCEL_DEMO_G_TO_PIXELS      128
#endif
#ifndef ACCEL_DEMO_ALLOW_ROTATE
    #define ACCEL_DEMO_ALLOW_ROTATE     1
#endif


#ifdef __cplusplus
}
#endif

/*-------------------------------------------------------------------------*
 * File:  uEZDemoCommon.h
 *-------------------------------------------------------------------------*/
