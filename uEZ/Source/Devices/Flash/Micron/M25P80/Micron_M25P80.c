/*-------------------------------------------------------------------------*
 * File:  Micron_M25P80.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Flash driver for Micron M2P80. Driver has expectations that sector and
 *     page boundaries are adhered to. The device requires that sectors be
 *     erased before pages in the sector can be written.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.
 *
 * uEZ(R) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(R) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(R); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(R), without being obliged
 * to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "Micron_M25P80.h"

#include <uEZ.h>
#include <string.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include <UEZSPI.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define MICRON_M23P80_WRITE_ENABLE          (0x06)
#define MICRON_M23P80_WRITE_DISABLE         (0x04)
#define MICRON_M23P80_READ_ID               (0x9F)//0x9E alternate command
#define MICRON_M23P80_READ_STATUS_REG       (0x05)
#define MICRON_M23P80_WRITE_STATUS_REG      (0x01)
#define MICRON_M23P80_READ_DATA             (0x03)
#define MICRON_M23P80_READ_DATA_FAST        (0x0B)
#define MICRON_M23P80_PAGE_PROGRAM          (0x02)
#define MICRON_M23P80_SECTOR_ERASE          (0xD8)
#define MICRON_M23P80_BULK_ERASE            (0xC7)
#define MICRON_M23P80_DEEP_POWER            (0xB9)
#define MICRON_M23P80_RELEASE_DEEP_POWER    (0xAB)

#define MICRON_M23P80_CLOCK_SPEED           (25000) //20MHz for initial testing
#define MICRON_M23P80_PAGE_SIZE             (256) //256 Byte Page Size
#define MICRON_M23P80_SECTOR_SIZE           (64 * 1024) //64KB Sector Size (512Kb)
#define MICRON_M23P80_NUM_SECTORS           (16)

#define MICRON_M23P80_MFG_ID                (0x20) // Multiple MFG IDs supported
#define MICRON_M23P80_JEDEC_1               (0x20)
#define MICRON_M23P80_JEDEC_2               (0x14)

#define STATUS_REG_WIP_BIT_MASK             (1<<0) //Write in Progress bit
#define STATUS_REG_WEL_BIT_MASK             (1<<1) //Write enable latch bit
#define STATUS_REG_BP0_BIT_MASK             (1<<2) //Block protection bits
#define STATUS_REG_BP1_BIT_MASK             (1<<3)
#define STATUS_REG_BP2_BIT_MASK             (1<<4)
#define STATUS_REG_SRWD_BIT_MASK            (1<<7) //Status register write protect

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        const DEVICE_Flash *iDevice;
        TUInt32 iNumOpen;
        T_uezSemaphore iSem;
        T_uezGPIOPortPin iChipSelect;
        T_uezGPIOPortPin iWriteProtect;
        T_uezGPIOPortPin iReset;
        char iSPIPortName[5];
        TUInt8 iMFGID;
        TUInt16 iJEDEC;
        T_uezDevice iSPI;
        TBool iDeviceFound;
        SPI_Request iRequest;
}T_Flash_Micron_M25P80_Workspace;

typedef struct {
    TInt32 iIndex;
    TUInt32 iAddress;
    TUInt32 iSize;
} T_sector;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static TUInt32 IMicron_M25P80_OffsetToSector(
        T_Flash_Micron_M25P80_Workspace *p,
        TUInt32 aOffset);
static T_uezError IMicron_M25P80_IsBusy(
        T_Flash_Micron_M25P80_Workspace *p,
        TBool *aBusy);
static T_uezError IMicron_M25P80_Start(T_Flash_Micron_M25P80_Workspace *p);
static T_uezError IMicron_M25P80_End(T_Flash_Micron_M25P80_Workspace *p);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
const T_sector G_SectorMemory[MICRON_M23P80_NUM_SECTORS] = {
    {   0,  0x00000000, 0x00010000 },
    {   1,  0x00010000, 0x00010000 },
    {   2,  0x00020000, 0x00010000 },
    {   3,  0x00030000, 0x00010000 },
    {   4,  0x00040000, 0x00010000 },
    {   5,  0x00050000, 0x00010000 },
    {   6,  0x00060000, 0x00010000 },
    {   7,  0x00070000, 0x00010000 },
    {   8,  0x00080000, 0x00010000 },
    {   9,  0x00090000, 0x00010000 },
    {  10,  0x000A0000, 0x00010000 },
    {  11,  0x000B0000, 0x00010000 },
    {  12,  0x000C0000, 0x00010000 },
    {  13,  0x000D0000, 0x00010000 },
    {  14,  0x000E0000, 0x00010000 },
    {  15,  0x000F0000, 0x00010000 },
};
static TBool G_sectorsNeedingErase[MICRON_M23P80_NUM_SECTORS];

/*---------------------------------------------------------------------------*
 * Routine:  IMicron_M25P80_OffsetToSector
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate the sector based on an offset
 *---------------------------------------------------------------------------*/
static TUInt32 IMicron_M25P80_OffsetToSector(
        T_Flash_Micron_M25P80_Workspace *p,
        TUInt32 aOffset)
{
    TUInt32 sector = MICRON_M23P80_NUM_SECTORS + 1; //initialize as invalid value
    TUInt32 i;

    for(i = 0; i < MICRON_M23P80_NUM_SECTORS; i++){
        if ((aOffset >= G_SectorMemory[i].iAddress) &&
            (aOffset < (G_SectorMemory[i].iAddress + G_SectorMemory[i].iSize))) {
            sector = G_SectorMemory[i].iIndex;
        }
    }

    return sector;
}

/*---------------------------------------------------------------------------*
 * Routine:  IMicron_M25P80_IsBusy
 *---------------------------------------------------------------------------*
 * Description:
 *      Check the device Status
 *---------------------------------------------------------------------------*/
static T_uezError IMicron_M25P80_IsBusy(
        T_Flash_Micron_M25P80_Workspace *p,
        TBool *aBusy)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt8 dataIn[2];
    TUInt8 dataOut[2];

    dataOut[0] = MICRON_M23P80_READ_STATUS_REG;

    p->iRequest.iNumTransfersIn = 1;
    p->iRequest.iNumTransfersOut = 1;
    p->iRequest.iNumTransfers = p->iRequest.iNumTransfersIn + p->iRequest.iNumTransfersOut;
    p->iRequest.iDataMISO = dataIn;
    p->iRequest.iDataMOSI = dataOut;

    if((error = UEZSPITransferPolled(p->iSPI, &p->iRequest)) == UEZ_ERROR_NONE){
        if((dataIn[0] & STATUS_REG_WIP_BIT_MASK) == STATUS_REG_WIP_BIT_MASK){
            *aBusy = ETrue;
        } else {
            *aBusy = EFalse;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IMicron_M25P80_Command
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a command to the device
 *---------------------------------------------------------------------------*/
static T_uezError IMicron_M25P80_Command(
        T_Flash_Micron_M25P80_Workspace *p,
        TUInt8 *aDataOut,
        TUInt32 aNumTransfersOut,
        TUInt8 *aDataIn,
        TUInt32 aNumTransfersIn)
{
    T_uezError error = UEZ_ERROR_NONE;

    p->iRequest.iNumTransfersIn = aNumTransfersIn;
    p->iRequest.iNumTransfersOut = aNumTransfersOut;
    p->iRequest.iNumTransfers = aNumTransfersOut + aNumTransfersIn;
    p->iRequest.iDataMISO = aDataIn;
    p->iRequest.iDataMOSI = aDataOut;

    error = UEZSPITransferPolled(p->iSPI, &p->iRequest);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IMicron_M25P80_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a new session.
 *---------------------------------------------------------------------------*/
static T_uezError IMicron_M25P80_Start(T_Flash_Micron_M25P80_Workspace *p)
{
    return UEZSPIOpen(p->iSPIPortName, &p->iSPI);
}

/*---------------------------------------------------------------------------*
 * Routine:  IMicron_M25P80_End
 *---------------------------------------------------------------------------*
 * Description:
 *      End a session.
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
static T_uezError IMicron_M25P80_End(T_Flash_Micron_M25P80_Workspace *p)
{
    return UEZSPIClose(p->iSPI);
}

/*---------------------------------------------------------------------------*
 * Routine:  MicronDeviceIDRead
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the Device ID to confirm the chip is there.
 *---------------------------------------------------------------------------*/
static TBool MicronDeviceIDRead(void *aWorkspace)
{
    TBool read = EFalse;
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;

    TUInt8 dataOut[25];
    TUInt8 dataIn[25];

    p->iRequest.iNumTransfersIn = 21;
    p->iRequest.iNumTransfersOut = 1;
    p->iRequest.iNumTransfers = p->iRequest.iNumTransfersIn + p->iRequest.iNumTransfersOut;
    p->iRequest.iDataMISO = dataIn;
    p->iRequest.iDataMOSI = dataOut;

    dataOut[0] = 0x9F; //Read ID Register

    if(UEZSPITransferPolled(p->iSPI, &p->iRequest) == UEZ_ERROR_NONE){
      if(dataIn[0] != 0){
          if(dataIn[1] == 0x20 && dataIn[2] == 0x14){
              // JEDEC Codes match expected values, test pass.
			  p->iMFGID = dataIn[0]; // The MFG ID can change even on the same part!
              read = ETrue;
          }
      }
    }

    return read;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the flash device. Initialize the flash
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_Open(void *aWorkspace)
{
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    /* Declare erase operation result container variable */
    p->iNumOpen++;
    if (p->iNumOpen == 1) {
        // If the first time, assume we were in some programming mode
        // and need to reset and Re-enable access to the Flash Data area
        error = IMicron_M25P80_Start(p);
        if(error){
            p->iNumOpen--;
            return UEZ_ERROR_NOT_FOUND;
        }

        if (MicronDeviceIDRead(aWorkspace)) {            
            p->iJEDEC = MICRON_M23P80_JEDEC_1 << 8 | MICRON_M23P80_JEDEC_2;
            p->iDeviceFound = ETrue;
        } else {
            p->iDeviceFound = EFalse;
            error = UEZ_ERROR_NOT_FOUND;
        }
        if(error){
            p->iNumOpen--;
            return UEZ_ERROR_NOT_FOUND;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the flash device.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_Close(void *aWorkspace)
{
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;
    if (p->iNumOpen) {
        p->iNumOpen--;
        if (p->iNumOpen == 0) {
            IMicron_M25P80_End(p);
        }
        return UEZ_ERROR_NONE;
    } else {
        return UEZ_ERROR_NOT_OPEN;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read bytes out of the Flash.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte Offset address into flash to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_Read(
    void *aWorkspace,
    TUInt32 aOffset,
    TUInt8 *aBuffer,
    TUInt32 aNumBytes)
{
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt8 dataOut[5];
    TBool isBusy = ETrue;

    if(!p->iDeviceFound){
        return UEZ_ERROR_NOT_FOUND;
    }

    // Make sure its not in a programming mode.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    while(isBusy){
        if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
            return UEZ_ERROR_INTERNAL_ERROR;
        }
    }

    dataOut[0] = MICRON_M23P80_READ_DATA;
    dataOut[1] = (TUInt8)((aOffset >> 16) & 0xFF);
    dataOut[2] = (TUInt8)((aOffset >> 8) & 0xFF);
    dataOut[3] = (TUInt8)((aOffset >> 0) & 0xFF);
    //dataOut[4] = 0; //dummy byte High speed read command only

    IMicron_M25P80_Command(p, dataOut, 4, aBuffer, aNumBytes);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write bytes into the Flash. Expecting that writes start on sector
 *      Boundaries. Writes larger then one sector will erase the next
 *      sector.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte Offset into flash to write
 *      TUInt8 *aBuffer             -- Pointer to buffer of data
 *      TUInt32 aNumBytes           -- Number of bytes to write
 * Outputs:
 *      T_uezError                  -- Error code.  Returns
 *                                      UEZ_ERROR_BAD_ALIGNMENT if offset
 *                                      or number of bytes
 *                                      is not on word boundary.
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_Write(
    void *aWorkspace,
    TUInt32 aOffset,
    TUInt8 *aBuffer,
    TUInt32 aNumBytes)
{
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 startSector;
    TUInt32 currentSector;
    TUInt32 bytesToWrite = aNumBytes;
    TUInt8 dataOut[4 + MICRON_M23P80_PAGE_SIZE];
    TUInt32 dataToWrite;
    TBool isBusy = ETrue;

    if(!p->iDeviceFound){
        return UEZ_ERROR_NOT_FOUND;
    }

    if((aOffset == 0) || ((aOffset % MICRON_M23P80_SECTOR_SIZE) == 0)){
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
        currentSector = startSector = IMicron_M25P80_OffsetToSector(p, aOffset);

        while(bytesToWrite){
            if(bytesToWrite > MICRON_M23P80_PAGE_SIZE){
                dataToWrite = MICRON_M23P80_PAGE_SIZE;
            } else {
                dataToWrite = bytesToWrite;
            }
            if(G_sectorsNeedingErase[currentSector]){
#if 1
                return UEZ_ERROR_ERASE_REQUIRED;
#else
                //TODO: Discuss this option
                //May need to change to returning and error UEZ_ERROR_ERASE_REQUIRED
                while(isBusy){
                    if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
                        return UEZ_ERROR_INTERNAL_ERROR;
                    }
                }
                isBusy = ETrue;

                dataOut[0] = MICRON_M23P80_WRITE_ENABLE;
                if ((error = IMicron_M25P80_Command(p, dataOut, 1, NULL, 0))
                        == UEZ_ERROR_NONE) {
                    while(isBusy){
                        if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
                            return UEZ_ERROR_INTERNAL_ERROR;
                        }
                    }
                    isBusy = ETrue;

                    dataOut[0] = MICRON_M23P80_SECTOR_ERASE;
                    dataOut[1] = (TUInt8)((aOffset >> 16) & 0xFF);
                    dataOut[2] = (TUInt8)((aOffset >> 8) & 0xFF);
                    dataOut[3] = (TUInt8)((aOffset >> 8) & 0xFF);
                    if ((error = IMicron_M25P80_Command(p, dataOut, 4, NULL, 0))
                            == UEZ_ERROR_NONE) {
                        G_sectorsNeedingErase[currentSector] = EFalse;
                    }
                }
                if(error){
                    break;
                }
#endif
            }
            while(isBusy){
                if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
                    return UEZ_ERROR_INTERNAL_ERROR;
                }
            }
            isBusy = ETrue;

            //Ready to start writing data
            dataOut[0] = MICRON_M23P80_WRITE_ENABLE;
            if ((error = IMicron_M25P80_Command(p, dataOut, 1, NULL, 0))
                    == UEZ_ERROR_NONE) {
                //while(isBusy){
                //    if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
                //        return UEZ_ERROR_INTERNAL_ERROR;
                //    }
                //}
                //isBusy = ETrue;

                dataOut[0] = MICRON_M23P80_PAGE_PROGRAM;
                dataOut[1] = (TUInt8)((aOffset >> 16) & 0xFF);
                dataOut[2] = (TUInt8)((aOffset >> 8) & 0xFF);
                dataOut[3] = (TUInt8)((aOffset >> 0) & 0xFF);

                memcpy((void*)&dataOut[4], aBuffer, dataToWrite);

                error = IMicron_M25P80_Command(p, dataOut, dataToWrite + 4, NULL, 0);
            }
            if(error){
                break;
            }
            bytesToWrite -= dataToWrite;
            aBuffer += dataToWrite;
            aOffset += dataToWrite;
            if((aOffset % MICRON_M23P80_SECTOR_SIZE) == 0){
                G_sectorsNeedingErase[currentSector] = ETrue;
                currentSector++;
            }
        }
        G_sectorsNeedingErase[currentSector] = ETrue;
        UEZSemaphoreRelease(p->iSem);
    } else {
        error = UEZ_ERROR_BAD_ALIGNMENT;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_BlockErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase one or more blocks in the given location. The offset should fall on
 *      a sector boundary. Any overflow to the next sector will result in data loss.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte offset into the flash memory
 *      TUInt32 aNumBytes           -- Number of bytes
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_BlockErase(
    void *aWorkspace,
    TUInt32 aOffset,
    TUInt32 aNumBytes)
{
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 startSector;
    TUInt32 currentSector;
    TUInt32 bytesToErase = aNumBytes;
    TUInt8 dataOut[5];
    TBool isBusy = ETrue;
    
    if(!p->iDeviceFound){
        return UEZ_ERROR_NOT_FOUND;
    }

    if((aOffset == 0) || ((aOffset % MICRON_M23P80_SECTOR_SIZE) == 0)){
        //offset is a sector boundary
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

        currentSector = startSector = IMicron_M25P80_OffsetToSector(p, aOffset);

        while(bytesToErase){
            if(G_sectorsNeedingErase[currentSector]){
                while(isBusy){
                    if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
                        return UEZ_ERROR_INTERNAL_ERROR;
                    }
                }
                isBusy = ETrue;

                dataOut[0] = MICRON_M23P80_WRITE_ENABLE;
                if ((error = IMicron_M25P80_Command(p, dataOut, 1, NULL, 0))
                        == UEZ_ERROR_NONE) {
                    dataOut[0] = MICRON_M23P80_SECTOR_ERASE;
                    dataOut[1] = (TUInt8)((G_SectorMemory[currentSector].iAddress >> 0) & 0xFF);
                    dataOut[2] = (TUInt8)((G_SectorMemory[currentSector].iAddress >> 8) & 0xFF);
                    dataOut[3] = (TUInt8)((G_SectorMemory[currentSector].iAddress >> 16) & 0xFF);

                    while(isBusy){
                        if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
                            return UEZ_ERROR_INTERNAL_ERROR;
                        }
                    }
                    isBusy = ETrue;

                    if ((error = IMicron_M25P80_Command(p, dataOut, sizeof(dataOut), NULL, 0))
                            == UEZ_ERROR_NONE) {
                        G_sectorsNeedingErase[currentSector] = EFalse;
                    }
                }
                if(error){
                    break;
                }
            }
            if(bytesToErase >= MICRON_M23P80_SECTOR_SIZE){
                bytesToErase -= MICRON_M23P80_SECTOR_SIZE;
            }else {
                bytesToErase = 0;
            }
            currentSector++;
        }

        // Done erasing, resume normal control
        UEZSemaphoreRelease(p->iSem);
    } else {
        error = UEZ_ERROR_BAD_ALIGNMENT;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_ChipErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the complete chip.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_ChipErase(void *aWorkspace)
{
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt8 dataOut[1];
    TBool isBusy = ETrue;

    if(!p->iDeviceFound){
        return UEZ_ERROR_NOT_FOUND;
    }

    // Prepare the sector for erase
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);


    while(isBusy){
        if(IMicron_M25P80_IsBusy(p, &isBusy) != UEZ_ERROR_NONE){
            return UEZ_ERROR_INTERNAL_ERROR;
        }
    }
    isBusy = ETrue;

    dataOut[0] = MICRON_M23P80_WRITE_ENABLE;
    if ((error = IMicron_M25P80_Command(p, dataOut, 1, NULL, 0))
            == UEZ_ERROR_NONE) {
        isBusy = ETrue;

        dataOut[0] = MICRON_M23P80_BULK_ERASE;
        if ((error = IMicron_M25P80_Command(p, dataOut, sizeof(dataOut), NULL, 0))
                == UEZ_ERROR_NONE) {
            memset((void*)G_sectorsNeedingErase, 0x00,
                    sizeof(G_sectorsNeedingErase));
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_QueryReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Query the chip for the given register (word based address).
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aReg                -- Register to query
 *      TUInt32 *aValue             -- Place to store value returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_QueryReg(
    void *aWorkspace,
    TUInt32 aReg,
    TUInt32 *aValue)
{
    T_uezError error = UEZ_ERROR_NONE;
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aReg);

    // Do nothing
    *aValue = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_GetChipInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the chip size
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_FlashChipInfo *aInfo      -- Pointer to structure to receive info
 * Outputs:
 *      T_uezError                   -- Error code.  Returns UEZ_ERROR_NOT_FOUND
 *                                      if flash information is not found
 *                                      (in this case, missing QRY header)
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_GetChipInfo(
    void *aWorkspace,
    T_FlashChipInfo *aInfo)
{
    T_Flash_Micron_M25P80_Workspace *p =
        (T_Flash_Micron_M25P80_Workspace *)aWorkspace;

    memset((void*)aInfo, 0, sizeof(T_FlashChipInfo));

    if(!p->iDeviceFound){
        return UEZ_ERROR_NOT_FOUND;
    }

    aInfo->iNumRegions = MICRON_M23P80_NUM_SECTORS;
    aInfo->iBitWidth = 32;
    aInfo->iNumBytesHigh = 0;
    aInfo->iNumBytesLow = MICRON_M23P80_SECTOR_SIZE * MICRON_M23P80_NUM_SECTORS;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for Data Flash.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_InitializeWorkspace(void *aW)
{
    T_Flash_Micron_M25P80_Workspace *p = (T_Flash_Micron_M25P80_Workspace *)aW;
    T_uezError error;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iNumOpen = 0;

    memset((void*)G_sectorsNeedingErase, 0xFF, sizeof(G_sectorsNeedingErase));

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Micron_M25P80_GetBlockInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the block
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte offset into flash
 *      T_FlashChipInfo *aInfo      -- Pointer to structure to receive info
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Micron_M25P80_GetBlockInfo(
    void *aWorkspace,
    TUInt32 aOffset,
    T_FlashBlockInfo *aBlockInfo)
{
    PARAM_NOT_USED(aWorkspace);

    aBlockInfo->iOffset = aOffset;
    aBlockInfo->iSize = MICRON_M23P80_SECTOR_SIZE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  TS_M25P80_Configure
*---------------------------------------------------------------------------*
* Description:
*      The SPI bus and GPIO device are being declared.
* Inputs:
*      void *aW                    -- Particular SPI workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
static T_uezError TS_M25P80_Configure(T_uezDeviceWorkspace *aW)
{
    T_Flash_Micron_M25P80_Workspace *p = (T_Flash_Micron_M25P80_Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    //Chip Select
    UEZGPIOSet(p->iChipSelect);
    UEZGPIOOutput(p->iChipSelect);
    UEZGPIOSetMux(p->iChipSelect, 0);

    //Write Protect
    UEZGPIOSet(p->iWriteProtect);
    UEZGPIOOutput(p->iWriteProtect);
    UEZGPIOSetMux(p->iWriteProtect, 0);

    //Reset
    UEZGPIOSet(p->iReset);
    UEZGPIOOutput(p->iReset);
    UEZGPIOSetMux(p->iReset, 0);

    p->iRequest.iBitsPerTransfer = 8;
    p->iRequest.iRate = MICRON_M23P80_CLOCK_SPEED;
    p->iRequest.iClockOutPolarity = ETrue; // rising edge
    p->iRequest.iClockOutPhase = ETrue;
    p->iRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(p->iChipSelect);
    p->iRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(p->iChipSelect);
    p->iRequest.iCSPolarity = EFalse;
    p->iRequest.iFlags = 0;

   return error;
}

void Flash_Micron_M25P80_Create(
        const char *aName,
        const char* aSPIPortName,
        T_uezGPIOPortPin aChipSelect,
        T_uezGPIOPortPin aWriteProtect,
        T_uezGPIOPortPin aReset)
{
    T_uezDeviceWorkspace *workspace;
    T_Flash_Micron_M25P80_Workspace *p;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Flash_Micron_M25P80_Interface,
            0,
            &workspace);
    p = (T_Flash_Micron_M25P80_Workspace*)workspace;

    strcpy(p->iSPIPortName, aSPIPortName);
    p->iChipSelect = aChipSelect;
    p->iWriteProtect = aWriteProtect;
    p->iReset = aReset;

    TS_M25P80_Configure((void *)p);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Flash Flash_Micron_M25P80_Interface = { {
// Common device interface
    "Flash:Micron:M25P80",
    0x0100,
    Flash_Micron_M25P80_InitializeWorkspace,
    sizeof(T_Flash_Micron_M25P80_Workspace), },

// Functions
    Flash_Micron_M25P80_Open,
    Flash_Micron_M25P80_Close,
    Flash_Micron_M25P80_Read,
    Flash_Micron_M25P80_Write,
    Flash_Micron_M25P80_BlockErase,
    Flash_Micron_M25P80_ChipErase,
    Flash_Micron_M25P80_QueryReg,
    Flash_Micron_M25P80_GetChipInfo,
    Flash_Micron_M25P80_GetBlockInfo,
};

/*-------------------------------------------------------------------------*
 * End of File:  Micron_M25P80.c
 *-------------------------------------------------------------------------*/

