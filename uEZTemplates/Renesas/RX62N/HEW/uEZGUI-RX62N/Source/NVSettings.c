/*-------------------------------------------------------------------------*
 * File:  NVSettings.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Load and Store non-volatile settings in global block on EEPROM0.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2008 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 * 	-   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 * 	-   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 	-   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * 	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * 	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * 	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * 	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * 	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * 	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * 	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <string.h>
#include <Device/EEPROM.h>
#include <Device/Flash.h>
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
	T_uezDevice flash;
    DEVICE_Flash **p_flash;
    T_uezError error;

    // Find the flash0 device
    error = UEZDeviceTableFind("Flash0", &flash);
    if (error)
        return error;

    // Look up its workspace
    UEZDeviceTableGetWorkspace(flash, (T_uezDeviceWorkspace **)&p_flash);

    // Calculate a new checksum
    G_nonvolatileSettings.iChecksum = NVSettingsCalcChecksum();
	error = (*p_flash)->Open(p_flash);
    // Save the setting
	error = (*p_flash)->ChipErase(p_flash);
    error =(*p_flash)->Write(
                p_flash,
                0,
                (TUInt8 *)&G_nonvolatileSettings,
                sizeof(G_nonvolatileSettings));
	if(error==UEZ_ERROR_READ_WRITE_ERROR)
		return error;
	error =(*p_flash)->Close(p_flash);
	if(error==UEZ_ERROR_READ_WRITE_ERROR)
		return error;
	else
		return UEZ_ERROR_NONE;
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
	T_uezDevice flash;
    DEVICE_Flash **p_flash;
    T_uezError error;

    // Find the flash0 device
    error = UEZDeviceTableFind("Flash0", &flash);
    if (error)
        return error;

    // Look up its workspace
    UEZDeviceTableGetWorkspace(flash, (T_uezDeviceWorkspace **)&p_flash);

    // Calculate a new checksum
    G_nonvolatileSettings.iChecksum = NVSettingsCalcChecksum();
	// Save the setting
    error = (*p_flash)->Read(
                p_flash,
                0,
                (void *)&G_nonvolatileSettings,
                sizeof(G_nonvolatileSettings));
	if(error == UEZ_ERROR_READ_WRITE_ERROR)
		return error;
	
	return NVSettingsIsValid();  

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
    // Clear the structure
    memset(&G_nonvolatileSettings, 0, sizeof(G_nonvolatileSettings));
	G_nonvolatileSettings.iNum = 3;
	G_nonvolatileSettings.iReadings[0].iFlags = 1;
	G_nonvolatileSettings.iReadings[0].iX = (TUInt32)0x2539;
	G_nonvolatileSettings.iReadings[0].iY = (TUInt32)0x2204;
	G_nonvolatileSettings.iReadings[0].iPressure = (TUInt32)0x9dd8;
	
	G_nonvolatileSettings.iReadings[1].iFlags = 1;
	G_nonvolatileSettings.iReadings[1].iX = (TInt32)0x1bd2;
	G_nonvolatileSettings.iReadings[1].iY = (TInt32)0x221b;
	G_nonvolatileSettings.iReadings[1].iPressure = (TInt32)0x9dd8;
	
	G_nonvolatileSettings.iReadings[2].iFlags = 1;
	G_nonvolatileSettings.iReadings[2].iX = (TInt32)0x2519;
	G_nonvolatileSettings.iReadings[2].iY = (TInt32)0x1e36;
	G_nonvolatileSettings.iReadings[2].iPressure = (TInt32)0x9dd8;
	
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
