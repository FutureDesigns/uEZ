/*-------------------------------------------------------------------------*
 * File:  Config_Build.h
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

#define COMPILER_TYPE       RenesasRX
#define UEZ_PROCESSOR       RENESAS_RX63N
#define RTOS                FreeRTOS

// Use up 48K of the heap
#define configTOTAL_HEAP_SIZE        ( ( size_t ) (( 50 * 1024 ) - 64))
#define configMINIMAL_STACK_SIZE    ( 140 )

#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000
#define EXTAL_FREQUENCY        12000000
#define ICLK_MUL            (8)
#define PCLK_MUL            (4)
#define BCLK_MUL            (4)

#define ICLK_FREQUENCY    EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY    EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY    EXTAL_FREQUENCY * BCLK_MUL

#define UEZ_ENABLE_WATCHDOG       0 // Turn on watchdog for testing

#define UEZ_USE_EXTERNAL_EEPROM   0

#define COLOR_T int
