/*-------------------------------------------------------------------------*
 * File:  CRC_Software.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a CRC that merely communicates with
 *      a simple CRC HAL driver implementation.
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include "CRC_Software.h"
#include <Types/CRC.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_CRC *iDevice;
    TBool iHasInitTable;
    T_uezSemaphore iSem;
    TUInt32 iCRCTable[256]; // Lookup table array
    T_uezCRCComputationType iComputationType;
    TUInt32 iSeed;
    TUInt32 iCRC;
} T_Software_CRC_Workspace;

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/

// Reflection is a requirement for the IEEE CRC-32 standard.
// You can create CRCs without it, but they won't conform to the standard.
static TUInt32 ICRC_Reflect(TUInt32 ref, TUInt8 ch)
{
    TUInt32 value = 0;

    // Swap bit 0 for bit 7, bit 1 for bit 6, etc.
    for (TUInt32 i = 1; i < (TUInt8) (ch + 1); i++) {
        if (ref & 1) {
            value |= 1 << (ch - i);
        }
        ref >>= 1;
    }
    return value;
}

// Call this function only once to initialize the CRC table.
static void ICRC32_InitTable(T_Software_CRC_Workspace *p)
{
    TUInt32 i, j;
    TUInt32 *table = p->iCRCTable;

    /****** setup CRC table, only needs to be done once ********/
    // 256 values for ASCII character codes.
    for (i = 0; i <= 0xFF; i++) {
        table[i] = ICRC_Reflect(i, 8) << 24;
        for (j = 0; j < 8; j++) {
            table[i] = (table[i] << 1)
                ^ (table[i] & (1U << 31) ? p->iComputationType : 0);
        }
        table[i] = ICRC_Reflect(table[i], 32);
    }

    p->iHasInitTable = ETrue;
}

static void ICRC32_Calc_CRC_8Bit(
    T_Software_CRC_Workspace *p,
    TUInt8 *buffer,
    TUInt32 dwSize)
{
    // Be sure to use unsigned variables,
    // because negative values introduce high bits
    // where zero bits are required.
    TUInt32 crc = p->iCRC;
    TUInt32 len;

    len = dwSize;
    // Perform the algorithm on each character
    // in the string, using the lookup table values.
    while (len--)
        crc = (crc >> 8) ^ p->iCRCTable[(crc & 0xFF) ^ *buffer++];

    p->iCRC = crc;
}

/*---------------------------------------------------------------------------*
 * Routine:  Software_CRC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace to connect to generic CRC HAL driver.
 *      Automatically connects to the default "CRC" HAL driver.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const char *aHALName        -- Name of HAL device (usually "CRC")
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError Software_CRC_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Software_CRC_Workspace *p = (T_Software_CRC_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iHasInitTable = EFalse;
    p->iSeed = 0xFFFFFFFF;
    p->iCRC = 0xFFFFFFFF;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Software_CRC_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link Software CRC to uEZ
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
/*void Software_CRC_Configure(void *aWorkspace)
 {
 T_Software_CRC_Workspace *p = (T_Software_CRC_Workspace *)aWorkspace;
 }*/

T_uezError Software_CRC_Create(const char *aName)
{
    T_uezDeviceWorkspace *p_crcdev;

    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&Software_CRC_Interface, 0, &p_crcdev);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: Software_CRC_Open
 *---------------------------------------------------------------------------*
 * Description:
 *    
 * Inputs:
 *      void *aWorkspace               -- Workspace
 * Outputs:
 *      T_uezError                     -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Software_CRC_Open(void *aWorkspace)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: Software_CRC_Close
 *---------------------------------------------------------------------------*
 * Description:
 *    
 * Inputs:
 *      void *aWorkspace               -- Workspace
 * Outputs:
 *      T_uezError                     -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Software_CRC_Close(void *aWorkspace)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: Software_CRC_SetComputationType
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets computation mode using CRC32 polynom (or none, currently)
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      T_uezCRCComputationType aType  -- Type of CRC computations to do
 * Outputs:
 *      T_uezError                     -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Software_CRC_SetComputationType(
    void *aWorkspace,
    T_uezCRCComputationType aType)
{
    T_Software_CRC_Workspace *p = (T_Software_CRC_Workspace *)aWorkspace;
    
    if (aType == p->iComputationType)
        return UEZ_ERROR_NONE;
    if (aType != UEZ_CRC_32)
        return UEZ_ERROR_NOT_SUPPORTED;

    p->iHasInitTable = EFalse;
    p->iComputationType = aType;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Software_CRC_AddModifier
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a modifier to the algorithm.  If the modifier already has been
 *      added, do nothing.
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      T_uezCRCModifier aModifier -- Type of modifier to add
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Software_CRC_AddModifier(
    void *aWorkspace,
    T_uezCRCModifier aModifier)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aModifier);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Software_CRC_RemoveModifier
 *---------------------------------------------------------------------------*
 * Description:
 *      Remove a modifier from the algorithm.  If the modifier already has been
 *      added, do nothing.
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      T_uezCRCModifier aModifier -- Type of modifier to add
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Software_CRC_RemoveModifier(
    void *aWorkspace,
    T_uezCRCModifier aModifier)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aModifier);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_Interface_SetSeed
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the seed (usually 0) of a new calculation.
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      TUInt32 aSeed                -- Seed value
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Software_CRC_SetSeed(void *aWorkspace, TUInt32 aSeed)
{
    T_Software_CRC_Workspace *p = (T_Software_CRC_Workspace *)aWorkspace;

    p->iSeed = aSeed;
    p->iCRC = aSeed;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Software_CRC_ReadChecksum
 *---------------------------------------------------------------------------*
 * Description:
 *      Return current checksum reading
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      TUInt32 *aValue -- Place to store checksum
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Software_CRC_ReadChecksum(void *aWorkspace, TUInt32 *aValue)
{
    T_Software_CRC_Workspace *p = (T_Software_CRC_Workspace *)aWorkspace;

    // Return the final value (inverted by the initial value)
    *aValue = p->iCRC ^ 0xffffffff;

    return UEZ_ERROR_NONE;
}

T_uezError Software_CRC_ComputeData(
    void *aWorkspace,
    void *aData,
    T_uezCRCDataElementSize aSize,
    TUInt32 aNumDataElements)
{
    T_Software_CRC_Workspace *p = (T_Software_CRC_Workspace *)aWorkspace;

    // Currently only support 8 bit
    if (aSize != UEZ_CRC_DATA_SIZE_UINT8)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (!p->iHasInitTable)
        ICRC32_InitTable(p);

    // Calculate the 8-bit CRC
    ICRC32_Calc_CRC_8Bit(p, aData, aNumDataElements);
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_CRC Software_CRC_Interface = {
    {
    // Common device interface
        "CRC:Software",
        0x0100,
        Software_CRC_InitializeWorkspace,
        sizeof(T_Software_CRC_Workspace), },

    /* High level functions */
    Software_CRC_Open,
    Software_CRC_Close,

    /* Advanced functions: */
    Software_CRC_SetComputationType,
    Software_CRC_AddModifier,
    Software_CRC_RemoveModifier,
    Software_CRC_SetSeed,
    Software_CRC_ComputeData,
    Software_CRC_ReadChecksum };

/*-------------------------------------------------------------------------*
 * End of File:  CRC_Software.c
 *-------------------------------------------------------------------------*/
