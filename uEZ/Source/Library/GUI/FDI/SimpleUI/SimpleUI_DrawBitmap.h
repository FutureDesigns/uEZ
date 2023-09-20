/*-------------------------------------------------------------------------*
 * File:  SimpleUI_DrawBitmap.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Routine to draw bitmaps on the screen
 *-------------------------------------------------------------------------*/
#ifndef _SIMPLEUI_DRAWBITMAP_H_
#define _SIMPLEUI_DRAWBITMAP_H_

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

#include "SimpleUI_Types.h"

#ifdef __cplusplus
extern "C" {
#endif


int32_t SUIDrawBitmap(const TUInt8 *aBitmapImage, TUInt32 aX, TUInt32 aY);

#ifdef __cplusplus
}
#endif

#endif // _SIMPLEUI_DRAWING_H_
/*-------------------------------------------------------------------------*
 * File:  SimpleUI_DrawBitmap.c
 *-------------------------------------------------------------------------*/
