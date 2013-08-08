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

#include <uEZ.h>
#include <uEZFlash.h>
#include <string.h>
#include "Flash_S29GL064N90.h"

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

#define CHUNK_ADDR_FROM_WORD_ADDR(word_addr)        ((word_addr) & 0xFFFFFFE0)
#define CMD16_WRITE_BUFFER_PRG_ADDR1                0x00000555
#define CMD16_WRITE_BUFFER_PRG_DATA1                0x00AA
#define CMD16_WRITE_BUFFER_PRG_ADDR2                0x000002AA
#define CMD16_WRITE_BUFFER_PRG_DATA2                0x0055
#define CMD16_WRITE_BUFFER_PRG_ADDR3(chunk_addr)    (chunk_addr)
#define CMD16_WRITE_BUFFER_PRG_DATA3                0x0025
#define CMD16_WRITE_BUFFER_PRG_ADDR4(chunk_addr)    (chunk_addr)
#define CMD16_WRITE_BUFFER_PRG_DATA4(num_writes)    ((num_writes)-1)
#define CMD16_WRITE_BUFFER_PRG_ADDR5(prg_addr)      (prg_addr)
#define CMD16_WRITE_BUFFER_PRG_DATA5(word)          (word)

#define CHUNK_SIZE          64 // bytes


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

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static T_uezError IEnsureChipInfo(T_Flash_S29GL064N90_Workspace *aWorkspace);

/*---------------------------------------------------------------------------*
 * Routine:  Flash_S29GL064N90_16Bit_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for I2C.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_S29GL064N90_16Bit_InitializeWorkspace(T_Flash_S29GL064N90_Workspace *aW)
{
    T_uezError error = UEZ_ERROR_NONE;

    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    p->iBaseAddr = (TUInt16 volatile *)0x80000000;
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
    volatile int i;
    if (p->iIsProgramMode) {
        // If we are in programming mode, exit it
        p->iBaseAddr[CMD16_READ_RESET_ADDR1] = CMD16_READ_RESET_DATA1;
        p->iBaseAddr[CMD16_READ_RESET_ADDR2] = CMD16_READ_RESET_DATA2;
        p->iBaseAddr[CMD16_READ_RESET_ADDR3] = CMD16_READ_RESET_DATA3;
//        UEZTaskDelay(1);
        for (i=0; i<1000000; i++)
            {}
        p->iIsProgramMode = EFalse;
    }
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
T_uezError Flash_S29GL064N90_16Bit_Open(T_Flash_S29GL064N90_Workspace *aWorkspace)
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
T_uezError Flash_S29GL064N90_16Bit_Close(T_Flash_S29GL064N90_Workspace *aWorkspace)
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
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IExitProgramMode(p);

    // The memory is now accessible, copy the data over
#if 1 // Simple copy
    memcpy(aBuffer, (void *)(((volatile TUInt8 *)p->iBaseAddr)+aOffset), aNumBytes);
#else
    // word by word copy.
    // Each word is stored as Low/High (Little endian format)
    // If we are reading bytes,
    if (aNumBytes) {
        // And we have an odd address, do a read of the
        // extra byte and copy over just that single byte.
        if (aOffset & 1) {
            *(aBuffer++) = (p->iBaseAddr[aOffset>>1] >> 8);
            aNumBytes--;
            aOffset++;
        }
    }
    // Copy over in 16-bit pieces
    aOffset >>= 1;
    while (aNumBytes>1) {
        ((TUInt16 *)aBuffer) = p->iBaseAddr[aOffset++];
        aBuffer += 2;
        aNumBytes-=2;
    }
    // Check for a last spare byte
    if (aNumBytes) {
        *aBuffer = p->iBaseAddr[aOffset] & 0xFF;
    }
#endif

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
 *      T_uezError                  -- Error code.  Returns
 *                                      UEZ_ERROR_INTERNAL_ERROR if error.
 *---------------------------------------------------------------------------*/
static T_uezError IWaitStatusDoneOrError(
        T_Flash_S29GL064N90_Workspace *p,
        TUInt32 aWordOffset)
{
    TUInt16 v1, v2;
    TUInt16 v1b, v2b;

    do {
        v1 = p->iBaseAddr[aWordOffset];
        v1b = v1 & 0x0040;
        v2 = p->iBaseAddr[aWordOffset];
        v2b = v2 & 0x0040;
        // If bit changed, we are getting a status
        if (v1b != v2b) {
            // If both reads reported an error, we got an error.
            if ((v1 & 0x0020) && (v2 & 0x0020)) {
// DQ5 is solid, are we still toggling?
                v1 = p->iBaseAddr[aWordOffset];
                v1b = v1 & 0x0040;
                v2 = p->iBaseAddr[aWordOffset];
                v2b = v2 & 0x0040;
                if (v1b != v2b) {
                    // If both reads reported an error, we got an error.
                    if ((v1 & 0x0020) && (v2 & 0x0020)) {
                        return UEZ_ERROR_INTERNAL_ERROR;
                    }
                }
            }
        }
    } while (v1b != v2b);

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
        T_Flash_S29GL064N90_Workspace *aWorkspace,
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
        TUInt32 offsetWord = aOffset>>1;
        TUInt32 numWords = aNumBytes>>1;

        // Let's do it
        while (numWords) {
            // Program the word
            p->iBaseAddr[CMD16_PRG_WORD_ADDR1] = CMD16_PRG_WORD_DATA1;
            p->iBaseAddr[CMD16_PRG_WORD_ADDR2] = CMD16_PRG_WORD_DATA2;
            p->iBaseAddr[CMD16_PRG_WORD_ADDR3] = CMD16_PRG_WORD_DATA3;
//            p->iBaseAddr[offsetWord] = *((TUInt16 *)aBuffer);
            p->iBaseAddr[offsetWord] = (((TUInt16)aBuffer[0])<<0) | (((TUInt16)aBuffer[1])<<8);

            // Wait for it to complete (or error status)
            error = IWaitStatusDoneOrError(p, offsetWord);
            if (error)
                break;

            aBuffer += 2;
            offsetWord++;
            numWords--;
        }
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
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aOffset,
        TUInt32 aNumBytes)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    TUInt32 blockAddr;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 blockSize;

    IEnsureChipInfo(aWorkspace);

    // TBD: Still assuming all blocks are same size!
    blockAddr = (aOffset & ~(p->iChipInfo.iRegions[0].iSizeEraseBlock-1))>>1;

    IExitProgramMode(p);

    // Remember that we are in program mode
    p->iIsProgramMode = ETrue;

    // Erase each block in this range
    while (aNumBytes) {
        // Erase block cmd
        p->iBaseAddr[CMD16_BLOCK_ERASE_ADDR1] = CMD16_BLOCK_ERASE_DATA1;
        p->iBaseAddr[CMD16_BLOCK_ERASE_ADDR2] = CMD16_BLOCK_ERASE_DATA2;
        p->iBaseAddr[CMD16_BLOCK_ERASE_ADDR3] = CMD16_BLOCK_ERASE_DATA3;
        p->iBaseAddr[CMD16_BLOCK_ERASE_ADDR4] = CMD16_BLOCK_ERASE_DATA4;
        p->iBaseAddr[CMD16_BLOCK_ERASE_ADDR5] = CMD16_BLOCK_ERASE_DATA5;
        p->iBaseAddr[CMD16_BLOCK_ERASE_ADDR6(blockAddr)] =
            CMD16_BLOCK_ERASE_DATA6;

        // Wait for it to finish (or error)
        error = IWaitStatusDoneOrError(p, blockAddr);
        if (error)
            break;

        // Setup for next block (if any)
        // TBD: Right now, we only support region 0!
        blockSize = p->iChipInfo.iRegions[0].iSizeEraseBlock;
        blockAddr += (blockSize>>1);
        if (aNumBytes > blockSize)
            aNumBytes -= blockSize;
        else
            aNumBytes = 0;
    }

    // Go ahead and force out of program mode
    IExitProgramMode(p);

    // Done erasing, resume normal control

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
T_uezError Flash_S29GL064N90_16Bit_ChipErase(T_Flash_S29GL064N90_Workspace *aWorkspace)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Let's do it

    // Remember that we are in program mode
    p->iIsProgramMode = ETrue;

    // Start the erasing list
    p->iBaseAddr[CMD16_CHIP_ERASE_ADDR1] = CMD16_CHIP_ERASE_DATA1;
    p->iBaseAddr[CMD16_CHIP_ERASE_ADDR2] = CMD16_CHIP_ERASE_DATA2;
    p->iBaseAddr[CMD16_CHIP_ERASE_ADDR3] = CMD16_CHIP_ERASE_DATA3;
    p->iBaseAddr[CMD16_CHIP_ERASE_ADDR4] = CMD16_CHIP_ERASE_DATA4;
    p->iBaseAddr[CMD16_CHIP_ERASE_ADDR5] = CMD16_CHIP_ERASE_DATA5;
    p->iBaseAddr[CMD16_CHIP_ERASE_ADDR6] = CMD16_CHIP_ERASE_DATA6;

    // Wait for it to finish (or error)
    error = IWaitStatusDoneOrError(p, 0);

    // Go ahead and force out of program mode
    IExitProgramMode(p);

    // Done erasing, resume normal control

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
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aReg,
        TUInt32 *aValue)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Let's do it

    // Put into Query mode
    p->iBaseAddr[CMD16_QUERY_ADDR1] = CMD16_QUERY_DATA1;
    *aValue = (TUInt32) (p->iBaseAddr[aReg]);

    // Note we are in program/query mode (and need to exit first)
    p->iIsProgramMode = ETrue;

    // Done erasing, resume normal control

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
static T_uezError IEnsureChipInfo(T_Flash_S29GL064N90_Workspace *aWorkspace)
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
        T_Flash_S29GL064N90_Workspace *aWorkspace,
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
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        TUInt32 aOffset,
        T_FlashBlockInfo *aBlockInfo)
{
    TUInt32 blockAddr;
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;

    IEnsureChipInfo(aWorkspace);

    // TBD: Still assuming all blocks are same size!
    blockAddr = (aOffset & ~(p->iChipInfo.iRegions[0].iSizeEraseBlock-1))>>1;

    aBlockInfo->iOffset = blockAddr<<1;
    // TBD: Right now we only support region 0!
    aBlockInfo->iSize = p->iChipInfo.iRegions[0].iSizeEraseBlock;

    return UEZ_ERROR_NONE;
}


#if 0
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
        T_Flash_S29GL064N90_Workspace *aWorkspace,
        void *aBaseAddr)
{
    T_Flash_S29GL064N90_Workspace *p = (T_Flash_S29GL064N90_Workspace *)aWorkspace;

    p->iBaseAddr = aBaseAddr;
}
#endif

/*-------------------------------------------------------------------------*
 * End of File:  Flash_S29GL064N90_16Bit_driver.c
 *-------------------------------------------------------------------------*/
