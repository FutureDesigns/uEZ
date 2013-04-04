/*-------------------------------------------------------------------------*
 * File:  LCDController.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC2478 LCD Controller.
 * Implementation:
 *      Most of the configuration comes down to the single table passed
 *      in.  From there, we map SDRAM to the LCD.
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
#include <uEZ.h>
#include <uEZBSP.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZRTOS.h>
#include "LPC2478_LCDController.h"
#include <HAL/Interrupt.h>

 /*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define LCD_INT_LNBUIM          (1<<2) // for LCD VSync

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_LCDController *iHAL;
    TUInt32 iBaseAddress;
    TUInt16 iPalette[256];
	// uEZ 2.04
    LCDControllerVerticalSync iVerticalSyncCallbackFunc;
    void *iVerticalSyncCallbackWorkspace;
} T_workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_LCDController LCDController_LPC2478_Interface;
static T_workspace *G_lpc2478_lcdWorkspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void ILPC2478_ProcessVerticalSync(T_workspace *p);
T_uezError LPC2478_LCDController_DisableVerticalSync(void *aWorkspace);

/*---------------------------------------------------------------------------*
 * Routine:  LCDControllerVerticalIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      Interrupt handler for LCD interrupts.  Process the interrupt
 *      in ILPC2478_ProcessVerticalSync().
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LCDControllerVerticalIRQ)
{
    IRQ_START();
    ILPC2478_ProcessVerticalSync(G_lpc2478_lcdWorkspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC2478_ProcessVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Interrupt handler for LCD interrupts.  Process the interrupt
 *      in ILPC2478_ProcessVerticalSync().
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
void ILPC2478_ProcessVerticalSync(T_workspace *p)
{
    // Determine the type of interrupt
    TUInt32 flags = LCD_INTSTAT;

    // Next base address updated?
    if (flags & LCD_INT_LNBUIM) {
        // Make the callback (if one is available)
        if (p->iVerticalSyncCallbackFunc) {
            // Make the callback
            p->iVerticalSyncCallbackFunc(p->iVerticalSyncCallbackWorkspace);
        }
    }

    // Clear any interrupts processed by this interrupt
    LCD_INTCLR = flags;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this LCD Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_InitializeWorkspace(void *aWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;
    p->iHAL = &LCDController_LPC2478_Interface;
    p->iBaseAddress = 0xA0000000;    // use default address
		// uEZ 2.04
    p->iVerticalSyncCallbackFunc = 0;
    p->iVerticalSyncCallbackWorkspace = 0;

    // Setup workspace for this space for interrupts
    G_lpc2478_lcdWorkspace = p;

    // Register interrupts for this LCD Controller
    InterruptRegister(INTERRUPT_CHANNEL_LCD, LCDControllerVerticalIRQ,
        INTERRUPT_PRIORITY_NORMAL, "VSync");
    InterruptDisable(INTERRUPT_CHANNEL_LCD);
		
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the LCD screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      T_LCDControllerSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_Configure(
            void *aWorkspace,
            T_LCDControllerSettings *aSettings)
{
#if 0
    static TUInt32 bitsPerPixel[] = {
	    1, // LCD_COLOR_RES_1=0,
	    2, // LCD_COLOR_RES_2,
	    4, // LCD_COLOR_RES_4,
	    8, // LCD_COLOR_RES_8,
	    16, // LCD_COLOR_RES_16_I555,  // first bit is intensity bit (1=high/0=low)
	    24, // LCD_COLOR_RES_24,	
	    16, //LCD_COLOR_RES_16_565,
	    16, //LCD_COLOR_RES_12_444,   // uses 16 bits to store 12 levels  0:R4:G4:B4
    };
#endif

    TUInt32 regValue = 0;
    TUInt32* pPal = 0;
    TUInt32 dotClockDivider;
    int i = 0;
    T_workspace *p = (T_workspace *)aWorkspace;
    TUInt32 calcDiv;

    // Turn on the power to the subsystem
    PCONP |= 0x00100000;        /* Power Control for CLCDC */

    // Disable ETM since LPC2478 shares these lines with LCD
    PINSEL10 = 0;

    // disable the display`
    LCD_CTRL &= ~LCD_CTRL_LcdEn;

    // ppl value to set = ppl / 16 -1 (2:7)
    // hsw value to set = hsw - 1     (8:15)
    // hfp value to set = hfp - 1     (16:23)
    // hbp value to set = hbp - 1     (24:31)
    LCD_TIMH = ((((((aSettings->iHorizontalPixelsPerLine)/16)-1) & 0x3F) << 2)
                | (((aSettings->iHorizontalSyncPulseWidth-1) & 0xFF) << 8)
                | (((aSettings->iHorizontalFrontPorch-1) & 0xFF) << 16)
                | (((aSettings->iHorizontalBackPorch-1) & 0xFF) << 24));

    LCD_TIMV = ((((aSettings->iVerticalLinesPerPanel-1) &0x3FF) << 0)
                | (((aSettings->iVerticalSyncPulseWidth-1) &0x03F) << 10)
                | (((aSettings->iVerticalFrontPorch-1) &0x0FF) << 16)
                | (((aSettings->iVerticalBackPorch-1) &0x0FF) << 24));

    regValue = 0;
    if(aSettings->iACBiasPinFrequency)
        regValue = (((aSettings->iACBiasPinFrequency-1) & 0x1F) << 6);

    if(aSettings->iInvertOutputEnable)
        regValue |= LCD_POL_IOE;

    if(aSettings->iInvertPanelClock)
        regValue |= LCD_POL_IPC;

    if(aSettings->iInvertHorizontalSync)
        regValue |= LCD_POL_IHS;

    if(aSettings->iInvertVerticalSync)
        regValue |= LCD_POL_IVS;

    // clocks per line (CPL)
    switch(aSettings->iPanelType)
    {
        case LCD_STANDARD_TFT:
        case LCD_ADVANCED_TFT:
        case LCD_HIGHLY_REFLECTIVE_TFT:
        default:
            regValue |= (((aSettings->iHorizontalPixelsPerLine-1) & 0x3FF) << 16);
            break;
        case LCD_4BIT_MONO:
            regValue |=  ((((aSettings->iHorizontalPixelsPerLine / 4)-1) & 0x3FF) << 16);
            break;
        case LCD_8BIT_MONO:
            regValue |=  ((((aSettings->iHorizontalPixelsPerLine / 8)-1) & 0x3FF) << 16);
            break;
        case LCD_COLOR_STN:
            regValue |= (((((aSettings->iHorizontalPixelsPerLine * 3)/8)-1) & 0x3FF) << 16);
            break;
    }

    // Find the dot clock divider with the fastest clock rate to match the requested
    // dot clock rate without going over.
    for (dotClockDivider=0; dotClockDivider<0x0F; dotClockDivider++) {
        calcDiv = (PROCESSOR_OSCILLATOR_FREQUENCY/(dotClockDivider+2));
        if (calcDiv <= aSettings->iDotClockHz)
            break;
    }
    regValue |= dotClockDivider;

    LCD_POL = regValue;

    // skip line end control
    if (aSettings->iLineEndDelay) {
        // Enable and set
        LCD_LE = ((aSettings->iLineEndDelay-1)<<0) | (1<<16);
    } else {
        LCD_LE = 0;
    }

    // disable interrupts
    // TBD: May need vertical interrupt in the future
    LCD_INTMSK = 0;

     switch (aSettings->iColorResolution)  {
        default:
        case LCD_COLOR_RES_1:
            regValue = (0<<LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_2:
            regValue = (1<<LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_4:
            regValue = (2<<LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_8:
            regValue = (3<<LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_12_444:
            regValue = (7<<LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_16_I555:
            regValue = (4<<LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_16_565:
            regValue = (6<<LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_24:
            regValue = (5<<LCD_CTRL_LcdBpp_BIT);
            break;
    }

    // default BGR
    if (aSettings->iColorOrder == LCD_COLOR_ORDER_BGR)
        regValue |= LCD_CTRL_BGR;

    switch(aSettings->iPanelType)
    {
        case LCD_ADVANCED_TFT:
        case LCD_HIGHLY_REFLECTIVE_TFT:
        case LCD_STANDARD_TFT:
            // Enable port and use TFT
            if (aSettings->iColorResolution == LCD_COLOR_RES_16_I555)
                PINSEL11 = 0x0000000D;
            else if (aSettings->iColorResolution == LCD_COLOR_RES_16_565)
                PINSEL11 = 0x0000000B;
            else if (aSettings->iColorResolution == LCD_COLOR_RES_24)
                PINSEL11 = 0x0000000F;
            else if (aSettings->iColorResolution == LCD_COLOR_RES_12_444)
                PINSEL11 = 0x00000009;
            regValue |= LCD_CTRL_LcdTFT;
            break;
        case LCD_4BIT_MONO:
            // Enable port and use 4-bit STN
            PINSEL11 = 0x00000001;
            regValue |= LCD_CTRL_LcdBW;
            break;
        case LCD_8BIT_MONO:            // Enable port and use 8-bit STN
            PINSEL11 = 0x00000003;
            regValue |= (LCD_CTRL_LcdMon8 | LCD_CTRL_LcdBW);
            break;
        case LCD_COLOR_STN:
            PINSEL11 = 0x00000003;
            break;
        default:
            // ERROR
            break;
    }

    if(aSettings->iIsDualPanel)
        regValue |= LCD_CTRL_LcdDual;

    LCD_CTRL = regValue;

    // clear palette (all black)
    pPal = (TUInt32*) (&LCD_PAL0);
    for(i = 0; i < 128; i++)  {
        *pPal = 0;
        pPal++;
    }

    // 0 = divide by 1 dot clock
    LCD_CFG = 0x0;

    // Set the base address
    p->iBaseAddress = aSettings->iBaseAddress;
    LCD_UPBASE = p->iBaseAddress;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Power on and enable the LCD.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_On(void *aWorkspace)
{
    // Ensure the LCD is enabled and powered
    LCD_CTRL |= LCD_CTRL_LcdPwr;
    LCD_CTRL |= LCD_CTRL_LcdEn;

// TBD: Should this delay be done different?
//    UEZTaskDelay(500);
    UEZBSPDelayMS(10);  // should delay be turned on in individual LCD routine?

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Power off and disable the LCD.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_Off(void *aWorkspace)
{
    // Ensure the LCD is disabled and unpowered
    LCD_CTRL &= ~LCD_CTRL_LcdEn;
    LCD_CTRL &= ~LCD_CTRL_LcdPwr;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_SetBacklightLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Control the backlight level.  For LPC2478, this is not a feature.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      TUInt32 aLevel            -- Level to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_SetBacklightLevel(
                void *aWorkspace,
                TUInt32 aLevel)
{
    // Not handled here in this LCD Controller
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_SetBaseAddr
 *---------------------------------------------------------------------------*
 * Description:
 *      Changes the address in memory where the LCD controller gets it's
 *      pixels and if that change should occur synchronously.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      TUInt32 aBaseAddress      -- New pixel memory base address
 *      TBool aSync               -- Wait for raster?
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_SetBaseAddr(
            void *aWorkspace,
            TUInt32 aBaseAddress,
            TBool aSync)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    p->iBaseAddress = aBaseAddress;
    LCD_UPBASE = p->iBaseAddress;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_SetPaletteColor
 *---------------------------------------------------------------------------*
 * Description:
 *      Change the color of a palette entry given the red, green, blue
 *      component of a particular color index.  The color components are
 *      expressed in full 16-bit values at a higher resolution than
 *      the hardware can usually perform.  The color is down shifted to what
 *      the hardware can handle.
 * Inputs:
 *      void *aWorkspace            -- LCD Controller's workspace
 *      TUInt32 aColorIndex         -- Index to palette entry
 *      TUInt16 aRed                -- Red value
 *      TUInt16 aGreen              -- Green value
 *      TUInt16 aBlue               -- Blue value
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the index is invalid, returns
 *                                  UEZ_ERROR_OUT_OF_RANGE.
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_SetPaletteColor(
            void *aWorkspace,
            TUInt32 aColorIndex,
            TUInt16 aRed,
            TUInt16 aGreen,
            TUInt16 aBlue)
{
    TUInt32 *pPal;
    T_workspace *p = (T_workspace *)aWorkspace;
    TUInt16 neighbor;
	TUInt32 aPackedColor;

    // Don't allow buffer overruns
    if (aColorIndex >= 256)
        return UEZ_ERROR_OUT_OF_RANGE;

    // Use the upper 5 bits of red, green, and blue
    // Creating a 5/5/5 palette entry (we'll leave the last bit always
    // 0 which for the LPC24xx family means intensity or unused).
    aPackedColor =
        ((aBlue>>11)&(0x1F<<0))|
        ((aGreen>>6)&(0x1F<<5))|
        ((aRed>>1)&(0x1F<<10));

    // Half and half, the index decides which half to use.
    // For now, going to assume we are MSB configured.
    pPal = ((TUInt32*) (&LCD_PAL0)) + (aColorIndex>>1);
    if (aColorIndex & 1) {
        neighbor = p->iPalette[aColorIndex-1];
        *pPal = (neighbor<<0) | (aPackedColor<<16);
    } else {
        neighbor = p->iPalette[aColorIndex];
        *pPal = (neighbor<<16) | (aPackedColor<<0);
    }
    // Remember this in the local palette
    p->iPalette[aColorIndex] = aPackedColor;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_EnableVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable Vertical sync on LCD.
 * Inputs:
 *      void *aWorkspace                         -- LCD Controller's workspace
 *      LCDControllerVerticalSync aCallbackFunc  -- Call back function
 *      void *aCallbackWorkspace                 -- Call back workspace
 * Outputs:
 *      T_uezError                               -- If successful, returns
 *                                               UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_EnableVerticalSync(
    void *aWorkspace,
    LCDControllerVerticalSync aCallbackFunc,
    void *aCallbackWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    // Disable the interrupt
    if (p->iVerticalSyncCallbackFunc)
        LPC2478_LCDController_DisableVerticalSync(aWorkspace);

    p->iVerticalSyncCallbackFunc = aCallbackFunc;
    p->iVerticalSyncCallbackWorkspace = aCallbackWorkspace;

    // Enable the mask
    LCD_INTMSK |= LCD_INT_LNBUIM;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_LCDController_DisableVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable Vertical sync on LCD.
 * Inputs:
 *      void *aWorkspace       -- LCD Controller's workspace
 * Outputs:
 *      T_uezError             -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_LCDController_DisableVerticalSync(void *aWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    if (p->iVerticalSyncCallbackFunc) {
        // Disable the mask (if it was on)
        LCD_INTMSK &= ~LCD_INT_LNBUIM;

        p->iVerticalSyncCallbackFunc = 0;
        p->iVerticalSyncCallbackWorkspace = 0;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_LCDController_Require(const T_LPC2478_LCDController_Pins *aPins)
{
    static const T_LPC2478_PINSEL_ConfigList pinsVD0[] = {
            {GPIO_P0_4,  1}, // LCD_VD0
            {GPIO_P2_6,  3}, // LCD_VD0
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD1[] = {
            {GPIO_P0_5,  1}, // LCD_VD1
            {GPIO_P2_7,  3}, // LCD_VD1
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD2[] = {
            {GPIO_P2_8,  3}, // LCD_VD2
            {GPIO_P4_28, 2}, // LCD_VD2
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD3[] = {
            {GPIO_P2_9,  3}, // LCD_VD3
            {GPIO_P2_12, 1}, // LCD_VD3
            {GPIO_P4_29, 2}, // LCD_VD3
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD4[] = {
            {GPIO_P2_6,  3}, // LCD_VD4
            {GPIO_P2_12, 1}, // LCD_VD4
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD5[] = {
            {GPIO_P2_7,  3}, // LCD_VD5
            {GPIO_P2_13, 1}, // LCD_VD5
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD6[] = {
            {GPIO_P1_20, 1}, // LCD_VD6
            {GPIO_P2_8,  3}, // LCD_VD6
            {GPIO_P4_28, 2}, // LCD_VD6
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD7[] = {
            {GPIO_P1_21, 1}, // LCD_VD7
            {GPIO_P2_9,  3}, // LCD_VD7
            {GPIO_P4_29, 2}, // LCD_VD7
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD8[] = {
            {GPIO_P0_6,  1}, // LCD_VD8
            {GPIO_P1_22, 1}, // LCD_VD8
            {GPIO_P2_12, 1}, // LCD_VD8
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD9[] = {
            {GPIO_P0_7,  1}, // LCD_VD9
            {GPIO_P1_23, 1}, // LCD_VD9
            {GPIO_P2_13, 1}, // LCD_VD9
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD10[] = {
            {GPIO_P1_20, 1}, // LCD_VD10
            {GPIO_P1_24, 1}, // LCD_VD10
            {GPIO_P4_28, 2}, // LCD_VD10
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD11[] = {
            {GPIO_P1_21, 1}, // LCD_VD11
            {GPIO_P1_25, 1}, // LCD_VD11
            {GPIO_P4_29, 2}, // LCD_VD11
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD12[] = {
            {GPIO_P1_22, 1}, // LCD_VD12
            {GPIO_P1_26, 1}, // LCD_VD12
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD13[] = {
            {GPIO_P1_23, 1}, // LCD_VD13
            {GPIO_P1_27, 1}, // LCD_VD13
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD14[] = {
            {GPIO_P1_24, 1}, // LCD_VD14
            {GPIO_P1_28, 1}, // LCD_VD14
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD15[] = {
            {GPIO_P1_25, 1}, // LCD_VD15
            {GPIO_P1_29, 1}, // LCD_VD15
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD16[] = {
            {GPIO_P0_8,  1}, // LCD_VD16
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD17[] = {
            {GPIO_P0_9,  1}, // LCD_VD17
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD18[] = {
            {GPIO_P2_12, 1}, // LCD_VD18
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD19[] = {
            {GPIO_P2_13, 1}, // LCD_VD19
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD20[] = {
            {GPIO_P1_26, 1}, // LCD_VD20
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD21[] = {
            {GPIO_P1_27, 1}, // LCD_VD21
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD22[] = {
            {GPIO_P1_28, 1}, // LCD_VD22
    };
    static const T_LPC2478_PINSEL_ConfigList pinsVD23[] = {
            {GPIO_P1_29, 1}, // LCD_VD23
    };
    static const T_LPC2478_PINSEL_ConfigList pinsPWR[] = {
            {GPIO_P2_0,  3}, // LCD_PWR
    };
    static const T_LPC2478_PINSEL_ConfigList pinsLE[] = {
            {GPIO_P2_1,  3}, // LCD_LE
    };
    static const T_LPC2478_PINSEL_ConfigList pinsDCLK[] = {
            {GPIO_P2_2,  3}, // LCD_DCLK
    };
    static const T_LPC2478_PINSEL_ConfigList pinsFP[] = {
            {GPIO_P2_3,  3}, // LCD_FP
    };
    static const T_LPC2478_PINSEL_ConfigList pinsENAB[] = {
            {GPIO_P2_4,  3}, // LCD_ENAB_M
    };
    static const T_LPC2478_PINSEL_ConfigList pinsLP[] = {
            {GPIO_P2_5,  3}, // LCD_LP
    };
    static const T_LPC2478_PINSEL_ConfigList pinsCLKIN[] = {
            {GPIO_P2_11, 1}, // LCD_CLKIN
    };

    HAL_DEVICE_REQUIRE_ONCE();

    // Register LCD Controller
    HALInterfaceRegister("LCDController",
            (T_halInterface *)&LCDController_LPC2478_Interface, 0, 0);
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iPWR, pinsPWR,
            ARRAY_COUNT(pinsPWR));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iLE, pinsLE,
            ARRAY_COUNT(pinsLE));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iDCLK, pinsDCLK,
            ARRAY_COUNT(pinsDCLK));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iFP, pinsFP,
            ARRAY_COUNT(pinsFP));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iENAB_M, pinsENAB,
            ARRAY_COUNT(pinsENAB));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iLP, pinsLP,
            ARRAY_COUNT(pinsLP));

    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[0], pinsVD0,
            ARRAY_COUNT(pinsVD0));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[1], pinsVD1,
            ARRAY_COUNT(pinsVD1));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[2], pinsVD2,
            ARRAY_COUNT(pinsVD2));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[3], pinsVD3,
            ARRAY_COUNT(pinsVD3));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[4], pinsVD4,
            ARRAY_COUNT(pinsVD4));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[5], pinsVD5,
            ARRAY_COUNT(pinsVD5));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[6], pinsVD6,
            ARRAY_COUNT(pinsVD6));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[7], pinsVD7,
            ARRAY_COUNT(pinsVD7));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[8], pinsVD8,
            ARRAY_COUNT(pinsVD8));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[9], pinsVD9,
            ARRAY_COUNT(pinsVD9));

    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[10], pinsVD10,
            ARRAY_COUNT(pinsVD10));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[11], pinsVD11,
            ARRAY_COUNT(pinsVD11));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[12], pinsVD12,
            ARRAY_COUNT(pinsVD12));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[13], pinsVD13,
            ARRAY_COUNT(pinsVD13));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[14], pinsVD14,
            ARRAY_COUNT(pinsVD14));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[15], pinsVD15,
            ARRAY_COUNT(pinsVD15));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[16], pinsVD16,
            ARRAY_COUNT(pinsVD16));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[17], pinsVD17,
            ARRAY_COUNT(pinsVD17));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[18], pinsVD18,
            ARRAY_COUNT(pinsVD18));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[19], pinsVD19,
            ARRAY_COUNT(pinsVD19));

    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[20], pinsVD20,
            ARRAY_COUNT(pinsVD20));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[21], pinsVD21,
            ARRAY_COUNT(pinsVD21));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[22], pinsVD22,
            ARRAY_COUNT(pinsVD22));
    LPC2478_PINSEL_ConfigPinOrNone(aPins->iVD[23], pinsVD23,
            ARRAY_COUNT(pinsVD23));

    LPC2478_PINSEL_ConfigPinOrNone(aPins->iCLKIN, pinsCLKIN,
            ARRAY_COUNT(pinsCLKIN));

    // Directly change the priorities of the AHB1 bus
    // We want the LCD to have a higher configuration than the
    // rest to avoid flickering when accessing the NOR flash
    // and running the LCD screen.
    //   3322 2222 2222 1111 1111 1100 0000 0000
    //   1098 7654 3210 9876 5432 1098 7654 3210
    //   0--- ---- ---- ---- ---- ---- ---- ----    31: Reserved
    //   -101 ---- ---- ---- ---- ---- ---- ---- 30-28: LCD priority 5 (highest)
    //   ---- 0--- ---- ---- ---- ---- ---- ----    27: Reserved
    //   ---- -011 ---- ---- ---- ---- ---- ---- 26-24: USB priority 3 (middle)
    //   ---- ---- 0--- ---- ---- ---- ---- ----    23: Reserved
    //   ---- ---- -001 ---- ---- ---- ---- ---- 22-20: AHB1 priority 1 (lowest)
    //   ---- ---- ---- 0--- ---- ---- ---- ----    19: Reserved
    //   ---- ---- ---- -100 ---- ---- ---- ---- 18-16: GPDMA priority 4 (next to highest)
    //   ---- ---- ---- ---- 0--- ---- ---- ----    15: Reserved
    //   ---- ---- ---- ---- -010 ---- ---- ---- 14-12: CPU prioirty 2 (next to lowest)
    //   ---- ---- ---- ---- ---- 0--- ---- ----    11: Reserved
    //   ---- ---- ---- ---- ---- -001 ---- ----  10-8: Master 1 (CPU) is the default master
    //   ---- ---- ---- ---- ---- ---- 0100 ----   7-4: Premeptive, re-aribtrate after 16 AHB quanta
    //   ---- ---- ---- ---- ---- ---- ---- 0---     3: A quanta is an AHB bus cycle
    //   ---- ---- ---- ---- ---- ---- ---- -10-   2-1: Break all defined length bursts greater than 8-beat
    //   ---- ---- ---- ---- ---- ---- ---- ---0     0: Priority scheduling
    AHBCFG1 = 0x53142144; // original
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_LCDController LCDController_LPC2478_Interface = {
    "LPC2478 LCDController",
    0x0100,
    LPC2478_LCDController_InitializeWorkspace,
    sizeof(T_workspace),

    LCDCONTROLLER_FEATURE_POWER|
        LCDCONTROLLER_FEATURE_DUAL_PANEL|
        LCDCONTROLLER_FEATURE_DYNAMIC_BASE_ADDR|
        LCDCONTROLLER_FEATURE_PALETTE_CONTROL,

    LPC2478_LCDController_Configure,
    LPC2478_LCDController_On,
    LPC2478_LCDController_Off,
    LPC2478_LCDController_SetBacklightLevel,
    LPC2478_LCDController_SetBaseAddr,
    LPC2478_LCDController_SetPaletteColor,

    // v2.04
    LPC2478_LCDController_EnableVerticalSync,
    LPC2478_LCDController_DisableVerticalSync
};

/*-------------------------------------------------------------------------*
 * End of File:  LCDController.c
 *-------------------------------------------------------------------------*/
