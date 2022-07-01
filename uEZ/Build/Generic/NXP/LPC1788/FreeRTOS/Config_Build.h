#define UEZ_LIBRARY                 1
#define UEZ_PROCESSOR               NXP_LPC1788
// uEZ Compiler define
#ifdef __IAR_SYSTEMS_ICC__
#define COMPILER_TYPE               IAR
#else
#ifdef __GNUC__
#define COMPILER_TYPE               RowleyARM
#endif
#endif
// end uEZ Compiler define

#define FATFS_MAX_MASS_STORAGE_DEVICES      2

#define UEZ_LCD_COLOR_DEPTH         UEZLCD_COLOR_DEPTH_I15_BIT

#define UEZ_LCD_DISPLAY_HEIGHT      UEZPlatform_LCDGetHeight()
#define UEZ_LCD_DISPLAY_WIDTH       UEZPlatform_LCDGetWidth()
#define FRAME(x)                    UEZPlatform_LCDGetFrame(x)
#define LCD_FRAME_BUFFER            UEZPlatform_LCDGetFrameBuffer()
#define FRAME_SIZE                  UEZPlatform_LCDGetFrameSize()
#define GUI_COLOR_CONVERSION        UEZPlatform_GUIColorConversion()
#define GUI_DISPLAY_DRIVER          UEZPlatform_GUIDisplayDriver()
#define LCD_DISPLAY_BASE_ADDRESS    UEZPlatform_GetBaseAddress()

#define SERIAL_PORTS_DEFAULT_BAUD   UEZPlatform_SerialGetDefaultBaud()

#define PROCESSOR_OSCILLATOR_FREQUENCY  UEZPlatform_ProcessorGetFrequency()
#define PCLK_FREQUENCY              UEZPlatform_GetPCLKFrequency()

#define INTERRUPT_BASED_EMAC        1

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY  1
#define configUSE_TIMERS          1
#define USE_PRIMASK_CS            1
#define SELECTED_PORT PORT_ARM_CortexM
#endif

#define SEGGER_ENABLE_RTT                   0
#define SEGGER_ENABLE_SYSTEM_VIEW           0

#ifdef NDEBUG
#define UEZ_REGISTER              0
#else
#define UEZ_REGISTER              1  //Used for registering Queues and Semaphores in the RTOS
#endif