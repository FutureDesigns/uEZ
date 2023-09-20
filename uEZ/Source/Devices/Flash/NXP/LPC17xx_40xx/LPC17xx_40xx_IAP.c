/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_IAP.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <string.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include "LPC17xx_40xx_IAP.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define MAX_NUM_SECTORS             32
#define BLOCK_SIZE                  256

#define IAP_CMD_PREPARE_SECTOR      50
#define IAP_CMD_COPY_TO_FLASH       51
#define IAP_CMD_ERASE_SECTOR        52

typedef TUInt32 TIAPResult;
#define CMD_SUCCESS                 0
#define INVALID_COMMAND             1
#define SRC_ADDR_ERROR              2
#define DST_ADDR_ERROR              3
#define SRC_ADDR_NOT_MAPPED         4
#define DST_ADDR_NOT_MAPPED         5
#define COUNT_ERROR                 6
#define INVALID_SECTOR              7
#define SECTOR_NOT_BLANK            8
#define SECTOR_NOT_PREPARED         9
#define COMPARE_ERROR               10
#define BUSY                        11
#define NOTHING_HAPPENED            0xFF

#define IAP_ROUTINE                 0x1FFF1FF1
#define BLOCK_NONE                  0xFFFFFFFF

#define CONFIG_DISABLE_PLL_ON_IAP_COMMANDS          0
#if CONFIG_DISABLE_PLL_ON_IAP_COMMANDS
#define PRG_FREQUENCY               XTAL_FREQUENCY
#else
#define PRG_FREQUENCY               PROCESSOR_OSCILLATOR_FREQUENCY
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TInt32 iIndex;
    TUInt32 iAddress;
    TUInt32 iSize;
} T_sector;

typedef struct {
    const DEVICE_Flash *iDevice;
    T_uezSemaphore iSem;
    TUInt32 iNumOpen;
    T_FlashChipInfo iChipInfo;
} T_Flash_NXP_LPC17xx_40xx_Workspace;

typedef void (*IAPRoutine)(TUInt32 *aParams, TUInt32 *aResults);
#define IAPCall       ((IAPRoutine)IAP_ROUTINE)

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static T_uezError IIAPCmdErase(TUInt32 aSector);
static T_uezError IIAPPrepareSector(TUInt32 aAddress);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
const T_sector G_sectorMemory[MAX_NUM_SECTORS] = {
#if (LPC17xx_40xx_IAP_PROTECT_FIRST_64K==1)
    {  -1,  0x00000000, 0x00010000 }, // Ignore first 64K
#else
    {   0,  0x00000000, 0x00001000 },
    {   1,  0x00001000, 0x00001000 },
    {   2,  0x00002000, 0x00001000 },
    {   3,  0x00003000, 0x00001000 },
    {   4,  0x00004000, 0x00001000 },
    {   5,  0x00005000, 0x00001000 },
    {   6,  0x00006000, 0x00001000 },
    {   7,  0x00007000, 0x00001000 },
    {   8,  0x00008000, 0x00001000 },
    {   9,  0x00009000, 0x00001000 },
    {  10,  0x0000A000, 0x00001000 },
    {  11,  0x0000B000, 0x00001000 },
    {  12,  0x0000C000, 0x00001000 },
    {  13,  0x0000D000, 0x00001000 },
    {  14,  0x0000E000, 0x00001000 },
    {  15,  0x0000F000, 0x00001000 },
#endif    
    {  16,  0x00010000, 0x00008000 },
    {  17,  0x00018000, 0x00008000 },
    {  18,  0x00020000, 0x00008000 },
    {  19,  0x00028000, 0x00008000 },
    {  20,  0x00030000, 0x00008000 },
    {  21,  0x00038000, 0x00008000 },
    {  22,  0x00040000, 0x00008000 },
    {  23,  0x00048000, 0x00008000 },
    {  24,  0x00050000, 0x00008000 },
    {  25,  0x00058000, 0x00008000 },
    {  26,  0x00060000, 0x00008000 },
    {  27,  0x00068000, 0x00008000 },
    {  28,  0x00070000, 0x00008000 },
    {  29,  0x00078000, 0x00008000 },
    {  -1,  0x00080000, 0xFFF80000 }    // Stop here
};

static TBool G_sectorsNeedingErase[MAX_NUM_SECTORS];
static TUInt8 G_block[BLOCK_SIZE] IN_INTERNAL_RAM;
static TUInt32 G_parameters[5] IN_INTERNAL_RAM;
static TUInt32 G_results[4] IN_INTERNAL_RAM;
static TUInt32 G_lastBlock;

/*-------------------------------------------------------------------------*
 * Function:  CPUEnableInterrupts
 *-------------------------------------------------------------------------*
 * Description:
 *      Enables all standard CPU interrupts (IRQ).
 *-------------------------------------------------------------------------*/
static TIAPResult IIAPCmd(TUInt8 aCmd)
{
    // Pass on the command
    G_parameters[0] = aCmd;

    // Zap results so we know if nothing happened
    G_results[0] = NOTHING_HAPPENED;

    // Execute with interrupts turned off
    CPUDisableInterrupts();

    // Before doing any IAP commands, turn off the PLL
#if CONFIG_DISABLE_PLL_ON_IAP_COMMANDS
    CPUPLLStop();
#endif

    IAPCall(G_parameters, G_results);

    // Regardless of the result, turn back on the PLL
#if CONFIG_DISABLE_PLL_ON_IAP_COMMANDS
    CPUPLLStart();
#endif

    CPUEnableInterrupts();

    return G_results[0];
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPAddressToSectorNumber
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the address into a sector number.
 * Outputs:
 *      TUInt32                   -- Sector number, or -1 if invalid.
 *---------------------------------------------------------------------------*/
static TInt32 IIAPAddressToSectorNumber(TUInt32 aAddress)
{
    const T_sector *p_sector = G_sectorMemory;
    const T_sector *p_prev = 0;

    while (1) {
        if (aAddress < p_sector->iAddress)
            return p_prev->iIndex;
        p_prev = p_sector;
        p_sector++;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPEraseIfNotErased
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the given sector if not already erased.
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
static T_uezError IIAPEraseIfNotErased(TUInt32 aSector)
{
    T_uezError error;
    if (G_sectorsNeedingErase[aSector])
        error = IIAPCmdErase(aSector);
    else
        error = UEZ_ERROR_NONE;
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPCopyBytes
 *---------------------------------------------------------------------------*
 * Description:
 *      Copy in a section of bytes into the block offset by a prefix.
 * Inputs:
 *      TUInt8 *aData -- Pointer to data to read
 *      TUInt32 aPrefix -- Offset into the block to read
 *      TUInt32 aNumBytes -- Number of bytes to read.
 *---------------------------------------------------------------------------*/
static void IIAPCopyBytes(TUInt8 *aData, TUInt32 aPrefix, TUInt32 aNumBytes)
{
    TUInt32 i;

    // Copy over the section of bytes we are going to
    // write
    for (i = 0; i < aNumBytes; i++, aData++)
        G_block[aPrefix++] = *aData;
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPCmdErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the given sector.
 * Inputs:
 *      TUInt32 aSector -- Sector to erase
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
static T_uezError IIAPCmdErase(TUInt32 aSector)
{
    T_uezError error;

    // Prepare the sector for erase
    error = IIAPPrepareSector(aSector);
    if (error)
        return error;

    // Start sector
    G_parameters[1] = aSector;
    // End sector
    G_parameters[2] = aSector;
    // Clock frequency in kHz
    G_parameters[3] = PRG_FREQUENCY / 1000;

    if (IIAPCmd(IAP_CMD_ERASE_SECTOR) != CMD_SUCCESS)
        return UEZ_ERROR_COULD_NOT_ERASE;

    // Mark sector as erased
    G_sectorsNeedingErase[aSector] = EFalse;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPPrepareSector
 *---------------------------------------------------------------------------*
 * Description:
 *      Prepare a sector for modification.
 * Inputs:
 *      TUInt32 aSector -- Sector to prepare
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
static T_uezError IIAPPrepareSector(TUInt32 aSector)
{
    // Start and end sector are the same
    G_parameters[1] = G_parameters[2] = aSector;

    if (IIAPCmd(IAP_CMD_PREPARE_SECTOR) != CMD_SUCCESS)
        return UEZ_ERROR_INCOMPLETE;

    // Got here?  Then it was good
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPFlashBlock
 *---------------------------------------------------------------------------*
 * Description:
 *      Flash a block of data with the current block.
 * Inputs:
 *      TUInt32 aAddress -- Address to write block
 *      TUInt32 aNumBytes -- Number of bytes to write
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
static T_uezError IIAPFlashBlock(TUInt32 aAddress, TUInt32 aNumBytes)
{
    // First, we have to prep the location
    IIAPPrepareSector(IIAPAddressToSectorNumber(aAddress));

    // Now we can write the complete block
    G_parameters[1] = aAddress;
    G_parameters[2] = (TUInt32)G_block;
    G_parameters[3] = aNumBytes;
    G_parameters[4] = PRG_FREQUENCY / 1000;

    if (IIAPCmd(IAP_CMD_COPY_TO_FLASH) != CMD_SUCCESS)
        return UEZ_ERROR_READ_WRITE_ERROR;

    // Got here, must have been good
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPSelectBlock
 *---------------------------------------------------------------------------*
 * Description:
 *      Select which block we are going to modify next
 * Inputs:
 *      TUInt32 aBlockAddress -- Address to select
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
static T_uezError IIAPSelectBlock(TUInt32 aBlockAddress)
{
    TUInt32 i;
    TInt32 sector;
    T_uezError error = UEZ_ERROR_NONE;

    // Is this the same block as last time?
    if (aBlockAddress != G_lastBlock) {
        // We're about to change blocks
        // Is the last block a real block?
        if (G_lastBlock != BLOCK_NONE) {
            // Which sector is this located?
            sector = IIAPAddressToSectorNumber(G_lastBlock);

            // Is this allowed?  If not, respond with a failure
            if (sector == -1)
                return UEZ_ERROR_NOT_FOUND;

            // Erase this block if needed.  If error, fail.
            error = IIAPEraseIfNotErased(sector);
            if (error)
                return error;

            // Time to write this block to flash.
            // Do the actual write and report if failed.
            error = IIAPFlashBlock(G_lastBlock, BLOCK_SIZE);
            if (error)
                return error;
        }

        // Remember what the new block is
        G_lastBlock = aBlockAddress;

        // Fill this block with FFs to match blank flash
        for (i = 0; i < BLOCK_SIZE; i++)
            G_block[i] = 0xFF;

        // We're good and ready
        return error;
    }

    // Otherwise, we're in the same block, keep going
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IIAPWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a block of data given an address, some data, and
 *      a number of bytes.  Update the address, data pointer, and number
 *      of bytes for the next block.  Only one block is written before
 *      this routine returns.
 * Inputs:
 *      TUInt32 *aAddress -- Pointer to address to write
 *      TUInt8 **aData -- Pointer to a pointer to the data
 *      TUInt32 *aNumBytes -- Pointer to the number of total bytes to write
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
static T_uezError IIAPWrite(
    TUInt32 *aAddress,
    TUInt8 **aData,
    TUInt32 *aNumBytes)
{
    TUInt32 prefix;
    TUInt32 numBytes;
    TUInt32 blockAddr;
    T_uezError error = UEZ_ERROR_NONE;

    // How many bytes of 'prefix' (bytes at the front) do we need skip
    prefix = *aAddress % BLOCK_SIZE;

    // How many bytes would be left after the prefix
    numBytes = BLOCK_SIZE - prefix;

    // Trim the number of bytes to what we are asking to write
    if (numBytes > *aNumBytes)
        numBytes = *aNumBytes;

    // Where does this block start (should be <= *aAddress)
    blockAddr = *aAddress - prefix;

    // Switch to the right block in memory, writing the last
    // block if needed
    error = IIAPSelectBlock(blockAddr);
    if (error)
        return error;

    // Copy over the data we are going to write
    IIAPCopyBytes(*aData, prefix, numBytes);

    // Update for next position
    *aAddress += numBytes;
    *aNumBytes -= numBytes;
    *aData += numBytes;

    // Everything went through, should be good now.
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IAPWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a section of bytes in the flash at the given address, with
 *      the given data, and a number of bytes.  Blocks are erased as
 *      needed (if that option was enabled).
 * Inputs:
 *      TUInt32 aAddress -- Address to write at
 *      TUInt8 *aData -- Pointer to data to write
 *      TUInt32 aNumBytes -- Number of bytes to write
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_IAP_Write(
    TUInt32 aAddress,
    TUInt8 *aData,
    TUInt32 aNumBytes)
{
    T_uezError error = UEZ_ERROR_NONE;

    // Write the given block of data to the given address
    // Cut up the write command into blocks of BLOCK_SIZE size.
    // Keep doing this until all the bytes are processed
    while (aNumBytes) {
        error = IIAPWrite(&aAddress, &aData, &aNumBytes);
        if (error != UEZ_ERROR_NONE)
            return error;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IAPSessionStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a new IAP session.  If ETrue is passed in, blocks need to
 *      be erased, otherwise try to overwrite existing blocks.
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_IAP_Start(TBool aNeedErasing)
{
    TUInt32 i;

    // Declare all sectors need to be erased
    for (i = 0; i < MAX_NUM_SECTORS; i++)
        G_sectorsNeedingErase[i] = aNeedErasing;

    // Declare no blocks loaded in memory
    G_lastBlock = BLOCK_NONE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IAPSessionEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      End an IAP session writing any buffered data.
 * Outputs:
 *      T_uezError -- UEZ_ERROR_NONE if successful, else error code.
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_IAP_End(void)
{
    // Write the last block out (if any)
    return IIAPSelectBlock(BLOCK_NONE);
}

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for Data Flash. For the NXP LPC17xx_40xx, we allow the MCU to
 *      access the data flash area.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC17xx_40xx_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Flash_NXP_LPC17xx_40xx_Workspace *p = (T_Flash_NXP_LPC17xx_40xx_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iNumOpen = 0;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_Open
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
T_uezError Flash_NXP_LPC17xx_40xx_Open(void *aWorkspace)
{
    T_Flash_NXP_LPC17xx_40xx_Workspace *p =
        (T_Flash_NXP_LPC17xx_40xx_Workspace *)aWorkspace;
    /* Declare erase operation result container variable */
    p->iNumOpen++;
    if (p->iNumOpen == 1) {
        // If the first time, assume we were in some programming mode
        // and need to reset and Re-enable access to the Flash Data area
        ILPC17xx_40xx_IAP_Start(ETrue);
    }

    return ((T_uezError)UEZ_ERROR_NONE);
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the flash device.  For the NXP LPC17xx_40xx, the flash area access
 *      needs to be closed using IExitProgramMode.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC17xx_40xx_Close(void *aWorkspace)
{
    T_Flash_NXP_LPC17xx_40xx_Workspace *p =
        (T_Flash_NXP_LPC17xx_40xx_Workspace *)aWorkspace;
    if (p->iNumOpen) {
        p->iNumOpen--;
        if (p->iNumOpen == 0) {
            ILPC17xx_40xx_IAP_End();
        }
        return UEZ_ERROR_NONE;
    } else {
        return UEZ_ERROR_NOT_OPEN;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_Read
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
T_uezError Flash_NXP_LPC17xx_40xx_Read(
    void *aWorkspace,
    TUInt32 aOffset,
    TUInt8 *aBuffer,
    TUInt32 aNumBytes)
{
    T_Flash_NXP_LPC17xx_40xx_Workspace *p =
        (T_Flash_NXP_LPC17xx_40xx_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Make sure its not in a programming mode.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    ILPC17xx_40xx_IAP_End();

    // The memory is now accessible, copy the data over
    // Simple copy with check to see if the requested area is within the data flash
    if ((aOffset + (TUInt32)aNumBytes) <= (512 * 1024)) {
        memcpy(aBuffer, (void *)(aOffset), aNumBytes);
    } else {
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_Write
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
T_uezError Flash_NXP_LPC17xx_40xx_Write(
    void *aWorkspace,
    TUInt32 aOffset,
    TUInt8 *aBuffer,
    TUInt32 aNumBytes)
{
    T_Flash_NXP_LPC17xx_40xx_Workspace *p =
        (T_Flash_NXP_LPC17xx_40xx_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Let's do it
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Write the given block of data to the given address
    // Cut up the write command into blocks of BLOCK_SIZE size.
    // Keep doing this until all the bytes are processed
    while (aNumBytes) {
        error = IIAPWrite(&aOffset, &aBuffer, &aNumBytes);
        if (error != UEZ_ERROR_NONE)
            break;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_BlockErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase one or more blocks in the given location
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte offset into the flash memory
 *      TUInt32 aNumBytes           -- Number of bytes
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC17xx_40xx_BlockErase(
    void *aWorkspace,
    TUInt32 aOffset,
    TUInt32 aNumBytes)
{
    T_Flash_NXP_LPC17xx_40xx_Workspace *p =
        (T_Flash_NXP_LPC17xx_40xx_Workspace *)aWorkspace;
    TInt32 blockNum;
    TInt32 blockEnd;

    T_uezError error = UEZ_ERROR_NONE;

    // No bytes?  Do nothing
    if (aNumBytes == 0)
        return UEZ_ERROR_NONE;

    // Find the block
    blockNum = IIAPAddressToSectorNumber(aOffset);
    if (blockNum == -1)
        return UEZ_ERROR_NOT_FOUND;
    blockEnd = IIAPAddressToSectorNumber(aOffset+aNumBytes-1);
    if (blockEnd == -1)
        return UEZ_ERROR_NOT_FOUND;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Erase that block
    while (blockNum <= blockEnd) {
        error = IIAPEraseIfNotErased(blockNum++);
        if (error)
            break;
    }

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_ChipErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the complete chip
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_NXP_LPC17xx_40xx_ChipErase(void *aWorkspace)
{
    T_Flash_NXP_LPC17xx_40xx_Workspace *p =
        (T_Flash_NXP_LPC17xx_40xx_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 sector;

    // Prepare the sector for erase
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
#if LPC17xx_40xx_IAP_PROTECT_FIRST_64K
    sector=16; // Skip the first 64K
#else
    sector=0;
#endif      
    for (; sector<30; sector++) {
        error = IIAPPrepareSector(sector);
        if (error)
            break;

        // Start sector
        G_parameters[1] = sector;
        // End sector
        G_parameters[2] = sector;
        // Clock frequency in kHz
        G_parameters[3] = PRG_FREQUENCY / 1000;

        if (IIAPCmd(IAP_CMD_ERASE_SECTOR) != CMD_SUCCESS) {
            error = UEZ_ERROR_COULD_NOT_ERASE;
            break;
        }

        // Mark sector as erased
        G_sectorsNeedingErase[sector] = EFalse;
        break;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_QueryReg
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
T_uezError Flash_NXP_LPC17xx_40xx_QueryReg(
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
 * Routine:  Flash_NXP_LPC17xx_40xx_GetChipInfo
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
T_uezError Flash_NXP_LPC17xx_40xx_GetChipInfo(
    void *aWorkspace,
    T_FlashChipInfo *aInfo)
{
    PARAM_NOT_USED(aWorkspace);

    // Manually fill in the structure with a list
    // of 16x 4K blocks followed by 14x 16K blocks
    aInfo->iBitWidth = 8;
    aInfo->iNumBytesHigh = 0;
    aInfo->iNumBytesLow = 512*1024;
    aInfo->iNumRegions = 2;
    aInfo->iRegions[0].iNumEraseBlocks = 16;
    aInfo->iRegions[0].iSizeEraseBlock = 4*1024;
    aInfo->iRegions[1].iNumEraseBlocks = 14;
    aInfo->iRegions[1].iSizeEraseBlock = 32*1024;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_NXP_LPC17xx_40xx_GetBlockInfo
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
T_uezError Flash_NXP_LPC17xx_40xx_GetBlockInfo(
    void *aWorkspace,
    TUInt32 aOffset,
    T_FlashBlockInfo *aBlockInfo)
{
    if (aOffset < 0x10000) {
        // 4K blocks
        aBlockInfo->iOffset = aOffset & 0xFFFFF000;
        aBlockInfo->iSize = 0x1000;
    } else {
        // 32K blocks
        aBlockInfo->iOffset = aOffset & 0xFFFF8000;
        aBlockInfo->iSize = 0x8000;
    }

    return UEZ_ERROR_NONE;
}

void Flash_NXP_LPC17xx_40xx_Create(const char *aName)
{
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&Flash_NXP_LPC17xx_40xx_Interface, 0, 0);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Flash Flash_NXP_LPC17xx_40xx_Interface = { {
// Common device interface
    "Flash:NXP:LPC17xx_40xx:16Bit",
    0x0100, //<<---- What is the 0x100 stand for??
    Flash_NXP_LPC17xx_40xx_InitializeWorkspace,
    sizeof(T_Flash_NXP_LPC17xx_40xx_Workspace), },

// Functions
    Flash_NXP_LPC17xx_40xx_Open,
    Flash_NXP_LPC17xx_40xx_Close,
    Flash_NXP_LPC17xx_40xx_Read,
    Flash_NXP_LPC17xx_40xx_Write,
    Flash_NXP_LPC17xx_40xx_BlockErase,
    Flash_NXP_LPC17xx_40xx_ChipErase,
    Flash_NXP_LPC17xx_40xx_QueryReg,
    Flash_NXP_LPC17xx_40xx_GetChipInfo,
    Flash_NXP_LPC17xx_40xx_GetBlockInfo, };

/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_IAP.c
 *-------------------------------------------------------------------------*/

