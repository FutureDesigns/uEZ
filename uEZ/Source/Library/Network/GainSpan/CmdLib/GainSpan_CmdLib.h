/**
 *  @file   GainSpan_CmdLib.h
 *  @brief  uEZ GainSpan command library
 *
 *  uEZ GainSpan command library
 */
#ifndef GAINSPAN_CMDLIB_H_
#define GAINSPAN_CMDLIB_H_

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
#include <uEZ.h>
#include <uEZGPIO.h>
#include "AtCmdLib.h"
#include <Types/SPI.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const char *iSPIDeviceName;
    TUInt32 iRate; // Hz rate to talk SPI

    // Pins.
    T_uezGPIOPortPin iSPIChipSelect;
    T_uezGPIOPortPin iDataReadyIO;
    T_uezGPIOPortPin iProgramMode;
    T_uezGPIOPortPin iSPIMode;
    T_uezGPIOPortPin iSRSTn;

    void (*iIncomingDataFunc)(uint8_t aByte);
} T_GainSpan_CmdLib_SPISettings;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError GainSpan_CmdLib_ConfigureForSPI(
    const T_GainSpan_CmdLib_SPISettings *aSettings);
/**
 *	TODO: Update Commands ?
 */
void GainSpan_CmdLib_Update(void);

#endif // GAINSPAN_CMDLIB_H_
/*-------------------------------------------------------------------------*
 * End of File:  GainSpan_CmdLib.h
 *-------------------------------------------------------------------------*/
