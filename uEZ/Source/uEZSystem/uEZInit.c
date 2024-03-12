/*-------------------------------------------------------------------------*
 * File:  uEZInit.c
 *-------------------------------------------------------------------------*
 * Description:
 *      This file holds the uEZ system initialization before any
 *      devices are initialized.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *
 *  @{
 *  @brief     uEZ Initialization  Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Initialization  interface.
 *
 *  @par Example code:
 *  Example task to
 *  @par
 *  @code
 *  #include <uEZ.h>
 *
 *  UEZSystemInit();
 *  @endcode
 */
#include "Config.h"
#include "uEZ.h"
#include <uEZMemory.h>
#include "uEZHandles.h"
#include <uEZGPIO.h>
#ifdef FREERTOS_PLUS_TRACE
//#include <trcUser.h>
#endif

extern void UEZTaskInit(void);

/*---------------------------------------------------------------------------*
 * Routine:  UEZSystemInit
 *---------------------------------------------------------------------------*/
/**
 *  Setup the memory, the handle table, and bring up the RTOS'
 *  multitasking system.
 *
 *  @return        T_uezError  	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  UEZSystemInit();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZSystemInit(void)
{
    // Initialize the memory system
    UEZMemInit();

    // Setup handles system that RTOS sits on top of
    uEZHandleTableInit();

    // Setup the GPIO table
    UEZGPIOReset();

    // Start FreeRTOS plus trace if enabled
#ifdef FREERTOS_PLUS_TRACE
    vTraceInitTraceData();
    #if (SEGGER_ENABLE_SYSTEM_VIEW == 1)
      #error "Cannot use SystemView with FreeRTOS+Trace!"
    #endif
#endif
    // Start SystemView if enabled
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1)
  #if (SEGGER_ENABLE_RTT == 0)
    #error "RTT Must be enabled to use SystemView!"
  #endif
    SEGGER_SYSVIEW_Conf(); // This runs SEGGER_SYSVIEW_Init and SEGGER_RTT_Init
#endif
    // Start the task system
    UEZTaskInit();

    return UEZ_ERROR_NONE;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZInit.c
 *-------------------------------------------------------------------------*/
