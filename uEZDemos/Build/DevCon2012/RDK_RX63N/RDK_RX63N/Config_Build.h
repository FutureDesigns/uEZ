/*-------------------------------------------------------------------------*
 * File:  Config_Build.h
 *-------------------------------------------------------------------------*/
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
#define COMPILER_TYPE       RenesasRX
#define UEZ_PROCESSOR       RENESAS_RX63N
#define RTOS                FreeRTOS

// Use up 48K of the heap
#define configTOTAL_HEAP_SIZE		( ( size_t ) (( 50 * 1024 ) - 64))
#define configMINIMAL_STACK_SIZE	( 140 )

#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000
#define EXTAL_FREQUENCY		12000000
#define ICLK_MUL        	(8)
#define PCLK_MUL        	(4)
#define BCLK_MUL        	(4)

#define ICLK_FREQUENCY	EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY	EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY	EXTAL_FREQUENCY * BCLK_MUL

#define UEZ_ENABLE_WATCHDOG       0 // Turn on watchdog for testing

#define UEZ_USE_EXTERNAL_EEPROM   0

#define COLOR_T int
