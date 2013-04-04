/*-------------------------------------------------------------------------*
 * File:  I2S.c
 *-------------------------------------------------------------------------*
 * Description:
 *
 * Implementation:
 *
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
#include "Config.h"
#include <uEZTypes.h>
#include <HAL/HAL.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include <HAL/I2SBus.h>
#include <uEZRTOS.h>
#include <HAL/I2SBus.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_I2S.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct{
    const HAL_I2S *iHAL;
    HAL_I2S_Callback_Transmit_Low iTransmitLowFunc;
    void *iCallBackWorkspace;
    T_I2S_Settings iSettings;
}T_lpc247x_i2s_Workspace;
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
T_lpc247x_i2s_Workspace *G_I2SWorkspace = 0;

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_lpc247x_i2s_Workspace *G_lpc247x_i2s_Workspace;
extern const HAL_I2S G_LPC247x_I2S;
TBool intRegistered = 0;
/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC247x_I2C0InterruptHandler);

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
void LPC247x_I2S_GetDat(T_lpc247x_i2s_Workspace *p)
{
    TUInt8 i;
    TInt32 samples[8];
    TInt32 *sample = samples;

    p->iTransmitLowFunc(p->iCallBackWorkspace,
                        sample,
                         4);//p->iSettings->iLowLevel);
      
    for( i = 0; i < 4; i++)
    {
        I2STXFIFO = sample[i];  
    }
}


/*---------------------------------------------------------------------------*
 * Routine:  
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the I2S interrupt and forward to LPC247x_I2S_OutputSample.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC247x_I2SInterruptHandler)
{
    IRQ_START();
    LPC247x_I2S_GetDat(G_lpc247x_i2s_Workspace);
    IRQ_END();
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
T_uezError LPC247x_I2S_InitializeWorkspace(void *aWorkspace)
{
   T_lpc247x_i2s_Workspace *p = (T_lpc247x_i2s_Workspace*)aWorkspace;
   G_lpc247x_i2s_Workspace = p;

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
T_uezError LPC247x_I2S_Stop(void *aWorkspace)
{
//Turn off Interrupts
    InterruptDisable(INTERRUPT_CHANNEL_I2S);
    I2SIRQ &= ~2;
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
T_uezError LPC247x_I2S_Start(void *aWorkspace)//transmit low functions ?
{
//Turn on transmit Interrupt
    I2STXFIFO = 0;
    I2STXFIFO = 0;
    I2STXFIFO = 0;
    I2STXFIFO = 0;
    I2STXFIFO = 0;
    I2STXFIFO = 0;
    I2STXFIFO = 0;
    I2STXFIFO = 0;
    InterruptEnable(INTERRUPT_CHANNEL_I2S);
    I2SIRQ |= 2;
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
T_uezError LPC247x_I2S_config_TX(void *aWorkspace, HAL_I2S_Callback_Transmit_Low aTransmitLowCallback)
{ 
//Initialize the workspace
    T_lpc247x_i2s_Workspace *p = (T_lpc247x_i2s_Workspace *)aWorkspace;

//Disable Interrupts
    InterruptDisable(INTERRUPT_CHANNEL_I2S);

//Power up the PCONP for the I2S
    PCONP |= (1 << 27); 

//    PCLKSEL1 |= (1<<22); // PCLK_I2S = CCLK/1

//Initialize function pointer
    p->iTransmitLowFunc = aTransmitLowCallback;

//Setup the SDAO register
//Disable data output (stop and reset, then release reset)
    I2SDAO |= 0x18;
    //I2SDAO &= 0x10;
//Configure I2S output for
    I2SDAO = 0x18 |
             (I2S_SETTING_SAMPLE_SIZE_16BIT << 0) |
             (I2S_SETTING_SAMPLE_FORMAT_MONO << 2) |
             (0 << 5) | //only supporting mater mode at this time
             (15 << 6);
             
//Setup the STATE register

//Cacluate the RATE and configure the register
    I2STXRATE = (((1*8000000) /  (44100 * 16 * 1)))+1;
//Set the frequency of the I2S to be 36 MHz (divide 72 MHz PCLK by 2)
//divided by a 44 kHz signal of 16 bits by 2

    I2SIRQ = 0;
// Set the TXFIFO depth to be 4 (sets irq flag)
    I2SIRQ &= ~(0xFF << 16);
    I2SIRQ |= (4 << 16);

//Enable DAO (go and release reset)
    I2SDAO &= ~0x18;

    if(!intRegistered)
    {
        InterruptRegister(INTERRUPT_CHANNEL_I2S,
                          ILPC247x_I2SInterruptHandler,
                          INTERRUPT_PRIORITY_NORMAL,
                          "I2S");
        intRegistered = 1;
    }

    G_lpc247x_i2s_Workspace = p;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_I2S_Require(const T_LPC2478_I2S_Settings *aSettings)
{
    static const T_LPC2478_PINSEL_ConfigList pinsRX_SCK[] = {
            {GPIO_P0_4,  1},
            {GPIO_P0_23, 2},
    };
    static const T_LPC2478_PINSEL_ConfigList pinsRX_SDA[] = {
            {GPIO_P0_6,  1},
            {GPIO_P0_25, 2},
    };
    static const T_LPC2478_PINSEL_ConfigList pinsRX_WS[] = {
            {GPIO_P0_5,  1},
            {GPIO_P0_24, 2},
    };

    static const T_LPC2478_PINSEL_ConfigList pinsTX_SCK[] = {
            {GPIO_P2_11, 3},
            {GPIO_P0_7,  1},
    };
    static const T_LPC2478_PINSEL_ConfigList pinsTX_SDA[] = {
            {GPIO_P2_13, 3},
            {GPIO_P0_9,  1},
    };
    static const T_LPC2478_PINSEL_ConfigList pinsTX_WS[] = {
            {GPIO_P2_12, 3},
            {GPIO_P0_8,  1},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register I2S Bus drivers
    HALInterfaceRegister("I2S", (T_halInterface *)&G_LPC2478_I2S_Interface, 0,
            0);
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iRX_SCK, pinsRX_SCK,
            ARRAY_COUNT(pinsRX_SCK));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iRX_SDA, pinsRX_SDA,
            ARRAY_COUNT(pinsRX_SDA));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iRX_WS, pinsRX_WS,
            ARRAY_COUNT(pinsRX_WS));

    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iTX_SCK, pinsTX_SCK,
            ARRAY_COUNT(pinsTX_SCK));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iTX_SDA, pinsTX_SDA,
            ARRAY_COUNT(pinsTX_SDA));
    LPC2478_PINSEL_ConfigPinOrNone(aSettings->iTX_WS, pinsTX_WS,
            ARRAY_COUNT(pinsTX_WS));
}

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

const HAL_I2S G_LPC2478_I2S_Interface = {
    "Audio:LCP247x  I2S",
    0x0100,
    LPC247x_I2S_InitializeWorkspace,
    sizeof(T_lpc247x_i2s_Workspace),

    LPC247x_I2S_config_TX,
    LPC247x_I2S_Start,
    LPC247x_I2S_Stop,
};

