/*
 * @brief Chip inclusion selector file
 *
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __CHIP_H_
#define __CHIP_H_

#include "CMSIS/LPC43xx.h"

#include <uEZ.h>

#ifndef STATIC
#define STATIC static
#endif

#ifndef INLINE
#define INLINE inline
#endif

#ifndef bool
#define bool TBool
#endif

#ifndef true
#define true ETrue
#endif

#ifndef false
#define false EFalse
#endif

#include "enet_18xx_43xx.h"
#include "rgu_18xx_43xx.h"
#include "cguccu_18xx_43xx.h"
#include "clock_18xx_43xx.h"
#include "sdif_18xx_43xx.h"
#include "sdmmc_18xx_43xx.h"
#include "usbhs_18xx_43xx.h"
#include "creg_18xx_43xx.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
}
#endif

#endif /* __CHIP_H_ */
