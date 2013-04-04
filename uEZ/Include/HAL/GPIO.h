/*-------------------------------------------------------------------------*
 * File:  GPIO.h
 *-------------------------------------------------------------------------*
 * Description:
 *     GPIO HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

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
#include <uEZTypes.h>
#include <HAL/HAL.h>

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
    T_uezError (*ConfigureInterruptCallback)(
            void *aWorkspace,
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

#endif // _HAL_GPIO_H_
/*-------------------------------------------------------------------------*
 * End of File:  GPIO.h
 *-------------------------------------------------------------------------*/
