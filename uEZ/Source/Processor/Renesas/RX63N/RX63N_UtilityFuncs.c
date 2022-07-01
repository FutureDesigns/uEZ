/*-------------------------------------------------------------------------*
 * File:  RX63N_UtilityFuncs.c
 *-------------------------------------------------------------------------*
 * Description:
 *      
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
#include "RX63N_UtilityFuncs.h"
#include <uEZProcessor.h>
#include <stdio.h>

volatile __evenaccess unsigned char *RX63N_MPC_PFS_Table[] = {
	&MPC.P00PFS.BYTE,
	&MPC.P10PFS.BYTE,
	&MPC.P20PFS.BYTE,
	&MPC.P30PFS.BYTE,
	&MPC.P40PFS.BYTE,
	&MPC.P50PFS.BYTE,
	&MPC.P60PFS.BYTE,
	&MPC.P70PFS.BYTE,
	&MPC.P80PFS.BYTE,
	&MPC.P90PFS.BYTE,
	&MPC.PA0PFS.BYTE,
	&MPC.PB0PFS.BYTE,
	&MPC.PC0PFS.BYTE,
	&MPC.PD0PFS.BYTE,
	&MPC.PE0PFS.BYTE,
	&MPC.PF0PFS.BYTE,
};

void RX63N_PinsLock(
        const T_uezGPIOPortPin *aPins,
        TUInt8 aCount)
{
    while (aCount--) {
        UEZGPIOLock(aPins[aCount]);
    }
}

void RX63N_UnlockProtectionBits()
{
	// Unlock protection bits
	SYSTEM.PRCR.WORD = 0xA50B;			/* Protect off */
    MPC.PWPR.BIT.B0WI = 0 ;     		/* Unlock protection register */
    MPC.PWPR.BIT.PFSWE = 1 ;    		/* Unlock MPC registers */  
}

void RX63N_LockProtectionBits()
{
	// Lock protection bits
    MPC.PWPR.BIT.PFSWE = 0 ;    		/* Lock MPC registers */
    MPC.PWPR.BIT.B0WI = 1 ;     		/* Lock protection register */ 
    SYSTEM.PRCR.WORD = 0xA500;			/* protect on */
}

void RX63N_MPC_ConfigAllPins(
        const T_RX63N_MPC_ConfigList *aList,
        TUInt8 aCount)
{
	volatile __evenaccess unsigned char *p_PFSByte;
	
    while (aCount--) {

        UEZGPIOLock(aList->iPortPin);
		
		// Set Portx.PMR.BIT.Bx = 1
		UEZGPIOSetMux(aList->iPortPin, 1);
		
		// Set MPC.PxxPFS.BIT.PSEL = aList->iPFSSetting
        p_PFSByte = RX63N_MPC_PFS_Table[UEZ_GPIO_PORT_FROM_PORT_PIN(aList->iPortPin)];
		p_PFSByte[UEZ_GPIO_PIN_FROM_PORT_PIN(aList->iPortPin)] = (aList->iPFSSetting & 0x1F);
			
        aList++;
    }
}

void RX63N_MPC_ConfigPin(
        T_uezGPIOPortPin aPortPin,
        const T_RX63N_MPC_ConfigList *aList,
        TUInt8 aCount)
{
	volatile __evenaccess unsigned char *p_PFSByte;
	
    while (aCount--) {
        if (aList->iPortPin == aPortPin) {
            UEZGPIOLock(aPortPin);
			
			// Set Portx.PMR.BIT.Bx = 1
			UEZGPIOSetMux(aPortPin, 1);
			
			// Set MPC.PxxPFS.BIT.PSEL = aList->iPFSSetting
            p_PFSByte = RX63N_MPC_PFS_Table[UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin)];
			p_PFSByte[UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin)] = (aList->iPFSSetting & 0x1F);
			
            return;
        }
        aList++;
    }
    UEZFailureMsg("Bad Pin");
}

void RX63N_MPC_ConfigPinOrNone(
        T_uezGPIOPortPin aPortPin,
        const T_RX63N_MPC_ConfigList *aList,
        TUInt8 aCount)
{
    if (aPortPin == GPIO_NONE)
        return;
    RX63N_MPC_ConfigPin(aPortPin, aList, aCount);
}
 
 /*-------------------------------------------------------------------------*
 * End of File:  RX63N_UtilityFuncs.c
 *-------------------------------------------------------------------------*/