/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************/

#ifndef GAPI_H
#define GAPI_H

#include <uEZ.h>
#include <uEZPacked.h>
#include <uEZInline.h>

/***********************************************************************************
FILE NAME    : GAPI.h

DESCRIPTION  : Structure definitions and function prototypes for Renesas Graphics API.

Copyright   : 2007 Renesas Technology America
Copyright   : 2007 Renesas Technology Corporation.
          All Rights Reserved
***********************************************************************************/

/***********************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
01.02.2007 RTA-JMB First Release
02.15.2007 RTA-JMB Improved transparency handling
02.26.2007 RTA-JMB Added BMP Fill
07.02.2007 RTA-MEV Rev 1.0
***********************************************************************************/
/* Typedef fundmental data types that may change between compilers/families */
typedef unsigned char uI08;
typedef unsigned short uI16;
typedef unsigned long uI32;
typedef signed char sI08;
typedef signed short sI16;
typedef signed long sI32;
typedef uI08 boolean;

//#define HW_BYTE_SWAPPED_BUS 1

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#define NULL	0
#endif

#define size_ra(ra) (sizeof(ra)/sizeof(ra[0]))

#include "gapi_font.h"

typedef int32_t gapiResp_type;


PACK_STRUCT_BEGIN
typedef struct
{
        PACK_STRUCT_FIELD(type_Efont base);
        PACK_STRUCT_FIELD(type_Eglyph glyphs[1]);  /* this will really be an array of as many glyphs in font */
} PACK_STRUCT_STRUCT GFont_type;
PACK_STRUCT_END

/* Data structure of BMP color table entry */
#if 0
typedef struct 
{
  uI08 Blue;
  uI08 Green;
  uI08 Red;
  uI08 reserved;
} ColorTable_type;
#else
#define CT_BLUE 0
#define CT_GREEN 1
#define CT_RED 2
PACK_STRUCT_BEGIN
typedef struct
{
        PACK_STRUCT_FIELD(uI08 channel[4]);
} PACK_STRUCT_STRUCT ColorTable_type;
PACK_STRUCT_END

#endif

/* BMP header structure */
PACK_STRUCT_BEGIN
typedef struct
{
        PACK_STRUCT_FIELD(uI08 bfSignature[2]);
        PACK_STRUCT_FIELD(uI08 bfFileSize[4]);
        PACK_STRUCT_FIELD(uI08 bfReserved[4]); // [0].0 = flag for reverse Y on received raster copies
        PACK_STRUCT_FIELD(uI08 bfDataOffset[4]);
  
        PACK_STRUCT_FIELD(uI08 biSize[4]);
        PACK_STRUCT_FIELD(uI08 biWidth[4]);
        PACK_STRUCT_FIELD(uI08 biHeight[4]);
        PACK_STRUCT_FIELD(uI08 biPlanes[2]);
        PACK_STRUCT_FIELD(uI08 biBitCount[2]);
        PACK_STRUCT_FIELD(uI08 biCompression[4]);
        PACK_STRUCT_FIELD(uI08 biImageSize[4]);
        PACK_STRUCT_FIELD(uI08 biXPixelsPerMeter[4]);
        PACK_STRUCT_FIELD(uI08 biYPixelsPerMeter[4]);
        PACK_STRUCT_FIELD(uI08 biClrUsed[4]);
        PACK_STRUCT_FIELD(uI08 biClrImportant[4]);
        PACK_STRUCT_FIELD(ColorTable_type biColorTable[1]); // must use 2 for proper packing
} PACK_STRUCT_STRUCT BMP_type;
PACK_STRUCT_END

/* internal BMP flags...stored in biCompression[1] */
#define BMP_NATIVE 0x80
#define BMP_NO_QUAD 0x40
#define BMP_ALPHA_MASK 0x20
#define BMP_16BPP_CT 0x10
#define BMP_4BPP_RLE 0x08
#define isBMPF(pB, test) ((pB->biCompression[1] & BMP_##test) == BMP_##test)
#define setBMPF(pB, test) pB->biCompression[1] |= BMP_##test

/* Flags to control LCDBMPCopyXlate */
#define BMP_CX_ROT_0    0x0000
#define BMP_CX_ROT_90   0x0001
#define BMP_CX_ROT_180  0x0002
#define BMP_CX_ROT_270  0x0003
#define BMP_CX_ROT_MASK 0x0003
#define BMP_CX_FLIPX    0x0004
#define BMP_CX_FLIPY    0x0008

/* 16bpp data structure for 5:6:5 (this may vary by MCU endian/compiler) */
typedef uI16 PIXEL_16bpp_type;

PACK_STRUCT_BEGIN
typedef struct
{
        PACK_STRUCT_FIELD(uI08 Blue);
        PACK_STRUCT_FIELD(uI08 Green);
        PACK_STRUCT_FIELD(uI08 Red);
} PACK_STRUCT_STRUCT P24bpp_type;
PACK_STRUCT_END

PACK_STRUCT_BEGIN
typedef union
{
        PACK_STRUCT_FIELD(uI08 *p08);
        PACK_STRUCT_FIELD(PIXEL_16bpp_type *p16);
        PACK_STRUCT_FIELD(P24bpp_type *p24);
        PACK_STRUCT_FIELD(ColorTable_type *pCT);
        PACK_STRUCT_FIELD(uI32 *p32);
} PACK_STRUCT_STRUCT pRaster_type;
PACK_STRUCT_END

/* structure definition for font information for "gputs" routine */
typedef struct
{
  GFont_type const **ppFont;  /* pointer to font pointer */
  ColorTable_type ct[2];      /* color table 0=BGND, 1=FGND (BGND only applies to two color fonts) */
  sI16 SpacingX;              /* 0=font specified, > 0 space between characters -1 set spacing to bbox values */
  sI16 SpacingY;              /* 0=font specified, > 0 space between characters, -1 set spacing to bbox values */
  struct 
  {
    uI16 Orientation: 2;      /* 0=Right/Left, 1: Left/Right, 2: Top/Bottom, 3: Bottom/Top */
    uI16 Rotation: 2;         /* 0=0, 1=90, 2=180, 3=270 */
    uI16 Hflip: 1;            /* 0=none, 1=flip on horizontal axis */
    uI16 Vflip: 1;            /* 0=none, 1=flip on vertical axis */
    uI16 Bgnd_transparent:1;  /* 0=background solid, 1=transparent...only 2 color fonts */
    uI16 Fgnd_transparent:1;  /* 0=foreground solid, 1=transparent...only 2 color fonts */
  } Options;
}GPUT_type;

typedef struct
{
  ColorTable_type ct[2];      /* color table for gemerating gradient for colorizing button...[0]=black replace, [1]=white replace */
  GPUT_type font;             /* information for text placement */
  struct
  {
    uI16 Colorize: 2;         /* colorize the passed image 0:no, 1:grayscale, 2:shade, 3: reserved */
    uI16 Text: 1;             /* Text Insertion 0:no, 1:yes */
    uI16 Transparency: 1;     /* Transparency selection 0:no, 1:yes */
  } Options;
}GSCHEME_type;

#define NO_TRANSPARENCY_COLOR 0xAAAAu  /* when passed as "transparency" paramter, causes no transparency handling */
#define WHITE_16 0xFFFFu /* 16bpp White */
#define BLACK_16 0x0000u /* 16bpp Black */
#define GREEN_16 0xE007u /* 16bpp Green */
#define BLUE_16  0x1F00u /* 16bpp Blue */
#define RED_16   0x00F8u /* 16bpp Red */

/* Function prototypes */
gapiResp_type LCDInitFrame(BMP_type *const d, uI32 r, const uI08 bpp, const sI16 Width, const sI16 Height);
sI16 BMP_Height(BMP_type const *const pBmp);
sI16 BMP_Width(BMP_type const *const pBmp);
sI32 BMP_Offset(BMP_type const *const pBmp);
uI32 BMP_FileSize(BMP_type const *const pBmp);
uI32 BMP_IndexColors(BMP_type const *const pBmp);

gapiResp_type LCDBMPCopySub( BMP_type const *const s, BMP_type *const d, sI16 dPosX, sI16 dPosY, sI16 sPosX, sI16 sPosY, sI16 Width, sI16 Height, ColorTable_type const *const ct, uI16 tr );
gapiResp_type LCDBMPCopyXlate( BMP_type const *const s, BMP_type *const d, sI16 dPosX, sI16 dPosY, sI16 sPosX, sI16 sPosY, sI16 Width, sI16 Height, uI16 Mode );
gapiResp_type LCDBMPFill(BMP_type *const d, const sI16 dPosX, const sI16 dPosY, sI16 Width, sI16 Height, ColorTable_type const *const ct);
gapiResp_type LCDBMPFillGradient(BMP_type *const d, const sI16 dPosX, const sI16 dPosY, sI16 Width, sI16 Height, ColorTable_type const *const ct, sI16 angle);
gapiResp_type LCDBMPCalcGradientCTN(ColorTable_type const *const ct, uI08 mode, ColorTable_type * ct_out, sI16 N);
gapiResp_type LCDBMPCalcShadeCT(ColorTable_type const * pS, ColorTable_type const *const ct, ColorTable_type * ct_out);
sI16 LCDBMPGPutS( uI08 const * s, GPUT_type const *Gdata, BMP_type *d, sI16 PosX, sI16 PosY);
sI16 LCDBMP_FontHeight(GPUT_type const *Gdata);
sI16 LCDBMP_FontWidthString(uI08 const * s, GPUT_type const *Gdata);
gapiResp_type LCDBMPColorCopy(BMP_type const *const s, BMP_type *d, sI16 PosX, sI16 PosY, GSCHEME_type const *pScheme);
gapiResp_type LCDBMPLabel(BMP_type const *const s, BMP_type *d, sI16 PosX, sI16 PosY, GSCHEME_type const *pScheme, uI08 const * label);
void LCDBMPScroll(BMP_type const *s, BMP_type *d, sI16 dPosX, sI16 dPosY, sI16 split, uI16 type);

/* define a function for GAPI to allocate memory */
uI08 * ExMemoryGet(uI16 size);
static INLINE uI08 *gapiMalloc(uI16 size)
{
//  return(ExMemoryGet(size));
  return (uI08 *)malloc(size);
}

/* define a function for GAPI to release memory */
uI08 ExMemoryFree(uI08 *pPool);
static INLINE uI08 gapiFree(uI08 *handle)
{
//  return(ExMemoryFree(handle));
  if (handle)
    free(handle);
  return 0;
}

/**********************************************************************************
Function Name:   LCDBMP24_16
Description:  converts 24bpp to 16bpp
              useful for generating transparency color
Parameters:   p24: pointer to source pixel
Return value:   16bpp equivalent
***********************************************************************************/
#if 0
#ifdef HW_BYTE_SWAPPED_BUS
static INLINE uI16 LCDBMP24_16( ColorTable_type const *const p24 )
{
  uI16 Tout;
  ColorTable_type local_ct = *p24;
  Tout  = (((uI16)local_ct.channel[CT_GREEN]) << 11) & 0xE000u;
  Tout |= (((uI16)local_ct.channel[CT_GREEN]) >>  5) & 0x0007u;
  Tout |= (((uI16)local_ct.channel[CT_BLUE] ) <<  5) & 0x1F00u;
  Tout |= (((uI16)local_ct.channel[CT_RED]  ) <<  0) & 0x00F8u;

  return(Tout);
}
#else
static INLINE uI16 LCDBMP24_16( ColorTable_type const *const p24 )
{
  uI16 Tout;
  ColorTable_type local_ct = *p24;
  Tout  = (((uI16)local_ct.channel[CT_GREEN]) <<  3) & 0x07E0u;
  Tout |= (((uI16)local_ct.channel[CT_BLUE] ) >>  3) & 0x001Fu;
  Tout |= (((uI16)local_ct.channel[CT_RED]  ) <<  8) & 0xF800u;

  return(Tout);
}
#endif
#else
    #if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_8_BIT)
        #define RGB(r, g, b) \
            ((((r>>5)&7)<<5)| \
            (((g>>5)&7)<<2)| \
            (((b>>6)&3)<<0))
    #endif
    #if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_16_BIT)
        #define RGB(r, g, b)      \
            ( (((r>>3)&0x1F)<<11)| \
              (((g>>2)&0x3F)<<5)| \
              (((b>>3)&0x1F)<<0) )
    #endif
    #if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_I15_BIT)
        #define RGB(r, g, b)      \
            ( (((r>>3)&0x1F)<<10)| \
              (((g>>3)&0x1F)<<5)| \
              (((b>>3)&0x1F)<<0) )
    #endif
#define RGB16(r, g, b)      \
                    ( (((r>>3)&0x1F)<<11)| \
                      (((g>>2)&0x3F)<<5)| \
                      (((b>>3)&0x1F)<<0) )
static INLINE uI16 LCDBMP24_16( ColorTable_type const *const p24 )
{
  uI16 Tout;
  ColorTable_type local_ct = *p24;
//  Tout  = (((uI16)local_ct.channel[CT_GREEN]) <<  3) & 0x07E0u;
//  Tout |= (((uI16)local_ct.channel[CT_BLUE] ) >>  3) & 0x001Fu;
//  Tout |= (((uI16)local_ct.channel[CT_RED]  ) <<  8) & 0xF800u;
  Tout = RGB(
            ((uI16)local_ct.channel[CT_RED]),
            ((uI16)local_ct.channel[CT_GREEN]),
            ((uI16)local_ct.channel[CT_BLUE]));

  return(Tout);
}
#endif

/**********************************************************************************
Function Name:   LCDBMPCopy
Description:  simplified BMP to BMP copy routine (inline to minimize stack usage)
Parameters:   s: pointer to source BMP
              d: pointer to destination BMP
              PosX: starting X coordinate within destination BMP (leftmost = 0)
              PosY: starting Y coordinate within destination BMP (bottommost = 0)
Return value:   0:OK, Not 0: NG
***********************************************************************************/
static INLINE gapiResp_type LCDBMPCopy( BMP_type const *const s, BMP_type *d, sI16 PosX, sI16 PosY )
{
  /* pass through to sub handler */
  return(LCDBMPCopySub(s, d, PosX, PosY, 0, 0, BMP_Width(s), BMP_Height(s), s->biColorTable, NO_TRANSPARENCY_COLOR));
}

/**********************************************************************************
Function Name:   LCDBMPCopyTransparent
Description:  simplified BMP to BMP copy routine (inline to minimize stack usage)
Parameters:   s: pointer to source BMP
              d: pointer to destination BMP
              PosX: starting X coordinate within destination BMP (leftmost = 0)
              PosY: starting Y coordinate within destination BMP (bottommost = 0)
Return value:   0:OK, Not 0: NG
***********************************************************************************/
static INLINE gapiResp_type LCDBMPCopyTransparent( BMP_type const *const s, BMP_type *d, sI16 PosX, sI16 PosY )
{
  /* pass through to sub handler */
  return(LCDBMPCopySub(s, d, PosX, PosY, 0, 0, BMP_Width(s), BMP_Height(s), s->biColorTable, (uI16)(BMP_IndexColors(s)-1)));
}

/**********************************************************************************
Function Name:   LCDBMPIndex
Description:  simplified BMP to BMP copy routine (inline to minimize stack usage)
Parameters:   s: pointer to source BMP
              d: pointer to destination BMP
              PosX: starting X coordinate within destination BMP (leftmost = 0)
              PosY: starting Y coordinate within destination BMP (bottommost = 0)
              index: Index into source BMP
Return value:   0:OK, Not 0: NG
***********************************************************************************/
static INLINE gapiResp_type LCDBMPIndex( BMP_type const *const s, BMP_type *const d, sI16 PosX, sI16 PosY, sI16 Index)
{
  sI16 Height = BMP_Height(s);
  /* pass through to sub handler */
  return(LCDBMPCopySub(s, d, PosX, PosY, Height*Index, 0, Height, Height, s->biColorTable, (uI16)(BMP_IndexColors(s)-1)));
}

/**********************************************************************************
Function Name:   LCDBMPLabel
Description:  Draw label in center of BMP
Parameters:   s: pointer to source button BMP
              d: pointer to destination BMP
              PosX: starting X coordinate within destination BMP (leftmost = 0)
              PosY: starting Y coordinate within destination BMP (bottommost = 0)
              pScheme: pointer to information about how to present the image
              Label: Text string to center on  button
Return value:   0:OK, Not 0: NG
***********************************************************************************/
static INLINE gapiResp_type LCDBMPButton(BMP_type const *const s, BMP_type *d, sI16 PosX, sI16 PosY, GSCHEME_type const *pScheme, uI08 const * label)
{
  gapiResp_type status;
  status = LCDBMPColorCopy(s,d,PosX,PosY,pScheme);
  if (0 != status)
    return(status);
  return(LCDBMPLabel(s,d,PosX,PosY,pScheme,label));
}

#endif
