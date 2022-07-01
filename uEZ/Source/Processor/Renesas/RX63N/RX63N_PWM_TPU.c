/*-------------------------------------------------------------------------*
 * File:  RX63N_PWM_TPU.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX63N PWM Interface using the 
 *		16-Bit Timer Pulse Unit (TPU). 
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
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/PWM.h>
#include <HAL/GPIO.h>
#include <HAL/Interrupt.h>
#include "RX63N_UtilityFuncs.h"
#include "RX63N_PWM_TPU.h"

/*---------------------------------------------------------------------------*
 * Defines:
 *---------------------------------------------------------------------------*/
#define TSYRA	TPUA.TSYR.BYTE
#define TSYRB	TPUB.TSYR.BYTE
#define TSTRA	TPUA.TSTR.BYTE
#define TSTRB	TPUB.TSTR.BYTE

#define MSTP_TPU_UNIT0_BIT	13
#define MSTP_TPU_UNIT1_BIT	12

#define CST0_BIT	0
#define CST1_BIT	1
#define CST2_BIT	2
#define CST3_BIT	6
#define CST4_BIT	7

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
				 TUInt8		iTPU_Idx;	// TPU Index 0-10
				 TUInt8 	iMSTPBit;	// MSTP bit number
    __evenaccess TVUInt8 	*iTCR;		// Timer Control Reg
				 TUInt8		iNumOutputs;// Number of TPU outputs. (2 or 4)
    __evenaccess TVUInt8 	*iTMDR;		// Timer Mode Reg
    __evenaccess TVUInt8 	*iTIORH;	// Timer I/O Control Reg (IOA & IOB)
    __evenaccess TVUInt8 	*iTIORL;	// Timer I/O Control Reg (IOC & IOD)
	__evenaccess TVUInt8 	*iTIER;		// Interrupt Enable Reg
	__evenaccess TVUInt16	*iTGR[4];		
	__evenaccess TVUInt8 	*iTSTR;		// Timer Start Reg (TSTRA or TSTRB)
				 TUInt8		iCSTBit;	// Start Bit (CSTn_BIT)
	__evenaccess TVUInt8 	*iTSYR;		// Sync Reg (TSYRA or TSYRB)
				 TUInt8		iVECT[4];
				 const char *iVectName[4];
				 TFPtr      iVectISR[4];
} T_RX63N_TPU_Info;

typedef void (*T_TPUCallback)(void *aCallbackWorkspace);

typedef struct {
    const HAL_PWM *iHAL;
    TUInt32 iPeriod;
    const T_RX63N_TPU_Info *iInfo;
	T_TPUCallback iMatchCallback[4];
	void *iMatchCallbackWorkspace[4];
	T_irqPriority iISRPriority;
} T_RX63N_TPU_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_RX63N_TPU_Workspace *G_workspaceTPU0 = 0;
extern const T_RX63N_TPU_Info RX63N_TPU0_Info;

T_RX63N_TPU_Workspace *G_workspaceTPU1 = 0;
extern const T_RX63N_TPU_Info RX63N_TPU1_Info;


/*---------------------------------------------------------------------------*
 * Interrupt Routines:
 *---------------------------------------------------------------------------*/
void TPUCallback(T_RX63N_TPU_Workspace *p, TUInt8 aIndex)
{
    p->iMatchCallback[aIndex](p->iMatchCallbackWorkspace[aIndex]);
}

//	TPU0 Interrupt Routines
IRQ_ROUTINE(TPU0A_ISR)
{
    IRQ_START();
    TPUCallback(G_workspaceTPU0, 0);
    IRQ_END();
}
IRQ_ROUTINE(TPU0B_ISR)
{
    IRQ_START();
    TPUCallback(G_workspaceTPU0, 1);
    IRQ_END();
}
IRQ_ROUTINE(TPU0C_ISR)
{
    IRQ_START();
    TPUCallback(G_workspaceTPU0, 2);
    IRQ_END();
}
IRQ_ROUTINE(TPU0D_ISR)
{
    IRQ_START();
    TPUCallback(G_workspaceTPU0, 3);
    IRQ_END();
}

//	TPU1 Interrupt Routines
IRQ_ROUTINE(TPU1A_ISR)
{
    IRQ_START();
    TPUCallback(G_workspaceTPU1, 0);
    IRQ_END();
}
IRQ_ROUTINE(TPU1B_ISR)
{
    IRQ_START();
    TPUCallback(G_workspaceTPU1, 1);
    IRQ_END();
}


/*---------------------------------------------------------------------------*
 * Routine:  RX63N_PWM_TPU_SetMaster
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to start -- chip select enabled for device
 * Outputs:
 *      void *aWorkspace        -- Workspace for PWM
 *      TUInt32 aPeriod         -- Number of Fpclk intervals per period
 *      TUInt8 aMatchRegister   -- Which register is used for master counter
 *---------------------------------------------------------------------------*/
static T_uezError RX63N_PWM_TPU_SetMaster(
        void *aWorkspace, 
        TUInt32 aPeriod,
        TUInt8 aMatchRegister)
{
    T_RX63N_TPU_Workspace *p = (T_RX63N_TPU_Workspace *)aWorkspace;
    const T_RX63N_TPU_Info *p_info = p->iInfo;

	if(aMatchRegister >= p_info->iNumOutputs)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;

    // Ensure TPU is on
	SYSTEM.MSTPCRA.LONG &= ~(1<<p_info->iMSTPBit);	// MSTP(TPUx) = 0;    
    
    // Ensure it is stopped
    *p_info->iTSTR &= ~(1<<p_info->iCSTBit); 	// CST = 0;

	// Set clear control bit to this match register
	if(aMatchRegister == 0)
		*p_info->iTCR = 1 << 5;	
	else if(aMatchRegister == 1)
		*p_info->iTCR = 2 << 5;
	else if(aMatchRegister == 2)
		*p_info->iTCR = 5 << 5;
	else if(aMatchRegister == 3)
		*p_info->iTCR = 6 << 5;
	
	*p_info->iTCR |= 0 << 3; 		// Clock on rising edge
	*p_info->iTCR |= 1 << 0,		// PCLK/4, for now
	    
    *p_info->iTMDR = 3 << 0;		// PWM mode 2

	aPeriod /= 2; // Adjust Fcclk to Fpclk
	
	// Set Period
	*p_info->iTGR[aMatchRegister] = (aPeriod/4);
	
	// Configure Interrupt if Needed
	if( p->iMatchCallback[aMatchRegister] &&
		!InterruptIsRegistered(p_info->iVECT[aMatchRegister]) )
	{
		InterruptRegister(
			p_info->iVECT[aMatchRegister],
			p_info->iVectISR[aMatchRegister],
			p->iISRPriority,
        	p_info->iVectName[aMatchRegister]);
			
		*p_info->iTIER |= 1 << aMatchRegister;
	}
	
	// Set start bit
	*p_info->iTSTR |= (1<<p_info->iCSTBit); 	// CST = 1;
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_PWM_TPU_SetMatchRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure one of the several match registers for this PWM bank.
 * Outputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Index to match register (0-7)
 *      TUInt32 aMatchPoint     -- Number of PWM cycles until match
 *      TBool aDoInterrupt      -- ETrue if want an interrupt, else EFalse
 *                                  (NOTE: Interrupts currently not 
 *                                  implemented)
 *      TBool aDoCounterReset   -- ETrue if counter for this PWM bank is
 *                                  to be reset on match.
 *      TBool aDoStop           -- ETrue if counter is to be stopped
 *                                  when match occurs.
 *---------------------------------------------------------------------------*/
static T_uezError RX63N_PWM_TPU_SetMatchRegister(
        void *aWorkspace, 
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{	
	T_RX63N_TPU_Workspace *p = (T_RX63N_TPU_Workspace *)aWorkspace;
    const T_RX63N_TPU_Info *p_info = p->iInfo;
	
	if(aMatchRegister >= p_info->iNumOutputs)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;
	
	aMatchPoint /= 2; // Adjust Fcclk to Fpclk
	
	*p_info->iTGR[aMatchRegister] = (aMatchPoint/4);
	
	if( p->iMatchCallback[aMatchRegister] &&
		!InterruptIsRegistered(p_info->iVECT[aMatchRegister]) )
	{
		InterruptRegister(
			p_info->iVECT[aMatchRegister],
			p_info->iVectISR[aMatchRegister],
			p->iISRPriority,
        	p_info->iVectName[aMatchRegister]);
			
		*p_info->iTIER |= 1 << aMatchRegister;
	}
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_PWM_TPU_EnableSingleEdgeOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where PWM switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError RX63N_PWM_TPU_EnableSingleEdgeOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_RX63N_TPU_Workspace *p = (T_RX63N_TPU_Workspace *)aWorkspace;
    const T_RX63N_TPU_Info *p_info = p->iInfo;
	TUInt8 i;
	
	if(aMatchRegister >= p_info->iNumOutputs)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;

	// This needs to be changed once there is a RX63N timer driver
	// - for now, if a callback is set, output is software controlled
	if( p->iMatchCallback[aMatchRegister] &&
		!InterruptIsRegistered(p_info->iVECT[aMatchRegister]) )
	{
		InterruptRegister(
			p_info->iVECT[aMatchRegister],
			p_info->iVectISR[aMatchRegister],
			p->iISRPriority,
        	p_info->iVectName[aMatchRegister]);
			
		*p_info->iTIER |= 1 << aMatchRegister;
	}
	
	if(!InterruptIsRegistered(p_info->iVECT[aMatchRegister]))
	{
		if(aMatchRegister == 0)
			*p_info->iTIORH |= 5;
		else if(aMatchRegister == 1)
			*p_info->iTIORH |= (5<<4);
		else if(aMatchRegister == 2)
			*p_info->iTIORL |= 5;
		else if(aMatchRegister == 3)
			*p_info->iTIORL |= (5<<4);			
	}
	else
	{
		// Enable any registered interrupts
		for(i=0; i<p_info->iNumOutputs; i++)
		{
			if(InterruptIsRegistered(p_info->iVECT[i]))
				InterruptEnable(p_info->iVECT[i]);
		}
	}

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_PWM_TPU_DisableOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where PWM switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError RX63N_PWM_TPU_DisableOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_RX63N_TPU_Workspace *p = (T_RX63N_TPU_Workspace *)aWorkspace;
	const T_RX63N_TPU_Info *p_info = p->iInfo;
	TUInt8 i;
	
	if(aMatchRegister >= p_info->iNumOutputs)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;
	
	if(!InterruptIsRegistered(p_info->iVECT[aMatchRegister]))
	{
		if(aMatchRegister == 0)
			*p_info->iTIORH &= ~5;
		else if(aMatchRegister == 1)
			*p_info->iTIORH &= ~(1<<5);
		else if(aMatchRegister == 2)
			*p_info->iTIORL &= ~5;
		else if(aMatchRegister == 3)
			*p_info->iTIORL &= ~(1<<5);			
	}
	else
	{
		// Enable any registered interrupts
		for(i=0; i<p_info->iNumOutputs; i++)
		{
			if(InterruptIsRegistered(p_info->iVECT[i]))
				InterruptDisable(p_info->iVECT[i]);
		}
	}

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_PWM_TPU_SetMatchCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the callback function for a Match Register
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *		TUInt8 aMatchRegister	-- The match register to attach callback
 *      void *aCallback   		-- Pointer to the callback method
 *      void *aCallbackWorkspace-- Workspace for this callback
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError RX63N_PWM_TPU_SetMatchCallback(
        void *aWorkspace,
        TUInt8 aMatchRegister,
		void (*aCallback)(void *),
		void *aCallbackWorkspace)
{
    T_RX63N_TPU_Workspace *p = (T_RX63N_TPU_Workspace *)aWorkspace;
	
	if(aMatchRegister < 4) {
		p->iMatchCallback[aMatchRegister] = aCallback;
        p->iMatchCallbackWorkspace[aMatchRegister] = aCallbackWorkspace;
	} else {
		return UEZ_ERROR_ILLEGAL_ARGUMENT;
	}
		
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_PWM_TPU_TPUx_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a RX63N PWM TPU for the given global workspace and info table.
 *		This routine is designed to be used by the specific TPU0-TPU10 initial
 *		routines below.
 * Inputs:
 *      void *aW                    -- Particular PWM workspace
 *		T_RX63N_TPU_Workspace *aWorkspaceTPUx
 *									-- pointer to a global RX63N TPU workspace
 *		RX63N_TPU0_Info *aTPUx_Info -- pointer to a global TPU into table
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_PWM_TPUx_InitializeWorkspace(
	void *aWorkspace,
	const T_RX63N_TPU_Info *aTPUx_Info)
{
	T_RX63N_TPU_Workspace *p = (T_RX63N_TPU_Workspace *)aWorkspace;
	TUInt8 i;

	// Setup the connecting between workspace and info block
    p->iInfo = aTPUx_Info;
    p->iISRPriority = INTERRUPT_PRIORITY_HIGH;

    // Ensure the callbacks are off
	for(i=0; i < aTPUx_Info->iNumOutputs; i++) {
		p->iMatchCallback[i] = 0;
        p->iMatchCallbackWorkspace[i] = 0;
	}

    return UEZ_ERROR_NONE;
}

T_uezError RX63N_PWM_TPUx_Configure(void *aWorkspace, T_irqPriority aPriority)
{
    T_RX63N_TPU_Workspace *p = (T_RX63N_TPU_Workspace *)aWorkspace;
    p->iISRPriority = aPriority;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Initialize Workspace Routines
 *---------------------------------------------------------------------------*/
T_uezError RX63N_PWM_TPU0_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceTPU0 = aWorkspace;
    return RX63N_PWM_TPUx_InitializeWorkspace(
		aWorkspace,
		&RX63N_TPU0_Info);
}

T_uezError RX63N_PWM_TPU1_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceTPU1 = aWorkspace;
    return RX63N_PWM_TPUx_InitializeWorkspace(
		aWorkspace,
		&RX63N_TPU1_Info);
}

/*---------------------------------------------------------------------------*
 * HAL Interfaces
 *---------------------------------------------------------------------------*/
const HAL_PWM PWM_RX63N_TPU0_Interface = {
    "PWM:RX63N:TPU0",
    0x0100,
    RX63N_PWM_TPU0_InitializeWorkspace,
    sizeof(T_RX63N_TPU_Workspace),

    RX63N_PWM_TPU_SetMaster,
    RX63N_PWM_TPU_SetMatchRegister,
    RX63N_PWM_TPU_EnableSingleEdgeOutput,
    RX63N_PWM_TPU_DisableOutput,
	RX63N_PWM_TPU_SetMatchCallback,
};

const HAL_PWM PWM_RX63N_TPU1_Interface = {
    "PWM:RX63N:TPU1",
    0x0100,
    RX63N_PWM_TPU1_InitializeWorkspace,
    sizeof(T_RX63N_TPU_Workspace),

    RX63N_PWM_TPU_SetMaster,
    RX63N_PWM_TPU_SetMatchRegister,
    RX63N_PWM_TPU_EnableSingleEdgeOutput,
    RX63N_PWM_TPU_DisableOutput,
	RX63N_PWM_TPU_SetMatchCallback,
};

/*---------------------------------------------------------------------------*
 * Info Tables
 *---------------------------------------------------------------------------*/
const T_RX63N_TPU_Info RX63N_TPU0_Info = {
		0,
		MSTP_TPU_UNIT0_BIT,
        &TPU0.TCR.BYTE,
		4,	// Num Outputs
        &TPU0.TMDR.BYTE,
        &TPU0.TIORH.BYTE,
        &TPU0.TIORL.BYTE,
		&TPU0.TIER.BYTE,
		{&TPU0.TGRA, &TPU0.TGRB, &TPU0.TGRC, &TPU0.TGRD},
		&TSTRA,
		CST0_BIT,
		&TSYRA,
		{VECT_TPU0_TGI0A, VECT_TPU0_TGI0B, VECT_TPU0_TGI0C, VECT_TPU0_TGI0D},
		{"TPU0A", "TPU0B","TPU0C","TPU0D"},
		{TPU0A_ISR, TPU0B_ISR, TPU0C_ISR, TPU0D_ISR},
};

const T_RX63N_TPU_Info RX63N_TPU1_Info = {
		1,
		MSTP_TPU_UNIT0_BIT,
        &TPU1.TCR.BYTE,
		2,	// Num Outputs
        &TPU1.TMDR.BYTE,
        &TPU1.TIOR.BYTE,
        0,
		&TPU1.TIER.BYTE,
		{&TPU1.TGRA, &TPU1.TGRB, 0, 0},
		&TSTRA,
		CST1_BIT,
		&TSYRA,
		{VECT_TPU1_TGI1A, VECT_TPU1_TGI1B, 0, 0},
		{"TPU1A", "TPU1B", 0, 0},
		{TPU1A_ISR, TPU1B_ISR, 0, 0},
};

void RX63N_PWM_TPU0_Require(T_uezGPIOPortPin aOutputPin)
{
	static const T_RX63N_MPC_ConfigList pinList[] = {
            {GPIO_P86, 0x03},	// TPIOCA0
			{GPIO_PA0, 0x03},	// TPIOCA0
			{GPIO_PA1, 0x03},	// TPIOCB0
			{GPIO_P17, 0x03},	// TPIOCB0
			{GPIO_P32, 0x03},	// TPIOCC0
			{GPIO_P33, 0x03},	// TPIOCD0
			{GPIO_PA3, 0x03},	// TPIOCD0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_TPU0", (T_halInterface *)&PWM_RX63N_TPU0_Interface, 0, 0);
	
	RX63N_MPC_ConfigPinOrNone(aOutputPin, pinList, ARRAY_COUNT(pinList));
}

void RX63N_PWM_TPU1_Require(T_uezGPIOPortPin aOutputPin)
{
	static const T_RX63N_MPC_ConfigList pinList[] = {
            {GPIO_P56, 0x03},	// TPIOCA1
			{GPIO_PA4, 0x03},	// TPIOCA1
			{GPIO_PA5, 0x03},	// TPIOCB1
			{GPIO_P16, 0x03},	// TPIOCB1
    };
	
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_TPU1", (T_halInterface *)&PWM_RX63N_TPU1_Interface, 0, 0);
	
	RX63N_MPC_ConfigPinOrNone(aOutputPin, pinList, ARRAY_COUNT(pinList));
}

/*-------------------------------------------------------------------------*
 * End of File:  RX63N_PWM_TPU.c
 *-------------------------------------------------------------------------*/

