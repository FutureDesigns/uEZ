/**********************************************************************
* $Id$		system_lpc43xx.c			2011-06-02
*//**
* @file		system_lpc43xx.c
* @brief	Cortex-M3 Device System Source File for NXP lpc43xx Series.
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#include "lpc43xx.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __IRC            (12000000UL)    /* IRC Oscillator frequency          */

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __IRC;		/*!< System Clock Frequency (Core Clock)*/

extern uint32_t getPC(void);

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
    SystemCoreClock = __IRC;

#if defined(CORE_M4) || defined(CORE_M3)
	// Enable VTOR register to point to vector table
	SCB->VTOR = getPC() & 0xFFF00000;
#endif

    /*Enable Buffer for External Flash*/
    LPC_EMC->STATICCONFIG0 |= 1<<19;
}
