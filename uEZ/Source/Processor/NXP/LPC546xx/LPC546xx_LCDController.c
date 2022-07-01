/*-------------------------------------------------------------------------*
 * File:  LCDController.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx LCD Controller.
 * Implementation:
 *      Most of the configuration comes down to the single table passed
 *      in.  From there, we map SDRAM to the LCD.
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
#include <uEZPlatform.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_LCDController.h>
#include <uEZBSP.h>
#include <HAL/Interrupt.h>

//TDOD: remove
#include "iar/Include/CMSIS/LPC54608.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#undef LCD_POL_IVS
#undef LCD_POL_IHS
#undef LCD_POL_IPC
#undef LCD_POL_IOE
#undef LCD_CTRL_BGR

#define LCD_POL_IVS             (1<<11)
#define LCD_POL_IHS             (1<<12)
#define LCD_POL_IPC             (1<<13)
#define LCD_POL_IOE             (1<<14)
#define LCD_CTRL_LcdBpp_BIT     1
#define LCD_CTRL_LcdEn          (1<<0)
#define LCD_CTRL_LcdBW          (1<<4)
#define LCD_CTRL_LcdTFT         (1<<5)
#define LCD_CTRL_LcdMon8        (1<<6)
#define LCD_CTRL_LcdDual        (1<<7)
#define LCD_CTRL_BGR            (1<<8)
#define LCD_CTRL_LcdPwr         (1<<11)

#define LCD_INT_LNBUIM          (1<<2)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_LCDController *iHAL;
    TUInt32 iBaseAddress;
    TUInt16 iPalette[256];
    HAL_GPIOPort **iPowerPort;
    TUInt32 iPowerPin;
    TBool iPowerPinIsActiveHigh;
    TUInt32 iPowerOnDelay;

    LCDControllerVerticalSync iVerticalSyncCallbackFunc;
    void *iVerticalSyncCallbackWorkspace;
} T_workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_LCDController LCDController_LPC546xx_Interface;
static T_workspace *G_LPC546xx_lcdWorkspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void ILPC546xx_ProcessVerticalSync(T_workspace *p);
T_uezError LPC546xx_LCDController_DisableVerticalSync(void *aWorkspace);

/*---------------------------------------------------------------------------*
 * Routine:  LCDControllerVerticalIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      Interrupt handler for LCD interrupts.  Process the interrupt
 *      in ILPC546xx_ProcessVerticalSync().
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LCDControllerVerticalIRQ)
{
    IRQ_START();
    ILPC546xx_ProcessVerticalSync(G_LPC546xx_lcdWorkspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC546xx_ProcessVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Interrupt handler for LCD interrupts.  Process the interrupt
 *      in ILPC546xx_ProcessVerticalSync().
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
void ILPC546xx_ProcessVerticalSync(T_workspace *p)
{
    // Determine the type of interrupt
    TUInt32 flags = LCD->INTSTAT;

    // Next base address updated?
    if (flags & LCD_INT_LNBUIM) {
        // Make the callback (if one is available)
        if (p->iVerticalSyncCallbackFunc) {
            // Make the callback
            p->iVerticalSyncCallbackFunc(p->iVerticalSyncCallbackWorkspace);
        }
    }
    // Clear any interrupts processed by this interrupt
    LCD->INTCLR = flags;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this LCD Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_LCDController_InitializeWorkspace(void *aWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;
    p->iHAL = &LCDController_LPC546xx_Interface;
    p->iBaseAddress = 0x28000000; // use default address
    p->iPowerPort = 0;
    p->iPowerPin = 0;
    p->iPowerPinIsActiveHigh = EFalse;
    p->iPowerOnDelay = 10; // ms
    p->iVerticalSyncCallbackFunc = 0;
    p->iVerticalSyncCallbackWorkspace = 0;

    // Setup workspace for this space for interrupts
    G_LPC546xx_lcdWorkspace = p;

    // Register interrupts for this LCD Controller
    InterruptRegister(LCD_IRQn, LCDControllerVerticalIRQ,
        INTERRUPT_PRIORITY_NORMAL, "VSync");
    InterruptEnable(LCD_IRQn);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the LCD screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      T_LCDControllerSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_LCDController_Configure(
        void *aWorkspace,
        T_LCDControllerSettings *aSettings)
{
    TUInt32 regValue = 0;
    TUInt32* pPal = 0;
    TUInt32 dotClockDivider;
    int i = 0;
    T_workspace *p = (T_workspace *)aWorkspace;
    TUInt32 calcDiv;

    // Turn on the power to the subsystem
    //Turn on peripheral clock
    LPC546xxPowerOn(kCLOCK_Lcd);
    SYSCON->LCDCLKDIV = 0;
    SYSCON->LCDCLKSEL = 0;
    
    SYSCON->PRESETCTRLSET[2] = (1<<2);    
    SYSCON->PRESETCTRLCLR[2] = (1<<2);

    // disable the display`
    LCD->CTRL &= ~LCD_CTRL_LcdEn;

    // ppl value to set = ppl / 16 -1 (2:7)
    // hsw value to set = hsw - 1     (8:15)
    // hfp value to set = hfp - 1     (16:23)
    // hbp value to set = hbp - 1     (24:31)
    LCD->TIMH = ((((((aSettings->iHorizontalPixelsPerLine) / 16) - 1)
            & 0x3F) << 2)
            | (((aSettings->iHorizontalSyncPulseWidth - 1) & 0xFF) << 8)
            | (((aSettings->iHorizontalFrontPorch - 1) & 0xFF) << 16)
            | (((aSettings->iHorizontalBackPorch - 1) & 0xFF) << 24));

    LCD->TIMV = ((((aSettings->iVerticalLinesPerPanel - 1) & 0x3FF) << 0)
            | (((aSettings->iVerticalSyncPulseWidth - 1) & 0x03F) << 10)
            | (((aSettings->iVerticalFrontPorch) & 0x0FF) << 16)
            | (((aSettings->iVerticalBackPorch) & 0x0FF) << 24));

    regValue = 0;
    if (aSettings->iACBiasPinFrequency)
        regValue = (((aSettings->iACBiasPinFrequency - 1) & 0x1F) << 6);

    if (aSettings->iInvertOutputEnable)
        regValue |= LCD_POL_IOE;

    if (aSettings->iInvertPanelClock)
        regValue |= LCD_POL_IPC;

    if (aSettings->iInvertHorizontalSync)
        regValue |= LCD_POL_IHS;

    if (aSettings->iInvertVerticalSync)
        regValue |= LCD_POL_IVS;

    // clocks per line (CPL)
    switch (aSettings->iPanelType) {
        case LCD_STANDARD_TFT:
        case LCD_ADVANCED_TFT:
        case LCD_HIGHLY_REFLECTIVE_TFT:
        default:
            regValue |= (((aSettings->iHorizontalPixelsPerLine - 1) & 0x3FF)
                    << 16);
            break;
        case LCD_4BIT_MONO:
            regValue |= ((((aSettings->iHorizontalPixelsPerLine / 4) - 1)
                    & 0x3FF) << 16);
            break;
        case LCD_8BIT_MONO:
            regValue |= ((((aSettings->iHorizontalPixelsPerLine / 8) - 1)
                    & 0x3FF) << 16);
            break;
        case LCD_COLOR_STN:
            regValue |= (((((aSettings->iHorizontalPixelsPerLine * 3) / 8) - 1)
                    & 0x3FF) << 16);
            break;
    }

    // Find the dot clock divider with the fastest clock rate to match the requested
    // dot clock rate without going over.
    for (dotClockDivider = 0; dotClockDivider < 0x0F; dotClockDivider++) {
        calcDiv = (PROCESSOR_OSCILLATOR_FREQUENCY / (dotClockDivider + 2));
        if (calcDiv <= aSettings->iDotClockHz)
            break;
    }
    regValue |= dotClockDivider;

    LCD->POL = regValue;

    // skip line end control
    if (aSettings->iLineEndDelay) {
        // Enable and set
        LCD->LE = ((aSettings->iLineEndDelay - 1) << 0) | (1 << 16);
    } else {
        LCD->LE = 0;
    }

    // disable interrupts
    // TBD: May need vertical interrupt in the future
    LCD->INTMSK = 0;

    switch (aSettings->iColorResolution) {
        default:
        case LCD_COLOR_RES_1:
            regValue = (0 << LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_2:
            regValue = (1 << LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_4:
            regValue = (2 << LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_8:
            regValue = (3 << LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_12_444:
            regValue = (7 << LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_16_I555:
            regValue = (4 << LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_16_565:
            regValue = (6 << LCD_CTRL_LcdBpp_BIT);
            break;
        case LCD_COLOR_RES_24:
            regValue = (5 << LCD_CTRL_LcdBpp_BIT);
            break;
    }

    // default BGR
    if (aSettings->iColorOrder == LCD_COLOR_ORDER_BGR)
        regValue |= LCD_CTRL_BGR;

    switch (aSettings->iPanelType) {
        case LCD_ADVANCED_TFT:
        case LCD_HIGHLY_REFLECTIVE_TFT:
        case LCD_STANDARD_TFT:
            // Enable port and use TFT
            regValue |= LCD_CTRL_LcdTFT;
            break;
        case LCD_4BIT_MONO:
            // Enable port and use 4-bit STN
            regValue |= LCD_CTRL_LcdBW;
            break;
        case LCD_8BIT_MONO: // Enable port and use 8-bit STN
            regValue |= (LCD_CTRL_LcdMon8 | LCD_CTRL_LcdBW);
            break;
        case LCD_COLOR_STN:
            break;
        default:
            // ERROR
            break;
    }

    if (aSettings->iIsDualPanel)
        regValue |= LCD_CTRL_LcdDual;

    LCD->CTRL = regValue;

    // clear palette (all black)
    pPal = (TUInt32*)(&(LCD->PAL));
    for (i = 0; i < 128; i++) {
        *pPal = 0;
        pPal++;
    }

    // 0 = divide by 1 dot clock
    //TBD: Where is this register really?    LCD->CFG = 0x0;

    // Set the base address
    p->iBaseAddress = aSettings->iBaseAddress;
    LCD->UPBASE = p->iBaseAddress;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Power on and enable the LCD.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_LCDController_On(void *aWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    // Ensure the LCD is enabled and powered
    LCD->CTRL |= LCD_CTRL_LcdEn;
    LCD->CTRL |= LCD_CTRL_LcdPwr;

    if (p->iPowerPort) {
        if (p->iPowerPinIsActiveHigh) {
            (*p->iPowerPort)->Set(p->iPowerPort, p->iPowerPin);
        } else {
            (*p->iPowerPort)->Clear(p->iPowerPort, p->iPowerPin);
        }
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Power off and disable the LCD.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_LCDController_Off(void *aWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    // Ensure the LCD is enabled and powered
    LCD->CTRL &= ~LCD_CTRL_LcdEn;
    LCD->CTRL &= ~LCD_CTRL_LcdPwr;

    // Turn off the power pin if we have one
    if (p->iPowerPort) {
        if (p->iPowerPinIsActiveHigh) {
            (*p->iPowerPort)->Clear(p->iPowerPort, p->iPowerPin);
        } else {
            (*p->iPowerPort)->Set(p->iPowerPort, p->iPowerPin);
        }
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_SetBacklightLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Control the backlight level.  For LPC546xx, this is not a feature.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      TUInt32 aLevel            -- Level to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_LCDController_SetBacklightLevel(
        void *aWorkspace,
        TUInt32 aLevel)
{
    // Not handled here in this LCD Controller
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_SetBaseAddr
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
T_uezError LPC546xx_LCDController_SetBaseAddr(
        void *aWorkspace,
        TUInt32 aBaseAddress,
        TBool aSync)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    p->iBaseAddress = aBaseAddress;
    LCD->UPBASE = p->iBaseAddress;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_SetPaletteColor
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
T_uezError LPC546xx_LCDController_SetPaletteColor(
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
    aPackedColor = ((aBlue >> 11) & (0x1F << 0))
            | ((aGreen >> 6) & (0x1F << 5)) | ((aRed >> 1) & (0x1F << 10));

    // Half and half, the index decides which half to use.
    // For now, going to assume we are MSB configured.
    pPal = ((TUInt32*)(&LCD->PAL[0])) + (aColorIndex >> 1);
    if (aColorIndex & 1) {
        neighbor = p->iPalette[aColorIndex - 1];
        *pPal = (neighbor << 0) | (aPackedColor << 16);
    } else {
        neighbor = p->iPalette[aColorIndex];
        *pPal = (neighbor << 16) | (aPackedColor << 0);
    }
    // Remember this in the local palette
    p->iPalette[aColorIndex] = aPackedColor;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCDController_LPC546xx_ConfigurePowerPin
 *---------------------------------------------------------------------------*
 * Description:
 *      Configures the power pin
 * Inputs:
 *      void *aWorkspace             -- LCD Controller's workspace
 *      HAL_GPIOPort **aPowerPort    -- Pointer to HAL GPIO port
 *      TUInt32 aPowerPinIndex       -- Index of port pin
 *      TBool aPowerPinIsActiveHigh  -- ETrue if active high, EFalse otherwise
 *      TUInt32 aPowerOnDelay        -- Delay (in milliseconds?)
 * Outputs:
 *      T_uezError                   -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCDController_LPC546xx_ConfigurePowerPin(
            void *aWorkspace,
            HAL_GPIOPort **aPowerPort,
            TUInt32 aPowerPinIndex,
            TBool aPowerPinIsActiveHigh,
            TUInt32 aPowerOnDelay)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    p->iPowerPort = aPowerPort;
    p->iPowerPin = 1<<aPowerPinIndex;
    p->iPowerPinIsActiveHigh = aPowerPinIsActiveHigh;
    p->iPowerOnDelay = aPowerOnDelay;

    // Ensure it is a GPIO and output mode and off
    if (p->iPowerPinIsActiveHigh)
        (*p->iPowerPort)->Clear(p->iPowerPort, p->iPowerPin);
    else
        (*p->iPowerPort)->Set(p->iPowerPort, p->iPowerPin);

    (*p->iPowerPort)->SetMux(p->iPowerPort, aPowerPinIndex, 0);
    (*p->iPowerPort)->SetOutputMode(p->iPowerPort, p->iPowerPin);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_EnableVerticalSync
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
T_uezError LPC546xx_LCDController_EnableVerticalSync(
    void *aWorkspace,
    LCDControllerVerticalSync aCallbackFunc,
    void *aCallbackWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    // Disable the interrupt
    if (p->iVerticalSyncCallbackFunc)
        LPC546xx_LCDController_DisableVerticalSync(aWorkspace);

    p->iVerticalSyncCallbackFunc = aCallbackFunc;
    p->iVerticalSyncCallbackWorkspace = aCallbackWorkspace;

    // Enable the mask
    LCD->INTMSK |= LCD_INT_LNBUIM;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_LCDController_DisableVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable Vertical sync on LCD.
 * Inputs:
 *      void *aWorkspace       -- LCD Controller's workspace
 * Outputs:
 *      T_uezError             -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_LCDController_DisableVerticalSync(void *aWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;

    if (p->iVerticalSyncCallbackFunc) {
        // Disable the mask (if it was on)
        LCD->INTMSK &= ~LCD_INT_LNBUIM;

        p->iVerticalSyncCallbackFunc = 0;
        p->iVerticalSyncCallbackWorkspace = 0;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_LCDController LCDController_LPC546xx_Interface = {
        {
        "LPC546xx LCDController",
        0x0100,
        LPC546xx_LCDController_InitializeWorkspace,
        sizeof(T_workspace),
        },

        LCDCONTROLLER_FEATURE_POWER | LCDCONTROLLER_FEATURE_DUAL_PANEL
                | LCDCONTROLLER_FEATURE_DYNAMIC_BASE_ADDR
                | LCDCONTROLLER_FEATURE_PALETTE_CONTROL,

        LPC546xx_LCDController_Configure,
        LPC546xx_LCDController_On,
        LPC546xx_LCDController_Off,
        LPC546xx_LCDController_SetBacklightLevel,
        LPC546xx_LCDController_SetBaseAddr,
        LPC546xx_LCDController_SetPaletteColor,
        LPC546xx_LCDController_EnableVerticalSync,
        LPC546xx_LCDController_DisableVerticalSync
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_LCDController_Require(const T_LPC546xx_LCDController_Pins *aPins)
{
    T_halWorkspace *p_lcd;
    
    static const T_LPC546xx_ICON_ConfigList pinsVD0[] = {
            {GPIO_P3_6,  IOCON_D_DEFAULT(2)}, // LCD_VD0
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD1[] = {
            {GPIO_P3_7,  IOCON_D_DEFAULT(2)}, // LCD_VD1
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD2[] = {
            {GPIO_P3_8,  IOCON_D_DEFAULT(2)}, // LCD_VD2
            {GPIO_P2_20,  IOCON_D_DEFAULT(2)}, // LCD_VD2
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD3[] = {
            {GPIO_P3_9,  IOCON_D_DEFAULT(2)}, // LCD_VD3
            {GPIO_P2_21,  IOCON_D_DEFAULT(1)}, // LCD_VD3
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD4[] = {
            {GPIO_P2_22,  IOCON_D_DEFAULT(1)}, // LCD_VD4
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD5[] = {
            {GPIO_P2_23,  IOCON_D_DEFAULT(1)}, // LCD_VD5
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD6[] = {
            {GPIO_P2_24,  IOCON_D_DEFAULT(1)}, // LCD_VD6
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD7[] = {
            {GPIO_P2_25,  IOCON_D_DEFAULT(1)}, // LCD_VD7
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD8[] = {
            {GPIO_P2_26,  IOCON_D_DEFAULT(1)}, // LCD_VD8
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD9[] = {
            {GPIO_P2_27,  IOCON_D_DEFAULT(1)}, // LCD_VD9
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD10[] = {
            {GPIO_P2_28,  IOCON_D_DEFAULT(1)}, // LCD_VD10
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD11[] = {
            {GPIO_P2_29,  IOCON_D_DEFAULT(1)}, // LCD_VD11
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD12[] = {
            {GPIO_P2_30,  IOCON_D_DEFAULT(1)}, // LCD_VD12
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD13[] = {
            {GPIO_P2_31,  IOCON_D_DEFAULT(1)}, // LCD_VD13
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD14[] = {
            {GPIO_P3_0, IOCON_D_DEFAULT(1)}, // LCD_VD14
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD15[] = {
            {GPIO_P3_1, IOCON_D_DEFAULT(1)}, // LCD_VD15
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD16[] = {
            {GPIO_P3_2,  IOCON_D_DEFAULT(1)}, // LCD_VD16
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD17[] = {
            {GPIO_P3_3,  IOCON_D_DEFAULT(1)}, // LCD_VD17
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD18[] = {
            {GPIO_P3_4, IOCON_D_DEFAULT(1)}, // LCD_VD18
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD19[] = {
            {GPIO_P3_5, IOCON_D_DEFAULT(1)}, // LCD_VD19
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD20[] = {
            {GPIO_P3_6, IOCON_D_DEFAULT(1)}, // LCD_VD20
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD21[] = {
            {GPIO_P3_7, IOCON_D_DEFAULT(1)}, // LCD_VD21
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD22[] = {
            {GPIO_P3_8, IOCON_D_DEFAULT(1)}, // LCD_VD22
    };
    static const T_LPC546xx_ICON_ConfigList pinsVD23[] = {
            {GPIO_P3_9, IOCON_D_DEFAULT(1)}, // LCD_VD23
    };
    static const T_LPC546xx_ICON_ConfigList pinsPWR[] = {
            {GPIO_P2_11,  IOCON_D_DEFAULT(1)}, // LCD_PWR
    };
    static const T_LPC546xx_ICON_ConfigList pinsLE[] = {
            {GPIO_P2_12,  IOCON_D_DEFAULT(1)}, // LCD_LE
    };
    static const T_LPC546xx_ICON_ConfigList pinsDCLK[] = {
            {GPIO_P2_13,  IOCON_D_DEFAULT(1)}, // LCD_DCLK
    };
    static const T_LPC546xx_ICON_ConfigList pinsFP[] = {
            {GPIO_P2_14,  IOCON_D_DEFAULT(1)}, // LCD_FP
    };
    static const T_LPC546xx_ICON_ConfigList pinsENAB[] = {
            {GPIO_P2_15,  IOCON_D_DEFAULT(1)}, // LCD_ENAB_AC
    };
    static const T_LPC546xx_ICON_ConfigList pinsLP[] = {
            {GPIO_P2_16,  IOCON_D_DEFAULT(1)}, // LCD_LP
    };
    static const T_LPC546xx_ICON_ConfigList pinsCLKIN[] = {
            {GPIO_P2_17, IOCON_D_DEFAULT(1)}, // LCD_CLKIN
    };

    // Register LCD Controller
    HALInterfaceRegister("LCDController",
            (T_halInterface *)&LCDController_LPC546xx_Interface, 0, &p_lcd);

    LPC546xx_ICON_ConfigPinOrNone(aPins->iPWR, pinsPWR,
            ARRAY_COUNT(pinsPWR));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iLE, pinsLE,
            ARRAY_COUNT(pinsLE));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iDCLK, pinsDCLK,
            ARRAY_COUNT(pinsDCLK));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iFP, pinsFP,
            ARRAY_COUNT(pinsFP));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iENAB_M, pinsENAB,
            ARRAY_COUNT(pinsENAB));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iLP, pinsLP,
            ARRAY_COUNT(pinsLP));

    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[0], pinsVD0,
            ARRAY_COUNT(pinsVD0));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[1], pinsVD1,
            ARRAY_COUNT(pinsVD1));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[2], pinsVD2,
            ARRAY_COUNT(pinsVD2));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[3], pinsVD3,
            ARRAY_COUNT(pinsVD3));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[4], pinsVD4,
            ARRAY_COUNT(pinsVD4));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[5], pinsVD5,
            ARRAY_COUNT(pinsVD5));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[6], pinsVD6,
            ARRAY_COUNT(pinsVD6));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[7], pinsVD7,
            ARRAY_COUNT(pinsVD7));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[8], pinsVD8,
            ARRAY_COUNT(pinsVD8));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[9], pinsVD9,
            ARRAY_COUNT(pinsVD9));

    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[10], pinsVD10,
            ARRAY_COUNT(pinsVD10));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[11], pinsVD11,
            ARRAY_COUNT(pinsVD11));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[12], pinsVD12,
            ARRAY_COUNT(pinsVD12));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[13], pinsVD13,
            ARRAY_COUNT(pinsVD13));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[14], pinsVD14,
            ARRAY_COUNT(pinsVD14));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[15], pinsVD15,
            ARRAY_COUNT(pinsVD15));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[16], pinsVD16,
            ARRAY_COUNT(pinsVD16));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[17], pinsVD17,
            ARRAY_COUNT(pinsVD17));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[18], pinsVD18,
            ARRAY_COUNT(pinsVD18));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[19], pinsVD19,
            ARRAY_COUNT(pinsVD19));

    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[20], pinsVD20,
            ARRAY_COUNT(pinsVD20));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[21], pinsVD21,
            ARRAY_COUNT(pinsVD21));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[22], pinsVD22,
            ARRAY_COUNT(pinsVD22));
    LPC546xx_ICON_ConfigPinOrNone(aPins->iVD[23], pinsVD23,
            ARRAY_COUNT(pinsVD23));

    LPC546xx_ICON_ConfigPinOrNone(aPins->iCLKIN, pinsCLKIN,
            ARRAY_COUNT(pinsCLKIN));

    if (aPins->iPowerPin != GPIO_NONE) {
        UEZGPIOLock(aPins->iPowerPin);
        if (aPins->iPowerPinIsActiveHigh) {
            UEZGPIOClear(aPins->iPowerPin);
        } else {
            UEZGPIOSet(aPins->iPowerPin);
        }
        //UEZGPIOOutput(aPins->iPowerPin);
        LCDController_LPC546xx_ConfigurePowerPin(p_lcd,
                GPIO_TO_HAL_PORT(aPins->iPowerPin),
                GPIO_TO_PIN_INDEX(aPins->iPowerPin), aPins->iPowerPinIsActiveHigh,
                aPins->iPowerOnDelay);
    }
}
/*-------------------------------------------------------------------------*
 * End of File:  LCDController.c
 *-------------------------------------------------------------------------*/