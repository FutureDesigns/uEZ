/*-------------------------------------------------------------------------*
 * File:  SimpleUI_DrawBitmap.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Utility routines to use for drawing graphics and showing pages
 *      of the LCD display.
 *
 * Implementation:
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZFile.h>
#include <string.h>
#include <uEZPlatform.h>
#include "SimpleUI.h"
#include "stdio.h"

/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/
#define FILE_HEADER_SIZE  14

/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/
extern T_SUISettings G_SUISettings;

enum T_BitmapCompressionMethods {
	BI_RGB,
	BI_RLE8,
	BI_RLE4,
	BI_BITFIELDS,
	BI_JPEG_OR_RLE24,
	BI_PNG,
	BI_ALPHABITFIELDS
};

enum T_BitmapHeaderSizes {
	BITMAPCOREHEADER = 12,
	BITMAPCOREHEADER2 = 64,		// Adds halftoning. Adds RLE and Huffman 1D compression.
	BITMAPINFOHEADER = 40, 		// Most Common - Removes RLE-24 and Huffman 1D compression. Adds 
								// 16bpp and 32bpp pixel formats. Adds optional RGB bit masks.
	BITMAPV2INFOHEADER = 52,	// Removes optional RGB bit masks. Adds mandatory RGB bit masks.
	BITMAPV3INFOHEADER = 56,	// Adds mandatory alpha channel bit mask.
	BITMAPV4HEADER = 108,		// Adds color space type and gamma correction
	BITMAPV5HEADER = 124		// Adds ICC color profiles
};

enum T_BitmapFileFormat {
	FILE_HEADER = 0,
	FILE_SIZE = 2,
	FILE_RESERVED1 = 6,
	FILE_RESERVED2 = 8,
	FILE_IMAGE_OFFSET = 10,
	BMP_HEADER_SIZE = 14,
	BMP_WIDTH = 18,
	BMP_HEIGHT = 22,
	BMP_NUM_COLOR_PLANES = 26,
	BMP_NUM_BPP = 28,
	BMP_COMPRESSION_METHOD = 30,
	BMP_IMAGE_SIZE = 34,
	BMP_HORIZONTAL_RES = 38,
	BMP_VERTICAL_RES = 42,
	BMP_COLOR_PALETTE_SIZE = 46,
	BMP_IMPORTANT_COLORS = 50,
	BMP_COLOR_PALETTE_START = 54
};

T_pixelColor ConvertColor32(TUInt32 aColor)
{
	TUInt8 r, g, b;
	r = (TUInt8)(aColor>>16);
	g = (TUInt8)(aColor>>8);
	b = (TUInt8)(aColor);
	return SUICallbackRGBConvert(r,g,b);
}

/*---------------------------------------------------------------------------*
 * Routine:  IHidePage0
 *---------------------------------------------------------------------------*
 * Description:
 *      Copy the current viewed page to the hidden page 1 and then show
 *      page 1 (hiding page 0 as changes are about to be made).
 *---------------------------------------------------------------------------*/
int SUIDrawBitmap(const TUInt8 *aBitmapImage, TUInt32 aX, TUInt32 aY)
{
	static T_pixelColor ColorPalette[256];
    TUInt32 bitmapHeaderSize;
	TUInt32 imageOffset;
	TUInt32 height, width;
	TUInt16 compressionMethod;
	TUInt32 colorPaletteSize;
	TUInt16 numBPP;
	TUInt8 *p = (TUInt8 *)aBitmapImage;
	TUInt8 paddingCount;
	TUInt32 i, x, y;
	TUInt8 firstByte, numPixelsToDraw, colorIndex;
	
	bitmapHeaderSize = *((TVUInt32 *)(p + BMP_HEADER_SIZE));
	imageOffset = *((TVUInt32 *)(p + FILE_IMAGE_OFFSET));
	width = *((TVUInt32 *)(p + BMP_WIDTH));
	height = *((TVUInt32 *)(p + BMP_HEIGHT));
	/* NOTE: Technically, compressionMethod should be a 32-bit number and not a
	 * 16-bit number, but on the LPC2478 and crossworks, the address being
	 * retrieved for the logo is across a 32-bit boundary and is causing the
	 * compiler to incorrectly see this as 0x00080001 instead of 0x000000001.
	 * For now, we're going to just use 16-bit values since all compression
	 * values are 0-7.  -- lshields 3/26/2013
         * Additionally, make them all volatile.  The compiler is over optimizing
         * too.  Weird.
	 */
	compressionMethod = *((TVUInt16 *)(p + BMP_COMPRESSION_METHOD));
	colorPaletteSize = *((TVUInt32 *)(p + BMP_COLOR_PALETTE_SIZE));
	numBPP = *((TVUInt32 *)(p + BMP_NUM_BPP));
	
	if(colorPaletteSize > 256) {
		printf("Bitmap Error: Invalid Color Palette Size.\r\n");
		return 0;
	}
	
	if(numBPP != 8) {
		printf("Bitmap Error: Bitmap must be a 256 color image.\r\n");
		return 0;
	}
	
	// Determine the padding count - each line must end in 4 byte boundaries
	paddingCount = (4-(width & 3)) & 3;
	
	p = (TUInt8 *)(aBitmapImage) + FILE_HEADER_SIZE + bitmapHeaderSize;
	for(i=0; i<colorPaletteSize; i++) {
		ColorPalette[i] = ConvertColor32(*((TUInt32 *)p));
		p += 4;
	}
	
	p = (TUInt8 *)(aBitmapImage) + imageOffset;
	
	if(compressionMethod == BI_RLE8) {
		for(y=height; y>0; y--) {
			x=0;
			while(x<width) {
				firstByte = *(p++);
				if(firstByte != 0) {
					// First byte is greater than 1. We just need to read the color index
					// and draw the number pixels equal to the first byte.
					colorIndex = *(p++);
					// Draw a raster of pixels
					swim_driver_fill_raster( &G_SUISettings.iWindow, x+aX, y+aY-1,
							ColorPalette[colorIndex], firstByte);
					x += firstByte;
				} else {
					// First byte is 0. The next byte will specify the number of pixels that
					// will follow.
					numPixelsToDraw = *(p++);
				
					swim_driver_put_raster_indexed_color(&G_SUISettings.iWindow,
	                	x+aX, y+aY-1, p, ColorPalette, numPixelsToDraw);	   
					x += numPixelsToDraw;
					p += numPixelsToDraw;
				
					if(numPixelsToDraw & 1) {
						// The next firstByte will always start on an even count. We must 
						// advance one byte after this pixel sequence if numPixelsToDraw
						// is odd.
						p++;
					}
				}
			}
			// We advance p if there is padding at the end of the line
			p += paddingCount;
		}
	} else if(compressionMethod == BI_RGB) {
		for(y=height; y>0; y--) {
			swim_driver_put_raster_indexed_color(&G_SUISettings.iWindow,
                	aX, y+aY-1, p, ColorPalette, width);
			p += width;
			p += paddingCount;   
		}
	} else {
		printf("Bitmap Error: Invalid Compression Method.\r\n");
		return 0;
	}
	return 1;
}


/*-------------------------------------------------------------------------*
 * File:  SimpleUI_DrawBitmap.c
 *-------------------------------------------------------------------------*/
