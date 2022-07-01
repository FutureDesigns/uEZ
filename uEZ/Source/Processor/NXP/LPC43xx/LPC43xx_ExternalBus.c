/*-------------------------------------------------------------------------*
 * File:  LPC43xx_Bus.c
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
#include "LPC43xx_ExternalBus.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void scu_pinmux(TUInt8 port, TUInt8 pin, TUInt8 mode, TUInt8 func)
{
  TUInt32 * scu_base=(TUInt32*)(LPC_SCU_BASE);
  scu_base[(PORT_OFFSET*port+PIN_OFFSET*pin)/4]=mode+func;
} /* scu_pinmux */

void LPC43xx_ExternalAddressBus_ConfigureIOPins(TUInt8 aBitSize)
{
    LPC_SCU->SFSP2_9 = EMC_PIN_SET | 3;    /* P2_9:  A0 */
    LPC_SCU->SFSP2_10 = EMC_PIN_SET | 3;   /* P2_10: A1 */
    LPC_SCU->SFSP2_11 = EMC_PIN_SET | 3;   /* P2_11: A2 */
    LPC_SCU->SFSP2_12 = EMC_PIN_SET | 3;   /* P2_12: A3 */
    LPC_SCU->SFSP2_13 = EMC_PIN_SET | 3;   /* P2_13: A4 */
    LPC_SCU->SFSP1_0 = EMC_PIN_SET | 2;    /* P1_0:  A5 */
    LPC_SCU->SFSP1_1 = EMC_PIN_SET | 2;    /* P1_1:  A6 */
    LPC_SCU->SFSP1_2 = EMC_PIN_SET | 2;    /* P1_2:  A7 */
    LPC_SCU->SFSP2_8 = EMC_PIN_SET | 3;    /* P2_8:  A8 */
    LPC_SCU->SFSP2_7 = EMC_PIN_SET | 3;    /* P2_7:  A9 */
    LPC_SCU->SFSP2_6 = EMC_PIN_SET | 2;    /* P2_6:  A10*/
    LPC_SCU->SFSP2_2 = EMC_PIN_SET | 2;    /* P2_2:  A11*/
    LPC_SCU->SFSP2_1 = EMC_PIN_SET | 2;    /* P2_1:  A12*/
    LPC_SCU->SFSP2_0 = EMC_PIN_SET | 2;    /* P2_0:  A13*/
    LPC_SCU->SFSP6_8 = EMC_PIN_SET | 1;    /* P6_8:  A14*/

//    LPC_SCU->SFSP6_7 = EMC_PIN_SET | 1;    /* P6_7:  A15*/
//    LPC_SCU->SFSPD_16 = EMC_PIN_SET | 2;    /* PD_16: A16*/
//    LPC_SCU->SFSPD_15 = EMC_PIN_SET | 2;    /* PD_15:  A17*/
//    LPC_SCU->SFSPE_0 = EMC_PIN_SET | 3;    /* PE_0:  A18*/
//    LPC_SCU->SFSPE_1 = EMC_PIN_SET | 3;    /* PE_1:  A19*/
//    LPC_SCU->SFSPE_2 = EMC_PIN_SET | 3;    /* PE_2:  A20*/
//    LPC_SCU->SFSPE_3 = EMC_PIN_SET | 3;    /* PE_3:  A21*/
//    LPC_SCU->SFSPE_4 = EMC_PIN_SET | 3;    /* PE_4:  A22*/
//    LPC_SCU->SFSPA_4 = EMC_PIN_SET | 3;    /* PA_4:  A23*/
}

void LPC43xx_ExternalDataBus_ConfigureIOPins(TUInt8 aBitSize)
{
    /* DATA LINES 0..31 > D0..D31 */
    LPC_SCU->SFSP1_7 = EMC_PIN_SET | 3;    /* P1_7:  D0 */
    LPC_SCU->SFSP1_8 = EMC_PIN_SET | 3;    /* P1_8:  D1 */
    LPC_SCU->SFSP1_9 = EMC_PIN_SET | 3;    /* P1_9:  D2 */
    LPC_SCU->SFSP1_10 = EMC_PIN_SET | 3;   /* P1_10: D3 */
    LPC_SCU->SFSP1_11 = EMC_PIN_SET | 3;   /* P1_11: D4 */
    LPC_SCU->SFSP1_12 = EMC_PIN_SET | 3;   /* P1_12: D5 */
    LPC_SCU->SFSP1_13 = EMC_PIN_SET | 3;   /* P1_13: D6 */
    LPC_SCU->SFSP1_14 = EMC_PIN_SET | 3;   /* P1_14: D7 */
    LPC_SCU->SFSP5_4 = EMC_PIN_SET | 2;    /* P5_4:  D8 */
    LPC_SCU->SFSP5_5 = EMC_PIN_SET | 2;    /* P5_5:  D9 */
    LPC_SCU->SFSP5_6 = EMC_PIN_SET | 2;    /* P5_6:  D10*/
    LPC_SCU->SFSP5_7 = EMC_PIN_SET | 2;    /* P5_7:  D11*/
    LPC_SCU->SFSP5_0 = EMC_PIN_SET | 2;    /* P5_0:  D12*/
    LPC_SCU->SFSP5_1 = EMC_PIN_SET | 2;    /* P5_1:  D13*/
    LPC_SCU->SFSP5_2 = EMC_PIN_SET | 2;    /* P5_2:  D14*/
    LPC_SCU->SFSP5_3 = EMC_PIN_SET | 2;    /* P5_3:  D15*/
    LPC_SCU->SFSPD_2 = EMC_PIN_SET | 2;    /* PD_2:  D16*/
    LPC_SCU->SFSPD_3 = EMC_PIN_SET | 2;    /* PD_3:  D17*/
    LPC_SCU->SFSPD_4 = EMC_PIN_SET | 2;    /* PD_4:  D18*/
    LPC_SCU->SFSPD_5 = EMC_PIN_SET | 2;    /* PD_5:  D19*/
    LPC_SCU->SFSPD_6 = EMC_PIN_SET | 2;    /* PD_6:  D20*/
    LPC_SCU->SFSPD_7 = EMC_PIN_SET | 2;    /* PD_7:  D21*/
    LPC_SCU->SFSPD_8 = EMC_PIN_SET | 2;    /* PD_8:  D22*/
    LPC_SCU->SFSPD_9 = EMC_PIN_SET | 2;    /* PD_9:  D23*/
    LPC_SCU->SFSPE_5 = EMC_PIN_SET | 3;    /* PE_5:  D24*/
    LPC_SCU->SFSPE_6 = EMC_PIN_SET | 3;    /* PE_6:  D25*/
    LPC_SCU->SFSPE_7 = EMC_PIN_SET | 3;    /* PE_7:  D26*/
    LPC_SCU->SFSPE_8 = EMC_PIN_SET | 3;    /* PE_8:  D27*/
    LPC_SCU->SFSPE_9 = EMC_PIN_SET | 3;    /* PE_9:  D28*/
    LPC_SCU->SFSPE_10 = EMC_PIN_SET | 3;   /* PE_10: D29*/
    LPC_SCU->SFSPE_11 = EMC_PIN_SET | 3;   /* PE_11: D30*/
    LPC_SCU->SFSPE_12 = EMC_PIN_SET | 3;   /* PE_12: D31*/
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_Bus.c
 *-------------------------------------------------------------------------*/
