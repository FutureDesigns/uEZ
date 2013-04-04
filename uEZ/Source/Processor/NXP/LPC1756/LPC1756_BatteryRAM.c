/*-------------------------------------------------------------------------*
 * File:  LPC1756_BatteryRAM.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1756 Battery RAM Interface.
 * Implementation:
 *      Direct access to Battery RAM address space is sufficient here.
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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_BatteryRAM.h>

/*-------------------------------------------------------------------------*
 * Defines:
 *-------------------------------------------------------------------------*/
#define BATTERY_RAM_ELEMENT_SIZE (4)
#define BATTERY_RAM_BYTE_SIZE (8)

#define BATTERY_RAM_START (0x40024044)

// Last item (General purpose register 3) is used to store checksum of values.
// General purpose register 4 is used to store the validity marker of the RTC.
// This means that there are only 12 useable bytes left over.
#define BATTERY_RAM_END   (0x40024050)
#define BATTERY_RAM_SIZE  (BATTERY_RAM_END - BATTERY_RAM_START)

#define BATTERY_RAM_OFFSET_TO_WORD32(offset) ((offset) / BATTERY_RAM_ELEMENT_SIZE)

// Last four offsets(one item) is used to store checksum of values.
#define BATTERY_RAM_DATA_SIZE (BATTERY_RAM_SIZE - BATTERY_RAM_ELEMENT_SIZE)

// Last element is used to store a magic number.
#define BATTERY_RAM_VALID_ADDRESS(offset) ((offset) < BATTERY_RAM_DATA_SIZE)

// Last item is used to store checksum of values.
#define BATTERY_RAM_MAGIC_CONSTANT   (0xAA55AA55UL)
#define BATTERY_RAM_MAGIC_WORD32  (BATTERY_RAM_OFFSET_TO_WORD32(BATTERY_RAM_DATA_SIZE))

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_BatteryRAM *iHAL;
    volatile TUInt32 *iRAMBatArray;
    TBool iContentValid;
} T_LPC1756_BatteryRAM_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_BatteryRAM_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1756 BatteryRAM workspace.
 * Inputs:
 *      void *aW                    -- Particular BatteryRAM workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_BatteryRAM_InitializeWorkspace(void *aWorkspace)
{

    T_LPC1756_BatteryRAM_Workspace *p =
            (T_LPC1756_BatteryRAM_Workspace *)aWorkspace;

    p->iRAMBatArray = (volatile TUInt32 *)BATTERY_RAM_START;

    // Check for magic constant.
    if ((TUInt32)BATTERY_RAM_MAGIC_CONSTANT
            == p->iRAMBatArray[BATTERY_RAM_MAGIC_WORD32]) {
        p->iContentValid = ETrue;
    } else {
        p->iContentValid = EFalse;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_BatteryRAM_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a singele byte of data from then given offset address.
 * Inputs:
 *      void *aW                     -- BatteryRAM workspace
 *      const TUInt32 aAddr          -- Offset address in bytes into Battery RAM
 *      TUInt8 *aData                -- Data to be read from Battery RAM
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_BatteryRAM_Read(
        void *aWorkspace,
        const TUInt32 aAddr,
        TUInt8 *aData)
{
    TUInt32 ramElement;
    TUInt32 bitOffset;
    T_LPC1756_BatteryRAM_Workspace *p =
            (T_LPC1756_BatteryRAM_Workspace *)aWorkspace;

    if (!BATTERY_RAM_VALID_ADDRESS(aAddr))
        return UEZ_ERROR_ILLEGAL_PARAMETER;

    // Get element from battery ram
    ramElement = p->iRAMBatArray[BATTERY_RAM_OFFSET_TO_WORD32(aAddr)];

    // Compute bit offset in word from offset.
    bitOffset = (aAddr % BATTERY_RAM_ELEMENT_SIZE) * BATTERY_RAM_BYTE_SIZE;

    // Extract byte data from battery ram.
    *aData = (TUInt8)((ramElement >> bitOffset) & 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_BatteryRAM_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a single byte of data to the given offset address.
 * Inputs:
 *      void *aW                     -- BatteryRAM workspace
 *      const TUInt32 aAddr          -- Offset address in Battery RAM
 *      const TUInt8 aData           -- Data to be written to B RAM
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_BatteryRAM_Write(
        void *aWorkspace,
        const TUInt32 aAddr,
        const TUInt8 aData)
{
    TUInt32 ramElement;
    TUInt32 bitOffset;
    T_LPC1756_BatteryRAM_Workspace *p =
            (T_LPC1756_BatteryRAM_Workspace *)aWorkspace;

    // Battery RAM can be accessed word-wise (32-bit) only.
    if (!BATTERY_RAM_VALID_ADDRESS(aAddr))
        return UEZ_ERROR_ILLEGAL_PARAMETER;

    // Get element from battery ram
    ramElement = p->iRAMBatArray[BATTERY_RAM_OFFSET_TO_WORD32(aAddr)];

    // Compute bit offset in word from offset.
    bitOffset = (aAddr % BATTERY_RAM_ELEMENT_SIZE) * BATTERY_RAM_BYTE_SIZE;

    // Set bits to zero on desired pos.
    ramElement &= ~(0xFF << bitOffset);

    // Set bits to new value on desired pos.
    ramElement |= (aData << bitOffset);

    // Store modified value back to battery ram.
    p->iRAMBatArray[BATTERY_RAM_OFFSET_TO_WORD32(aAddr)] = ramElement;

    // Write magic checksum data into battery ram.
    p->iRAMBatArray[BATTERY_RAM_MAGIC_WORD32] = BATTERY_RAM_MAGIC_CONSTANT;

    // Data and checksum was written, modify flag.
    p->iContentValid = ETrue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_BatteryRAM_Valid
 *---------------------------------------------------------------------------*
 * Description:
 *      Return validity of stored data.
 * Inputs:
 *      void *aW                     -- BatteryRAM workspace
 * Outputs:
 *      TBool                       -- Valide true | false
 *---------------------------------------------------------------------------*/
TBool LPC1756_BatteryRAM_IsValid(void *aWorkspace)
{
    T_LPC1756_BatteryRAM_Workspace *p =
            (T_LPC1756_BatteryRAM_Workspace *)aWorkspace;

    return p->iContentValid;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_BatteryRAM_GetSize
 *---------------------------------------------------------------------------*
 * Description:
 *      Return battery RAM's size
 * Inputs:
 *      void *aW                    -- BatteryRAM workspace
 * Outputs:
 *      TUInt32                     -- Size of RAM in bytes
 *---------------------------------------------------------------------------*/
TUInt32 LPC1756_BatteryRAM_GetSize(void *aWorkspace)
{
    // Return full size minus one complete element
    return BATTERY_RAM_SIZE - BATTERY_RAM_ELEMENT_SIZE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_BatteryRAM LPC1756_BatteryRAM_Interface = {
        {
                "LPC1756:BatteryRAM",
                0x0100,
                LPC1756_BatteryRAM_InitializeWorkspace,
                sizeof(T_LPC1756_BatteryRAM_Workspace), },

        LPC1756_BatteryRAM_Read,
        LPC1756_BatteryRAM_Write,
        LPC1756_BatteryRAM_IsValid,
        LPC1756_BatteryRAM_GetSize };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1756_BatteryRAM_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("BatteryRAM",
            (T_halInterface *)&LPC1756_BatteryRAM_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_BatteryRAM.c
 *-------------------------------------------------------------------------*/
