#include <uEZTypes.h>

#define UEZ_LCD_DISPLAY_WIDTH				800
#define UEZ_LCD_DISPLAY_HEIGHT				480
#define DISPLAY_WIDTH						UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT						UEZ_LCD_DISPLAY_HEIGHT
#define BUILD_NUMBER                        2
#define BUILD_DATE                          "10/4/2012"

#define COMPILER_TYPE                       VisualC
#define UEZ_PROCESSOR                       NXP_LPC1788

#define RTOS                                FreeRTOS

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM070RBHG04

#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

typedef int SWIM_WINDOW_T;
typedef int T_pixelColor;
typedef int T_choice;
