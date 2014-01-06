/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_MCI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC17xx_40xx_MCI_H_
#define LPC17xx_40xx_MCI_H_

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
#include <uEZTypes.h>
#include <uEZGPIO.h>
#include <HAL/MCI.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezGPIOPortPin iDAT0;
    T_uezGPIOPortPin iDAT1;
    T_uezGPIOPortPin iDAT2;
    T_uezGPIOPortPin iDAT3;
    T_uezGPIOPortPin iCLK;
    T_uezGPIOPortPin iCMD;
    T_uezGPIOPortPin iCardDetect;
    T_uezGPIOPortPin iWriteProtect;
} T_LPC17xx_40xx_MCI_Pins;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC17xx_40xx_MCI_Require(const T_LPC17xx_40xx_MCI_Pins *aPins, const char *aGPDMA);
extern const HAL_MCI LPC17xx_40xx_MCI_Interface;

#endif // LPC17xx_40xx_MCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_MCI.h
 *-------------------------------------------------------------------------*/
