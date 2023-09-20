/*-------------------------------------------------------------------------*
 * File:  IO_PCF8574T.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <uEZ.h>
#include <uEZGPIO.h>
#include <uEZI2C.h>
#include "GPIO_PCF8574T.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define PCF8574T_I2C_RATE            400 /* kHz */

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_GPIOPort *iHAL;
    T_uezDevice iI2C;
    TUInt32 iI2CAddr;
    TUInt32 iLocked;
    /** Bit mask of which pins are inputs, 0=output, 1=input */
    TUInt8 iIsInput;
    TUInt8 iOutputs;
} T_GPIO_PCF8574T_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_InitializeWorkspace(void *aWorkspace)
{
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPIO_PCF8574T_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the outputs to the PCF8574T or return an error.
 * Inputs:
 *      T_GPIO_PCF8574T_Workspace *p -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IGPIO_PCF8574T_Write(T_GPIO_PCF8574T_Workspace *p)
{
    TUInt8 data;

    // Create an output value.  Inputs must be high.
    data = p->iOutputs | p->iIsInput;
    
    return UEZI2CWrite(p->iI2C, p->iI2CAddr, PCF8574T_I2C_RATE, &data, 1, 100);
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPIO_PCF8574T_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the outputs to the PCF8574T or return an error.
 * Inputs:
 *      T_GPIO_PCF8574T_Workspace *p -- Particular GPIO workspace
 *      TUInt8 *aV -- Place to put the result
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IGPIO_PCF8574T_Read(T_GPIO_PCF8574T_Workspace *p, TUInt8 *aV)
{
    // Read in the two bytes that make up the reading
    return UEZI2CRead(p->iI2C, p->iI2CAddr, PCF8574T_I2C_RATE, aV, 1, 100);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_Activate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are active as GPIO pins.
 *      Once active, the pins are set to GPIO inputs or outputs.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;

    // Output mode is set by putting 0's in the iIsInput register
    p->iIsInput &= ~(aPortPins);
    IGPIO_PCF8574T_Write(p);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;

    // Output mode is set by putting 1's in the iIsInput register
    p->iIsInput |= (aPortPins);
    IGPIO_PCF8574T_Write(p);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;

    // Output is set by putting 1's in the iOutput register
    p->iOutputs |= (aPortPins);
    IGPIO_PCF8574T_Write(p);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;

    // Output is set by putting 0's in the iOutput register
    p->iOutputs &= ~(aPortPins);
    IGPIO_PCF8574T_Write(p);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a group of GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 *      TUInt32 *aPinsRead          -- Pointer to pins result.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_Read(
        void *aWorkspace,
        TUInt32 aPortPins,
        TUInt32 *aPinsRead)
{
    T_uezError error;
    TUInt8 v;
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;

    // Get the bits
    error = IGPIO_PCF8574T_Read(p, &v);
    *aPinsRead = (v & aPortPins);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_SetPull(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioPull aPull)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPinIndex);
    PARAM_NOT_USED(aPull);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_SetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pin's mux feature.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 *      T_gpioMux aMux              -- Mux setting to use
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_SetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux aMux)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPinIndex);

    if (aMux != 0)
        return UEZ_ERROR_NOT_SUPPORTED;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8574T_GetMux
 *---------------------------------------------------------------------------*
 * Description:
 *      Get pin's mux feature.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 *      T_gpioMux aMux              -- Mux setting being used
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8574T_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPinIndex);

    *aMux = 0;

    return UEZ_ERROR_NONE;
}

T_uezError GPIO_PCF8574T_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8574T_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8574T_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8574T_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aInterruptCallback);PARAM_NOT_USED(aInterruptCallbackWorkspace);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8574T_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPinIndex);
    PARAM_NOT_USED(aControl);
    PARAM_NOT_USED(aValue);

    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8574T_Lock(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        UEZFailureMsg("PinLock PCF8574T");
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError GPIO_PCF8574T_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8574T_Workspace *p = (T_GPIO_PCF8574T_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_PCF8574T_Interface = {
        {
        "GPIO:PCF8574T",
        0x0214,
        GPIO_PCF8574T_InitializeWorkspace,
        sizeof(T_GPIO_PCF8574T_Workspace),
        },

        GPIO_PCF8574T_Configure,
        GPIO_PCF8574T_Activate,
        GPIO_PCF8574T_Deactivate,
        GPIO_PCF8574T_SetOutputMode,
        GPIO_PCF8574T_SetInputMode,
        GPIO_PCF8574T_Set,
        GPIO_PCF8574T_Clear,
        GPIO_PCF8574T_Read,
        GPIO_PCF8574T_SetPull,
        GPIO_PCF8574T_SetMux,
        GPIO_PCF8574T_ConfigureInterruptCallback_NotSupported,
        GPIO_PCF8574T_EnableInterrupts_NotSupported,
        GPIO_PCF8574T_DisableInterrupts_NotSupported,
        GPIO_PCF8574T_ClearInterrupts_NotSupported,
        GPIO_PCF8574T_Control,
        GPIO_PCF8574T_Lock,
        GPIO_PCF8574T_Unlock,
        GPIO_PCF8574T_GetMux };

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void GPIO_PCF8574T_Create(
    const char *aName,
    T_uezGPIOPort aGPIOPort,
    const char *aI2CName,
    TUInt8 aI2CAddr)
{
    T_GPIO_PCF8574T_Workspace *p;

    // Register GPIO3 and allow all pins
    HALInterfaceRegister(aName, (T_halInterface *)&GPIO_PCF8574T_Interface, 0,
        (T_halWorkspace **)&p);
    UEZI2COpen(aI2CName, &p->iI2C);
    p->iI2CAddr = aI2CAddr;
    p->iIsInput = 0xFF;
    p->iOutputs = 0xFF;

    UEZGPIORegisterPort(aGPIOPort, (HAL_GPIOPort **)p);
}

/*-------------------------------------------------------------------------*
 * End of File:  IO_PCF8574T.c
 *-------------------------------------------------------------------------*/
