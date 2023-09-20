/*-------------------------------------------------------------------------*
 * File:  RX63N_ADCBank_S12AD.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX63N 12 bit A/D Bus Interface.
 * Implementation:
 *      A single S12AD is created, but the code can be easily changed for
 *      other processors to use multiple ADC busses.
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
#include <HAL/Interrupt.h>
#include "RX63N_UtilityFuncs.h"
#include "RX63N_ADCBank_S12AD.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
__evenaccess const static TVUInt16 *G_DataRegisters[21] = {
    &S12AD.ADDR0,	// AN000
	&S12AD.ADDR1,	// AN001
	&S12AD.ADDR2,	// AN002
	&S12AD.ADDR3,	// AN003
	&S12AD.ADDR4,	// AN004
	&S12AD.ADDR5,	// AN005
	&S12AD.ADDR6,	// AN006
	&S12AD.ADDR7,	// AN007
	&S12AD.ADDR8,	// AN008
	&S12AD.ADDR9,	// AN009
	&S12AD.ADDR10,	// AN010
	&S12AD.ADDR11,	// AN011
	&S12AD.ADDR12,	// AN012
	&S12AD.ADDR13,	// AN013
	&S12AD.ADDR14,	// AN014
	&S12AD.ADDR15,	// AN015
	&S12AD.ADDR16,	// AN016
	&S12AD.ADDR17,	// AN017
	&S12AD.ADDR18,	// AN018
	&S12AD.ADDR19,	// AN019
	&S12AD.ADDR20	// AN020
};

__evenaccess static TVUInt8 *G_ADC_PFS[21] = {
    &MPC.P40PFS.BYTE,	// AN000
	&MPC.P41PFS.BYTE,	// AN001
	&MPC.P42PFS.BYTE,	// AN002
	&MPC.P43PFS.BYTE,	// AN003
	&MPC.P44PFS.BYTE,	// AN004
	&MPC.P45PFS.BYTE,	// AN005
	&MPC.P46PFS.BYTE,	// AN006
	&MPC.P47PFS.BYTE,	// AN007
	&MPC.PD0PFS.BYTE,	// AN008
	&MPC.PD1PFS.BYTE,	// AN009
	&MPC.PD2PFS.BYTE,	// AN010
	&MPC.PD3PFS.BYTE,	// AN011
	&MPC.PD4PFS.BYTE,	// AN012
	&MPC.PD5PFS.BYTE,	// AN013
	&MPC.P90PFS.BYTE,	// AN014
	&MPC.P91PFS.BYTE,	// AN015
	&MPC.P92PFS.BYTE,	// AN016
	&MPC.P93PFS.BYTE,	// AN017
	&MPC.P00PFS.BYTE,	// AN018
	&MPC.P01PFS.BYTE,	// AN019
	&MPC.P02PFS.BYTE,	// AN020
};

typedef struct {
    const HAL_ADCBank *iHAL;
    ADC_RequestSingle iRequest; // Current request settings
    void *iCallbackWorkspace;
    const T_adcBankCallbackInterface *iCallbackAPI;
    TUInt32 iInterruptChannel;
} T_RX63N_ADC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_RX63N_ADC_Workspace *G_S12ADWorkspace = 0;

/*---------------------------------------------------------------------------*
 * Routine:  IRX63N_ADC_S12AD_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N S12AD workspace.
 * Inputs:
 *      T_RX63N_ADC_Workspace *aW -- ADC Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void IRX63N_ADC_S12AD_ISR(T_RX63N_ADC_Workspace *p)
{
	*p->iRequest.iCapturedData = *G_DataRegisters[p->iRequest.iADCChannel];

    // Tell the callback routine the data is ready
    p->iCallbackAPI->CaptureComplete(
            p->iCallbackWorkspace,
            &p->iRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  IADCConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC for reading but don't start it
 * Outputs:
 *      T_RX63N_ADC_Workspace *aW -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Configuration of ADC device
 *---------------------------------------------------------------------------*/
static void IADCConfig(
                T_RX63N_ADC_Workspace *p,
                ADC_RequestSingle *aRequest)
{
	// Ensure the power is on
	MSTP(S12AD) = 0;
	
	S12AD.ADCSR.BIT.ADST = 0;	//	0: Stops a scan conversion process.
								//	1: Starts a scan conversion process.
	
	S12AD.ADCSR.BIT.EXTRG = 0; 	//	0: 	Scan conversion is started by a timer 
								//		source selected by the A/D start
								//		trigger select register (ADSTRGR).
								//	1:	Scan conversion is started by an 
								//		external trigger (ADTRG0#).
								
	S12AD.ADCSR.BIT.TRGE = 0;	//	0: 	Disables scan conversion to be started 
								//		by an external trigger (ADTRG0#) or a 
								//		trigger of MTU or TMR.
								//	1: 	Enables scan conversion to be started 
								//		by an external trigger (ADTRG0#) or a 
								//		trigger of MTU or TMR.
	
	S12AD.ADCSR.BIT.CKS = 0;	//	00: PCLK/8
								//	01: PCLK/4
								//	10: PCLK/2
								//	11: PCLK
								
	S12AD.ADCSR.BIT.ADIE = 0;	//	0: 	Disables ADI interrupt generation 
								//		upon scan conversion completion.
								//	1: 	Enables ADI interrupt generation 
								//		upon scan conversion completion.
								
	S12AD.ADCSR.BIT.ADCS = 0;	//	0: 	Single-cycle scan mode
								//	1: 	Continuous scan mode
								
	S12AD.ADCER.BIT.ACE = 0;	//	0: 	Disables automatic clearing of the 
								//		A/D data register n (ADDRn) after 
								//		it has been read.
								//	1: 	Enables automatic clearing of the 
								//		A/D data register n (ADDRn) after 
								//		it has been read.
							
	S12AD.ADCER.BIT.ADRFMT = 0;	//	0: 	Right-alignment is selected for the 
								//		A/D data register n (ADDRn) format.
								//	1: 	Left-alignment is selected for the 
								//		A/D data register n (ADDRn) format.					
	
	// A/D Channel Select
	if(aRequest->iADCChannel < 16) {
		S12AD.ADANS0.WORD = 1<<aRequest->iADCChannel;
		S12AD.ADANS1.WORD = 0;
	} else {
		S12AD.ADANS0.WORD = 0;
		S12AD.ADANS1.WORD = 1<<(aRequest->iADCChannel-16);
	}
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_ADC_Request
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a ADC request either doing a single triggered request and stopping,
 *      doing continuous readings, or stop a previously started request.
 * Inputs:
 *      void *aW                    -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Request to perform
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_ADC_S12AD_RequestSingle(void *aWorkspace, ADC_RequestSingle *aRequest)
{
    T_RX63N_ADC_Workspace *p = (T_RX63N_ADC_Workspace *)aWorkspace;

    // Ensure the interrupt is disabled
    InterruptDisable(p->iInterruptChannel);

    // Transfer the configuration information to the ADC
    p->iRequest = *aRequest;

    // Setup the hardware for the reading type (but don't start)
    IADCConfig(p, &p->iRequest);

    // At this point, nothing is enabled.
    // Are we starting or stopping?
    if (aRequest->iTrigger != ADC_TRIGGER_STOP) {
        // ADI interrupt is enabled by completing A/D conversion
		S12AD.ADCSR.BIT.ADIE = 1;

        // Allow interrupt to be enabled (but it won't at this point)
        InterruptEnable(p->iInterruptChannel);

        // Start A/D conversion
       	S12AD.ADCSR.BIT.ADST = 1;
    }

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Interrupt Routine:  IRX63N_ADC_S12AD_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch S12AD interrupt and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(IRX63N_ADC_S12AD_Interrupt)
{
    IRQ_START();
    IRX63N_ADC_S12AD_ISR(G_S12ADWorkspace);
    IRQ_END();
}

T_uezError RX63N_ADC_S12AD_Configure(
            void *aWorkspace,
            void *aCallbackWorkspace,
            const T_adcBankCallbackInterface * const aCallbackAPI)
{
    T_RX63N_ADC_Workspace *p = (T_RX63N_ADC_Workspace *)aWorkspace;
    p->iCallbackWorkspace = aCallbackWorkspace;
    p->iCallbackAPI = aCallbackAPI;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX63N_ADC_S12AD_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX63N S12AD workspace.
 * Inputs:
 *      void *aW                    -- Particular ADC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX63N_ADC_S12AD_InitializeWorkspace(void *aWorkspace)
{
    T_RX63N_ADC_Workspace *p = (T_RX63N_ADC_Workspace *)aWorkspace;
    G_S12ADWorkspace = p;
    p->iInterruptChannel = VECT_S12AD0_S12ADI0;

    // Register an interrupt for this ADC but don't start it.
    InterruptRegister(
        p->iInterruptChannel,
        IRX63N_ADC_S12AD_Interrupt,
        INTERRUPT_PRIORITY_NORMAL,
        "S12AD");

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_ADCBank RX63N_ADC_S12AD_Interface = {
    "ADC:RX63N:S12AD",
    0x0100,
    RX63N_ADC_S12AD_InitializeWorkspace,
    sizeof(T_RX63N_ADC_Workspace),

    RX63N_ADC_S12AD_Configure,
    RX63N_ADC_S12AD_RequestSingle,
};

void RX63N_S12AD_Require(TUInt32 channelMask)
{
	TUInt8 i;
	
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_P40,  	// AN000
			GPIO_P41,	// AN001
			GPIO_P42,  	// AN002
			GPIO_P43,	// AN003
			GPIO_P44,  	// AN004
			GPIO_P45,	// AN005
			GPIO_P46,  	// AN006
			GPIO_P47,	// AN007
			GPIO_PD0,	// AN008
			GPIO_PD1,	// AN009
			GPIO_PD2,	// AN010
			GPIO_PD3,	// AN011
			GPIO_PD4,	// AN012
			GPIO_PD5,	// AN013
			GPIO_P90,	// AN014
			GPIO_P91,	// AN015
			GPIO_P92,	// AN016
			GPIO_P93,	// AN017
			GPIO_P00,	// AN018
			GPIO_P01,	// AN019
			GPIO_P02,	// AN020
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("ADC_S12AD", (T_halInterface *)&RX63N_ADC_S12AD_Interface,
            0, 0);
			
	for(i=0; i<21; i++) {
		if(channelMask & (1<<i)) {
			*G_ADC_PFS[i] = 1;	// Set ADC i to Analog Input
			UEZGPIOLock(pins[i]);
		}
	}
}

/*-------------------------------------------------------------------------*
 * End of File:  ADC.c
 *-------------------------------------------------------------------------*/

