/*-------------------------------------------------------------------------*
 * File:  RX63N_GPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX63N:GPIO:Interface.
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
#include <Source/Processor/Renesas/RX63N/RX63N_GPIO.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt8 *iPDR;  // Port Direction Register (input or output)
    TUInt8 *iPODR; // Port Output Data Register
    TUInt8 *iPIDR; // Port Input Data Register
    TUInt8 *iPMR;  // Input Buffer Control Register
	TUInt8 *iODR0; // Open-Drain Control Register 0
	TUInt8 *iODR1; // Open-Drain Control Register 1
    TUInt8 *iPCR;  // Pull-Up Resister Control Register
    TUInt8 *iDSCR; // Drive Capacity Control Register
} RX63N_GPIO_PInfo;

typedef struct {
    const HAL_GPIOPort *iHAL;
    const RX63N_GPIO_PInfo *iPortInfo;
	TUInt8 iLocked;
} T_RX63N_GPIO_Workspace;


const RX63N_GPIO_PInfo GPIO_RX63N_Port0_PortInfo = {
    (TUInt8 *)&PORT0.PDR,
    (TUInt8 *)&PORT0.PODR,
    (TUInt8 *)&PORT0.PIDR,
    (TUInt8 *)&PORT0.PMR,
    (TUInt8 *)&PORT0.ODR0,
    (TUInt8 *)&PORT0.ODR1,
	(TUInt8 *)&PORT0.PCR,
	(TUInt8 *)&PORT0.DSCR 
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port1_PortInfo = {
    (TUInt8 *)&PORT1.PDR,
    (TUInt8 *)&PORT1.PODR,
    (TUInt8 *)&PORT1.PIDR,
    (TUInt8 *)&PORT1.PMR,
    (TUInt8 *)&PORT1.ODR0,
    (TUInt8 *)&PORT1.ODR1,
	(TUInt8 *)&PORT1.PCR,
	(TUInt8 *)0 
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port2_PortInfo = {
    (TUInt8 *)&PORT2.PDR,
    (TUInt8 *)&PORT2.PODR,
    (TUInt8 *)&PORT2.PIDR,
    (TUInt8 *)&PORT2.PMR,
    (TUInt8 *)&PORT2.ODR0,
    (TUInt8 *)&PORT2.ODR1,
	(TUInt8 *)&PORT2.PCR,
	(TUInt8 *)&PORT2.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port3_PortInfo = {
    (TUInt8 *)&PORT3.PDR,
    (TUInt8 *)&PORT3.PODR,
    (TUInt8 *)&PORT3.PIDR,
    (TUInt8 *)&PORT3.PMR,
    (TUInt8 *)&PORT3.ODR0,
    (TUInt8 *)&PORT3.ODR1,
	(TUInt8 *)&PORT3.PCR,
	(TUInt8 *)0 
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port4_PortInfo = {
    (TUInt8 *)&PORT4.PDR,
    (TUInt8 *)&PORT4.PODR,
    (TUInt8 *)&PORT4.PIDR,
    (TUInt8 *)&PORT4.PMR,
    (TUInt8 *)&PORT4.ODR0,
    (TUInt8 *)&PORT4.ODR1,
	(TUInt8 *)&PORT4.PCR,
	(TUInt8 *)0 
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port5_PortInfo = {
    (TUInt8 *)&PORT5.PDR,
    (TUInt8 *)&PORT5.PODR,
    (TUInt8 *)&PORT5.PIDR,
    (TUInt8 *)&PORT5.PMR,
    (TUInt8 *)&PORT5.ODR0,
    (TUInt8 *)&PORT5.ODR1,
	(TUInt8 *)&PORT5.PCR,
	(TUInt8 *)&PORT0.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port6_PortInfo = {
    (TUInt8 *)&PORT6.PDR,
    (TUInt8 *)&PORT6.PODR,
    (TUInt8 *)&PORT6.PIDR,
    (TUInt8 *)&PORT6.PMR,
    (TUInt8 *)&PORT6.ODR0,
    (TUInt8 *)&PORT6.ODR1,
	(TUInt8 *)&PORT6.PCR,
	(TUInt8 *)0
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port7_PortInfo = {
    (TUInt8 *)&PORT7.PDR,
    (TUInt8 *)&PORT7.PODR,
    (TUInt8 *)&PORT7.PIDR,
    (TUInt8 *)&PORT7.PMR,
    (TUInt8 *)&PORT7.ODR0,
    (TUInt8 *)&PORT7.ODR1,
	(TUInt8 *)&PORT7.PCR,
	(TUInt8 *)0
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port8_PortInfo = {
    (TUInt8 *)&PORT8.PDR,
    (TUInt8 *)&PORT8.PODR,
    (TUInt8 *)&PORT8.PIDR,
    (TUInt8 *)&PORT8.PMR,
    (TUInt8 *)&PORT8.ODR0,
    (TUInt8 *)&PORT8.ODR1,
	(TUInt8 *)&PORT8.PCR,
	(TUInt8 *)0
};

const RX63N_GPIO_PInfo GPIO_RX63N_Port9_PortInfo = {
    (TUInt8 *)&PORT9.PDR,
    (TUInt8 *)&PORT9.PODR,
    (TUInt8 *)&PORT9.PIDR,
    (TUInt8 *)&PORT9.PMR,
    (TUInt8 *)&PORT9.ODR0,
    (TUInt8 *)&PORT9.ODR1,
	(TUInt8 *)&PORT9.PCR,
	(TUInt8 *)&PORT9.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortA_PortInfo = {
    (TUInt8 *)&PORTA.PDR,
    (TUInt8 *)&PORTA.PODR,
    (TUInt8 *)&PORTA.PIDR,
    (TUInt8 *)&PORTA.PMR,
    (TUInt8 *)&PORTA.ODR0,
    (TUInt8 *)&PORTA.ODR1,
	(TUInt8 *)&PORTA.PCR,
	(TUInt8 *)&PORTA.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortB_PortInfo = {
    (TUInt8 *)&PORTB.PDR,
    (TUInt8 *)&PORTB.PODR,
    (TUInt8 *)&PORTB.PIDR,
    (TUInt8 *)&PORTB.PMR,
    (TUInt8 *)&PORTB.ODR0,
    (TUInt8 *)&PORTB.ODR1,
	(TUInt8 *)&PORTB.PCR,
	(TUInt8 *)&PORTB.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortC_PortInfo = {
    (TUInt8 *)&PORTC.PDR,
    (TUInt8 *)&PORTC.PODR,
    (TUInt8 *)&PORTC.PIDR,
    (TUInt8 *)&PORTC.PMR,
    (TUInt8 *)&PORTC.ODR0,
    (TUInt8 *)&PORTC.ODR1,
	(TUInt8 *)&PORTC.PCR,
	(TUInt8 *)&PORTC.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortD_PortInfo = {
    (TUInt8 *)&PORTD.PDR,
    (TUInt8 *)&PORTD.PODR,
    (TUInt8 *)&PORTD.PIDR,
    (TUInt8 *)&PORTD.PMR,
    (TUInt8 *)&PORTD.ODR0,
    (TUInt8 *)&PORTD.ODR1,
	(TUInt8 *)&PORTD.PCR,
	(TUInt8 *)&PORTD.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortE_PortInfo = {
    (TUInt8 *)&PORTE.PDR,
    (TUInt8 *)&PORTE.PODR,
    (TUInt8 *)&PORTE.PIDR,
    (TUInt8 *)&PORTE.PMR,
    (TUInt8 *)&PORTE.ODR0,
    (TUInt8 *)&PORTE.ODR1,
	(TUInt8 *)&PORTE.PCR,
	(TUInt8 *)&PORTE.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortF_PortInfo = {
    (TUInt8 *)&PORTF.PDR,
    (TUInt8 *)&PORTF.PODR,
    (TUInt8 *)&PORTF.PIDR,
    (TUInt8 *)&PORTF.PMR,
    (TUInt8 *)&PORTF.ODR0,
    (TUInt8 *)&PORTF.ODR1,
	(TUInt8 *)&PORTF.PCR,
	(TUInt8 *)0
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortG_PortInfo = {
    (TUInt8 *)&PORTG.PDR,
    (TUInt8 *)&PORTG.PODR,
    (TUInt8 *)&PORTG.PIDR,
    (TUInt8 *)&PORTG.PMR,
    (TUInt8 *)&PORTG.ODR0,
    (TUInt8 *)&PORTG.ODR1,
	(TUInt8 *)&PORTG.PCR,
	(TUInt8 *)&PORTG.DSCR
};

const RX63N_GPIO_PInfo GPIO_RX63N_PortJ_PortInfo = {
    (TUInt8 *)&PORTJ.PDR,
    (TUInt8 *)&PORTJ.PODR,
    (TUInt8 *)&PORTJ.PIDR,
    (TUInt8 *)&PORTJ.PMR,
    (TUInt8 *)&PORTJ.ODR0,
    (TUInt8 *)&PORTJ.ODR1,
	(TUInt8 *)&PORTJ.PCR,
	(TUInt8 *)0
};

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P0_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port0_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port1 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P1_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port1_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port2 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P2_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port2_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port3 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P3_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port3_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port4 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P4_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port4_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P5_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port5 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P5_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port5_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P6_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port6 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P6_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port6_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P7_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port7 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P7_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port7_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P8_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port8 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P8_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port8_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_P9_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:Port9 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_P9_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_Port9_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_PA_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:PortA workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_PA_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_PortA_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_PB_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:PortB workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_PB_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_PortB_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_PC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:PortC workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_PC_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_PortC_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_PD_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:PortD workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_PD_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_PortD_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_PE_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:PortE workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_PE_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_PortE_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_PF_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:PortF workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_PF_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_PortF_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_PG_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N:GPIO:PortG workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_PG_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_RX63N_PortG_PortInfo;
	p->iLocked = 0;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    //    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
	
    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_Activate
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
T_uezError RX63N_GPIO_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has data direction support
    if (!p_info->iPDR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Output mode is set by putting 1's in the IODIR register
    (*p_info->iPDR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has data direction support
    if (!p_info->iPDR)
        return UEZ_ERROR_ILLEGAL_OPERATION;   

    // Input mode is set by putting 0's in the IODIR register
    // Make those bits 0
    (*p_info->iPDR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has output support
    if (!p_info->iPODR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Set the bits corresponding to those pins
    (*p_info->iPODR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has output support
    if (!p_info->iPODR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Set the bits corresponding to those pins
    (*p_info->iPODR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_Read
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
T_uezError RX63N_GPIO_Read(
    void *aWorkspace,
    TUInt32 aPortPins,
    TUInt32 *aPinsRead)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    // Make sure this port has input support
    if (!p_info->iPIDR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Get the bits
    *aPinsRead = ((*p_info->iPIDR) & aPortPins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_SetPull
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
T_uezError RX63N_GPIO_SetPull(
    void *aWorkspace,
    TUInt8 aPortPinIndex,
    T_gpioPull aPull)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

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
 * Routine:  RX63N_GPIO_SetMux
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
T_uezError RX63N_GPIO_SetMux(
    void *aWorkspace,
    TUInt8 aPortPinIndex,
    T_gpioMux aMux)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    if (aPortPinIndex >= 8)
        return UEZ_ERROR_INVALID_PARAMETER;
    if (aMux > 1)
        return UEZ_ERROR_INVALID_PARAMETER;

	if (!p_info->iPMR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

	// Set port mode register
    if (aMux == 0) {
		// Set the pin as a general I/O pin
        (*p_info->iPMR) &= ~(1 << aPortPinIndex);
		
    } else {
        // Set the pin as an I/O port for peripheral functions.
        (*p_info->iPMR) |= (1 << aPortPinIndex); 
    }
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_GPIO_GetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Get pin's mux feature.  1=peripheral pin, 0=generic GPIO pin
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-7
 *      T_gpioMux *aMux             -- Returned mux option (0=io pin, 1=peripheral)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_GPIO_GetMux(
    void *aWorkspace,
    TUInt8 aPortPinIndex,
    T_gpioMux *aMux)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    // Default is generic i/o
    *aMux = 0;

    if (aPortPinIndex >= 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    if (!p_info->iPMR)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Set port mode register
    if ((*p_info->iPMR) & (1 << aPortPinIndex)) {
        // Pin is for peripheral functions.
        *aMux = 1;
    } else {
        // Pin is for general I/O
        *aMux = 0;
    }

    return UEZ_ERROR_NONE;
}

T_uezError RX63N_GPIO_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
    const RX63N_GPIO_PInfo *p_info = p->iPortInfo;

    PARAM_NOT_USED(aValue);

	if (aPortPinIndex >= 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Look up the IOCON register
    switch (aControl) {
        case GPIO_NOP:
            // Yep, do no operation
            break;
        case GPIO_CONTROL_ENABLE_INPUT_BUFFER:
            // Not Implemented
            break;
        case GPIO_CONTROL_DISABLE_INPUT_BUFFER:
            // Not Implemented
            break;
		case GPIO_CONTROL_ENABLE_OPEN_DRAIN:
			if (!p_info->iODR0)
        		return UEZ_ERROR_ILLEGAL_OPERATION;
				
            (*p_info->iODR0) |= (1 << aPortPinIndex);
            break;
		case GPIO_CONTROL_DISABLE_OPEN_DRAIN:
			if (!p_info->iODR0)
        		return UEZ_ERROR_ILLEGAL_OPERATION;
				
            (*p_info->iODR0) &= ~(1 << aPortPinIndex);
            break;
		case GPIO_CONTROL_SET_CONFIG_BITS:
			// Not Implemented
			break;		
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
    }
    return UEZ_ERROR_NONE;
}

T_uezError RX63N_GPIO_ConfigureInterruptCallbackNotSupported(
    void *aWorkspace,
    T_gpioInterruptHandler iInterruptCallback,
    void *iInterruptCallbackWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX63N_GPIO_EnableInterruptsNotSupported(
    void *aWorkspace,
    TUInt32 aPortPins,
    T_gpioInterruptType aType)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX63N_GPIO_DisableInterruptsNotSupported(
    void *aWorkspace,
    TUInt32 aPortPins,
    T_gpioInterruptType aType)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX63N_GPIO_ClearInterruptsNotSupported(
    void *aWorkspace,
    TUInt32 aPortPins)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RX63N_GPIO_Lock(void *aWorkspace, TUInt32 aPortPins)
{
#ifndef NDEBUG
    char buffer [50];
#endif
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;
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

T_uezError RX63N_GPIO_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_RX63N_GPIO_Workspace *p = (T_RX63N_GPIO_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_RX63N_Port0_Interface = {
    "RX63N:GPIO:Port 0",
    0x0100,
    RX63N_GPIO_P0_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
	RX63N_GPIO_Unlock,
	/* uEZ v2.04 */
	RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port1_Interface = {
    "RX63N:GPIO:Port 1",
    0x0100,
    RX63N_GPIO_P1_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port2_Interface = {
    "RX63N:GPIO:Port 2",
    0x0100,
    RX63N_GPIO_P2_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port3_Interface = {
    "RX63N:GPIO:Port 3",
    0x0100,
    RX63N_GPIO_P3_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port4_Interface = {
    "RX63N:GPIO:Port 4",
    0x0100,
    RX63N_GPIO_P4_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port5_Interface = {
    "RX63N:GPIO:Port 5",
    0x0100,
    RX63N_GPIO_P5_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port6_Interface = {
    "RX63N:GPIO:Port 6",
    0x0100,
    RX63N_GPIO_P6_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port7_Interface = {
    "RX63N:GPIO:Port 7",
    0x0100,
    RX63N_GPIO_P7_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port8_Interface = {
    "RX63N:GPIO:Port 8",
    0x0100,
    RX63N_GPIO_P8_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_Port9_Interface = {
    "RX63N:GPIO:Port 9",
    0x0100,
    RX63N_GPIO_P9_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortA_Interface = {
    "RX63N:GPIO:Port A",
    0x0100,
    RX63N_GPIO_PA_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortB_Interface = {
    "RX63N:GPIO:Port B",
    0x0100,
    RX63N_GPIO_PB_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortC_Interface = {
    "RX63N:GPIO:Port C",
    0x0100,
    RX63N_GPIO_PC_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortD_Interface = {
    "RX63N:GPIO:Port D",
    0x0100,
    RX63N_GPIO_PD_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortE_Interface = {
    "RX63N:GPIO:Port E",
    0x0100,
    RX63N_GPIO_PE_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortF_Interface = {
    "RX63N:GPIO:Port F",
    0x0100,
    RX63N_GPIO_PF_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortG_Interface = {
    "RX63N:GPIO:Port G",
    0x0100,
    RX63N_GPIO_PG_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

const HAL_GPIOPort GPIO_RX63N_PortJ_Interface = {
    "RX63N:GPIO:Port J",
    0x0100,
    RX63N_GPIO_PG_InitializeWorkspace,
    sizeof(T_RX63N_GPIO_Workspace),

    0,
    0,
    0,
    RX63N_GPIO_SetOutputMode,
    RX63N_GPIO_SetInputMode,
    RX63N_GPIO_Set,
    RX63N_GPIO_Clear,
    RX63N_GPIO_Read,
    RX63N_GPIO_SetPull,
    RX63N_GPIO_SetMux,

    RX63N_GPIO_ConfigureInterruptCallbackNotSupported,
    RX63N_GPIO_EnableInterruptsNotSupported,
    RX63N_GPIO_DisableInterruptsNotSupported,
    RX63N_GPIO_ClearInterruptsNotSupported,

    RX63N_GPIO_Control,
	RX63N_GPIO_Lock,
    RX63N_GPIO_Unlock,
    /* uEZ v2.04 */
    RX63N_GPIO_GetMux,
};

void RX63N_GPIO_P0_Require(void)
{
    HAL_GPIOPort **p_gpio0;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO0",
            (T_halInterface *)&GPIO_RX63N_Port0_Interface, 0,
            (T_halWorkspace **)&p_gpio0);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_0, p_gpio0);
}

void RX63N_GPIO_P1_Require(void)
{
    HAL_GPIOPort **p_gpio1;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO1",
            (T_halInterface *)&GPIO_RX63N_Port1_Interface, 0,
            (T_halWorkspace **)&p_gpio1);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_1, p_gpio1);
}

void RX63N_GPIO_P2_Require(void)
{
    HAL_GPIOPort **p_gpio2;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO2",
            (T_halInterface *)&GPIO_RX63N_Port2_Interface, 0,
            (T_halWorkspace **)&p_gpio2);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_2, p_gpio2);
}

void RX63N_GPIO_P3_Require(void)
{
    HAL_GPIOPort **p_gpio3;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO3",
            (T_halInterface *)&GPIO_RX63N_Port3_Interface, 0,
            (T_halWorkspace **)&p_gpio3);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_3, p_gpio3);
}

void RX63N_GPIO_P4_Require(void)
{
    HAL_GPIOPort **p_gpio4;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO4",
            (T_halInterface *)&GPIO_RX63N_Port4_Interface, 0,
            (T_halWorkspace **)&p_gpio4);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_4, p_gpio4);
}

void RX63N_GPIO_P5_Require(void)
{
    HAL_GPIOPort **p_gpio5;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO5",
            (T_halInterface *)&GPIO_RX63N_Port5_Interface, 0,
            (T_halWorkspace **)&p_gpio5);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_5, p_gpio5);
}

void RX63N_GPIO_P6_Require(void)
{
    HAL_GPIOPort **p_gpio6;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO6",
            (T_halInterface *)&GPIO_RX63N_Port6_Interface, 0,
            (T_halWorkspace **)&p_gpio6);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_6, p_gpio6);
}

void RX63N_GPIO_P7_Require(void)
{
    HAL_GPIOPort **p_gpio7;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO7",
            (T_halInterface *)&GPIO_RX63N_Port7_Interface, 0,
            (T_halWorkspace **)&p_gpio7);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_7, p_gpio7);
}

void RX63N_GPIO_P8_Require(void)
{
    HAL_GPIOPort **p_gpio8;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO8",
            (T_halInterface *)&GPIO_RX63N_Port8_Interface, 0,
            (T_halWorkspace **)&p_gpio8);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_8, p_gpio8);
}

void RX63N_GPIO_P9_Require(void)
{
    HAL_GPIOPort **p_gpio9;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO9",
            (T_halInterface *)&GPIO_RX63N_Port9_Interface, 0,
            (T_halWorkspace **)&p_gpio9);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_9, p_gpio9);
}

void RX63N_GPIO_PA_Require(void)
{
    HAL_GPIOPort **p_gpioA;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOA",
            (T_halInterface *)&GPIO_RX63N_PortA_Interface, 0,
            (T_halWorkspace **)&p_gpioA);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_A, p_gpioA);
}

void RX63N_GPIO_PB_Require(void)
{
    HAL_GPIOPort **p_gpioB;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOB",
            (T_halInterface *)&GPIO_RX63N_PortB_Interface, 0,
            (T_halWorkspace **)&p_gpioB);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_B, p_gpioB);
}

void RX63N_GPIO_PC_Require(void)
{
    HAL_GPIOPort **p_gpioC;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOC",
            (T_halInterface *)&GPIO_RX63N_PortC_Interface, 0,
            (T_halWorkspace **)&p_gpioC);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_C, p_gpioC);
}

void RX63N_GPIO_PD_Require(void)
{
    HAL_GPIOPort **p_gpioD;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOD",
            (T_halInterface *)&GPIO_RX63N_PortD_Interface, 0,
            (T_halWorkspace **)&p_gpioD);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_D, p_gpioD);
}

void RX63N_GPIO_PE_Require(void)
{
    HAL_GPIOPort **p_gpioE;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOE",
            (T_halInterface *)&GPIO_RX63N_PortE_Interface, 0,
            (T_halWorkspace **)&p_gpioE);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_E, p_gpioE);
}

void RX63N_GPIO_PF_Require(void)
{
    HAL_GPIOPort **p_gpioF;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOF",
            (T_halInterface *)&GPIO_RX63N_PortF_Interface, 0,
            (T_halWorkspace **)&p_gpioF);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_F, p_gpioF);
}

void RX63N_GPIO_PG_Require(void)
{
    HAL_GPIOPort **p_gpioG;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOG",
            (T_halInterface *)&GPIO_RX63N_PortG_Interface, 0,
            (T_halWorkspace **)&p_gpioG);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_G, p_gpioG);
}

void RX63N_GPIO_PJ_Require(void)
{
    HAL_GPIOPort **p_gpioJ;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIOJ",
            (T_halInterface *)&GPIO_RX63N_PortJ_Interface, 0,
            (T_halWorkspace **)&p_gpioJ);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_J, p_gpioJ);
}

/*-------------------------------------------------------------------------*
 * End of File:  GPIO.c
 *-------------------------------------------------------------------------*/
