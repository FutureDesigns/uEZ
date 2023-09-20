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
#include <stdio.h>
#include <string.h>
#include <uEZEEPROM.h>
#include "NVSettings.h"
#include <uEZFile.h>
#include <Device/MassStorage.h>

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
 *      Avoid calling this function when uneeded to save write cycles!
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
    G_nonvolatileSettings.iNeedRecalibrate = ETrue;
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

/*---------------------------------------------------------------------------*
 * Routine:  uEZFormatDrive
 *---------------------------------------------------------------------------*/
/**
 * Format the chosen drive with either FAT or EXFAT file system
 *
 * @param[in]   driveLetter         Drive number '0', '1', etc as character.
 *
 * @return T_uezError               Error code, if any. UEZ_ERROR_OK if
 *                                  successful.

 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <NVSettings.h>
 *
 *  if (uEZFormatDrive('1') == UEZ_ERROR_NONE) {
 *      // the format was successful
 *  } else {
 *      // an error occurred 
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError uEZFormatDrive(char driveLetter) {
  T_uezFile file;
  char fileName[14] = {0};
  char drivePath[4] = {driveLetter,':','/',0}; // logical drive path

  printf("Formatting Drive....");
  if (UEZFileMKFS(driveLetter) != UEZ_ERROR_NONE) {
     printf("FAIL: Format Error\n");
     return UEZ_ERROR_FAIL;
  } else {
     printf("Drive Formatted Successfully.\n");
  }

  sprintf(fileName,"%c:REPORT.TXT", driveLetter);
  if (UEZFileOpen(fileName, FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
    TUInt32 numWritten;
    char buff[80] = {0};

    sprintf(buff, "Device Formated:\n");
    UEZFileWrite(file, buff, strlen(buff), &numWritten);
    printf(buff);

    T_msSizeInfo aDeviceInfo;
    if (UEZFileSystemGetStorageInfo(drivePath, &aDeviceInfo) == UEZ_ERROR_NONE) {
        sprintf(buff, "Storage Medium Report:\n  Sectors: %u\n", aDeviceInfo.iNumSectors);           
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        printf(buff);

        sprintf(buff, "  Sector Size: %u\n  Block Size: %u\n",
            aDeviceInfo.iSectorSize, aDeviceInfo.iBlockSize);
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        printf(buff);
    }

    T_uezFileSystemVolumeInfo aFSInfo;
    if (UEZFileSystemGetVolumeInfo(drivePath, &aFSInfo) == UEZ_ERROR_NONE) {
        sprintf(buff, "File System Report:\n");
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        printf(buff);

        sprintf(buff, "  Bytes Per Sector: %u\n  Sectors Per Cluster: %u\n", 
            aFSInfo.iBytesPerSector, aFSInfo.iSectorsPerCluster);
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        printf(buff);

        sprintf(buff, "  Num Clusters Total: %u\n  Num Clusters Free: %u\n",
            aFSInfo.iNumClustersTotal, aFSInfo.iNumClustersFree);
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        printf(buff);
    }

    if (UEZFileClose(file) != UEZ_ERROR_NONE) {             
      return UEZ_ERROR_FAIL;// Error closing file
    }
  } else {
     return UEZ_ERROR_FAIL; // Error opening file
  }
  return UEZ_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
 * End of File:  NVSettings.c
 *-------------------------------------------------------------------------*/
