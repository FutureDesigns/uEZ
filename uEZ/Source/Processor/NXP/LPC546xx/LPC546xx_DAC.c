/*-------------------------------------------------------------------------*
 * File:  DAC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx DAC Interface.
 * Implementation:
 *        Setting VRef has no impact on real output value. It should correspond
 *        to real VREF for proper counting of millivolt values.
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_DAC.h>
#include "LPC546xx_GPIO.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define DACR_BIAS               (1<<16)
#define DACR_VALUE_BIT          6

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_DAC *iHAL;
    TUInt32 iVRef;
    TBool iBias;
} T_LPC546xx_DAC_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_DAC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx DAC workspace.
 * Inputs:
 *      void *aW                    -- Particular DAC workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_DAC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_DAC_Workspace *p = (T_LPC546xx_DAC_Workspace *)aWorkspace;

    // Default: bias is not used.
    p->iBias = EFalse;

    // Default: VRef 3,3V -> 3300mv
    p->iVRef = 3300;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_DAC_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write value to the DAC.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 aValue               -- Value to be written to DAC
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_DAC_Write(void *aWorkspace, TUInt32 aValue)
{
    T_LPC546xx_DAC_Workspace *p = (T_LPC546xx_DAC_Workspace *)aWorkspace;

    // Modify value to conform DAC register.
    aValue = (aValue << DACR_VALUE_BIT) & 0xFFC0;

    if (EFalse != p->iBias) {
        // The settling time of the DAC is 2.5 µs and the maximum current is 350 µA.
        aValue |= DACR_BIAS;
    } else {
        //The settling time of the DAC is 1 µs max, and the maximum current is 700 µA.
    }

    // Write modified value to the register.
    LPC_DAC->CR = aValue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_DAC_WriteMilliVolt
 *---------------------------------------------------------------------------*
 * Description:
 *      Write value to the DAC in millivolts.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 aMilliVolts          -- Value in millivolts to be written to DAC
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_DAC_WriteMilliVolt(void *aWorkspace, TUInt32 aMilliVolts)
{
    T_LPC546xx_DAC_Workspace *p = (T_LPC546xx_DAC_Workspace *)aWorkspace;

    // Too high?
    if (aMilliVolts > p->iVRef)
        return UEZ_ERROR_OUT_OF_RANGE;

    // Count register value from mV. Using the same register(stack variable).
    // (10 bits of resolution)
    aMilliVolts = (aMilliVolts * 1024) / p->iVRef;

    // Modify value to conform DAC register.
    aMilliVolts = (aMilliVolts << DACR_VALUE_BIT) & 0xFFC0;

    if (EFalse != p->iBias) {
        // The settling time of the DAC is 2.5 µs
        // and the maximum current is 350 µA.
        aMilliVolts |= DACR_BIAS;
    } else {
        // The settling time of the DAC is 1 µs max,
        // and the maximum current is 700 µA.
    }

    // Write modified value to the register.
    LPC_DAC->CR = aMilliVolts;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_DAC_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read value from the DAC.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 *aValue              -- Value to be written to.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_DAC_Read(void *aWorkspace, TUInt32 *aValue)
{
    // Read value from DAC register, disable BIAS and reserved bits.
    *aValue = (LPC_DAC->CR & 0xFFC0) >> DACR_VALUE_BIT;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_DAC_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read value from the DAC in milliVolts.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 *aMilliVolts         -- Value in mV to be written to.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_DAC_ReadMilliVolt(void *aWorkspace, TUInt32 * aMilliVolts)
{
    TUInt32 value;

    T_LPC546xx_DAC_Workspace *p = (T_LPC546xx_DAC_Workspace *)aWorkspace;

    // Read value from DAC register, disable BIAS and reserved bits.
    value = (LPC_DAC->CR & 0xFFC0) >> DACR_VALUE_BIT;

    // Convert value to millivolts.
    *aMilliVolts = (value * p->iVRef) / 1024;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_DAC_SetBias
 *---------------------------------------------------------------------------*
 * Description:
 *      Set BIAS.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TBool aBias                  -- Value to determine if BIAS should be set.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_DAC_SetBias(void *aWorkspace, TBool aBias)
{
    T_LPC546xx_DAC_Workspace *p = (T_LPC546xx_DAC_Workspace *)aWorkspace;

    // Set BIAS or not.
    if (EFalse != aBias) {
        p->iBias = ETrue;
    } else {
        p->iBias = EFalse;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_DAC_SetVRef
 *---------------------------------------------------------------------------*
 * Description:
 *      Set internal voltage reference.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      const TUInt32 aVRef          -- Set VRef in millivolts.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_DAC_SetVRef(void *aWorkspace, const TUInt32 aVRef)
{
    T_LPC546xx_DAC_Workspace *p = (T_LPC546xx_DAC_Workspace *)aWorkspace;

    // Set Voltage reference.
    p->iVRef = aVRef;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_DAC DAC_LPC546xx_DAC0_Interface = {
        {
                "LPC546xx:DAC0",
                0x0100,
                LPC546xx_DAC_InitializeWorkspace,
                sizeof(T_LPC546xx_DAC_Workspace), },

        LPC546xx_DAC_Read,
        LPC546xx_DAC_ReadMilliVolt,
        LPC546xx_DAC_Write,
        LPC546xx_DAC_WriteMilliVolt,
        LPC546xx_DAC_SetBias,
        LPC546xx_DAC_SetVRef };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_DAC0_Require(T_uezGPIOPortPin aPin)
{
    T_LPC546xx_SCU_ConfigList dac[] = {
            { GPIO_P2_4, SCU_NORMAL_DRIVE(0, \
                    SCU_EPD_DISABLE, \
                    SCU_EPUN_DISABLE, \
                    SCU_EHS_SLOW, \
                    SCU_EZI_DISABLE, \
                    SCU_ZIF_ENABLE)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    // Register DAC0
    HALInterfaceRegister("DAC0", (T_halInterface *)&DAC_LPC546xx_DAC0_Interface,
            0, 0);

    if(aPin != GPIO_NONE){
        LPC546xx_GPIO2_Require();
    }

    LPC546xx_SCU_ConfigPin(aPin, dac, ARRAY_COUNT(dac));

    if(aPin == GPIO_P2_4){
        LPC_SCU->ENAIO0 = (1<<0);
        LPC_DAC->CTRL |= (1<<3); //Despite the name this enable the DAC output
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  DAC.c
 *-------------------------------------------------------------------------*/
