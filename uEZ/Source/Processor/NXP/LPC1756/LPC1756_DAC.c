/*-------------------------------------------------------------------------*
 * File:  DAC.c
 *-------------------------------------------------------------------------*
 * DesDACRiption:
 *      HAL implementation of the LPC1756 DAC Interface.
 * Implementation:
 *        Setting VRef has no impact on real output value. It should correspond
 *        to real VREF for proper counting of millivolt values.
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_DAC.h>

#include <Source/Processor/NXP/LPC1756/LPC1756_UtilityFuncs.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_GPIO.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define DADACR_BIAS               (1<<16)
#define DADACR_VALUE_BIT          6

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_DAC *iHAL;
    TUInt32 iVRef;
    TBool iBias;
} T_LPC1756_DAC_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_DAC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * DesDACRiption:
 *      Setup the LPC1756 DAC workspace.
 * Inputs:
 *      void *aW                    -- Particular DAC workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_DAC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_DAC_Workspace *p = (T_LPC1756_DAC_Workspace *)aWorkspace;

    // Default: bias is not used.
    p->iBias = EFalse;

    // Default: VRef 3,3V -> 3300mv
    p->iVRef = 3300;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_DAC_Write
 *---------------------------------------------------------------------------*
 * DesDACRiption:
 *      Write value to the DAC.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 aValue               -- Value to be written to DAC
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_DAC_Write(void *aWorkspace, TUInt32 aValue)
{
    T_LPC1756_DAC_Workspace *p = (T_LPC1756_DAC_Workspace *)aWorkspace;

    // Modify value to conform DAC register.
    aValue = (aValue << DADACR_VALUE_BIT) & 0xFFC0;

    if (EFalse != p->iBias) {
        // The settling time of the DAC is 2.5 µs and the maximum current is 350 µA.
        aValue |= DADACR_BIAS;
    } else {
        //The settling time of the DAC is 1 µs max, and the maximum current is 700 µA.
    }

    // Write modified value to the register.
    DAC->DACR = aValue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_DAC_WriteMilliVolt
 *---------------------------------------------------------------------------*
 * DesDACRiption:
 *      Write value to the DAC in millivolts.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 aMilliVolts          -- Value in millivolts to be written to DAC
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_DAC_WriteMilliVolt(void *aWorkspace, TUInt32 aMilliVolts)
{
    T_LPC1756_DAC_Workspace *p = (T_LPC1756_DAC_Workspace *)aWorkspace;

    // Too high?
    if (aMilliVolts > p->iVRef)
        return UEZ_ERROR_OUT_OF_RANGE;

    // Count register value from mV. Using the same register(stack variable).
    // (10 bits of resolution)
    aMilliVolts = (aMilliVolts * 1024) / p->iVRef;

    // Modify value to conform DAC register.
    aMilliVolts = (aMilliVolts << DADACR_VALUE_BIT) & 0xFFC0;

    if (EFalse != p->iBias) {
        // The settling time of the DAC is 2.5 µs
        // and the maximum current is 350 µA.
        aMilliVolts |= DADACR_BIAS;
    } else {
        // The settling time of the DAC is 1 µs max,
        // and the maximum current is 700 µA.
    }

    // Write modified value to the register.
    DAC->DACR = aMilliVolts;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_DAC_Read
 *---------------------------------------------------------------------------*
 * DesDACRiption:
 *      Read value from the DAC.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 *aValue              -- Value to be written to.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_DAC_Read(void *aWorkspace, TUInt32 *aValue)
{
    // Read value from DAC register, disable BIAS and reserved bits.
    *aValue = (DAC->DACR & 0xFFC0) >> DADACR_VALUE_BIT;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_DAC_Read
 *---------------------------------------------------------------------------*
 * DesDACRiption:
 *      Read value from the DAC in milliVolts.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 *aMilliVolts         -- Value in mV to be written to.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_DAC_ReadMilliVolt(void *aWorkspace, TUInt32 * aMilliVolts)
{
    TUInt32 value;

    T_LPC1756_DAC_Workspace *p = (T_LPC1756_DAC_Workspace *)aWorkspace;

    // Read value from DAC register, disable BIAS and reserved bits.
    value = (DAC->DACR & 0xFFC0) >> DADACR_VALUE_BIT;

    // Convert value to millivolts.
    *aMilliVolts = (value * p->iVRef) / 1024;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_DAC_SetBias
 *---------------------------------------------------------------------------*
 * DesDACRiption:
 *      Set BIAS.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TBool aBias                  -- Value to determine if BIAS should be set.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_DAC_SetBias(void *aWorkspace, TBool aBias)
{
    T_LPC1756_DAC_Workspace *p = (T_LPC1756_DAC_Workspace *)aWorkspace;

    // Set BIAS or not.
    if (EFalse != aBias) {
        p->iBias = ETrue;
    } else {
        p->iBias = EFalse;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_DAC_SetVRef
 *---------------------------------------------------------------------------*
 * DesDACRiption:
 *      Set internal voltage reference.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      const TUInt32 aVRef          -- Set VRef in millivolts.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1756_DAC_SetVRef(void *aWorkspace, const TUInt32 aVRef)
{
    T_LPC1756_DAC_Workspace *p = (T_LPC1756_DAC_Workspace *)aWorkspace;

    // Set Voltage reference.
    p->iVRef = aVRef;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_DAC DAC_LPC1756_DAC0_Interface = {
        {
                "LPC1756:DAC0",
                0x0100,
                LPC1756_DAC_InitializeWorkspace,
                sizeof(T_LPC1756_DAC_Workspace), },

        LPC1756_DAC_Read,
        LPC1756_DAC_ReadMilliVolt,
        LPC1756_DAC_Write,
        LPC1756_DAC_WriteMilliVolt,
        LPC1756_DAC_SetBias,
        LPC1756_DAC_SetVRef };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1756_DAC0_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC1756_IOCON_ConfigList dac0[] = {
            {GPIO_P0_26, IOCON_A_DEFAULT(2)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    // Register DAC0
    HALInterfaceRegister("DAC0", (T_halInterface *)&DAC_LPC1756_DAC0_Interface,
            0, 0);
    LPC1756_IOCON_ConfigPin(aPin, dac0, ARRAY_COUNT(dac0));
}

/*-------------------------------------------------------------------------*
 * End of File:  DAC.c
 *-------------------------------------------------------------------------*/
