/*-------------------------------------------------------------------------*
 * File:  Flash_S29GL064N90_16Bit_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Flash for Spansion S29GL064N90
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

#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZMemory.h>
#include "Flash_S29GL064N90_16bit.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
// CFI style commands
//#define BLOCK_SIZE          (128*1024) // bytes

#define CMD16_READ_RESET_ADDR1          0x00000555
#define CMD16_READ_RESET_DATA1          0x00AA
#define CMD16_READ_RESET_ADDR2          0x000002AA
#define CMD16_READ_RESET_DATA2          0x0055
#define CMD16_READ_RESET_ADDR3          0x00000000
#define CMD16_READ_RESET_DATA3          0x00F0

#define PAGE_ADDR_FROM_WORD_ADDR(word_addr)         ((word_addr) & 0xFFFFFFF0)
#define CMD16_WRITE_BUFFER_PRG_ADDR1                0x00000555
#define CMD16_WRITE_BUFFER_PRG_DATA1                0x00AA
#define CMD16_WRITE_BUFFER_PRG_ADDR2                0x000002AA
#define CMD16_WRITE_BUFFER_PRG_DATA2                0x0055
#define CMD16_WRITE_BUFFER_PRG_ADDR3(sector_addr)   (sector_addr)
#define CMD16_WRITE_BUFFER_PRG_DATA3                0x0025
#define CMD16_WRITE_BUFFER_PRG_ADDR4(sector_addr)   (sector_addr)
#define CMD16_WRITE_BUFFER_PRG_DATA4(num_writes)    ((num_writes)-1)
#define CMD16_WRITE_BUFFER_PRG_ADDR5(prg_addr)      (prg_addr)
#define CMD16_WRITE_BUFFER_PRG_DATA5(word)          (word)

#define CMD16_WRITE_BUFFER_TO_FLASH_ADDR(sector_addr)   (sector_addr)
#define CMD16_WRITE_BUFFER_TO_FLASH_DATA                0x0029

#define CMD16_BLOCK_ERASE_ADDR1                     0x00000555
#define CMD16_BLOCK_ERASE_DATA1                     0x00AA
#define CMD16_BLOCK_ERASE_ADDR2                     0x000002AA
#define CMD16_BLOCK_ERASE_DATA2                     0x0055
#define CMD16_BLOCK_ERASE_ADDR3                     0x00000555
#define CMD16_BLOCK_ERASE_DATA3                     0x0080
#define CMD16_BLOCK_ERASE_ADDR4                     0x00000555
#define CMD16_BLOCK_ERASE_DATA4                     0x00AA
#define CMD16_BLOCK_ERASE_ADDR5                     0x000002AA
#define CMD16_BLOCK_ERASE_DATA5                     0x0055
#define CMD16_BLOCK_ERASE_ADDR6(block_addr)         (block_addr)
#define CMD16_BLOCK_ERASE_DATA6                     0x0030

#define CMD16_CHIP_ERASE_ADDR1                     0x00000555
#define CMD16_CHIP_ERASE_DATA1                     0x00AA
#define CMD16_CHIP_ERASE_ADDR2                     0x000002AA
#define CMD16_CHIP_ERASE_DATA2                     0x0055
#define CMD16_CHIP_ERASE_ADDR3                     0x00000555
#define CMD16_CHIP_ERASE_DATA3                     0x0080
#define CMD16_CHIP_ERASE_ADDR4                     0x00000555
#define CMD16_CHIP_ERASE_DATA4                     0x00AA
#define CMD16_CHIP_ERASE_ADDR5                     0x000002AA
#define CMD16_CHIP_ERASE_DATA5                     0x0055
#define CMD16_CHIP_ERASE_ADDR6                     0x00000555
#define CMD16_CHIP_ERASE_DATA6                     0x0010

#define CMD16_QUERY_ADDR1                      0x00000055
#define CMD16_QUERY_DATA1                      0x0098

#define CMD16_PRG_WORD_ADDR1                     0x00000555
#define CMD16_PRG_WORD_DATA1                     0x00AA
#define CMD16_PRG_WORD_ADDR2                     0x000002AA
#define CMD16_PRG_WORD_DATA2                     0x0055
#define CMD16_PRG_WORD_ADDR3                     0x00000555
#define CMD16_PRG_WORD_DATA3                     0x00A0
#define CMD16_PRG_WORD_ADDR4(word_addr)          (word_addr)
#define CMD16_PRG_WORD_DATA4(word)               (word)

#define CMD16_WRITE_BUFFER_ABORT_RESET_ADDR1        0x00000555
#define CMD16_WRITE_BUFFER_ABORT_RESET_DATA1        0x00AA
#define CMD16_WRITE_BUFFER_ABORT_RESET_ADDR2        0x000002AA
#define CMD16_WRITE_BUFFER_ABORT_RESET_DATA2        0x0055
#define CMD16_WRITE_BUFFER_ABORT_RESET_ADDR3        0x00000555
#define CMD16_WRITE_BUFFER_ABORT_RESET_DATA3        0x00F0

/*---------------------------------------------------------------------------*
 * CFI Registers (16-bit based):
 *---------------------------------------------------------------------------*/
#define CMD16_REG_Q                                 0x0010
#define CMD16_REG_R                                 0x0011
#define CMD16_REG_Y                                 0x0012
#define CMD16_REG_DEVICE_SIZE                       0x0027
#define CMD16_REG_BIT_WIDTH                         0x0028
#define CMD16_REG_NUM_REGIONS                       0x002C
#define CMD16_REG_REGION_NUM_ERASE_BLOCKS_LOW(x)    (0x002D + (4*(x)))
#define CMD16_REG_REGION_NUM_ERASE_BLOCKS_HIGH(x)   (0x002E + (4*(x)))
#define CMD16_REG_REGION_BLOCK_SIZE_LOW(x)          (0x002F + (4*(x)))
#define CMD16_REG_REGION_BLOCK_SIZE_HIGH(x)         (0x0030 + (4*(x)))

#define DQ0                                         ((TUInt16)(1<<0))
#define DQ1                                         ((TUInt16)(1<<1))
#define DQ2                                         ((TUInt16)(1<<2))
#define DQ5                                         ((TUInt16)(1<<5))
#define DQ6                                         ((TUInt16)(1<<6))
#define DQ7                                         ((TUInt16)(1<<7))

#define IFRead(addr)            UEZMemRead16((TUInt32)(p->iBaseAddr+(addr)))
#define IFWrite(addr, value)    UEZMemWrite16((TUInt32)(p->iBaseAddr+(addr)), value)
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Flash *iDevice;
    T_uezSemaphore iSem;
    volatile TUInt16 *iBaseAddr;
    TUInt32 iNumOpen;
    TBool iIsProgramMode;
    TBool iIsChipInfoReady;
    T_FlashChipInfo iChipInfo;
} T_Flash_S29GL064N90_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static T_uezError IEnsureChipInfo(void *aWorkspace);
T_uezError Flash_S29GL064N90_16Bit_GetBlockInfo(
        void *aWorkspace,
        TUInt32 aOffset,
        T_FlashBlockInfo *aBlockInfo);

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for Flash.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_S29GL064N90_16Bit_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iBaseAddr = 0;
    p->iNumOpen = 0;
    p->iIsProgramMode = EFalse;
    p->iIsChipInfoReady = EFalse;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IExitProgramMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit programming mode
 * Inputs:
 *      T_Flash_S29GL064N90_Workspace *p -- Workspace
 *---------------------------------------------------------------------------*/
static void IExitProgramMode(T_Flash_S29GL064N90_Workspace *p)
{
    if (p->iIsProgramMode) {
        // If we are in programming mode, exit it
        IFWrite(CMD16_READ_RESET_ADDR1, CMD16_READ_RESET_DATA1);
        IFWrite(CMD16_READ_RESET_ADDR2, CMD16_READ_RESET_DATA2);
        IFWrite(CMD16_READ_RESET_ADDR3, CMD16_READ_RESET_DATA3);
        UEZTaskDelay(1);
        p->iIsProgramMode = EFalse;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  IWriteBufferAbortReset
 *---------------------------------------------------------------------------*
 * Description:
 *      Something failed in the write buffer command.  Abort and reset.
 * Inputs:
 *      T_Flash_S29GL064N90_Workspace *p -- Workspace
 *---------------------------------------------------------------------------*/
static void IWriteBufferAbortReset(T_Flash_S29GL064N90_Workspace *p)
{
    // If we are in programming mode, exit it
    IFWrite(CMD16_WRITE_BUFFER_ABORT_RESET_ADDR1, CMD16_WRITE_BUFFER_ABORT_RESET_DATA1);
    IFWrite(CMD16_WRITE_BUFFER_ABORT_RESET_ADDR2, CMD16_WRITE_BUFFER_ABORT_RESET_DATA2);
    IFWrite(CMD16_WRITE_BUFFER_ABORT_RESET_ADDR3, CMD16_WRITE_BUFFER_ABORT_RESET_DATA3);
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the flash device.  For the S29GL064N90, nothing needs to happen.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_S29GL064N90_16Bit_Open(void *aWorkspace)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;

    p->iNumOpen++;
    if (p->iNumOpen == 1) {
        // If the first time, assume we were in some programming mode
        // and need to reset
        p->iIsProgramMode = ETrue;
        IExitProgramMode(p);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the flash device.  For the S29GL064N90, nothing needs to happen.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_S29GL064N90_16Bit_Close(void *aWorkspace)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    if (p->iNumOpen) {
        p->iNumOpen--;
        if (p->iNumOpen == 0)
            IExitProgramMode(p);
        return UEZ_ERROR_NONE;
    } else {
        return UEZ_ERROR_NOT_OPEN;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_Read
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
T_uezError Flash_S29GL064N90_16Bit_Read(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Make sure its not in a programming mode.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    IExitProgramMode(p);

    // The memory is now accessible, copy the data over
#if 0 // Simple copy
    memcpy(aBuffer, (void *)(((volatile TUInt8 *)p->iBaseAddr)+aOffset), aNumBytes);
#else
    // word by word copy.
    // Each word is stored as Low/High (Little endian format)
    // If we are reading bytes,
    if (aNumBytes) {
        // And we have an odd address, do a read of the
        // extra byte and copy over just that single byte.
        if (aOffset & 1) {
            *(aBuffer++) = (IFRead(aOffset>>1) >> 8);
            aNumBytes--;
            aOffset++;
        }
    }
    // Copy over in 16-bit pieces
    aOffset >>= 1;
    while (aNumBytes >= 2) {
        *((TUInt16 *)aBuffer) = IFRead(aOffset);
        aBuffer += 2;
        aNumBytes-=2;
        aOffset++;
    }
    // Check for a last spare byte
    if (aNumBytes) {
        *aBuffer = IFRead(aOffset) & 0xFF;
    }
#endif

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IWaitStatusDoneOrError
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for the status to show that the there is an error OR that
 *      the operation has completed.
 * Inputs:
 *      T_Flash_S29GL064N90_Workspace *p -- Workspace
 *      TUInt32 aWordOffset         -- Offset of last write/program/etc.
 * Outputs:
 *      T_uezError                  -- Error code.  Returns UEZ_ERROR_ABORTED
 *                                      if a write is aborted.  Returns
 *                                      UEZ_ERROR_INTERNAL_ERROR if error.
 *---------------------------------------------------------------------------*/
static T_uezError IWaitStatusDoneOrError(
        T_Flash_S29GL064N90_Workspace *p,
        TUInt32 aWordOffset)
{
    TUInt16 v1, v2, v3;
    TUInt16 dq2_toggles;
    TUInt16 dq6_toggles;
    TUInt16 toggling;
    TBool retry = ETrue;

    do {
        v1 = IFRead(aWordOffset);
        v2 = IFRead(aWordOffset);
        v3 = IFRead(aWordOffset);
        // What bits are toggling?
        toggling = ((v1 ^ v2) & (v2 ^ v3));
        // DQ2 toggle check
        dq2_toggles = toggling & DQ2; 
        // DQ6 toggle check
        dq6_toggles = toggling & DQ6;

        // Is DQ6 toggling?
        if (dq6_toggles) {
            // We are toggling DQ6, check for toggling DQ2
            if (!dq2_toggles) {
                // Check for Write Buffer Abort on DQ1
                if (v1 & (1<<1)) {
                    // DQ1 is set!
                    return UEZ_ERROR_ABORTED;
                }
            }
            // Is DQ5 set to high?
            if ((v2 & DQ5) && (v3 & DQ5)) {
                // DQ5 is solid
                if(retry){
                    retry = EFalse;
                } else {
                    return UEZ_ERROR_INTERNAL_ERROR;
                }
            }
        }
    } while (dq6_toggles);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_Write
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
T_uezError Flash_S29GL064N90_16Bit_Write(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    if ((aOffset & 1) || (aNumBytes & 1)) {
        // Can only write on proper word boundaries
        error = UEZ_ERROR_BAD_ALIGNMENT;
    } else {
#if 1 // Fast page writes
        // Good boundary and size, use 16-bit word sizes
        TUInt32 numWords = aNumBytes>>1;
        TUInt32 offsetWord = aOffset>>1;
        TUInt32 pageAddr;
        T_FlashBlockInfo block;
        //TUInt32 sectorAddr;
        TUInt32 numWordsInPage;
        TUInt32 i;
        TUInt32 count;
        TUInt32 lastAddr;

        error = IEnsureChipInfo(aWorkspace);
        if (error)
            return error;

        // At this point, to keep from confusing between from
        // 128k blocks that are erased and 32 byte blocks that
        // can be programmed, we'll call the smaller 32 byte blocks 'pages'

        // Determine the page address of first page
        pageAddr = aOffset>>1;

        // Let's do it
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

        // Say we're still in programming mode to ensure a
        // reset occurs later if they read the rom.
        p->iIsProgramMode = ETrue;

        // Process in page sizes (16 words, or 32 bytes)
        while (numWords) {
            // How many words in this page
            count = numWords;

            // Get information about this block (where the base sector address is)
            Flash_S29GL064N90_16Bit_GetBlockInfo(aWorkspace, aOffset, &block);
            // Convert to word address
            //sectorAddr = block.iOffset >> 1;

            if (count > 16)
                count = 16;

            // How many words in this page based on where we are going (up to one page)
            if (PAGE_ADDR_FROM_WORD_ADDR(offsetWord+count) != pageAddr) {
                // Do a partial page
                numWordsInPage = 16-(offsetWord&15);
            } else {
                numWordsInPage = 16;
            }
            if (count > numWordsInPage)
                count = numWordsInPage;

            // Setup for programming that page
            IFWrite(CMD16_WRITE_BUFFER_PRG_ADDR1, CMD16_WRITE_BUFFER_PRG_DATA1);
            IFWrite(CMD16_WRITE_BUFFER_PRG_ADDR2, CMD16_WRITE_BUFFER_PRG_DATA2);
            IFWrite(CMD16_WRITE_BUFFER_PRG_ADDR3(pageAddr), CMD16_WRITE_BUFFER_PRG_DATA3);
            IFWrite(CMD16_WRITE_BUFFER_PRG_ADDR4(pageAddr), CMD16_WRITE_BUFFER_PRG_DATA4(count));

            // Write out the buffer
            for (i=0; i<count; i++) {
                lastAddr = pageAddr+i;
                IFWrite(CMD16_WRITE_BUFFER_PRG_ADDR5(pageAddr+i),
                    CMD16_WRITE_BUFFER_PRG_DATA5(((TUInt16 *)aBuffer)[i]));
            }

            // Now send the program buffer command
            IFWrite(CMD16_WRITE_BUFFER_TO_FLASH_ADDR(lastAddr), 
                CMD16_WRITE_BUFFER_TO_FLASH_DATA);

            // Wait for it to complete (or error status)
            error = IWaitStatusDoneOrError(p, lastAddr);
            if (error) {
                if (error == UEZ_ERROR_ABORTED) {
                    IWriteBufferAbortReset(p);
                    IExitProgramMode(p);
                }
                break;
            }

            // Done with that one, go to the next one
            offsetWord += count;
            numWords -= count;
            pageAddr += count;
            aBuffer += count * 2;
        }

        UEZSemaphoreRelease(p->iSem);
#else
        TUInt32 offsetWord = aOffset>>1;
        TUInt32 numWords = aNumBytes>>1;

        // Let's do it
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
        while (numWords) {
            // Program the word
            IFWrite(CMD16_PRG_WORD_ADDR1, CMD16_PRG_WORD_DATA1);
            IFWrite(CMD16_PRG_WORD_ADDR2, CMD16_PRG_WORD_DATA2);
            IFWrite(CMD16_PRG_WORD_ADDR3, CMD16_PRG_WORD_DATA3);
            IFWrite(offsetWord, (((TUInt16)aBuffer[0])<<0) | (((TUInt16)aBuffer[1])<<8));

            // Wait for it to complete (or error status)
            error = IWaitStatusDoneOrError(p, offsetWord);
            if (error)
                break;

            aBuffer += 2;
            offsetWord++;
            numWords--;
        }
        UEZSemaphoreRelease(p->iSem);
#endif
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_BlockErase
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
T_uezError Flash_S29GL064N90_16Bit_BlockErase(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt32 aNumBytes)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    TUInt32 blockAddr;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 blockSize;
    TUInt32 extraBytes;

    IEnsureChipInfo(aWorkspace);
    // WE are now in program mode

    // Let's do it
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // TODO: Still assuming all blocks are same size!
    blockAddr = (aOffset & ~(p->iChipInfo.iRegions[0].iSizeEraseBlock-1))>>1;
    extraBytes = aOffset - (blockAddr<<1);
    aNumBytes += extraBytes;

    // Erase each block in this range
    while (aNumBytes) {
        // TBD: Still assuming all blocks are same size!
        blockAddr = (aOffset & ~(p->iChipInfo.iRegions[0].iSizeEraseBlock-1))>>1;

        // Erase block cmd
        IFWrite(CMD16_BLOCK_ERASE_ADDR1, CMD16_BLOCK_ERASE_DATA1);
        IFWrite(CMD16_BLOCK_ERASE_ADDR2, CMD16_BLOCK_ERASE_DATA2);
        IFWrite(CMD16_BLOCK_ERASE_ADDR3, CMD16_BLOCK_ERASE_DATA3);
        IFWrite(CMD16_BLOCK_ERASE_ADDR4, CMD16_BLOCK_ERASE_DATA4);
        IFWrite(CMD16_BLOCK_ERASE_ADDR5, CMD16_BLOCK_ERASE_DATA5);
        IFWrite(CMD16_BLOCK_ERASE_ADDR6(blockAddr), CMD16_BLOCK_ERASE_DATA6);

        // Wait for it to finish (or error)
        error = IWaitStatusDoneOrError(p, blockAddr);
        if (error)
            break;

        // Setup for next block (if any)
        // TBD: Right now, we only support region 0!
        blockSize = p->iChipInfo.iRegions[0].iSizeEraseBlock;
        aOffset += blockSize;
        if (aNumBytes > blockSize)
            aNumBytes -= blockSize;
        else
            aNumBytes = 0;
    }

    // Go ahead and force out of program mode
    IExitProgramMode(p);

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_ChipErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the complete chip
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_S29GL064N90_16Bit_ChipErase(void *aWorkspace)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Let's do it
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Remember that we are in program mode
    p->iIsProgramMode = ETrue;

    // Start the erasing list
    IFWrite(CMD16_CHIP_ERASE_ADDR1, CMD16_CHIP_ERASE_DATA1);
    IFWrite(CMD16_CHIP_ERASE_ADDR2, CMD16_CHIP_ERASE_DATA2);
    IFWrite(CMD16_CHIP_ERASE_ADDR3, CMD16_CHIP_ERASE_DATA3);
    IFWrite(CMD16_CHIP_ERASE_ADDR4, CMD16_CHIP_ERASE_DATA4);
    IFWrite(CMD16_CHIP_ERASE_ADDR5, CMD16_CHIP_ERASE_DATA5);
    IFWrite(CMD16_CHIP_ERASE_ADDR6, CMD16_CHIP_ERASE_DATA6);

    // Wait for it to finish (or error)
    error = IWaitStatusDoneOrError(p, 0);

    // Go ahead and force out of program mode
    IExitProgramMode(p);

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_QueryReg
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
T_uezError Flash_S29GL064N90_16Bit_QueryReg(
        void *aWorkspace,
        TUInt32 aReg,
        TUInt32 *aValue)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Let's do it
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Put into Query mode
    IFWrite(CMD16_QUERY_ADDR1, CMD16_QUERY_DATA1);
    *aValue = (TUInt32) (IFRead(aReg));

    // Note we are in program/query mode (and need to exit first)
    p->iIsProgramMode = ETrue;

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IEnsureChipInfo
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
static T_uezError IEnsureChipInfo(void *aWorkspace)
{
    TUInt32 i;
    TUInt32 value;
    TUInt32 v;
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;

    if (!p->iIsChipInfoReady) {
        // See if we get the proper header
        Flash_S29GL064N90_16Bit_QueryReg(aWorkspace, CMD16_REG_Q, &value);
        if (value != (TUInt32)'Q')
            return UEZ_ERROR_NOT_FOUND;
        Flash_S29GL064N90_16Bit_QueryReg(aWorkspace, CMD16_REG_R, &value);
        if (value != (TUInt32)'R')
            return UEZ_ERROR_NOT_FOUND;
        Flash_S29GL064N90_16Bit_QueryReg(aWorkspace, CMD16_REG_Y, &value);
        if (value != (TUInt32)'Y')
            return UEZ_ERROR_NOT_FOUND;

        // Fill in the size and width
        Flash_S29GL064N90_16Bit_QueryReg(aWorkspace, CMD16_REG_DEVICE_SIZE, &value);
        if (value >= 32) {
            p->iChipInfo.iNumBytesLow = 0;
            p->iChipInfo.iNumBytesHigh = (1<<(value-32));
        } else {
            p->iChipInfo.iNumBytesLow = 1<<value;
            p->iChipInfo.iNumBytesHigh = 0;
        }
        Flash_S29GL064N90_16Bit_QueryReg(aWorkspace, CMD16_REG_BIT_WIDTH, &value);
        p->iChipInfo.iBitWidth = 8*value;

        // Fill in the region info
        Flash_S29GL064N90_16Bit_QueryReg(aWorkspace, CMD16_REG_NUM_REGIONS, &value);
        p->iChipInfo.iNumRegions = value;
        for (i=0; i<FLASH_CHIP_MAX_TRACKED_REGIONS; i++) {
            if (i < p->iChipInfo.iNumRegions) {
                // Determine number of blocks
                Flash_S29GL064N90_16Bit_QueryReg(
                        aWorkspace,
                        CMD16_REG_REGION_NUM_ERASE_BLOCKS_LOW(i),
                        &value);
                v = value & 0xFF;
                Flash_S29GL064N90_16Bit_QueryReg(
                        aWorkspace,
                        CMD16_REG_REGION_NUM_ERASE_BLOCKS_HIGH(i),
                        &value);
                v |= ((value<<8)&0xFF00);
                p->iChipInfo.iRegions[i].iNumEraseBlocks = v+1;

                // Determine size of each block (in bytes)
                Flash_S29GL064N90_16Bit_QueryReg(
                        aWorkspace,
                        CMD16_REG_REGION_BLOCK_SIZE_LOW(i),
                        &value);
                v = value & 0xFF;
                Flash_S29GL064N90_16Bit_QueryReg(
                        aWorkspace,
                        CMD16_REG_REGION_BLOCK_SIZE_HIGH(i),
                        &value);
                v |= ((value<<8)&0xFF00);
                p->iChipInfo.iRegions[i].iSizeEraseBlock = v;
                p->iChipInfo.iRegions[i].iSizeEraseBlock *= 256; // put in bytes
            } else {
                // Empty region
                p->iChipInfo.iRegions[i].iNumEraseBlocks = 0;
                p->iChipInfo.iRegions[i].iSizeEraseBlock = 0;
            }
        }
        p->iIsChipInfoReady = ETrue;
    }
    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_GetChipInfo
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
T_uezError Flash_S29GL064N90_16Bit_GetChipInfo(
        void *aWorkspace,
        T_FlashChipInfo *aInfo)
{
    T_uezError error;
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;

    // Ensure it is loaded in memory
    error = IEnsureChipInfo(aWorkspace);
    if (error)
        return error;

    // Copy the information over
    *aInfo = p->iChipInfo;

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_GetBlockInfo
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
T_uezError Flash_S29GL064N90_16Bit_GetBlockInfo(
        void *aWorkspace,
        TUInt32 aOffset,
        T_FlashBlockInfo *aBlockInfo)
{
    TUInt32 blockAddr;
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error;

    error = IEnsureChipInfo(aWorkspace);
    if (error)
        return error;

    // TBD: Still assuming all blocks are same size!
    blockAddr = (aOffset & ~(p->iChipInfo.iRegions[0].iSizeEraseBlock-1))>>1;

    aBlockInfo->iOffset = blockAddr<<1;
    // TBD: Right now we only support region 0!
    aBlockInfo->iSize = p->iChipInfo.iRegions[0].iSizeEraseBlock;

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the flash chip's location
 * Inputs:
 *      void *aW                    -- Workspace
 *      void *aBaseAddr             -- Base address of this chip in
 *                                     processor memory map space
 *---------------------------------------------------------------------------*/
void Flash_S29GL064N90_16Bit_Configure(
        void *aWorkspace,
        void *aBaseAddr)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;

    p->iBaseAddr = (volatile TUInt16 *)aBaseAddr;
}

/*---------------------------------------------------------------------------*
 * Create functions:
 *---------------------------------------------------------------------------*/
void Flash_S29GL064N90_16Bit_Create(const char *aName, TUInt32 aBaseAddr)
{
    T_uezDeviceWorkspace *p_flash0;

    // Setup the NOR flash
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Flash_Spansion_S29GL064N90_16Bit_Interface,
            0,
            &p_flash0);
    Flash_S29GL064N90_16Bit_Configure(p_flash0, (void *)aBaseAddr);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Flash Flash_Spansion_S29GL064N90_16Bit_Interface = {
	{
		// Common device interface
	    "Flash:Spansion:S29GL064N90:16Bit",
	    0x0100,
	    Flash_S29GL064N90_16Bit_InitializeWorkspace,
	    sizeof(T_Flash_S29GL064N90_Workspace),
	},
	
    // Functions
    Flash_S29GL064N90_16Bit_Open,
    Flash_S29GL064N90_16Bit_Close,
    Flash_S29GL064N90_16Bit_Read,
    Flash_S29GL064N90_16Bit_Write,
    Flash_S29GL064N90_16Bit_BlockErase,
    Flash_S29GL064N90_16Bit_ChipErase,
    Flash_S29GL064N90_16Bit_QueryReg,
    Flash_S29GL064N90_16Bit_GetChipInfo,
    Flash_S29GL064N90_16Bit_GetBlockInfo,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  Flash_S29GL064N90_16Bit_driver.c
 *-------------------------------------------------------------------------*/
