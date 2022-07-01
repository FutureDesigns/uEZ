/*-------------------------------------------------------------------------*
 * File:  GPIO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC43xx GPIO Interface.
 * Implementation:
 *      A single GPIO0 is created, but the code can be easily changed for
 *      other processors to use multiple GPIO busses.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPIO.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 *iDIR;
    TVUInt32 *iMASK;
    TVUInt32 *iPIN;
    TVUInt32 *iMPIN;
    TVUInt32 *iSET;
    TVUInt32 *iCLR;
    TVUInt32 *iNOT;
    TVUInt32 *iSCUTable[32];
} LPC43xx_GPIO_PortInfo;

typedef struct {
    const HAL_GPIOPort *iHAL;
    const LPC43xx_GPIO_PortInfo *iPortInfo;
    TUInt32 iLocked;
} T_LPC43xx_GPIO_Workspace;

typedef struct {
    TUInt32 iPortPin;
    T_gpioInterruptHandler iInterruptCallback;
    void *iInterruptCallbackWorkspace;
    T_gpioInterruptType iInterruptType;
}T_GPIO_Interrupt;

typedef struct {
    TVInt32 *iPINTSEL0; //interrupt select register for int32_t. 0 - 3
    TVInt32 *iPINTSEL1; //interrupt select register for int32_t. 4 - 7
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

    TUInt32 iConfiguredInterrupts;
    TUInt32 iInterruptNumber[8];
    TUInt8 iInterruptName[8][10];
    T_GPIO_Interrupt iInterrupt[8];
}T_LPC43xx_GPIO_Interrupt_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void LPC43xx_GPIO_ProcessIRQ(TUInt32 aInterrupt);

/*---------------------------------------------------------------------------*
 * Globals and Tables:
 *---------------------------------------------------------------------------*/

static T_LPC43xx_GPIO_Interrupt_Workspace G_Interrupt_Workspace = {
    (TVInt32 *)&LPC_SCU->PINTSEL0,
    (TVInt32 *)&LPC_SCU->PINTSEL1,
    //Driver only supports pin interrupt
    (TVInt32 *)&LPC_GPIO_PIN_INT->ISEL,
    (TVInt32 *)&LPC_GPIO_PIN_INT->IENR,
    (TVInt32 *)&LPC_GPIO_PIN_INT->SIENR,
    (TVInt32 *)&LPC_GPIO_PIN_INT->CIENR,
    (TVInt32 *)&LPC_GPIO_PIN_INT->IENF,
    (TVInt32 *)&LPC_GPIO_PIN_INT->SIENF,
    (TVInt32 *)&LPC_GPIO_PIN_INT->CIENF,
    (TVInt32 *)&LPC_GPIO_PIN_INT->RISE,
    (TVInt32 *)&LPC_GPIO_PIN_INT->FALL,
    (TVInt32 *)&LPC_GPIO_PIN_INT->IST,
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

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port0_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[0],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[0],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[0],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[0],
        (TUInt32 *)&LPC_GPIO_PORT->SET[0],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[0],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[0],
        {
                (TUInt32*)&LPC_SCU->SFSP0_0 ,
                (TUInt32*)&LPC_SCU->SFSP0_1 ,
                (TUInt32*)&LPC_SCU->SFSP1_15,
                (TUInt32*)&LPC_SCU->SFSP1_16,
                (TUInt32*)&LPC_SCU->SFSP1_0 ,
                (TUInt32*)&LPC_SCU->SFSP6_6 ,
                (TUInt32*)&LPC_SCU->SFSP3_6 ,
                (TUInt32*)&LPC_SCU->SFSP2_7 ,
                (TUInt32*)&LPC_SCU->SFSP1_1 ,
                (TUInt32*)&LPC_SCU->SFSP1_2 ,
                (TUInt32*)&LPC_SCU->SFSP1_3 ,
                (TUInt32*)&LPC_SCU->SFSP1_4 ,
                (TUInt32*)&LPC_SCU->SFSP1_17,
                (TUInt32*)&LPC_SCU->SFSP1_18,
                (TUInt32*)&LPC_SCU->SFSP2_10,
                (TUInt32*)&LPC_SCU->SFSP1_20,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
        }
};

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port1_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[1],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[1],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[1],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[1],
        (TUInt32 *)&LPC_GPIO_PORT->SET[1],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[1],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[1],
        {
                (TUInt32*)&LPC_SCU->SFSP1_7 ,
                (TUInt32*)&LPC_SCU->SFSP1_8 ,
                (TUInt32*)&LPC_SCU->SFSP1_9 ,
                (TUInt32*)&LPC_SCU->SFSP1_10,
                (TUInt32*)&LPC_SCU->SFSP1_11,
                (TUInt32*)&LPC_SCU->SFSP1_12,
                (TUInt32*)&LPC_SCU->SFSP1_13,
                (TUInt32*)&LPC_SCU->SFSP1_7 ,
                (TUInt32*)&LPC_SCU->SFSP1_5 ,
                (TUInt32*)&LPC_SCU->SFSP1_6 ,
                (TUInt32*)&LPC_SCU->SFSP2_9 ,
                (TUInt32*)&LPC_SCU->SFSP2_10,
                (TUInt32*)&LPC_SCU->SFSP2_12,
                (TUInt32*)&LPC_SCU->SFSP2_13,
                (TUInt32*)&LPC_SCU->SFSP3_4 ,
                (TUInt32*)&LPC_SCU->SFSP3_5 ,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
        }
};

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port2_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[2],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[2],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[2],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[2],
        (TUInt32 *)&LPC_GPIO_PORT->SET[2],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[2],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[2],
        {
                (TUInt32*)&LPC_SCU->SFSP4_0 ,
                (TUInt32*)&LPC_SCU->SFSP4_1 ,
                (TUInt32*)&LPC_SCU->SFSP4_2 ,
                (TUInt32*)&LPC_SCU->SFSP4_3 ,
                (TUInt32*)&LPC_SCU->SFSP4_4 ,
                (TUInt32*)&LPC_SCU->SFSP4_5 ,
                (TUInt32*)&LPC_SCU->SFSP4_6 ,
                (TUInt32*)&LPC_SCU->SFSP5_7 ,
                (TUInt32*)&LPC_SCU->SFSP6_12,
                (TUInt32*)&LPC_SCU->SFSP5_0 ,
                (TUInt32*)&LPC_SCU->SFSP5_1 ,
                (TUInt32*)&LPC_SCU->SFSP5_2 ,
                (TUInt32*)&LPC_SCU->SFSP5_3 ,
                (TUInt32*)&LPC_SCU->SFSP5_4 ,
                (TUInt32*)&LPC_SCU->SFSP5_5 ,
                (TUInt32*)&LPC_SCU->SFSP5_6 ,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
        }
};

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port3_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[3],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[3],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[3],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[3],
        (TUInt32 *)&LPC_GPIO_PORT->SET[3],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[3],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[3],
        {
                (TUInt32*)&LPC_SCU->SFSP6_1 ,
                (TUInt32*)&LPC_SCU->SFSP6_2 ,
                (TUInt32*)&LPC_SCU->SFSP6_3 ,
                (TUInt32*)&LPC_SCU->SFSP6_4 ,
                (TUInt32*)&LPC_SCU->SFSP6_5 ,
                (TUInt32*)&LPC_SCU->SFSP6_9 ,
                (TUInt32*)&LPC_SCU->SFSP6_10,
                (TUInt32*)&LPC_SCU->SFSP6_11,
                (TUInt32*)&LPC_SCU->SFSP7_0 ,
                (TUInt32*)&LPC_SCU->SFSP7_1 ,
                (TUInt32*)&LPC_SCU->SFSP7_2 ,
                (TUInt32*)&LPC_SCU->SFSP7_3 ,
                (TUInt32*)&LPC_SCU->SFSP7_4 ,
                (TUInt32*)&LPC_SCU->SFSP7_5 ,
                (TUInt32*)&LPC_SCU->SFSP7_6 ,
                (TUInt32*)&LPC_SCU->SFSP7_7 ,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
        }
};

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port4_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[4],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[4],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[4],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[4],
        (TUInt32 *)&LPC_GPIO_PORT->SET[4],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[4],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[4],
        {
                (TUInt32*)&LPC_SCU->SFSP8_0 ,
                (TUInt32*)&LPC_SCU->SFSP8_1 ,
                (TUInt32*)&LPC_SCU->SFSP8_2 ,
                (TUInt32*)&LPC_SCU->SFSP8_3 ,
                (TUInt32*)&LPC_SCU->SFSP8_4 ,
                (TUInt32*)&LPC_SCU->SFSP8_5 ,
                (TUInt32*)&LPC_SCU->SFSP8_6 ,
                (TUInt32*)&LPC_SCU->SFSP8_7 ,
                (TUInt32*)&LPC_SCU->SFSPA_1 ,
                (TUInt32*)&LPC_SCU->SFSPA_2 ,
                (TUInt32*)&LPC_SCU->SFSPA_3 ,
                (TUInt32*)&LPC_SCU->SFSP9_6 ,
                (TUInt32*)&LPC_SCU->SFSP9_0 ,
                (TUInt32*)&LPC_SCU->SFSP9_1 ,
                (TUInt32*)&LPC_SCU->SFSP9_2 ,
                (TUInt32*)&LPC_SCU->SFSP9_3 ,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
        }
};

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port5_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[5],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[5],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[5],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[5],
        (TUInt32 *)&LPC_GPIO_PORT->SET[5],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[5],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[5],
        {
                (TUInt32*)&LPC_SCU->SFSP2_0 ,
                (TUInt32*)&LPC_SCU->SFSP2_1 ,
                (TUInt32*)&LPC_SCU->SFSP2_2 ,
                (TUInt32*)&LPC_SCU->SFSP2_3 ,
                (TUInt32*)&LPC_SCU->SFSP2_4 ,
                (TUInt32*)&LPC_SCU->SFSP2_5 ,
                (TUInt32*)&LPC_SCU->SFSP2_6 ,
                (TUInt32*)&LPC_SCU->SFSP2_8 ,
                (TUInt32*)&LPC_SCU->SFSP3_1 ,
                (TUInt32*)&LPC_SCU->SFSP3_2 ,
                (TUInt32*)&LPC_SCU->SFSP3_7 ,
                (TUInt32*)&LPC_SCU->SFSP3_8 ,
                (TUInt32*)&LPC_SCU->SFSP4_8 ,
                (TUInt32*)&LPC_SCU->SFSP4_9 ,
                (TUInt32*)&LPC_SCU->SFSP4_10,
                (TUInt32*)&LPC_SCU->SFSP6_7 ,
                (TUInt32*)&LPC_SCU->SFSP6_8 ,
                (TUInt32*)&LPC_SCU->SFSP9_4 ,
                (TUInt32*)&LPC_SCU->SFSP9_5 ,
                (TUInt32*)&LPC_SCU->SFSPA_4 ,
                (TUInt32*)&LPC_SCU->SFSPB_0 ,
                (TUInt32*)&LPC_SCU->SFSPB_1 ,
                (TUInt32*)&LPC_SCU->SFSPB_2 ,
                (TUInt32*)&LPC_SCU->SFSPB_3 ,
                (TUInt32*)&LPC_SCU->SFSPB_4 ,
                (TUInt32*)&LPC_SCU->SFSPB_5 ,
                (TUInt32*)&LPC_SCU->SFSPB_6 ,
                0,
                0,
                0,
                0,
        }
};

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port6_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[6],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[6],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[6],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[6],
        (TUInt32 *)&LPC_GPIO_PORT->SET[6],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[6],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[6],
        {
                (TUInt32*)&LPC_SCU->SFSPC_1 ,
                (TUInt32*)&LPC_SCU->SFSPC_2 ,
                (TUInt32*)&LPC_SCU->SFSPC_3 ,
                (TUInt32*)&LPC_SCU->SFSPC_4 ,
                (TUInt32*)&LPC_SCU->SFSPC_5 ,
                (TUInt32*)&LPC_SCU->SFSPC_6 ,
                (TUInt32*)&LPC_SCU->SFSPC_7 ,
                (TUInt32*)&LPC_SCU->SFSPC_8 ,
                (TUInt32*)&LPC_SCU->SFSPC_9 ,
                (TUInt32*)&LPC_SCU->SFSPC_10,
                (TUInt32*)&LPC_SCU->SFSPC_11,
                (TUInt32*)&LPC_SCU->SFSPC_12,
                (TUInt32*)&LPC_SCU->SFSPC_13,
                (TUInt32*)&LPC_SCU->SFSPC_14,
                (TUInt32*)&LPC_SCU->SFSPD_0 ,
                (TUInt32*)&LPC_SCU->SFSPD_1 ,
                (TUInt32*)&LPC_SCU->SFSPD_2 ,
                (TUInt32*)&LPC_SCU->SFSPD_3 ,
                (TUInt32*)&LPC_SCU->SFSPD_4 ,
                (TUInt32*)&LPC_SCU->SFSPD_5 ,
                (TUInt32*)&LPC_SCU->SFSPD_6 ,
                (TUInt32*)&LPC_SCU->SFSPD_7 ,
                (TUInt32*)&LPC_SCU->SFSPD_8 ,
                (TUInt32*)&LPC_SCU->SFSPD_9 ,
                (TUInt32*)&LPC_SCU->SFSPD_10,
                (TUInt32*)&LPC_SCU->SFSPD_11,
                (TUInt32*)&LPC_SCU->SFSPD_12,
                (TUInt32*)&LPC_SCU->SFSPD_13,
                (TUInt32*)&LPC_SCU->SFSPD_14,
                (TUInt32*)&LPC_SCU->SFSPD_15,
                (TUInt32*)&LPC_SCU->SFSPD_16,
                0,
        }
};

const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_Port7_PortInfo
= {
        (TUInt32 *)&LPC_GPIO_PORT->DIR[7],
        (TUInt32 *)&LPC_GPIO_PORT->MASK[7],
        (TUInt32 *)&LPC_GPIO_PORT->PIN[7],
        (TUInt32 *)&LPC_GPIO_PORT->MPIN[7],
        (TUInt32 *)&LPC_GPIO_PORT->SET[7],
        (TUInt32 *)&LPC_GPIO_PORT->CLR[7],
        (TUInt32 *)&LPC_GPIO_PORT->NOT[7],
        {
                (TUInt32*)&LPC_SCU->SFSPE_0 ,
                (TUInt32*)&LPC_SCU->SFSPE_1 ,
                (TUInt32*)&LPC_SCU->SFSPE_2 ,
                (TUInt32*)&LPC_SCU->SFSPE_3 ,
                (TUInt32*)&LPC_SCU->SFSPE_4 ,
                (TUInt32*)&LPC_SCU->SFSPE_5 ,
                (TUInt32*)&LPC_SCU->SFSPE_6 ,
                (TUInt32*)&LPC_SCU->SFSPE_7 ,
                (TUInt32*)&LPC_SCU->SFSPE_8 ,
                (TUInt32*)&LPC_SCU->SFSPE_9 ,
                (TUInt32*)&LPC_SCU->SFSPE_10,
                (TUInt32*)&LPC_SCU->SFSPE_11,
                (TUInt32*)&LPC_SCU->SFSPE_12,
                (TUInt32*)&LPC_SCU->SFSPE_13,
                (TUInt32*)&LPC_SCU->SFSPE_14,
                (TUInt32*)&LPC_SCU->SFSPE_15,
                (TUInt32*)&LPC_SCU->SFSPF_1 ,
                (TUInt32*)&LPC_SCU->SFSPF_2 ,
                (TUInt32*)&LPC_SCU->SFSPF_3 ,
                (TUInt32*)&LPC_SCU->SFSPF_5 ,
                (TUInt32*)&LPC_SCU->SFSPF_6 ,
                (TUInt32*)&LPC_SCU->SFSPF_7 ,
                (TUInt32*)&LPC_SCU->SFSPF_8 ,
                (TUInt32*)&LPC_SCU->SFSPF_9 ,
                (TUInt32*)&LPC_SCU->SFSPF_10,
                (TUInt32*)&LPC_SCU->SFSPF_11,
                0,
        }
};

//Pins on the LPC4357 that do not have GPIO Functionality
const LPC43xx_GPIO_PortInfo GPIO_LPC43xx_PortZ_PortInfo
= {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        {
                (TUInt32*)&LPC_SCU->SFSP1_19,
                (TUInt32*)&LPC_SCU->SFSP3_0 ,
                (TUInt32*)&LPC_SCU->SFSP3_3 ,
                (TUInt32*)&LPC_SCU->SFSP4_7 ,
                (TUInt32*)&LPC_SCU->SFSP6_0 ,
                (TUInt32*)&LPC_SCU->SFSP8_8 ,
                (TUInt32*)&LPC_SCU->SFSPA_0 ,
                (TUInt32*)&LPC_SCU->SFSPC_0 ,
                (TUInt32*)&LPC_SCU->SFSPF_0 ,
                (TUInt32*)&LPC_SCU->SFSPF_4 ,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
        }
};

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port0 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port0_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port1 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port1_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port2 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port2_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port3 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port3_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port3_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port4_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port4 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port4_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port4_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port5_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port5 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port5_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port5_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port6_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port6 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port6_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port6_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Port7_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO Port7 workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Port7_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_Port7_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_PortZ_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx GPIO PortZ workspace.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_PortZ_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    p->iPortInfo = &GPIO_LPC43xx_PortZ_PortInfo;
    p->iLocked = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are usable.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aUsablePins         -- Pins allowed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Configure(void *aWorkspace, TUInt32 aUsablePins)
{
    //    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;

    PARAM_NOT_USED(aUsablePins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Activate
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
T_uezError LPC43xx_GPIO_Activate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Deactivate
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare which pins on the GPIO port are inactive as GPIO pins.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to activate
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Deactivate(void *aWorkspace, TUInt32 aPortPins)
{
    // Defunc function
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_SetOutputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_SetOutputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;

    if(!p_info->iPIN){
        //Pin does not have GPIO Functionality
        return UEZ_ERROR_INVALID;
    }
    // Output mode is set by putting 1's in the IODIR register
    (*p_info->iDIR) |= aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_SetInputMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be input mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be input mode
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_SetInputMode(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;

    if(!p_info->iPIN){
        //Pin does not have GPIO Functionality
        return UEZ_ERROR_INVALID;
    }

    // Input mode is set by putting 0's in the IODIR register
    // Make those bits 0
    (*p_info->iDIR) &= ~aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output HIGH.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Set(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;

    if(!p_info->iPIN){
        //Pin does not have GPIO Functionality
        return UEZ_ERROR_INVALID;
    }
    // Set the bits corresponding to those pins
    (*p_info->iSET) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Clear
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output LOW.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Pins to be output HIGH
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_Clear(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;

    if(!p_info->iPIN){
        //Pin does not have GPIO Functionality
        return UEZ_ERROR_INVALID;
    }
    // Set the bits corresponding to those pins
    (*p_info->iCLR) = aPortPins;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_Read
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
T_uezError LPC43xx_GPIO_Read(
        void *aWorkspace,
        TUInt32 aPortPins,
        TUInt32 *aPinsRead)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;

    if(!p_info->iPIN){
        //Pin does not have GPIO Functionality
        return UEZ_ERROR_INVALID;
    }
    // Get the bits
    *aPinsRead = ((*p_info->iPIN) & aPortPins);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_SetPull
 *---------------------------------------------------------------------------*
 * Description:
 *      Set pins to be output mode.
 * Inputs:
 *      void *aW                    -- Particular GPIO workspace
 *      TUInt8 aPortPinIndex        -- Index to pin 0-31
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_SetPull(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioPull aPull)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;
    TVUInt32 *p_scu;
    TUInt32 mode;

    // Indexes only
    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Look up the IOCON register
    p_scu = p_info->iSCUTable[aPortPinIndex];
    if (!p_scu)
        return UEZ_ERROR_NOT_SUPPORTED;

    // Map to a bit field
    switch (aPull) {
        case GPIO_PULL_UP:
            mode = SCU_EPD_DISABLE | SCU_EPUN_ENABLE;
            break;
        case GPIO_PULL_NONE:
            mode = SCU_EPD_DISABLE | SCU_EPUN_DISABLE;
            break;
        case GPIO_PULL_DOWN:
            mode = SCU_EPD_ENABLE | SCU_EPUN_DISABLE;
            break;
        case GPIO_REPEATER:
            mode = SCU_EPD_ENABLE | SCU_EPUN_ENABLE;
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
#if (COMPILER_TYPE != IAR)
            break;
#endif
    }

    /* Change bits 4:3 MODE of the IOCON register */
    *p_scu = (*p_scu & ~(3 << 3)) | mode;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_SetMux
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
T_uezError LPC43xx_GPIO_SetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux aMux)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;
    TVUInt32 *p_scu;

    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;
    if (aMux >= 8)
        return UEZ_ERROR_OUT_OF_RANGE;

    p_scu = p_info->iSCUTable[aPortPinIndex];
    if (!p_scu)
        return UEZ_ERROR_NOT_SUPPORTED;

    /* Change bits 2:0 FUNC */
    *p_scu = (*p_scu & ~(7 << 0)) | aMux;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_GetMux
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
T_uezError LPC43xx_GPIO_GetMux(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioMux *aMux)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;
    TVUInt32 *p_scu;

    if (aPortPinIndex >= 32)
        return UEZ_ERROR_INVALID_PARAMETER;

    p_scu = p_info->iSCUTable[aPortPinIndex];
    if (!p_scu)
        return UEZ_ERROR_NOT_SUPPORTED;

    /* Get bits 2:0 FUNC */
    *aMux = (*p_scu & (7 << 0));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC43xx_GPIO_IRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO interrupts have fired off, do the right one.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
static IRQ_ROUTINE(LPC43xx_PIN1_IRQ)
{
    IRQ_START();
    if(*G_Interrupt_Workspace.iIST & (1 << 0)){
        LPC43xx_GPIO_ProcessIRQ(0);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 1)){
        LPC43xx_GPIO_ProcessIRQ(1);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 2)){
        LPC43xx_GPIO_ProcessIRQ(2);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 3)){
        LPC43xx_GPIO_ProcessIRQ(3);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 4)){
        LPC43xx_GPIO_ProcessIRQ(4);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 5)){
        LPC43xx_GPIO_ProcessIRQ(5);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 6)){
        LPC43xx_GPIO_ProcessIRQ(6);
    }

    if(*G_Interrupt_Workspace.iIST & (1 << 7)){
        LPC43xx_GPIO_ProcessIRQ(7);
    }
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_SetInterrupt
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
T_uezError LPC43xx_GPIO_ConfigureInterruptCallback(
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
            LPC43xx_PIN1_IRQ, INTERRUPT_PRIORITY_HIGH,
            (const char*)G_Interrupt_Workspace.iInterruptName[G_Interrupt_Workspace.iConfiguredInterrupts]);

    if(G_Interrupt_Workspace.iConfiguredInterrupts < 4){
        *G_Interrupt_Workspace.iPINTSEL0 |= ((aPortPins >> 8) << (5 + (8 * G_Interrupt_Workspace.iConfiguredInterrupts))) |\
                ((aPortPins & 0xFF) << (0 + (8 * G_Interrupt_Workspace.iConfiguredInterrupts)));
    } else {
        *G_Interrupt_Workspace.iPINTSEL1 |= ((aPortPins >> 8) << (5 + (8 * (G_Interrupt_Workspace.iConfiguredInterrupts - 4)))) |\
                ((aPortPins & 0xFF) << (0 + (8 * (G_Interrupt_Workspace.iConfiguredInterrupts - 4))));
    }

    p->iInterruptCallback = aInterruptCallback;
    p->iInterruptCallbackWorkspace = aInterruptCallbackWorkspace;
    p->iPortPin = aPortPins;

    InterruptEnable(G_Interrupt_Workspace.iInterruptNumber[G_Interrupt_Workspace.iConfiguredInterrupts]);
    G_Interrupt_Workspace.iConfiguredInterrupts++;

    return UEZ_ERROR_NONE;
}

T_uezError LPC43xx_GPIO_ConfigureInterruptCallback_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptHandler aInterruptCallback,
        void *aInterruptCallbackWorkspace)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aInterruptCallback);PARAM_NOT_USED(aInterruptCallbackWorkspace);
    PARAM_NOT_USED(aPortPins);

    // No IRQs on these
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_ProcessIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      One of the GPIO ports is reporting an IRQ.  This processes one
 *      of the ports for falling or rising edge events.
 * NOTES:
 *      This routine is inside an interrupt!
 *---------------------------------------------------------------------------*/
void LPC43xx_GPIO_ProcessIRQ(TUInt32 aInterrupt)
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
 * Routine:  LPC43xx_GPIO_EnableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn on the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to enable.
 *      T_gpioInterruptType aType   -- Type of interrupts to enable.
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_EnableInterrupts(
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
 * Routine:  LPC43xx_GPIO_DisableInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn off the interrupts for the given type of one or more port pins.
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *      T_gpioInterruptType aType   -- Type of interrupts to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_DisableInterrupts(
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

    if(aType == GPIO_INTERRUPT_FALLING_EDGE){
        *G_Interrupt_Workspace.iCIENF |= (1 << i); //Clear Falling Edge
    } else if (aType == GPIO_INTERRUPT_RISING_EDGE){
        *G_Interrupt_Workspace.iCIENR |= (1 << i); //Clear Rising Edge
    }


    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_GPIO_ClearInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear interrupts on a specific GPIO line
 * Inputs:
 *      void *aWorkspace            -- Particular GPIO workspace
 *      TUInt32 aPortPins           -- Masked list of ports to disable.
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_GPIO_ClearInterrupts(void *aWorkspace, TUInt32 aPortPins)
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

T_uezError LPC43xx_GPIO_EnableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC43xx_GPIO_DisableInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);PARAM_NOT_USED(aType);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC43xx_GPIO_ClearInterrupts_NotSupported(
        void *aWorkspace,
        TUInt32 aPortPins)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aPortPins);
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError LPC43xx_GPIO_Control(
        void *aWorkspace,
        TUInt8 aPortPinIndex,
        T_gpioControl aControl,
        TUInt32 aValue)
{
    TVUInt32 *p_scu;
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
    const LPC43xx_GPIO_PortInfo *p_info = p->iPortInfo;

    PARAM_NOT_USED(aValue);

    // Look up the IOCON register

    p_scu = p_info->iSCUTable[aPortPinIndex];
    if (!p_scu)
        return UEZ_ERROR_NOT_SUPPORTED;

    switch (aControl) {
        case GPIO_NOP:
            // Yep, do no operation
            break;
        case GPIO_CONTROL_ENABLE_INPUT_BUFFER:
            *p_scu |= (1 << 6);
            break;
        case GPIO_CONTROL_DISABLE_INPUT_BUFFER:
            *p_scu &= ~(1 << 6);
            break;
        case GPIO_CONTROL_SET_CONFIG_BITS:
            *p_scu = aValue;
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
#if (COMPILER_TYPE != IAR)
            break;
#endif
    }

    return UEZ_ERROR_NONE;
}

T_uezError LPC43xx_GPIO_Lock(void *aWorkspace, TUInt32 aPortPins)
{
#ifndef NDEBUG
    char buffer [50];
#endif
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;
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

T_uezError LPC43xx_GPIO_Unlock(void *aWorkspace, TUInt32 aPortPins)
{
    T_LPC43xx_GPIO_Workspace *p = (T_LPC43xx_GPIO_Workspace *)aWorkspace;

    p->iLocked &= ~aPortPins;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_GPIOPort GPIO_LPC43xx_Port0_Interface = {
        {
        "LPC43xx GPIO Port 0",
        0x0106,
        LPC43xx_GPIO_Port0_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_Port1_Interface = {
        {
        "LPC43xx GPIO Port 1",
        0x0106,
        LPC43xx_GPIO_Port1_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_Port2_Interface = {
        {
        "LPC43xx GPIO Port 2",
        0x0106,
        LPC43xx_GPIO_Port2_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_Port3_Interface = {
        {
        "LPC43xx GPIO Port 3",
        0x0106,
        LPC43xx_GPIO_Port3_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_Port4_Interface = {
        {
        "LPC43xx GPIO Port 4",
        0x0106,
        LPC43xx_GPIO_Port4_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_Port5_Interface = {
        {
        "LPC43xx GPIO Port 5",
        0x0106,
        LPC43xx_GPIO_Port5_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_Port6_Interface = {
        {
        "LPC43xx GPIO Port 6",
        0x0106,
        LPC43xx_GPIO_Port6_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_Port7_Interface = {
        {
        "LPC43xx GPIO Port 7",
        0x0106,
        LPC43xx_GPIO_Port7_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback,
        LPC43xx_GPIO_EnableInterrupts,
        LPC43xx_GPIO_DisableInterrupts,
        LPC43xx_GPIO_ClearInterrupts,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

const HAL_GPIOPort GPIO_LPC43xx_PortZ_Interface = {
        {
        "LPC43xx GPIO Port Z",
        0x0106,
        LPC43xx_GPIO_PortZ_InitializeWorkspace,
        sizeof(T_LPC43xx_GPIO_Workspace),
        },

        LPC43xx_GPIO_Configure,
        LPC43xx_GPIO_Activate,
        LPC43xx_GPIO_Deactivate,
        LPC43xx_GPIO_SetOutputMode,
        LPC43xx_GPIO_SetInputMode,
        LPC43xx_GPIO_Set,
        LPC43xx_GPIO_Clear,
        LPC43xx_GPIO_Read,
        LPC43xx_GPIO_SetPull,
        LPC43xx_GPIO_SetMux,
        LPC43xx_GPIO_ConfigureInterruptCallback_NotSupported,
        LPC43xx_GPIO_EnableInterrupts_NotSupported,
        LPC43xx_GPIO_DisableInterrupts_NotSupported,
        LPC43xx_GPIO_ClearInterrupts_NotSupported,
        LPC43xx_GPIO_Control,
        LPC43xx_GPIO_Lock,
        LPC43xx_GPIO_Unlock,
        LPC43xx_GPIO_GetMux };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC43xx_GPIO0_Require(void)
{
    HAL_GPIOPort **p_gpio0;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO0 and allow all pins
    HALInterfaceRegister("GPIO0",
            (T_halInterface *)&GPIO_LPC43xx_Port0_Interface, 0,
            (T_halWorkspace **)&p_gpio0);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_0, p_gpio0);
}

void LPC43xx_GPIO1_Require(void)
{
    HAL_GPIOPort **p_gpio1;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO1 and allow all pins
    HALInterfaceRegister("GPIO1",
            (T_halInterface *)&GPIO_LPC43xx_Port1_Interface, 0,
            (T_halWorkspace **)&p_gpio1);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_1, p_gpio1);
}

void LPC43xx_GPIO2_Require(void)
{
    HAL_GPIOPort **p_gpio2;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO2 and allow all pins
    HALInterfaceRegister("GPIO2",
            (T_halInterface *)&GPIO_LPC43xx_Port2_Interface, 0,
            (T_halWorkspace **)&p_gpio2);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_2, p_gpio2);
}

void LPC43xx_GPIO3_Require(void)
{
    HAL_GPIOPort **p_gpio3;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO3 and allow all pins
    HALInterfaceRegister("GPIO3",
            (T_halInterface *)&GPIO_LPC43xx_Port3_Interface, 0,
            (T_halWorkspace **)&p_gpio3);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_3, p_gpio3);
}

void LPC43xx_GPIO4_Require(void)
{
    HAL_GPIOPort **p_gpio4;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO4 and allow all pins
    HALInterfaceRegister("GPIO4",
            (T_halInterface *)&GPIO_LPC43xx_Port4_Interface, 0,
            (T_halWorkspace **)&p_gpio4);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_4, p_gpio4);
}

void LPC43xx_GPIO5_Require(void)
{
    HAL_GPIOPort **p_gpio5;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO5 and allow all pins
    HALInterfaceRegister("GPIO5",
            (T_halInterface *)&GPIO_LPC43xx_Port5_Interface, 0,
            (T_halWorkspace **)&p_gpio5);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_5, p_gpio5);
}

void LPC43xx_GPIO6_Require(void)
{
    HAL_GPIOPort **p_gpio6;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO5 and allow all pins
    HALInterfaceRegister("GPIO6",
            (T_halInterface *)&GPIO_LPC43xx_Port6_Interface, 0,
            (T_halWorkspace **)&p_gpio6);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_6, p_gpio6);
}

void LPC43xx_GPIO7_Require(void)
{
    HAL_GPIOPort **p_gpio7;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO5 and allow all pins
    HALInterfaceRegister("GPIO7",
            (T_halInterface *)&GPIO_LPC43xx_Port7_Interface, 0,
            (T_halWorkspace **)&p_gpio7);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_7, p_gpio7);
}

//Does not have GPIO functional, added to simplify pin configuration
void LPC43xx_GPIOZ_Require(void)
{
    HAL_GPIOPort **p_gpioZ;
    HAL_DEVICE_REQUIRE_ONCE();
    // Register GPIO5 and allow all pins
    HALInterfaceRegister("GPIOZ",
            (T_halInterface *)&GPIO_LPC43xx_PortZ_Interface, 0,
            (T_halWorkspace **)&p_gpioZ);
    UEZGPIORegisterPort(UEZ_GPIO_PORT_Z, p_gpioZ);
}

/*-------------------------------------------------------------------------*
 * End of File:  GPIO.c
 *-------------------------------------------------------------------------*/
