/*-------------------------------------------------------------------------*
 * File:  Flash_NXP_LPC_SPIFI_M4_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Flash for NXP LPC SPIFI M4 devices
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2013 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZMemory.h>
#include <uEZProcessor.h>
#include "Source/Devices/Flash/NXP/LPC_SPIFI_M4F/spifilib_api.h"
#include "Source/Devices/Flash/NXP/LPC_SPIFI_M4F/spifilib_dev.h"
#include "Source/Devices/Flash/NXP/LPC_SPIFI_M4F/spifilib_chiphw.h"
#include "Source/Devices/Flash/NXP/LPC_SPIFI_M4F/Flash_NXP_LPC_SPIFI_M4.h"
#include "spifilib_dev.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#if (UEZ_PROCESSOR == NXP_LPC4088)
#define SPIFLASH_BASE_ADDRESS           (0x28000000)
#define SPIFI_BASE                      LPC_SPIFI_BASE
#define LPC_SPIFI_BASE                  (0x20080000  + 0x14000)
#elif (UEZ_PROCESSOR == NXP_LPC4357)
#define SPIFLASH_BASE_ADDRESS           (0x14000000)
#define SPIFI_BASE                      LPC_SPIFI_BASE
#elif (UEZ_PROCESSOR == NXP_LPC546xx)
#define SPIFLASH_BASE_ADDRESS           (0x10000000)
#define SPIFI_BASE                      SPIFI0_BASE
#else
#error "MCU not supported!"
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Flash *iDevice;
    T_uezSemaphore iSem;
    SPIFI_HANDLE_T *iPSPIFI;
} T_Flash_NXP_LPC_SPIFI_M4_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for Flash.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC_SPIFI_M4_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the flash device.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC_SPIFI_M4_Open(void *aWorkspace)
{
    // Nothing to do
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_Close
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
T_uezError Flash_NXP_LPC_SPIFI_M4_Close(void *aWorkspace)
{
    // Nothing to do
    return UEZ_ERROR_NONE;
}

T_uezError IFlash_NXP_LPC_SPIFI_M4_HandleError(TUInt32 aSpifiResult)
{
    switch(aSpifiResult) {
        case SPIFI_ERR_NONE:
            return UEZ_ERROR_NONE;
        case SPIFI_ERR_BUSY:
            return UEZ_ERROR_BUSY;
        case SPIFI_ERR_GEN:
            return UEZ_ERROR_DEVICE_STATUS;
        case SPIFI_ERR_NOTSUPPORTED:
            return UEZ_ERROR_NOT_SUPPORTED;
        case SPIFI_ERR_ALIGNERR:
            return UEZ_ERROR_BAD_ALIGNMENT;
        case SPIFI_ERR_LOCKED:
            return UEZ_ERROR_PROTECTED;
        case SPIFI_ERR_PROGERR:
        case SPIFI_ERR_ERASEERR:
        case SPIFI_ERR_NOTBLANK:
        case SPIFI_ERR_PAGESIZE:
        case SPIFI_ERR_VAL:
            return UEZ_ERROR_INVALID;
        case SPIFI_ERR_RANGE:
            return UEZ_ERROR_OUT_OF_RANGE;
        case SPIFI_ERR_MEMMODE:
        default:
            return UEZ_ERROR_INVALID;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_Read
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
T_uezError Flash_NXP_LPC_SPIFI_M4_Read(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;
    TUInt32 spifiResult = 0;

    // Make sure its not in a programming mode.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    spifiDevSetMemMode(p->iPSPIFI, 0);
    //memcpy((void*)aBuffer, (void*)(((volatile TUInt8 *)SPIFLASH_BASE_ADDRESS)+ aOffset), aNumBytes);
    spifiResult = spifiDevRead(p->iPSPIFI, SPIFLASH_BASE_ADDRESS + aOffset, (TUInt32*)aBuffer, aNumBytes);
    spifiDevSetMemMode(p->iPSPIFI, 1);

    UEZSemaphoreRelease(p->iSem);

    return IFlash_NXP_LPC_SPIFI_M4_HandleError(spifiResult);
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write bytes into the Flash
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
T_uezError Flash_NXP_LPC_SPIFI_M4_Write(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;
    TUInt32 spifiResult = 0;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    spifiDevSetMemMode(p->iPSPIFI, 0);
    //What's going to happen when the buffer is converted?
    spifiResult = spifiProgram(p->iPSPIFI, SPIFLASH_BASE_ADDRESS + aOffset, (TUInt32*)aBuffer, aNumBytes);
    spifiDevSetMemMode(p->iPSPIFI, 1);

    UEZSemaphoreRelease(p->iSem);

    return IFlash_NXP_LPC_SPIFI_M4_HandleError(spifiResult);
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_BlockErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase one ore more blocks in the given location
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to write
 *      TUInt8 *aBuffer             -- Pointer to buffer of data
 *      TUInt32 aNumBytes           -- Number of bytes to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC_SPIFI_M4_BlockErase(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt32 aNumBytes)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;
    TUInt32 spifiResult = 0;
    TUInt32 firstSubBlock, lastsubBlock;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    spifiDevSetMemMode(p->iPSPIFI, 0);
    firstSubBlock = spifiGetSubBlockFromAddr(p->iPSPIFI, SPIFLASH_BASE_ADDRESS + aOffset);
    lastsubBlock  = spifiGetSubBlockFromAddr(p->iPSPIFI, SPIFLASH_BASE_ADDRESS + aOffset + aNumBytes - 1);

    /* Limit to legal address range */
    if ((firstSubBlock != ~0UL) && (lastsubBlock != ~0UL)) {
        spifiResult = spifiEraseSubBlocks(p->iPSPIFI, firstSubBlock, ((lastsubBlock - firstSubBlock) + 1));
    }

    spifiDevSetMemMode(p->iPSPIFI, 1);

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return IFlash_NXP_LPC_SPIFI_M4_HandleError(spifiResult);
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_ChipErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the complete chip
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC_SPIFI_M4_ChipErase(void *aWorkspace)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;
    TUInt32 spifiResult = 0;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    spifiDevSetMemMode(p->iPSPIFI, 0);
    spifiResult = spifiEraseByAddr(p->iPSPIFI, SPIFLASH_BASE_ADDRESS,
            spifiDevGetInfo(p->iPSPIFI, SPIFI_INFO_DEVSIZE));
    spifiDevSetMemMode(p->iPSPIFI, 1);

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return IFlash_NXP_LPC_SPIFI_M4_HandleError(spifiResult);
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_QueryReg
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
T_uezError Flash_NXP_LPC_SPIFI_M4_QueryReg(
        void *aWorkspace,
        TUInt32 aReg,
        TUInt32 *aValue)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_GetChipInfo
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
T_uezError Flash_NXP_LPC_SPIFI_M4_GetChipInfo(
        void *aWorkspace,
        T_FlashChipInfo *aInfo)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;

    aInfo->iNumBytesLow = spifiDevGetInfo(p->iPSPIFI, SPIFI_INFO_DEVSIZE);

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_GetBlockInfo
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
T_uezError Flash_NXP_LPC_SPIFI_M4_GetBlockInfo(
        void *aWorkspace,
        TUInt32 aOffset,
        T_FlashBlockInfo *aBlockInfo)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;

    aBlockInfo->iOffset = 0;
    aBlockInfo->iSize = spifiDevGetInfo(p->iPSPIFI, SPIFI_INFO_ERASE_BLOCKSIZE);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  I_SPIFI_IO_CLK_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn ON SPIFI and configure pins for 4088
 *---------------------------------------------------------------------------*/
static void I_SPIFI_IO_CLK_Init()
{
#if (UEZ_PROCESSOR == NXP_LPC4088)
    LPC_SC->PCONP |= 0x00010000;

    // Ensure power is on to the part
    LPC17xx_40xxPowerOn(1UL << 16);

    LPC_IOCON->P2_7 &= ~0x07;
    LPC_IOCON->P2_7 |= 0x05;    /* SPIFI_CSN @ P2.7 */
    LPC_IOCON->P0_22 &= ~0x07;
    LPC_IOCON->P0_22 |= 0x05;   /* SPIFI_CLK @ P0.22 */
    LPC_IOCON->P0_15 &= ~0x07;
    LPC_IOCON->P0_15 |= 0x5;    /* SPIFI_IO2 @ P0.15 */
    LPC_IOCON->P0_16 &= ~0x07;
    LPC_IOCON->P0_16 |= 0x5;    /* SPIFI_IO3 @ P0.16 */
    LPC_IOCON->P0_17 &= ~0x07;
    LPC_IOCON->P0_17 |= 0x5;    /* SPIFI_IO1 @ P0.17 */
    LPC_IOCON->P0_18 &= ~0x07;
    LPC_IOCON->P0_18 |= 0x5;    /* SPIFI_IO0 @ P0.18 */
#elif(UEZ_PROCESSOR == NXP_LPC4357)
    LPC_SCU->SFSP3_8 = ((1 << 7) | (1 << 6) | (1 << 5) | (2 << 3)) | 3;
    LPC_SCU->SFSP3_3 = ((1 << 7) | (1 << 6) | (1 << 5) | (2 << 3)) | 3;
    LPC_SCU->SFSP3_7 = ((1 << 7) | (1 << 6) | (1 << 5) | (2 << 3)) | 3;
    LPC_SCU->SFSP3_6 = ((1 << 7) | (1 << 6) | (1 << 5) | (2 << 3)) | 3;
    LPC_SCU->SFSP3_5 = ((1 << 7) | (1 << 6) | (1 << 5) | (2 << 3)) | 3;
    LPC_SCU->SFSP3_4 = ((1 << 7) | (1 << 6) | (1 << 5) | (2 << 3)) | 3;
#elif(UEZ_PROCESSOR == NXP_LPC546xx)
    LPC546xxPowerOn(kCLOCK_Spifi);
    SYSCON->SPIFICLKSEL = 0; //main clock
    SYSCON->SPIFICLKDIV = 1;  //divide by 0 TODO: change when clock rate goes higher

    IOCON->PIO[0][23] = (1<<8) | (2<<4) | 6; //CS

    IOCON->PIO[0][24] = (1<<8) | (2<<4) | 6; //0
    IOCON->PIO[0][25] = (1<<8) | (2<<4) | 6; //0
    IOCON->PIO[0][28] = (1<<8) | (2<<4) | 6; //2
    IOCON->PIO[0][27] = (1<<8) | (2<<4) | 6; //3

    IOCON->PIO[0][26] = (1<<8) | (2<<4) | 6; //CLK
#else
#error "MCU not supported!"
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC_SPIFI_M4_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the flash chip's location
 * Inputs:
 *      void *aW                    -- Workspace
 *      void *aBaseAddr             -- Base address of this chip in
 *                                     processor memory map space
 *---------------------------------------------------------------------------*/
static uint32_t lmem[21];
void Flash_NXP_LPC_SPIFI_M4_Configure(
        void *aWorkspace)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;
    TUInt32 memSize;
    SPIFI_ERR_T spifiError;

    I_SPIFI_IO_CLK_Init();

    spifiError = spifiInit(SPIFI_BASE, ETrue);
    if(spifiError != 0){
      return;
    }

    spifiRegisterFamily(spifi_REG_FAMILY_CommonCommandSet);

    //TUInt32 idx = 
    spifiGetSuppFamilyCount();

    memSize = spifiGetHandleMemSize(SPIFI_BASE);
    if(memSize != 0){
        p->iPSPIFI = spifiInitDevice(&lmem, sizeof(lmem), SPIFI_BASE, SPIFLASH_BASE_ADDRESS);

        //spifiDevUnlockDevice(p->iPSPIFI);
        spifiDevSetMemMode(p->iPSPIFI, 1);
    }
}

/*---------------------------------------------------------------------------*
 * Create functions:
 *---------------------------------------------------------------------------*/
void Flash_NXP_LPC_SPIFI_M4_Create(const char *aName)
{
    T_uezDeviceWorkspace *p_flash;

    // Setup the NOR flash
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Flash_NXP_LPC_SPIFI_M4_Interface,
            0,
            &p_flash);
    Flash_NXP_LPC_SPIFI_M4_Configure(p_flash);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Flash Flash_NXP_LPC_SPIFI_M4_Interface = {
    {
            // Common device interface
        "Flash:NXP:LPC:SPIFI:M4",
        0x0100,
        Flash_NXP_LPC_SPIFI_M4_InitializeWorkspace,
        sizeof(T_Flash_NXP_LPC_SPIFI_M4_Workspace),
    },

    // Functions
    Flash_NXP_LPC_SPIFI_M4_Open,
    Flash_NXP_LPC_SPIFI_M4_Close,
    Flash_NXP_LPC_SPIFI_M4_Read,
    Flash_NXP_LPC_SPIFI_M4_Write,
    Flash_NXP_LPC_SPIFI_M4_BlockErase,
    Flash_NXP_LPC_SPIFI_M4_ChipErase,
    Flash_NXP_LPC_SPIFI_M4_QueryReg,
    Flash_NXP_LPC_SPIFI_M4_GetChipInfo,
    Flash_NXP_LPC_SPIFI_M4_GetBlockInfo,
} ;
/*-------------------------------------------------------------------------*
 * End of File:  Flash_NXP_LPC_SPIFI_M4_driver.c
 *-------------------------------------------------------------------------*/
