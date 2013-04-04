/*-------------------------------------------------------------------------*
 * File:  RX62N_PWM_TMR.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX62N PWM Interface using the 
 *		8 bit Timer (TMR).
 *
 * Implementation:
 *		Each TMR can be enabled by it's config macro, UEZ_ENABLE_PWM_TMRx,
 *		where x is the MTU channel
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
//#include "Config.h"
#include <uEZ.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/PWM.h>
#include <HAL/GPIO.h>
#include <HAL/Interrupt.h>
#include "RX62N_PWM_TMR.h"

/*---------------------------------------------------------------------------*
 * Defines:
 *---------------------------------------------------------------------------*/
#define MSTPA4	4
#define MSTPA5	5

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
				 TUInt8 	iMSTPBit;	// MSTP bit number
    __evenaccess TVUInt8 	*iTCNT;		// Timer Counter (TCNT)
    __evenaccess TVUInt8 	*iTCORA;	// Time Constant Register A (TCORA)
    __evenaccess TVUInt8 	*iTCORB;	// Time Constant Register B (TCORB)
    __evenaccess TVUInt8 	*iTCR;		// Timer Control Register (TCR)
	__evenaccess TVUInt8 	*iTCCR;		// Timer Counter Control Register (TCCR)
	__evenaccess TVUInt8 	*iTCSR;		// Timer Control/Status Register (TCSR)	
} T_RX62N_PWM_TMR_Info;

typedef struct {
    const HAL_PWM *iHAL;
    TUInt32 iPeriod;
	TMR_PCLK_DIV iPCLKDivide;
    const T_RX62N_PWM_TMR_Info *iInfo;
} T_RX62N_PWM_TMR_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_RX62N_PWM_TMR_Workspace *G_workspaceTMR0 = 0;
extern const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR0_Info;

T_RX62N_PWM_TMR_Workspace *G_workspaceTMR1 = 0;
extern const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR1_Info;

T_RX62N_PWM_TMR_Workspace *G_workspaceTMR2 = 0;
extern const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR2_Info;

T_RX62N_PWM_TMR_Workspace *G_workspaceTMR3 = 0;
extern const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR3_Info;


/*---------------------------------------------------------------------------*
 * Routine:  RX62N_PWM_TMR_SetMaster
 *---------------------------------------------------------------------------*
 * Description:
 *      TMR is going to start -- chip select enabled for device
 * Outputs:
 *      void *aWorkspace        -- Workspace for TMR
 *      TUInt32 aPeriod         -- Number of Fpclk intervals per period
 *      TUInt8 aMatchRegister   -- match 0 (A) or 1 (B)
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_PWM_TMR_SetMaster(
        void *aWorkspace,
        TUInt32 aPeriod,
        TUInt8 aMatchRegister)
{
    T_RX62N_PWM_TMR_Workspace *p = (T_RX62N_PWM_TMR_Workspace *)aWorkspace;
    const T_RX62N_PWM_TMR_Info *p_info = p->iInfo;
	
	if(aMatchRegister >= 2)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;

    // Ensure TMR is on
	SYSTEM.MSTPCRA.LONG &= ~(1<<p_info->iMSTPBit);	// MSTP(TMRx) = 0;    

	// Set clear control bit to this match register
	if(aMatchRegister == 0)	// match A
		*p_info->iTCR = 1 << 3;	// CCLR = 0 1: Cleared by match A
	else // match B
		*p_info->iTCR = 1 << 4;	// CCLR = 1 0: Cleared by match B

	aPeriod /= 2; // Adjust Fcclk to Fpclk
	
	// Set Master Period
	if(aMatchRegister == 0) // match A
		*p_info->iTCORA = (aPeriod/p->iPCLKDivide);
	else // match B
		*p_info->iTCORB = (aPeriod/p->iPCLKDivide);
	
	// Select PCLK divider and start the timer
	switch(p->iPCLKDivide)
	{
		case TMR_DIV_1:
			*p_info->iTCCR = 0x08; 	// PCLK
			break;
		case TMR_DIV_2:
			*p_info->iTCCR = 0x09; 	// PCLK/2
			break;
		case TMR_DIV_8:
			*p_info->iTCCR = 0x0A; 	// PCLK/8
			break;
		case TMR_DIV_32:
			*p_info->iTCCR = 0x0B; 	// PCLK/32
			break;
		case TMR_DIV_64:
			*p_info->iTCCR = 0x0C; 	// PCLK/64
			break;
		case TMR_DIV_1024:
			*p_info->iTCCR = 0x0D; 	// PCLK/1024
			break;
		case TMR_DIV_8192:
			*p_info->iTCCR = 0x0E; 	// PCLK/8192
			break;
	}
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_PWM_TMR_SetMatchRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure one of the several match registers for this TMR bank.
 * Outputs:
 *      void *aWorkspace        -- TMR's workspace
 *      TUInt8 aMatchRegister   -- match 0 (A) or 1 (B)
 *      TUInt32 aMatchPoint     -- Number of TMR cycles until match
 *      TBool aDoInterrupt      -- ETrue if want an interrupt, else EFalse
 *                                  (NOTE: Interrupts currently not 
 *                                  implemented)
 *      TBool aDoCounterReset   -- ETrue if counter for this TMR bank is
 *                                  to be reset on match.
 *      TBool aDoStop           -- ETrue if counter is to be stopped
 *                                  when match occurs.
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_PWM_TMR_SetMatchRegister(
        void *aWorkspace, 
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{	
	T_RX62N_PWM_TMR_Workspace *p = (T_RX62N_PWM_TMR_Workspace *)aWorkspace;
    const T_RX62N_PWM_TMR_Info *p_info = p->iInfo;
	
	if(aMatchRegister >= 2)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;
	
	aMatchPoint /= 2; // Adjust Fcclk to Fpclk
	
	// Set Match Point
	if(aMatchRegister == 0) // match A
		*p_info->iTCORA = (aMatchPoint/p->iPCLKDivide);
	else // match B
		*p_info->iTCORB = (aMatchPoint/p->iPCLKDivide);
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_PWM_TMR_EnableSingleEdgeOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      TMR is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- TMR's workspace
 *      TUInt8 aMatchRegister   -- match 0 (A) or 1 (B)
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_PWM_TMR_EnableSingleEdgeOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
 	T_RX62N_PWM_TMR_Workspace *p = (T_RX62N_PWM_TMR_Workspace *)aWorkspace;
    const T_RX62N_PWM_TMR_Info *p_info = p->iInfo;
	
	if(aMatchRegister >= 2)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;

	if(aMatchRegister == 0) // match A
		*p_info->iTCSR = 0xE9;	// OSA = 01b, OSB = 10b
	else // match B
		*p_info->iTCSR = 0xE6;	// OSA = 10b, OSB = 01b
		
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_PWM_TMR_DisableOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      TMR is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- TMR's workspace
 *      TUInt8 aMatchRegister   -- match 0 (A) or 1 (B)
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_PWM_TMR_DisableOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_RX62N_PWM_TMR_Workspace *p = (T_RX62N_PWM_TMR_Workspace *)aWorkspace;
    const T_RX62N_PWM_TMR_Info *p_info = p->iInfo;
	
	if(aMatchRegister >= 2)
		return UEZ_ERROR_ILLEGAL_ARGUMENT;
	
	*p_info->iTCSR = 0xE0;	// OSA = 00b, OSB = 00b
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_PWM_TMRx_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a RX62N TMR MTU for the given global workspace and info table.
 *		This routine is designed to be used by the specific MTU0-MTU10 initial
 *		routines below.
 * Inputs:
 *      void *aW                    -- Particular TMR workspace
 *		T_RX62N_MTU_Workspace *aWorkspaceMTUx
 *									-- pointer to a global RX62N MTU workspace
 *		RX62N_MTU0_Info *aMTUx_Info -- pointer to a global MTU into table
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError PWM_RX62N_TMRx_InitializeWorkspace(
	void *aWorkspace,
	const T_RX62N_PWM_TMR_Info *aTMRx_Info)
{
	T_RX62N_PWM_TMR_Workspace *p = (T_RX62N_PWM_TMR_Workspace *)aWorkspace;
    p->iInfo = aTMRx_Info;
	
	p->iPCLKDivide = TMR_DIV_32; // For now, need a way to change this

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Initialize Workspace Routines
 *---------------------------------------------------------------------------*/
T_uezError PWM_RX62N_TMR0_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceTMR0 = aWorkspace;
    return PWM_RX62N_TMRx_InitializeWorkspace(
		aWorkspace,
		&RX62N_PWM_TMR0_Info);
}

T_uezError PWM_RX62N_TMR1_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceTMR1 = aWorkspace;
    return PWM_RX62N_TMRx_InitializeWorkspace(
		aWorkspace,
		&RX62N_PWM_TMR1_Info);
}

T_uezError PWM_RX62N_TMR2_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceTMR2 = aWorkspace;
    return PWM_RX62N_TMRx_InitializeWorkspace(
		aWorkspace,
		&RX62N_PWM_TMR2_Info);
}

T_uezError PWM_RX62N_TMR3_InitializeWorkspace(void *aWorkspace)
{
    G_workspaceTMR3 = aWorkspace;
    return PWM_RX62N_TMRx_InitializeWorkspace(
		aWorkspace,
		&RX62N_PWM_TMR3_Info);
}

/*---------------------------------------------------------------------------*
 * HAL Interfaces
 *---------------------------------------------------------------------------*/
const HAL_PWM PWM_RX62N_TMR0_Interface = {
    "PWM:RX62N:TMR0",
    0x0100,
    PWM_RX62N_TMR0_InitializeWorkspace,
    sizeof(T_RX62N_PWM_TMR_Workspace),
    RX62N_PWM_TMR_SetMaster,
    RX62N_PWM_TMR_SetMatchRegister,
    RX62N_PWM_TMR_EnableSingleEdgeOutput,
    RX62N_PWM_TMR_DisableOutput,
};

const HAL_PWM PWM_RX62N_TMR1_Interface = {
    "PWM:RX62N:TMR1",
    0x0100,
    PWM_RX62N_TMR1_InitializeWorkspace,
    sizeof(T_RX62N_PWM_TMR_Workspace),
    RX62N_PWM_TMR_SetMaster,
    RX62N_PWM_TMR_SetMatchRegister,
    RX62N_PWM_TMR_EnableSingleEdgeOutput,
    RX62N_PWM_TMR_DisableOutput,
};

const HAL_PWM PWM_RX62N_TMR2_Interface = {
    "PWM:RX62N:TMR2",
    0x0100,
    PWM_RX62N_TMR2_InitializeWorkspace,
    sizeof(T_RX62N_PWM_TMR_Workspace),
    RX62N_PWM_TMR_SetMaster,
    RX62N_PWM_TMR_SetMatchRegister,
    RX62N_PWM_TMR_EnableSingleEdgeOutput,
    RX62N_PWM_TMR_DisableOutput,
};

const HAL_PWM PWM_RX62N_TMR3_Interface = {
    "PWM:RX62N:TMR3",
    0x0100,
    PWM_RX62N_TMR3_InitializeWorkspace,
    sizeof(T_RX62N_PWM_TMR_Workspace),
    RX62N_PWM_TMR_SetMaster,
    RX62N_PWM_TMR_SetMatchRegister,
    RX62N_PWM_TMR_EnableSingleEdgeOutput,
    RX62N_PWM_TMR_DisableOutput,
};

/*---------------------------------------------------------------------------*
 * Info Tables
 *---------------------------------------------------------------------------*/
const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR0_Info = {
		MSTPA5,
        &TMR0.TCNT,
        &TMR0.TCORA,
        &TMR0.TCORB,
        &TMR0.TCR.BYTE,
		&TMR0.TCCR.BYTE,
		&TMR0.TCSR.BYTE,
};

const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR1_Info = {
		MSTPA5,
        &TMR1.TCNT,
        &TMR1.TCORA,
        &TMR1.TCORB,
        &TMR1.TCR.BYTE,
		&TMR1.TCCR.BYTE,
		&TMR1.TCSR.BYTE,
};

const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR2_Info = {
		MSTPA4,
        &TMR2.TCNT,
        &TMR2.TCORA,
        &TMR2.TCORB,
        &TMR2.TCR.BYTE,
		&TMR2.TCCR.BYTE,
		&TMR2.TCSR.BYTE,
};

const T_RX62N_PWM_TMR_Info RX62N_PWM_TMR3_Info = {
		MSTPA4,
        &TMR3.TCNT,
        &TMR3.TCORA,
        &TMR3.TCORB,
        &TMR3.TCR.BYTE,
		&TMR3.TCCR.BYTE,
		&TMR3.TCSR.BYTE,
};

void RX62N_PWM_TMR0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_TMR0", (T_halInterface *)&PWM_RX62N_TMR0_Interface, 0, 0);
}

void RX62N_PWM_TMR1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_TMR1", (T_halInterface *)&PWM_RX62N_TMR1_Interface, 0, 0);
}

void RX62N_PWM_TMR2_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_TMR2", (T_halInterface *)&PWM_RX62N_TMR2_Interface, 0, 0);
}

void RX62N_PWM_TMR3_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("PWM_TMR3", (T_halInterface *)&PWM_RX62N_TMR3_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_PWM_TMR.c
 *-------------------------------------------------------------------------*/

