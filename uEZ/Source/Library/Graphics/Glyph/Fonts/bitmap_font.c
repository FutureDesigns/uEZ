/*-------------------------------------------------------------------------*
 * File:  font_bitmap.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Overrides the glyph library bitmap font for custom use
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdint.h>

//	18	7E	FF	81
//	0	0	X	X
//	0	X	X	0
//	0	X	X	0
//	X	X	X	0
//	X	X	X	0
//	0	X	X	0
//	0	X	X	0
//	0	0	X	X
const uint8_t Bitmap_LoadingBar_Left[] = {
    0x04, 0x08, // width=4, height=8
    0x18, 0x7E, 0xFF, 0x81
};

//	81	FF	7E	19
//	X	X	0	0
//	0	X	X	0
//	0	X	X	0
//	0	X	X	X
//	0	X	X	X
//	0	X	X	0
//	0	X	X	0
//	X	X	0	0
const uint8_t Bitmap_LoadingBar_Right[] = {
    0x04, 0x08, // width=4, height=8
    0x81, 0xFF, 0x7E, 0x18
};

// 	81
// 	X
// 	0
// 	0
// 	0
// 	0
// 	0
// 	0
//	X
const uint8_t Bitmap_LoadingBar_Empty[] = {
    0x01, 0x08, // width=1, height=8
    0x81
};

// 	BD
// 	X
// 	0
// 	X
// 	X
// 	X
// 	X
// 	0
//	X
const uint8_t Bitmap_LoadingBar_Fill[] = {
    0x01, 0x08, // width=1, height=8
    0xBD
};

const uint8_t * const Bitmaps_table[256] = {
    Bitmap_LoadingBar_Left,		// 0
    Bitmap_LoadingBar_Right,	// 1
    Bitmap_LoadingBar_Empty,	// 2
    Bitmap_LoadingBar_Fill,		// 3
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
	0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
