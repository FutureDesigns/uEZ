/*-------------------------------------------------------------------------*
 * File:  LED_Generic_GPIO.c
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
#include <uEZ.h>
#include <Device/LEDBank.h>
#include <uEZDeviceTable.h>
#include <HAL/GPIO.h>
#include "LED_Generic_GPIO.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef LED_GENERIC_GPIO_MAX_NUM_GPIOS
#define LED_GENERIC_GPIO_MAX_NUM_GPIOS          32
#endif

#define STATE_ON                        0
#define STATE_OFF                       1
#define STATE_UNKNOWN                   2
//#define STATE_BLINK0                    2
//#define STATE_BLINK1                    3

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt8 iLEDState;
    HAL_GPIOPort **iGPIOPort;
    TUInt8 iGPIOPinIndex;
    TBool iIsHighTrue;
} T_LEDGPIOEntry;

typedef struct {
    const DEVICE_LEDBank *iDevice;
    T_uezSemaphore iSem;
    T_LEDGPIOEntry iEntries[LED_GENERIC_GPIO_MAX_NUM_GPIOS];
    TUInt8 iNumEntries;
} T_LED_Generic_GPIO_Workspace;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  LED_Generic_GPIO_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for a generic GPIO driver.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_Generic_GPIO_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_LED_Generic_GPIO_Workspace *p = (T_LED_Generic_GPIO_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    p->iNumEntries = 0;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_Generic_GPIO_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current PCA9551 LEDBank reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_Generic_GPIO_On(void *aWorkspace, TUInt32 aLEDs)
{
    T_LED_Generic_GPIO_Workspace *p =
        (T_LED_Generic_GPIO_Workspace *)aWorkspace;
    TUInt32 bitMask;
    TUInt8 i;
    T_LEDGPIOEntry *p_entry;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p_entry = p->iEntries;
    for (i = 0, bitMask = 1; i < p->iNumEntries; i++, bitMask <<= 1, p_entry++) {
        // Set the bits to on if bit is set
        if (aLEDs & bitMask) {
            p_entry->iLEDState = STATE_ON;
            if (p_entry->iIsHighTrue) {
                (*p_entry->iGPIOPort)->Set(p_entry->iGPIOPort, (1
                    << p_entry->iGPIOPinIndex));
            } else {
                (*p_entry->iGPIOPort)->Clear(p_entry->iGPIOPort, (1
                    << p_entry->iGPIOPinIndex));
            }
            (*p_entry->iGPIOPort)->SetOutputMode(p_entry->iGPIOPort, (1
                << p_entry->iGPIOPinIndex));
        }
    }
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_Generic_GPIO_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current PCA9551 LEDBank reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_Generic_GPIO_Off(void *aWorkspace, TUInt32 aLEDs)
{
    T_LED_Generic_GPIO_Workspace *p =
        (T_LED_Generic_GPIO_Workspace *)aWorkspace;
    TUInt32 bitMask;
    TUInt8 i;
    T_LEDGPIOEntry *p_entry;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p_entry = p->iEntries;
    for (i = 0, bitMask = 1; i < p->iNumEntries; i++, bitMask <<= 1, p_entry++) {
        // Set the bits to on if bit is set
        if (aLEDs & bitMask) {
            p_entry->iLEDState = STATE_OFF;
            if (p_entry->iIsHighTrue) {
                (*p_entry->iGPIOPort)->Clear(p_entry->iGPIOPort, (1
                    << p_entry->iGPIOPinIndex));
            } else {
                (*p_entry->iGPIOPort)->Set(p_entry->iGPIOPort, (1
                    << p_entry->iGPIOPinIndex));
            }
            (*p_entry->iGPIOPort)->SetOutputMode(p_entry->iGPIOPort, (1
                << p_entry->iGPIOPinIndex));
        }
    }
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_Generic_GPIO_Blink
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink the given LEDs with the blink register.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aBlinkReg           -- Blink register used to control blink
 *      TUInt32 aLEDs               -- Bit mask of LEDs to blink
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_Generic_GPIO_Blink(
    void *aWorkspace,
    TUInt32 aBlinkReg,
    TUInt32 aLEDs)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_Generic_GPIO_SetBlinkRate
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the rate of a given blink register (as close as possible).
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aBlinkReg           -- Blink register used to control blink
 *      TUInt32 aPeriod             -- Period in ms
 *      TUInt8 aDutyCycle           -- Duty cycle in 0 (0%) to 255 (100%)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_Generic_GPIO_SetBlinkRate(
    void *aWorkspace,
    TUInt32 aBlinkReg,
    TUInt32 aPeriod,
    TUInt8 aDutyCycle)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_Generic_GPIO_Configure_AddLED
 *---------------------------------------------------------------------------*
 * Description:
 *      Add an LED to the list of LEDs in this LED Generic GPIO driver.
 *      The bit number assigned to each entry starts at 0 and goes to
 *      the limit of the driver (usually 32 total).
 *      NOTE: This routine is NOT semaphore and expected to be called
 *          before the RTOS is up and running.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_GneericLEDGPIODefinition *iDefinition -- LED definition structure
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_Generic_GPIO_Configure_AddLED(
    void *aWorkspace,
    const T_GenericLEDGPIODefinition *aDefinition)
{
    T_LED_Generic_GPIO_Workspace *p =
        (T_LED_Generic_GPIO_Workspace *)aWorkspace;
    T_LEDGPIOEntry *p_entry;
    HAL_GPIOPort **p_gpio;
    T_uezError error;

    error = HALInterfaceFind(aDefinition->iGPIOPortName,
        (T_halWorkspace **)&p_gpio);
    if (error)
        return error;

    if (p->iNumEntries < LED_GENERIC_GPIO_MAX_NUM_GPIOS) {
        p_entry = p->iEntries + p->iNumEntries++;
        p_entry->iGPIOPort = (HAL_GPIOPort **)p_gpio;
        p_entry->iIsHighTrue = aDefinition->iIsHighTrue;
        p_entry->iGPIOPinIndex = aDefinition->iGPIOPinIndex;
        p_entry->iLEDState = STATE_UNKNOWN;
        // Start with LED turned off
        if (p_entry->iIsHighTrue) {
            (*p_gpio)->Set(*p_gpio, (1 << p_entry->iGPIOPinIndex));
        } else {
            (*p_gpio)->Clear(*p_gpio, (1 << p_entry->iGPIOPinIndex));
        }
        (*p_gpio)->SetOutputMode(*p_gpio, (1 << p_entry->iGPIOPinIndex));
        return UEZ_ERROR_NONE;
    } else {
        return UEZ_ERROR_OVERFLOW;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_Generic_GPIO_Configure_AddLED
 *---------------------------------------------------------------------------*
 * Description:
 *      Add an LED to the list of LEDs in this LED Generic GPIO driver.
 *      The bit number assigned to each entry starts at 0 and goes to
 *      the limit of the driver (usually 32 total).
 *      NOTE: This routine is NOT semaphore and expected to be called
 *          before the RTOS is up and running.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_Generic_GPIO_Configure_DefineLEDs(
    void *aWorkspace,
    const T_GenericLEDGPIODefinition *aDefinitionArray,
    TUInt8 aNumLEDs)
{
    TUInt8 i;
    T_uezError error = UEZ_ERROR_NONE;

    // Define all the LEDs
    for (i = 0; i < aNumLEDs; i++) {
        error = LED_Generic_GPIO_Configure_AddLED(aWorkspace, aDefinitionArray
            + i);
        if (error)
            break;
    }
    return error;
}

T_uezError LED_Generic_GPIO_Configure_Create(
    const char *aName,
    const T_GenericLEDGPIODefinition *aDefinitionArray,
    TUInt8 aNumLEDs)
{
    void *p;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&LEDBank_Generic_GPIO_Interface, 0,
            (T_uezDeviceWorkspace **)&p);

    return LED_Generic_GPIO_Configure_DefineLEDs(p, aDefinitionArray, aNumLEDs);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LEDBank LEDBank_Generic_GPIO_Interface = { {
// Common device interface
    "LEDBank:Generic:GPIO",
    0x0100,
    LED_Generic_GPIO_InitializeWorkspace,
    sizeof(T_LED_Generic_GPIO_Workspace), },

// Functions
    LED_Generic_GPIO_On,
    LED_Generic_GPIO_Off,
    LED_Generic_GPIO_Blink,
    LED_Generic_GPIO_SetBlinkRate, };

/*-------------------------------------------------------------------------*
 * End of File:  LED_Generic_GPIO.c
 *-------------------------------------------------------------------------*/
