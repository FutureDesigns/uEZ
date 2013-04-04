/*-------------------------------------------------------------------------*
 * File:  USBSharedInterrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      When both the USB Host and Device are running, they must share
 *      this interrupt.
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
#include "Config.h"
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
// TBD:  not here!
#include <uEZRTOS.h>

extern void LPC2478_InterruptProcess_USBDevice(void);
extern void LPC2478_InterruptProcess_USBHost(void);
static TBool G_setup = EFalse;

IRQ_ROUTINE(ILPC2478_USB_InterruptVector)
{
    IRQ_START();

    // Chain together these functions, one for the device, one for the host
    LPC2478_InterruptProcess_USBDevice();
    LPC2478_InterruptProcess_USBHost();

    IRQ_END();
}

void USBSharedInterruptSetup(void)
{
    if (!G_setup) {
        G_setup = ETrue;
        InterruptRegister(
            INTERRUPT_CHANNEL_USB, 
            ILPC2478_USB_InterruptVector, 
            INTERRUPT_PRIORITY_NORMAL, 
            "USB");
        InterruptEnable(INTERRUPT_CHANNEL_USB);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  USBSharedInterrupt.c
 *-------------------------------------------------------------------------*/
