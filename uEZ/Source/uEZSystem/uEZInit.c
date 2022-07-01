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
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *
 *  @{
 *  @brief     uEZ Initialization  Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
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
#include "uEZHandles.h"
#include <uEZGPIO.h>

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

    // Start the task system
#if FREERTOS_PLUS_TRACE
    vTraceInitTraceData();
#endif
    UEZTaskInit();

    return UEZ_ERROR_NONE;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZInit.c
 *-------------------------------------------------------------------------*/
