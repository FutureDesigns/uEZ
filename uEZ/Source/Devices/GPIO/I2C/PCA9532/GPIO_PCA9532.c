/*-------------------------------------------------------------------------*
 * File:  IO_PCA9532.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
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
#include "GPIO_PCA9532.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define PCA9532_I2C_RATE            400//400 /* kHz */

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_GPIOPort *iHAL;
    T_uezDevice iI2C;
    TUInt32 iI2CAddr;
    TUInt32 iLocked;
    /** Bit mask of which pins are inputs, 0=output, 1=input */
    TUInt8 iInput0;
    TUInt8 iInput1;
    TUInt8 iPSCO;
    TUInt8 iPWM0;
    TUInt8 iPSC1;
    TUInt8 iPWM1;
    TUInt8 iLS0;
    TUInt8 iLS1;
    TUInt8 iLS2;
    TUInt8 iLS3;
} T_GPIO_PCA9532_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_InitializeWorkspace(void *aWorkspace)
{
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPIO_PCA9532_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the outputs to the PCA9532 or return an error.
 * Inputs:
 *      T_GPIO_PCA9532_Workspace *p -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IGPIO_PCA9532_Write(T_GPIO_PCA9532_Workspace *p)
{
    TUInt8 data[9];

    data[0] = 0x12; //Start writing at the PSC0 register with auto increment
    data[1] = p->iPSCO;
    data[2] = p->iPWM0;
    data[3] = p->iPSC1;
    data[4] = p->iPWM1;
    data[5] = p->iLS0;
    data[6] = p->iLS1;
    data[7] = p->iLS2;
    data[8] = p->iLS3;

    
    return UEZI2CWrite(p->iI2C, p->iI2CAddr, PCA9532_I2C_RATE, data, 9, 100);
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPIO_PCA9532_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Write the outputs to the PCA9532 or return an error.
 * Inputs:
 *      T_GPIO_PCA9532_Workspace *p -- Particular GPIO workspace
 *      TUInt16 *aV -- Place to put the result
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IGPIO_PCA9532_Read(T_GPIO_PCA9532_Workspace *p, TUInt16 *aV)
{
    TUInt8 data[2];
    T_uezError error;

    // Read in the two bytes that make up the reading
    error = UEZI2CRead(p->iI2C, p->iI2CAddr, PCA9532_I2C_RATE, data, 2, 100);

    // Store the value (regardless of error)
    *aV = (data[1]<<8) | data[0];

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    //    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;

    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_Activate
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
T_uezError GPIO_PCA9532_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPins);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;



    return IGPIO_PCA9532_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;
    


    return IGPIO_PCA9532_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;
    TUInt8 i = 0, bit = 0;

    for(i = 0; i < 32; i++){
        if(aPortPins == (1<<i)){
            break;
        }
        bit++;
    }
    switch(bit){
        case 0:
            p->iLS0 |= (1<<0);
            break;
        case 1:
            p->iLS0 |= (1<<2);
            break;
        case 2:
            p->iLS0 |= (1<<4);
            break;
        case 3:
            p->iLS0 |= (1<<6);
            break;
        case 4:
            p->iLS1 |= (1<<0);
            break;
        case 5:
            p->iLS1 |= (1<<2);
            break;
        case 6:
            p->iLS1 |= (1<<4);
            break;
        case 7:
            p->iLS1 |= (1<<6);
            break;
        case 8:
            p->iLS2 |= (1<<0);
            break;
        case 9:
            p->iLS2 |= (1<<2);
            break;
        case 10:
            p->iLS2 |= (1<<4);
            break;
        case 11:
            p->iLS2 |= (1<<6);
            break;
        case 12:
            p->iLS3 |= (1<<0);
            break;
        case 13:
            p->iLS3 |= (1<<2);
            break;
        case 14:
            p->iLS3 |= (1<<4);
            break;
        case 15:
            p->iLS3 |= (1<<6);
            break;
    }
    return IGPIO_PCA9532_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;
    TUInt8 i = 0, bit = 0;

    for(i = 0; i < 32; i++){
        if(aPortPins == (1<<i)){
            break;
        }
        bit++;
    }

    switch(bit){
        case 0:
            p->iLS0 &= ~(3<<0);
            break;
        case 1:
            p->iLS0 &= ~(3<<2);
            break;
        case 2:
            p->iLS0 &= ~(3<<4);
            break;
        case 3:
            p->iLS0 &= ~(3<<6);
            break;
        case 4:
            p->iLS1 &= ~(3<<0);
            break;
        case 5:
            p->iLS1 &= ~(3<<2);
            break;
        case 6:
            p->iLS1 &= ~(3<<4);
            break;
        case 7:
            p->iLS1 &= ~(3<<6);
            break;
        case 8:
            p->iLS2 &= ~(3<<0);
            break;
        case 9:
            p->iLS2 &= ~(3<<2);
            break;
        case 10:
            p->iLS2 &= ~(3<<4);
            break;
        case 11:
            p->iLS2 &= ~(3<<6);
            break;
        case 12:
            p->iLS3 &= ~(3<<0);
            break;
        case 13:
            p->iLS3 &= ~(3<<2);
            break;
        case 14:
            p->iLS3 &= ~(3<<4);
            break;
        case 15:
            p->iLS3 &= ~(3<<6);
            break;
    }


    return IGPIO_PCA9532_Write(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_Read
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
T_uezError GPIO_PCA9532_Read(
        void *aWorkspace,
        TUInt32 aPortPins,
        TUInt32 *aPinsRead)
{
    TUInt16 v;
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;
    T_uezError error;

    // Get the bits
    error = IGPIO_PCA9532_Read(p, &v);
    *aPinsRead = (v & aPortPins);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  GPIO_PCA9532_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError GPIO_PCA9532_SetPull(
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
 * Routine:  GPIO_PCA9532_SetMux
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
T_uezError GPIO_PCA9532_SetMux(
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
 * Routine:  GPIO_PCA9532_GetMux
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
T_uezError GPIO_PCA9532_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aPortPinIndex);

    *aMux = 0;

    return UEZ_ERROR_NONE;
}

T_uezError GPIO_PCA9532_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCA9532_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCA9532_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCA9532_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aInterruptCallback);PARAM_NOT_USED(aInterruptCallbackWorkspace);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError GPIO_PCA9532_Control(
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

T_uezError GPIO_PCA9532_Lock(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        UEZFailureMsg("PinLock PCA9532");
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError GPIO_PCA9532_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_GPIO_PCA9532_Workspace *p = (T_GPIO_PCA9532_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_PCA9532_Interface = {
        {
        "GPIO:PCA9532",
        0x0214,
        GPIO_PCA9532_InitializeWorkspace,
        sizeof(T_GPIO_PCA9532_Workspace),
        },

        GPIO_PCA9532_Configure,
        GPIO_PCA9532_Activate,
        GPIO_PCA9532_Deactivate,
        GPIO_PCA9532_SetOutputMode,
        GPIO_PCA9532_SetInputMode,
        GPIO_PCA9532_Set,
        GPIO_PCA9532_Clear,
        GPIO_PCA9532_Read,
        GPIO_PCA9532_SetPull,
        GPIO_PCA9532_SetMux,
        GPIO_PCA9532_ConfigureInterruptCallback_NotSupported,
        GPIO_PCA9532_EnableInterrupts_NotSupported,
        GPIO_PCA9532_DisableInterrupts_NotSupported,
        GPIO_PCA9532_ClearInterrupts_NotSupported,
        GPIO_PCA9532_Control,
        GPIO_PCA9532_Lock,
        GPIO_PCA9532_Unlock,
        GPIO_PCA9532_GetMux };

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void GPIO_PCA9532_Create(
    const char *aName,
    T_uezGPIOPort aGPIOPort,
    const char *aI2CName,
    TUInt8 aI2CAddr)
{
    T_GPIO_PCA9532_Workspace *p;

    // Register GPIO3 and allow all pins
    HALInterfaceRegister(aName, (T_halInterface *)&GPIO_PCA9532_Interface, 0,
        (T_halWorkspace **)&p);
    UEZI2COpen(aI2CName, &p->iI2C);
    p->iI2CAddr = aI2CAddr;

    p->iInput0 = 0;
    p->iInput1 = 0;
    p->iPSCO = 0;
    p->iPWM0 = 0;
    p->iPSC1 = 0;
    p->iPWM1 = 0;
    p->iLS0 = 0;//(1<<6) | (1<<4) | (1<<2) | (1<<0); // Set all outputs to low
    p->iLS1 = 0;//(1<<6) | (1<<4) | (1<<2) | (1<<0);
    p->iLS2 = 0;//(1<<6) | (1<<4) | (1<<2) | (1<<0);
    p->iLS3 = 0;//(1<<6) | (1<<4) | (1<<2) | (1<<0);

    IGPIO_PCA9532_Write(p);
    UEZGPIORegisterPort(aGPIOPort, (HAL_GPIOPort **)p);
}

/*-------------------------------------------------------------------------*
 * End of File:  IO_PCA9532.c
 *-------------------------------------------------------------------------*/
