/*-------------------------------------------------------------------------*
 * File:  NVSettings.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Load and Store non-volatile settings in global block on EEPROM0.
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
#include <uEZ.h>
#include <string.h>
#include <uEZEEPROM.h>
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
    TUInt32 checksum = 0;
    TUInt32 i;
    TUInt8 *p;
    const TUInt32 len = (TUInt8 *)&G_nonvolatileSettings.iChecksum
            - (TUInt8 *)&G_nonvolatileSettings;

    p = (TUInt8 *)&G_nonvolatileSettings;
    for (i = 0; i < len; i++)
        checksum += *(p++);

    return ~checksum;
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
    T_uezError error;
    T_uezDevice eeprom;
    // Calculate a new checksum
    G_nonvolatileSettings.iChecksum = NVSettingsCalcChecksum();
    // Open EEPROM0   
    error = UEZEEPROMOpen("EEPROM0", &eeprom);
    if (error)
    {
        return error;
    }
    // the device opened properly, write data to EEPROM
    return UEZEEPROMWrite(eeprom,(TUInt8 *)&G_nonvolatileSettings, 0, 
                          sizeof(G_nonvolatileSettings));
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
    T_uezError error;
    T_uezDevice eeprom;
    
    // Open EEPROM0
    error =UEZEEPROMOpen("EEPROM0", &eeprom);
    if (error)
    {
        return error;
    }
    
    // read data from EEPROM
    error = UEZEEPROMRead(eeprom,(TUInt8 *)&G_nonvolatileSettings, 0,
                                      sizeof(G_nonvolatileSettings));
    if (error)
    {
        return error;
    }
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

void *UEZEMACGetMACAddr(TUInt32 aUnitNumber)
{
    extern T_nonvolatileSettings G_nonvolatileSettings;
    return G_nonvolatileSettings.iMACAddr;
}

/*-------------------------------------------------------------------------*
 * End of File:  NVSettings.c
 *-------------------------------------------------------------------------*/
