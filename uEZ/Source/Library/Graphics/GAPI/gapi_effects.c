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
#include "GAPI.H"

/**********************************************************************************
Function Name:   LCDBMPScroll
Description:  Paints the requested BMP by "splitting" image vertically or horizontally.
              Called repeatedely, this can create a "scrolling" effect.
Parameters:   s: pointer to source BMP                                            
              d: pointer to destination BMP                                       
              dPosX: starting X coordinate within destination BMP (leftmost = 0)  
              dPosY: starting Y coordinate within destination BMP (bottommost = 0)
              split: split location within bitmap                                 
              type: 0=horizontal split, 1=vertical split                                        
Return value:   none
***********************************************************************************/
void LCDBMPScroll(BMP_type const *s, BMP_type *d, sI16 dPosX, sI16 dPosY, sI16 split, uI16 type)
{
  sI16 sbiHeight = BMP_Height(s);
  sI16 sbiWidth = BMP_Width(s);

  if((s==NULL) || (d==NULL))
    return;
    
  if (type == 0)
  {
    /* horizontal split */
    split = split % sbiWidth;
    (void)LCDBMPCopySub(s, d, dPosX,                dPosY, split, 0, sbiWidth-split, sbiHeight,s->biColorTable , NO_TRANSPARENCY_COLOR );
    (void)LCDBMPCopySub(s, d, dPosX+sbiWidth-split, dPosY, 0,     0, split,          sbiHeight,s->biColorTable , NO_TRANSPARENCY_COLOR );
  }
  else
  {
    /* vertical split */
    split = split % sbiHeight;
    (void)LCDBMPCopySub(s, d, dPosX, dPosY,                 0, split, sbiWidth, sbiHeight-split,s->biColorTable , NO_TRANSPARENCY_COLOR );
    (void)LCDBMPCopySub(s, d, dPosX, dPosY+sbiHeight-split, 0,     0, sbiWidth, split,          s->biColorTable , NO_TRANSPARENCY_COLOR );
  }
}

