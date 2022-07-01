/*-------------------------------------------------------------------------*
 * File:  RX62N_GPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX62N:GPIO:Interface.
 * Implementation:
 *      A single GPIO0 is created, but the code can be easily changed for
 *      other processors to use multiple GPIO busses.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Types/GPIO.h>
#include <HAL/GPIO.h>
#include <Source/Processor/Renesas/RX62N/RX62N_GPIO.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt8 *iDDR; // Data Direction Register (input or output)
    TUInt8 *iDR; // Data Register (Output data)
    TUInt8 *iPORT; // Port Register (Input data)
    TUInt8 *iICR; // Input Buffer Control Register
    TUInt8 *iPCR; // Pull-Up MOS Control Register
    TUInt8 *iODR; // Open-Drain Control Register
} RX62N_GPIO_PInfo;

typedef struct {
    const HAL_GPIOPort *iHAL;
    const RX62N_GPIO_PInfo *iPortInfo;
	TUInt8 iLocked;
} T_RX62N_GPIO_Workspace;


const RX62N_GPIO_PInfo GPIO_RX62N_Port0_PortInfo = {
    (TUInt8 *)&PORT0.DDR,
    (TUInt8 *)&PORT0.DR,
    (TUInt8 *)&PORT0.PORT,
    (TUInt8 *)&PORT0.ICR,
    (TUInt8 *)0,
    (TUInt8 *)&PORT0.ODR };

const RX62N_GPIO_PInfo GPIO_RX62N_Port1_PortInfo = {
    (TUInt8 *)&PORT1.DDR,
    (TUInt8 *)&PORT1.DR,
    (TUInt8 *)&PORT1.PORT,
    (TUInt8 *)&PORT1.ICR,
    (TUInt8 *)0,
    (TUInt8 *)&PORT1.ODR };

const RX62N_GPIO_PInfo GPIO_RX62N_Port2_PortInfo = {
    (TUInt8 *)&PORT2.DDR,
    (TUInt8 *)&PORT2.DR,
    (TUInt8 *)&PORT2.PORT,
    (TUInt8 *)&PORT2.ICR,
    (TUInt8 *)0,
    (TUInt8 *)&PORT2.ODR };

const RX62N_GPIO_PInfo GPIO_RX62N_Port3_PortInfo = {
    (TUInt8 *)&PORT3.DDR,
    (TUInt8 *)&PORT3.DR,
    (TUInt8 *)&PORT3.PORT,
    (TUInt8 *)&PORT3.ICR,
    (TUInt8 *)0,
    (TUInt8 *)&PORT3.ODR };

const RX62N_GPIO_PInfo GPIO_RX62N_Port4_PortInfo = {
    (TUInt8 *)&PORT4.DDR,
    (TUInt8 *)&PORT4.DR,
    (TUInt8 *)&PORT4.PORT,
    (TUInt8 *)&PORT4.ICR,
    (TUInt8 *)0,
    (TUInt8 *)0 };

const RX62N_GPIO_PInfo GPIO_RX62N_Port5_PortInfo = {
    (TUInt8 *)&PORT5.DDR,
    (TUInt8 *)&PORT5.DR,
    (TUInt8 *)&PORT5.PORT,
    (TUInt8 *)&PORT5.ICR,
    (TUInt8 *)0,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_Port6_PortInfo = {
    (TUInt8 *)&PORT6.DDR,
    (TUInt8 *)&PORT6.DR,
    (TUInt8 *)&PORT6.PORT,
    (TUInt8 *)&PORT6.ICR,
    (TUInt8 *)0,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_Port7_PortInfo = {
    (TUInt8 *)&PORT7.DDR,
    (TUInt8 *)&PORT7.DR,
    (TUInt8 *)&PORT7.PORT,
    (TUInt8 *)&PORT7.ICR,
    (TUInt8 *)0,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_Port8_PortInfo = {
    (TUInt8 *)&PORT8.DDR,
    (TUInt8 *)&PORT8.DR,
    (TUInt8 *)&PORT8.PORT,
    (TUInt8 *)&PORT8.ICR,
    (TUInt8 *)0,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_Port9_PortInfo = {
    (TUInt8 *)&PORT9.DDR,
    (TUInt8 *)&PORT9.DR,
    (TUInt8 *)&PORT9.PORT,
    (TUInt8 *)&PORT9.ICR,
    (TUInt8 *)&PORT9.PCR,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_PortA_PortInfo = {
    (TUInt8 *)&PORTA.DDR,
    (TUInt8 *)&PORTA.DR,
    (TUInt8 *)&PORTA.PORT,
    (TUInt8 *)&PORTA.ICR,
    (TUInt8 *)&PORTA.PCR,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_PortB_PortInfo = {
    (TUInt8 *)&PORTB.DDR,
    (TUInt8 *)&PORTB.DR,
    (TUInt8 *)&PORTB.PORT,
    (TUInt8 *)&PORTB.ICR,
    (TUInt8 *)&PORTB.PCR,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_PortC_PortInfo = {
    (TUInt8 *)&PORTC.DDR,
    (TUInt8 *)&PORTC.DR,
    (TUInt8 *)&PORTC.PORT,
    (TUInt8 *)&PORTC.ICR,
    (TUInt8 *)&PORTC.PCR,
    (TUInt8 *)&PORTC.ODR
};

const RX62N_GPIO_PInfo GPIO_RX62N_PortD_PortInfo = {
    (TUInt8 *)&PORTD.DDR,
    (TUInt8 *)&PORTD.DR,
    (TUInt8 *)&PORTD.PORT,
    (TUInt8 *)&PORTD.ICR,
    (TUInt8 *)&PORTD.PCR,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_PortE_PortInfo = {
    (TUInt8 *)&PORTE.DDR,
    (TUInt8 *)&PORTE.DR,
    (TUInt8 *)&PORTE.PORT,
    (TUInt8 *)&PORTE.ICR,
    (TUInt8 *)&PORTE.PCR,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_PortF_PortInfo = {
    (TUInt8 *)&PORTF.DDR,
    (TUInt8 *)&PORTF.DR,
    (TUInt8 *)&PORTF.PORT,
    (TUInt8 *)&PORTF.ICR,
    (TUInt8 *)0,
    (TUInt8 *)0
};

const RX62N_GPIO_PInfo GPIO_RX62N_PortG_PortInfo = {
    (TUInt8 *)&PORTG.DDR,
    (TUInt8 *)&PORTG.DR,
    (TUInt8 *)&PORTG.PORT,
    (TUInt8 *)&PORTG.ICR,
    (TUInt8 *)&PORTG.PCR,
    (TUInt8 *)0
};

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P0_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port0_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port1 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P1_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port1_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port2 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P2_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port2_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port3 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P3_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port3_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port4 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P4_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port4_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P5_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port5 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P5_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port5_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P6_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port6 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P6_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port6_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P7_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port7 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P7_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port7_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P8_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port8 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P8_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port8_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_P9_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:Port9 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_P9_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_Port9_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_PA_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:PortA workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_PA_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_PortA_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_PB_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:PortB workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_PB_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_PortB_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_PC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:PortC workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_PC_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_PortC_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_PD_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:PortD workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_PD_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_PortD_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_PE_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:PortE workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_PE_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_PortE_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_PF_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:PortF workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_PF_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_PortF_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_PG_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N:GPIO:PortG workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_PG_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX62N_PortG_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    //    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
	
    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_Activate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are active as GPIO pins.
 *      Once active, the pins are set to GPIO inputs or outputs.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has data direction support
    if (!p_info->iDDR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Output mode is set by putting 1's in the IODIR register
    (*p_info->iDDR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has data direction support
    if (!p_info->iDDR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Input mode is set by putting 0's in the IODIR register
    // Make those bits 0
    (*p_info->iDDR) &= ~aPortPins;

    if (p_info->iICR)
        (*p_info->iICR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has output support
    if (!p_info->iDR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Set the bits corresponding to those pins
    (*p_info->iDR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has output support
    if (!p_info->iDR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Set the bits corresponding to those pins
    (*p_info->iDR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a group of GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 *      TUInt32 *aPinsRead          -- Pointer to pins result.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_Read(
    void *aWorkspace,
    TUInt32 aPortPins,
    TUInt32 *aPinsRead)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has input support
    if (!p_info->iPORT)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Get the bits
    *aPinsRead = ((*p_info->iPORT) & aPortPins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 *      T_gpioPull aPull            -- Type of pull
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_SetPull(
    void *aWorkspace,
    TUInt8 aPortPinIndex,
    T_gpioPull aPull)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    if (aPortPinIndex >= 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Make sure this port has pull-up support
    if (!p_info->iPCR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Only pull up or no pull is allowed
    if ((aPull == GPIO_PULL_UP) || (aPull == GPIO_PULL_NONE)) {
        // Set the pull up to on or off
        if (aPull == GPIO_PULL_UP) {
            (*p_info->iPCR) |= (1 << aPortPinIndex);
        } else {
            // Turn off the pull up
            (*p_info->iPCR) &= ~(1 << aPortPinIndex);
        }
    } else {
        // Incorrect pull up type
        return UEZ_ERROR_ILLEGAL_OPERATION;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_SetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pin's mux feature.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-7
 *      T_gpioMux aMux              -- 0 for open drain off, 1 for open drain on
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_SetMux(
    void *aWorkspace,
    TUInt8 aPortPinIndex,
    T_gpioMux aMux)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    if (aPortPinIndex >= 8)
        return UEZ_ERROR_INVALID_PARAMETER;
    if (aMux > 1)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Make sure this port has pull-up support
    if (!p_info->iODR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Set the open drain to on or off
    if (aMux == 1) {
        (*p_info->iODR) |= (1 << aPortPinIndex);
    } else {
        // Turn off the pull up
        (*p_info->iODR) &= ~(1 << aPortPinIndex);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_GPIO_GetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Get pin's mux feature.  1=pull-up is on, 0=pull-up is off.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-7
 *      T_gpioMux *aMux             -- Returned mux option (0=io pin, 1=peripheral)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_GPIO_GetMux(
    void *aWorkspace,
    TUInt8 aPortPinIndex,
    T_gpioMux *aMux)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    // Default is generic i/o (pull-up is off)
    *aMux = 0;

    if (aPortPinIndex >= 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Make sure this port has pull-up support
    if (!p_info->iODR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Set port mode register
    if ((*p_info->iODR) & (1 << aPortPinIndex)) {
        // Pin is for pull-up is on
        *aMux = 1;
    } else {
        // Pin is for pull-up is off
        *aMux = 0;
    }

    return UEZ_ERROR_NONE;
}
T_uezError RX62N_GPIO_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    const RX62N_GPIO_PInfo *p_info = p->iPortInfo;

    PARAM_NOT_USED(aValue);

    // Look up the IOCON register
    switch (aControl) {
        case GPIO_NOP:
            // Yep, do no operation
            break;
        case GPIO_CONTROL_ENABLE_INPUT_BUFFER:
            if (!p_info->iICR)
                return UEZ_ERROR_ILLEGAL_OPERATION;
            (*p_info->iICR) |= (1<<aPortPinIndex);
            break;
        case GPIO_CONTROL_DISABLE_INPUT_BUFFER:
            if (!p_info->iICR)
                return UEZ_ERROR_ILLEGAL_OPERATION;
            (*p_info->iICR) &= ~(1<<aPortPinIndex);
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
    }
    return UEZ_ERROR_NONE;
}

T_uezError RX62N_GPIO_ConfigureInterruptCallbackNotSupported(
    void *aWorkspace,
    T_gpioInterruptHandler iInterruptCallback,
    void *iInterruptCallbackWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX62N_GPIO_EnableInterruptsNotSupported(
    void *aWorkspace,
    TUInt32 aPortPins,
    T_gpioInterruptType aType)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX62N_GPIO_DisableInterruptsNotSupported(
    void *aWorkspace,
    TUInt32 aPortPins,
    T_gpioInterruptType aType)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX62N_GPIO_ClearInterruptsNotSupported(
    void *aWorkspace,
    TUInt32 aPortPins)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX62N_GPIO_Lock(void *aWorkspace, TUInt32 aPortPins)
{
#ifndef NDEBUG
    char buffer [50];
#endif
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        #ifndef NDEBUG
          sprintf (buffer, "PinLock on %s Pin %d", p->iHAL->iInterface.iName, aPortPins);
          UEZFailureMsg(buffer);
        #else
          UEZFailureMsg("PinLock");
        #endif
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError RX62N_GPIO_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX62N_GPIO_Workspace *p = (T_RX62N_GPIO_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_RX62N_Port0_Interface = {
    "RX62N:GPIO:Port 0",
    0x0100,
    RX62N_GPIO_P0_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
	RX62N_GPIO_Unlock,
	/* uEZ v2.04 */
	RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port1_Interface = {
    "RX62N:GPIO:Port 1",
    0x0100,
    RX62N_GPIO_P1_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port2_Interface = {
    "RX62N:GPIO:Port 2",
    0x0100,
    RX62N_GPIO_P2_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port3_Interface = {
    "RX62N:GPIO:Port 3",
    0x0100,
    RX62N_GPIO_P3_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port4_Interface = {
    "RX62N:GPIO:Port 4",
    0x0100,
    RX62N_GPIO_P4_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port5_Interface = {
    "RX62N:GPIO:Port 5",
    0x0100,
    RX62N_GPIO_P5_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port6_Interface = {
    "RX62N:GPIO:Port 6",
    0x0100,
    RX62N_GPIO_P6_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port7_Interface = {
    "RX62N:GPIO:Port 7",
    0x0100,
    RX62N_GPIO_P7_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port8_Interface = {
    "RX62N:GPIO:Port 8",
    0x0100,
    RX62N_GPIO_P8_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_Port9_Interface = {
    "RX62N:GPIO:Port 9",
    0x0100,
    RX62N_GPIO_P9_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_PortA_Interface = {
    "RX62N:GPIO:Port A",
    0x0100,
    RX62N_GPIO_PA_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_PortB_Interface = {
    "RX62N:GPIO:Port B",
    0x0100,
    RX62N_GPIO_PB_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_PortC_Interface = {
    "RX62N:GPIO:Port C",
    0x0100,
    RX62N_GPIO_PC_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_PortD_Interface = {
    "RX62N:GPIO:Port D",
    0x0100,
    RX62N_GPIO_PD_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_PortE_Interface = {
    "RX62N:GPIO:Port E",
    0x0100,
    RX62N_GPIO_PE_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_PortF_Interface = {
    "RX62N:GPIO:Port F",
    0x0100,
    RX62N_GPIO_PF_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

const HAL_GPIOPort GPIO_RX62N_PortG_Interface = {
    "RX62N:GPIO:Port G",
    0x0100,
    RX62N_GPIO_PG_InitializeWorkspace,
    sizeof(T_RX62N_GPIO_Workspace),

    0,
    0,
    0,
    RX62N_GPIO_SetOutputMode,
    RX62N_GPIO_SetInputMode,
    RX62N_GPIO_Set,
    RX62N_GPIO_Clear,
    RX62N_GPIO_Read,
    RX62N_GPIO_SetPull,
    RX62N_GPIO_SetMux,

    RX62N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX62N_GPIO_EnableInterruptsNotSupported,
    RX62N_GPIO_DisableInterruptsNotSupported,
    RX62N_GPIO_ClearInterruptsNotSupported,

    RX62N_GPIO_Control,
	RX62N_GPIO_Lock,
    RX62N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX62N_GPIO_GetMux
};

void RX62N_GPIO_P0_Require(void)
{
    HAL_GPIOPort **p_gpio0;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO0",
            (T_halInterface *)&GPIO_RX62N_Port0_Interface, 0,
            (T_halWorkspace **)&p_gpio0);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_0, p_gpio0);
}

void RX62N_GPIO_P1_Require(void)
{
    HAL_GPIOPort **p_gpio1;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO1",
            (T_halInterface *)&GPIO_RX62N_Port1_Interface, 0,
            (T_halWorkspace **)&p_gpio1);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_1, p_gpio1);
}

void RX62N_GPIO_P2_Require(void)
{
    HAL_GPIOPort **p_gpio2;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO2",
            (T_halInterface *)&GPIO_RX62N_Port2_Interface, 0,
            (T_halWorkspace **)&p_gpio2);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_2, p_gpio2);
}

void RX62N_GPIO_P3_Require(void)
{
    HAL_GPIOPort **p_gpio3;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO3",
            (T_halInterface *)&GPIO_RX62N_Port3_Interface, 0,
            (T_halWorkspace **)&p_gpio3);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_3, p_gpio3);
}

void RX62N_GPIO_P4_Require(void)
{
    HAL_GPIOPort **p_gpio4;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO4",
            (T_halInterface *)&GPIO_RX62N_Port4_Interface, 0,
            (T_halWorkspace **)&p_gpio4);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_4, p_gpio4);
}

void RX62N_GPIO_P5_Require(void)
{
    HAL_GPIOPort **p_gpio5;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO5",
            (T_halInterface *)&GPIO_RX62N_Port5_Interface, 0,
            (T_halWorkspace **)&p_gpio5);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_5, p_gpio5);
}

void RX62N_GPIO_P6_Require(void)
{
    HAL_GPIOPort **p_gpio6;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO6",
            (T_halInterface *)&GPIO_RX62N_Port6_Interface, 0,
            (T_halWorkspace **)&p_gpio6);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_6, p_gpio6);
}

void RX62N_GPIO_P7_Require(void)
{
    HAL_GPIOPort **p_gpio7;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO7",
            (T_halInterface *)&GPIO_RX62N_Port7_Interface, 0,
            (T_halWorkspace **)&p_gpio7);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_7, p_gpio7);
}

void RX62N_GPIO_P8_Require(void)
{
    HAL_GPIOPort **p_gpio8;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO8",
            (T_halInterface *)&GPIO_RX62N_Port8_Interface, 0,
            (T_halWorkspace **)&p_gpio8);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_8, p_gpio8);
}

void RX62N_GPIO_P9_Require(void)
{
    HAL_GPIOPort **p_gpio9;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO9",
            (T_halInterface *)&GPIO_RX62N_Port9_Interface, 0,
            (T_halWorkspace **)&p_gpio9);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_9, p_gpio9);
}

void RX62N_GPIO_PA_Require(void)
{
    HAL_GPIOPort **p_gpioA;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOA",
            (T_halInterface *)&GPIO_RX62N_PortA_Interface, 0,
            (T_halWorkspace **)&p_gpioA);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_A, p_gpioA);
}

void RX62N_GPIO_PB_Require(void)
{
    HAL_GPIOPort **p_gpioB;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOB",
            (T_halInterface *)&GPIO_RX62N_PortB_Interface, 0,
            (T_halWorkspace **)&p_gpioB);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_B, p_gpioB);
}

void RX62N_GPIO_PC_Require(void)
{
    HAL_GPIOPort **p_gpioC;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOC",
            (T_halInterface *)&GPIO_RX62N_PortC_Interface, 0,
            (T_halWorkspace **)&p_gpioC);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_C, p_gpioC);
}

void RX62N_GPIO_PD_Require(void)
{
    HAL_GPIOPort **p_gpioD;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOD",
            (T_halInterface *)&GPIO_RX62N_PortD_Interface, 0,
            (T_halWorkspace **)&p_gpioD);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_D, p_gpioD);
}

void RX62N_GPIO_PE_Require(void)
{
    HAL_GPIOPort **p_gpioE;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOE",
            (T_halInterface *)&GPIO_RX62N_PortE_Interface, 0,
            (T_halWorkspace **)&p_gpioE);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_E, p_gpioE);
}

void RX62N_GPIO_PF_Require(void)
{
    HAL_GPIOPort **p_gpioF;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOF",
            (T_halInterface *)&GPIO_RX62N_PortF_Interface, 0,
            (T_halWorkspace **)&p_gpioF);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_F, p_gpioF);
}

void RX62N_GPIO_PG_Require(void)
{
    HAL_GPIOPort **p_gpioG;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOG",
            (T_halInterface *)&GPIO_RX62N_PortG_Interface, 0,
            (T_halWorkspace **)&p_gpioG);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_G, p_gpioG);
}

/*-------------------------------------------------------------------------*
 * End of File:  GPIO.c
 *-------------------------------------------------------------------------*/
