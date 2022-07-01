/*-------------------------------------------------------------------------*
 * File:  uEZGPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *     		The uEZ GPIO interface.
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
/**
 *    @addtogroup uEZGPIO
 *  @{
 *  @brief     uEZ GPIO Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ GPIO interface.
 *
 * @par Example code:
 * Example task to test GPIO input/output
 * @par
 * @code
 * #include <uEZ.h>
 * #include <uEZGPIO.h>
 *  *
 * TUInt32 GPIOTest(T_uezTask aMyTask, void *aParams)
 * {
 * 	UEZGPIOReset();	 
 *
 * 	UEZGPIOOutput(GPIO_P0_11);	// set P0_11 to output
 * 	UEZGPIOSet(GPIO_P0_11); 		// set P0_11 high
 * 	UEZGPIOClear(GPIO_P0_11);	// set P0_11 low
 *
 * 	UEZGPIOInput(GPIO_P0_12);					// set P0_12 to input
 * 	UEZGPIOSetPull(GPIO_P0_12, GPIO_PULL_UP); 	// set P0_12 pull up to high
 * 	if (UEZGPIORead(GPIO_P0_12) == ETrue) {
 *		//port was high
 * 	}
 * 	return 0;
 * }
 * @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZGPIO.h>
#include <uEZBSP.h>

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static HAL_GPIOPort **G_gpioPorts[UEZ_GPIO_MAX_PORTS];

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOReset
 *---------------------------------------------------------------------------*/
/**
 *  At power up, call this routine to reset the list of GPIO ports
 *      in the system.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOReset(void);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZGPIOReset(void)
{
    TUInt32 i;
    // Initialize the registered ports to none
    for (i = 0; i < UEZ_GPIO_MAX_PORTS; i++)
        G_gpioPorts[i] = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOGetPort
 *---------------------------------------------------------------------------*/
/**
 *  At power up, call this routine to reset the list of GPIO ports
 *      in the system.  Returns 0 if the port does not exist or is out
 *      of range.
 *
 *  @param [in]    aPort 				Port to look up
 *
 *  @return        **HAL_GPIOPort 		Workspace of GPIO Port found.  
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  **HAL_GPIOPort workspace;
 *  UEZGPIORegisterPort(UEZ_GPIO_PORT_0, workspace);
 *  workspace = UEZGPIOGetPort(UEZ_GPIO_PORT_0);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
HAL_GPIOPort **UEZGPIOGetPort(T_uezGPIOPort aPort)
{
    // Return null pointer for no port entry
    if (aPort == UEZ_GPIO_PORT_NONE)
        return 0;
    if (aPort >= UEZ_GPIO_MAX_PORTS)
        UEZFailureMsg("Invalid GPIO Port!");
    return G_gpioPorts[aPort];
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOGetPortOrFail
 *---------------------------------------------------------------------------*/
/**
 *  Get the port driver of the given port pin.  If the port is out
 *      of range or not registered, fail with a failure message.
 *
 *  @param [in]    aPort 				Port to look up
 *
 *  @return        **HAL_GPIOPort   	Workspace of GPIO Port found.  
 *										Any failures to return a valid value
 * 										will cause a runtime failure.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  **HAL_GPIOPort workspace;
 *  UEZGPIORegisterPort(UEZ_GPIO_PORT_0, workspace);
 *  workspace = UEZGPIOGetPortOrFail(UEZ_GPIO_PORT_0);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
HAL_GPIOPort **UEZGPIOGetPortOrFail(T_uezGPIOPort aPort)
{
    HAL_GPIOPort * *p;
    if (aPort >= UEZ_GPIO_MAX_PORTS)
        UEZFailureMsg("Invalid GPIO Port!");
    p = G_gpioPorts[aPort];
    if (!p)
        UEZFailureMsg("Access of invalid GPIO Port!");
    return p;
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIORegisterPort
 *---------------------------------------------------------------------------*/
/**
 *  Register the given port number
 *
 *  @param [in]    aPortNum			Port Number
 *
 *  @param [in]    **aPort  	    Workspace of GPIO Port found.  Any failures
 *                                  to return a valid value will cause a 
 *                                  runtime failure.
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  **HAL_GPIOPort workspace;
 *  UEZGPIORegisterPort(UEZ_GPIO_PORT_0, workspace);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIORegisterPort(T_uezGPIOPort aPortNum, HAL_GPIOPort **aPort)
{
    // Register only once
    if (G_gpioPorts[aPortNum]) {
        UEZFailureMsg("UEZGPIORegisterPort");
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    G_gpioPorts[aPortNum] = aPort;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOSet
 *---------------------------------------------------------------------------*/
/**
 *  Set the given pin to HIGH.
 *
 *  @param [in]    aPortPin 	Port pin to affect
 *
 *  @return        T_uezError 	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOOutput(GPIO_P0_12);		// set P0_12 to output
 *  UEZGPIOSet(GPIO_P0_12); 		// set P0_12 high
 *  UEZGPIOClear(GPIO_P0_12);		// set P0_12 low
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOSet(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->Set(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin));
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOClear
 *---------------------------------------------------------------------------*/
/**
 *  Clear the given pin to LOW.
 *
 *  @param [in]    aPortPin 	Port pin to affect
 *
 *  @return        T_uezError 	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOOutput(GPIO_P0_12);		// set P0_12 to output
 *  UEZGPIOSet(GPIO_P0_12); 		// set P0_12 high
 *  UEZGPIOClear(GPIO_P0_12);		// set P0_12 low
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOClear(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->Clear(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin));
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOOutput
 *---------------------------------------------------------------------------*/
/**
 *  Put the given pin into output mode.
 *
 *  @param [in]    aPortPin 	Port pin to affect
 *
 *  @return        T_uezError 	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOOutput(GPIO_P0_12);		// set P0_12 to output
 *  UEZGPIOSet(GPIO_P0_12); 		// set P0_12 high
 *  UEZGPIOClear(GPIO_P0_12);		// set P0_12 low
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOOutput(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->SetOutputMode(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin));
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOInput
 *---------------------------------------------------------------------------*/
/**
 *  Put the given pin into input mode.
 *
 *  @param [in]    aPortPin		Port pin to affect
 *
 *  @return        T_uezError 	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOInput(GPIO_P0_12);		// set P0_12 to input
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOInput(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->SetInputMode(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin));
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOSetPull
 *---------------------------------------------------------------------------*/
/**
 *  Set the given pin's pull up/down/none mode.
 *
 *  @param [in]    aPortPin		Port pin to affect
 *
 *  @param [in]    aPull 		Direction to pull the pin.
 *
 *  @return        T_uezError 	Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOSetPull(GPIO_P0_12, GPIO_PULL_UP); // set P0_12 pull up to high
 *  // GPIO_PULL_UP
 *  // GPIO_RESERVED
 *  // GPIO_PULL_NONE
 *  // GPIO_PULL_DOWN
 *  // GPIO_REPEATER // Pull-up when 1, pull-down when low
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOSetPull(T_uezGPIOPortPin aPortPin, T_gpioPull aPull)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->SetPull(p, UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin), aPull);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOSetMux
 *---------------------------------------------------------------------------*/
/**
 *  Set the given pin's mux register (if any).  The functionality of this
 *      register is CPU dependent.
 *
 *  @param [in]    aPortPin 	Port pin to affect
 *
 *  @param [in]    aMux 		Mux mode of the port pin
 *
 *  @return        T_uezError 	Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  // set to 0 for GPIO, higher for other pin functions
 *  UEZGPIOSetMux(GPIO_P0_12, 0); 
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOSetMux(T_uezGPIOPortPin aPortPin, T_gpioMux aMux)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->SetMux(p, UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin), aMux);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOGetMux
 *---------------------------------------------------------------------------*/
/**
 *  Get the given pin's mux register (if any).  The functionality of this
 *      register is CPU dependent.
 *
 *  @param [in]    aPortPin     Port pin to affect
 *
 *  @param [out]   *aMux        Mux mode of the port pin
 *
 *  @return        T_uezError   Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  // set to 0 for GPIO, higher for other pin functions
 *  UEZGPIOSetMux(GPIO_P0_12, 0);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOGetMux(T_uezGPIOPortPin aPortPin, T_gpioMux *aMux)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->GetMux(p, UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin), aMux);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOEnableIRQ
 *---------------------------------------------------------------------------*/
/**
 *  Enable the pin's ability to generate interrupts.
 *
 *  @param [in]    aPortPin 	Port pin to affect
 *
 *  @param [in]    aType 		Type of interrupt
 *
 *  @return        T_uezError	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOEnableIRQ(GPIO_P0_12,GPIO_INTERRUPT_RISING_EDGE);
 *  UEZGPIOClearIRQ(GPIO_P0_12);
 *  UEZGPIODisableIRQ(GPIO_P0_12,GPIO_INTERRUPT_RISING_EDGE);
 *  // typedef enum {
 *  //     GPIO_INTERRUPT_FALLING_EDGE,
 *  //     GPIO_INTERRUPT_RISING_EDGE,
 *  // } T_gpioInterruptType;
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOEnableIRQ(
        T_uezGPIOPortPin aPortPin,
        T_gpioInterruptType aType)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->EnableInterrupts(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin),
            aType);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIODisableIRQ
 *---------------------------------------------------------------------------*/
/**
 *  Disable the pin's ability to generate interrupts.
 *
 *  @param [in]    aPortPin 	Port pin to affect
 *
 *  @param [in]    aType  		Type of interrupt
 *
 *  @return        T_uezError 	Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOEnableIRQ(GPIO_P0_12,GPIO_INTERRUPT_RISING_EDGE);
 *  UEZGPIOClearIRQ(GPIO_P0_12);
 *  UEZGPIODisableIRQ(GPIO_P0_12,GPIO_INTERRUPT_RISING_EDGE);
 *  // typedef enum {
 *  //     GPIO_INTERRUPT_FALLING_EDGE,
 *  //     GPIO_INTERRUPT_RISING_EDGE,
 *  // } T_gpioInterruptType;
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIODisableIRQ(
        T_uezGPIOPortPin aPortPin,
        T_gpioInterruptType aType)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->DisableInterrupts(p,
            1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin), aType);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOClearIRQ
 *---------------------------------------------------------------------------*/
/**
 *  Clear's any interrupt flags related to this pin's interrupt.
 *
 *  @param [in]    aPortPin 	Port pin to affect.
 *
 *  @return        T_uezError 	Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOEnableIRQ(GPIO_P0_12,GPIO_INTERRUPT_RISING_EDGE);
 *  UEZGPIOClearIRQ(GPIO_P0_12);
 *  UEZGPIODisableIRQ(GPIO_P0_12,GPIO_INTERRUPT_RISING_EDGE);
 *  // typedef enum {
 *  //     GPIO_INTERRUPT_FALLING_EDGE,
 *  //     GPIO_INTERRUPT_RISING_EDGE,
 *  // } T_gpioInterruptType;
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOClearIRQ(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->ClearInterrupts(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin));
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOConfigureInterruptCallback
 *---------------------------------------------------------------------------*/
/**
 *  Configure a GPIO port's interrupt capability related to this whole
 *      port.  Most processors have a single interrupt when any single
 *      group of pins are activated.
 *
 *  @param [in]    aPort 						Port with interrupt handler.
 *
 *  @param [in]    aInterruptCallback 	 		Interrupt handler to call when
 *  											any pins trigger an interrupt.
 *  @param [in]    *aInterruptCallbackWorkspace Workspace to pass to the
 *				          						interrupt handler (if any).
 *  @return        T_uezError  					Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  T_TS_FourWireTouchResist_Workspace *p =
 *      (T_TS_FourWireTouchResist_Workspace *) aW;
 *	// Setup IRQ handler for these lines
 *  UEZGPIOConfigureInterruptCallback(
 *  UEZ_GPIO_PORT_FROM_PORT_PIN(p->iConfig.iYMinus.iGPIO),
 *           TS_FourWireTouchResistCallback, p);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOConfigureInterruptCallback(
        T_uezGPIOPort aPort,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(aPort);
    return (*p)->ConfigureInterruptCallback(p, aInterruptCallback,
            aInterruptCallbackWorkspace);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOControl
 *---------------------------------------------------------------------------*/
/**
 *  UEZGPIOControl provides for special configuration or control of
 *      a GPIO pin.  Not all processors and/or GPIO pins support the same
 *      functionality of control, but this routine will attempt to do it.
 *
 *  @param [in]    aPortPin 	Port pin to control
 *
 *  @param [in]    aControl 	Control Function requested
 *
 *  @param [in]    aValue 		Value to pass with control.
 *
 *  @return        T_uezError	Error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  T_uezError UEZGPIOControl(GPIO_P0_12,GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0);
 * // typedef enum {
 * // GPIO_NOP=0,
 * // GPIO_CONTROL_ENABLE_INPUT_BUFFER,
 * // GPIO_CONTROL_DISABLE_INPUT_BUFFER,
 * // GPIO_CONTROL_SET_CONFIG_BITS, // system specific
 * // } T_gpioControl;
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOControl(
        T_uezGPIOPortPin aPortPin,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    return (*p)->Control(p, UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin), aControl, aValue);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIORead
 *---------------------------------------------------------------------------*/
/**
 *  Reads the given pin and returns either true or false.
 *
 *  @param [in]    aPortPin 	Port pin to control
 *
 *  @return        TBool 		ETrue if pin is HIGH, or EFalse if pin is LOW.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOInput(GPIO_P0_12);		// set P0_12 to input
 *  if (UEZGPIORead(GPIO_P0_12) == ETrue) {
 *		//port was high
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool UEZGPIORead(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));
    TUInt32 read = 0;

    (*p)->Read(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin), &read);

    return (read) ? ETrue : EFalse;
}

/**
 *  Reads an entire GPIO port
 *
 *  @param [in]    aPort 	    Port to control
 *  @param [out]   aReadValue 	Port read value
 *
 *  @return        T_uezError
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  TUInt32 readValue, i;
 *  UEZGPIOInput(GPIO_P0_12);		// set P0_12 to input
 *  
 *  if (UEZGPIOReadPort(GPIO_P0, &readValue) == UEZ_ERROR_NONE) {
 *		for(i=0; i<NUM_PINS; i++) {
 *          if(readValue>>i)
 *              // Pin 0.i is high
*       }
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZGPIOReadPort(T_uezGPIOPort aPort, TUInt32 *aReadValue)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(aPort);

    return (*p)->Read(p, 0xFFFFFFFF, aReadValue);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOLock
 *---------------------------------------------------------------------------*/
/**
 *  Locks the given pin.  Locking an already locked pin will cause
 *      a UEZ Failure Msg of "PinLock".
 *
 *  @param [in]    aPortPin 		Port pin to control
 *
 *  @return        void
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOLock(GPIO_P0_12); // lock P0_12
 *  UEZGPIOUnlock(GPIO_P0_12); // unlock P0_12
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZGPIOLock(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));

    if (aPortPin != GPIO_NONE)
        (*p)->Lock(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin));
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGPIOUnlock
 *---------------------------------------------------------------------------*/
/**
 *  Unlocks the given pin so it can be locked again later.
 *
 *  @param [in]    aPortPin 	Port pin to control
 *
 *  @return        TBool 		ETrue if pin is HIGH, or EFalse if pin is LOW.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZGPIO.h>
 *
 *  UEZGPIOLock(GPIO_P0_12); // lock P0_12
 *  UEZGPIOUnlock(GPIO_P0_12); // unlock P0_12
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZGPIOUnlock(T_uezGPIOPortPin aPortPin)
{
    HAL_GPIOPort **p = UEZGPIOGetPortOrFail(
            UEZ_GPIO_PORT_FROM_PORT_PIN(aPortPin));

    if (aPortPin != GPIO_NONE)
        (*p)->Unlock(p, 1 << UEZ_GPIO_PIN_FROM_PORT_PIN(aPortPin));
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZGPIO.c
 *-------------------------------------------------------------------------*/
