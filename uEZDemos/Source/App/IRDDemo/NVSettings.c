/*-------------------------------------------------------------------------*
 * File:  NVSettings.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Load and Store non-volatile settings in global block on EEPROM0.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <string.h>
#include <Device/EEPROM.h>
#include <uEZDeviceTable.h>
#include "NVSettings.h"

T_nonvolatileSettings G_nonvolatileSettings;

/*---------------------------------------------------------------------------*
 * Routine:  NVSettingsCalcChecksum
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate the checksum of the calibration settings (excluding the
 *      checksum value itself).
 * Outputs:
 *      TUInt32                   -- Calculated checksum
 *---------------------------------------------------------------------------*/
static TUInt32 NVSettingsCalcChecksum(void)
{
    TUInt32 checksum = G_nonvolatileSettings.iNum;
    TUInt32 i;
    T_uezTSReading *p;

    for (i=0; i<3; i++) {
        p = G_nonvolatileSettings.iReadings + i;
        checksum += p->iFlags;
        checksum += p->iX;
        checksum += p->iY;
        checksum += p->iPressure;
    }
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
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 checksum = NVSettingsCalcChecksum();
    if (G_nonvolatileSettings.iChecksum != checksum)
        error = UEZ_ERROR_CHECKSUM_BAD;
    return error;
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
    T_uezDevice eeprom;
    DEVICE_EEPROM **p_eeprom;
    T_uezError error;

    // Find the eeprom0 device
    error = UEZDeviceTableFind("EEPROM0", &eeprom);
    if (error)
        return error;

    // Look up its workspace
    UEZDeviceTableGetWorkspace(eeprom, (T_uezDeviceWorkspace **)&p_eeprom);

    // Calculate a new checksum
    G_nonvolatileSettings.iChecksum = NVSettingsCalcChecksum();

    // Save the setting
    return ((*p_eeprom)->Write(
                p_eeprom,
                0,
                (TUInt8 *)&G_nonvolatileSettings,
                sizeof(G_nonvolatileSettings)));
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
    // Clear the structure
    memset(&G_nonvolatileSettings, 0, sizeof(G_nonvolatileSettings));
    G_nonvolatileSettings.iIPAddr[0] = emacIPADDR0;
    G_nonvolatileSettings.iIPAddr[1] = emacIPADDR1;
    G_nonvolatileSettings.iIPAddr[2] = emacIPADDR2;
    G_nonvolatileSettings.iIPAddr[3] = emacIPADDR3;
    G_nonvolatileSettings.iIPMask[0] = emacNET_MASK0;
    G_nonvolatileSettings.iIPMask[1] = emacNET_MASK1;
    G_nonvolatileSettings.iIPMask[2] = emacNET_MASK2;
    G_nonvolatileSettings.iIPMask[3] = emacNET_MASK3;
    G_nonvolatileSettings.iIPGateway[0] = emacGATEWAY_ADDR0;
    G_nonvolatileSettings.iIPGateway[1] = emacGATEWAY_ADDR1;
    G_nonvolatileSettings.iIPGateway[2] = emacGATEWAY_ADDR2;
    G_nonvolatileSettings.iIPGateway[3] = emacGATEWAY_ADDR3;
    G_nonvolatileSettings.iMACAddr[0] = emacMACADDR0;
    G_nonvolatileSettings.iMACAddr[1] = emacMACADDR1;
    G_nonvolatileSettings.iMACAddr[2] = emacMACADDR2;
    G_nonvolatileSettings.iMACAddr[3] = emacMACADDR3;
    G_nonvolatileSettings.iMACAddr[4] = emacMACADDR4;
    G_nonvolatileSettings.iMACAddr[5] = emacMACADDR5;
}

T_uezError NVSettingsGetMACAddress(TUInt8 *aMACAddress)
{
    memcpy(aMACAddress, G_nonvolatileSettings.iMACAddr, 6);
    return UEZ_ERROR_NONE;
}
/*-------------------------------------------------------------------------*
 * End of File:  NVSettings.c
 *-------------------------------------------------------------------------*/
