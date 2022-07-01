/*-------------------------------------------------------------------------*
 * File:  Configuration_EEPROM.c
 *-------------------------------------------------------------------------*
 * Description: Configuration data for FDI products. Boards with and Ethernet
 *              PHY will include a MAC address.
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2017 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <UEZ.h>
#include <UEZEEPROM.h>
#include <NVSettings.h>
#include "Configuration_EEPROM.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define FCT_TESTING         1

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_ConfigureationData G_ConfigurationData;
#if !FCT_TESTING
TUInt8 ub_licence_key[99];//CS commented out for FCT
#endif

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Function: Configuration_EEPROM_Program_HW_V
 *-------------------------------------------------------------------------*/
/** Program the MAC address into the EEPROM. This function should only be
 *  used by FDI for setting up the board.
 *
 * @param [in] aHW_Version       Pointer to a hardware version to be programmed
 *
 * @retval  UEZ_ERROR_NONE if successful, otherwise return the error of the
 *          EEPROM API
 */
/*------------------------------------------------------------------------*/
T_uezError Configuration_EEPROM_Program_HW_V(TUInt8 *aHW_Version)
{
    T_uezError error;
    T_uezDevice eeprom;
    char v[5];
    TUInt8 i;
    TUInt8 j = 0;

    memset((void*)v, 0, sizeof(v));

    for(i = 0; i < 2; i++){
        if(aHW_Version[i] == '.'){
            break;
        }
        if(aHW_Version[i] <= 0x39 && aHW_Version[i] >= 0x30){
            v[i] = aHW_Version[i];
        } else {
            return UEZ_ERROR_OUT_OF_RANGE;
        }
    }
    i++;

    G_ConfigurationData.iHWMajor = atoi(v);

    memset((void*)v, 0, sizeof(v));

    for(i = i; i < 5; i++){
        if(aHW_Version[i] == '.' || aHW_Version[i] == '\0'){
            break;
        }
        if(aHW_Version[i] <= 0x39 && aHW_Version[i] >= 0x30){
            v[j++] = aHW_Version[i];
        } else {
            return UEZ_ERROR_OUT_OF_RANGE;
        }
    }

    G_ConfigurationData.iHWMinor = atoi(v);

    if ((error = UEZEEPROMOpen("EEPROM1", &eeprom)) == UEZ_ERROR_NONE) {
        error = UEZEEPROMWrite(eeprom, (TUInt8*)&G_ConfigurationData, 0,
                sizeof(G_ConfigurationData));
        UEZEEPROMClose(eeprom);
    }

    return error;
}

/*-------------------------------------------------------------------------*
 * Function: Configuration_EEPROM_Program_MAC
 *-------------------------------------------------------------------------*/
/** Program the MAC address into the EEPROM. This function should only be
 *  used by FDI for setting up the board.
 *
 * @param [in] aMACAddress       Pointer to a MAC Address to be programmed
 *
 * @retval  UEZ_ERROR_NONE if successful, otherwise return the error of the
 *          EEPROM API
 */
/*------------------------------------------------------------------------*/
T_uezError Configuration_EEPROM_Program_MAC(TUInt8 *aMACAddress)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_uezDevice eeprom;

    if (memcmp((void*)&G_ConfigurationData.iMACAddress, (void*)aMACAddress,
            sizeof(G_ConfigurationData.iMACAddress)) != 0) {
        //New MAC Address, check if Valid and program into EEPROM
        if (aMACAddress[0] == MFG_ID_1) {
            memcpy((void*)&G_ConfigurationData.iMACAddress, (void*)aMACAddress,
                    sizeof(G_ConfigurationData.iMACAddress));
            if ((error = UEZEEPROMOpen("EEPROM1", &eeprom)) == UEZ_ERROR_NONE) {
                error = UEZEEPROMWrite(eeprom, (TUInt8*)&G_ConfigurationData, 0,
                        sizeof(G_ConfigurationData));
                UEZEEPROMClose(eeprom);
            }
        } else {
            error = UEZ_ERROR_OUT_OF_RANGE;
        }
        G_ConfigurationData.iValidConfigurations |= MAC_ADDRESS_VALID;
    } else {
        //MAC address is current
        G_ConfigurationData.iValidConfigurations |= MAC_ADDRESS_VALID;
    }

    return error;
}

/*-------------------------------------------------------------------------*
 * Function: Configuration_EEPROM_Program_UbiquiosKey
 *-------------------------------------------------------------------------*/
/** Program the Ubiquios Key into the EEPROM. This function should only be
 *  used by FDI for setting up the board.
 *
 * @param [in] aUbiquiosKeyNumber   Pointer to a Ubiquios Key Number to be
 *                                  programmed. Expecting ASCII number 9 long
 * @param [in] iUbiquiosKey         Pointer to a Ubiquios Key to be programmed
 *                                  Expecting a byte array of 99 bytes
 *
 * @retval  UEZ_ERROR_NONE if successful, otherwise return the error of the
 *          EEPROM API
 */
/*------------------------------------------------------------------------*/
T_uezError Configuration_EEPROM_Program_UbiquiosKey(
        TUInt8 *aUbiquiosKeyNumber,
        TUInt8 *aUbiquiosKey)
{
    T_uezError error;
    T_uezDevice eeprom;

    if (memcmp((void*)&G_ConfigurationData.iUbiquiosKeyNumber,
            (void*)aUbiquiosKeyNumber,
            sizeof(G_ConfigurationData.iUbiquiosKeyNumber)) != 0
            || memcmp((void*)&G_ConfigurationData.iUbiquiosKey,
                    (void*)aUbiquiosKey,
                    sizeof(G_ConfigurationData.iUbiquiosKey)) != 0) {
        //Ubiquios Key needs to be updated
        memcpy((void*)&G_ConfigurationData.iUbiquiosKeyNumber,
                (void*)aUbiquiosKeyNumber,
                sizeof(G_ConfigurationData.iUbiquiosKeyNumber));
        memcpy((void*)&G_ConfigurationData.iUbiquiosKey, (void*)aUbiquiosKey,
                sizeof(G_ConfigurationData.iUbiquiosKey));
        if ((error = UEZEEPROMOpen("EEPROM1", &eeprom)) == UEZ_ERROR_NONE) {
            error = UEZEEPROMWrite(eeprom, (TUInt8*)&G_ConfigurationData, 0,
                    sizeof(G_ConfigurationData));
            UEZEEPROMClose(eeprom);
        }
    }
    if (error == UEZ_ERROR_NONE) {
        G_ConfigurationData.iValidConfigurations |= UBIQUIOS_KEY_VALID;
    }
    return error;
}

/*-------------------------------------------------------------------------*
 * Function: Configuration_EEPROM_Init
 *-------------------------------------------------------------------------*/
/** Initialize the configuration data from the EEPROM and set which
 * configurations are valid. Should be called at startup in MainTask before
 * other peripherals are initialized. This function will modify the
 * G_nonvolatileSettings MAC address setting if they do not match.
 *
 * @retval  UEZ_ERROR_NONE if successful, otherwise return the error of the
 *          EEPROM API
 */
/*------------------------------------------------------------------------*/
T_uezError Configuration_EEPROM_Init()
{
    T_uezError error;
    T_uezDevice eeprom;
#if !FCT_TESTING
    TUInt8 i;
#endif

    memset((void*)&G_ConfigurationData, 0, sizeof(G_ConfigurationData));

    //Read the MAC Address from the I2C EEPROM
    if ((error = UEZEEPROMOpen("EEPROM1", &eeprom)) == UEZ_ERROR_NONE) {
        error = UEZEEPROMRead(eeprom, (TUInt8*)&G_ConfigurationData, 0,
                sizeof(G_ConfigurationData));
        if (G_ConfigurationData.iMACAddress[0] == MFG_ID_1
                && error == UEZ_ERROR_NONE) {
            //found an FDI MAC Address
            G_ConfigurationData.iValidConfigurations |= MAC_ADDRESS_VALID;
            if (memcmp((void*)&G_nonvolatileSettings.iMACAddr,
                    (void*)&G_ConfigurationData.iMACAddress,
                    sizeof(G_nonvolatileSettings.iMACAddr)) != 0) {
                //Not the right address, change it to match
                memcpy((void*)G_nonvolatileSettings.iMACAddr,
                        (void*)&G_ConfigurationData.iMACAddress,
                        sizeof(G_nonvolatileSettings.iMACAddr));
                NVSettingsSave();
            } //else, MAC Addresses match
        } else {
            G_ConfigurationData.iValidConfigurations = 0;
            error = UEZ_ERROR_NOT_AVAILABLE;
        }
        UEZEEPROMClose(eeprom);
    }

    if (G_ConfigurationData.iUbiquiosKeyNumber[0] != 0xFF) {
        G_ConfigurationData.iValidConfigurations |= UBIQUIOS_KEY_VALID;
        error = UEZ_ERROR_NONE;
#if !FCT_TESTING
        for(i = 0; i < 99; i++){
            //Using for loop because memcopy caused unaligned transfer crash
            ub_licence_key[i] = G_ConfigurationData.iUbiquiosKey[i];
        }
#endif
    }

    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  Configuration_EEPROM.c
 *-------------------------------------------------------------------------*/
