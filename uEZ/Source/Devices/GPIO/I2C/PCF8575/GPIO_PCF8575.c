/*-------------------------------------------------------------------------*
 * File:  IO_PCF8575.c
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
#include "GPIO_PCF8575.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define PCF8575_I2C_RATE            400 /* kHz */

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_GPIOPort *iHAL;
    T_uezDevice iI2C;
    TUInt32 iI2CAddr;
    TUInt32 iLocked;
    /** Bit mask of which pins are inputs, 0=output, 1=input */
    TUInt16 iIsInput;
    TUInt16 iOutputs;
} T_GPIO_PCF8575_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_InitializeWorkspace(void *aWorkspace)
{
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPIO_PCF8575_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the outputs to the PCF8575 or return an error.
 * Inputs:
 *      T_GPIO_PCF8575_Workspace *p -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IGPIO_PCF8575_Write(T_GPIO_PCF8575_Workspace *p)
{
    TUInt8 data[2];

    // Create an output value.  Inputs must be high.
    TUInt16 v = p->iOutputs | p->iIsInput;

    data[0] = v & 0xFF; // Low bits
    data[1] = (v >> 8) & 0xFF; // High bits
    
    return UEZI2CWrite(p->iI2C, p->iI2CAddr, PCF8575_I2C_RATE, data, 2, 100);
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPIO_PCF8575_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the outputs to the PCF8575 or return an error.
 * Inputs:
 *      T_GPIO_PCF8575_Workspace *p -- Particular GPIO workspace
 *      TUInt16 *aV -- Place to put the result
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IGPIO_PCF8575_Read(T_GPIO_PCF8575_Workspace *p, TUInt16 *aV)
{
    TUInt8 data[2];
    T_uezError error;

    // Read in the two bytes that make up the reading
    error = UEZI2CRead(p->iI2C, p->iI2CAddr, PCF8575_I2C_RATE, data, 2, 100);

    // Store the value (regardless of error)
    *aV = (data[1]<<8) | data[0];

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    //    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;

    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_Activate
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
T_uezError GPIO_PCF8575_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;

    // Output mode is set by putting 0's in the iIsInput register
    p->iIsInput &= ~(aPortPins);

    return IGPIO_PCF8575_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;
    
    // Output mode is set by putting 1's in the iIsInput register
    p->iIsInput |= (aPortPins);

    return IGPIO_PCF8575_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;

    // Output is set by putting 1's in the iOutput register
    p->iOutputs |= (aPortPins);

    return IGPIO_PCF8575_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;

    // Output is set by putting 0's in the iOutput register
    p->iOutputs &= ~(aPortPins);

    return IGPIO_PCF8575_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_Read
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
T_uezError GPIO_PCF8575_Read(
        void *aWorkspace,
        TUInt32 aPortPins,
        TUInt32 *aPinsRead)
{
    TUInt16 v;
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;
    T_uezError error;

    // Get the bits
    error = IGPIO_PCF8575_Read(p, &v);
    *aPinsRead = (v & aPortPins);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCF8575_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCF8575_SetPull(
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
 * Routine:  GPIO_PCF8575_SetMux
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
T_uezError GPIO_PCF8575_SetMux(
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
 * Routine:  GPIO_PCF8575_GetMux
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
T_uezError GPIO_PCF8575_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPinIndex);

    *aMux = 0;

    return UEZ_ERROR_NONE;
}

T_uezError GPIO_PCF8575_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8575_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8575_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8575_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aInterruptCallback);PARAM_NOT_USED(aInterruptCallbackWorkspace);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCF8575_Control(
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

T_uezError GPIO_PCF8575_Lock(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        UEZFailureMsg("PinLock PCF8575");
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError GPIO_PCF8575_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCF8575_Workspace *p = (T_GPIO_PCF8575_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_PCF8575_Interface = {
        {
        "GPIO:PCF8575",
        0x0214,
        GPIO_PCF8575_InitializeWorkspace,
        sizeof(T_GPIO_PCF8575_Workspace),
        },

        GPIO_PCF8575_Configure,
        GPIO_PCF8575_Activate,
        GPIO_PCF8575_Deactivate,
        GPIO_PCF8575_SetOutputMode,
        GPIO_PCF8575_SetInputMode,
        GPIO_PCF8575_Set,
        GPIO_PCF8575_Clear,
        GPIO_PCF8575_Read,
        GPIO_PCF8575_SetPull,
        GPIO_PCF8575_SetMux,
        GPIO_PCF8575_ConfigureInterruptCallback_NotSupported,
        GPIO_PCF8575_EnableInterrupts_NotSupported,
        GPIO_PCF8575_DisableInterrupts_NotSupported,
        GPIO_PCF8575_ClearInterrupts_NotSupported,
        GPIO_PCF8575_Control,
        GPIO_PCF8575_Lock,
        GPIO_PCF8575_Unlock,
        GPIO_PCF8575_GetMux };

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void GPIO_PCF8575_Create(
    const char *aName,
    T_uezGPIOPort aGPIOPort,
    const char *aI2CName,
    TUInt8 aI2CAddr)
{
    T_GPIO_PCF8575_Workspace *p;

    // Register GPIO3 and allow all pins
    HALInterfaceRegister(aName, (T_halInterface *)&GPIO_PCF8575_Interface, 0,
        (T_halWorkspace **)&p);
    UEZI2COpen(aI2CName, &p->iI2C);
    p->iI2CAddr = aI2CAddr;
    p->iIsInput = 0xFFFF;
    p->iOutputs = 0xFFFF;

    UEZGPIORegisterPort(aGPIOPort, (HAL_GPIOPort **)p);
}

/*-------------------------------------------------------------------------*
 * End of File:  IO_PCF8575.c
 *-------------------------------------------------------------------------*/
