/*-------------------------------------------------------------------------*
 * File:  LPC43xx_PLL.c
 *-------------------------------------------------------------------------*
 * Description:
 *
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include "LPC43xx_PLL.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
/** Branch clocks from CGU_BASE_SAFE */
#define CGU_ENTITY_NONE             CGU_ENTITY_NUM

/** Check bit at specific position is clear or not */
#define ISBITCLR(x,bit)             ((x&(1<<bit))^(1<<bit))
/** Check bit at specific position is set or not */
#define ISBITSET(x,bit)             (x&(1<<bit))
/** Set mask */
#define ISMASKSET(x,mask)           (x&mask)

/** CGU number of clock source */
#define CGU_CLKSRC_NUM (CGU_CLKSRC_IDIVE+1)

/*********************************************************************//**
 * Macro defines for CGU control mask bit definitions
 **********************************************************************/
/** CGU control enable mask bit */
#define CGU_CTRL_EN_MASK            1
/** CGU control clock-source mask bit */
#define CGU_CTRL_SRC_MASK           (0xF<<24)
/** CGU control auto block mask bit */
#define CGU_CTRL_AUTOBLOCK_MASK     (1<<11)

/*********************************************************************//**
 * Macro defines for CGU PLL1 mask bit definitions
 **********************************************************************/
/** CGU PLL1 feedback select mask bit */
#define CGU_PLL1_FBSEL_MASK         (1<<6)
/** CGU PLL1 Input clock bypass control mask bit */
#define CGU_PLL1_BYPASS_MASK        (1<<1)
/** CGU PLL1 direct CCO output mask bit */
#define CGU_PLL1_DIRECT_MASK        (1<<7)

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
TUInt32 G_OscillatorFrequency IN_INTERNAL_RAM;
TUInt32 G_ProcessorFrequency IN_INTERNAL_RAM;
TUInt32 G_PeripheralFrequency IN_INTERNAL_RAM;
TUInt32 G_EMCFrequency IN_INTERNAL_RAM;
TUInt32 G_USBFrequency IN_INTERNAL_RAM;

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        __I  uint32_t  RESERVED0[5];
        __IO uint32_t  FREQ_MON;                  /*!< (@ 0x40050014) Frequency monitor register */
        __IO uint32_t  XTAL_OSC_CTRL;             /*!< (@ 0x40050018) Crystal oscillator control register */
        __I  uint32_t  PLL0USB_STAT;              /*!< (@ 0x4005001C) PLL0 (USB) status register */
        __IO uint32_t  PLL0USB_CTRL;              /*!< (@ 0x40050020) PLL0 (USB) control register */
        __IO uint32_t  PLL0USB_MDIV;              /*!< (@ 0x40050024) PLL0 (USB) M-divider register */
        __IO uint32_t  PLL0USB_NP_DIV;            /*!< (@ 0x40050028) PLL0 (USB) N/P-divider register */
        __I  uint32_t  PLL0AUDIO_STAT;            /*!< (@ 0x4005002C) PLL0 (audio) status register */
        __IO uint32_t  PLL0AUDIO_CTRL;            /*!< (@ 0x40050030) PLL0 (audio) control register */
        __IO uint32_t  PLL0AUDIO_MDIV;            /*!< (@ 0x40050034) PLL0 (audio) M-divider register */
        __IO uint32_t  PLL0AUDIO_NP_DIV;          /*!< (@ 0x40050038) PLL0 (audio) N/P-divider register */
        __IO uint32_t  PLL0AUDIO_FRAC;            /*!< (@ 0x4005003C) PLL0 (audio)           */
        __I  uint32_t  PLL1_STAT;                 /*!< (@ 0x40050040) PLL1 status register   */
        __IO uint32_t  PLL1_CTRL;                 /*!< (@ 0x40050044) PLL1 control register  */
        __IO uint32_t  IDIVA_CTRL;                /*!< (@ 0x40050048) Integer divider A control register */
        __IO uint32_t  IDIVB_CTRL;                /*!< (@ 0x4005004C) Integer divider B control register */
        __IO uint32_t  IDIVC_CTRL;                /*!< (@ 0x40050050) Integer divider C control register */
        __IO uint32_t  IDIVD_CTRL;                /*!< (@ 0x40050054) Integer divider D control register */
        __IO uint32_t  IDIVE_CTRL;                /*!< (@ 0x40050058) Integer divider E control register */
        __IO uint32_t  BASE_SAFE_CLK;             /*!< (@ 0x4005005C) Output stage 0 control register for base clock BASE_SAFE_CLK */
        __IO uint32_t  BASE_USB0_CLK;             /*!< (@ 0x40050060) Output stage 1 control register for base clock BASE_USB0_CLK */
        __IO uint32_t  BASE_PERIPH_CLK;           /*!< (@ 0x40050064) Output stage 2 control register for base clock BASE_PERIPH_CLK */
        __IO uint32_t  BASE_USB1_CLK;             /*!< (@ 0x40050068) Output stage 3 control register for base clock BASE_USB1_CLK */
        __IO uint32_t  BASE_M4_CLK;               /*!< (@ 0x4005006C) Output stage BASE_M4_CLK control register */
        __IO uint32_t  BASE_SPIFI_CLK;            /*!< (@ 0x40050070) Output stage BASE_SPIFI_CLK control register */
        __IO uint32_t  BASE_SPI_CLK;              /*!< (@ 0x40050074) Output stage BASE_SPI_CLK control register */
        __IO uint32_t  BASE_PHY_RX_CLK;           /*!< (@ 0x40050078) Output stage BASE_PHY_RX_CLK control register */
        __IO uint32_t  BASE_PHY_TX_CLK;           /*!< (@ 0x4005007C) Output stage BASE_PHY_TX_CLK control register */
        __IO uint32_t  BASE_APB1_CLK;             /*!< (@ 0x40050080) Output stage BASE_APB1_CLK control register */
        __IO uint32_t  BASE_APB3_CLK;             /*!< (@ 0x40050084) Output stage BASE_APB3_CLK control register */
        __IO uint32_t  BASE_LCD_CLK;              /*!< (@ 0x40050088) Output stage BASE_LCD_CLK control register */
        __I  uint32_t  RESERVED2;
        __IO uint32_t  BASE_SDIO_CLK;             /*!< (@ 0x40050090) Output stage BASE_SDIO_CLK control register */
        __IO uint32_t  BASE_SSP0_CLK;             /*!< (@ 0x40050094) Output stage BASE_SSP0_CLK control register */
        __IO uint32_t  BASE_SSP1_CLK;             /*!< (@ 0x40050098) Output stage BASE_SSP1_CLK control register */
        __IO uint32_t  BASE_UART0_CLK;            /*!< (@ 0x4005009C) Output stage BASE_UART0_CLK control register */
        __IO uint32_t  BASE_UART1_CLK;            /*!< (@ 0x400500A0) Output stage BASE_UART1_CLK control register */
        __IO uint32_t  BASE_UART2_CLK;            /*!< (@ 0x400500A4) Output stage BASE_UART2_CLK control register */
        __IO uint32_t  BASE_UART3_CLK;            /*!< (@ 0x400500A8) Output stage BASE_UART3_CLK control register */
        __IO uint32_t  BASE_OUT_CLK;              /*!< (@ 0x400500AC) Output stage 20 control register for base clock BASE_OUT_CLK */
        __I  uint32_t  RESERVED3[4];
        __IO uint32_t  BASE_APLL_CLK;             /*!< (@ 0x400500C0) Output stage 25 control register for base clock BASE_APLL_CLK */
        __IO uint32_t  BASE_CGU_OUT0_CLK;         /*!< (@ 0x400500C4) Output stage 25 control register for base clock BASE_CGU_OUT0_CLK */
        __IO uint32_t  BASE_CGU_OUT1_CLK;         /*!< (@ 0x400500C8) Output stage 25 control register for base clock BASE_CGU_OUT1_CLK */
} T_pllRegs;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static void IPLLSetup1(T_pllRegs *pll, TUInt32 aGoalFreq, TUInt32 aRefFreq)
{
    TUInt32 m = (aGoalFreq / aRefFreq) - 1;
    TUInt32 p = 0;
    TUInt32 pval = 1;

    pll->PLL1_CTRL &= ~(CGU_PLL1_FBSEL_MASK |
                        CGU_PLL1_BYPASS_MASK |
                        CGU_PLL1_DIRECT_MASK |
                        (0x03<<8) | (0xFF<<16) | (0x03<<12));

    if(aGoalFreq < 156000000){
        while((2 * (pval) * aGoalFreq) < 156000000){
            p++;
            pval*=2;
        }

        pll->PLL1_CTRL |= (m<<16) | (0<<12) | (p<<8) | CGU_PLL1_FBSEL_MASK;
    } else if(aGoalFreq < 320000000){
        pll->PLL1_CTRL |= (m<<16) | (0<<12) | (p<<8) | CGU_PLL1_DIRECT_MASK | CGU_PLL1_FBSEL_MASK;
    } else {
        while(1); //Invalid frequency
    }

    //Wait for PLL1 to lock
    while(!(pll->PLL1_STAT & (1<<0)));
}

void LPC43xx_PLL_SetFrequencies(const T_LPC43xx_PLL_Frequencies *aFreq)
{
    //TUInt32 pll0_clk;
    //TUInt32 pll1_clk;
    //TUInt32 f_clk;
    //TUInt32 usbsel_clk;
    //TUInt32 tempReg;

    TUInt32 us;

    LPC_CGU->BASE_M4_CLK = (0x1<<24) | (1<<11);

    //Set for high or low frequency
    if(aFreq->iOscillatorClockHz < 15000000){
        LPC_CGU->XTAL_OSC_CTRL &= ~(1<<2);
    } else if(aFreq->iOscillatorClockHz <= 25000000){
        LPC_CGU->XTAL_OSC_CTRL = (1<<2);
    } else {
        //Frequency out of range
        while(1);
    }

    G_OscillatorFrequency = aFreq->iOscillatorClockHz;

    //Enable the Crystal oscillator
    LPC_CGU->XTAL_OSC_CTRL &= ~CGU_CTRL_EN_MASK;

    //Delay 250 uS
    us = 250 * (4);
    while(us--);

    LPC_CGU->PLL1_CTRL = (0x6<<24) | (1<<11);

    IPLLSetup1((T_pllRegs *)LPC_CGU, aFreq->iOscillatorClockHz, aFreq->iOscillatorClockHz);

    //Select the PLL1 as BASE_M4_CLK source
    LPC_CGU->BASE_M4_CLK = (1<<11) | (0x9<<24);

    IPLLSetup1((T_pllRegs *)LPC_CGU, aFreq->iPLL1Frequency/2, aFreq->iOscillatorClockHz);

    //wait 50us
    us = 50 * (4);
    while(us--);

    //Ramp up to full speed
    IPLLSetup1((T_pllRegs *)LPC_CGU, aFreq->iPLL1Frequency, aFreq->iOscillatorClockHz);

    TUInt32 n,c,temp;
    int i;

    /* Maximum allow RCOUNT number */
    c= 511;
    /* Check PLL1 frequency */
    LPC_CGU->FREQ_MON = (0x9<<24) | 1<<23 | c;
    while(LPC_CGU->FREQ_MON & (1 <<23));
    for(i=0;i<10000;i++);
    temp = (LPC_CGU->FREQ_MON >>9) & 0x3FFF;

    if(temp == 0) { /* too low F < 12000000/511*/
        //return -1;
    }
    if(temp > 511){ /* larger */

        c = 511 - (LPC_CGU->FREQ_MON&0x1FF);
    }else{
        do{
            c--;
            LPC_CGU->FREQ_MON = (0x9<<24) | 1<<23 | c;
            while(LPC_CGU->FREQ_MON & (1 <<23));
            for(i=0;i<10000;i++);
            n = (LPC_CGU->FREQ_MON >>9) & 0x3FFF;
        }while(n==temp);
        c++;
    }

    G_ProcessorFrequency = aFreq->iOscillatorClockHz * ((temp / c) + 1);

    if(aFreq->iPLL1Frequency <= 180000000){
        LPC_CCU1->CLK_M4_EMCDIV_CFG = (0<<5) | (1<<2) | (1<<1) | (1<<0);
        LPC_CCU1->CLK_M4_EMC_CFG = (0 << 2) | (0 << 1) | 1;
        LPC_CREG->CREG6 &= ~(1<<16);
    } else {
        LPC_CCU1->CLK_M4_EMCDIV_CFG = (1<<5) | (1<<2) | (1<<1) | (1<<0);     // Turn on clock / 2
        LPC_CCU1->CLK_M4_EMC_CFG = (0 << 2) | (0 << 1) | 1;
        LPC_CREG->CREG6 |= (1<<16);         // EMC divided by 2
    }

    G_EMCFrequency = G_ProcessorFrequency / aFreq->iEMCClockDivider;

    LPC_SCU->SFSCLK_0 = ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4)) | 0;            /* CLK0*/
    LPC_SCU->SFSCLK_1 = ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4)) | 0;            /* CLK1*/
    LPC_SCU->SFSCLK_2 = ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4)) | 0;            /* CLK2*/
    LPC_SCU->SFSCLK_3 = ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4)) | 0;            /* CLK3*/

    LPC_CGU->BASE_PERIPH_CLK = (9<<24) | (1<<11);
    G_PeripheralFrequency = G_ProcessorFrequency;

    if(aFreq->iPLL1Frequency > 102000000){
        LPC_CGU->IDIVC_CTRL = (9<<24) | (1<<11) | (1<<2);
    } else {
        LPC_CGU->IDIVC_CTRL = (9<<24) | (1<<11) | (0<<2);
    }

    //Divder A is set to use USB PLL
    LPC_CGU->IDIVA_CTRL = (7<<24) | (1<<11) | (0<<2);
    //Divider B is set to produce a 40MHz clock to be used by the SD Card interface
    LPC_CGU->IDIVB_CTRL = (0x0C<<24) | (1<<11) | (0xB<<2);

    LPC_CGU->IDIVD_CTRL = (1<<24); //DIV D setup for USB 1 60MHz if needed.
    LPC_CGU->IDIVE_CTRL = (9<<24) | (1<<11) | (4<<2);

    //SPIFI clock to use Divider C
    LPC_CGU->BASE_SPIFI_CLK = (0xE<<24) | (1<<11);

    //Ethernet.
    LPC_CGU->BASE_PHY_RX_CLK = (3<<24) | (1<<11);
    LPC_CGU->BASE_PHY_TX_CLK = (3<<24) | (1<<11);
    LPC_CCU1->CLK_M4_ETHERNET_CFG = 0x07;

    LPC_CGU->BASE_APB1_CLK = (9<<24) | (1<<11);
    LPC_CGU->BASE_APB3_CLK = (9<<24) | (1<<11);

    LPC_CGU->BASE_OUT_CLK = (1<<24);

    LPC_CCU2->CLK_APB2_USART3_CFG = 3;

    //1KHz and 32KHz enabled
    LPC_CREG->CREG0 = (0<<3) | (1<<0);

    //Setup USB Clock
    LPC_CGU->PLL0USB_CTRL = (6<<24) | (1<<4) | (1<<3) | (1<<2) | (0<<1) | 1;

    LPC_CGU->PLL0USB_MDIV =  (0<<28) | (24<<22) |(11<<17)| (0x7FFA<<0);
    LPC_CGU->PLL0USB_NP_DIV = (0<<12) | (0<<0);

    LPC_CGU->PLL0USB_CTRL &= ~1;

    while((LPC_CGU->PLL0USB_STAT & 0x1) == 0);

    LPC_CGU->PLL0USB_CTRL |= (1<<4);

    LPC_CCU1->CLK_M4_USB0_CFG = 3;
    LPC_CCU1->CLK_M4_USB1_CFG = 3;

    G_USBFrequency = 48000000;
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_PLL.c
 *-------------------------------------------------------------------------*/
