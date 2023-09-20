/*-------------------------------------------------------------------------*
 * File:  LPC1756_USBSharedInterrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      When both the USB Host and Device are running, they must share
 *      this interrupt.
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
/*
 *  @{
 *  @brief     uEZ LPC1756 USB Shared Interrupt Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ LPC1756 USB Shared Interrupt Interface.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * #include 
 * TODO
 * @endcode
 */
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <uEZRTOS.h>

/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/
extern void LPC1756_InterruptProcess_USBDevice(void);
extern void LPC1756_InterruptProcess_USBHost(void);

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_USB_InterruptVector
 *---------------------------------------------------------------------------*/
/**
 *  Interrupt Vector routine that handles both USB device and USB host
 *   (if they are compiled in)
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *	TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC1756_USB_InterruptVector)
{
    IRQ_START();

    // Chain together these functions, one for the device, one for the host
    if (USB->u2.USBClkSt & (1<<1))
        LPC1756_InterruptProcess_USBDevice();
    LPC1756_InterruptProcess_USBHost();
    
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  USBSharedInterruptSetup
 *---------------------------------------------------------------------------*/
/**
 *  Both Host and Device setup their interrupts by calling this routine
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *	TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void USBSharedInterruptSetup(void)
{
    if (!InterruptIsRegistered(USB_IRQn)) {
        InterruptRegister(
            USB_IRQn, 
            ILPC1756_USB_InterruptVector, 
            INTERRUPT_PRIORITY_NORMAL, 
            "USB");
        InterruptEnable(USB_IRQn);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_USBSharedInterrupt.c
 *-------------------------------------------------------------------------*/
