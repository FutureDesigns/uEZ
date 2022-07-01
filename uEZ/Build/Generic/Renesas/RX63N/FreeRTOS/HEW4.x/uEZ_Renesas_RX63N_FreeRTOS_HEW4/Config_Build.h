#define UEZ_LIBRARY                 1
#define UEZ_PROCESSOR               RENESAS_RX63N
#define COMPILER_TYPE               RenesasRX
#define RTOS                		FreeRTOS

#define FATFS_MAX_MASS_STORAGE_DEVICES      2

#define UEZ_LCD_COLOR_DEPTH         UEZLCD_COLOR_DEPTH_16_BIT

#define UEZ_LCD_DISPLAY_HEIGHT      UEZPlatform_LCDGetHeight()
#define UEZ_LCD_DISPLAY_WIDTH       UEZPlatform_LCDGetWidth()
#define FRAME(x)                    UEZPlatform_LCDGetFrame(x)
#define LCD_FRAME_BUFFER            UEZPlatform_LCDGetFrameBuffer()
#define FRAME_SIZE                  UEZPlatform_LCDGetFrameSize()
#define GUI_COLOR_CONVERSION        UEZPlatform_GUIColorConversion()
#define GUI_DISPLAY_DRIVER          UEZPlatform_GUIDisplayDriver()
#define LCD_DISPLAY_BASE_ADDRESS    UEZPlatform_GetBaseAddress()

#define SERIAL_PORTS_DEFAULT_BAUD   UEZPlatform_SerialGetDefaultBaud()

//#define PROCESSOR_OSCILLATOR_FREQUENCY  UEZPlatform_ProcessorGetFrequency()
#define PROCESSOR_OSCILLATOR_FREQUENCY	96000000

#define configKERNEL_INTERRUPT_PRIORITY		1
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    15

#define EXTAL_FREQUENCY		12000000
#define ICLK_MUL        	(8)
#define PCLK_MUL        	(4)
#define BCLK_MUL        	(4)

#define ICLK_FREQUENCY	EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY	EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY	EXTAL_FREQUENCY * BCLK_MUL

#ifndef EMWIN_RAM_BASE_ADDR
#define EMWIN_RAM_BASE_ADDR  0x08080000
#endif
#ifndef EMWIN_RAM_SIZE
#define EMWIN_RAM_SIZE       0x00080000

#define configTICK_VECTOR	 _CMT0_CMI0

// increase from 33 for new 2.07 LWIP, also RX62N build has 48 set
#define configTOTAL_HEAP_SIZE		( ( size_t ) (( 48 * 1024 ) - 64)) 

#endif
