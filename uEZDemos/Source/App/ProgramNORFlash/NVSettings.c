/*-------------------------------------------------------------------------*
 * File:  NVSettings.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Load and Store non-volatile settings in global block on EEPROM0.
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
#include <Config.h>
#include <string.h>
#include <Device/EEPROM.h>
#include <uEZDeviceTable.h>
#include "NVSettings.h"

volatile T_nonvolatileSettings G_nonvolatileSettings;
static const char G_nonvolatileSettingsModel[32] =
        PROJECT_NAME " " VERSION_AS_TEXT;

/*---------------------------------------------------------------------------*
 * Routine:  NVSettingsCalcChecksum
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate the checksum of the calibration settings (excluding the
 *      checksum value itself).
 * Outputs:
 *      TUInt32                   -- Calculated checksum
 *---------------------------------------------------------------------------*/
#if 0
static TUInt32 NVSettingsCalcChecksum(void)
{
    TUInt32 checksum = G_nonvolatileSettings.iNum;
    TUInt32 i;
    T_uezTSReading volatile *p;

    for (i=0; i<3; i++) {
        p = G_nonvolatileSettings.iReadings + i;
        checksum += p->iFlags;
        checksum += p->iX;
        checksum += p->iY;
        checksum += p->iPressure;
    }
    for (i=0; i<32; i++)
        checksum += G_nonvolatileSettings.iModelAndVersion[i];
    for (i=0; i<6; i++)
        checksum += G_nonvolatileSettings.iMACAddr[i];
    for (i=0; i<4; i++)
        checksum += G_nonvolatileSettings.iIPAddr[i];
    for (i=0; i<4; i++)
        checksum += G_nonvolatileSettings.iIPMask[i];
    for (i=0; i<4; i++)
        checksum += G_nonvolatileSettings.iIPGateway[i];

    return checksum;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  NVSettingsIsValid
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if the settings in memory are valid.
 * Outputs:
 *      T_uezError                  -- Error code if failed, else
 *                                      UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
T_uezError NVSettingsIsValid(void)
{
#if 0
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 checksum = NVSettingsCalcChecksum();

    if (G_nonvolatileSettings.iChecksum != checksum)
        error = UEZ_ERROR_CHECKSUM_BAD;
    // Check if this is the proper version
    else if (memcmp(G_nonvolatileSettingsModel,
            (void *)G_nonvolatileSettings.iModelAndVersion, 32) != 0)
        error = UEZ_ERROR_INVALID;

    return error;
#endif
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  NVSettingsSave
 *---------------------------------------------------------------------------*
 * Description:
 *      Save the current settings in memory to EEPROM0.
 * Outputs:
 *      T_uezError                  -- Error code if failed, else
 *                                      UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
T_uezError NVSettingsSave(void)
{
#if 0
  T_uezDevice eeprom;
    DEVICE_EEPROM **p_eeprom;
    T_uezError error;
T_nonvolatileSettings check;

    // Find the eeprom0 device
    error = UEZDeviceTableFind("EEPROM0", &eeprom);
    if (error)
        return error;

    // Look up its workspace
    UEZDeviceTableGetWorkspace(eeprom, (T_uezDeviceWorkspace **)&p_eeprom);

    // Calculate a new checksum
    G_nonvolatileSettings.iChecksum = NVSettingsCalcChecksum();

    // Save the setting
    error = ((*p_eeprom)->Write(
                p_eeprom,
                0,
                (TUInt8 *)&G_nonvolatileSettings,
                sizeof(G_nonvolatileSettings)));

    ((*p_eeprom)->Read(
                p_eeprom,
                0,
                (TUInt8 *)&check,
                sizeof(check)));
    if (memcmp((void *)&check, (void *)&G_nonvolatileSettings, sizeof(check)) != 0)
        return UEZ_ERROR_CHECKSUM_BAD;

    return error;
#else
        return UEZ_ERROR_NONE;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  NVSettingsLoad
 *---------------------------------------------------------------------------*
 * Description:
 *      Load the current settings into memory from EEPROM0.
 * Outputs:
 *      T_uezError                  -- Error code if failed, else
 *                                      UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
T_uezError NVSettingsLoad(void)
{
#if 0
    T_uezDevice eeprom;
    DEVICE_EEPROM **p_eeprom;
    T_uezError error;

    // Find the eeprom0 device
    error = UEZDeviceTableFind("EEPROM0", &eeprom);
    if (error)
        return error;

    // Look up its workspace
    UEZDeviceTableGetWorkspace(eeprom, (T_uezDeviceWorkspace **)&p_eeprom);

    // Load the settings and return any errros;
    error = (*p_eeprom)->Read(
                p_eeprom,
                0,
                (TUInt8 *)&G_nonvolatileSettings,
                sizeof(G_nonvolatileSettings));
    if (error)
        return error;

    // Determine if the checksum is valid and return
    return NVSettingsIsValid();
#endif
    NVSettingsInit();
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  NVSettingsInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initilize the settings to their default settings in memory.
 *      Will need to save.
 *---------------------------------------------------------------------------*/
void NVSettingsInit(void)
{
#ifndef UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION
#error "Missing LCD Touchscreen default calibration information!"
#endif
    static const T_uezTSReading G_expectedReadings[5] = {
        UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION
    };

    // Clear the structure
    memset((void *)&G_nonvolatileSettings, 0, sizeof(G_nonvolatileSettings));
//    G_nonvolatileSettings.iIPAddr[0] = emacIPADDR0;
//    G_nonvolatileSettings.iIPAddr[1] = emacIPADDR1;
//    G_nonvolatileSettings.iIPAddr[2] = emacIPADDR2;
//    G_nonvolatileSettings.iIPAddr[3] = emacIPADDR3;
//    G_nonvolatileSettings.iIPMask[0] = emacNET_MASK0;
//    G_nonvolatileSettings.iIPMask[1] = emacNET_MASK1;
//    G_nonvolatileSettings.iIPMask[2] = emacNET_MASK2;
//    G_nonvolatileSettings.iIPMask[3] = emacNET_MASK3;
//    G_nonvolatileSettings.iIPGateway[0] = emacGATEWAY_ADDR0;
//    G_nonvolatileSettings.iIPGateway[1] = emacGATEWAY_ADDR1;
//    G_nonvolatileSettings.iIPGateway[2] = emacGATEWAY_ADDR2;
//    G_nonvolatileSettings.iIPGateway[3] = emacGATEWAY_ADDR3;
//    G_nonvolatileSettings.iMACAddr[0] = emacMACADDR0;
//    G_nonvolatileSettings.iMACAddr[1] = emacMACADDR1;
//    G_nonvolatileSettings.iMACAddr[2] = emacMACADDR2;
//    G_nonvolatileSettings.iMACAddr[3] = emacMACADDR3;
//    G_nonvolatileSettings.iMACAddr[4] = emacMACADDR4;
//    G_nonvolatileSettings.iMACAddr[5] = emacMACADDR5;

    G_nonvolatileSettings.iNum = 3;
    G_nonvolatileSettings.iReadings[0].iFlags = 1;
    G_nonvolatileSettings.iReadings[0].iX = G_expectedReadings[0].iX;
    G_nonvolatileSettings.iReadings[0].iY = G_expectedReadings[0].iY;
    G_nonvolatileSettings.iReadings[1].iFlags = 1;
    G_nonvolatileSettings.iReadings[1].iX = G_expectedReadings[1].iX;
    G_nonvolatileSettings.iReadings[1].iY = G_expectedReadings[1].iY;
    G_nonvolatileSettings.iReadings[2].iFlags = 1;
    G_nonvolatileSettings.iReadings[2].iX = G_expectedReadings[2].iX;
    G_nonvolatileSettings.iReadings[2].iY = G_expectedReadings[2].iY;

    G_nonvolatileSettings.iNeedRecalibrate = 0;
    memcpy((void *)G_nonvolatileSettings.iModelAndVersion, G_nonvolatileSettingsModel,
            32);
}

T_uezError NVSettingsGetMACAddress(TUInt8 *aMACAddress)
{
//    memcpy(aMACAddress, (void *)G_nonvolatileSettings.iMACAddr, 6);
    return UEZ_ERROR_NONE;
}
/*-------------------------------------------------------------------------*
 * End of File:  NVSettings.c
 *-------------------------------------------------------------------------*/
