/*-------------------------------------------------------------------------*
 * File:  RX62N_ADCBank_S12AD.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX62N 12 bit A/D Bus Interface.
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
#include "RX62N_UtilityFuncs.h"
#include "RX62N_ADCBank_S12AD.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
__evenaccess const static TVUInt16 *G_DataRegisters[8] = {
    &S12AD.ADDRA,
	&S12AD.ADDRB,
	&S12AD.ADDRC,
	&S12AD.ADDRD,
	&S12AD.ADDRE,
	&S12AD.ADDRF,
	&S12AD.ADDRG,
	&S12AD.ADDRH
};

typedef struct {
    const HAL_ADCBank *iHAL;
    ADC_RequestSingle iRequest; // Current request settings
    void *iCallbackWorkspace;
    const T_adcBankCallbackInterface *iCallbackAPI;
    TUInt32 iInterruptChannel;
} T_RX62N_ADC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_RX62N_ADC_Workspace *G_S12ADWorkspace = 0;

/*---------------------------------------------------------------------------*
 * Routine:  IRX62N_ADC_S12AD_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N S12AD workspace.
 * Inputs:
 *      T_RX62N_ADC_Workspace *aW -- ADC Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void IRX62N_ADC_S12AD_ISR(T_RX62N_ADC_Workspace *p)
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
 *      T_RX62N_ADC_Workspace *aW -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Configuration of ADC device
 *---------------------------------------------------------------------------*/
static void IADCConfig(
                T_RX62N_ADC_Workspace *p,
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
	S12AD.ADANS.WORD = 1<<aRequest->iADCChannel;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_ADC_Request
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
T_uezError RX62N_ADC_S12AD_RequestSingle(void *aWorkspace, ADC_RequestSingle *aRequest)
{
    T_RX62N_ADC_Workspace *p = (T_RX62N_ADC_Workspace *)aWorkspace;

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
 * Interrupt Routine:  IRX62N_ADC_S12AD_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch S12AD interrupt and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(IRX62N_ADC_S12AD_Interrupt)
{
    IRQ_START();
    IRX62N_ADC_S12AD_ISR(G_S12ADWorkspace);
    IRQ_END();
}

T_uezError RX62N_ADC_S12AD_Configure(
            void *aWorkspace,
            void *aCallbackWorkspace,
            const T_adcBankCallbackInterface * const aCallbackAPI)
{
    T_RX62N_ADC_Workspace *p = (T_RX62N_ADC_Workspace *)aWorkspace;
    p->iCallbackWorkspace = aCallbackWorkspace;
    p->iCallbackAPI = aCallbackAPI;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_ADC_S12AD_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N S12AD workspace.
 * Inputs:
 *      void *aW                    -- Particular ADC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_ADC_S12AD_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_ADC_Workspace *p = (T_RX62N_ADC_Workspace *)aWorkspace;
    G_S12ADWorkspace = p;
    p->iInterruptChannel = VECT_S12AD_ADI;

    // Register an interrupt for this ADC but don't start it.
    InterruptRegister(
        p->iInterruptChannel,
        IRX62N_ADC_S12AD_Interrupt,
        INTERRUPT_PRIORITY_NORMAL,
        "S12AD");

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_ADCBank RX62N_ADC_S12AD_Interface = {
    "ADC:RX62N:S12AD",
    0x0100,
    RX62N_ADC_S12AD_InitializeWorkspace,
    sizeof(T_RX62N_ADC_Workspace),

    RX62N_ADC_S12AD_Configure,
    RX62N_ADC_S12AD_RequestSingle,
};

void RX62N_S12AD_Require(TUInt8 channelMask)
{
	TUInt8 i;
	
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_P40,  	// AN0
			GPIO_P41,	// AN1
			GPIO_P42,  	// AN2
			GPIO_P43,	// AN3
			GPIO_P44,  	// AN4
			GPIO_P45,	// AN5
			GPIO_P46,  	// AN6
			GPIO_P47,	// AN7
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("ADC_S12AD", (T_halInterface *)&RX62N_ADC_S12AD_Interface,
            0, 0);
			
	for(i=0; i<8; i++) {
		if(channelMask & (1<<i)) {
			UEZGPIOLock(pins[i]);
		}
	}
}

/*-------------------------------------------------------------------------*
 * End of File:  ADC.c
 *-------------------------------------------------------------------------*/

