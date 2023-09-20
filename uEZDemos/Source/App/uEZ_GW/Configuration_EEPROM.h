/*-------------------------------------------------------------------------*
 * File:  Configuration_EEPROM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef SOURCE_APP_UEZ_GW_CONFIGURATION_EEPROM_H_
#define SOURCE_APP_UEZ_GW_CONFIGURATION_EEPROM_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
//FDI MAC address assigned by the IEEE, used for validating FDI Products
#define MFG_ID_1            0x14
#define MFG_ID_2            0x35
#define MFG_ID_3            0xB3

#define MAC_ADDRESS_VALID   (1 << 0)
#define UBIQUIOS_KEY_VALID  (1 << 1)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt8 iValidConfigurations;
    TUInt8 iMACAddress[6]; //Assigned to boards with Ethernet PHY on board
    TUInt8 iUbiquiosKeyNumber[9];
    TUInt8 iUbiquiosKey[99];
    TUInt8 iHWMajor;
    TUInt8 iHWMinor;
    TUInt8 iReserved[139];
}T_ConfigureationData;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern T_ConfigureationData G_ConfigurationData;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError Configuration_EEPROM_Init();
T_uezError Configuration_EEPROM_Program_MAC(TUInt8 *aMACAddress);
T_uezError Configuration_EEPROM_Program_UbiquiosKey(
        TUInt8 *aUbiquiosKeyNumber,
        TUInt8 *aUbiquiosKey);
T_uezError Configuration_EEPROM_Program_HW_V(TUInt8 *aHW_Version);

#endif // SOURCE_APP_UEZ_GW_CONFIGURATION_EEPROM_H_
/*-------------------------------------------------------------------------*
 * End of File:  Configuration_EEPROM.h
 *-------------------------------------------------------------------------*/
