/*-------------------------------------------------------------------------*
 * File:  LPC2478_BSOD.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Utilty functions that are specific to the LPC2478 processor.
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
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZGPIO.h>

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

typedef struct {
    TUInt32 iR0;
    TUInt32 iR1;
    TUInt32 iR2;
    TUInt32 iR3;
    TUInt32 iR12;
    TUInt32 iLR;
    TUInt32 iPC;
    TUInt32 iPSR;
} T_exceptionInformation;
volatile T_exceptionInformation G_exceptionInformation;

#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Graphics/SWIM/lpc_swim_font.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16_subset.h>
#include <stdio.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
char BSODLine[100];
SWIM_WINDOW_T G_bsodWin;
TBool G_bsodActive = EFalse;
extern void UEZBSP_FatalError(int aNumBlinks);
void BSODPrint(const char *aLine)
{
    if (G_bsodActive) {
        swim_put_text(&G_bsodWin, aLine);
        swim_put_newline(&G_bsodWin);
    }
}

void BSODStart(const char *aLine)
{
    T_pixelColor *pixels;
    T_uezDevice lcd;
    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDOn(lcd);
        UEZLCDBacklight(lcd, 256);
        UEZLCDGetFrame(lcd, 0, (void **)&pixels);
        UEZLCDShowFrame(lcd, 0);
        swim_window_open(&G_bsodWin, UEZ_LCD_DISPLAY_WIDTH, UEZ_LCD_DISPLAY_HEIGHT,
                         (COLOR_T *)pixels, 0, 0, UEZ_LCD_DISPLAY_WIDTH-1,
                         UEZ_LCD_DISPLAY_HEIGHT-1, 2, WHITE, BLUE, WHITE);
        swim_set_font(&G_bsodWin, &font_winfreesys14x16_subset);
        swim_set_pen_color(&G_bsodWin, WHITE);
        G_bsodActive = ETrue;
        BSODPrint(aLine);
    }
}

void UEZFailureMsg(const char *aLine)
{
    BSODStart("UEZ Failure!\n");
    BSODPrint(aLine);
    while (1)
        UEZBSP_FatalError(10);
}

void HardFault_HandlerC(
    T_exceptionInformation *aHardFaultArgs,
    TUInt32 aExceptionCode)
{
    G_exceptionInformation.iR0 = aHardFaultArgs->iR0;
    G_exceptionInformation.iR1 = aHardFaultArgs->iR1;
    G_exceptionInformation.iR2 = aHardFaultArgs->iR2;
    G_exceptionInformation.iR3 = aHardFaultArgs->iR3;
    G_exceptionInformation.iR12 = aHardFaultArgs->iR12;
    G_exceptionInformation.iLR = aHardFaultArgs->iLR;
    G_exceptionInformation.iPC = aHardFaultArgs->iPC;
    G_exceptionInformation.iPSR = aHardFaultArgs->iPSR;

    BSODStart("** uEZ Hard Fault Exception! **\n");
    sprintf(BSODLine, "R0 : %08X", G_exceptionInformation.iR0);
    BSODPrint(BSODLine);
    sprintf(BSODLine, "R1 : %08X", G_exceptionInformation.iR1);
    BSODPrint(BSODLine);
    sprintf(BSODLine, "R2 : %08X", G_exceptionInformation.iR2);
    BSODPrint(BSODLine);
    sprintf(BSODLine, "R3 : %08X", G_exceptionInformation.iR3);
    BSODPrint(BSODLine);
    sprintf(BSODLine, "R12: %08X", G_exceptionInformation.iR12);
    BSODPrint(BSODLine);
    sprintf(BSODLine, "LR : %08X", G_exceptionInformation.iLR);
    BSODPrint(BSODLine);
    sprintf(BSODLine, "PC : %08X", G_exceptionInformation.iPC);
    BSODPrint(BSODLine);
    sprintf(BSODLine, "PSR: %08X", G_exceptionInformation.iPSR);
    BSODPrint(BSODLine);
    while (1)
        UEZBSP_FatalError(aExceptionCode);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC2478_BSOD.c
 *-------------------------------------------------------------------------*/
