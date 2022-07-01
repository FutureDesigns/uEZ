/*-------------------------------------------------------------------------*
 * File:  Flash_NXP_LPC_SPIFI_M4_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Flash for NXP LPC SPIFI M4 devices
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZMemory.h>
#include <uEZProcessor.h>
#include "Source/Devices/Flash/NXP/LPC_SPIFI_M4/spifi_rom_api.h"
#include "Source/Devices/Flash/NXP/LPC_SPIFI_M4/Flash_NXP_LPC_SPIFI_M4.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
extern SPIFI_RTNS spifi_table;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Flash *iDevice;
    T_uezSemaphore iSem;
    SPIFIobj iObj;
    SPIFI_RTNS * p_iSpifi;
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
        case 0:
            // Success
            return UEZ_ERROR_NONE;
        case 0x00002:
            // Internal error in API code.
            return UEZ_ERROR_INTERNAL_ERROR;
        case 0x00003:
            // Time-out waiting for program or erase to begin: protection could
            // not be removed.
            return UEZ_ERROR_TIMEOUT;
        case 0x00004:
            // The dest and/or length operands were out of range.
            return UEZ_ERROR_OUT_OF_RANGE;
        case 0x00005:
            // Device status error
            return UEZ_ERROR_DEVICE_STATUS;
        case 0x20007:
            // Unknown device ID code
            return UEZ_ERROR_UNKNOWN_DEVICE;
        case 0x0000B:
            // S_CALLER_ERASE is included in options, and erasure is required.
            return UEZ_ERROR_ERASE_REQUIRED;
        default:
            // Other non-zero values can occur if options selects verification. 
            // They will be the address in the SPIFI memory area at which the 
            // first discrepancy was found.
            return UEZ_ERROR_VERIFY_FAILED;
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
    T_uezError error = UEZ_ERROR_NONE;

    // Make sure its not in a programming mode.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // The memory is now accessible, copy the data over
    memcpy(aBuffer, (void *)(((volatile TUInt8 *)p->iObj.base)+aOffset), aNumBytes);

    UEZSemaphoreRelease(p->iSem);

    return error;
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
    SPIFIopers opers;
    TUInt32 spifiResult = 0;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    
    opers.dest = (char *)(p->iObj.base + aOffset);
    opers.length = aNumBytes;
    opers.scratch = NULL;
    opers.protect = 0;
    opers.options = S_CALLER_ERASE;

    /* Write */
    spifiResult = p->p_iSpifi->spifi_program (&p->iObj, (char *)aBuffer, &opers);
    
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
    SPIFIopers opers;
    TUInt32 spifiResult = 0;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    opers.dest = (char *)(p->iObj.base + aOffset);
    opers.length = aNumBytes;
    opers.scratch = NULL;
    opers.options = S_VERIFY_ERASE;
    
    /* Erase Device */
    spifiResult = p->p_iSpifi->spifi_erase(&p->iObj, &opers);
       
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
    SPIFIopers opers;
    TUInt32 spifiResult = 0;
    
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    opers.dest = (char *)(p->iObj.base);
    opers.length = p->iObj.memSize;
    opers.scratch = NULL;
    opers.options = S_VERIFY_ERASE;
    
    /* Erase Device */
    spifiResult = p->p_iSpifi->spifi_erase(&p->iObj, &opers);
       
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
    
    aInfo->iNumBytesLow = p->iObj.memSize;
    
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
    aBlockInfo->iSize = p->iObj.memSize;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  I_LPC4088_SPIFI_IO_CLK_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn ON SPIFI and configure pins for 4088
 *---------------------------------------------------------------------------*/
void I_LPC4088_SPIFI_IO_CLK_Init()
{
    LPC_SC->PCONP |= 0x00010000;

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
void Flash_NXP_LPC_SPIFI_M4_Configure(
        void *aWorkspace)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;
    TUInt32 spifiResult = 0;
    
    p->p_iSpifi = &spifi_table;
    
    I_LPC4088_SPIFI_IO_CLK_Init();
    
    spifiResult = p->p_iSpifi->spifi_init(&p->iObj, 4, S_RCVCLK | S_FULLCLK, 60);
    
    // Ensure power is on to the part
    LPC17xx_40xxPowerOn(1UL << 16);
    
    switch(spifiResult) {
        case 0:
            // Success
            break;
        case 0x00004:
            // S_MODE3 + S_FULLCLK + S_RCVCLK selected in options
            UEZFailureMsg("SPIFI Init Failed: bad options");
        case 0x00005:
            // Device status error
            UEZFailureMsg("SPIFI Init Failed: Device status error");
        case 0x00006:
            // Unknown extended device ID value
            UEZFailureMsg("SPIFI Init Failed: Unknown extended device ID value");
        case 0x20007:
            // Unknown device ID code
            UEZFailureMsg("SPIFI Init Failed: Unknown device ID code");
        case 0x20008:
            // Unknown device type
            UEZFailureMsg("SPIFI Init Failed: Unknown device type");
        case 0x20009:
            // Unknown manufacturer code
            UEZFailureMsg("SPIFI Init Failed: Unknown manufacturer code");
        case 0x2000A:
            // No operative serial flash (JEDEC ID all zeroes or all ones)
            UEZFailureMsg("SPIFI Init Failed: No operative serial flash");
        default:
            UEZFailureMsg("SPIFI Init Failed: Unknown");
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

void Flash_NXP_LPC_SPIFI_M4_Test(void *aWorkspace)
{
    T_Flash_NXP_LPC_SPIFI_M4_Workspace *p = (T_Flash_NXP_LPC_SPIFI_M4_Workspace *)aWorkspace;
    unsigned char ProgramData[PROG_SIZE];
    SPIFIopers opers;
    TUInt32 i;
    
    printf("OK\r\nErasing QSPI device...");
    /* Erase Entire SPIFI Device if required */
    for ( i = 0 ; i < p->iObj.memSize / 4; i+=4 )
    {
        if ( *((uint32_t *)(p->iObj.base+i)) != 0xFFFFFFFF )
        {
            opers.dest = (char *)(p->iObj.base);
            opers.length = p->iObj.memSize;
            opers.scratch = NULL;
            opers.options = S_VERIFY_ERASE;
            /* Erase Device */
            if (p->p_iSpifi->spifi_erase(&p->iObj, &opers)) while (1);
            break;
        }
    }
  
    printf("OK\r\nProgramming + verifying QSPI device...");
    for(i=0;i<PROG_SIZE;i++)
        ProgramData[i] = i%256;

    opers.length = PROG_SIZE;
    opers.scratch = NULL;
    opers.protect = 0;
    opers.options = S_CALLER_ERASE;
    for ( i = 0 ; i < p->iObj.memSize / PROG_SIZE; i++ )
    {
        /* Write */
        opers.dest = (char *)( p->iObj.base + (i*PROG_SIZE) );
        if (p->p_iSpifi->spifi_program (&p->iObj, (char *)ProgramData, &opers)) while (1);
        /* Verify */
        if (memcmp((void *)opers.dest,(void *)ProgramData,PROG_SIZE)) while (1);
    }
    printf("OK!\r\n");
}

/*-------------------------------------------------------------------------*
 * End of File:  Flash_NXP_LPC_SPIFI_M4_driver.c
 *-------------------------------------------------------------------------*/
