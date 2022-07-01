/*-------------------------------------------------------------------------*
 * File:  Keypad_NXP_I2C_PCA9555.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Keypad for the PCA9555
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
#include <Device/ExternalInterrupt.h>
#include "Keypad_NXP_I2C_PCA9555.h"

#ifndef KEYPAD_USES_EXTERNAL_IRQ
// TBD: This feature doesn't work well yet
#define KEYPAD_USES_EXTERNAL_IRQ        0
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define PCA9555_I2C_SPEED               400 // kHz
#define KEYPAD_EINT2_CHANNEL            2

#define PCA9555_REG_INPUT_0             0x00
#define PCA9555_REG_INPUT_1             0x01
#define PCA9555_REG_OUTPUT_0            0x02
#define PCA9555_REG_OUTPUT_1            0x03
#define PCA9555_REG_POLARITY_INV_0      0x04
#define PCA9555_REG_POLARITY_INV_1      0x05
#define PCA9555_REG_CONFIG_0            0x06
#define PCA9555_REG_CONFIG_1            0x07

#define KEYPAD_NUM_ROWS              6
#define KPAD_NB_COLUMN           6

/*Keypad defines*/
#define P1_KPAD_COLUMN           0x3F     // 6 columns P1.0 to P1.5
#define KEYPAD_COLUMN_MASK_PORT0      0xC0
#define P1_KPAD_ROW_OUT_REG      0xC0     // column configured as output
#define P0_KPAD_ROW              0x3F     // 6 rows P0.0 to P0.5

#define P0_6_BACKLIGHT           0x40     // P0.6 used for backlight control
#define P0_6_BACKLIGHT_OUT_REG   0xBF     // P0.6 configured as output
#define P0_7_LED                 0x80     // P0.7 used for LED control
#define P0_7_LED_OUT_REG         0x7F     // P0.7 configured as output
#define P1_7_6_LED               0xC0     // P1.6-7 used for LED control
#define P1_7_6_LED_OUT_REG       0x3F     // P1.6-7 configured as output

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
    DEVICE_I2C_BUS **iI2C;
    TUInt8 iI2CAddr;
    T_keypadRegistered *iRegisteredQueues;
    TBool iNumOpen;
#if KEYPAD_USES_EXTERNAL_IRQ
    T_uezDevice eintDevice;
    DEVICE_ExternalInterrupt **iEINT;
#endif
    T_uezSemaphore iReady;
    T_uezTask iMonitorTask;
} T_Keypad_NXP_I2C_PCA9555_Workspace;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define IGrab(p)         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease(p)      UEZSemaphoreRelease(p->iSem)

/*---------------------------------------------------------------------------*
 * Routine:  IKeypad_PCA9555_WriteReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a 8-bit register to the PCA9555.
 * Inputs:
 *      T_Keypad_NXP_I2C_PCA9555_Workspace *p -- Workspace
 *      TUInt8 aReg -- Register number
 *      TUInt8 aValue -- Value to write
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IKeypad_PCA9555_WriteReg(
                T_Keypad_NXP_I2C_PCA9555_Workspace *p, 
                TUInt8 aReg,
                TUInt8 aValue)
{
    I2C_Request r;
    TUInt8 data[2];

    data[0] = aReg;
    data[1] = aValue;

    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9555_I2C_SPEED;
    r.iReadLength = 0;
    r.iWriteData = data;
    r.iWriteLength = 2;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Routine:  IKeypad_PCA9555_ReadReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Read an 8-bit register from the PCA9555.
 * Inputs:
 *      T_Keypad_NXP_I2C_PCA9555_Workspace *p -- Workspace
 *      TUInt8 aReg -- Register number
 *      TUInt8 *aValue -- Place to store read value
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IKeypad_PCA9555_ReadReg(
                T_Keypad_NXP_I2C_PCA9555_Workspace *p, 
                TUInt8 aReg,
                TUInt8 *aValue)
{
    I2C_Request r;

    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9555_I2C_SPEED;
    r.iWriteData = &aReg;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadLength = 1;
    r.iReadData = aValue;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Routine:  IKeypad_PCA9555_Scan
 *---------------------------------------------------------------------------*
 * Description:
 *      Scan the keypad for a key.  If none is found, use KEY_NONE.
 * Inputs:
 *      T_Keypad_NXP_I2C_PCA9555_Workspace *p -- Workspace
 *      TUInt32 *aKey -- Pointer to scan key to return.
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IKeypad_PCA9555_Scan(
        T_Keypad_NXP_I2C_PCA9555_Workspace *p, 
        TUInt32 *aKey)
{
    const TUInt8 rowCode[KEYPAD_NUM_ROWS] = {
        0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0
    };
    const TUInt16 keypadMapping[KPAD_NB_COLUMN][KEYPAD_NUM_ROWS] = {
        { KEY_END_ON_OFF, KEY_END_ON_OFF, KEY_END_ON_OFF, KEY_END_ON_OFF, KEY_END_ON_OFF, KEY_END_ON_OFF },
        { KEY_NONE, KEY_POUND, KEY_0, KEY_ASTERISK, KEY_9, KEY_8 },
        { KEY_NONE, KEY_NONE, KEY_7, KEY_6, KEY_5, KEY_4 },
        { KEY_NONE, KEY_3, KEY_2, KEY_1, KEY_CLEAR, KEY_SEND },
        { KEY_MENU, KEY_ARROW_DOWN, KEY_MINUS, KEY_ARROW_RIGHT, KEY_OK, KEY_ARROW_LEFT },
        { KEY_F4, KEY_ARROW_UP, KEY_PLUS, KEY_F3, KEY_F2, KEY_F1 },
    };
    TUInt8 scan;
    TUInt8 row=0;
    TUInt8 column=0;
    T_uezError error = UEZ_ERROR_NONE;
    TBool found = EFalse;
    TUInt8 rowTest;

    IGrab(p);

    // First, read the low 8 bits regardless of proper decoding 
    error = IKeypad_PCA9555_ReadReg(p, PCA9555_REG_INPUT_0, &scan);
    if (!error) {
        // Any keys touched?
        if ((scan | KEYPAD_COLUMN_MASK_PORT0) != 0xFF) {
            // Determine which key is being pressed
            // decode the column
            switch (scan | KEYPAD_COLUMN_MASK_PORT0) {
                case 0xFE:  
                    column = 0;
                    break;
                case 0xFD:  
                    column = 1;
                    break;
                case 0xFB:  
                    column = 2;
                    break;
                case 0xF7:  
                    column = 3;
                    break;
                case 0xEF:  
                    column = 4;
                    break;
                case 0xDF:  
                    column = 5;
                    break;
                default:    
                    break;
            }

            // Now that he column has been determined, which row is it
            for (rowTest=0; rowTest<KEYPAD_NUM_ROWS; rowTest++) {
                // Setup a to scan a single column
                IKeypad_PCA9555_WriteReg(
                        p,
                        PCA9555_REG_OUTPUT_1, 
                        rowCode[rowTest]);

                // Now read the output
                IKeypad_PCA9555_ReadReg(p, PCA9555_REG_INPUT_0, &scan);

                // Is any of these on? (low = on)
                if ((scan | KEYPAD_COLUMN_MASK_PORT0) != 0xFF) {
                    // Record if we found it
                    if (!found) {
                        row = rowTest;
                        found = ETrue;
                    }
                }
            }
            // map it
            if (found)
                *aKey = keypadMapping[column][row];
        } else {
            // No key is being pressed now
            *aKey = KEY_NONE;
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
 *      T_Keypad_NXP_I2C_PCA9555_Workspace *p -- Workspace
 *      T_uezQueue aNewQueue        -- New queue to add
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IQueueAdd(
        T_Keypad_NXP_I2C_PCA9555_Workspace *p, 
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
 *      T_Keypad_NXP_I2C_PCA9555_Workspace *p -- Workspace
 *      T_uezQueue aNewQueue        -- Queue to remove
 *---------------------------------------------------------------------------*/
static void IQueueRemove(
        T_Keypad_NXP_I2C_PCA9555_Workspace *p, 
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
 * Routine:  Keypad_NXP_I2C_PCA9555_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCA9555.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Keypad_NXP_I2C_PCA9555_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
            (T_Keypad_NXP_I2C_PCA9555_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    // Nobody has registered yet
    p->iRegisteredQueues = 0;
    p->iNumOpen = 0;

#if KEYPAD_USES_EXTERNAL_IRQ
    error = UEZSemaphoreCreateCounting(&p->iReady, 1, 0);
#endif

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_I2C_PCA9555_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Increment the number of users
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Keypad_NXP_I2C_PCA9555_Open(void *aWorkspace)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);

    p->iNumOpen++;
    if (p->iNumOpen==1) {
        // Setup the keypad's input/output pins
        IKeypad_PCA9555_WriteReg(p, 0x06, P0_7_LED_OUT_REG & P0_6_BACKLIGHT_OUT_REG);
        IKeypad_PCA9555_WriteReg(p, 0x07, P1_7_6_LED_OUT_REG & P1_KPAD_ROW_OUT_REG);

        // LEDs off
        IKeypad_PCA9555_WriteReg(p, 0x03, 0xC0);

#if KEYPAD_USES_EXTERNAL_IRQ
        // First to open?  Enable interrupts
        error = (*p->iEINT)->Enable(p->iEINT, KEYPAD_EINT2_CHANNEL);
#endif
    }

    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_I2C_PCA9555_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare the number of users
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Keypad_NXP_I2C_PCA9555_Close(void *aWorkspace)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);

    if (p->iNumOpen) {
        p->iNumOpen--;
        if (p->iNumOpen==0) {
#if KEYPAD_USES_EXTERNAL_IRQ
            // First to open?  Enable interrupts
            error = (*p->iEINT)->Disable(p->iEINT, KEYPAD_EINT2_CHANNEL);
#endif
        }
    }

    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_PCA9555_Callback
 *---------------------------------------------------------------------------*
 * Description:
 *      External interrupt callback.  This routine is inside an ISR, so be
 *      very careful.  All this does is let the monitoring task run.
 * Inputs:
 *      T_uezTask aMyTask           -- This task
 *      void *aParameters           -- Passed in workspace
 *---------------------------------------------------------------------------*/
#if KEYPAD_USES_EXTERNAL_IRQ
static TBool Keypad_NXP_PCA9555_Callback(
            void *aCallbackWorkspace, 
            TUInt32 aChannel)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aCallbackWorkspace;
    PARAM_NOT_USED(aChannel);

    // Notify the thread we got one ready by releasing the semaphore
    // that is holding the monitoring thread
    _isr_UEZSemaphoreRelease(p->iReady);

    // Don't reset the interrupt, we'll do it when we are ready in the task
    return EFalse;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_I2C_PCA9555_Monitor
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a task to run and monitor the keypad for presses.
 * Inputs:
 *      T_uezTask aMyTask           -- This task
 *      void *aParameters           -- Passed in workspace
 *---------------------------------------------------------------------------*/
static void Keypad_NXP_I2C_PCA9555_Monitor(
        T_uezTask aMyTask, 
        void *aParameters)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aParameters;
    TUInt32 key;
    TUInt32 key2;
    T_uezInputEvent event;
    T_keypadRegistered *p_reg;

    PARAM_NOT_USED(aMyTask);

    // Sit here and wait for an event
    while (1) {
        // At this point, the keypad is fully grabbed, so we'll
        // wait for a release to occur.
#if KEYPAD_USES_EXTERNAL_IRQ
        UEZSemaphoreGrab(p->iReady, UEZ_TIMEOUT_INFINITE);
#else
        UEZTaskDelay(50);
#endif
        // Process the keypad interrupt
        IKeypad_PCA9555_Scan(p, &key);
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

            // Done, return to our control
            IRelease(p);

            // Watch the scan waiting for no press every at 10 ms intervals
            while (1) {
                if (IKeypad_PCA9555_Scan(p, &key2) != UEZ_ERROR_NONE)
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

#if KEYPAD_USES_EXTERNAL_IRQ
            // Turn back on the external interrupts
            (*p->iEINT)->Clear(p->iEINT, KEYPAD_EINT2_CHANNEL);
#endif

            // Done, return to our control
            IRelease(p);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_I2C_PCA9555_Register
 *---------------------------------------------------------------------------*
 * Description:
 *      Register a queue to receive T_uezInputEvent entries from the keypad.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      T_uezQueue                  -- Queue to receive events
 * Outputs:
 *      
 *---------------------------------------------------------------------------*/
T_uezError Keypad_NXP_I2C_PCA9555_Register(
        void *aWorkspace, 
        T_uezQueue aQueue)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    error = IQueueAdd(p, aQueue);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_I2C_PCA9555_Unregister
 *---------------------------------------------------------------------------*
 * Description:
 *      Remove the given queue from the list of event queues.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      T_uezQueue                  -- Queue to remove from events
 * Outputs:
 *      
 *---------------------------------------------------------------------------*/
T_uezError Keypad_NXP_I2C_PCA9555_Unregister(
        void *aWorkspace, 
        T_uezQueue aQueue)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab(p);
    IQueueRemove(p, aQueue);
    IRelease(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_I2C_PCA9555_Scan
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
T_uezError Keypad_NXP_I2C_PCA9555_Scan(
        void *aWorkspace, 
        TUInt32 *aScanKey)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aWorkspace;
    return IKeypad_PCA9555_Scan(p, aScanKey);
}

/*---------------------------------------------------------------------------*
 * Routine:  Keypad_NXP_I2C_PCA9555_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the PCA9555's I2C setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
T_uezError Keypad_NXP_PCA9555_Configure(
        void *aWorkspace, 
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr,
        const char *aExternalInterruptsName,
        TUInt8 aExternalInterruptChannel)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p = 
        (T_Keypad_NXP_I2C_PCA9555_Workspace *)aWorkspace;
    
    T_uezError error;

    // Set the I2C bus
    p->iI2C = aI2C;
    p->iI2CAddr = aI2CAddr;

#if KEYPAD_USES_EXTERNAL_IRQ
    // Setup the external interrupt
    error = UEZDeviceTableFind(aExternalInterruptsName, (T_uezDevice *)&p->eintDevice);
    if (error)
        return error;
    UEZDeviceTableGetWorkspace(p->eintDevice, (T_uezDeviceWorkspace **)&p->iEINT);

    error = (*p->iEINT)->Set(
                p->iEINT,
                KEYPAD_EINT2_CHANNEL,
                EINT_TRIGGER_EDGE_FALLING,
                Keypad_NXP_PCA9555_Callback,
                (void *)p,
                INTERRUPT_PRIORITY_HIGH,
                "EINT2n:KEYPAD_IRQ");
    if (error)
        return error;
#endif

    error = UEZTaskCreate(
                (T_uezTaskFunction)Keypad_NXP_I2C_PCA9555_Monitor,
                "Keypad",
                128,
                aWorkspace,
                UEZ_PRIORITY_HIGH,
                &p->iMonitorTask);

    return error;
}

T_uezError Keypad_NXP_PCA9555_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr,
        const char *aExternalInterruptsName,
        TUInt8 aExternalInterruptChannel)
{
    T_Keypad_NXP_I2C_PCA9555_Workspace *p;
    T_uezDevice i2c;
    DEVICE_I2C_BUS **p_i2c;
    T_uezError error;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Keypad_NXP_PCA9555_Interface, 0,
            (T_uezDeviceWorkspace **)&p);

    error = UEZDeviceTableFind(aI2CBusName, &i2c);
    if (error)
        return error;
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);

    error = Keypad_NXP_PCA9555_Configure(p, p_i2c, aI2CAddr, aExternalInterruptsName,
        aExternalInterruptChannel);
    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Keypad Keypad_NXP_PCA9555_Interface = {
	{
	    // Common device interface
	    "Keypad:NXP:PCA9555",
	    0x0100,
	    Keypad_NXP_I2C_PCA9555_InitializeWorkspace,
	    sizeof(T_Keypad_NXP_I2C_PCA9555_Workspace),
	},
	
    // Functions
    Keypad_NXP_I2C_PCA9555_Open,
    Keypad_NXP_I2C_PCA9555_Close,
    Keypad_NXP_I2C_PCA9555_Scan,
    Keypad_NXP_I2C_PCA9555_Register,
    Keypad_NXP_I2C_PCA9555_Unregister,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  Keypad_NXP_I2C_PCA9555.c
 *-------------------------------------------------------------------------*/
