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

#ifdef NDEBUG
#define UEZ_REGISTER              0

#else
#define UEZ_REGISTER              1  //Used for registering Queues and Semaphores in the RTOS

#endif

#define DEBUG_HTTP_SERVER           	      0

#define NETWORK_LWIP_MAX_NUM_SOCKETS          16

// Set this variable in a release bootloader size-optimized build.
//#define DISABLE_FILESYSTEM_FORMAT_SUPPORT   1 // This will make bootloaders smaller

// RTOS Heap selection defines:

// FreeRTOS 10 Heap Notes:
//  heap_1 is less useful since FreeRTOS added support for static allocation.
//  heap_2 is now considered legacy as the newer heap_4 implementation is preferred.
//  Use heap_5 for dual heap.
#ifndef FREERTOS_HEAP_SELECTION // Make sure the number 1-5 is set here.
#ifdef DISABLE_FEATURES_FOR_BOOTLOADER
#define FREERTOS_HEAP_SELECTION         2 // default Heap for bootloader, less code space than 4.
#else
#define FREERTOS_HEAP_SELECTION         3 // default Heap for all other projects
// TODO switch default demo projects to use heap 4
#endif
#endif

#if ((FREERTOS_HEAP_SELECTION==1) |(FREERTOS_HEAP_SELECTION==2) | (FREERTOS_HEAP_SELECTION==4))
// These heaps will use ucHeap[configTOTAL_HEAP_SIZE] global variable;
// If configAPPLICATION_ALLOCATED_HEAP is 1 make sure to declare ucHeap in application project.
// You should #undef configTOTAL_HEAP_SIZE in application then redfine it to change the size per app.
#define configAPPLICATION_ALLOCATED_HEAP    1
#endif

#if (FREERTOS_HEAP_SELECTION==3)
// This heap uses the application level heap sized defined in the IDE project settings.
// For IAR set __ICFEDIT_size_heap__ in ICF file.
// For Crossworks/Segger Embedded set arm_linker_heap_size in project file.
#endif

#if (FREERTOS_HEAP_SELECTION==5)
// TODO see https://www.freertos.org/a00111.html#heap_5.
#endif

#define ALLOW_LOCKING_XSPI_OTP 0 // set to 1 to allow sending WRSCUR command for 512/1024 byte hidden region in spifiLockOtp function.

#ifdef DISABLE_FEATURES_FOR_BOOTLOADER
// Placeholder
#endif
