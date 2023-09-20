/*-------------------------------------------------------------------------*
 * File:  Keypad_Generic_GPIO.c
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include "Keypad_Generic_GPIO.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct _T_keypadRegistered {
    struct _T_keypadRegistered *iNext;
    T_uezQueue iQueue;
} T_keypadRegistered;

typedef struct {
    const DEVICE_Keypad *iDevice;
    T_uezSemaphore iSem;
    T_keypadRegistered *iRegisteredQueues;
    TUInt32 iGPIOPort;
    const T_GPIOKeypadAssignment *iKeyAssignment;
    TUInt32 iNumKeys;
    T_KeypadGPIOPolarity iGPIOPolarity;
    TBool iNumOpen;
    TBool iStopTask;
    T_uezTask iMonitorTask;
    T_uezGPIOPortPin iPowerLedPortPin;
    T_uezGPIOPortPin iKeyPressLedPortPin;
} T_Keypad_Generic_GPIO_Workspace;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define IGrab(p)         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease(p)      UEZSemaphoreRelease(p->iSem)

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void Keypad_Generic_GPIO_Monitor(
        T_uezTask aMyTask, 
        void *aParameters);

/*---------------------------------------------------------------------------*
 * Routine:  IKeypad_Generic_GPIO_Scan
 *---------------------------------------------------------------------------*
 * Description:
 *      Scan the keypad for a key.  If none is found, use KEY_NONE.
 * Inputs:
 *      T_Keypad_Generic_GPIO_Workspace *p -- Workspace
 *      TUInt32 *aKey -- Pointer to scan key to return.
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IKeypad_Generic_GPIO_Scan(
        T_Keypad_Generic_GPIO_Workspace *p, 
        TUInt32 *aKey)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i, readValue;

    *aKey = KEY_NONE;
    
    IGrab(p);

    error = UEZGPIOReadPort(p->iGPIOPort, &readValue);
    if(!error) {
        for(i=0; i<p->iNumKeys; i++) {
            if(p->iGPIOPolarity == KEYPAD_LOW_TRUE_SIGNALS) {
                if(!((readValue>>p->iKeyAssignment[i].iPin) & 1))
                   *aKey = p->iKeyAssignment[i].iKey;
            } else /* High True */ {
                if((readValue>>p->iKeyAssignment[i].iPin) & 1)
                   *aKey = p->iKeyAssignment[i].iKey;
            }
        }
    }

    IRelease(p);
    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  IQueueAdd
 *---------------------------------------------------------------------------*
 * Description:
 *      Add another registered queue to the list
 * Inputs:
 *      T_Keypad_Generic_GPIO_Workspace *p -- Workspace
 *      T_uezQueue aNewQueue        -- New queue to add
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IQueueAdd(
        T_Keypad_Generic_GPIO_Workspace *p, 
        T_uezQueue aQueue)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_keypadRegistered *p_reg;

    p_reg = UEZMemAlloc(sizeof(T_keypadRegistered));
    if (p_reg) {
        p_reg->iQueue = aQueue;
        p_reg->iNext = p->iRegisteredQueues;
        p->iRegisteredQueues = p_reg;
    } else {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IQueueRemove
 *---------------------------------------------------------------------------*
 * Description:
 *      Remove previously registered queue from the list.
 * Inputs:
 *      T_Keypad_Generic_GPIO_Workspace *p -- Workspace
 *      T_uezQueue aNewQueue        -- Queue to remove
 *---------------------------------------------------------------------------*/
static void IQueueRemove(
        T_Keypad_Generic_GPIO_Workspace *p, 
        T_uezQueue aQueue)
{
    T_keypadRegistered *p_reg=p->iRegisteredQueues;
    T_keypadRegistered *p_prev=0;

    while (p_reg) {
        // Remove it from the list
        if (p_reg->iQueue == aQueue) {
            if (p_prev)
                p_prev->iNext = p_reg->iNext;
            else
                p->iRegisteredQueues = p_reg->iNext;
            UEZMemFree(p_reg);
            break;
        }

        p_prev = p_reg;
        p_reg = p_reg->iNext;
    }
    
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a workspace for this device.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_InitializeWorkspace(void *aW)
{
    T_Keypad_Generic_GPIO_Workspace *p = 
            (T_Keypad_Generic_GPIO_Workspace *)aW;

    // Nobody has registered yet
    p->iNumOpen = 0;
    p->iRegisteredQueues = 0;
    
    // Then create a semaphore to limit the number of accessors
    return UEZSemaphoreCreateBinary(&p->iSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the GPIO direction and start the monitor task
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_Open(void *aWorkspace)
{
    T_Keypad_Generic_GPIO_Workspace *p = 
        (T_Keypad_Generic_GPIO_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    IGrab(p);

    p->iNumOpen++;
    if (p->iNumOpen==1) {
        
        for(i=0;i<p->iNumKeys;i++)
            UEZGPIOInput(UEZ_GPIO_PORT_PIN(p->iGPIOPort, p->iKeyAssignment[i].iPin));
        
        p->iStopTask = EFalse;
        error = UEZTaskCreate(
            (T_uezTaskFunction)Keypad_Generic_GPIO_Monitor,
            "Keypad",
            128,
            aWorkspace,
            UEZ_PRIORITY_HIGH,
            &p->iMonitorTask);
        
    } else {
        error = UEZ_ERROR_ALREADY_EXISTS;
    }
    
    if(p->iKeyPressLedPortPin != GPIO_NONE) {
        UEZGPIOOutput(p->iKeyPressLedPortPin);
        UEZGPIOSet(p->iKeyPressLedPortPin);
    }
    
    if(p->iPowerLedPortPin != GPIO_NONE) {
        UEZGPIOOutput(p->iPowerLedPortPin);
        UEZGPIOClear(p->iPowerLedPortPin);
    }
    
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare the number of users
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_Close(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_Monitor
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a task to run and monitor the keypad for presses.
 * Inputs:
 *      T_uezTask aMyTask           -- This task
 *      void *aParameters           -- Passed in workspace
 *---------------------------------------------------------------------------*/
static void Keypad_Generic_GPIO_Monitor(
        T_uezTask aMyTask, 
        void *aParameters)
{
    T_Keypad_Generic_GPIO_Workspace *p = 
        (T_Keypad_Generic_GPIO_Workspace *)aParameters;
    TUInt32 key;
    TUInt32 key2;
    T_uezInputEvent event;
    T_keypadRegistered *p_reg;

    PARAM_NOT_USED(aMyTask);

    // Sit here and wait for an event
    while (!p->iStopTask) {
        // At this point, the keypad is fully grabbed, so we'll
        // wait for a release to occur.
        UEZTaskDelay(50);

        // Process the keypad interrupt
        IKeypad_Generic_GPIO_Scan(p, &key);
        if (key != KEY_NONE) {
            // Don't let others access the keypad while we process
            IGrab(p);

            // Send it out a press event to everyone
            event.iType = INPUT_EVENT_TYPE_BUTTON;
            event.iEvent.iButton.iKey = key;
            event.iEvent.iButton.iAction = BUTTON_ACTION_PRESS;
            for (p_reg = p->iRegisteredQueues; p_reg; p_reg = p_reg->iNext) {
                // Send the event, but don't block.
                UEZQueueSend(p_reg->iQueue, (void *)&event, 0);
            }
            
            if(p->iKeyPressLedPortPin != GPIO_NONE)
                UEZGPIOClear(p->iKeyPressLedPortPin);

            // Done, return to our control
            IRelease(p);

            // Watch the scan waiting for no press every at 10 ms intervals
            while (1) {
                if (IKeypad_Generic_GPIO_Scan(p, &key2) != UEZ_ERROR_NONE)
                    break;
                if (key2 == KEY_NONE)
                    break;
                UEZTaskDelay(10);
            }

            // Don't let others access the keypad while we process
            IGrab(p);

            // Finally, no button is pressed
            // Send out a press release to everyone
            event.iType = INPUT_EVENT_TYPE_BUTTON;
            event.iEvent.iButton.iKey = key;
            event.iEvent.iButton.iAction = BUTTON_ACTION_RELEASE;
            for (p_reg = p->iRegisteredQueues; p_reg; p_reg = p_reg->iNext) {
                // Send the event, but don't block.
                UEZQueueSend(p_reg->iQueue, (void *)&event, 0);
            }

            if(p->iKeyPressLedPortPin != GPIO_NONE)
                UEZGPIOSet(p->iKeyPressLedPortPin);
            
            // Done, return to our control
            IRelease(p);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_Register
 *---------------------------------------------------------------------------*
 * Description:
 *      Register a queue to receive T_uezInputEvent entries from the keypad.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      T_uezQueue                  -- Queue to receive events
 * Outputs:
 *      
 *---------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_Register(
        void *aWorkspace, 
        T_uezQueue aQueue)
{
    T_Keypad_Generic_GPIO_Workspace *p = 
        (T_Keypad_Generic_GPIO_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    error = IQueueAdd(p, aQueue);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_Unregister
 *---------------------------------------------------------------------------*
 * Description:
 *      Remove the given queue from the list of event queues.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      T_uezQueue                  -- Queue to remove from events
 * Outputs:
 *      
 *---------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_Unregister(
        void *aWorkspace, 
        T_uezQueue aQueue)
{
    T_Keypad_Generic_GPIO_Workspace *p = 
        (T_Keypad_Generic_GPIO_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    IQueueRemove(p, aQueue);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_Scan
 *---------------------------------------------------------------------------*
 * Description:
 *      Force an immediate scan of the key pad.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 *aScanKey           -- Resulting scan key.  If KEY_NONE, then
 *                                      no key is being pressed.
 * Outputs:
 *      T_uezError                  -- Error if a problem.
 *---------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_Scan(
        void *aWorkspace, 
        TUInt32 *aScanKey)
{
    T_Keypad_Generic_GPIO_Workspace *p = 
        (T_Keypad_Generic_GPIO_Workspace *)aWorkspace;
    
    return IKeypad_Generic_GPIO_Scan(p, aScanKey);
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_Generic_GPIO_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      
 * Inputs:
 *      const char *aName,
 *      TUInt32 iGPIOPort,
 *      T_GPIOKeypadAssignment *iButtonBoardKeys,
 *      T_KeypadGPIOPolarity iGPIOPolarity
 *---------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_Create(
    const char *aName,
    TUInt32 aGPIOPort,
    const T_GPIOKeypadAssignment *aKeyAssignment,
    TUInt32 aNumKeys,
    T_KeypadGPIOPolarity aGPIOPolarity,
    T_uezGPIOPortPin aPowerLedPortPin,
    T_uezGPIOPortPin aKeyPressLedPortPin)
{
    T_Keypad_Generic_GPIO_Workspace *p;
    T_uezError error;

    error = UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Keypad_Generic_GPIO_Interface, 0,
            (T_uezDeviceWorkspace **)&p);
    
    p->iGPIOPort = aGPIOPort;
    p->iKeyAssignment = aKeyAssignment;
    p->iNumKeys = aNumKeys;
    p->iGPIOPolarity = aGPIOPolarity;
    p->iPowerLedPortPin = aPowerLedPortPin;
    p->iKeyPressLedPortPin = aKeyPressLedPortPin;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Keypad Keypad_Generic_GPIO_Interface = {
	{
	    // Common device interface
	    "Keypad:GPIO",
	    0x0100,
	    Keypad_Generic_GPIO_InitializeWorkspace,
	    sizeof(T_Keypad_Generic_GPIO_Workspace),
	},
	
    // Functions
    Keypad_Generic_GPIO_Open,
    Keypad_Generic_GPIO_Close,
    Keypad_Generic_GPIO_Scan,
    Keypad_Generic_GPIO_Register,
    Keypad_Generic_GPIO_Unregister,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  Keypad_Generic_GPIO.c
 *-------------------------------------------------------------------------*/
