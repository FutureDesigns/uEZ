/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.0.2
 * Percepio AB, www.percepio.com
 *
 * trcConfig.h
 *
 * Configuration parameters for the streaming version trace recorder library.
 * Before using the trace recorder library, please check that the default
 * settings are appropriate for your system, and if necessary adjust these.
 *
 * Terms of Use
 * This software (the "Tracealyzer Recorder Library") is the intellectual
 * property of Percepio AB and may not be sold or in other ways commercially
 * redistributed without explicit written permission by Percepio AB.
 *
 * Separate conditions applies for the SEGGER branded source code included.
 *
 * The recorder library is free for use together with Percepio products.
 * You may distribute the recorder library in its original form, but public
 * distribution of modified versions require approval by Percepio AB.
 *
 * Disclaimer
 * The trace tool and recorder library is being delivered to you AS IS and
 * Percepio AB makes no warranty as to its use or performance. Percepio AB does
 * not and cannot warrant the performance or results you may obtain by using the
 * software or documentation. Percepio AB make no warranties, express or
 * implied, as to noninfringement of third party rights, merchantability, or
 * fitness for any particular purpose. In no event will Percepio AB, its
 * technology partners, or distributors be liable to you for any consequential,
 * incidental or special damages, including any lost profits or lost savings,
 * even if a representative of Percepio AB has been advised of the possibility
 * of such damages, or for any claim by any third party. Some jurisdictions do
 * not allow the exclusion or limitation of incidental, consequential or special
 * damages, or the exclusion of implied warranties or limitations on how long an
 * implied warranty may last, so the above limitations may not apply to you.
 *
 * Tabs are used for indent in this file (1 tab = 4 spaces)
 *
 * Copyright Percepio AB, 2015.
 * www.percepio.com
 ******************************************************************************/

#ifndef TRC_CONFIG_H
#define TRC_CONFIG_H

#ifdef __cplusplus
extern “C” {
#endif

#include <Config.h>

/*******************************************************************************
 * NTask, NISR, NQueue, NSemaphore, NMutex
 *
 * A group of Macros which should be defined as an integer value of zero (0) 
 * or larger.
 *
 * This defines the capacity of the Object Property Table - the maximum number
 * of objects active at any given point within each object class.
 * 
 * NOTE: In case objects are deleted and created during runtime, this setting
 * does not limit the total amount of objects, only the number of concurrently
 * active objects. 
 *
 * Using too small values will give an error message through the vTraceError
 * routine, which makes the error message appear when opening the trace data
 * in Tracealyzer. If you are using the recorder status monitor task,
 * any error messages are displayed in console prints, assuming that the
 * print macro has been defined properly (vConsolePrintMessage). 
 *
 * It can be wise to start with very large values for these constants, 
 * unless you are very confident on these numbers. Then do a recording and
 * check the actual usage in Tracealyzer. This is shown by selecting
 * View -> Trace Details -> Resource Usage -> Object Table
 * 
 * NOTE 2: Remember to account for all tasks and other objects created by 
 * the kernel, such as the IDLE task, any timer tasks, and any tasks created 
 * by other 3rd party software components, such as communication stacks.
 * Moreover, one task slot is used to indicate "(startup)", i.e., a fictive 
 * task that represent the time before the scheduler starts. 
 * NTask should thus be at least 2-3 slots larger than your application task count.
 *
 ******************************************************************************/
#define NTask             100
#define NISR              255
#define NQueue            255
#define NSemaphore        255
#define NMutex            60
#define NTimer            2
#define NEventGroup       2

/* Maximum object name length for each class (includes zero termination) */
#define NameLenTask       configMAX_TASK_NAME_LEN
#define NameLenISR        10
#define NameLenQueue      15
#define NameLenSemaphore  15
#define NameLenMutex      15
#define NameLenTimer      15
#define NameLenEventGroup 15

/*******************************************************************************
 * EVENT_BUFFER_SIZE
 *
 * Macro which should be defined as an integer value.
 *
 * This defines the capacity of the event buffer, i.e., the number of records
 * it may store. Each registered event typically use one record (4 byte), but
 * vTracePrintF may use multiple records depending on the number of data args.
 ******************************************************************************/
#define EVENT_BUFFER_SIZE 4000 /* Adjust wrt. to available RAM */

/*******************************************************************************
 * SYMBOL_TABLE_SIZE
 *
 * Macro which should be defined as an integer value.
 *
 * This defines the capacity of the symbol table, in bytes. This symbol table 
 * stores User Events labels and names of deleted tasks, queues, or other kernel
 * objects. Note that the names of active objects not stored here but in the 
 * Object Table. Thus, if you don't use User Events or delete any kernel 
 * objects you set this to a very low value, e.g. 4, but not zero (0) since 
 * this causes a declaration of a zero-sized array, for which the C compiler
 * behavior is not standardized and may cause misaligned data.
 ******************************************************************************/
#define SYMBOL_TABLE_SIZE 800

#if (SYMBOL_TABLE_SIZE == 0)
#error "SYMBOL_TABLE_SIZE may not be zero!"
#endif
  
/*******************************************************************************
 * Configuration Macro: TRC_RECORDER_HARDWARE_PORT
 *
 * Specify what hardware is used.
 *
 * See trcHardwarePort.h for available ports, or to define your own.
 ******************************************************************************/ 
/****** Port Name ******************** Code ** Official ** OS Platform *********
* PORT_APPLICATION_DEFINED               -2     -           -                 
* PORT_NOT_SET                           -1     -           -                 
* PORT_HWIndependent                     0      Yes         Any               
* PORT_Win32                             1      Yes         FreeRTOS Win32
* PORT_Atmel_AT91SAM7                    2      No          Any               
* PORT_Atmel_UC3A0                       3      No          Any               
* PORT_ARM_CortexM                       4      Yes         Any               
* PORT_Renesas_RX600                     5      Yes         Any               
* PORT_Microchip_dsPIC_AND_PIC24         6      Yes         Any               
* PORT_TEXAS_INSTRUMENTS_TMS570          7      No          Any               
* PORT_TEXAS_INSTRUMENTS_MSP430          8      No          Any               
* PORT_MICROCHIP_PIC32                   9      No          Any               
* PORT_XILINX_PPC405                     10     No          FreeRTOS          
* PORT_XILINX_PPC440                     11     No          FreeRTOS          
* PORT_XILINX_MICROBLAZE                 12     No          Any               
* PORT_NXP_LPC210X                       13     No          Any               
*******************************************************************************/
#define TRC_RECORDER_HARDWARE_PORT TRC_PORT_ARM_Cortex_M

/******************************************************************************
 * BSP and other project specific includes
 * 
 * Include the necessary header files.
 *****************************************************************************/
//#include "board.h"

/******************************************************************************
 * TRC_FREERTOS_VERSION
 * 
 * Specify what version of FreeRTOS that is used. This is necessary compensate 
 * for renamed symbols in the FreeRTOS kernel (does not build if incorrect).
 * 
 * TRC_FREERTOS_VERSION_7_3_OR_7_4 (= 1)		If using FreeRTOS v7.3.0 - v7.4.2
 * TRC_FREERTOS_VERSION_7_5_OR_7_6 (= 2)		If using FreeRTOS v7.5.0 - v7.6.0
 * TRC_FREERTOS_VERSION_8_0_OR_LATER (= 3)		If using FreeRTOS v8.0.0 or later
 *****************************************************************************/
#define TRC_FREERTOS_VERSION_NOT_SET			0
#define TRC_FREERTOS_VERSION_7_3_OR_7_4			1
#define TRC_FREERTOS_VERSION_7_5_OR_7_6			2
#define TRC_FREERTOS_VERSION_8_0_OR_LATER		3

#define TRC_FREERTOS_VERSION	TRC_FREERTOS_VERSION_8_0_OR_LATER

/******************************************************************************
 * INCLUDE_OBJECT_DELETE
 * 
 * Macro which should be defined as either zero (0) or one (1). 
 * Default is 1.
 *
 * This must be enabled (1) if tasks, queues or other 
 * traced kernel objects are deleted at runtime. If no deletes are made, this 
 * can be set to 0 in order to exclude the delete-handling code.
 *****************************************************************************/
#define INCLUDE_OBJECT_DELETE 1

/*******************************************************************************
 * Configuration Macro: TRC_SYMBOL_TABLE_SLOTS
 *
 * The maximum number of symbols names that can be stored. This includes:
 * - Task names
 * - Named ISRs (vTraceSetISRProperties)
 * - Named kernel objects (vTraceStoreKernelObjectName)
 * - User event channels (vTraceStoreUserEventChannelName)
 *
 * If this value is too small, not all symbol names will be stored and the
 * trace display will be affected. In that case, there will be warnings
 * (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_SYMBOL_TABLE_SLOTS 30

/*******************************************************************************
 * Configuration Macro: TRC_SYMBOL_MAX_LENGTH
 *
 * The maximum length of symbol names, including:
 * - Task names
 * - Named ISRs (vTraceSetISRProperties)
 * - Named kernel objects (vTraceStoreKernelObjectName)
 * - User event channel names (vTraceStoreUserEventChannelName)
 *
 * If longer symbol names are used, they will be truncated by the recorder,
 * which will affect the trace display. In that case, there will be warnings
 * (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_SYMBOL_MAX_LENGTH 25

/*******************************************************************************
 * Configuration Macro: TRC_OBJECT_DATA_SLOTS
 *
 * The maximum number of object data entries (used for task priorities) that can
 * be stored at the same time. Must be sufficient for all tasks, otherwise there
 * will be warnings (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_OBJECT_DATA_SLOTS 20

/*******************************************************************************
 * Configuration Macro: TRC_RTT_UP_BUFFER_INDEX
 *
 * Defines the RTT buffer to use for writing the trace data. Make sure that
 * the PC application has the same setting (File->Settings).
 *
 ******************************************************************************/
#define TRC_RTT_UP_BUFFER_INDEX 0

/*******************************************************************************
 * Configuration Macro: TRC_RTT_DOWN_BUFFER_INDEX
 *
 * Defines the RTT buffer to use for reading the trace data. Make sure that
 * the PC application has the same setting (File->Settings).
 *
 ******************************************************************************/
#define TRC_RTT_DOWN_BUFFER_INDEX 0

/*******************************************************************************
 * Configuration Macro: TRC_CTRL_TASK_STACK_SIZE
 *
 * The stack size of the TzCtrl task, that receive commands.
 * We are aiming to remove this extra task in future versions.
 ******************************************************************************/
#define TRC_CTRL_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

/*******************************************************************************
 * Configuration Macro: TRC_CTRL_TASK_PRIORITY
 *
 * The priority of the TzCtrl task, that receive commands from.
 * We are aiming to remove this extra task in future versions.
 ******************************************************************************/
#define TRC_CTRL_TASK_PRIORITY 1

/*******************************************************************************
 * Configuration Macro: TRC_CTRL_TASK_DELAY
 *
 * The delay between every loop of the TzCtrl task. A high delay will reduce the
 * CPU load, but may cause missed events if the TzCtrl task is performing the 
 * trace transfer.
 ******************************************************************************/
#define TRC_CTRL_TASK_DELAY ((10 * configTICK_RATE_HZ) / 1000)

/*******************************************************************************
 * Configuration Macro: TRC_MEASURE_BLOCKING_TIME
 *
 * If using SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL, this activates detection and
 * warnings in case of blocking in SEGGER_RTT_Write (if too small buffer).
 *
 * If enabling this option (set to 1) warnings are given as User Events if
 * blocking occurs, including the longest blocking time. These warnings are
 * given on the channel "Blocking on trace buffer".
 *
 * Note: If you get such warnings, you can study the blocking time in the User
 * Event Signal Plot to get an overview of the magnitude of the blocking and
 * decide if acceptable.
 *
 * To eliminate or at least reduce occurrences of blocking:
 *
 * - Verify the J-Link Speed in the Settings dialog of the PC application.
 *   Default is 4 MHz, but can be increased a lot depending on your J-Link.
 *
 *   Note: If you set the speed higher than your J-Link can manage, the J-Link
 *   driver will instead use the fastest supported speed. The actual speed used
 *   is shown in the title of the trace receiver window.
 *
 * - Increase the buffer size (BUFFER_SIZE_UP in SEGGER_RTT_Conf.h).
 *
 * - Reduce the amount of data produced, e.g., by removing frequent User Events.
 ******************************************************************************/
#define TRC_MEASURE_BLOCKING_TIME 0

/*******************************************************************************
 * Configuration Macro: TRC_BLOCKING_MIN_CYCLES
 *
 * Threshold value for deciding if SEGGER_RTT_Write has blocked. Most events
 * take 200-300 cycles on ARM Cortex-M MCUs, so anything above 500 cycles should
 * be due to blocking on a full buffer (i.e., waiting for the debugger to read
 * the RTT buffer data and make room for more...).
 ******************************************************************************/
#define TRC_BLOCKING_MIN_CYCLES 500

/*******************************************************************************
 * Configuration Macro: TRC_RECORDER_BUFFER_ALLOCATION
 *
 * Specifies how the recorder buffer is allocated.
 *
 * Values:
 * TRC_RECORDER_BUFFER_ALLOCATION_STATIC
 * TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC
 ******************************************************************************/
#define TRC_RECORDER_BUFFER_ALLOCATION TRC_RECORDER_BUFFER_ALLOCATION_STATIC

/******************************************************************************
 * TRACE_DATA_ALLOCATION
 *
 * This defines how to allocate the recorder data structure, i.e., using a 
 * static declaration or using a dynamic allocation in runtime (malloc).
 *
 * Should be one of these two options:
 * - TRACE_DATA_ALLOCATION_STATIC (default)
 * - TRACE_DATA_ALLOCATION_DYNAMIC
 *
 * Using static allocation has the benefits of compile-time errors if the buffer 
 * is too large (too large constants in trcConfig.h) and no need to call the 
 * initialization routine (xTraceInitTraceData).
 *
 * Using dynamic allocation may give more flexibility in some cases.
 *****************************************************************************/
#define TRACE_DATA_ALLOCATION TRACE_DATA_ALLOCATION_STATIC
  
/*******************************************************************************
 * Configuration Macro: TRC_RECORDER_TRANSFER_METHOD
 *
 * Specifies what type of transfer method is used.
 *
 * Values:
 * TRC_RECORDER_TRANSFER_METHOD_JLINK_RTT_BLOCK
 * TRC_RECORDER_TRANSFER_METHOD_JLINK_RTT_NOBLOCK
 * TRC_RECORDER_TRANSFER_METHOD_TCPIP
 * TRC_RECORDER_TRANSFER_METHOD_CUSTOM
 ******************************************************************************/
#define TRC_RECORDER_TRANSFER_METHOD TRC_RECORDER_TRANSFER_METHOD_JLINK_RTT_BLOCK

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_BLOCKING_TRANSFER
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Specifies how the custom transfer method handles full buffers.
 *
 * Values:
 * 0 - The custom transfer method skips sending the data if the buffer is full.
 * 1 - The custom transfer method blocks on send if the buffer is full.
 ******************************************************************************/
 #define TRC_STREAM_CUSTOM_BLOCKING_TRANSFER 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_ALLOCATE_FIELDS
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Macro that should allocate any buffers needed for the transfer method.
 * See trcStreamPort.h for examples.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_ALLOCATE_FIELDS() 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_INIT
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Used to initialize and set up the transfer method.
 * See trcStreamPort.h for examples.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_INIT() 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_ALLOCATE_EVENT
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Specifies how the trace events that should be sent using the transfer method
 * are allocated first.
 * See trcStreamPort.h for examples.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_ALLOCATE_EVENT(_type, _ptr, _size) 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_COMMIT_EVENT
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Specifies how trace events are sent/written.
 * See trcStreamPort.h for examples.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_COMMIT_EVENT(_ptr, _size) 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_READ_DATA
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Specifies how data is read using the transfer method.
 * See trcStreamPort.h for examples.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_READ_DATA(_ptrData, _size, _ptrBytesRead) 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_PERIODIC_SEND_DATA
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Specifies how data is sent periodically. Used by certain transfer methods.
 * See trcStreamPort.h for examples.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_PERIODIC_SEND_DATA(_ptrBytesSent) 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_ON_TRACE_BEGIN
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Called on tracing is started. Use this to perform any necessary steps to 
 * properly start the trace, such as clearing buffers.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_ON_TRACE_BEGIN() 

/*******************************************************************************
 * Configuration Macro: TRC_STREAM_CUSTOM_ON_TRACE_END
 *
 * Note: Only active if TRC_RECORDER_TRANSFER_METHOD_CUSTOM is used.
 *
 * Called when tracing is disabled. Use this to perform any necessary steps to 
 * properly shut down the tracing, such as clearing buffers.
 ******************************************************************************/
#define TRC_STREAM_CUSTOM_ON_TRACE_END() 

/******************************************************************************
 * USE_16BIT_OBJECT_HANDLES
 *
 * Macro which should be defined as either zero (0) or one (1).
 * Default is 0.
 *
 * If set to 0 (zero), the recorder uses 8-bit handles to identify kernel 
 * objects such as tasks and queues. This limits the supported number of
 * concurrently active objects to 255 of each type (object class).
 *
 * If set to 1 (one), the recorder uses 16-bit handles to identify kernel 
 * objects such as tasks and queues. This limits the supported number of
 * concurrent objects to 65535 of each type (object class). However, since the
 * object property table is limited to 64 KB, the practical limit is about
 * 3000 objects in total. 
 * 
 * NOTE: An object with a high ID (> 255) will generate an extra event 
 * (= 4 byte) in the event buffer. 
 * 
 * NOTE: Some internal tables in the recorder gets larger when using 16-bit 
 * handles. The additional RAM usage is 5-10 byte plus 1 byte per kernel object
 *, i.e., task, queue, semaphore, mutex, etc.
 *****************************************************************************/
#define USE_16BIT_OBJECT_HANDLES 0
  
/******************************************************************************
* USE_PRIMASK_CS (for Cortex M devices only)
*
* An integer constant that selects between two options for the critical
* sections of the recorder library.
 *
*   0: The default FreeRTOS critical section (BASEPRI) - default setting
*   1: Always disable ALL interrupts (using PRIMASK)
 *
* Option 0 uses the standard FreeRTOS macros for critical sections.
* However, on Cortex-M devices they only disable interrupts with priorities 
* below a certain configurable level, while higher priority ISRs remain active.
* Such high-priority ISRs may not use the recorder functions in this mode.
*
* Option 1 allows you to safely call the recorder from any ISR, independent of 
* the interrupt priority. This mode may however cause higher IRQ latencies
* (some microseconds) since ALL configurable interrupts are disabled during 
* the recorder's critical sections in this mode, using the PRIMASK register.
 ******************************************************************************/
/*#ifndef USE_PRIMASK_CS
#define USE_PRIMASK_CS 0
#endif*/

/******************************************************************************
* HEAP_SIZE_BELOW_16M
*
* An integer constant that can be used to reduce the buffer usage of memory
* allocation events (malloc/free). This value should be 1 if the heap size is 
* below 16 MB (2^24 byte), and you can live with addresses truncated to the 
* lower 24 bit. Otherwise set it to 0 to get the full 32-bit addresses.
******************************************************************************/
//#define HEAP_SIZE_BELOW_16M 0

#ifdef __cplusplus
}
#endif

#endif /* TRC_CONFIG_H */
