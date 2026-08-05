/**********************************************************************
* $Id$		system_LPC407x_8x_177x_8x.c			2012-01-16
*//**
* @file		system_LPC407x_8x_177x_8x.c
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
**********************************************************************/
#include <stdint.h>
#include "LPC407x_8x_177x_8x.h"
#include "system_LPC407x_8x_177x_8x.h"

#ifdef CORE_M4
#define	LPC_CPACR	    	0xE000ED88

#define SCB_MVFR0           0xE000EF40
#define SCB_MVFR0_RESET     0x10110021

#define SCB_MVFR1           0xE000EF44
#define SCB_MVFR1_RESET     0x11000011
#endif

#ifdef CORE_M4
/**
 * See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0449b/ch02s01s02.html
 * Home > Programmers Model > About the programmers model > Enabling VFP support
 * 2.1.2. Enabling VFP support -> From reset, the VFP extension is disabled. 
 * Any attempt to execute a VFP instruction results in an Undefined Instruction exception 
 * being taken. To enable software to access VFP features ensure that:
 * * Access to CP10 and CP11 is enabled for the appropriate privilege level. 
 * * If Non-secure access to the VFP features is required, the access flags for CP10 and CP11
 *   in the NSACR must be set to 1. See VFP register access.
 * In addition, software must set the FPEXC.EN bit to 1 to enable most VFP operations. 
 * See Floating-Point Exception Register.
 * When VFP operation is disabled because FPEXC.EN is 0, all VFP instructions are treated as
 * undefined instructions except for execution of the following in privileged modes:
 * a VMSR to the FPEXC or FPSID register
 * a VMRS from the FPEXC, FPSID, MVFR0, or MVFR1 registers.
 *
 * Note: Operation is UNPREDICTABLE if you configure the Coprocessor Access Control Register
 * (CPACR) such that CP10 and CP11 do not have identical access permissions.
 *
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.set.cortexm/index.html
 * Home > Cortex-M4 Peripherals > Floating Point Unit (FPU) > Enabling the FPU
 * 4.6.6. Enabling the FPU -> The FPU is disabled from reset. You must enable it before you
 * can use any floating-point instructions. Example 4.1 shows an example code sequence for
 * enabling the FPU in both privileged and user modes. The processor must be in privileged 
 * mode to read from and write to the CPACR.
 */
// From arm trm manual:
//   LDR.W R0, =0xE000ED88		; CPACR is located at address 0xE000ED88
//   LDR R1, [R0]  				; Read CPACR
//   ORR R1, R1, #(0xF << 20) 	; Set bits 20-23 to enable CP10 and CP11 coprocessors
//   STR R1, [R0] 				; Write back the modified value to the CPACR

void fpu_init(void)
{
    volatile uint32_t* regCpacr = (uint32_t*) LPC_CPACR;
    volatile uint32_t* regMvfr0 = (uint32_t*) SCB_MVFR0;
    volatile uint32_t* regMvfr1 = (uint32_t*) SCB_MVFR1;
    volatile uint32_t Cpacr;
    volatile uint32_t Mvfr0;
    volatile uint32_t Mvfr1;
    char vfpPresent = 0;

    Mvfr0 = *regMvfr0;
    Mvfr1 = *regMvfr1;

    vfpPresent = ((SCB_MVFR0_RESET == Mvfr0) && (SCB_MVFR1_RESET == Mvfr1));

    if(vfpPresent)
    {
        Cpacr = *regCpacr;
        Cpacr |= (0xF << 20);
        *regCpacr = Cpacr;   // enable CP10 and CP11 for full access
    }
}
#endif

void Lpc17xx40xxSystemInit (void)
{
#ifdef CORE_M4
    fpu_init();
#endif
}
