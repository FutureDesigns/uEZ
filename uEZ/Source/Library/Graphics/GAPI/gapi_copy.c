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
***********************************************************************************/
//#include <machine.h>
#include <string.h>
#include "GAPI.H"

/**********************************************************************************
Function Name:   CopyLine_01_16
Description:  Helper function to extract width pixels within one line of source 1bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              count: number of pixels to copy
              ct: color table to use
              tr: transparency color for copy
Return value:   none
***********************************************************************************/
const uI08 BitMask[]= {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
static sI32 CopyLine_01_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08 + (sPosX/8);
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  uI08 bits = (uI08)(sPosX % 8);
  uI08 pixel;

  uI16 index;

  if (NO_TRANSPARENCY_COLOR == tr)
  {
    for (pixel = *ps; count > 0; count--, pd++, bits++)
    {
      bits &= 0x07;

      *pd = ((pixel & BitMask[bits]) == 0x00) ? ct[0] : ct[1];  /* select prebuilt pixel */

      if (bits == 0x07)
      {
        ps++;         /* increment source pointer if done processing pixel byte */
        pixel = *ps;  /* load next pixel byte */
      }
    }
  }
  else
  {
    for (pixel = *ps; count > 0; count--, pd++, bits++)
    {
      bits &= 0x07;

      index = ((pixel & BitMask[bits]) == 0x00) ? 0 : 1;  /* determine */

      /* write pixel if not tranparency */
      if (tr != index)
        *pd = ct[index];

      if (bits == 0x07)
      {
        ps++;         /* increment source pointer if done processing pixel byte */
        pixel = *ps;  /* load next pixel byte */
      }
    }
  }
  return(0);
}

/**********************************************************************************
Function Name:   CopyLine_04_16
Description:  Helper function to extract width pixels within one line of source 4bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              count: number of pixels to copy
              ct: color table to use
              tr: transparency color for copy
Return value:   none
***********************************************************************************/
static sI32 CopyLine_04_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08 + (sPosX/2);
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  uI08 bits = (uI08)(sPosX % 2);
  uI08 pixel;

  if (NO_TRANSPARENCY_COLOR == tr)
  {
    for (; count > 0; count--, pd++, bits++)
    {
      pixel = *ps;                      /* load pixel byte to temporary */

      if ((bits & 0x01) == 0x00)
      {
        pixel >>= 4;                    /* shift data down if high pixel bits */
      }
      else
      {
        pixel &= 0x0F;                  /* mask data if low pixel bits */
        ps++;                           /* increment source pointer if done processing this pixel byte */
      }

      *pd = ct[pixel];
    }
  }
  else
  {
    for (; count > 0; count--, pd++, bits++)
    {
      pixel = *ps;                      /* load pixel byte to temporary */

      if ((bits & 0x01) == 0x00)
      {
        pixel >>= 4;                    /* shift data down if high pixel bits */
      }
      else
      {
        pixel &= 0x0F;                  /* mask data if low pixel bits */
        ps++;                           /* increment source pointer if done processing this pixel byte */
      }

      /* write pixel if not tranparency */
      if (pixel != (uI08)tr)
        *pd = ct[pixel];
    }
  }
  return(0);
}

/**********************************************************************************
Function Name:   alpha_CT
Description:  Helper function to blend a 16bpp pixel with a color table pixel
Parameters:   p16: 16bpp source pixel
              alpha_mask: value to blend with 0=p16, 0xFF=color table value
              ct: color table to use...foreground color for alpha mask
Return value:   none
***********************************************************************************/
static const uI08 mul_5x5[32*32]=
{
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,
  0x00,0x01,0x02,0x03,0x04,0x05,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x18,0x19,0x1A,0x1B,0x1C,
  0x00,0x01,0x02,0x03,0x04,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x19,0x1A,0x1B,
  0x00,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0E,0x0F,0x10,0x11,0x12,0x13,0x13,0x14,0x15,0x16,0x17,0x18,0x18,0x19,0x1A,
  0x00,0x01,0x02,0x02,0x03,0x04,0x05,0x06,0x07,0x07,0x08,0x09,0x0A,0x0B,0x0B,0x0C,0x0D,0x0E,0x0F,0x0F,0x10,0x11,0x12,0x13,0x14,0x14,0x15,0x16,0x17,0x18,0x18,0x19,
  0x00,0x01,0x02,0x02,0x03,0x04,0x05,0x05,0x06,0x07,0x08,0x09,0x09,0x0A,0x0B,0x0C,0x0D,0x0D,0x0E,0x0F,0x10,0x10,0x11,0x12,0x13,0x14,0x14,0x15,0x16,0x17,0x17,0x18,
  0x00,0x01,0x02,0x02,0x03,0x04,0x05,0x05,0x06,0x07,0x08,0x08,0x09,0x0A,0x0B,0x0B,0x0C,0x0D,0x0E,0x0E,0x0F,0x10,0x11,0x11,0x12,0x13,0x14,0x14,0x15,0x16,0x17,0x17,
  0x00,0x01,0x01,0x02,0x03,0x04,0x04,0x05,0x06,0x06,0x07,0x08,0x09,0x09,0x0A,0x0B,0x0C,0x0C,0x0D,0x0E,0x0E,0x0F,0x10,0x11,0x11,0x12,0x13,0x13,0x14,0x15,0x16,0x16,
  0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x0A,0x0A,0x0B,0x0C,0x0C,0x0D,0x0E,0x0E,0x0F,0x10,0x11,0x11,0x12,0x13,0x13,0x14,0x15,0x15,
  0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x05,0x06,0x07,0x07,0x08,0x09,0x09,0x0A,0x0B,0x0B,0x0C,0x0C,0x0D,0x0E,0x0E,0x0F,0x10,0x10,0x11,0x12,0x12,0x13,0x14,0x14,
  0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09,0x0A,0x0B,0x0B,0x0C,0x0D,0x0D,0x0E,0x0E,0x0F,0x10,0x10,0x11,0x12,0x12,0x13,0x13,
  0x00,0x01,0x01,0x02,0x02,0x03,0x04,0x04,0x05,0x05,0x06,0x07,0x07,0x08,0x08,0x09,0x0A,0x0A,0x0B,0x0B,0x0C,0x0C,0x0D,0x0E,0x0E,0x0F,0x0F,0x10,0x11,0x11,0x12,0x12,
  0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x08,0x08,0x09,0x0A,0x0A,0x0B,0x0B,0x0C,0x0C,0x0D,0x0E,0x0E,0x0F,0x0F,0x10,0x10,0x11,0x11,
  0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x08,0x09,0x09,0x0A,0x0A,0x0B,0x0B,0x0C,0x0C,0x0D,0x0D,0x0E,0x0E,0x0F,0x0F,0x10,0x10,
  0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x08,0x08,0x09,0x09,0x0A,0x0A,0x0B,0x0B,0x0C,0x0C,0x0D,0x0D,0x0E,0x0E,0x0F,0x0F,0x10,
  0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x08,0x08,0x08,0x09,0x09,0x0A,0x0A,0x0B,0x0B,0x0C,0x0C,0x0D,0x0D,0x0E,0x0E,0x0F,
  0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x07,0x08,0x08,0x09,0x09,0x0A,0x0A,0x0B,0x0B,0x0B,0x0C,0x0C,0x0D,0x0D,0x0E,
  0x00,0x00,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x04,0x04,0x04,0x05,0x05,0x06,0x06,0x07,0x07,0x07,0x08,0x08,0x09,0x09,0x09,0x0A,0x0A,0x0B,0x0B,0x0B,0x0C,0x0C,0x0D,
  0x00,0x00,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x05,0x05,0x05,0x06,0x06,0x06,0x07,0x07,0x08,0x08,0x08,0x09,0x09,0x09,0x0A,0x0A,0x0B,0x0B,0x0B,0x0C,
  0x00,0x00,0x01,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x04,0x05,0x05,0x06,0x06,0x06,0x07,0x07,0x07,0x08,0x08,0x08,0x09,0x09,0x09,0x0A,0x0A,0x0A,0x0B,
  0x00,0x00,0x01,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x05,0x05,0x05,0x06,0x06,0x06,0x07,0x07,0x07,0x08,0x08,0x08,0x08,0x09,0x09,0x09,0x0A,
  0x00,0x00,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x06,0x06,0x06,0x06,0x07,0x07,0x07,0x08,0x08,0x08,0x08,0x09,
  0x00,0x00,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x06,0x06,0x06,0x06,0x07,0x07,0x07,0x07,0x08,0x08,
  0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x05,0x06,0x06,0x06,0x06,0x07,0x07,
  0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x05,0x05,0x06,0x06,
  0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,
  0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x04,
  0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03,0x03,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

/* for 8bpp operations */
#if 0
static INLINE uI16 alpha_CT8(uI16 p16, uI08 alpha_mask, uI16 const *const ct)
{
  switch (alpha_mask)
  {
    case 0:
      return(p16);
    case 0xFF:
      return(ct[0xFF]);
    default:
    {
      uI16 mul = (uI16)alpha_mask <<2 & 0x03E0;
      uI16 out;
      
#ifdef HW_BYTE_SWAPPED_BUS
      p16 = (p16 >> 8) | (p16 << 8);
#endif
      
      out = mul_5x5[mul+((p16 >> 11) & 0x1f)];
      out <<=5;
      out |= mul_5x5[mul+((p16 >> 6) & 0x1f)];
      out <<=6;
      out |= mul_5x5[mul+((p16 >> 0) & 0x1f)];
      
#ifdef HW_BYTE_SWAPPED_BUS
      {
        p16 = ct[alpha_mask];
        out +=  (p16 >> 8) | (p16 << 8);
        out = (out >> 8) | (out << 8);
      }
#else
      out += ct[alpha_mask];
#endif

      return(out);
    }
  }
  return(0);
}

/* for 4bpp operations */
static INLINE uI16 alpha_CT4(uI16 p16, uI08 alpha_mask, uI16 const *const ct)
{
  switch (alpha_mask)
  {
    case 0:
      return(p16);
    case 0xF:
      return(ct[0xF]);
    default:
    {
      uI16 mul = (uI16)alpha_mask <<6 & 0x03E0;
      uI16 out;
      
#ifdef HW_BYTE_SWAPPED_BUS
      p16 = (p16 >> 8) | (p16 << 8);
#endif
      
      out = mul_5x5[mul+((p16 >> 11) & 0x1f)];
      out <<=5;
      out |= mul_5x5[mul+((p16 >> 6) & 0x1f)];
      out <<=6;
      out |= mul_5x5[mul+((p16 >> 0) & 0x1f)];
      
#ifdef HW_BYTE_SWAPPED_BUS
      {
        p16 = ct[alpha_mask];
        out +=  (p16 >> 8) | (p16 << 8);
        out = (out >> 8) | (out << 8);
      }
#else
      out += ct[alpha_mask];
#endif

      return(out);
    }
  }
  return(0);
}

#else
#define alpha_CT8(a, b, c) alpha8_pixel565(a, c[0xff], b)
static INLINE uI16  alpha8_pixel565(uI16 BackPixel, uI16 ForePixel, uI08 alpha)
{
  switch (alpha)
  {
    case 0:
      return(BackPixel);
    case 0xFF:
      return(ForePixel);
    default:
    {
      uI16 mul;
      uI16 out;
      
      /* configure the multiplier...(1-alpha) for background (use 6 bits for multiplier) */
      mul = (uI16)(0xFF-alpha)>>2;
      /* mask off Red/Blue bits, multiply shift back and mask Red/Blue bits (6 bits * 5 bits = 11 bits (no overflow between color channels) */
      out = ((uI32)(mul * (BackPixel & 0xF81F))>>6) & 0xF81F;
      
      /* mask off green bits, multiply shift back and mask green bits */
      out += ((uI32)(mul * (BackPixel & 0x07E0))>>6) & 0x07E0;

      /* configure the multiplier (alpha) for foreground (use 6 bits for multiplier) */
      mul = (uI16)alpha>>2;
      
      /* mask off Red/Blue bits, multiply shift back and mask Red/Blue bits */
      out += ((uI32)(mul * (ForePixel & 0xF81F))>>6) & 0xF81F;
      
      /* mask off green bits, multiply shift back and mask green bits */
      out += ((uI32)(mul * (ForePixel & 0x07E0))>>6) & 0x07E0;

      return(out);
    }
  }
  return(0);
}

#define alpha_CT4(a, b, c) alpha4_pixel565(a, c[0xf], b)
static INLINE uI16  alpha4_pixel565(uI16 BackPixel, uI16 ForePixel, uI08 alpha)
{
  switch (alpha)
  {
    case 0:
      return(BackPixel);
    case 0xF:
      return(ForePixel);
    default:
    {
      uI16 mul;
      uI16 out;
      
      /* configure the multiplier...(1-alpha) for background (use 4 bits for multiplier) */
      mul = (uI16)(0xF-alpha);
      /* mask off Red/Blue bits, multiply shift back and mask Red/Blue bits (4 bits * 5 bits = 9 bits (no overflow between color channels) */
      out = ((uI32)(mul * (BackPixel & 0xF81F))>>4) & 0xF81F;
      
      /* mask off green bits, multiply shift back and mask green bits */
      out += ((uI32)(mul * (BackPixel & 0x07E0))>>4) & 0x07E0;

      /* configure the multiplier (alpha) for foreground (use 6 bits for multiplier) */
      mul = (uI16)alpha;
      
      /* mask off Red/Blue bits, multiply shift back and mask Red/Blue bits */
      out += ((uI32)(mul * (ForePixel & 0xF81F))>>4) & 0xF81F;
      
      /* mask off green bits, multiply shift back and mask green bits */
      out += ((uI32)(mul * (ForePixel & 0x07E0))>>4) & 0x07E0;

      return(out);
    }
  }
  return(0);
}
#endif



/**********************************************************************************
Function Name:   AlphaLine_04_16
Description:  Helper function to blend width pixels within one line of source 4bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line (alpha mask)
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              count: number of pixels to copy
              ct: color table to use...foreground color for alpha mask
              tr: not used
Return value:   none
***********************************************************************************/
static sI32 AlphaLine_04_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08 + (sPosX/2);
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  uI08 bits = (uI08)(sPosX % 2);
  uI08 pixel;

  for (; count > 0; count--, pd++, bits++)
  {
    pixel = *ps;                      /* load pixel byte to temporary */

    if ((bits & 0x01) == 0x00)
    {
      pixel >>= 4;                    /* shift data down if high pixel bits */
    }
    else
    {
      pixel &= 0x0F;                  /* mask data if low pixel bits */
      ps++;                           /* increment source pointer if done processing this pixel byte */
    }

    *pd = alpha_CT4(*pd, pixel, ct);
  }
  return(0);
}

/**********************************************************************************
Function Name:   AlphaLine_04rle_16
Description:  Helper function to blend width pixels within one line of source 
              Proprietary Run Length Encoded 4bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              Width: number of pixels to copy
              ct: color table to use
              tr: not used
Return value:   number of source bytes used
***********************************************************************************/
static sI32 AlphaLine_04rle_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 Width, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08;
  sI16 IndexW = 0;

  /* advance destination to X offset */
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  /* cycle through RLE records */
  for(;;)
  {
    switch (*ps)
    {
      case 0:
      {
        /* new line...fill to end with 0 value (transparency copy) */
        for (; IndexW < (sPosX+Width); IndexW++, pd++)
          *pd = alpha_CT4(*pd, 0, ct);

        /* increment source pointer */
        ps += 1;

        /* line complete, so return */
        return((sI32)(ps - psr.p08));
      }
      case 1:
      {
        /* end of BMP */
        return(0L);
      }
      case 2:
      {
        /* offset x,y (not sure how to handle this because of line skipping) */
        ps += 1;
        break;
      }
      default:
      {
        sI16 start, stop;
        uI08 RLEr[16];

        if (*ps < 0x10)
        {
          /* discrete pixels (in 2x4 bit pairs) */
          start = 0;
          stop = (sI16)(*ps & 0x0F);
        }
        else
        {
          /* repeat record */
          start = 0;
          stop = (sI16)(*ps >> 4) & 0x0F;
        }

        /* check if we'll start copying in this record */
        if ((IndexW < sPosX) && ((IndexW+stop) >= sPosX))
          start = (sPosX - IndexW);

        /* check if we'll stop copying in this record */
        if ((IndexW < (sPosX+Width)) && ((IndexW+stop) >= (sPosX+Width)))
          stop -= (IndexW + stop) - (sPosX+Width);

        /* update the output raster...if we're still copying */
        if (IndexW < (sPosX+Width))
        {
          if (*ps < 0x10)
          {
            /* discrete pixels (in 2x4 bit pairs) */
            uI08 index;
            /* expand RLE record for easy access */
            for (index = 0; index < 8; index++)
            {
              RLEr[index*2] = (ps[1+index] >> 4) & 0x0F;
              RLEr[index*2+1] = (ps[1+index] ) & 0x0F;
            }
            /* increment index position and pointer...extra + 1 for "0:size" */
            index = *ps & 0x0F;
            IndexW += index;
            ps += (((index-1)/2)+1+1);
          }
          else
          {
            /* repeat record */
            uI08 index;
            /* expand RLE record for easy access */
            for (index = 0; index < 16; index++)
            {
              RLEr[index] = *ps & 0x0F;
            }
            /* increment index position and source pointer */
            IndexW += (*ps >> 4) & 0x0F;
            ps += 1;
          }

          for ( ; start<stop; start++, pd++)
            *pd = alpha_CT4(*pd, RLEr[start], ct);
        }
        else
        {
          /* not updating raster, but still must update source pointer */
          if (*ps < 0x10)
            ps += ((((*ps & 0x0F)-1)/2)+1+1);
          else
            ps += 1;
        }

        break;
      } /* default */

    } /* case */
  } /* for */
}

/**********************************************************************************
Function Name:   AlphaLine_08_16
Description:  Helper function to blend width pixels within one line of source 8bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line (alpha mask)
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              count: number of pixels to copy
              ct: color table to use...foreground color for alpha mask
              tr: not used
Return value:   none
***********************************************************************************/
static sI32 AlphaLine_08_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08 + sPosX;
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  for (; count > 0; count--, ps++, pd++)
    *pd = alpha_CT8(*pd, *ps, ct);
  return(0);
}

/**********************************************************************************
Function Name:   AlphaLine_08rle_16
Description:  Helper function to blend width pixels within one line of source 
              Run Length Encoded 8bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              Width: number of pixels to copy
              ct: color table to use
              tr: not used
Return value:   number of source bytes used
***********************************************************************************/
static sI32 AlphaLine_08rle_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 Width, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08;
  sI16 IndexW = 0;

  /* advance destination to X offset */
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  /* cycle through RLE records */
  for(;;)
  {
    /* repeat pixel record */
    if (ps[0] != 0)
    {
      sI16 start = 0, stop = (sI16)ps[0];

      /* check if we'll start copying in this record */
      if ((IndexW < sPosX) && ((IndexW+stop) >= sPosX))
        start = (sPosX - IndexW);

      /* check if we'll stop copying in this record */
      if ((IndexW < (sPosX+Width)) && ((IndexW+stop) >= (sPosX+Width)))
        stop -= (IndexW + stop) - (sPosX+Width);

      /* update the output raster...if we're still copying */
      if (IndexW < (sPosX+Width))
      {
        /* copy pixels because not transparent */
        for ( ; start<stop; start++, pd++)
          *pd = alpha_CT8(*pd, ps[1], ct);
      }

      /* increment index position and source pointer */
      IndexW += ps[0];
      ps += 2;
    }
    else
    {
      /* escape sequences */
      switch (ps[1])
      {
        case 0:
        {
          /* new line...fill to end with 0 value (transparency copy) */
          for (; IndexW < (sPosX+Width); IndexW++, pd++)
            *pd = alpha_CT8(*pd, 0, ct);

          /* increment source pointer */
          ps += 2;

          /* line complete, so return */
          return((sI32)(ps - psr.p08));
        }
        case 1:
        {
          /* end of BMP */
          return(0L);
        }
        case 2:
        {
          /* offset x,y (not sure how to handle this because of line skipping) */
          ps += 2;
          break;
        }
        default:
        {
          /* discrete pixels (in 16 bit pairs) */
          sI16 start = 0, stop = (sI16)ps[1];

          /* check if we'll start copying in this record */
          if ((IndexW < sPosX) && ((IndexW+stop) >= sPosX))
            start = (sPosX - IndexW);

          /* check if we'll stop copying in this record */
          if ((IndexW < (sPosX+Width)) && ((IndexW+stop) >= (sPosX+Width)))
            stop -= (IndexW + stop) - (sPosX+Width);

          /* update the output raster...if we're still copying */
          if (IndexW < (sPosX+Width))
          {
            for ( ; start<stop; start++, pd++)
              *pd = alpha_CT8(*pd, ps[start+2], ct);
          }
  
          /* increment index position and pointer...extra + 1 for "0:size" */
          IndexW += ps[1];
          ps += (((ps[1]-1)/2)+1+1)*2;
          break;
        } /* default */
      } /* switch */
    } /* if */
  } /* for */
}

/**********************************************************************************
Function Name:   CopyLine_08_16
Description:  Helper function to extract width pixels within one line of source 8bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              count: number of pixels to copy
              ct: color table to use
              tr: transparency color for copy
Return value:   none
***********************************************************************************/
static sI32 CopyLine_08_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08 + sPosX;
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  uI08 pixel;

  if (NO_TRANSPARENCY_COLOR == tr)
  {
    for (; count > 0; count--, ps++, pd++)
    {
      *pd = ct[*ps];  /* obtain source pixel */
    }
  }
  else
  {
    for (; count > 0; count--, ps++, pd++)
    {
      pixel = *ps;  /* obtain source pixel */

      /* write pixel if not tranparency */
      if (pixel != (uI08)tr)
        *pd = ct[pixel];
    }
  }
  return(0);
}

/**********************************************************************************
Function Name:   CopyLine_16_16
Description:  Helper function to extract width pixels within one line of source 16bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              count: number of pixels to copy
              ct: color table to use
              tr: transparency color for copy
Return value:   none
***********************************************************************************/
//lint -e{715} suppress ct not used
static sI32 CopyLine_16_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  PIXEL_16bpp_type const *ps = psr.p16 + sPosX;
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  if (tr != NO_TRANSPARENCY_COLOR)
  {
    for (; count > 0; count--, ps++, pd++)
    {
      /* write pixel */
      if (*ps != tr)
        *pd = *ps;
    }
  }
  else
  {
    /* using memcpy takes advantage of H8SX movm instructions */
    (void)memcpy(pd, ps, count*2);
  }
  return(0);
}

static sI32 CopyLine_16_16_Flip(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08 + (sPosX * 2);
  uI08 *pd = pdr.p08 + (dPosX * 2);

  if (tr != NO_TRANSPARENCY_COLOR)
  {
    for (; count > 0; count--)
    {
      /* write pixel */
      if (*(uI16 *)ps != tr)
      {
        pd[0] = ps[1];
        pd[1] = ps[0];
        ps += 2;
        pd += 2;
      }
    }
  }
  else
  {
    for (; count > 0; count--)
    {
      /* write pixel */
      pd[0] = ps[1];
      pd[1] = ps[0];
      ps += 2;
      pd += 2;
    }
  }
  return(0);
}

/**********************************************************************************
Function Name:   CopyLine_24_16
Description:  Helper function to extract width pixels within line of source 24bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              count: number of pixels to copy
              ct: color table to use
              tr: transparency color for copy
Return value:   none
***********************************************************************************/
//lint -e{715} suppress ct not used
static sI32 CopyLine_24_16(pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr)
{
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  uI16 pixel16;

  psr.p24 += sPosX;
  if (tr != NO_TRANSPARENCY_COLOR)
  {
    for (; count > 0; count--, psr.p24++, pd++)
    {
      pixel16 = LCDBMP24_16( psr.pCT );  /* convert source pixel */
      if (pixel16 != tr)
        *pd = pixel16;
    }
  }
  else
  {
    for (; count > 0; count--, psr.p24++, pd++)
    {
      pixel16 = LCDBMP24_16( psr.pCT );  /* convert source pixel */
      *pd = pixel16;
    }
  }
  return(0);
}

/**********************************************************************************
Function Name:   AddLine_04rle
Description:  Helper function to offset number of "lines" into RLE bpp4 source image
Parameters:   psr: pointer to source BMP line
              Height: number of lines to seek end of
Return value:   number of source bytes used
              NOTE: THIS IS NOT STANDARD 4BPP RLE SUPPORT
***********************************************************************************/
static sI32 AddLine_04rle(const pRaster_type psr, sI16 Height)
{
  uI08 const *ps = psr.p08;

  /* support 4bpp RLE compressed BMP */
  /* advance to Y offset line */
  for (; Height > 0; ps++)
  {
    if (*ps == 0x00)
    {
      /* found end of line marker */
      Height--;
    }
    if (*ps == 0x01)
    {
      /* end of BMP...nothing written */
      return(0);
    }
  } /* for */
  return((sI32)(ps - psr.p08));
}

/**********************************************************************************
Function Name:   AddLine_08rle
Description:  Helper function to offset number of "lines" into RLE bpp8 source image
Parameters:   psr: pointer to source BMP line
              Height: number of lines to seek end of
Return value:   number of source bytes used
***********************************************************************************/
static sI32 AddLine_08rle(const pRaster_type psr, sI16 Height)
{
  uI08 const *ps = psr.p08;

  /* support 8bpp RLE compressed BMP */
  /* advance to Y offset line */
  for (; Height > 0; )
  {
    if (ps[0] != 0)
    {
      /* repeat pixel record */
      ps += 2;
    }
    else
    {
      /* escape sequences */
      switch (ps[1])
      {
        case 0:
        {
          /* new line */
          ps += 2;
          Height--;
          break;
        }
        case 1:
        {
          /* end of BMP...nothing written */
          return(0);
        }
        case 2:
        {
          /* offset x,y (not sure how to handle this because of line skipping) */
          ps += 2;
          break;
        }
        default:
        {
          /* discrete bits (in 16 bit pairs) extra + 1 for "0:size" */
          ps += (((ps[1]-1)/2)+1+1)*2;
          break;
        } /* default */
      } /* switch */
    } /* if/else */
  } /* for */
  return((sI32)(ps - psr.p08));
}

/**********************************************************************************
Function Name:   CopyLine_08rle_16
Description:  Helper function to extract width pixels within one line of source 
              Run Length Encoded 8bpp image to one 16bpp line at destination
Parameters:   psr: pointer to source BMP line
              sPosX: offset within source line to copy from
              pdr: pointer to destination BMP line
              dPosX: offset within destination line to copy to
              Width: number of pixels to copy
              ct: color table to use
              tr: transparency color for copy
Return value:   number of source bytes used
***********************************************************************************/
static sI32 CopyLine_08rle_16(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 Width, uI16 const *const ct, const uI16 tr)
{
  uI08 const *ps = psr.p08;
  sI16 IndexW = 0;

  /* advance destination to X offset */
  PIXEL_16bpp_type *pd = pdr.p16 + dPosX;

  /* cycle through RLE records */
  for(;;)
  {
    /* repeat pixel record */
    if (ps[0] != 0)
    {
      sI16 start = 0, stop = (sI16)ps[0];
      uI16 pixel16 = ct[ps[1]];

      /* check if we'll start copying in this record */
      if ((IndexW < sPosX) && ((IndexW+stop) >= sPosX))
        start = (sPosX - IndexW);

      /* check if we'll stop copying in this record */
      if ((IndexW < (sPosX+Width)) && ((IndexW+stop) >= (sPosX+Width)))
        stop -= (IndexW + stop) - (sPosX+Width);

      /* update the output raster...if we're still copying */
      if (IndexW < (sPosX+Width))
      {
        if (ps[1] != tr)
        {
          /* copy pixels because not transparent */
          for ( ; start<stop; start++, pd++)
            *pd = pixel16;
        }
        else
        {
          /* else just jump over */
          pd += stop-start;
        }
      }

      /* increment index position and source pointer */
      IndexW += ps[0];
      ps += 2;
    }
    else
    {
      /* escape sequences */
      switch (ps[1])
      {
        case 0:
        {
          /* new line...fill to end with 0 value (if not 0=transparent) */
          uI16 pixel16 = ct[0];
          if (0 != tr)
          {
            for (; IndexW < (sPosX+Width); IndexW++, pd++)
              *pd = pixel16;
          }

          /* increment source pointer */
          ps += 2;

          /* line complete, so return */
          return((sI32)(ps - psr.p08));
        }
        case 1:
        {
          /* end of BMP */
          return(0L);
        }
        case 2:
        {
          /* offset x,y (not sure how to handle this because of line skipping) */
          ps += 2;
          break;
        }
        default:
        {
          /* discrete pixels (in 16 bit pairs) */
          sI16 start = 0, stop = (sI16)ps[1];

          /* check if we'll start copying in this record */
          if ((IndexW < sPosX) && ((IndexW+stop) >= sPosX))
            start = (sPosX - IndexW);

          /* check if we'll stop copying in this record */
          if ((IndexW < (sPosX+Width)) && ((IndexW+stop) >= (sPosX+Width)))
            stop -= (IndexW + stop) - (sPosX+Width);

          /* update the output raster...if we're still copying */
          if (IndexW < (sPosX+Width))
          {
            for ( ; start<stop; start++, pd++)
            {
              uI08 pixel = ps[start+2];
              if(pixel != tr)
                *pd = ct[pixel];
            }
          }
  
          /* increment index position and pointer...extra + 1 for "0:size" */
          IndexW += ps[1];
          ps += (((ps[1]-1)/2)+1+1)*2;
          break;
        } /* default */
      } /* switch */
    } /* if */
  } /* for */
}

/**********************************************************************************
Function Name:   BMPCopySub
Description:  BMP to BMP copy with additional hooks
Parameters:   s: pointer to source BMP
              d: pointer to destination BMP
              dPosX: starting X coordinate within destination BMP (leftmost = 0)
              dPosY: starting Y coordinate within destination BMP (bottommost = 0)
              sPosX: starting X coordinate within source BMP (leftmost = 0)
              sPosY: starting Y coordinate within source BMP (bottommost = 0)
              Width: "width" of source area to copy
              Height: "height" of source area to copy
              ct: Pointer to color table to use (allows substituion of source color tables)
              tr: Transparency color to use if NO_TRANSPARENCY_COLOR no transparency handling is used
Return value:   0:OK, Not 0: NG
***********************************************************************************/
gapiResp_type LCDBMPCopySub( BMP_type const *const s, BMP_type *const d, sI16 dPosX, sI16 dPosY, sI16 sPosX, sI16 sPosY, sI16 Width, sI16 Height, ColorTable_type const *const ct, uI16 tr )
{
  const sI16 sbiHeight = BMP_Height(s);
  sI16 sbiWidth = BMP_Width(s);
  int32_t invertSY = 0;
  int32_t invertDY = 0;
  const sI16 dbiHeight = BMP_Height(d);
  sI16 dbiWidth = BMP_Width(d);

  uI16 ct_size=0;

  pRaster_type psRaster, pdRaster;
  sI32 (*copy_func)(const pRaster_type psr, const sI16 sPosX, pRaster_type pdr, const sI16 dPosX, sI16 count, uI16 const *const ct, const uI16 tr) = NULL;

  if((s==NULL) || (d==NULL))
    return(-1);
    
  if (d->bfReserved[0] & 0x01)
      invertDY = 1;
  if (s->bfReserved[0] & 0x01)
      invertSY = 1;

  /* only handling 16bpp native destination */
  if ((d->biBitCount[0] != 16) || !isBMPF(d,NATIVE))
    return(-1);

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

  if ((Height < 0) || (Width < 0))
    return(-1);

  /* convert width in pixels to width in bytes */
  dbiWidth = (sI16)((((sI32)dbiWidth * (sI32)d->biBitCount[0] - 1L)/8L) + 1L);
  sbiWidth = (sI16)((((sI32)sbiWidth * (sI32)s->biBitCount[0] - 1L)/8L) + 1L);
  
  /* create a quad byte line width for "standard" BMPs */
  if (!isBMPF(d, NO_QUAD))
    dbiWidth = (((dbiWidth - 1)/4) + 1) * 4;
  
  if (!isBMPF(s, NO_QUAD))
    sbiWidth = (((sbiWidth - 1)/4) + 1) * 4;

  /* point to source raster data */
  psRaster.p08 = ((uI08 *)s) + BMP_Offset(s);

  /* point to destination raster data */
  pdRaster.p08 = ((uI08 *)d) + BMP_Offset(d);

  /* point to first destination line */
  if (invertDY)
      pdRaster.p08 += (sI32)dbiWidth * (sI32)(dbiHeight-1-dPosY);
  else
      pdRaster.p08 += (sI32)dbiWidth * (sI32)dPosY;

  /* determine how to handle source file */
  switch (s->biCompression[0])
  {
    case 0:
    {
    
      /* BI_RGB */
      switch (s->biBitCount[0])
      {
        case 1:
        {
          /* Standard 1BPP raster data */
          copy_func = CopyLine_01_16;
          ct_size = 2;
          break;
        }

        case 4:
        {
          if (isBMPF(s,4BPP_RLE))
          {
            /* Proprietary 4BPP raster data (for alpha mask) */
            copy_func = AlphaLine_04rle_16;
            /* point to first source line */
            if (invertSY)
                psRaster.p08 += AddLine_04rle(psRaster, (sbiHeight-1-sPosY));
            else
                psRaster.p08 += AddLine_04rle(psRaster, sPosY);
          }
          else
          {
            /* Standard 4BPP raster data */
            if (isBMPF(s,ALPHA_MASK))
              copy_func = AlphaLine_04_16;
            else
              copy_func = CopyLine_04_16;
          }
          ct_size = 16;
          break;
        }

        case 8:
        {
          /* Standard 8BPP raster data */
          if (isBMPF(s,ALPHA_MASK))
            copy_func = AlphaLine_08_16;
          else
            copy_func = CopyLine_08_16;
          ct_size = 256;
          break;
        }

        case 24:
        {
          /* Standard 24BPP raster data */
          copy_func = CopyLine_24_16;
          break;
        }

        default:
          return(-1);
      }

      /* point to first source line */
      if (invertSY)
          psRaster.p08 += (sI32)sbiWidth * (sI32)(sbiHeight-1-sPosY);
      else
          psRaster.p08 += (sI32)sbiWidth * (sI32)sPosY;
      break;
    }
    
    case 1:
    {
      /* BI_RLE8 */
      if (s->biBitCount[0] != 8)
        return(-1);

      if (isBMPF(s,ALPHA_MASK))
        copy_func = AlphaLine_08rle_16;
      else
        copy_func = CopyLine_08rle_16;

      ct_size = 256;

      /* point to first source line */
      if (invertSY)
          psRaster.p08 += AddLine_08rle(psRaster, sbiHeight-1-sPosY);
      else
          psRaster.p08 += AddLine_08rle(psRaster, sPosY);
      break;
    }
    
    case 3:
    {
      /* BI_BITFIELDS */
      if (s->biBitCount[0] != 16)
        return(-1);

      if (isBMPF(s,NATIVE))
      {
        copy_func = CopyLine_16_16;
#if 0
        {
          int32_t LCDBLT(uI16 *sB, sI16 sW, sI16 sH, sI16 sR, uI16 *dB, sI16 dW, sI16 dH, sI16 dR);
          (void)LCDBLT(psRaster.p16, Width, Height, sbiWidth, pdRaster.p16, Width, Height, dbiWidth);
          return(0);
        }
#endif      
      }
      else
      {    
#ifdef HW_BYTE_SWAPPED_BUS
        copy_func = CopyLine_16_16;
#else
        copy_func = CopyLine_16_16_Flip;
#endif
      }
      
      /* point to first source line */
      if (invertSY)
          psRaster.p08 += (sI32)sbiWidth * (sI32)(sbiHeight-1-sPosY);
      else
          psRaster.p08 += (sI32)sbiWidth * (sI32)sPosY;

      break;
    }
        
    default:
   {
      /* unsupported */
      return(-1);
    
    }
  }

  {
    uI16 *pCT16 = (uI16 *) ct;
    uI16 *pLocalCT16 = NULL;
    uI16 index;
    
    /* Create a 16bpp color table (if not already 16bpp color table)
      ...this greatly accelerates most BMP operations */
    if (!isBMPF(s,16BPP_CT))
    {
      // Acquire scratchpad memory to create a 16bpp color table
      pLocalCT16 = (void *)gapiMalloc(1024);

      if(pLocalCT16 == NULL)
        return(-1);
        
      /* assign 16bpp color table to this memory */
      pCT16 = pLocalCT16;
      /* convert the color table */
      for (index=0; index<ct_size; index++)
        pCT16[index] = LCDBMP24_16(&ct[index]);
    }
  
    for ( ; Height > 0; Height-- )
    { 
      /* copy width count of pixels */
      const sI32 value = copy_func(psRaster, sPosX, pdRaster, dPosX, (sI16)Width, pCT16, tr);

      /* increment source pointer */
      if ((s->biCompression[0] == 1) || isBMPF(s,4BPP_RLE)) {
        psRaster.p08 += value;        /* RLE */
      } else {
          if (invertSY)
              psRaster.p08 -= sbiWidth; /* non-RLE */
          else
              psRaster.p08 += sbiWidth; /* non-RLE */
      }

      /* increment destination pointer */
      if (invertDY)
          pdRaster.p08 -= dbiWidth;
      else
          pdRaster.p08 += dbiWidth;
    }
    (void)gapiFree((void *)pLocalCT16);
  }

  return(0);
}

