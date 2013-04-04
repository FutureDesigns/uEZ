/*-------------------------------------------------------------------------*
 * File:  uEZPlatform.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Contains definitions for the uEZGUI_RX62N_35QT platform.
 *-------------------------------------------------------------------------*/
#ifndef UEZPLATFORM_H_
#define UEZPLATFORM_H_

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
#include <UEZNetwork.h>

/*-------------------------------------------------------------------------*
 * Platform Settings:
 *-------------------------------------------------------------------------*/
#define UEZ_PROCESSOR               RENESAS_RX63N
#define RX62N_PACKAGE               RX63N_PACKAGE_LQFP100

#ifndef UEZ_NUM_HANDLES
#define UEZ_NUM_HANDLES           150 // more than the average number of handles
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000   // clocks per second (when using LCD, must be slower)
#endif

#ifndef SDRAM_CLOCK_FREQUENCY
    #define SDRAM_CLOCK_FREQUENCY               (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef EMC_CLOCK_FREQUENCY
    #define EMC_CLOCK_FREQUENCY                 (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef UEZ_CONSOLE_READ_BUFFER_SIZE
    #define UEZ_CONSOLE_READ_BUFFER_SIZE        128 // bytes
#endif
#ifndef UEZ_CONSOLE_WRITE_BUFFER_SIZE
    #define UEZ_CONSOLE_WRITE_BUFFER_SIZE       128 // bytes
#endif

#ifndef FATFS_MAX_MASS_STORAGE_DEVICES
    #define FATFS_MAX_MASS_STORAGE_DEVICES      2
#endif

#include "lwipopts.h"

#if (COMPILER_TYPE==IAR)
    #define NOP() __no_operation()
#endif

#ifndef configKERNEL_INTERRUPT_PRIORITY
#define configKERNEL_INTERRUPT_PRIORITY     4
#endif

#ifndef LCD_H
#define LCD_H       64
#endif

#ifndef LCD_W
#define LCD_W       96
#endif

#define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
#define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
#define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180000

#define DISPLAY_WIDTH       96
#define DISPLAY_HEIGHT      64

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

void UEZPlatform_Standard_Require(void);
void UEZPlatform_Full_Require(void);
void UEZPlatform_Minimal_Require(void);

#endif // UEZPLATFORM_SETTINGS_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform_Settings.h
 *-------------------------------------------------------------------------*/
