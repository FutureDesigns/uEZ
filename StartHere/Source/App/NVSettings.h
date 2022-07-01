#ifndef _NVSETTINGS_H_
#define _NVSETTINGS_H_
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
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
#include <uEZ.h>

typedef struct {
    TUInt32 iNum;
    T_uezTSReading iReadings[3];
    TUInt8 iMACAddr[6];
    TUInt8 iIPAddr[4];
    TUInt8 iIPMask[4];
    TUInt8 iIPGateway[4];
    TBool iNeedRecalibrate;
    // Checksum: Must be last entry
    TUInt32 iChecksum;
} T_nonvolatileSettings;

extern T_nonvolatileSettings G_nonvolatileSettings;

void NVSettingsInit(void);
T_uezError NVSettingsLoad(void);
T_uezError NVSettingsSave(void);
T_uezError NVSettingsGetMACAddress(TUInt8 *aMACAddress);

#endif // _NVSETTINGS_H_
