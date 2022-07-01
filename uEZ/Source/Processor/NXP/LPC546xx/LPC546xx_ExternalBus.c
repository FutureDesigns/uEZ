/*-------------------------------------------------------------------------*
 * File:  LPC546xx_Bus.c
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
 *    |    We can get you up and running fast!      |
 *    |      See http://www.teamfdi.com/uez for more details.|
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include "LPC546xx_ExternalBus.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define EMC_IOCON       ((0x01 << 0x0A) | (0x01 << 0x09) | (0x01 << 0x08) | (0x06))

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC546xx_ExternalAddressBus_ConfigureIOPins()
{
    //EMC_A[0] to A[14]
    IOCON->PIO[0][18] = EMC_IOCON;  //A0
    IOCON->PIO[0][19] = EMC_IOCON;  //A1
    IOCON->PIO[0][20] = EMC_IOCON;  //A2
    IOCON->PIO[0][21] = EMC_IOCON;  //A3
    IOCON->PIO[1][5]  = EMC_IOCON;  //A4
    IOCON->PIO[1][6]  = EMC_IOCON;  //A5
    IOCON->PIO[1][7]  = EMC_IOCON;  //A6
    IOCON->PIO[1][8]  = EMC_IOCON;  //A7
    IOCON->PIO[1][26] = EMC_IOCON;  //A8
    IOCON->PIO[1][27] = EMC_IOCON;  //A9
    IOCON->PIO[1][16] = EMC_IOCON;  //A10
    IOCON->PIO[1][23] = EMC_IOCON;  //A11
    IOCON->PIO[1][24] = EMC_IOCON;  //A12
    IOCON->PIO[1][25] = EMC_IOCON;  //A13
    IOCON->PIO[3][25] = EMC_IOCON;  //A14
    //IOCON->PIO[3][26] = EMC_IOCON;  //A15
}

void LPC546xx_ExternalDataBus_ConfigureIOPins()
{
    //EMC_D[0] to D[14]
    IOCON->PIO[0][2]  = EMC_IOCON;  //D0
    IOCON->PIO[0][3]  = EMC_IOCON;  //D1
    IOCON->PIO[0][4]  = EMC_IOCON;  //D2
    IOCON->PIO[0][5]  = EMC_IOCON;  //D3
    IOCON->PIO[0][6]  = EMC_IOCON;  //D4
    IOCON->PIO[0][7]  = EMC_IOCON;  //D5
    IOCON->PIO[0][8]  = EMC_IOCON;  //D6
    IOCON->PIO[0][9]  = EMC_IOCON;  //D7
    IOCON->PIO[1][19] = EMC_IOCON;  //D8
    IOCON->PIO[1][20] = EMC_IOCON;  //D9
    IOCON->PIO[1][21] = EMC_IOCON;  //D10
    IOCON->PIO[1][4]  = EMC_IOCON;  //D11
    IOCON->PIO[1][28] = EMC_IOCON;  //D12
    IOCON->PIO[1][29] = EMC_IOCON;  //D13
    IOCON->PIO[1][30] = EMC_IOCON;  //D14
    IOCON->PIO[1][31] = EMC_IOCON;  //D15
}

void LPC546xx_ExternalControl_ConfigureIOPins()
{
    IOCON->PIO[1][9]   = EMC_IOCON;//EMC_CASN
    IOCON->PIO[1][10]  = EMC_IOCON;//EMC_RASN
    IOCON->PIO[1][11]  = EMC_IOCON;//EMC_CLK[0]
    IOCON->PIO[1][12]  = EMC_IOCON;//EMC_DYCSN[0]
    IOCON->PIO[1][13]  = EMC_IOCON;//EMC_DQM[0]
    IOCON->PIO[1][14]  = EMC_IOCON;//EMC_DQM[1]
    IOCON->PIO[1][15]  = EMC_IOCON;//EMC_CKE[0]
    IOCON->PIO[0][15]  = EMC_IOCON;//EMC_WEN
}


/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_Bus.c
 *-------------------------------------------------------------------------*/
