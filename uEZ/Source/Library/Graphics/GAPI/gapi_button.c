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
FILE NAME    : GAPI_Button.c

DESCRIPTION  : BMP Button Support for Renesas Graphics API.

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
07.15.2008 RTA-JMB Addition to GAPI
***********************************************************************************/
#include <string.h>
#include "gapi.h"

/**********************************************************************************
Function Name:   LCDBMPColorCopy
Description:  Draw Grayscale Button Colorized
Parameters:   s: pointer to source button BMP
              d: pointer to destination BMP
              PosX: starting X coordinate within destination BMP (leftmost = 0)
              PosY: starting Y coordinate within destination BMP (bottommost = 0)
              pScheme: pointer to information about how to present the image
Return value:   0:OK, Not 0: NG
***********************************************************************************/
gapiResp_type LCDBMPColorCopy(BMP_type const *const s, BMP_type *d, sI16 PosX, sI16 PosY, GSCHEME_type const *pScheme)
{
  ColorTable_type *pCT=(ColorTable_type *)s->biColorTable;
  uI16 transparency=NO_TRANSPARENCY_COLOR;
  gapiResp_type status=0;
  int32_t allocated = 0;

  if((s==NULL) || (d==NULL))
    return(-1);
    
  if(0 != pScheme->Options.Colorize)
  {
    // Acquire scratchpad memory to create a color table
    pCT = (void *)gapiMalloc(1024);
    if(NULL == pCT)
       return(-1);
    allocated = 1;
  
    if(1 == pScheme->Options.Colorize)
      // generate for grayscale color table
      if (status == 0) status = LCDBMPCalcGradientCTN(pScheme->ct,0,pCT,256);
  
    if(2 == pScheme->Options.Colorize)
      // generate shaded color table
      if (status == 0) status = LCDBMPCalcShadeCT(s->biColorTable,pScheme->ct,pCT);
  }

  if(pScheme->Options.Transparency)
    transparency = (uI16)(BMP_IndexColors(s)-1);

  // Paint the button
  if(status == 0) status = LCDBMPCopySub(s, d, PosX, PosY, 0, 0, BMP_Width(s), BMP_Height(s), pCT, transparency);

  // Release scratchpad memory
  if (allocated)
    (void)gapiFree((void *)pCT);
  return (status);
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
gapiResp_type LCDBMPLabel(BMP_type const *const s, BMP_type *d, sI16 PosX, sI16 PosY, GSCHEME_type const *pScheme, uI08 const * label)
{
  gapiResp_type status=0;
  if((s==NULL) || (d==NULL))
    return(-1);
    
  if (pScheme->Options.Text)
  {
    // overlay text on the button...calculate label width
    sI16 width = LCDBMP_FontWidthString(label, &pScheme->font);

    if (0 != width)
    {
      // Determine position to center horizontal
      PosX += (BMP_Width(s) - width)/2;
      PosY += (BMP_Height(s) - LCDBMP_FontHeight(&pScheme->font))/2;
      if (0 != (status = LCDBMPGPutS(label, &pScheme->font, d, PosX, PosY)))
          return(status);
    }
  }
  return (status);
}

