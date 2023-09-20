/*-------------------------------------------------------------------------*
 * File:  RX62N_PWM_MTU.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX62N PWM Interface using the 
 *		Multi-Function Timer Pulse Unit 2 (MTU2).
 *
 * Implementation:
 *      All MTUs are included accept MTU5 in this driver due to different
 *		Functionality.
 *		
 *		Each MTU can be enabled by it's config macro, UEZ_ENABLE_PWM_MTUx,
 *		where x is the MTU channel
 *
 *		If a callback methods are defined, PWM output is automatically  
 *		disabled and the callback methods are responsible for all output
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
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/PWM.h>
#include <HAL/GPIO.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/Renesas/RX62N/RXToolset/iodefine.h>
#include "RX62N_PWM_MTU.h"

/*---------------------------------------------------------------------------*
 * Defines:
 *---------------------------------------------------------------------------*/
#define TSYRA	MTUA.TSYR.BYTE
#define TSYRB	MTUB.TSYR.BYTE
#define TSTRA	MTUA.TSTR.BYTE
#define TSTRB	MTUB.TSTR.BYTE

#define MSTP_MTUA_BIT	9
#define MSTP_MTUB_BIT	8

#define CST0_BIT	0
#define CST1_BIT	1
#define CST2_BIT	2
#define CST3_BIT	6
#define CST4_BIT	7

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
				 TUInt8		iMTU_Idx;	// MTU Index 0-10
				 TUInt8 	iMSTPBit;	// MSTP bit number
    __evenaccess TVUInt8 	*iTCR;		// Timer Control Reg
				 TUInt8		iNumOutputs;// Number of MTU outputs. (2 or 4)
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
} T_RX62N_MTU_Info;

typedef void (*T_MTUCallback)(void *aCallbackWorkspace);

typedef struct {
    const HAL_PWM *iHAL;
    TUInt32 iPeriod;
    const T_RX62N_MTU_Info *iInfo;
	T_MTUCallback iMatchCallback[4];
	void *iMatchCallbackWorkspace[4];
	T_irqPriority iISRPriority;
} T_RX62N_MTU_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_RX62N_MTU_Workspace *G_workspaceMTU0 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU0_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU1 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU1_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU2 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU2_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU3 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU3_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU4 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU4_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU6 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU6_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU7 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU7_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU8 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU8_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU9 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU9_Info;

T_RX62N_MTU_Workspace *G_workspaceMTU10 = 0;
extern const T_RX62N_MTU_Info RX62N_MTU10_Info;


/*---------------------------------------------------------------------------*
 * Interrupt Routines:
 *---------------------------------------------------------------------------*/
void MTUCallback(T_RX62N_MTU_Workspace *p, TUInt8 aIndex)
{
    p->iMatchCallback[aIndex](p->iMatchCallbackWorkspace[aIndex]);
}

//	MTU0 Interrupt Routines
IRQ_ROUTINE(MTU0A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU0, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU0B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU0, 1);
    IRQ_END();
}
IRQ_ROUTINE(MTU0C_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU0, 2);
    IRQ_END();
}
IRQ_ROUTINE(MTU0D_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU0, 3);
    IRQ_END();
}

//	MTU1 Interrupt Routines
IRQ_ROUTINE(MTU1A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU1, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU1B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU1, 1);
    IRQ_END();
}

//	MTU2 Interrupt Routines
IRQ_ROUTINE(MTU2A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU2, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU2B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU2, 1);
    IRQ_END();
}

//	MTU3 Interrupt Routines
IRQ_ROUTINE(MTU3A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU3, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU3B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU3, 1);
    IRQ_END();
}
IRQ_ROUTINE(MTU3C_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU3, 2);
    IRQ_END();
}
IRQ_ROUTINE(MTU3D_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU3, 3);
    IRQ_END();
}

//	MTU4 Interrupt Routines
IRQ_ROUTINE(MTU4A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU4, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU4B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU4, 1);
    IRQ_END();
}
IRQ_ROUTINE(MTU4C_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU4, 2);
    IRQ_END();
}
IRQ_ROUTINE(MTU4D_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU4, 3);
    IRQ_END();
}

//	MTU6 Interrupt Routines
IRQ_ROUTINE(MTU6A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU6, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU6B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU6, 1);
    IRQ_END();
}
IRQ_ROUTINE(MTU6C_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU6, 2);
    IRQ_END();
}
IRQ_ROUTINE(MTU6D_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU6, 3);
    IRQ_END();
}

//	MTU7 Interrupt Routines
IRQ_ROUTINE(MTU7A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU7, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU7B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU7, 1);
    IRQ_END();
}

//	MTU8 Interrupt Routines
IRQ_ROUTINE(MTU8A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU8, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU8B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU8, 1);
    IRQ_END();
}

//	MTU9 Interrupt Routines
IRQ_ROUTINE(MTU9A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU9, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU9B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU9, 1);
    IRQ_END();
}
IRQ_ROUTINE(MTU9C_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU9, 2);
    IRQ_END();
}
IRQ_ROUTINE(MTU9D_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU9, 3);
    IRQ_END();
}

//	MTU10 Interrupt Routines
IRQ_ROUTINE(MTU10A_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU10, 0);
    IRQ_END();
}
IRQ_ROUTINE(MTU10B_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU10, 1);
    IRQ_END();
}
IRQ_ROUTINE(MTU10C_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU10, 2);
    IRQ_END();
}
IRQ_ROUTINE(MTU10D_ISR)
{
    IRQ_START();
    MTUCallback(G_workspaceMTU10, 3);
    IRQ_END();
}


/*---------------------------------------------------------------------------*
 * Routine:  RX62N_PWM_MTU_SetMaster
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to start -- chip select enabled for device
 * Outputs:
 *      void *aWorkspace        -- Workspace for PWM
 *      TUInt32 aPeriod         -- Number of Fpclk intervals per period
 *      TUInt8 aMatchRegister   -- Which register is used for master counter
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_PWM_MTU_SetMaster(
        void *aWorkspace, 
        TUInt32 aPeriod,
        TUInt8 aMatchRegister)
{
    T_RX62N_MTU_Workspace *p = (T_RX62N_MTU_Workspace *)aWorkspace;
    const T_RX62N_MTU_Info *p_info = p->iInfo;

	if(aMatchRegister >= p_info->iNumOutputs)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;

    // Ensure MTU is on
	SYSTEM.MSTPCRA.LONG &= ~(1<<p_info->iMSTPBit);	// MSTP(MTUx) = 0;    
    
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
	    
    *p_info->iTMDR = 2 << 0;		// PWM mode 1: TMDR.MD = 2

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
 * Routine:  RX62N_PWM_MTU_SetMatchRegister
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
static T_uezError RX62N_PWM_MTU_SetMatchRegister(
        void *aWorkspace, 
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{	
	T_RX62N_MTU_Workspace *p = (T_RX62N_MTU_Workspace *)aWorkspace;
    const T_RX62N_MTU_Info *p_info = p->iInfo;
	
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
 * Routine:  RX62N_PWM_MTU_EnableSingleEdgeOutput
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
static T_uezError RX62N_PWM_MTU_EnableSingleEdgeOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_RX62N_MTU_Workspace *p = (T_RX62N_MTU_Workspace *)aWorkspace;
    const T_RX62N_MTU_Info *p_info = p->iInfo;
	TUInt8 i;
	
	if(aMatchRegister >= p_info->iNumOutputs)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;

	// This needs to be changed once there is a rx62n timer driver
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
			*p_info->iTIORH |= (1<<5);
		else if(aMatchRegister == 2)
			*p_info->iTIORL |= 5;
		else if(aMatchRegister == 3)
			*p_info->iTIORL |= (1<<5);			
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
 * Routine:  RX62N_PWM_MTU_DisableOutput
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
static T_uezError RX62N_PWM_MTU_DisableOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_RX62N_MTU_Workspace *p = (T_RX62N_MTU_Workspace *)aWorkspace;
	const T_RX62N_MTU_Info *p_info = p->iInfo;
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
 * Routine:  RX62N_PWM_MTU_SetMatchCallback
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
static T_uezError RX62N_PWM_MTU_SetMatchCallback(
        void *aWorkspace,
        TUInt8 aMatchRegister,
		void (*aCallback)(void *),
		void *aCallbackWorkspace)
{
    T_RX62N_MTU_Workspace *p = (T_RX62N_MTU_Workspace *)aWorkspace;
	
	if(aMatchRegister < 4) {
		p->iMatchCallback[aMatchRegister] = aCallback;
        p->iMatchCallbackWorkspace[aMatchRegister] = aCallbackWorkspace;
	} else {
		return UEZ_ERROR_ILLEGAL_ARGUMENT;
	}
		
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_PWM_MTU_MTUx_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a RX62N PWM MTU for the given global workspace and info table.
 *		This routine is designed to be used by the specific MTU0-MTU10 initial
 *		routines below.
 * Inputs:
 *      void *aW                    -- Particular PWM workspace
 *		T_RX62N_MTU_Workspace *aWorkspaceMTUx
 *									-- pointer to a global RX62N MTU workspace
 *		RX62N_MTU0_Info *aMTUx_Info -- pointer to a global MTU into table
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_PWM_MTUx_InitializeWorkspace(
	void *aWorkspace,
	const T_RX62N_MTU_Info *aMTUx_Info)
{
	T_RX62N_MTU_Workspace *p = (T_RX62N_MTU_Workspace *)aWorkspace;
	TUInt8 i;

	// Setup the connecting between workspace and info block
    p->iInfo = aMTUx_Info;
    p->iISRPriority = INTERRUPT_PRIORITY_HIGH;

    // Ensure the callbacks are off
	for(i=0; i < aMTUx_Info->iNumOutputs; i++) {
		p->iMatchCallback[i] = 0;
        p->iMatchCallbackWorkspace[i] = 0;
	}

    return UEZ_ERROR_NONE;
}

T_uezError RX62N_PWM_MTUx_Configure(void *aWorkspace, T_irqPriority aPriority)
{
    T_RX62N_MTU_Workspace *p = (T_RX62N_MTU_Workspace *)aWorkspace;
    p->iISRPriority = aPriority;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Initialize Workspace Routines
 *---------------------------------------------------------------------------*/
T_uezError RX62N_PWM_MTU0_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU1 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU0_Info);
}

T_uezError RX62N_PWM_MTU1_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU1 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU1_Info);
}

T_uezError RX62N_PWM_MTU2_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU2 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU2_Info);
}

T_uezError RX62N_PWM_MTU3_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU3 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU3_Info);
}

T_uezError RX62N_PWM_MTU4_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU4 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU4_Info);
}

T_uezError RX62N_PWM_MTU6_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU6 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU6_Info);
}

T_uezError RX62N_PWM_MTU7_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU7 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU7_Info);
}

T_uezError RX62N_PWM_MTU8_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU8 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU8_Info);
}

T_uezError RX62N_PWM_MTU9_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU9 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU9_Info);
}

T_uezError RX62N_PWM_MTU10_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceMTU10 = aWorkspace;
    return RX62N_PWM_MTUx_InitializeWorkspace(
		aWorkspace,
		&RX62N_MTU10_Info);
}

/*---------------------------------------------------------------------------*
 * HAL Interfaces
 *---------------------------------------------------------------------------*/
const HAL_PWM PWM_RX62N_MTU0_Interface = {
    "PWM:RX62N:MTU0",
    0x0100,
    RX62N_PWM_MTU0_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU1_Interface = {
    "PWM:RX62N:MTU1",
    0x0100,
    RX62N_PWM_MTU1_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU2_Interface = {
    "PWM:RX62N:MTU2",
    0x0100,
    RX62N_PWM_MTU2_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU3_Interface = {
    "PWM:RX62N:MTU3",
    0x0100,
    RX62N_PWM_MTU3_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU4_Interface = {
    "PWM:RX62N:MTU4",
    0x0100,
    RX62N_PWM_MTU4_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU6_Interface = {
    "PWM:RX62N:MTU6",
    0x0100,
    RX62N_PWM_MTU6_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU7_Interface = {
    "PWM:RX62N:MTU7",
    0x0100,
    RX62N_PWM_MTU7_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU8_Interface = {
    "PWM:RX62N:MTU8",
    0x0100,
    RX62N_PWM_MTU8_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU9_Interface = {
    "PWM:RX62N:MTU9",
    0x0100,
    RX62N_PWM_MTU9_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

const HAL_PWM PWM_RX62N_MTU10_Interface = {
    "PWM:RX62N:MTU10",
    0x0100,
    RX62N_PWM_MTU10_InitializeWorkspace,
    sizeof(T_RX62N_MTU_Workspace),

    RX62N_PWM_MTU_SetMaster,
    RX62N_PWM_MTU_SetMatchRegister,
    RX62N_PWM_MTU_EnableSingleEdgeOutput,
    RX62N_PWM_MTU_DisableOutput,
	RX62N_PWM_MTU_SetMatchCallback,
};

/*---------------------------------------------------------------------------*
 * Info Tables
 *---------------------------------------------------------------------------*/
const T_RX62N_MTU_Info RX62N_MTU0_Info = {
		0,
		MSTP_MTUA_BIT,
        &MTU0.TCR.BYTE,
		4,	// Num Outputs
        &MTU0.TMDR.BYTE,
        &MTU0.TIORH.BYTE,
        &MTU0.TIORL.BYTE,
		&MTU0.TIER.BYTE,
		{&MTU0.TGRA, &MTU0.TGRB, &MTU0.TGRC, &MTU0.TGRD},
		&TSTRA,
		CST0_BIT,
		&TSYRA,
		{IR_MTU0_TGIA0, IR_MTU0_TGIB0, IR_MTU0_TGIC0, IR_MTU0_TGID0},
		{"MTU0A", "MTU0B","MTU0C","MTU0D"},
		{MTU0A_ISR, MTU0B_ISR, MTU0C_ISR, MTU0D_ISR},
};

const T_RX62N_MTU_Info RX62N_MTU1_Info = {
		1,
		MSTP_MTUA_BIT,
        &MTU1.TCR.BYTE,
		2,	// Num Outputs
        &MTU1.TMDR.BYTE,
        &MTU1.TIOR.BYTE,
        0,
		&MTU1.TIER.BYTE,
		{&MTU1.TGRA, &MTU1.TGRB, 0, 0},
		&TSTRA,
		CST1_BIT,
		&TSYRA,
		{IR_MTU1_TGIA1, IR_MTU1_TGIB1, 0, 0},
		{"MTU1A", "MTU1B", 0, 0},
		{MTU1A_ISR, MTU1B_ISR, 0, 0},
};

const T_RX62N_MTU_Info RX62N_MTU2_Info = {
		2,
		MSTP_MTUA_BIT,
        &MTU2.TCR.BYTE,
		2,	// Num Outputs
        &MTU2.TMDR.BYTE,
        &MTU2.TIOR.BYTE,
        0,
		&MTU2.TIER.BYTE,
		{&MTU2.TGRA, &MTU2.TGRB, 0, 0},
		&TSTRA,
		CST2_BIT,
		&TSYRA,
		{IR_MTU2_TGIA2, IR_MTU2_TGIB2, 0, 0},
        {"MTU2A", "MTU2B", 0, 0},
        {MTU2A_ISR, MTU2B_ISR, 0, 0},
};

const T_RX62N_MTU_Info RX62N_MTU3_Info = {
		3,
		MSTP_MTUA_BIT,
        &MTU3.TCR.BYTE,
		4,	// Num Outputs
        &MTU3.TMDR.BYTE,
        &MTU3.TIORH.BYTE,
        &MTU3.TIORL.BYTE,
		&MTU3.TIER.BYTE,
		{&MTU3.TGRA, &MTU3.TGRB, &MTU3.TGRC, &MTU3.TGRD},
		&TSTRA,
		CST3_BIT,
		&TSYRA,
		{IR_MTU3_TGIA3, IR_MTU3_TGIB3, IR_MTU3_TGIC3, IR_MTU3_TGID3},
		{"MTU3A", "MTU3B","MTU3C","MTU3D"},
		{MTU3A_ISR, MTU3B_ISR, MTU3C_ISR, MTU3D_ISR},
};

const T_RX62N_MTU_Info RX62N_MTU4_Info = {
		4,
		MSTP_MTUA_BIT,
        &MTU4.TCR.BYTE,
		4,	// Num Outputs
        &MTU4.TMDR.BYTE,
        &MTU4.TIORH.BYTE,
        &MTU4.TIORL.BYTE,
		&MTU4.TIER.BYTE,
		{&MTU4.TGRA, &MTU4.TGRB, &MTU4.TGRC, &MTU4.TGRD},
		&TSTRA,
		CST4_BIT,
		&TSYRA,
		{IR_MTU4_TGIA4, IR_MTU4_TGIB4, IR_MTU4_TGIC4, IR_MTU4_TGID4},
        {"MTU4A", "MTU4B","MTU4C","MTU4D"},
        {MTU4A_ISR, MTU4B_ISR, MTU4C_ISR, MTU4D_ISR},
};

const T_RX62N_MTU_Info RX62N_MTU6_Info = {
		6,
		MSTP_MTUB_BIT,
        &MTU6.TCR.BYTE,
		4,	// Num Outputs
        &MTU6.TMDR.BYTE,
        &MTU6.TIORH.BYTE,
        &MTU6.TIORL.BYTE,
		&MTU6.TIER.BYTE,
		{&MTU6.TGRA, &MTU6.TGRB, &MTU6.TGRC, &MTU6.TGRD},
		&TSTRB,
		CST0_BIT,
		&TSYRB,
		{IR_MTU6_TGIA6, IR_MTU6_TGIB6, IR_MTU6_TGIC6, IR_MTU6_TGID6},
		{"MTU6A", "MTU6B", "MTU6C", "MTU6D"},
		{MTU6A_ISR, MTU6B_ISR, MTU6C_ISR, MTU6D_ISR},
};

const T_RX62N_MTU_Info RX62N_MTU7_Info = {
		7,
		MSTP_MTUB_BIT,
        &MTU7.TCR.BYTE,
		2,	// Num Outputs
        &MTU7.TMDR.BYTE,
        &MTU7.TIOR.BYTE,
        0,
		&MTU7.TIER.BYTE,
		{&MTU7.TGRA, &MTU7.TGRB, 0, 0},
		&TSTRB,
		CST1_BIT,
		&TSYRB,
		{IR_MTU7_TGIA7, IR_MTU7_TGIB7, 0, 0},
		{"MTU7A", "MTU7B", 0, 0},
		{MTU7A_ISR, MTU7B_ISR, 0, 0},
};

const T_RX62N_MTU_Info RX62N_MTU8_Info = {
		8,
		MSTP_MTUB_BIT,
        &MTU8.TCR.BYTE,
		2,	// Num Outputs
        &MTU8.TMDR.BYTE,
        &MTU8.TIOR.BYTE,
        0,
		&MTU8.TIER.BYTE,
		{&MTU8.TGRA, &MTU8.TGRB, 0, 0},
		&TSTRB,
		CST2_BIT,
		&TSYRB,
		{IR_MTU8_TGIA8, IR_MTU8_TGIB8, 0, 0},
        {"MTU8A", "MTU8B", 0, 0},
        {MTU8A_ISR, MTU8B_ISR, 0, 0},
};

const T_RX62N_MTU_Info RX62N_MTU9_Info = {
		9,
		MSTP_MTUB_BIT,
        &MTU9.TCR.BYTE,
		4,	// Num Outputs
        &MTU9.TMDR.BYTE,
        &MTU9.TIORH.BYTE,
        &MTU9.TIORL.BYTE,
		&MTU9.TIER.BYTE,
		{&MTU9.TGRA, &MTU9.TGRB,
		&MTU9.TGRC, &MTU9.TGRD},
		&TSTRB,
		CST3_BIT,
		&TSYRB,
		{IR_MTU9_TGIA9, IR_MTU9_TGIB9,
		 IR_MTU9_TGIC9, IR_MTU9_TGID9},
		{"MTU9A", "MTU9B","MTU9C","MTU9D"},
		{MTU9A_ISR, MTU9B_ISR, MTU9C_ISR, MTU9D_ISR},
};

const T_RX62N_MTU_Info RX62N_MTU10_Info = {
		10,
		MSTP_MTUB_BIT,
        &MTU10.TCR.BYTE,
		4,	// Num Outputs
        &MTU10.TMDR.BYTE,
        &MTU10.TIORH.BYTE,
        &MTU10.TIORL.BYTE,
		&MTU10.TIER.BYTE,
		{&MTU10.TGRA, &MTU10.TGRB, &MTU10.TGRC,
		&MTU10.TGRD},
		&TSTRB,
		CST4_BIT,
		&TSYRB,
		{IR_MTU10_TGIA10, IR_MTU10_TGIB10, 
		 IR_MTU10_TGIC10, IR_MTU10_TGID10},
        {"MTU10A", "MTU10B","MTU10C","MTU10D"},
        {MTU10A_ISR, MTU10B_ISR, MTU10C_ISR, MTU10D_ISR},
};

void RX62N_PWM_MTU0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU0", (T_halInterface *)&PWM_RX62N_MTU0_Interface, 0, 0);
}

void RX62N_PWM_MTU1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU1", (T_halInterface *)&PWM_RX62N_MTU1_Interface, 0, 0);
}

void RX62N_PWM_MTU2_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU2", (T_halInterface *)&PWM_RX62N_MTU2_Interface, 0, 0);
}

void RX62N_PWM_MTU3_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU3", (T_halInterface *)&PWM_RX62N_MTU3_Interface, 0, 0);
}

void RX62N_PWM_MTU4_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU4", (T_halInterface *)&PWM_RX62N_MTU4_Interface, 0, 0);
}

void RX62N_PWM_MTU5_Require(void)
{
    // Error message
}

void RX62N_PWM_MTU6_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU6", (T_halInterface *)&PWM_RX62N_MTU6_Interface, 0, 0);
}

void RX62N_PWM_MTU7_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU7", (T_halInterface *)&PWM_RX62N_MTU7_Interface, 0, 0);
}

void RX62N_PWM_MTU8_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU8", (T_halInterface *)&PWM_RX62N_MTU8_Interface, 0, 0);
}

void RX62N_PWM_MTU9_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU9", (T_halInterface *)&PWM_RX62N_MTU9_Interface, 0, 0);
}

void RX62N_PWM_MTU10_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_MTU10", (T_halInterface *)&PWM_RX62N_MTU10_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_PWM_MTU.c
 *-------------------------------------------------------------------------*/

