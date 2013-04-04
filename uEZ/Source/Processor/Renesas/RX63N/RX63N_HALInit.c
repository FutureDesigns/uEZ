/*-------------------------------------------------------------------------*
 * File:  HALInit.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the HAL processor specific interfaces.
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
#include <uEZProcessor.h>

void uEZProcessorServicesInit(void)
{
	RX63N_GPIO_P0_Require();
	RX63N_GPIO_P1_Require();
	RX63N_GPIO_P2_Require();
	RX63N_GPIO_P3_Require();
	RX63N_GPIO_P4_Require();
	RX63N_GPIO_P5_Require();
	RX63N_GPIO_P6_Require();
	RX63N_GPIO_P7_Require();
	RX63N_GPIO_P8_Require();
	RX63N_GPIO_P9_Require();
	RX63N_GPIO_PA_Require();
	RX63N_GPIO_PB_Require();
	RX63N_GPIO_PC_Require();
	RX63N_GPIO_PD_Require();
	RX63N_GPIO_PE_Require();
	RX63N_GPIO_PF_Require();
	RX63N_GPIO_PG_Require();
	RX63N_GPIO_PJ_Require();
}

/*-------------------------------------------------------------------------*
 * End of File:  HALInit.c
 *-------------------------------------------------------------------------*/
