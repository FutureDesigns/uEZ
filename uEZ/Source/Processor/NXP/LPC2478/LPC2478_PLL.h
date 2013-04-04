/*-------------------------------------------------------------------------*
 * File:  LPC2478_PLL.h
 *-------------------------------------------------------------------------*
 * Description:
 *    See Figure 7. Clock Generation for the LPC178x/LPC177x
 *-------------------------------------------------------------------------*/
#ifndef LPC2478_PLL_H_
#define LPC2478_PLL_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define LPC2478_IRC_FREQUENCY       12000000 // Hz +/- 1%

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    LPC2478_CLKSRC_SELECT_INTERNAL_RC = 0,
    LPC2478_CLKSRC_SELECT_MAIN_OSCILLATOR = 1
} T_LPC2478_ClockSourceSelect;

typedef struct {
        // Input frequencies:
        TUInt32 iOscillatorClockHz;

        // PLL frequencies:
        // PLL0 is the processor frequency:
        TUInt32 iPLL0Frequency;

        // Dividers:
        TUInt8 iCPUClockDivider; // 1 to 31
        TUInt8 iPeripheralClockDivider; // 1 to 31
        TUInt8 iEMCClockDivider; // 1 or 2
        TUInt8 iUSBClockDivider;
} T_LPC2478_PLL_Frequencies;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern TUInt32 G_OscillatorFrequency;
extern TUInt32 G_ProcessorFrequency;
extern TUInt32 G_PeripheralFrequency;
extern TUInt32 G_EMCFrequency;
extern TUInt32 G_USBFrequency;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC2478_PLL_SetFrequencies(const T_LPC2478_PLL_Frequencies *aFreq);

#endif // LPC2478_PLL_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC2478_PLL.h
 *-------------------------------------------------------------------------*/
