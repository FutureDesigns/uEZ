/*-------------------------------------------------------------------------*
 * File:  uEZGPIO.h
 *-------------------------------------------------------------------------*
 * Description:
 *     The UEZGPIO System puts all GPIO Ports into a single system and
 *     allows all drivers and subsystems to access the port pins directly.
 *     Be warned, however, for this system is not thread safe for the sake
 *     of speed and to allow these routines to be called from within interrupt
 *     routines.
 *
 *     Ports are registered to the subsystem and assigned a port number.
 *     All ports are assumed to have 0-31 pins.  Each port pin can then
 *     be referenced by a single 16-bit number called a T_uezGPIOPortPin --
 *     a number created by the macro UEZ_GPIO_PORT_PIN(port, pin).  The 16-bit
 *     number contains the port index and the pin index.  The routines here
 *     than take that number and do the standard GPIO operations needed
 *     (set, clear, input, output, etc.).
 *
 *     These routines have been optimized to be as quick as possible, so
 *     calling UEZGPIOSet, UEZGPIOClear, etc. are very fast.
 *
 *     If speed is necessary, the routine UEZGPIOGetPort can be used to
 *     decode which port is in use once and then call the GPIO_HALPort
 *     directly.
 *
 *     Calling set, clear, etc. on a port that has not been registered
 *     will cause a fatal error.  Don't do it.
 *-------------------------------------------------------------------------*/
#ifndef UEZGPIO_H_
#define UEZGPIO_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/**
 *  @file   uEZGPIO.h
 *  @brief  uEZ GPIO
 *
 *  uEZ GPIO
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <HAL/GPIO.h>
#include <Types/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
/** Number of GPIO Port slots in the uEZ GPIO system. */
#ifndef UEZ_GPIO_MAX_PORTS
#define UEZ_GPIO_MAX_PORTS      40
#endif

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void UEZGPIOReset(void);
T_uezError UEZGPIORegisterPort(T_uezGPIOPort aPortNum, HAL_GPIOPort **aPort);
HAL_GPIOPort **UEZGPIOGetPort(T_uezGPIOPort aPort);
HAL_GPIOPort **UEZGPIOGetPortOrFail(T_uezGPIOPort aPort);
T_uezError UEZGPIOSet(T_uezGPIOPortPin aPortPin);
T_uezError UEZGPIOClear(T_uezGPIOPortPin aPortPin);
T_uezError UEZGPIOOutput(T_uezGPIOPortPin aPortPin);
T_uezError UEZGPIOInput(T_uezGPIOPortPin aPortPin);
T_uezError UEZGPIOSetPull(T_uezGPIOPortPin aPortPin, T_gpioPull aPull);

T_uezError UEZGPIOSetMux(T_uezGPIOPortPin aPortPin, T_gpioMux aMux);
T_uezError UEZGPIOGetMux(T_uezGPIOPortPin aPortPin, T_gpioMux *aMux);
T_uezError UEZGPIOEnableIRQ(
        T_uezGPIOPortPin aPortPin,
        T_gpioInterruptType aType);
T_uezError UEZGPIODisableIRQ(
        T_uezGPIOPortPin aPortPin,
        T_gpioInterruptType aType);
T_uezError UEZGPIOClearIRQ(T_uezGPIOPortPin aPortPin);
T_uezError UEZGPIOConfigureInterruptCallback(
        T_uezGPIOPort aPort,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace);
T_uezError UEZGPIOControl(
        T_uezGPIOPortPin aPortPin,
        T_gpioControl aControl,
        TUInt32 aValue);
TBool UEZGPIORead(T_uezGPIOPortPin aPortPin);
T_uezError UEZGPIOReadPort(T_uezGPIOPort aPort, TUInt32 *aReadValue);
void UEZGPIOLock(T_uezGPIOPortPin aPortPin);
void UEZGPIOUnlock(T_uezGPIOPortPin aPortPin);

#define GPIO_TO_HAL_PORT(portpin)  \
    UEZGPIOGetPort(UEZ_GPIO_PORT_FROM_PORT_PIN(portpin))
#define GPIO_TO_HAL_PORT_OR_FAIL(portpin)  \
    UEZGPIOGetPortOrFail(UEZ_GPIO_PORT_FROM_PORT_PIN(portpin))
#define GPIO_TO_PIN_BIT(portpin)     (1<<UEZ_GPIO_PIN_FROM_PORT_PIN(portpin))
#define GPIO_TO_PIN_INDEX(portpin)   (UEZ_GPIO_PIN_FROM_PORT_PIN(portpin))

#ifdef __cplusplus
}
#endif

#endif // UEZGPIO_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZGPIO.h
 *-------------------------------------------------------------------------*/
