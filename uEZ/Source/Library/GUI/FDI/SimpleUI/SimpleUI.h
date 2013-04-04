/*-------------------------------------------------------------------------*
 * File:  SimpleUI.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Collection of routines for doing simple user interface processing
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef _SIMPLEUI_H_
#define _SIMPLEUI_H_

#include <uEZ.h>

#include "SimpleUI_Types.h"
#include "SimpleUI_UtilityFunctions.h"
#include "SimpleUI_Regions.h"
#include "SimpleUI_Drawing.h"
#include "SimpleUI_Choices.h"

/*---------------------------------------------------------------------------*
 * Required outside routines:
 *---------------------------------------------------------------------------*/
void SUICallbackSetLCDBase(void *aAddress);
void *SUICallbackGetLCDBase(void);
T_pixelColor SUICallbackRGBConvert(int r, int g, int b);

#endif // _SIMPLEUI_H_
/*-------------------------------------------------------------------------*
 * File:  SimpleUI.h
 *-------------------------------------------------------------------------*/
