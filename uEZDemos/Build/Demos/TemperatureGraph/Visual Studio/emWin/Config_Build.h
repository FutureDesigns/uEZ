#include <uEZTypes.h>

#define UEZ_LCD_DISPLAY_WIDTH				800
#define UEZ_LCD_DISPLAY_HEIGHT				480
#define DISPLAY_WIDTH						UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT						UEZ_LCD_DISPLAY_HEIGHT
#define BUILD_NUMBER                        1
#define BUILD_DATE                          "1/20/2013"

#define COMPILER_TYPE                       VisualC
#define UEZ_PROCESSOR                       NXP_LPC1788

#define RTOS                                FreeRTOS

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM070RBHG04

#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

typedef int SWIM_WINDOW_T;
typedef int T_pixelColor;
typedef int T_choice;

#define GUI_NUM_VIRTUAL_DISPLAYS	2
#define GUI_PIXEL_WIDTH				2
#define LCD_FRAME_BUFFER			G_frameBuffer
#define GUI_VRAM_SIZE				0xA0000000
#define GUI_VRAM_BASE_ADDR			0xA0000000

unsigned long int *GUI_pMem;
int GUI_MemSize;

#define UEZFailureMsg(n)

extern TUInt8 G_frameBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT * GUI_PIXEL_WIDTH * GUI_NUM_VIRTUAL_DISPLAYS];

typedef int T_uezSempaphore;
