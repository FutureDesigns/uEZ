/*-------------------------------------------------------------------------*
 * File:  GPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1788 GPIO Interface.
 * Implementation:
 *      A single GPIO0 is created, but the code can be easily changed for
 *      other processors to use multiple GPIO busses.
 *-------------------------------------------------------------------------*/

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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC1788/LPC1788_GPIO.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 *iIOPIN;
    TVUInt32 *iIOSET;
    TVUInt32 *iIOCLR;
    TVUInt32 *iIODIR;
    TVUInt32 *iIOIntEnR;
    TVUInt32 *iIOIntEnF;
    TVUInt32 *iIOIntStatR;
    TVUInt32 *iIOIntStatF;
    TVUInt32 *iIOIntClr;
    TVUInt32 *iIOCONTable[32];
} LPC1788_GPIO_PortInfo;

typedef struct {
    const HAL_GPIOPort *iHAL;
    const LPC1788_GPIO_PortInfo *iPortInfo;
    T_gpioInterruptHandler iInterruptCallback;
    void *iInterruptCallbackWorkspace;
    TUInt32 iLocked;
} T_LPC1788_GPIO_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void LPC1788_GPIO_ProcessIRQ(T_LPC1788_GPIO_Workspace *p);

/*---------------------------------------------------------------------------*
 * Globals and Tables:
 *---------------------------------------------------------------------------*/
const LPC1788_GPIO_PortInfo GPIO_LPC1788_Port0_PortInfo
= {
    (TUInt32 *)&LPC_GPIO0->PIN,
    (TUInt32 *)&LPC_GPIO0->SET,
    (TUInt32 *)&LPC_GPIO0->CLR,
    (TUInt32 *)&LPC_GPIO0->DIR,
    (TUInt32 *)&LPC_GPIOINT->IO0IntEnR,
    (TUInt32 *)&LPC_GPIOINT->IO0IntEnF,
    (TUInt32 *)&LPC_GPIOINT->IO0IntStatR,
    (TUInt32 *)&LPC_GPIOINT->IO0IntStatF,
    (TUInt32 *)&LPC_GPIOINT->IO0IntClr,
    {
        (TUInt32 *)&LPC_IOCON->P0_0,
        (TUInt32 *)&LPC_IOCON->P0_1,
        (TUInt32 *)&LPC_IOCON->P0_2,
        (TUInt32 *)&LPC_IOCON->P0_3,
        (TUInt32 *)&LPC_IOCON->P0_4,
        (TUInt32 *)&LPC_IOCON->P0_5,
        (TUInt32 *)&LPC_IOCON->P0_6,
        (TUInt32 *)&LPC_IOCON->P0_7,
        (TUInt32 *)&LPC_IOCON->P0_8,
        (TUInt32 *)&LPC_IOCON->P0_9,
        (TUInt32 *)&LPC_IOCON->P0_10,
        (TUInt32 *)&LPC_IOCON->P0_11,
        (TUInt32 *)&LPC_IOCON->P0_12,
        (TUInt32 *)&LPC_IOCON->P0_13,
        (TUInt32 *)&LPC_IOCON->P0_14,
        (TUInt32 *)&LPC_IOCON->P0_15,
        (TUInt32 *)&LPC_IOCON->P0_16,
        (TUInt32 *)&LPC_IOCON->P0_17,
        (TUInt32 *)&LPC_IOCON->P0_18,
        (TUInt32 *)&LPC_IOCON->P0_19,
        (TUInt32 *)&LPC_IOCON->P0_20,
        (TUInt32 *)&LPC_IOCON->P0_21,
        (TUInt32 *)&LPC_IOCON->P0_22,
        (TUInt32 *)&LPC_IOCON->P0_23,
        (TUInt32 *)&LPC_IOCON->P0_24,
        (TUInt32 *)&LPC_IOCON->P0_25,
        (TUInt32 *)&LPC_IOCON->P0_26,
        (TUInt32 *)&LPC_IOCON->P0_27,
        (TUInt32 *)&LPC_IOCON->P0_28,
        (TUInt32 *)&LPC_IOCON->P0_29,
        (TUInt32 *)&LPC_IOCON->P0_30,
        (TUInt32 *)&LPC_IOCON->P0_31,
    }
};

const LPC1788_GPIO_PortInfo GPIO_LPC1788_Port1_PortInfo = {
        (TUInt32 *)&LPC_GPIO1->PIN,
        (TUInt32 *)&LPC_GPIO1->SET,
        (TUInt32 *)&LPC_GPIO1->CLR,
        (TUInt32 *)&LPC_GPIO1->DIR,
        0,
        0,
        0,
        0,
        0,
        {
                (TUInt32 *)&LPC_IOCON->P1_0,
                (TUInt32 *)&LPC_IOCON->P1_1,
                (TUInt32 *)&LPC_IOCON->P1_2,
                (TUInt32 *)&LPC_IOCON->P1_3,
                (TUInt32 *)&LPC_IOCON->P1_4,
                (TUInt32 *)&LPC_IOCON->P1_5,
                (TUInt32 *)&LPC_IOCON->P1_6,
                (TUInt32 *)&LPC_IOCON->P1_7,
                (TUInt32 *)&LPC_IOCON->P1_8,
                (TUInt32 *)&LPC_IOCON->P1_9,
                (TUInt32 *)&LPC_IOCON->P1_10,
                (TUInt32 *)&LPC_IOCON->P1_11,
                (TUInt32 *)&LPC_IOCON->P1_12,
                (TUInt32 *)&LPC_IOCON->P1_13,
                (TUInt32 *)&LPC_IOCON->P1_14,
                (TUInt32 *)&LPC_IOCON->P1_15,
                (TUInt32 *)&LPC_IOCON->P1_16,
                (TUInt32 *)&LPC_IOCON->P1_17,
                (TUInt32 *)&LPC_IOCON->P1_18,
                (TUInt32 *)&LPC_IOCON->P1_19,
                (TUInt32 *)&LPC_IOCON->P1_20,
                (TUInt32 *)&LPC_IOCON->P1_21,
                (TUInt32 *)&LPC_IOCON->P1_22,
                (TUInt32 *)&LPC_IOCON->P1_23,
                (TUInt32 *)&LPC_IOCON->P1_24,
                (TUInt32 *)&LPC_IOCON->P1_25,
                (TUInt32 *)&LPC_IOCON->P1_26,
                (TUInt32 *)&LPC_IOCON->P1_27,
                (TUInt32 *)&LPC_IOCON->P1_28,
                (TUInt32 *)&LPC_IOCON->P1_29,
                (TUInt32 *)&LPC_IOCON->P1_30,
                (TUInt32 *)&LPC_IOCON->P1_31, }

};

const LPC1788_GPIO_PortInfo GPIO_LPC1788_Port2_PortInfo
= {
    (TUInt32 *)&LPC_GPIO2->PIN,
    (TUInt32 *)&LPC_GPIO2->SET,
    (TUInt32 *)&LPC_GPIO2->CLR,
    (TUInt32 *)&LPC_GPIO2->DIR,
    (TUInt32 *)&LPC_GPIOINT->IO2IntEnR,
    (TUInt32 *)&LPC_GPIOINT->IO2IntEnF,
    (TUInt32 *)&LPC_GPIOINT->IO2IntStatR,
    (TUInt32 *)&LPC_GPIOINT->IO2IntStatF,
    (TUInt32 *)&LPC_GPIOINT->IO2IntClr,
    {
        (TUInt32 *)&LPC_IOCON->P2_0,
        (TUInt32 *)&LPC_IOCON->P2_1,
        (TUInt32 *)&LPC_IOCON->P2_2,
        (TUInt32 *)&LPC_IOCON->P2_3,
        (TUInt32 *)&LPC_IOCON->P2_4,
        (TUInt32 *)&LPC_IOCON->P2_5,
        (TUInt32 *)&LPC_IOCON->P2_6,
        (TUInt32 *)&LPC_IOCON->P2_7,
        (TUInt32 *)&LPC_IOCON->P2_8,
        (TUInt32 *)&LPC_IOCON->P2_9,
        (TUInt32 *)&LPC_IOCON->P2_10,
        (TUInt32 *)&LPC_IOCON->P2_11,
        (TUInt32 *)&LPC_IOCON->P2_12,
        (TUInt32 *)&LPC_IOCON->P2_13,
        (TUInt32 *)&LPC_IOCON->P2_14,
        (TUInt32 *)&LPC_IOCON->P2_15,
        (TUInt32 *)&LPC_IOCON->P2_16,
        (TUInt32 *)&LPC_IOCON->P2_17,
        (TUInt32 *)&LPC_IOCON->P2_18,
        (TUInt32 *)&LPC_IOCON->P2_19,
        (TUInt32 *)&LPC_IOCON->P2_20,
        (TUInt32 *)&LPC_IOCON->P2_21,
        (TUInt32 *)&LPC_IOCON->P2_22,
        (TUInt32 *)&LPC_IOCON->P2_23,
        (TUInt32 *)&LPC_IOCON->P2_24,
        (TUInt32 *)&LPC_IOCON->P2_25,
        (TUInt32 *)&LPC_IOCON->P2_26,
        (TUInt32 *)&LPC_IOCON->P2_27,
        (TUInt32 *)&LPC_IOCON->P2_28,
        (TUInt32 *)&LPC_IOCON->P2_29,
        (TUInt32 *)&LPC_IOCON->P2_30,
        (TUInt32 *)&LPC_IOCON->P2_31,
    }
};

const LPC1788_GPIO_PortInfo GPIO_LPC1788_Port3_PortInfo = {
        (TUInt32 *)&LPC_GPIO3->PIN,
        (TUInt32 *)&LPC_GPIO3->SET,
        (TUInt32 *)&LPC_GPIO3->CLR,
        (TUInt32 *)&LPC_GPIO3->DIR,
        0,
        0,
        0,
        0,
        0,
        {
                (TUInt32 *)&LPC_IOCON->P3_0,
                (TUInt32 *)&LPC_IOCON->P3_1,
                (TUInt32 *)&LPC_IOCON->P3_2,
                (TUInt32 *)&LPC_IOCON->P3_3,
                (TUInt32 *)&LPC_IOCON->P3_4,
                (TUInt32 *)&LPC_IOCON->P3_5,
                (TUInt32 *)&LPC_IOCON->P3_6,
                (TUInt32 *)&LPC_IOCON->P3_7,
                (TUInt32 *)&LPC_IOCON->P3_8,
                (TUInt32 *)&LPC_IOCON->P3_9,
                (TUInt32 *)&LPC_IOCON->P3_10,
                (TUInt32 *)&LPC_IOCON->P3_11,
                (TUInt32 *)&LPC_IOCON->P3_12,
                (TUInt32 *)&LPC_IOCON->P3_13,
                (TUInt32 *)&LPC_IOCON->P3_14,
                (TUInt32 *)&LPC_IOCON->P3_15,
                (TUInt32 *)&LPC_IOCON->P3_16,
                (TUInt32 *)&LPC_IOCON->P3_17,
                (TUInt32 *)&LPC_IOCON->P3_18,
                (TUInt32 *)&LPC_IOCON->P3_19,
                (TUInt32 *)&LPC_IOCON->P3_20,
                (TUInt32 *)&LPC_IOCON->P3_21,
                (TUInt32 *)&LPC_IOCON->P3_22,
                (TUInt32 *)&LPC_IOCON->P3_23,
                (TUInt32 *)&LPC_IOCON->P3_24,
                (TUInt32 *)&LPC_IOCON->P3_25,
                (TUInt32 *)&LPC_IOCON->P3_26,
                (TUInt32 *)&LPC_IOCON->P3_27,
                (TUInt32 *)&LPC_IOCON->P3_28,
                (TUInt32 *)&LPC_IOCON->P3_29,
                (TUInt32 *)&LPC_IOCON->P3_30,
                (TUInt32 *)&LPC_IOCON->P3_31, } };

const LPC1788_GPIO_PortInfo GPIO_LPC1788_Port4_PortInfo = {
        (TUInt32 *)&LPC_GPIO4->PIN,
        (TUInt32 *)&LPC_GPIO4->SET,
        (TUInt32 *)&LPC_GPIO4->CLR,
        (TUInt32 *)&LPC_GPIO4->DIR,
        0,
        0,
        0,
        0,
        0,
        {
                (TUInt32 *)&LPC_IOCON->P4_0,
                (TUInt32 *)&LPC_IOCON->P4_1,
                (TUInt32 *)&LPC_IOCON->P4_2,
                (TUInt32 *)&LPC_IOCON->P4_3,
                (TUInt32 *)&LPC_IOCON->P4_4,
                (TUInt32 *)&LPC_IOCON->P4_5,
                (TUInt32 *)&LPC_IOCON->P4_6,
                (TUInt32 *)&LPC_IOCON->P4_7,
                (TUInt32 *)&LPC_IOCON->P4_8,
                (TUInt32 *)&LPC_IOCON->P4_9,
                (TUInt32 *)&LPC_IOCON->P4_10,
                (TUInt32 *)&LPC_IOCON->P4_11,
                (TUInt32 *)&LPC_IOCON->P4_12,
                (TUInt32 *)&LPC_IOCON->P4_13,
                (TUInt32 *)&LPC_IOCON->P4_14,
                (TUInt32 *)&LPC_IOCON->P4_15,
                (TUInt32 *)&LPC_IOCON->P4_16,
                (TUInt32 *)&LPC_IOCON->P4_17,
                (TUInt32 *)&LPC_IOCON->P4_18,
                (TUInt32 *)&LPC_IOCON->P4_19,
                (TUInt32 *)&LPC_IOCON->P4_20,
                (TUInt32 *)&LPC_IOCON->P4_21,
                (TUInt32 *)&LPC_IOCON->P4_22,
                (TUInt32 *)&LPC_IOCON->P4_23,
                (TUInt32 *)&LPC_IOCON->P4_24,
                (TUInt32 *)&LPC_IOCON->P4_25,
                (TUInt32 *)&LPC_IOCON->P4_26,
                (TUInt32 *)&LPC_IOCON->P4_27,
                (TUInt32 *)&LPC_IOCON->P4_28,
                (TUInt32 *)&LPC_IOCON->P4_29,
                (TUInt32 *)&LPC_IOCON->P4_30,
                (TUInt32 *)&LPC_IOCON->P4_31, } };

const LPC1788_GPIO_PortInfo GPIO_LPC1788_Port5_PortInfo = {
        (TUInt32 *)&LPC_GPIO5->PIN,
        (TUInt32 *)&LPC_GPIO5->SET,
        (TUInt32 *)&LPC_GPIO5->CLR,
        (TUInt32 *)&LPC_GPIO5->DIR,
        0,
        0,
        0,
        0,
        0,
        {
                (TUInt32 *)&LPC_IOCON->P5_0,
                (TUInt32 *)&LPC_IOCON->P5_1,
                (TUInt32 *)&LPC_IOCON->P5_2,
                (TUInt32 *)&LPC_IOCON->P5_3,
                (TUInt32 *)&LPC_IOCON->P5_4,
                0, /* 5 */
                0, /* 6 */
                0, /* 7 */
                0, /* 8 */
                0, /* 9 */
                0, /* 10 */
                0, /* 11 */
                0, /* 12 */
                0, /* 13 */
                0, /* 14 */
                0, /* 15 */
                0, /* 16 */
                0, /* 17 */
                0, /* 18 */
                0, /* 19 */
                0, /* 20 */
                0, /* 21 */
                0, /* 22 */
                0, /* 23 */
                0, /* 24 */
                0, /* 25 */
                0, /* 26 */
                0, /* 27 */
                0, /* 28 */
                0, /* 29 */
                0, /* 30 */
                0, /* 31 */
        } };

static T_LPC1788_GPIO_Workspace *G_LPC1788Port0Workspace = 0;
static T_LPC1788_GPIO_Workspace *G_LPC1788Port2Workspace = 0;

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Port0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1788_Port0_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    G_LPC1788Port0Workspace = p;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Port1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port1 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Port1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1788_Port1_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Port2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port2 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Port2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1788_Port2_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    G_LPC1788Port2Workspace = p;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Port3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port3 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Port3_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1788_Port3_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Port4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port4 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Port4_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1788_Port4_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Port5_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 GPIO Port5 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Port5_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC1788_Port5_PortInfo;
    p->iInterruptCallback = 0;
    p->iInterruptCallbackWorkspace = 0;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    //    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;

    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Activate
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
T_uezError LPC1788_GPIO_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    // Output mode is set by putting 1's in the IODIR register
    (*p_info->iIODIR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    // Input mode is set by putting 0's in the IODIR register
    // Make those bits 0
    (*p_info->iIODIR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOSET) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOCLR) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_Read
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
T_uezError LPC1788_GPIO_Read(
        void *aWorkspace,
        TUInt32 aPortPins,
        TUInt32 *aPinsRead)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    // Get the bits
    *aPinsRead = ((*p_info->iIOPIN) & aPortPins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_SetPull(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioPull aPull)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;
    TVUInt32 *p_iocon;
    TUInt32 mode;

    // Indexes only
    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Look up the IOCON register
    p_iocon = p_info->iIOCONTable[aPortPinIndex];
    if (!p_iocon)
        return UEZ_ERROR_NOT_SUPPORTED;

    // Map to a bit field
    switch (aPull) {
        case GPIO_PULL_UP:
            mode = 2 << 3;
            break;
        case GPIO_PULL_NONE:
            mode = 0 << 3;
            break;
        case GPIO_PULL_DOWN:
            mode = 1 << 3;
            break;
        case GPIO_REPEATER:
            mode = 3 << 3;
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
#if (COMPILER_TYPE != IAR)
            break;
#endif
    }

    /* Change bits 4:3 MODE of the IOCON register */
    *p_iocon = (*p_iocon & ~(3 << 3)) | mode;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_SetMux
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
T_uezError LPC1788_GPIO_SetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux aMux)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;
    TVUInt32 *p_iocon;

    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;
    if (aMux >= 8)
        return UEZ_ERROR_OUT_OF_RANGE;

    p_iocon = p_info->iIOCONTable[aPortPinIndex];
    if (!p_iocon)
        return UEZ_ERROR_NOT_SUPPORTED;

    /* Change bits 2:0 FUNC */
    *p_iocon = (*p_iocon & ~(7 << 0)) | aMux;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_GetMux
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
T_uezError LPC1788_GPIO_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;
    TVUInt32 *p_iocon;

    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;

    p_iocon = p_info->iIOCONTable[aPortPinIndex];
    if (!p_iocon)
        return UEZ_ERROR_NOT_SUPPORTED;

    /* Get bits 2:0 FUNC */
    *aMux = (*p_iocon & (7 << 0));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC1788_GPIO_IRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO interrupts have fired off, do the right one.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
static IRQ_ROUTINE(LPC1788_GPIO_IRQ)
{
#define IOIntStatus_P0Int       (1<<0)
#define IOIntStatus_P2Int       (1<<2)

    IRQ_START();

    // It's port 0 and/or port 2, is this port 0 firing?
    if (LPC_GPIOINT->IntStatus & IOIntStatus_P0Int)
        LPC1788_GPIO_ProcessIRQ(G_LPC1788Port0Workspace);
    if (LPC_GPIOINT->IntStatus & IOIntStatus_P2Int)
        LPC1788_GPIO_ProcessIRQ(G_LPC1788Port2Workspace);

    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_SetInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the GPIO's interrupt mode (enable if not already enabled)
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      T_gpioInterruptHandler iInterruptCallback -- Routine to call on
 *                                      interrupt.
 *      void *iInterruptCallbackWorkspace -- Workspace of callback
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_ConfigureInterruptCallback(
        void *aWorkspace,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;

    // Register the interrupt handler if not already registered
    InterruptRegister(GPIO_IRQn, LPC1788_GPIO_IRQ, INTERRUPT_PRIORITY_HIGH,
            "GPIOIrq");
    InterruptEnable(GPIO_IRQn);

    p->iInterruptCallback = aInterruptCallback;
    p->iInterruptCallbackWorkspace = aInterruptCallbackWorkspace;

    return UEZ_ERROR_NONE;
}

T_uezError LPC1788_GPIO_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aInterruptCallback);PARAM_NOT_USED(aInterruptCallbackWorkspace);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_ProcessIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO ports is reporting an IRQ.  This processes one
 *      of the ports for falling or rising edge events.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
void LPC1788_GPIO_ProcessIRQ(T_LPC1788_GPIO_Workspace *p)
{
    TUInt32 regF;
    TUInt32 regR;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    // Make sure we have a handler
    if (!p->iInterruptCallback)
        InterruptFatalError();

    // Do we have any falling edge interrupts?
    regF = *(p_info->iIOIntStatF);
    regR = *(p_info->iIOIntStatR);
    if (regF)
        p->iInterruptCallback(p->iInterruptCallbackWorkspace, regF,
                GPIO_INTERRUPT_FALLING_EDGE);

    // Do we have any rising edge interrupts?
    if (regR)
        p->iInterruptCallback(p->iInterruptCallbackWorkspace, regR,
                GPIO_INTERRUPT_RISING_EDGE);

    // Now clear all the interrupts processed above
    // (regardless of enabled/disabled state)
    *(p_info->iIOIntClr) = regF | regR;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_EnableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn on the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to enable.
 *      T_gpioInterruptType aType   -- Type of interrupts to enable.
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_EnableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    if (aType == GPIO_INTERRUPT_FALLING_EDGE)
        (*p_info->iIOIntEnF) |= aPortPins;
    else if (aType == GPIO_INTERRUPT_RISING_EDGE)
        (*p_info->iIOIntEnR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_DisableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn off the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *      T_gpioInterruptType aType   -- Type of interrupts to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_DisableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    if (aType == GPIO_INTERRUPT_FALLING_EDGE)
        (*p_info->iIOIntEnF) &= ~aPortPins;
    else if (aType == GPIO_INTERRUPT_RISING_EDGE)
        (*p_info->iIOIntEnR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_GPIO_ClearInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear interrupts on a specific GPIO line
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_GPIO_ClearInterrupts(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    (*p_info->iIOIntClr) = aPortPins;

    return UEZ_ERROR_NONE;
}

T_uezError LPC1788_GPIO_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC1788_GPIO_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC1788_GPIO_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC1788_GPIO_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    TVUInt32 *p_iocon;
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    const LPC1788_GPIO_PortInfo *p_info = p->iPortInfo;

    PARAM_NOT_USED(aValue);

    // Look up the IOCON register
    p_iocon = p_info->iIOCONTable[aPortPinIndex];
    if (!p_iocon)
        return UEZ_ERROR_NOT_SUPPORTED;

    switch (aControl) {
        case GPIO_NOP:
            // Yep, do no operation
            break;
        case GPIO_CONTROL_ENABLE_INPUT_BUFFER:
            *p_iocon |= (1 << 7);
            break;
        case GPIO_CONTROL_DISABLE_INPUT_BUFFER:
            *p_iocon &= ~(1 << 7);
            break;
        case GPIO_CONTROL_SET_CONFIG_BITS:
            *p_iocon = aValue;
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
#if (COMPILER_TYPE != IAR)
            break;
#endif
    }

    return UEZ_ERROR_NONE;
}

T_uezError LPC1788_GPIO_Lock(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        UEZFailureMsg("PinLock");
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError LPC1788_GPIO_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC1788_GPIO_Workspace *p = (T_LPC1788_GPIO_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_LPC1788_Port0_Interface = {
        {
        "LPC1788 GPIO Port 0",
        0x0106,
        LPC1788_GPIO_Port0_InitializeWorkspace,
        sizeof(T_LPC1788_GPIO_Workspace),
        },

        LPC1788_GPIO_Configure,
        LPC1788_GPIO_Activate,
        LPC1788_GPIO_Deactivate,
        LPC1788_GPIO_SetOutputMode,
        LPC1788_GPIO_SetInputMode,
        LPC1788_GPIO_Set,
        LPC1788_GPIO_Clear,
        LPC1788_GPIO_Read,
        LPC1788_GPIO_SetPull,
        LPC1788_GPIO_SetMux,
        LPC1788_GPIO_ConfigureInterruptCallback,
        LPC1788_GPIO_EnableInterrupts,
        LPC1788_GPIO_DisableInterrupts,
        LPC1788_GPIO_ClearInterrupts,
        LPC1788_GPIO_Control,
        LPC1788_GPIO_Lock,
        LPC1788_GPIO_Unlock,
        LPC1788_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1788_Port1_Interface = {
        {
        "LPC1788 GPIO Port 1",
        0x0106,
        LPC1788_GPIO_Port1_InitializeWorkspace,
        sizeof(T_LPC1788_GPIO_Workspace),
        },

        LPC1788_GPIO_Configure,
        LPC1788_GPIO_Activate,
        LPC1788_GPIO_Deactivate,
        LPC1788_GPIO_SetOutputMode,
        LPC1788_GPIO_SetInputMode,
        LPC1788_GPIO_Set,
        LPC1788_GPIO_Clear,
        LPC1788_GPIO_Read,
        LPC1788_GPIO_SetPull,
        LPC1788_GPIO_SetMux,
        LPC1788_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC1788_GPIO_EnableInterrupts_NotSupported,
        LPC1788_GPIO_DisableInterrupts_NotSupported,
        LPC1788_GPIO_ClearInterrupts_NotSupported,
        LPC1788_GPIO_Control,
        LPC1788_GPIO_Lock,
        LPC1788_GPIO_Unlock,
        LPC1788_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1788_Port2_Interface = {
        {
        "LPC1788 GPIO Port 2",
        0x0106,
        LPC1788_GPIO_Port2_InitializeWorkspace,
        sizeof(T_LPC1788_GPIO_Workspace),
        },

        LPC1788_GPIO_Configure,
        LPC1788_GPIO_Activate,
        LPC1788_GPIO_Deactivate,
        LPC1788_GPIO_SetOutputMode,
        LPC1788_GPIO_SetInputMode,
        LPC1788_GPIO_Set,
        LPC1788_GPIO_Clear,
        LPC1788_GPIO_Read,
        LPC1788_GPIO_SetPull,
        LPC1788_GPIO_SetMux,
        LPC1788_GPIO_ConfigureInterruptCallback,
        LPC1788_GPIO_EnableInterrupts,
        LPC1788_GPIO_DisableInterrupts,
        LPC1788_GPIO_ClearInterrupts,
        LPC1788_GPIO_Control,
        LPC1788_GPIO_Lock,
        LPC1788_GPIO_Unlock,
        LPC1788_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1788_Port3_Interface = {
        {
        "LPC1788 GPIO Port 3",
        0x0106,
        LPC1788_GPIO_Port3_InitializeWorkspace,
        sizeof(T_LPC1788_GPIO_Workspace),
        },

        LPC1788_GPIO_Configure,
        LPC1788_GPIO_Activate,
        LPC1788_GPIO_Deactivate,
        LPC1788_GPIO_SetOutputMode,
        LPC1788_GPIO_SetInputMode,
        LPC1788_GPIO_Set,
        LPC1788_GPIO_Clear,
        LPC1788_GPIO_Read,
        LPC1788_GPIO_SetPull,
        LPC1788_GPIO_SetMux,
        LPC1788_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC1788_GPIO_EnableInterrupts_NotSupported,
        LPC1788_GPIO_DisableInterrupts_NotSupported,
        LPC1788_GPIO_ClearInterrupts_NotSupported,
        LPC1788_GPIO_Control,
        LPC1788_GPIO_Lock,
        LPC1788_GPIO_Unlock,
        LPC1788_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1788_Port4_Interface = {
        {
        "LPC1788 GPIO Port 4",
        0x0106,
        LPC1788_GPIO_Port4_InitializeWorkspace,
        sizeof(T_LPC1788_GPIO_Workspace),
        },

        LPC1788_GPIO_Configure,
        LPC1788_GPIO_Activate,
        LPC1788_GPIO_Deactivate,
        LPC1788_GPIO_SetOutputMode,
        LPC1788_GPIO_SetInputMode,
        LPC1788_GPIO_Set,
        LPC1788_GPIO_Clear,
        LPC1788_GPIO_Read,
        LPC1788_GPIO_SetPull,
        LPC1788_GPIO_SetMux,
        LPC1788_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC1788_GPIO_EnableInterrupts_NotSupported,
        LPC1788_GPIO_DisableInterrupts_NotSupported,
        LPC1788_GPIO_ClearInterrupts_NotSupported,
        LPC1788_GPIO_Control,
        LPC1788_GPIO_Lock,
        LPC1788_GPIO_Unlock,
        LPC1788_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC1788_Port5_Interface = {
        {
        "LPC1788 GPIO Port 5",
        0x0106,
        LPC1788_GPIO_Port5_InitializeWorkspace,
        sizeof(T_LPC1788_GPIO_Workspace),
        },

        LPC1788_GPIO_Configure,
        LPC1788_GPIO_Activate,
        LPC1788_GPIO_Deactivate,
        LPC1788_GPIO_SetOutputMode,
        LPC1788_GPIO_SetInputMode,
        LPC1788_GPIO_Set,
        LPC1788_GPIO_Clear,
        LPC1788_GPIO_Read,
        LPC1788_GPIO_SetPull,
        LPC1788_GPIO_SetMux,
        LPC1788_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC1788_GPIO_EnableInterrupts_NotSupported,
        LPC1788_GPIO_DisableInterrupts_NotSupported,
        LPC1788_GPIO_ClearInterrupts_NotSupported,
        LPC1788_GPIO_Control,
        LPC1788_GPIO_Lock,
        LPC1788_GPIO_Unlock,
        LPC1788_GPIO_GetMux };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1788_GPIO0_Require(void)
{
    HAL_GPIOPort **p_gpio0;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO0",
            (T_halInterface *)&GPIO_LPC1788_Port0_Interface, 0,
            (T_halWorkspace **)&p_gpio0);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_0, p_gpio0);
}

void LPC1788_GPIO1_Require(void)
{
    HAL_GPIOPort **p_gpio1;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO1 and allow all pins
    HALInterfaceRegister("GPIO1",
            (T_halInterface *)&GPIO_LPC1788_Port1_Interface, 0,
            (T_halWorkspace **)&p_gpio1);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_1, p_gpio1);
}

void LPC1788_GPIO2_Require(void)
{
    HAL_GPIOPort **p_gpio2;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO2 and allow all pins
    HALInterfaceRegister("GPIO2",
            (T_halInterface *)&GPIO_LPC1788_Port2_Interface, 0,
            (T_halWorkspace **)&p_gpio2);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_2, p_gpio2);
}

void LPC1788_GPIO3_Require(void)
{
    HAL_GPIOPort **p_gpio3;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO3 and allow all pins
    HALInterfaceRegister("GPIO3",
            (T_halInterface *)&GPIO_LPC1788_Port3_Interface, 0,
            (T_halWorkspace **)&p_gpio3);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_3, p_gpio3);
}

void LPC1788_GPIO4_Require(void)
{
    HAL_GPIOPort **p_gpio4;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO4 and allow all pins
    HALInterfaceRegister("GPIO4",
            (T_halInterface *)&GPIO_LPC1788_Port4_Interface, 0,
            (T_halWorkspace **)&p_gpio4);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_4, p_gpio4);
}

void LPC1788_GPIO5_Require(void)
{
    HAL_GPIOPort **p_gpio5;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO5 and allow all pins
    HALInterfaceRegister("GPIO5",
            (T_halInterface *)&GPIO_LPC1788_Port5_Interface, 0,
            (T_halWorkspace **)&p_gpio5);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_5, p_gpio5);
}

/*-------------------------------------------------------------------------*
 * End of File:  GPIO.c
 *-------------------------------------------------------------------------*/
