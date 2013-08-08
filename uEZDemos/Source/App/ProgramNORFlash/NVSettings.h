#ifndef _NVSETTINGS_H_
#define _NVSETTINGS_H_
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

typedef struct {
    char iModelAndVersion[32]; // string for this hardware
    TUInt32 iNum;
    T_uezTSReading iReadings[3];
	TUInt8 iNeedRecalibrate;
    //TUInt8 iMACAddr[6];
    //TUInt8 iIPAddr[4];
    //TUInt8 iIPMask[4];
    //TUInt8 iIPGateway[4];
    TUInt32 iChecksum;
} T_nonvolatileSettings;

extern volatile T_nonvolatileSettings G_nonvolatileSettings;

void NVSettingsInit(void);
T_uezError NVSettingsLoad(void);
T_uezError NVSettingsSave(void);
T_uezError NVSettingsGetMACAddress(TUInt8 *aMACAddress);

#endif // _NVSETTINGS_H_
