/*-------------------------------------------------------------------------*
 * File:  GPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx GPIO Interface.
 * Implementation:
 *      A single GPIO0 is created, but the code can be easily changed for
 *      other processors to use multiple GPIO busses.
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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_GPIO.h>
#include <stdio.h>

//TODO: Remove testing
#include "iar/Include/CMSIS/LPC54608.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 *iIOPIN;
    TVUInt32 *iIOSET;
    TVUInt32 *iIOCLR;
    TVUInt32 *iIODIR;

    TVUInt32 *iIOCONTable[32];
} LPC546xx_GPIO_PortInfo;

typedef struct {
    const HAL_GPIOPort *iHAL;
    const LPC546xx_GPIO_PortInfo *iPortInfo;
    TUInt32 iLocked;
} T_LPC546xx_GPIO_Workspace;

typedef struct {
    TUInt32 iPortPin;
    T_gpioInterruptHandler iInterruptCallback;
    void *iInterruptCallbackWorkspace;
    T_gpioInterruptType iInterruptType;
}T_GPIO_Interrupt;

typedef struct {
    TVInt32 *iPINTSEL; //interrupt select register for int.
    TVInt32 *iISEL;     /*!< Pin Interrupt Mode register */
    TVInt32 *iIENR;     /*!< Pin Interrupt Enable (Rising) register */
    TVInt32 *iSIENR;    /*!< Set Pin Interrupt Enable (Rising) register */
    TVInt32 *iCIENR;    /*!< Clear Pin Interrupt Enable (Rising) register */
    TVInt32 *iIENF;     /*!< Pin Interrupt Enable Falling Edge / Active Level register */
    TVInt32 *iSIENF;    /*!< Set Pin Interrupt Enable Falling Edge / Active Level register */
    TVInt32 *iCIENF;    /*!< Clear Pin Interrupt Enable Falling Edge / Active Level address */
    TVInt32 *iRISE;     /*!< Pin Interrupt Rising Edge register */
    TVInt32 *iFALL;     /*!< Pin Interrupt Falling Edge register */
    TVInt32 *iIST;      /*!< Pin Interrupt Status register */
    TVInt32 *iPMCTRL;      /**< Pattern match interrupt control register, offset: 0x28 */
    TVInt32 *iPMSRC;      /**< Pattern match interrupt bit-slice source register, offset: 0x2C */
    TVInt32 *iPMCFG;      /**< Pattern match interrupt bit slice configuration register, offset: 0x30 */

    TUInt32 iConfiguredInterrupts;
    TUInt32 iInterruptNumber[8];
    TUInt8 iInterruptName[8][10];
    T_GPIO_Interrupt iInterrupt[8];
}T_LPC546xx_GPIO_Interrupt_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void LPC546xx_GPIO_ProcessIRQ(TUInt32 aInterrupt);

/*---------------------------------------------------------------------------*
 * Globals and Tables:
 *---------------------------------------------------------------------------*/
static T_LPC546xx_GPIO_Interrupt_Workspace G_Interrupt_Workspace = {
    (TVInt32 *)&INPUTMUX->PINTSEL,
    (TVInt32 *)&PINT->ISEL,
    (TVInt32 *)&PINT->IENR,
    (TVInt32 *)&PINT->SIENR,
    (TVInt32 *)&PINT->CIENR,
    (TVInt32 *)&PINT->IENF,
    (TVInt32 *)&PINT->SIENF,
    (TVInt32 *)&PINT->CIENF,
    (TVInt32 *)&PINT->RISE,
    (TVInt32 *)&PINT->FALL,
    (TVInt32 *)&PINT->IST,
    0,
    0,
    0,
    0,
    {
        PIN_INT0_IRQn,  /*!<  32  PIN_INT0 */
        PIN_INT1_IRQn,  /*!<  33  PIN_INT1 */
        PIN_INT2_IRQn,  /*!<  34  PIN_INT2 */
        PIN_INT3_IRQn,  /*!<  35  PIN_INT3 */
        PIN_INT4_IRQn,  /*!<  36  PIN_INT4 */
        PIN_INT5_IRQn,  /*!<  37  PIN_INT5 */
        PIN_INT6_IRQn,  /*!<  38  PIN_INT6 */
        PIN_INT7_IRQn,  /*!<  39  PIN_INT7 */
    },
    {
        "PIN_INT0",
        "PIN_INT1",
        "PIN_INT2",
        "PIN_INT3",
        "PIN_INT4",
        "PIN_INT5",
        "PIN_INT6",
        "PIN_INT7",
    },
};

const LPC546xx_GPIO_PortInfo GPIO_LPC546xx_Port0_PortInfo
= {
    (TUInt32 *)&GPIO->PIN[0],
    (TUInt32 *)&GPIO->SET[0],
    (TUInt32 *)&GPIO->CLR[0],
    (TUInt32 *)&GPIO->DIR[0],
    {
        (TUInt32 *)&IOCON->PIO[0][0],
        (TUInt32 *)&IOCON->PIO[0][1],
        (TUInt32 *)&IOCON->PIO[0][2],
        (TUInt32 *)&IOCON->PIO[0][3],
        (TUInt32 *)&IOCON->PIO[0][4],
        (TUInt32 *)&IOCON->PIO[0][5],
        (TUInt32 *)&IOCON->PIO[0][6],
        (TUInt32 *)&IOCON->PIO[0][7],
        (TUInt32 *)&IOCON->PIO[0][8],
        (TUInt32 *)&IOCON->PIO[0][9],
        (TUInt32 *)&IOCON->PIO[0][10],
        (TUInt32 *)&IOCON->PIO[0][11],
        (TUInt32 *)&IOCON->PIO[0][12],
        (TUInt32 *)&IOCON->PIO[0][13],
        (TUInt32 *)&IOCON->PIO[0][14],
        (TUInt32 *)&IOCON->PIO[0][15],
        (TUInt32 *)&IOCON->PIO[0][16],
        (TUInt32 *)&IOCON->PIO[0][17],
        (TUInt32 *)&IOCON->PIO[0][18],
        (TUInt32 *)&IOCON->PIO[0][19],
        (TUInt32 *)&IOCON->PIO[0][20],
        (TUInt32 *)&IOCON->PIO[0][21],
        (TUInt32 *)&IOCON->PIO[0][22],
        (TUInt32 *)&IOCON->PIO[0][23],
        (TUInt32 *)&IOCON->PIO[0][24],
        (TUInt32 *)&IOCON->PIO[0][25],
        (TUInt32 *)&IOCON->PIO[0][26],
        (TUInt32 *)&IOCON->PIO[0][27],
        (TUInt32 *)&IOCON->PIO[0][28],
        (TUInt32 *)&IOCON->PIO[0][29],
        (TUInt32 *)&IOCON->PIO[0][30],
        (TUInt32 *)&IOCON->PIO[0][31],
    }
};

const LPC546xx_GPIO_PortInfo GPIO_LPC546xx_Port1_PortInfo = {
        (TUInt32 *)&GPIO->PIN[1],
        (TUInt32 *)&GPIO->SET[1],
        (TUInt32 *)&GPIO->CLR[1],
        (TUInt32 *)&GPIO->DIR[1],
        {
                (TUInt32 *)&IOCON->PIO[1][0],
                (TUInt32 *)&IOCON->PIO[1][1],
                (TUInt32 *)&IOCON->PIO[1][2],
                (TUInt32 *)&IOCON->PIO[1][3],
                (TUInt32 *)&IOCON->PIO[1][4],
                (TUInt32 *)&IOCON->PIO[1][5],
                (TUInt32 *)&IOCON->PIO[1][6],
                (TUInt32 *)&IOCON->PIO[1][7],
                (TUInt32 *)&IOCON->PIO[1][8],
                (TUInt32 *)&IOCON->PIO[1][9],
                (TUInt32 *)&IOCON->PIO[1][10],
                (TUInt32 *)&IOCON->PIO[1][11],
                (TUInt32 *)&IOCON->PIO[1][12],
                (TUInt32 *)&IOCON->PIO[1][13],
                (TUInt32 *)&IOCON->PIO[1][14],
                (TUInt32 *)&IOCON->PIO[1][15],
                (TUInt32 *)&IOCON->PIO[1][16],
                (TUInt32 *)&IOCON->PIO[1][17],
                (TUInt32 *)&IOCON->PIO[1][18],
                (TUInt32 *)&IOCON->PIO[1][19],
                (TUInt32 *)&IOCON->PIO[1][20],
                (TUInt32 *)&IOCON->PIO[1][21],
                (TUInt32 *)&IOCON->PIO[1][22],
                (TUInt32 *)&IOCON->PIO[1][23],
                (TUInt32 *)&IOCON->PIO[1][24],
                (TUInt32 *)&IOCON->PIO[1][25],
                (TUInt32 *)&IOCON->PIO[1][26],
                (TUInt32 *)&IOCON->PIO[1][27],
                (TUInt32 *)&IOCON->PIO[1][28],
                (TUInt32 *)&IOCON->PIO[1][29],
                (TUInt32 *)&IOCON->PIO[1][30],
                (TUInt32 *)&IOCON->PIO[1][31], }

};

const LPC546xx_GPIO_PortInfo GPIO_LPC546xx_Port2_PortInfo
= {
    (TUInt32 *)&GPIO->PIN[2],
    (TUInt32 *)&GPIO->SET[2],
    (TUInt32 *)&GPIO->CLR[2],
    (TUInt32 *)&GPIO->DIR[2],
    {
        (TUInt32 *)&IOCON->PIO[2][0],
        (TUInt32 *)&IOCON->PIO[2][1],
        (TUInt32 *)&IOCON->PIO[2][2],
        (TUInt32 *)&IOCON->PIO[2][3],
        (TUInt32 *)&IOCON->PIO[2][4],
        (TUInt32 *)&IOCON->PIO[2][5],
        (TUInt32 *)&IOCON->PIO[2][6],
        (TUInt32 *)&IOCON->PIO[2][7],
        (TUInt32 *)&IOCON->PIO[2][8],
        (TUInt32 *)&IOCON->PIO[2][9],
        (TUInt32 *)&IOCON->PIO[2][10],
        (TUInt32 *)&IOCON->PIO[2][11],
        (TUInt32 *)&IOCON->PIO[2][12],
        (TUInt32 *)&IOCON->PIO[2][13],
        (TUInt32 *)&IOCON->PIO[2][14],
        (TUInt32 *)&IOCON->PIO[2][15],
        (TUInt32 *)&IOCON->PIO[2][16],
        (TUInt32 *)&IOCON->PIO[2][17],
        (TUInt32 *)&IOCON->PIO[2][18],
        (TUInt32 *)&IOCON->PIO[2][19],
        (TUInt32 *)&IOCON->PIO[2][20],
        (TUInt32 *)&IOCON->PIO[2][21],
        (TUInt32 *)&IOCON->PIO[2][22],
        (TUInt32 *)&IOCON->PIO[2][23],
        (TUInt32 *)&IOCON->PIO[2][24],
        (TUInt32 *)&IOCON->PIO[2][25],
        (TUInt32 *)&IOCON->PIO[2][26],
        (TUInt32 *)&IOCON->PIO[2][27],
        (TUInt32 *)&IOCON->PIO[2][28],
        (TUInt32 *)&IOCON->PIO[2][29],
        (TUInt32 *)&IOCON->PIO[2][30],
        (TUInt32 *)&IOCON->PIO[2][31],
    }
};

const LPC546xx_GPIO_PortInfo GPIO_LPC546xx_Port3_PortInfo = {
        (TUInt32 *)&GPIO->PIN[3],
        (TUInt32 *)&GPIO->SET[3],
        (TUInt32 *)&GPIO->CLR[3],
        (TUInt32 *)&GPIO->DIR[3],
        {
                (TUInt32 *)&IOCON->PIO[3][0],
                (TUInt32 *)&IOCON->PIO[3][1],
                (TUInt32 *)&IOCON->PIO[3][2],
                (TUInt32 *)&IOCON->PIO[3][3],
                (TUInt32 *)&IOCON->PIO[3][4],
                (TUInt32 *)&IOCON->PIO[3][5],
                (TUInt32 *)&IOCON->PIO[3][6],
                (TUInt32 *)&IOCON->PIO[3][7],
                (TUInt32 *)&IOCON->PIO[3][8],
                (TUInt32 *)&IOCON->PIO[3][9],
                (TUInt32 *)&IOCON->PIO[3][10],
                (TUInt32 *)&IOCON->PIO[3][11],
                (TUInt32 *)&IOCON->PIO[3][12],
                (TUInt32 *)&IOCON->PIO[3][13],
                (TUInt32 *)&IOCON->PIO[3][14],
                (TUInt32 *)&IOCON->PIO[3][15],
                (TUInt32 *)&IOCON->PIO[3][16],
                (TUInt32 *)&IOCON->PIO[3][17],
                (TUInt32 *)&IOCON->PIO[3][18],
                (TUInt32 *)&IOCON->PIO[3][19],
                (TUInt32 *)&IOCON->PIO[3][20],
                (TUInt32 *)&IOCON->PIO[3][21],
                (TUInt32 *)&IOCON->PIO[3][22],
                (TUInt32 *)&IOCON->PIO[3][23],
                (TUInt32 *)&IOCON->PIO[3][24],
                (TUInt32 *)&IOCON->PIO[3][25],
                (TUInt32 *)&IOCON->PIO[3][26],
                (TUInt32 *)&IOCON->PIO[3][27],
                (TUInt32 *)&IOCON->PIO[3][28],
                (TUInt32 *)&IOCON->PIO[3][29],
                (TUInt32 *)&IOCON->PIO[3][30],
                (TUInt32 *)&IOCON->PIO[3][31], } };

const LPC546xx_GPIO_PortInfo GPIO_LPC546xx_Port4_PortInfo = {
        (TUInt32 *)&GPIO->PIN[4],
        (TUInt32 *)&GPIO->SET[4],
        (TUInt32 *)&GPIO->CLR[4],
        (TUInt32 *)&GPIO->DIR[4],
        {
                (TUInt32 *)&IOCON->PIO[4][0],
                (TUInt32 *)&IOCON->PIO[4][1],
                (TUInt32 *)&IOCON->PIO[4][2],
                (TUInt32 *)&IOCON->PIO[4][3],
                (TUInt32 *)&IOCON->PIO[4][4],
                (TUInt32 *)&IOCON->PIO[4][5],
                (TUInt32 *)&IOCON->PIO[4][6],
                (TUInt32 *)&IOCON->PIO[4][7],
                (TUInt32 *)&IOCON->PIO[4][8],
                (TUInt32 *)&IOCON->PIO[4][9],
                (TUInt32 *)&IOCON->PIO[4][10],
                (TUInt32 *)&IOCON->PIO[4][11],
                (TUInt32 *)&IOCON->PIO[4][12],
                (TUInt32 *)&IOCON->PIO[4][13],
                (TUInt32 *)&IOCON->PIO[4][14],
                (TUInt32 *)&IOCON->PIO[4][15],
                (TUInt32 *)&IOCON->PIO[4][16],
                (TUInt32 *)&IOCON->PIO[4][17],
                (TUInt32 *)&IOCON->PIO[4][18],
                (TUInt32 *)&IOCON->PIO[4][19],
                (TUInt32 *)&IOCON->PIO[4][20],
                (TUInt32 *)&IOCON->PIO[4][21],
                (TUInt32 *)&IOCON->PIO[4][22],
                (TUInt32 *)&IOCON->PIO[4][23],
                (TUInt32 *)&IOCON->PIO[4][24],
                (TUInt32 *)&IOCON->PIO[4][25],
                (TUInt32 *)&IOCON->PIO[4][26],
                (TUInt32 *)&IOCON->PIO[4][27],
                (TUInt32 *)&IOCON->PIO[4][28],
                (TUInt32 *)&IOCON->PIO[4][29],
                (TUInt32 *)&IOCON->PIO[4][30],
                (TUInt32 *)&IOCON->PIO[4][31], } };

const LPC546xx_GPIO_PortInfo GPIO_LPC546xx_Port5_PortInfo = {
        (TUInt32 *)&GPIO->PIN[5],
        (TUInt32 *)&GPIO->SET[5],
        (TUInt32 *)&GPIO->CLR[5],
        (TUInt32 *)&GPIO->DIR[5],
        {
                (TUInt32 *)&IOCON->PIO[5][0],
                (TUInt32 *)&IOCON->PIO[5][1],
                (TUInt32 *)&IOCON->PIO[5][2],
                (TUInt32 *)&IOCON->PIO[5][3],
                (TUInt32 *)&IOCON->PIO[5][4],
                (TUInt32 *)&IOCON->PIO[5][5],
                (TUInt32 *)&IOCON->PIO[5][6],
                (TUInt32 *)&IOCON->PIO[5][7],
                (TUInt32 *)&IOCON->PIO[5][8],
                (TUInt32 *)&IOCON->PIO[5][9],
                (TUInt32 *)&IOCON->PIO[5][10],
                0, /* 11 */
                0, /* 12 */
                0, /* 13 */
                0, /* 14 */
                0, /* 15 */
                (TUInt32 *)&IOCON->PIO[5][16],
                (TUInt32 *)&IOCON->PIO[5][17],
                (TUInt32 *)&IOCON->PIO[5][18],
                (TUInt32 *)&IOCON->PIO[5][19],
                (TUInt32 *)&IOCON->PIO[5][20],
                (TUInt32 *)&IOCON->PIO[5][21],
                (TUInt32 *)&IOCON->PIO[5][22],
                (TUInt32 *)&IOCON->PIO[5][23],
                (TUInt32 *)&IOCON->PIO[5][24],
                (TUInt32 *)&IOCON->PIO[5][25],
                (TUInt32 *)&IOCON->PIO[5][26],
                0, /* 27 */
                0, /* 28 */
                0, /* 29 */
                0, /* 30 */
                0, /* 31 */
        } };

//static T_LPC546xx_GPIO_Workspace *G_LPC546xxPort0Workspace = 0;
//static T_LPC546xx_GPIO_Workspace *G_LPC546xxPort2Workspace = 0;

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Port0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC546xx_Port0_PortInfo;
    //G_LPC546xxPort0Workspace = p;
    p->iLocked = 0;

    LPC546xxPowerOn(kCLOCK_InputMux);
    LPC546xxPowerOn(kCLOCK_Gpio0);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Port1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx GPIO Port1 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Port1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC546xx_Port1_PortInfo;
    p->iLocked = 0;

    LPC546xxPowerOn(kCLOCK_InputMux);
    LPC546xxPowerOn(kCLOCK_Gpio1);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Port2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx GPIO Port2 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Port2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC546xx_Port2_PortInfo;
    //G_LPC546xxPort2Workspace = p;
    p->iLocked = 0;

    LPC546xxPowerOn(kCLOCK_Gpio2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Port3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx GPIO Port3 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Port3_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC546xx_Port3_PortInfo;
    p->iLocked = 0;

    LPC546xxPowerOn(kCLOCK_Gpio3);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Port4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx GPIO Port4 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Port4_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC546xx_Port4_PortInfo;
    p->iLocked = 0;

    LPC546xxPowerOn(kCLOCK_Gpio4);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Port5_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx GPIO Port5 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Port5_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC546xx_Port5_PortInfo;
    p->iLocked = 0;

    LPC546xxPowerOn(kCLOCK_Gpio5);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    //    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;

    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Activate
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
T_uezError LPC546xx_GPIO_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;

    // Output mode is set by putting 1's in the IODIR register
    (*p_info->iIODIR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;

    // Input mode is set by putting 0's in the IODIR register
    // Make those bits 0
    (*p_info->iIODIR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOSET) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;

    // Set the bits corresponding to those pins
    (*p_info->iIOCLR) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_Read
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
T_uezError LPC546xx_GPIO_Read(
        void *aWorkspace,
        TUInt32 aPortPins,
        TUInt32 *aPinsRead)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;

    // Get the bits
    *aPinsRead = ((*p_info->iIOPIN) & aPortPins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_SetPull(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioPull aPull)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;
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
 * Routine:  LPC546xx_GPIO_SetMux
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
T_uezError LPC546xx_GPIO_SetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux aMux)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;
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
 * Routine:  LPC546xx_GPIO_GetMux
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
T_uezError LPC546xx_GPIO_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;
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
 * Interrupt Routine:  LPC546xx_GPIO_IRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO interrupts have fired off, do the right one.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
static IRQ_ROUTINE(LPC546xx_PIN1_IRQ)
{
    IRQ_START();
    if(*G_Interrupt_Workspace.iIST & (1 << 0)){
        LPC546xx_GPIO_ProcessIRQ(0);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 1)){
        LPC546xx_GPIO_ProcessIRQ(1);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 2)){
        LPC546xx_GPIO_ProcessIRQ(2);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 3)){
        LPC546xx_GPIO_ProcessIRQ(3);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 4)){
        LPC546xx_GPIO_ProcessIRQ(4);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 5)){
        LPC546xx_GPIO_ProcessIRQ(5);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 6)){
        LPC546xx_GPIO_ProcessIRQ(6);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 7)){
        LPC546xx_GPIO_ProcessIRQ(7);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_SetInterrupt
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
T_uezError LPC546xx_GPIO_ConfigureInterruptCallback(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    T_GPIO_Interrupt *p = (T_GPIO_Interrupt *)&G_Interrupt_Workspace.iInterrupt[G_Interrupt_Workspace.iConfiguredInterrupts];

    if(G_Interrupt_Workspace.iConfiguredInterrupts == 8){
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    // Register the interrupt handler if not already registered
    InterruptRegister(G_Interrupt_Workspace.iInterruptNumber[G_Interrupt_Workspace.iConfiguredInterrupts],
            LPC546xx_PIN1_IRQ, INTERRUPT_PRIORITY_HIGH,
            (const char*)G_Interrupt_Workspace.iInterruptName[G_Interrupt_Workspace.iConfiguredInterrupts]);

    G_Interrupt_Workspace.iPINTSEL[G_Interrupt_Workspace.iConfiguredInterrupts] |= ((aPortPins >> 8) * 32) + (aPortPins & 0xF);

    p->iInterruptCallback = aInterruptCallback;
    p->iInterruptCallbackWorkspace = aInterruptCallbackWorkspace;
    p->iPortPin = aPortPins;

    InterruptEnable(G_Interrupt_Workspace.iInterruptNumber[G_Interrupt_Workspace.iConfiguredInterrupts]);
    G_Interrupt_Workspace.iConfiguredInterrupts++;

    return UEZ_ERROR_NONE;
}

T_uezError LPC546xx_GPIO_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aInterruptCallback);PARAM_NOT_USED(aInterruptCallbackWorkspace);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_ProcessIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO ports is reporting an IRQ.  This processes one
 *      of the ports for falling or rising edge events.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
void LPC546xx_GPIO_ProcessIRQ(TUInt32 aInterrupt)
{
    T_GPIO_Interrupt *p = (T_GPIO_Interrupt *)&G_Interrupt_Workspace.iInterrupt[aInterrupt];

    // Make sure we have a handler
    if(!p->iInterruptCallback){
        InterruptFatalError();
    }

    p->iInterruptCallback(p->iInterruptCallbackWorkspace,
            p->iPortPin, p->iInterruptType);

    *G_Interrupt_Workspace.iIST |= (1 << aInterrupt);
}


/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_EnableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn on the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to enable.
 *      T_gpioInterruptType aType   -- Type of interrupts to enable.
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_EnableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    TUInt32 i;

    //Find the interrupt in the workspace
    for(i = 0; i < 8; i++){
        if(aPortPins == G_Interrupt_Workspace.iInterrupt[i].iPortPin){
            break;
        }
    }

    if(i == 8){
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    //Currently only supporting single edge triggering
    *G_Interrupt_Workspace.iISEL &= ~(1 << i);
    if(aType == GPIO_INTERRUPT_FALLING_EDGE){
        *G_Interrupt_Workspace.iSIENF |= (1 << i); //Set Falling Edge
        *G_Interrupt_Workspace.iCIENR |= (1 << i); //Clear Rising Edge
    } else if (aType == GPIO_INTERRUPT_RISING_EDGE){
        *G_Interrupt_Workspace.iSIENR |= (1 << i); //Set Rising Edge
        *G_Interrupt_Workspace.iCIENF |= (1 << i); //Clear Falling Edge
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_DisableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn off the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *      T_gpioInterruptType aType   -- Type of interrupts to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_DisableInterrupts(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    TUInt8 i;

    //Find the interrupt in the workspace
    for(i = 0; i < 8; i++){
        if(aPortPins == G_Interrupt_Workspace.iInterrupt[i].iPortPin){
            break;
        }
    }

    if(i == 8){
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    if(aType == GPIO_INTERRUPT_FALLING_EDGE){
        *G_Interrupt_Workspace.iCIENF |= (1 << i); //Clear Falling Edge
    } else if (aType == GPIO_INTERRUPT_RISING_EDGE){
        *G_Interrupt_Workspace.iCIENR |= (1 << i); //Clear Rising Edge
    }


    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_GPIO_ClearInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear interrupts on a specific GPIO line
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_GPIO_ClearInterrupts(void *aWorkspace, TUInt32 aPortPins)
{
    TUInt32 i;

    //Find the interrupt in the workspace
    for(i = 0; i < 8; i++){
        if(aPortPins == G_Interrupt_Workspace.iInterrupt[i].iPortPin){
            break;
        }
    }

    if(i == 8){
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    *G_Interrupt_Workspace.iIST |= (1 << i);

    return UEZ_ERROR_NONE;
}

T_uezError LPC546xx_GPIO_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC546xx_GPIO_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC546xx_GPIO_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC546xx_GPIO_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    TVUInt32 *p_iocon;
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    const LPC546xx_GPIO_PortInfo *p_info = p->iPortInfo;

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

T_uezError LPC546xx_GPIO_Lock(void *aWorkspace, TUInt32 aPortPins)
{
#ifndef NDEBUG
    char buffer [50];
#endif
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;
    // Any pins already locked?
    if (p->iLocked & aPortPins) {
        #ifndef NDEBUG
          sprintf (buffer, "PinLock on %s Pin %d", p->iHAL->iInterface.iName, aPortPins);
          UEZFailureMsg(buffer);
        #else
          UEZFailureMsg("PinLock");
        #endif
        return UEZ_ERROR_NOT_AVAILABLE;
    }
    p->iLocked |= aPortPins;
    return UEZ_ERROR_NONE;
}

T_uezError LPC546xx_GPIO_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC546xx_GPIO_Workspace *p = (T_LPC546xx_GPIO_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_LPC546xx_Port0_Interface = {
        {
        "LPC546xx GPIO Port 0",
        0x0106,
        LPC546xx_GPIO_Port0_InitializeWorkspace,
        sizeof(T_LPC546xx_GPIO_Workspace),
        },

        LPC546xx_GPIO_Configure,
        LPC546xx_GPIO_Activate,
        LPC546xx_GPIO_Deactivate,
        LPC546xx_GPIO_SetOutputMode,
        LPC546xx_GPIO_SetInputMode,
        LPC546xx_GPIO_Set,
        LPC546xx_GPIO_Clear,
        LPC546xx_GPIO_Read,
        LPC546xx_GPIO_SetPull,
        LPC546xx_GPIO_SetMux,
        LPC546xx_GPIO_ConfigureInterruptCallback,
        LPC546xx_GPIO_EnableInterrupts,
        LPC546xx_GPIO_DisableInterrupts,
        LPC546xx_GPIO_ClearInterrupts,
        LPC546xx_GPIO_Control,
        LPC546xx_GPIO_Lock,
        LPC546xx_GPIO_Unlock,
        LPC546xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC546xx_Port1_Interface = {
        {
        "LPC546xx GPIO Port 1",
        0x0106,
        LPC546xx_GPIO_Port1_InitializeWorkspace,
        sizeof(T_LPC546xx_GPIO_Workspace),
        },

        LPC546xx_GPIO_Configure,
        LPC546xx_GPIO_Activate,
        LPC546xx_GPIO_Deactivate,
        LPC546xx_GPIO_SetOutputMode,
        LPC546xx_GPIO_SetInputMode,
        LPC546xx_GPIO_Set,
        LPC546xx_GPIO_Clear,
        LPC546xx_GPIO_Read,
        LPC546xx_GPIO_SetPull,
        LPC546xx_GPIO_SetMux,
        LPC546xx_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC546xx_GPIO_EnableInterrupts_NotSupported,
        LPC546xx_GPIO_DisableInterrupts_NotSupported,
        LPC546xx_GPIO_ClearInterrupts_NotSupported,
        LPC546xx_GPIO_Control,
        LPC546xx_GPIO_Lock,
        LPC546xx_GPIO_Unlock,
        LPC546xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC546xx_Port2_Interface = {
        {
        "LPC546xx GPIO Port 2",
        0x0106,
        LPC546xx_GPIO_Port2_InitializeWorkspace,
        sizeof(T_LPC546xx_GPIO_Workspace),
        },

        LPC546xx_GPIO_Configure,
        LPC546xx_GPIO_Activate,
        LPC546xx_GPIO_Deactivate,
        LPC546xx_GPIO_SetOutputMode,
        LPC546xx_GPIO_SetInputMode,
        LPC546xx_GPIO_Set,
        LPC546xx_GPIO_Clear,
        LPC546xx_GPIO_Read,
        LPC546xx_GPIO_SetPull,
        LPC546xx_GPIO_SetMux,
        LPC546xx_GPIO_ConfigureInterruptCallback,
        LPC546xx_GPIO_EnableInterrupts,
        LPC546xx_GPIO_DisableInterrupts,
        LPC546xx_GPIO_ClearInterrupts,
        LPC546xx_GPIO_Control,
        LPC546xx_GPIO_Lock,
        LPC546xx_GPIO_Unlock,
        LPC546xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC546xx_Port3_Interface = {
        {
        "LPC546xx GPIO Port 3",
        0x0106,
        LPC546xx_GPIO_Port3_InitializeWorkspace,
        sizeof(T_LPC546xx_GPIO_Workspace),
        },

        LPC546xx_GPIO_Configure,
        LPC546xx_GPIO_Activate,
        LPC546xx_GPIO_Deactivate,
        LPC546xx_GPIO_SetOutputMode,
        LPC546xx_GPIO_SetInputMode,
        LPC546xx_GPIO_Set,
        LPC546xx_GPIO_Clear,
        LPC546xx_GPIO_Read,
        LPC546xx_GPIO_SetPull,
        LPC546xx_GPIO_SetMux,
        LPC546xx_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC546xx_GPIO_EnableInterrupts_NotSupported,
        LPC546xx_GPIO_DisableInterrupts_NotSupported,
        LPC546xx_GPIO_ClearInterrupts_NotSupported,
        LPC546xx_GPIO_Control,
        LPC546xx_GPIO_Lock,
        LPC546xx_GPIO_Unlock,
        LPC546xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC546xx_Port4_Interface = {
        {
        "LPC546xx GPIO Port 4",
        0x0106,
        LPC546xx_GPIO_Port4_InitializeWorkspace,
        sizeof(T_LPC546xx_GPIO_Workspace),
        },

        LPC546xx_GPIO_Configure,
        LPC546xx_GPIO_Activate,
        LPC546xx_GPIO_Deactivate,
        LPC546xx_GPIO_SetOutputMode,
        LPC546xx_GPIO_SetInputMode,
        LPC546xx_GPIO_Set,
        LPC546xx_GPIO_Clear,
        LPC546xx_GPIO_Read,
        LPC546xx_GPIO_SetPull,
        LPC546xx_GPIO_SetMux,
        LPC546xx_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC546xx_GPIO_EnableInterrupts_NotSupported,
        LPC546xx_GPIO_DisableInterrupts_NotSupported,
        LPC546xx_GPIO_ClearInterrupts_NotSupported,
        LPC546xx_GPIO_Control,
        LPC546xx_GPIO_Lock,
        LPC546xx_GPIO_Unlock,
        LPC546xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC546xx_Port5_Interface = {
        {
        "LPC546xx GPIO Port 5",
        0x0106,
        LPC546xx_GPIO_Port5_InitializeWorkspace,
        sizeof(T_LPC546xx_GPIO_Workspace),
        },

        LPC546xx_GPIO_Configure,
        LPC546xx_GPIO_Activate,
        LPC546xx_GPIO_Deactivate,
        LPC546xx_GPIO_SetOutputMode,
        LPC546xx_GPIO_SetInputMode,
        LPC546xx_GPIO_Set,
        LPC546xx_GPIO_Clear,
        LPC546xx_GPIO_Read,
        LPC546xx_GPIO_SetPull,
        LPC546xx_GPIO_SetMux,
        LPC546xx_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC546xx_GPIO_EnableInterrupts_NotSupported,
        LPC546xx_GPIO_DisableInterrupts_NotSupported,
        LPC546xx_GPIO_ClearInterrupts_NotSupported,
        LPC546xx_GPIO_Control,
        LPC546xx_GPIO_Lock,
        LPC546xx_GPIO_Unlock,
        LPC546xx_GPIO_GetMux };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_GPIO0_Require(void)
{
    HAL_GPIOPort **p_gpio0;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO0",
            (T_halInterface *)&GPIO_LPC546xx_Port0_Interface, 0,
            (T_halWorkspace **)&p_gpio0);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_0, p_gpio0);
}

void LPC546xx_GPIO1_Require(void)
{
    HAL_GPIOPort **p_gpio1;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO1 and allow all pins
    HALInterfaceRegister("GPIO1",
            (T_halInterface *)&GPIO_LPC546xx_Port1_Interface, 0,
            (T_halWorkspace **)&p_gpio1);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_1, p_gpio1);
}

void LPC546xx_GPIO2_Require(void)
{
    HAL_GPIOPort **p_gpio2;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO2 and allow all pins
    HALInterfaceRegister("GPIO2",
            (T_halInterface *)&GPIO_LPC546xx_Port2_Interface, 0,
            (T_halWorkspace **)&p_gpio2);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_2, p_gpio2);
}

void LPC546xx_GPIO3_Require(void)
{
    HAL_GPIOPort **p_gpio3;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO3 and allow all pins
    HALInterfaceRegister("GPIO3",
            (T_halInterface *)&GPIO_LPC546xx_Port3_Interface, 0,
            (T_halWorkspace **)&p_gpio3);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_3, p_gpio3);
}

void LPC546xx_GPIO4_Require(void)
{
    HAL_GPIOPort **p_gpio4;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO4 and allow all pins
    HALInterfaceRegister("GPIO4",
            (T_halInterface *)&GPIO_LPC546xx_Port4_Interface, 0,
            (T_halWorkspace **)&p_gpio4);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_4, p_gpio4);
}

void LPC546xx_GPIO5_Require(void)
{
    HAL_GPIOPort **p_gpio5;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO5 and allow all pins
    HALInterfaceRegister("GPIO5",
            (T_halInterface *)&GPIO_LPC546xx_Port5_Interface, 0,
            (T_halWorkspace **)&p_gpio5);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_5, p_gpio5);
}

/*-------------------------------------------------------------------------*
 * End of File:  GPIO.c
 *-------------------------------------------------------------------------*/
