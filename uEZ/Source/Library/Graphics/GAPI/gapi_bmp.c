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
FILE NAME    : GAPI_BMP.c

DESCRIPTION  : BMP Information Support for Renesas Graphics API.

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
#include "GAPI.H"

/**********************************************************************************
Function Name:   LCDInitFrame
Description:  Create a BMP header in destination
Parameters:   d: pointer to destination BMP
              r: pointer to raster
              bpp: bits per pixel of BMP 
              (it is assumed color table will be outside of BMP...in RAM)
              Width: width of BMP in pixels
              Height: height of BMP in pixels
Return value:   0:OK, Not 0: NG
***********************************************************************************/

static const BMP_type BMP_header={
{0x42, 0x4D},                 /* bfSignature[2]:            "BM"                                    */
{0x48, 0x58, 0x02, 0x00},     /* bfFileSize[4]:             0x00025848 <153,672 bytes files size>   */
{0x00, 0x00, 0x00, 0x00},     /* bfReserved[4]:             0x00000000                              */
{0x3A, 0x00, 0x00, 0x00},     /* bfDataOffset[4]:           0x0000003A <58 bytes offset to raster>  */
{0x38, 0x00, 0x00, 0x00},     /* biSize[4]:                 0x00000038 <56 bytes header size>       */
{0x40, 0x01, 0x00, 0x00},     /* biWidth[4]:                0x00000140 <320 bytes wide>             */
{0xF0, 0x00, 0x00, 0x00},     /* biHeight[4]:               0x000000F0 <240 bytes high>             */
{0x01, 0x00},                 /* biPlanes[2]:                   0x0001 <1 plane>                    */
{0x10, 0x00},                 /* biBitCount[2]:                 0x0010 <16bpp>                      */
{0x00, 0x00, 0x00, 0x00},     /* biCompression[4]:          0x00000000 <BIT fields...no compress>   */
{0x02, 0x58, 0x02, 0x00},     /* biImageSize[4]:            0x00025802 <153,602 bytes raster size>  */
{0x12, 0x0B, 0x00, 0x00},     /* biXPixelsPerMeter[4]:      0x00000B12 <2834 pixel/m, 72 pixel/inch */
{0x12, 0x0B, 0x00, 0x00},     /* biYPixelsPerMeter[4]:      0x00000B12 <2834 pixel/m, 72 pixel/inch */
{0x00, 0x00, 0x00, 0x00},     /* biClrUsed[4]:              0x00000000 <0 index colors used>        */
{0x00, 0x00, 0x00, 0x00},     /* biClrImportant[4]:         0x00000000 <0 index colors important>   */
{{0x00, 0xF8, 0x00, 0x00}}    /* Table_type biColorTable[4]:           <Bit field RGB definition>   */        
};

PACK_STRUCT_BEGIN
typedef union
{
        PACK_STRUCT_FIELD(unsigned long w);
        PACK_STRUCT_FIELD(unsigned char b[4]);
} PACK_STRUCT_STRUCT tWordWrite;
PACK_STRUCT_END

volatile unsigned long G_debugValue;

static void INLINE WordWrite(tWordWrite const *const input, uI32 *output)
{
  ((tWordWrite *)output)->b[0] = input->b[0];
  ((tWordWrite *)output)->b[1] = input->b[1];
  ((tWordWrite *)output)->b[2] = input->b[2];
  ((tWordWrite *)output)->b[3] = input->b[3];
}

gapiResp_type LCDInitFrame(BMP_type *const d, uI32 r, const uI08 bpp, const sI16 sWidth, const sI16 sHeight)
{
  uI32 value;
  tWordWrite temp;
  uI16 Width = (uI16)sWidth;
  uI16 Height = (uI16)sHeight;
  
  /* copy over a baseline of header */
  *d = BMP_header;

  /* check bpp */
  switch(bpp)
  {
    case 24:
      d->biBitCount[0] = 24;
      break;
    case 16:
      d->biBitCount[0] = 16;
      d->biCompression[0] = 0x3;  /* 16bpp */
      setBMPF(d,NATIVE);          /* stored "native" endian format */
      break;
    case 8:
      d->biBitCount[0] = 8;
      setBMPF(d,NO_QUAD);         /* stored with no quad restriction */
      break;
    case 4:
      d->biBitCount[0] = 4;
      setBMPF(d,NO_QUAD);         /* stored with no quad restriction */
      break;
    case 1:
      d->biBitCount[0] = 1;
      setBMPF(d,NO_QUAD);         /* stored with no quad restriction */
      break;
    default:
      return(-1);
  }
  
  temp.b[0] = (uI08)(Width >> 0);
  temp.b[1] = (uI08)(Width >> 8);
  temp.b[2] = 0;
  temp.b[3] = 0;
  WordWrite(&temp, (uI32 *) d->biWidth); 
  
  temp.b[0] = (uI08)(Height >> 0);
  temp.b[1] = (uI08)(Height >> 8);
  temp.b[2] = 0;
  temp.b[3] = 0;
  WordWrite(&temp, (uI32 *) d->biHeight); 
  
  /* determine number of longwords in file size */
  value = ((((uI32)Width * (uI32)Height * (uI32)bpp) + ((uI32)sizeof(BMP_header) * 8) -1)/32)+1;
  /* number of bytes in file size */
  value *= 4UL;

  temp.b[0] = (uI08)(value >> 0);
  temp.b[1] = (uI08)(value >> 8);
  temp.b[2] = (uI08)(value >>16);
  temp.b[3] = (uI08)(value >>24);
  WordWrite(&temp, (uI32 *) d->bfFileSize); 
 
  value -= sizeof(BMP_header);

  temp.b[0] = (uI08)(value >> 0);
  temp.b[1] = (uI08)(value >> 8);
  temp.b[2] = (uI08)(value >>16);
  temp.b[3] = (uI08)(value >>24);
  WordWrite(&temp, (uI32 *) d->biImageSize); 

  value = (uI32)r - (uI32)d;
  temp.b[0] = (uI08)(value >> 0);
  temp.b[1] = (uI08)(value >> 8);
  temp.b[2] = (uI08)(value >>16);
  temp.b[3] = (uI08)(value >>24);
  WordWrite(&temp, (uI32 *) d->bfDataOffset); 

  d->bfReserved[0] |= 0x01; // mark it as reversed Y (special internal flag)

  return(0);
}

/**********************************************************************************
Function Name:   BMP_xxxx
Description:  Provide information about a BMP
Parameters:   pBmp: pointer to BMP
Return value:   Attribute requested from BMP
***********************************************************************************/
sI16 BMP_Height(BMP_type const *const pBmp)
{
  if(pBmp == NULL)
    return(0);
  return((sI16)(((uI16)pBmp->biHeight[1] << 8) + (uI16)pBmp->biHeight[0]));
}

sI16 BMP_Width(BMP_type const *const pBmp)
{
  if(pBmp == NULL)
    return(0);
  return((sI16)(((uI16)pBmp->biWidth[1] << 8) + (uI16)pBmp->biWidth[0]));
}

sI32 BMP_Offset(BMP_type const *const pBmp)
{
  if(pBmp == NULL)
    return(0);
  return((sI32)(((uI32)pBmp->bfDataOffset[3] << 24)+((uI32)pBmp->bfDataOffset[2] << 16)+((uI32)pBmp->bfDataOffset[1] << 8)+((uI32)pBmp->bfDataOffset[0] << 0)));
}

uI32 BMP_FileSize(BMP_type const *const pBmp)
{
  if(pBmp == NULL)
    return(0);
  return(((uI32)pBmp->bfFileSize[3] << 24)+((uI32)pBmp->bfFileSize[2] << 16)+((uI32)pBmp->bfFileSize[1] << 8)+((uI32)pBmp->bfFileSize[0] << 0));
}

uI32 BMP_IndexColors(BMP_type const *const pBmp)
{
  uI32 biClrImportant;
  if(pBmp == NULL)
    return(0);

  biClrImportant = ((uI32)pBmp->biClrImportant[3] << 24)+((uI32)pBmp->biClrImportant[2] << 16)+((uI32)pBmp->biClrImportant[1] << 8)+((uI32)pBmp->biClrImportant[0] << 0);
  if (0 == biClrImportant)
  {
    /* Check if this is an indexed format */
    if(pBmp->biBitCount[0] <= 8)
      biClrImportant = 1UL << pBmp->biBitCount[0];
  }
  return(biClrImportant);
}


