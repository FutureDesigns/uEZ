/**********************************************************************
* $Id$		system_LPC407x_8x_177x_8x.h			2012-01-16
*//**
* @file		system_LPC407x_8x_177x_8x.h
* @brief	CMSIS Cortex-M3, M4 Device Peripheral Access Layer Source File
*          	for the NXP LPC407x_8x_177x_8x Device Series
*
*			ARM Limited (ARM) is supplying this software for use with
*			Cortex-M processor based microcontrollers.  This file can be
*			freely distributed within development tools that are supporting
*			such ARM based processors.
*
* @version	1.2
* @date		20. June. 2012
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2012, NXP Semiconductor
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
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/
#ifndef __SYSTEM_LPC407x_8x_177x_8x_H
#define __SYSTEM_LPC407x_8x_177x_8x_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Do special initialization such as Cortex M4 FPU init.
 */
extern void Lpc17xx40xxSystemInit (void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_LPC407x_8x_177x_8x_H */
