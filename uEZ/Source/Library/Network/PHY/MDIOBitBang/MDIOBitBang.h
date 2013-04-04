/*-------------------------------------------------------------------------*
 * File:  MDIOBitBang.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef MDIOBITBANG_H_
#define MDIOBITBANG_H_

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
#include <uEZGPIO.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezGPIOPortPin iPinMDC;
    T_uezGPIOPortPin iPinMDIO;
    TBool iIsClause45;
    TUInt32 iDelayUSPerHalfBit;
} T_mdioBitBangSettings;


/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
TUInt16 MDIOBitBangRead(
    const T_mdioBitBangSettings *aWorkspace,
    TUInt16 aPHY,
    TUInt32 aReg);
void MDIOBitBangWrite(
    const T_mdioBitBangSettings *aWorkspace,
    TUInt16 aPHY,
    TUInt32 aReg,
    TUInt16 aValue);

#endif // MDIOBITBANG_H_
/*-------------------------------------------------------------------------*
 * End of File:  MDIOBitBang.h
 *-------------------------------------------------------------------------*/
