/*-------------------------------------------------------------------------*
 * File:  I2S.c
 *-------------------------------------------------------------------------*
 * Description:
 *
 * Implementation:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include "Config.h"
#include <uEZTypes.h>
#include <HAL/HAL.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include <HAL/I2SBus.h>
#include <uEZRTOS.h>
#include <HAL/I2SBus.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_I2S.h>
#include <uEZPlatform.h>
#include "LPC1768_UtilityFuncs.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_I2S *iHAL;
    HAL_I2S_Callback_Transmit_Low iTransmitLowFunc;
    void *iCallBackWorkspace;
    T_I2S_Settings iSettings;
} T_lpc1768_i2s_Workspace;
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
T_lpc1768_i2s_Workspace *G_I2SWorkspace = 0;

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_lpc1768_i2s_Workspace *G_lpc1768_i2s_Workspace;
extern const HAL_I2S G_LPC1768_I2S;
TBool intRegistered = 0;
/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC1768_I2SInterruptHandler)
;

/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
void LPC1768_I2S_GetDat(T_lpc1768_i2s_Workspace *p)
{
    TUInt8 i;
    TInt32 samples[8];
    TInt32 *sample = samples;

    p->iTransmitLowFunc(p->iCallBackWorkspace, sample, 4);//p->iSettings->iLowLevel);

    for (i = 0; i < 4; i++) {
        LPC_I2S->I2STXFIFO = sample[i];
    }
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the I2S interrupt and forward to LPC1768_I2S_OutputSample.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC1768_I2SInterruptHandler)
{
    IRQ_START();
    LPC1768_I2S_GetDat(G_lpc1768_i2s_Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_I2S_InitializeWorkspace(void *aWorkspace)
{
    T_lpc1768_i2s_Workspace *p = (T_lpc1768_i2s_Workspace*)aWorkspace;
    G_lpc1768_i2s_Workspace = p;

    //   p->iSettings.iLowLevel = 4;
    //   p->iSettings.iSampleSize = I2S_SETTING_SAMPLE_SIZE_16BIT;
    //   p->iSettings.iFormat = I2S_SETTING_SAMPLE_FORMAT_STEREO;
    //   p->iSettings.iSampleRate = 441000;

    return UEZ_ERROR_NONE;
}
/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_I2S_Stop(void *aWorkspace)
{
    //Turn off Interrupts
    InterruptDisable(I2S_IRQn);
    LPC_I2S->I2SIRQ &= ~2;
    return UEZ_ERROR_NONE;
}
/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_I2S_Start(void *aWorkspace)//transmit low functions ?
{
    //Turn on transmit Interrupt
    LPC_I2S->I2STXFIFO = 0;
    LPC_I2S->I2STXFIFO = 0;
    LPC_I2S->I2STXFIFO = 0;
    LPC_I2S->I2STXFIFO = 0;
    LPC_I2S->I2STXFIFO = 0;
    LPC_I2S->I2STXFIFO = 0;
    LPC_I2S->I2STXFIFO = 0;
    LPC_I2S->I2STXFIFO = 0;
    InterruptEnable(I2S_IRQn);
    LPC_I2S->I2SIRQ |= 2;
    return UEZ_ERROR_NONE;
}
/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_I2S_config_TX(
        void *aWorkspace,
        HAL_I2S_Callback_Transmit_Low aTransmitLowCallback)
{
    //Initialize the workspace
    T_lpc1768_i2s_Workspace *p = (T_lpc1768_i2s_Workspace *)aWorkspace;

    //Disable Interrupts
    //InterruptDisable(I2S_IRQn);

    //Power up the PCONP for the I2S
    LPC1768PowerOn(1 << 27);

    //Initialize function pointer
    p->iTransmitLowFunc = aTransmitLowCallback;

    //Setup the SDAO register
    //Disable data output (stop and reset, then release reset)
    LPC_I2S->I2SDAO |= 0x18;
    //I2SDAO &= 0x10;
    //Configure I2S output for
    LPC_I2S->I2SDAO = 0x18 | (I2S_SETTING_SAMPLE_SIZE_16BIT << 0)
            | (I2S_SETTING_SAMPLE_FORMAT_MONO << 2) | (0 << 5) | //only supporting mater mode at this time
            (15 << 6);

    //Cacluate the RATE and configure the register
    LPC_I2S->I2STXRATE = (1 << 8) | 2;

    LPC_I2S->I2STXBITRATE = (((PROCESSOR_OSCILLATOR_FREQUENCY) / 2) / (44100 * 16
            * 1));
    //Set the frequency of the I2S to be 36 MHz (divide 72 MHz PCLK by 2)
    //divided by a 44 kHz signal of 16 bits by 2

    LPC_I2S->I2SIRQ = 0;
    // Set the TXFIFO depth to be 4 (sets irq flag)
    LPC_I2S->I2SIRQ &= ~(0xFF << 16);
    LPC_I2S->I2SIRQ |= (4 << 16);

    //Enable DAO (go and release reset)
    LPC_I2S->I2SDAO &= ~0x18;

    if (!intRegistered) {
        InterruptRegister(I2S_IRQn, ILPC1768_I2SInterruptHandler,
                INTERRUPT_PRIORITY_NORMAL, "I2S");
        InterruptEnable(I2S_IRQn);
        intRegistered = 1;
    }

    G_lpc1768_i2s_Workspace = p;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

const HAL_I2S G_LPC1768_I2S_Interface = {
        {
        "NXP:LPC1768:I2S",
        0x0100,
        LPC1768_I2S_InitializeWorkspace,
        sizeof(T_lpc1768_i2s_Workspace),
        },

        LPC1768_I2S_config_TX,
        LPC1768_I2S_Start,
        LPC1768_I2S_Stop, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1768_I2S_Require(const T_LPC1768_I2S_Settings *aSettings)
{
    static const T_LPC1768_IOCON_ConfigList pinsRX_SCK[] = {
            {GPIO_P0_4,  IOCON_D_DEFAULT(1)},
            {GPIO_P0_23, IOCON_A_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList pinsRX_SDA[] = {
            {GPIO_P0_6,  IOCON_D_DEFAULT(1)},
            {GPIO_P0_25, IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList pinsRX_WS[] = {
            {GPIO_P0_5,  IOCON_D_DEFAULT(1)},
            {GPIO_P0_24, IOCON_A_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList pinsRX_MCLK[] = {
            {GPIO_P4_28, IOCON_D_DEFAULT(2)},
    };

    static const T_LPC1768_IOCON_ConfigList pinsTX_SCK[] = {
            {GPIO_P2_11, IOCON_D_DEFAULT(3)},
            {GPIO_P0_7,  IOCON_W_DEFAULT(1)},
    };
    static const T_LPC1768_IOCON_ConfigList pinsTX_SDA[] = {
            {GPIO_P2_13, IOCON_D_DEFAULT(3)},
            {GPIO_P0_9,  IOCON_W_DEFAULT(1)},
    };
    static const T_LPC1768_IOCON_ConfigList pinsTX_WS[] = {
            {GPIO_P2_12, IOCON_D_DEFAULT(3)},
            {GPIO_P0_8,  IOCON_W_DEFAULT(1)},
    };
    static const T_LPC1768_IOCON_ConfigList pinsTX_MCLK[] = {
            {GPIO_P4_29, IOCON_D_DEFAULT(2)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register I2S Bus drivers
    HALInterfaceRegister("I2S", (T_halInterface *)&G_LPC1768_I2S_Interface, 0,
            0);
    LPC1768_IOCON_ConfigPinOrNone(aSettings->iRX_SCK, pinsRX_SCK,
            ARRAY_COUNT(pinsRX_SCK));
    LPC1768_IOCON_ConfigPinOrNone(aSettings->iRX_SDA, pinsRX_SDA,
            ARRAY_COUNT(pinsRX_SDA));
    LPC1768_IOCON_ConfigPinOrNone(aSettings->iRX_WS, pinsRX_WS,
            ARRAY_COUNT(pinsRX_WS));
    LPC1768_IOCON_ConfigPinOrNone(aSettings->iRX_MCLK, pinsRX_MCLK,
            ARRAY_COUNT(pinsRX_MCLK));

    LPC1768_IOCON_ConfigPinOrNone(aSettings->iTX_SCK, pinsTX_SCK,
            ARRAY_COUNT(pinsTX_SCK));
    LPC1768_IOCON_ConfigPinOrNone(aSettings->iTX_SDA, pinsTX_SDA,
            ARRAY_COUNT(pinsTX_SDA));
    LPC1768_IOCON_ConfigPinOrNone(aSettings->iTX_WS, pinsTX_WS,
            ARRAY_COUNT(pinsTX_WS));
    LPC1768_IOCON_ConfigPinOrNone(aSettings->iTX_MCLK, pinsTX_MCLK,
            ARRAY_COUNT(pinsTX_MCLK));
}





/*
#define I2S                   ((      I2S_TypeDef *)       I2S_BASE)
------------- Inter IC Sound (I2S) -----------------------------------------*
typedef struct
{
  __IO uint32_t I2SDAO;
  __I  uint32_t I2SDAI;
  __O  uint32_t I2STXFIFO;
  __I  uint32_t I2SRXFIFO;
  __I  uint32_t I2SSTATE;
  __IO uint32_t I2SDMA1;
  __IO uint32_t I2SDMA2;
  __IO uint32_t I2SIRQ;
  __IO uint32_t I2STXRATE;
  __IO uint32_t I2SRXRATE;
  __IO uint32_t I2STXBITRATE;
  __IO uint32_t I2SRXBITRATE;
  __IO uint32_t I2STXMODE;
  __IO uint32_t I2SRXMODE;
} I2S_TypeDef;
*/

/*-------------------------------------------------------------------------*
 * End of File:  I2S.c
 *-------------------------------------------------------------------------*/
