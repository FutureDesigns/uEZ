/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_CRC.c
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
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_CRC.h>
#include <uEZProcessor.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define CRC_POLY_OFFSET                     0
#define     CRC_POLY_MASK                       3
#define     CRC_POLY_CRC32                      2
#define     CRC_POLY_CRC16                      1
#define     CRC_POLY_CRC_CCITT                  0
#define BIT_RVS_WR_OFFSET                   2
#define CMPL_WR_OFFSET                      3
#define BIT_RVS_SUM_OFFSET                  4
#define CMPL_SUM_OFFSET                     5

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_CRC *iHAL;
// Not used
} T_LPC17xx_40xx_CRC_Workspace;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static TUInt32 ICRCModifierBits(T_uezCRCModifier aModifier);

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  ICRCModifierBits
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert a modifier into a bit field for the CRC_MODE register.
 *      Returns 0 if not supported.
 * Inputs:
 *      T_uezCRCModifier aModifier -- Type of modifier to add
 * Outputs:
 *      TUInt32 -- Bit field for CRC_MODE
 *---------------------------------------------------------------------------*/
static TUInt32 ICRCModifierBits(T_uezCRCModifier aModifier)
{
    switch (aModifier) {
        case UEZ_CRC_MODIFIER_INPUT_REVERSE_BIT_ORDER:
            return (1 << BIT_RVS_WR_OFFSET);
        case UEZ_CRC_MODIFIER_INPUT_ONES_COMPLIMENT:
            return (1 << CMPL_WR_OFFSET);
        case UEZ_CRC_MODIFIER_CHECKSUM_REVERSE_BIT_ORDER:
            return (1 << BIT_RVS_SUM_OFFSET);
        case UEZ_CRC_MODIFIER_CHECKSUM_ONES_COMPLIMENT:
            return (1 << CMPL_SUM_OFFSET);
        default:
            // No bits! Not supported
            return 0;
    }
#if (COMPILER_TYPE!=IAR)    
    return 0;
#endif    
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_CRC_SetComputationType
 *---------------------------------------------------------------------------*
 * Description:
 *      Set a computational method for this CRC device
 * Inputs:
 *      void *aWorkspace -- HAL_CRC Workspace
 *      T_uezCRCComputationType aType -- Type of CRC computations to do
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_CRC_SetComputationType(
        void *aWorkspace,
        T_uezCRCComputationType aType)
{
    switch (aType) {
        case UEZ_CRC_16_CCITT:
            LPC_CRC->MODE = 0x00;
            break;
        case UEZ_CRC_16_IBM:
            LPC_CRC->MODE = 0x15;
            break;
        case UEZ_CRC_32_ANSI:
            LPC_CRC->MODE = 0x36;
            break;
        default:
            return UEZ_ERROR_INVALID_PARAMETER;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_CRC_AddModifier
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a modifier to the algorithm.  If the modifier already has been
 *      added, do nothing.
 * Inputs:
 *      void *aWorkspace -- HAL_CRC Workspace
 *      T_uezCRCModifier aModifier -- Type of modifier to add
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_CRC_AddModifier(
        void *aWorkspace,
        T_uezCRCModifier aModifier)
{
    TUInt32 bits = ICRCModifierBits(aModifier);
    if (bits) {
        // Turn on these features
        LPC_CRC->MODE |= bits;
        return UEZ_ERROR_NONE;
    }
    return UEZ_ERROR_INVALID_PARAMETER;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_CRC_RemoveModifier
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a modifier to the algorithm.  If the modifier already has been
 *      added, do nothing.
 * Inputs:
 *      void *aWorkspace -- HAL_CRC Workspace
 *      T_uezCRCModifier aModifier -- Type of modifier to add
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_CRC_RemoveModifier(
        void *aWorkspace,
        T_uezCRCModifier aModifier)
{
    TUInt32 bits = ICRCModifierBits(aModifier);
    PARAM_NOT_USED(aWorkspace);

    if (bits) {
        // Turn off these features
        LPC_CRC->MODE &= ~bits;
        return UEZ_ERROR_NONE;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_CRC_SetSeed
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the seed (usually 0) of a new calculation.
 * Inputs:
 *      void *aWorkspace -- HAL_CRC Workspace
 *      TUInt32 aSeed                -- Seed value
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_CRC_SetSeed(void *aWorkspace, TUInt32 aSeed)
{
    PARAM_NOT_USED(aWorkspace);

    LPC_CRC->SEED = aSeed;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_CRC_ComputeData
 *---------------------------------------------------------------------------*
 * Description:
 *      Pass in a set of data to compute a checksum.  This can be a single
 *      value or a group of data.  The size of the data elements are passed
 *      as well to tell if computations on small data (bytes) or larger
 *      chunks (words).
 * Inputs:
 *      void *aWorkspace -- HAL_CRC Workspace
 *      void *aData -- Pointer to data array
 *      T_uezCRCDataElementSize aSize -- Size of each element
 *      TUInt32 aNumDataElements -- Number of data elements to process
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_CRC_ComputeData(
        void *aWorkspace,
        void *aData,
        T_uezCRCDataElementSize aSize,
        TUInt32 aNumDataElements)
{
    TUInt8 *p8;
    TUInt16 *p16;
    TUInt32 *p32;
    PARAM_NOT_USED(aWorkspace);

    switch (aSize) {
        case UEZ_CRC_DATA_SIZE_UINT8: {
            p8 = (TUInt8 *)aData;
            while (aNumDataElements--)
                LPC_CRC->WR_DATA_8 = *(p8++);
        }
            break;
        case UEZ_CRC_DATA_SIZE_UINT16: {
            p16 = (TUInt16 *)aData;
            while (aNumDataElements--)
                LPC_CRC->WR_DATA_16 = *(p16++);
        }
            break;
        case UEZ_CRC_DATA_SIZE_UINT32: {
            p32 = (TUInt32 *)aData;
            while (aNumDataElements--)
                LPC_CRC->WR_DATA_32 = *(p32++);
        }
            break;
        default:
            return UEZ_ERROR_INVALID_PARAMETER;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_CRC_ReadChecksum
 *---------------------------------------------------------------------------*
 * Description:
 *      Return current checksum reading
 * Inputs:
 *      void *aWorkspace -- HAL_CRC Workspace
 *      TUInt32 *aValue -- Place to store checksum
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ILPC17xx_40xx_CRC_ReadChecksum(void *aWorkspace, TUInt32 *aValue)
{
    PARAM_NOT_USED(aWorkspace);

    *aValue = (TUInt32)LPC_CRC->SUM;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_CRC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the workspace
 * Inputs:
 *      void *aWorkspace -- HAL_CRC Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_CRC_InitializeWorkspace(void *aWorkspace)
{
    //T_LPC17xx_40xx_CRC_Workspace *p = (T_LPC17xx_40xx_CRC_Workspace *)aWorkspace;
    PARAM_NOT_USED(aWorkspace);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interface: LPC17xx_40xx CRC
 *---------------------------------------------------------------------------*/
const HAL_CRC CRC_LPC17xx_40xx_Interface = {
        {
                "NXP:LPC17xx_40xx:CRC",
                0x0110,
                LPC17xx_40xx_CRC_InitializeWorkspace,
                sizeof(T_LPC17xx_40xx_CRC_Workspace), },

        ILPC17xx_40xx_CRC_SetComputationType,
        ILPC17xx_40xx_CRC_AddModifier,
        ILPC17xx_40xx_CRC_RemoveModifier,
        ILPC17xx_40xx_CRC_SetSeed,
        ILPC17xx_40xx_CRC_ComputeData,
        ILPC17xx_40xx_CRC_ReadChecksum };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_CRC0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("CRC0", (T_halInterface *)&CRC_LPC17xx_40xx_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_CRC.c
 *-------------------------------------------------------------------------*/
