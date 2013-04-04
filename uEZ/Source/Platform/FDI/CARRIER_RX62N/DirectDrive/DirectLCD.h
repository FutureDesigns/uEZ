/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :Directlcd.h                                           */
/*  DATE        :Jan 23, 2008                                          */
/*  DESCRIPTION :Direct Drive Application Programming Interface        */
/*                                                                     */
/***********************************************************************/
/************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
01.24.2008 RTA-JMB Rev 1.0
************************************************************************/
#ifndef DIRECTLCD_H
#define DIRECTLCD_H

#include <HAL/LCDController.h>

extern uI16 LCD_Vcount;          // frame counter
extern uI16 LCD_BusActive;       // flag indicating if we're running ExDMA on this line

#ifdef RAM_32WIDE
typedef uI32 Raster_type;
#else
typedef uI16 Raster_type;
#endif

typedef enum
{
  LCDAPI_SUCCESS = 0,
  LCDAPI_ERR_UNINITIALIZED,        // lcd api function called prior to initialization
  LCDAPI_ERR_UNSUPPORTED,
  LCDAPI_ERR_INVALID_PARAMETER,
  LCDAPI_ERR_NULL_POINTER,
  LCDAPI_ERR_ODDADDRESS            // frame buffer must lie on even address boundary
}LCDErrorType;

// Function Prototypes

LCDErrorType LCDInit(T_LCDControllerSettings *aSettings);
sI16 LCDSetLineSource(sI16 Region, sI16 LineCount, Raster_type *pSource, sI16 LineStep);
Raster_type * LCDSetActiveRaster(uI32 frame);
uI16 LCDGetActiveFrame(void);
sI16 LCDSetFrameRate(sI16 rate);
sI16 LCDGetFrameRate(void);
sI16 LCDSetRasterOffset(sI16 x, sI16 y);
void LCDBacklight(int state);
void LCDOff(void);


#endif
