/*-------------------------------------------------------------------------*
 * File:  Calibration.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Present calibration screen and do it.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm)+ Bootloader - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ+ Bootloader distribution.
 *
 * uEZ(tm)+ Bootloader is commercial software licensed under the
 * Software End-User License Agreement (EULA) delivered with this source
 * code package.  The Software and Documentation contain material that is
 * protected by United States Copyright Law and trade secret law, and by
 * international treaty provisions. All rights not granted to Licensee
 * herein are expressly reserved by FDI. Licensee may not remove any
 * proprietary notice of FDI from any copy of the Software or
 * Documentation.
 *
 * For more details, please review your EULA agreement included with this
 * software package.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/


#include <uEZ.h>
#include <uEZTS.h>
#include "NVSettings.h"
#include "Calibration.h"

#ifndef TP_INSET
#define TP_INSET        50
#endif

static const T_uezTSReading G_expectedReadings[3] = { 
  {   0,  0 + TP_INSET,    TP_INSET,    1 }, // 0
  {   0,  DISPLAY_WIDTH - TP_INSET, TP_INSET, 1 }, // 1
  {   0,  0 + TP_INSET, DISPLAY_HEIGHT - TP_INSET, 1 }, // 2
};

/*---------------------------------------------------------------------------*
 * Task:  Calibrate
 *---------------------------------------------------------------------------*
 * Description:
 *      Goes through the calibration procedure
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 *---------------------------------------------------------------------------*/
void Calibrate(void)
{
    T_uezDevice ts;
    TUInt32 i;
    T_uezTSReading G_calibrateSetting;

    // Feed in the data points
    if (UEZTSOpen("Touchscreen", &ts, 0) == UEZ_ERROR_NONE) {
        UEZTSCalibrationStart(ts);
        for (i = 0; i < 3; i++) {
            G_calibrateSetting = G_expectedReadings[i];
            // Change the touchscreen to be flipped with the screen
            UEZTSCalibrationAddReading(ts,
                (const T_uezTSReading *)&G_nonvolatileSettings.iReadings[i],
                &G_calibrateSetting);
        }
        UEZTSCalibrationEnd(ts);
    }
}

/*-------------------------------------------------------------------------*
 * File:  Calibration.c
 *-------------------------------------------------------------------------*/
