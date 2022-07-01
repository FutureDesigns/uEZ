/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Technology Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Technology Corp. and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* TECHNOLOGY CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THE THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************************
FILE NAME    : GAPI_Font.c

DESCRIPTION  : Font Support for Renesas Graphics API.
              Requires GAPI BMP functions

Copyright   : 2007 Renesas Technology America
Copyright   : 2007 Renesas Technology Corporation.
          All Rights Reserved
***********************************************************************************/

/***********************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
01.02.2007 RTA-JMB First Release
02.15.2007 RTA-JMB Improved transparency handling
03.01.2007 RTA-JMB Added size flexibility to font BMPs
07.02.2007 RTA-MEV Rev 1.0
01.28.2008 RTA-JMB Revised to remove redundent API
***********************************************************************************/
#include <string.h>
#include "GAPI.h"

/**********************************************************************************
Function Name:   LCDGlyphFind
Description:  Find the Glyph information associated with the code
Parameters:   
      pF: Pointer to font structure
      code: value of code to look up.
Return value:   pointer to glyph record.
***********************************************************************************/
static type_Eglyph const * LCDGlyphFind(GFont_type const *pF, uI32 code)
{
  type_Eglyph const *pG;
  sI32 Low=0, High = pF->base.num_glyphs-1, index;
  while(Low <= High)
  {
    index = (High+Low)/2;
    pG = &pF->glyphs[index];

    if (code == pG->char_code)
      return(pG);
      
    if (code < pG->char_code)
      High = index -1;
    else
      Low = index +1;
  }
  return(NULL); 
}

/**********************************************************************************
Function Name:   LCDBMP_UTF8decode
Description:  extract 32bit unicode value from UTF-8 stream
Parameters:   s: pointer to pointer to user string (POINTER IS UPDATED BY THIS ROUTINE)
Return value:   width of string.
***********************************************************************************/
static uI32 LCDBMP_UTF8decode(uI08 const ** pS)
{
  uI08 const *s = *pS;
  /* step past first character */
  *pS += 1;
  if(*s < 0x80)
  {
    return((uI32)*s);
  }
  if((0xC2  <= *s ) && (*s <= 0xF4))
  {
    uI32 value;
    int32_t index;
    if(*s < 0xE0)
    {
      /* two byte sequence*/
      value = (((uI32)s[0] & 0x1F) << 6) | (((uI32)s[1] & 0x3F) << 0);
      index = 1;
    }
    else if (*s < 0xF0)
    {
      /* three byte sequence*/
      value = (((uI32)s[0] & 0x0F) << 12) | (((uI32)s[1] & 0x3F) << 6) | (((uI32)s[2] & 0x3F) << 0);
      index = 2;
    }
    else
    {
      /* four byte sequence*/
      value = (((uI32)s[0] & 0x07) << 18) | (((uI32)s[1] & 0x3F) << 12) | (((uI32)s[2] & 0x3F) << 6) | (((uI32)s[3] & 0x3F) << 0);
      index = 3;
    }

    /* point to secondary bytes and check their range*/
    s++;
    for (; index > 0; index--, s++)
    {
      if((*s < 0x80) || (*s > 0xBF))
      {
        /* out of range, return error */
        *pS = s + 1;
        return(0x20);
      }
    }
    *pS = s;
    return(value);
  }
  /* shouldn't be here */
  return(0x20);
}

/**********************************************************************************
Function Name:   LCDBMP_FontHeight
Description:  Determine height of the font (in pixels)
Parameters:   Gdata: pointer to font information 
Return value:   max vertical size of font.
***********************************************************************************/
sI16 LCDBMP_FontHeight(GPUT_type const *Gdata)
{
  GFont_type const * pFont = *Gdata->ppFont;
  if(pFont == NULL)
    return(0);
    
  return(pFont->base.bbox.yMax - pFont->base.bbox.yMin);
}

/**********************************************************************************
Function Name:   LCDBMP_FontWidthString
Description:  Determine width of the supplied string (in pixels)
Parameters:   s: pointer to user string
              Gdata: pointer to font information 
Return value:   Width of the drawn string
***********************************************************************************/
sI16 LCDBMP_FontWidthString(uI08 const * s, GPUT_type const *Gdata)
{
  uI32 code;
  type_Eglyph const *pG;
  GFont_type const * pFont = *Gdata->ppFont;
  sI16 PosX = -pFont->base.bbox.xMin;
  sI16 Max = 0;
  sI16 IncX = Gdata->SpacingX;
  
  if(pFont == NULL)
    return(0);
    
  if(IncX < 0)
    IncX = pFont->base.bbox.xMax - pFont->base.bbox.xMin;
  
  while ((code = LCDBMP_UTF8decode(&s)) != 0)
  {
    /* Ensure character is in font */
    pG = LCDGlyphFind(pFont, code);
    if (pG != NULL)
    {
      sI16 test = PosX + pG->left + pG->width;
      if(test > Max)
        Max = test;  /* this case will typically occur on every glyph */

      if(IncX == 0)
      {
        /* Use proportional font advancement*/
        PosX += pG->advance;
      }
      else
      {
        PosX += IncX;
      }
    }
  }
  if(Max < PosX) Max = PosX;
  return(Max);
}

/**********************************************************************************
Function Name:   LCDBMPGPutS
Description:  Places user string within destination BMP
Parameters:   s: pointer to user string
              Gdata: pointer to font information (also includes substitution color table
                and orientation information).
              d: pointer to destination BMP
              PosX: starting X coordinate within destination BMP (leftmost = 0)
              PosY: starting Y coordinate within destination BMP (bottommost = 0)
Return value:   position of dominant axis on return (can be used to append text if used as starting point).
***********************************************************************************/
sI16 LCDBMPGPutS( uI08 const * s, GPUT_type const *Gdata, BMP_type *d, sI16 PosX, sI16 PosY)
{
  gapiResp_type status=0;
  type_Eglyph const *pG;
  GFont_type const * pFont = *Gdata->ppFont;
  uI32 code;
  uI16 tr = NO_TRANSPARENCY_COLOR;
  sI16 IncX, IncY;
  sI16 FillX = PosX, FillY = PosY;
  ColorTable_type *pCT;
  BMP_type *pBMPFont;

  if(pFont == NULL)
    return(0);
    
  pCT = (void *)gapiMalloc(1024);
  pBMPFont = (void *)gapiMalloc(256);
  if((NULL == pCT) || (NULL == pBMPFont))
  {
    /* free up any allocated buffers and return */
    (void)gapiFree((void *)pCT);
    (void)gapiFree((void *)pBMPFont);
    return(-1);
  }

  IncX = Gdata->SpacingX;
  if(IncX < 0)
    IncX = pFont->base.bbox.xMax - pFont->base.bbox.xMin;
  IncY = Gdata->SpacingY;
  if(IncY < 0)
    IncY = pFont->base.bbox.yMax - pFont->base.bbox.yMin;

  /* compensate for font baseline */
  PosX -= pFont->base.bbox.xMin;
  PosY -= pFont->base.bbox.yMin;
  
  /* initialize for 8bpp anti-aliased fonts */
  if ((pFont->base.render_style & RENDER_8BPP) == RENDER_8BPP)
  {
    sI16 index;
    uI16 *pCT16 = (uI16 *)pCT;
    /* generate a color table for the font */
    if (status == 0) status = LCDBMPCalcGradientCTN(&Gdata->ct[1],2,pCT,256);
    
    /* convert to 16bpp...so we don't have to on every glpyh */
    for (index=0; index<256; index++)
      pCT16[index] = LCDBMP24_16(&pCT[index]);
  }

  /* initialize for 4bpp anti-aliased fonts */
  if ((pFont->base.render_style & RENDER_4BPP) == RENDER_4BPP)
  {
    sI16 index;
    uI16 *pCT16 = (uI16 *)pCT;
    /* generate a color table for the font */
    if (status == 0) status = LCDBMPCalcGradientCTN(&Gdata->ct[1],2,pCT,16);
    
    /* convert to 16bpp...so we don't have to on every glpyh */
    for (index=0; index<16; index++)
      pCT16[index] = LCDBMP24_16(&pCT[index]);
  }

  /* initialize for non-anti-aliased 1bpp fonts */
  if ((pFont->base.render_style & RENDER_1BPP) == RENDER_1BPP)
  {
    /* convert to 16bpp...so we don't have to on every glpyh */
    sI16 index;
    uI16 *pCT16 = (uI16 *)pCT;
    for (index=0; index<2; index++)
      pCT16[index] = LCDBMP24_16(&Gdata->ct[index]);
      
    /* handle transparency */
    if (Gdata->Options.Bgnd_transparent)
      tr = 0;

    if (Gdata->Options.Fgnd_transparent)
      tr = 1;
  }

  while ((code = LCDBMP_UTF8decode(&s)) != 0)
  {
    /* Ensure character is in font */
    pG = LCDGlyphFind(pFont, code);
    if (pG != NULL)
    {
      uI32 pRaster = ((uI32)pG + pG->raster_offset);

      if((pG->raster_style & EFNT_8BPP) == EFNT_8BPP)
      {
        /* 8 BPP alpha mask */
        LCDInitFrame(pBMPFont, pRaster, 8, pG->pitch, pG->height);
        if((pG->raster_style & EFNT_RLE) == EFNT_RLE)
          pBMPFont->biCompression[0] = 1;
        setBMPF(pBMPFont,ALPHA_MASK);
        setBMPF(pBMPFont,16BPP_CT);
        tr = 0;
      }
      else if((pG->raster_style & EFNT_4BPP) == EFNT_4BPP)
      {
        /* 4 BPP alpha mask */
        LCDInitFrame(pBMPFont, pRaster, 4, pG->pitch * 2, pG->height);
        if((pG->raster_style & EFNT_RLE) == EFNT_RLE)
          setBMPF(pBMPFont,4BPP_RLE);
        setBMPF(pBMPFont,ALPHA_MASK);
        setBMPF(pBMPFont,16BPP_CT);
        tr = 0;
      }
      else
      {
        /* 1 BPP image */
        LCDInitFrame(pBMPFont, pRaster, 1, pG->pitch * 8, pG->height);
        setBMPF(pBMPFont,16BPP_CT);
      }  
        
      if (Gdata->Options.Bgnd_transparent == 0)
      {
        /* paint the background solid */
        sI16 Width = (pG->advance > (pG->left + pG->width)) ? pG->advance : (pG->left + pG->width);
        Width = Width + PosX - FillX;
        if (Width > 0)
        {
          (void)LCDBMPFill(d, FillX, FillY, Width, LCDBMP_FontHeight(Gdata), &Gdata->ct[0]);
          FillX += Width;
        }
      }

      if((IncX == 0) && (IncY == 0))
      {
        /* Use proportional font advancement */
        /* Extract font data and write to destination using requested color table */
        (void)LCDBMPCopySub( pBMPFont, d, PosX+pG->left, PosY+pG->bottom, 0, 0, pG->width, pG->height, pCT, tr);
        PosX += pG->advance;
      }
      else
      {
        /* use fixed font advancement override */
        (void)LCDBMPCopySub( pBMPFont, d, PosX+(IncX- (pG->width+pG->left))/2, PosY+pG->bottom, 0, 0, pG->width, pG->height, pCT, tr);
        PosX += IncX;
        PosY += IncY;
      }
    }
  }
  (void)gapiFree((void *)pCT);
  (void)gapiFree((void *)pBMPFont);
  /* Return position of dominant axis */
  return(PosX);
}
