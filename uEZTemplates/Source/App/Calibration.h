/*-------------------------------------------------------------------------*
 * File:  Calibration.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Present calibration screen and do it.
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
#ifndef _APP_CALIBRATION_H_
#define _APP_CALIBRATION_H_

#include <uEZ.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>

void CalibrateProcedure(
                T_pixelColor *aPixels,
                T_uezDevice ts,
                TBool aForceCalibrate);
TUInt32 Calibrate(TBool aForceCalibrate);
void CalibrateMode(const T_choice *p_choice);
TBool CalibrateTestIfTouchscreenHeld(void);

#endif // _APP_CALIBRATION_H_
/*-------------------------------------------------------------------------*
 * File:  Calibration.h
 *-------------------------------------------------------------------------*/
