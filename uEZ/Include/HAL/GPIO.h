/*-------------------------------------------------------------------------*
 * File:  GPIO.h
 *-------------------------------------------------------------------------*
 * Description:
 *     GPIO HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

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
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/**
 *  @file   /Include/HAL/GPIO.h
 *  @brief  uEZ GPIO HAL Interface
 *
 *  The uEZ GPIO HAL Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Include/uEZTypes.h>
#include <Include/HAL/HAL.h>

#ifdef __cplusplus
extern "C" {
#endif


/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    GPIO_PULL_UP=0,
    GPIO_RESERVED=1,
    GPIO_PULL_NONE=2,
    GPIO_PULL_DOWN=3,
    GPIO_REPEATER=4, /**< Pull-up when 1, pull-down when low */
} T_gpioPull;

typedef enum {
    GPIO_INTERRUPT_FALLING_EDGE,
    GPIO_INTERRUPT_RISING_EDGE,
} T_gpioInterruptType;

typedef enum {
    GPIO_NOP=0,
    GPIO_CONTROL_ENABLE_INPUT_BUFFER,
    GPIO_CONTROL_DISABLE_INPUT_BUFFER,
	GPIO_CONTROL_ENABLE_OPEN_DRAIN,
	GPIO_CONTROL_DISABLE_OPEN_DRAIN,
    GPIO_CONTROL_SET_CONFIG_BITS, /**< system specific */
} T_gpioControl;

typedef void (*T_gpioInterruptHandler)(
        void *aInterruptHandlerWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType);


/**
 * Mux different functionality (0=GPIO, 1..x are other functions for that pin)
 */ 
typedef TUInt32 T_gpioMux;

typedef struct {
/**
 * Header
 */
    T_halInterface iInterface;

/**
 * v1.00 Functions
 */
    T_uezError (*deprecated_Configure)(void *aWorkspace, TUInt32 aUsablePins);
    T_uezError (*deprecated_Activate)(void *aWorkspace, TUInt32 aPortPins);
    T_uezError (*deprecated_Deactivate)(void *aWorkspace, TUInt32 aPortPins);
    T_uezError (*SetOutputMode)(void *aWorkspace, TUInt32 aPortPins);
    T_uezError (*SetInputMode)(void *aWorkspace, TUInt32 aPortPins);
    T_uezError (*Set)(void *aWorkspace, TUInt32 aPortPins);
    T_uezError (*Clear)(void *aWorkspace, TUInt32 aPortPins);
    T_uezError (*Read)(void *aWorkspace, TUInt32 aPortPins, TUInt32 *aPinsRead);
    T_uezError (*SetPull)(void *aWorkspace, TUInt8 aPortPinIndex, T_gpioPull aPull);
    T_uezError (*SetMux)(void *aWorkspace, TUInt8 aPortPinIndex, T_gpioMux aMux);

/**
 * v1.05 Functions
 */
    //Modified in v2.08 to pass the port pin number
    T_uezError (*ConfigureInterruptCallback)(
            void *aWorkspace,
            TUInt32 aPortPins,
            T_gpioInterruptHandler iInterruptCallback,
            void *iInterruptCallbackWorkspace);
    T_uezError (*EnableInterrupts)(
            void *aWorkspace,
            TUInt32 aPortPins,
            T_gpioInterruptType aType);
    T_uezError (*DisableInterrupts)(
            void *aWorkspace,
            TUInt32 aPortPins,
            T_gpioInterruptType aType);
    T_uezError (*ClearInterrupts)(
            void *aWorkspace,
            TUInt32 aPortPins);

/**
 * v1.06 Functions
 */
    T_uezError (*Control)(
            void *aWorkspace,
            TUInt8 aPortPinIndex,
            T_gpioControl aControl,
            TUInt32 aValue);

/**
 * v1.13 Functions
 */
    T_uezError (*Lock)(
            void *aWorkspace,
            TUInt32 aPortPins);
    T_uezError (*Unlock)(
            void *aWorkspace,
            TUInt32 aPortPins);

    // v2.04 Functions
    T_uezError (*GetMux)(void *aWorkspace, TUInt8 aPortPinIndex, T_gpioMux *aMux);
} HAL_GPIOPort ;

#define GPIO_SET(p, pins)       (((*(p))->Set)(p, pins))
#define GPIO_CLEAR(p, pins)     (((*(p))->Clear)(p, pins))
#define GPIO_READ(p, pins, r)   (((*(p))->Read)(p, pins, r))

#ifdef __cplusplus
}
#endif

#endif // _HAL_GPIO_H_
/*-------------------------------------------------------------------------*
 * End of File:  GPIO.h
 *-------------------------------------------------------------------------*/
