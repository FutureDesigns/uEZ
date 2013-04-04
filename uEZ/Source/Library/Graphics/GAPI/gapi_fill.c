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
FILE NAME    : GAPI_Copy.c

DESCRIPTION  : BMP copy Support for Renesas Graphics API.

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
07.02.2007 RT!-MEV Rev 1.0
02.04.2008 RTA-JMB Split from GAPI_BMP
06.22.2008 RTA-JMB Split from GAPI_Copy
***********************************************************************************/
#include "GAPI.H"

static const ColorTable_type CT_zero={0,0,0,0};

/**********************************************************************************
Function Name:   calc_gradient_pixel
Description:  Helper function to calculate intermediate gradient pixel colors
Parameters:   Gradient: position within the gradient scale
              Gradient_end: total size of gradient to ratio over
              ct: pointer to color table for gradient coloring [0] index = start, [1] index = end
              mode: specify order of operations on the color table.
Return value:  ct_out: color corresponding to intermediate gradient position.
***********************************************************************************/
static sI16 calc_gradient_pixel(sI32 Gradient, sI32 Gradient_end, ColorTable_type const *const ct, uI08 mode)
{
  sI32 rounding = Gradient_end/2;
  ColorTable_type local_ct[2];
  if (mode == 0)
  {
    local_ct[0] = ct[0];
    local_ct[1] = ct[1];
  }
  else
  {
    local_ct[0] = ct[1];
    local_ct[1] = ct[0];
  }

  local_ct[0].channel[CT_RED]   = (uI08)((sI32)local_ct[0].channel[CT_RED]   + (((sI32)local_ct[1].channel[CT_RED]   - (sI32)local_ct[0].channel[CT_RED])   * (sI32)Gradient + rounding) / Gradient_end);
  local_ct[0].channel[CT_GREEN] = (uI08)((sI32)local_ct[0].channel[CT_GREEN] + (((sI32)local_ct[1].channel[CT_GREEN] - (sI32)local_ct[0].channel[CT_GREEN]) * (sI32)Gradient + rounding) / Gradient_end);
  local_ct[0].channel[CT_BLUE]  = (uI08)((sI32)local_ct[0].channel[CT_BLUE]  + (((sI32)local_ct[1].channel[CT_BLUE]  - (sI32)local_ct[0].channel[CT_BLUE])  * (sI32)Gradient + rounding) / Gradient_end);
  return(LCDBMP24_16(&local_ct[0]));
}

/* helper function to create gradient increment */
static INLINE sI32 calc_gradient_increment(sI32 RangeIn, sI32 RangeOut)
{
  return((RangeIn << 16)/RangeOut);
}

/* helper function to copy gradient value */
static INLINE void copy_gradient(uI16 pixel, uI16 *pd, sI16 count, sI16 step)
{
  for (; count > 0; count--, pd += step)
    *pd = pixel;
}

/**********************************************************************************
Function Name:   BMPFill
Description:  Fill area of BMP with a color
Parameters:   d: pointer to destination BMP
              dPosX: starting X coordinate within destination BMP (leftmost = 0)
              dPosY: starting Y coordinate within destination BMP (bottommost = 0)
              Width: Width of area to fill
              Height: Height of area to fill
              ct: pointer to fill color
Return value:   0:OK, Not 0: NG
***********************************************************************************/
gapiResp_type LCDBMPFill(BMP_type *const d, const sI16 dPosX, const sI16 dPosY, sI16 Width, sI16 Height, ColorTable_type const *const ct)
{
  const sI16 dbiHeight = BMP_Height(d);
  sI16 dbiWidth = BMP_Width(d);
  pRaster_type pdRaster;
  sI16 indexW;
  uI16 pixel16;

  if(d==NULL)
    return(-1);
    
  /* only handling 16bpp destination */
  if (d->biBitCount[0] != 16)
    return(-1);

  /* bounds check destination and reject or trim if necessary */
  if ((dPosX > dbiWidth) || (dPosY > dbiHeight))
    return(-1);

  if ((Width < 0) || (Height < 0))
    return(-1);
    
  if ((Width + dPosX) > dbiWidth)
    Width = dbiWidth - dPosX;
  if ((Height + dPosY) > dbiHeight)
    Height = dbiHeight - dPosY;

  /* create a "true" quad line width (*2 for 16 bit pixels) */
  dbiWidth = (sI16)((((sI32)dbiWidth * (sI32)d->biBitCount[0] - 1)/32) + 1) * 2;

  /* point to destination raster data */
  pdRaster.p08 = ((uI08 *)d) + BMP_Offset(d);
  /* point to first destination line */
  pdRaster.p16 += (sI32)dbiWidth * (sI32)dPosY;
  /* point to destination pixel */
  pdRaster.p16 += dPosX;

  /* convert output pixel */
  pixel16=LCDBMP24_16(&ct[0]);

  /* fill pixels */
  for (; Height > 0; Height--, pdRaster.p16 += dbiWidth)
  {
    /* Copy this pixel to the pixels on this row */
    copy_gradient(pixel16, pdRaster.p16, Width, 1);
  }

  return(0);
}

/**********************************************************************************
Function Name:   LCDBMPCalcGradientCTN
Description:  function to build a gradient color table...usefull for "colorizing" grayscale BMPs
Parameters:   ct: pointer to color table for gradient coloring , [1] index = end
              mode: gradient generation mode
               0= ct[0] start-> ct[1] end
               1= ct[1] start-> ct[0] end
               2= 0:0:0 start-> ct[0] end
              N: Number of elements in color table
Return value:  ct_out: color corresponding to intermediate gradient position.
***********************************************************************************/
gapiResp_type LCDBMPCalcGradientCTN(ColorTable_type const *const ct, uI08 mode, ColorTable_type * ct_out, sI16 N)
{
  ColorTable_type local_ct[2];
  sI32 inc;
  sI32 sum;
  sI16 index,ch;
  switch (mode)
  {
    case 0:
    {
      local_ct[0] = ct[0];
      local_ct[1] = ct[1];
      break;
    }
    case 1:  
    {
      local_ct[0] = ct[1];
      local_ct[1] = ct[0];
      break;
    }
    default:
    {
      local_ct[0] = CT_zero;
      local_ct[1] = ct[0];
      break;
    }
  }
  
  /* initialize the channel value "sum" and determine the increment */
  for(ch=0; ch < 3; ch++)
  {
    ColorTable_type * ct_write = ct_out;
    sum = (uI32)local_ct[0].channel[ch] << 16;
    inc = calc_gradient_increment(((sI32)local_ct[1].channel[ch] - (sI32)local_ct[0].channel[ch]), N-1);
    /* calculate value for each color in the table */
    for (index = 0; index < N; index++, ct_write++)
    {
      ct_write->channel[ch] = (uI08)(sum >> 16);
      sum += inc;
    }
  }

  return (0);
}

/**********************************************************************************
Function Name:   LCDBMPCalcShadeCT
Description:  function to build a shaded color table...usefull for "dimming" color BMPs
Parameters:     pS: Source color table
                ct: pointer to color table for shade percentage [0] index= shade percentage, [1] index = na
Return value:  ct_out: color corresponding to shaded output
***********************************************************************************/
gapiResp_type LCDBMPCalcShadeCT(ColorTable_type const * pS, ColorTable_type const *const ct, ColorTable_type * ct_out)
{
  sI16 index;
  for (index = 0; index < 256; index++,ct_out++,pS++)
  {
    ct_out->channel[CT_RED] = (uI08)(((uI16)pS->channel[CT_RED] * (uI16)ct[0].channel[CT_RED])>>8);
    ct_out->channel[CT_GREEN] = (uI08)(((uI16)pS->channel[CT_GREEN] * (uI16)ct[0].channel[CT_GREEN])>>8);
    ct_out->channel[CT_BLUE] = (uI08)(((uI16)pS->channel[CT_BLUE] * (uI16)ct[0].channel[CT_BLUE])>>8);
  }
  return (0);
}

/**********************************************************************************
Function Name:   LCDBMPFillGradient
Description:  Fill area of BMP with a gradient color
Parameters:   d: pointer to destination BMP
              dPosX: starting X coordinate within destination BMP (leftmost = 0)
              dPosY: starting Y coordinate within destination BMP (bottommost = 0)
              Width: Width of area to fill
              Height: Height of area to fill
              ct: pointer to fill colors
              angle: angle (degrees) of gradient (presently only supporting 0,90
Return value:   0:OK, Not 0: NG
***********************************************************************************/

gapiResp_type LCDBMPFillGradient(BMP_type *const d, const sI16 dPosX, const sI16 dPosY, sI16 Width, sI16 Height, ColorTable_type const *const ct, sI16 angle)
{
  const sI16 dbiHeight = BMP_Height(d);
  sI16 dbiWidth = BMP_Width(d);
  pRaster_type pdRaster;
  sI16 indexH,indexW;
  uI16 pixel16;

  if(d==NULL)
    return(-1);
    
  /* only handling 16bpp destination */
  if (d->biBitCount[0] != 16)
    return(-1);

  /* bounds check destination and reject or trim if necessary */
  if (dPosX > dbiWidth)
    return(-1);
  if (dPosY > dbiHeight)
    return(-1);

  if ((Width + dPosX) > dbiWidth)
    Width = dbiWidth - dPosX;
  if ((Height + dPosY) > dbiHeight)
    Height = dbiHeight - dPosY;

  /* create a "true" quad line width (*2 for 16 bit pixels) */
  dbiWidth = (sI16)((((sI32)dbiWidth * (sI32)d->biBitCount[0] - 1)/32) + 1) * 2;

  /* point to destination raster data */
  pdRaster.p08 = ((uI08 *)d) + BMP_Offset(d);
  /* point to first destination line */
  pdRaster.p16 += (sI32)dbiWidth * (sI32)dPosY;
  /* point to destination pixel */
  pdRaster.p16 += dPosX;

  switch (angle)
  {
    case 0:
      /* Gradient increases horizontally left (ct[0]) to right (ct[1])*/
    case 180:
    {
      /* Gradient increases horizontally left (ct[1]) to right (ct[0])*/
      for (indexW = 0; indexW < Width; indexW++, pdRaster.p16++)
      {
        /* Calculate Pixel for this column */
        pixel16=calc_gradient_pixel((sI32)indexW,(sI32)Width,ct,angle==0?0:1);

        /* Copy this pixel to the pixels on this column */
        copy_gradient(pixel16, pdRaster.p16, Height, dbiWidth);
      }
      return(0);
    }

    case 90:
      /* Gradient increases vertically bottom (ct[0]) to top (ct[1]) */
    case 270:
    {
      /* Gradient increases vertically bottom (ct[1]) to top (ct[0]) */
      for (indexH = 0; indexH < Height; indexH++, pdRaster.p16 += dbiWidth)
      {
        /* Calculate Pixel for this row */
        pixel16=calc_gradient_pixel((sI32)indexH,(sI32)Height,ct,angle==90?0:1);

        /* Copy this pixel to the pixels on this row */
        copy_gradient(pixel16, pdRaster.p16, Width, 1);
      }
      return(0);
    }

    default:
      return(-1);
  }
}

/**********************************************************************************
Function Name:   LCDBMPCopyXlate
Description:  16bpp to 16bpp copy with rotate/mirror.
Parameters:   s: pointer to source BMP
              d: pointer to destination BMP
              dPosX: starting X coordinate within destination BMP (leftmost = 0)
              dPosY: starting Y coordinate within destination BMP (bottommost = 0)
              sPosX: starting X coordinate within source BMP (leftmost = 0)
              sPosY: starting Y coordinate within source BMP (bottommost = 0)
              Width: "width" of source area to copy
              Height: "height" of source area to copy
              Mode:  Opertions to perform duing copy
Return value:   0:OK, Not 0: NG
***********************************************************************************/
gapiResp_type LCDBMPCopyXlate( BMP_type const *const s, BMP_type *const d, sI16 dPosX, sI16 dPosY, sI16 sPosX, sI16 sPosY, sI16 Width, sI16 Height, uI16 Mode )
{
  const sI16 sbiHeight = BMP_Height(s);
  const sI16 sbiWidth = BMP_Width(s);

  const sI16 dbiHeight = BMP_Height(d);
  const sI16 dbiWidth = BMP_Width(d);

  uI08 rotate = 0;
  sI16 dHeight=Height, dWidth=Width;

  pRaster_type psRaster, pdRaster;

  if((s==NULL) || (d==NULL))
    return(-1);
    
  /* only handling 16bpp native destination */
  if ((d->biBitCount[0] != 16) || !isBMPF(d,NATIVE))
    return(-1);

  if((((Mode & BMP_CX_ROT_MASK) & BMP_CX_ROT_90) == BMP_CX_ROT_90) ||
    (((Mode & BMP_CX_ROT_MASK) & BMP_CX_ROT_270) == BMP_CX_ROT_270)) 
  {
    rotate = 1;
    dHeight = Width;
    dWidth = Height;
  }

  if (rotate == 0)
  {
    /* Region is NOT rotated in destination */
    /* bounds check source and reject or trim if necessary */
    if (sPosX > sbiWidth)
      return(-1);
    if (sPosY > sbiHeight)
      return(-1);
  
    if (sPosX < 0)
    {
      dPosX -= sPosX;
      Width += sPosX;
      sPosX = 0;
    }
    if (sPosY < 0)
    {
      dPosY -= sPosY;
      Height += sPosY;
      sPosY = 0;
    }
  
    if ((Width + sPosX) > sbiWidth)
      Width = sbiWidth - sPosX;
    if ((Height + sPosY) > sbiHeight)
      Height = sbiHeight - sPosY;
  
    /* bounds check destination and reject or trim if necessary */
    if (dPosX > dbiWidth)
      return(-1);
    if (dPosY > dbiHeight)
      return(-1);
  
    if (dPosX < 0)
    {
      sPosX -= dPosX;
      Width += dPosX;
      dPosX = 0;
    }
    if (dPosY < 0)
    {
      sPosY -= dPosY;
      Height += dPosY;
      dPosY = 0;
    }
    if ((Width + dPosX) > dbiWidth)
      Width = dbiWidth - dPosX;
    if ((Height + dPosY) > dbiHeight)
      Height = dbiHeight - dPosY;
  }
  else
  {
    /* Region IS rotated in destination */
    /* bounds check source and reject or trim if necessary */
    if (sPosX > sbiWidth)
      return(-1);
    if (sPosY > sbiHeight)
      return(-1);
  
    if (sPosX < 0)
    {
      dPosY -= sPosX;
      Width += sPosX;
      sPosX = 0;
    }
    if (sPosY < 0)
    {
      dPosX -= sPosY;
      Height += sPosY;
      sPosY = 0;
    }
  
    if ((Width + sPosX) > sbiWidth)
      Width = sbiWidth - sPosX;
    if ((Height + sPosY) > sbiHeight)
      Height = sbiHeight - sPosY;
  
    /* bounds check destination and reject or trim if necessary */
    if (dPosX > dbiWidth)
      return(-1);
    if (dPosY > dbiHeight)
      return(-1);
  
    if (dPosX < 0)
    {
      sPosY -= dPosX;
      Height += dPosX;
      dPosX = 0;
    }
    if (dPosY < 0)
    {
      sPosX -= dPosY;
      Width += dPosY;
      dPosY = 0;
    }
    if ((Height + dPosX) > dbiWidth)
      Height = dbiWidth - dPosX;
    if ((Width + dPosY) > dbiHeight)
      Width = dbiHeight - dPosY;
  }

  if ((Height < 0) || (Width < 0))
    return(-1);

  /* point to source raster data */
  psRaster.p08 = ((uI08 *)s) + BMP_Offset(s);

  /* point to first source pixel */
  psRaster.p16 += (sI32)sbiWidth * (sI32)sPosY + (sI32)sPosX;

  /* point to destination raster data */
  pdRaster.p08 = ((uI08 *)d) + BMP_Offset(d);

  /* point to first destination pixel (maybe) */
  pdRaster.p16 += (sI32)dbiWidth * (sI32)dPosY + (sI32)dPosX;

  {
    sI32 dLineInc, dPixelInc, dPixelIndex, dLineIndex;
    switch (Mode & 0xF)
    {
      case 0x00:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_0   */
      case 0x0E:   /*         0,           0,          1,       dW,    BMP_CX_FLIPY,    BMP_CX_FLIPX, BMP_CX_ROT_180 */
        pdRaster.p16 += (0 * (sI32)dbiWidth) + (sI32)0;
        dPixelInc = 1;
        dLineInc = dbiWidth;
        break;

      case 0x07:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY,    BMP_CX_FLIPX, BMP_CX_ROT_270 */                                                  
      case 0x09:   /*         0,           0,         dw,        1,    BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_90 */ 
        pdRaster.p16 += (0 * (sI32)dbiWidth) + (sI32)0;
        dPixelInc = dbiWidth;
        dLineInc = 1;
        break;

      case 0x01:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_90 */                                                  
      case 0x0F:   /*         0,           H,         dw,       -1,    BMP_CX_FLIPY,    BMP_CX_FLIPX, BMP_CX_ROT_270 */ 
        pdRaster.p16 += (0 * (sI32)dbiWidth) + (sI32)Height;
        dPixelInc = dbiWidth;
        dLineInc = -1;
        break;

      case 0x04:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_270 */                                                  
      case 0x0A:   /*         0,           W,         -1,       dW,    BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_180 */ 
        pdRaster.p16 += (0 * (sI32)dbiWidth) + (sI32)Width;
        dPixelInc = -1;
        dLineInc = dbiWidth;
        break;

      case 0x06:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY,    BMP_CX_FLIPX, BMP_CX_ROT_90 */                                                  
      case 0x08:   /*         H,           0,          1,      -dW,    BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_0 */ 
        pdRaster.p16 += ((sI32)Height * (sI32)dbiWidth) + (sI32)0;
        dPixelInc = 1;
        dLineInc = -dbiWidth;
        break;

      case 0x03:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_270 */                                                  
      case 0x0D:   /*         W,           0,        -dw,        1,    BMP_CX_FLIPY,    BMP_CX_FLIPX, BMP_CX_ROT_90 */ 
        pdRaster.p16 += ((sI32)Width * (sI32)dbiWidth) + (sI32)0;
        dPixelInc = -dbiWidth;
        dLineInc = 1;
        break;

      case 0x02:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_180 */                                                  
      case 0x0C:   /*         H,           W,         -1,      -dW,    BMP_CX_FLIPY,    BMP_CX_FLIPX, BMP_CX_ROT_0 */ 
        pdRaster.p16 += ((sI32)Height * (sI32)dbiWidth) + (sI32)Width;
        dPixelInc = -1;
        dLineInc = -dbiWidth;
        break;

      case 0x05:   /* row start, pixel start,   PixelInc,   LineInc NO BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_180 */                                                  
      case 0x0B:   /*         W,           H,        -dw,       -1,    BMP_CX_FLIPY, NO BMP_CX_FLIPX, BMP_CX_ROT_270 */ 
        pdRaster.p16 += ((sI32)Width * (sI32)dbiWidth) + (sI32)Height;
        dPixelInc = -dbiWidth;
        dLineInc = -1;
        break;
    }

    {
      sI16 RowIndex, PixelIndex;

      for (RowIndex = 0; RowIndex<Height; RowIndex++, psRaster.p16+=sbiWidth, pdRaster.p16+=dLineInc)
      {
        PIXEL_16bpp_type *pS = psRaster.p16, *pD = pdRaster.p16;
        for (PixelIndex = 0; PixelIndex < Width; PixelIndex++, pS++, pD+=dPixelInc)
        {
          *pD = *pS;
        }
      }
    }
  }
  return(0);
}


