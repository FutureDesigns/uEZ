/*-------------------------------------------------------------------------*
 * File:  USBSharedInterrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      When both the USB Host and Device are running, they must share
 *      this interrupt.
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include "Config.h"
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <uEZRTOS.h>

/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/
extern void LPC17xx_40xx_InterruptProcess_USBDevice(void);
extern void LPC17xx_40xx_InterruptProcess_USBHost(void);

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_USB_InterruptVector
 *---------------------------------------------------------------------------*
 * Description:
 *      Interrupt Vector routine that handles both USB device and USB host
 *      (if they are compiled in)
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC17xx_40xx_USB_InterruptVector)
{
    IRQ_START();

    // Chain together these functions, one for the device, one for the host
    if (LPC_USB->USBClkSt & (1<<1))
        LPC17xx_40xx_InterruptProcess_USBDevice();
    LPC17xx_40xx_InterruptProcess_USBHost();

    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  USBSharedInterruptSetup
 *---------------------------------------------------------------------------*
 * Description:
 *      Both Host and Device setup their interrupts by calling this routine
 * Outputs:
 *      T_LPC247x_SSP_Workspace *aW -- Particular SSP workspace
 *      SSP_Request *aRequest      -- Configuration of SSP device
 *---------------------------------------------------------------------------*/
void USBSharedInterruptSetup(void)
{
    if (!InterruptIsRegistered(USB_IRQn)) {
        InterruptRegister(
            USB_IRQn, 
            ILPC17xx_40xx_USB_InterruptVector,
            INTERRUPT_PRIORITY_NORMAL, 
            "USB");
        InterruptEnable(USB_IRQn);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  USBSharedInterrupt.c
 *-------------------------------------------------------------------------*/
