/*-------------------------------------------------------------------------*
 * File:  uEZBSPInit.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic BSP intialization.  Effectively is the system main.
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
 *  
 *  @brief     uEZ BSPInit Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ BSPInit interface.
 *
 *  @par Example code:
 *  Example task to init system
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  UEZBSP_Startup();
 *  @endcode
 */
#include "uEZ.h"
#include "uEZBSP.h"
#include <uEZPlatform.h>

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_uezTask G_startupTask;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern TUInt32 uEZPlatformStartup(T_uezTask aMyTask, void *aParameters);

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_Startup
 *---------------------------------------------------------------------------*/
/**
 *  Bring up the system.  Before this routine is called, all memory
 *      should have been initialized from SystemInit (which is called
 *      before C code initialization in the startup assembly file).
 *
 *  @return        void
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  UEZBSP_Startup();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZBSP_Startup(void)
{
    // Reset the interrupts
    UEZBSP_InterruptsReset();

    // Initialize the core components of the UEZ System (such as the 
    // HAL and Device registry) in preparation for Processor and Platform
    // configuration.
    UEZSystemInit();

#if (SEGGER_ENABLE_RTT ==1 )  // enable RTT
#if (SEGGER_ENABLE_SYSTEM_VIEW != 1) //systemview will auto init RTT
    (void)_SEGGER_RTT; // GCC complains if we don't use this.
    SEGGER_RTT_Init(); // RTT can be enabled and used early here before RTOS, (no RTOS required)
    //SEGGER_RTT_WriteString(0, "Hello World RTT 0!\n"); // Test RTT Interface
#endif  
#endif

    // Initialize the devices
    uEZProcessorServicesInit();

    // Do any platform initialization
    uEZPlatformInit();

    // Create a startup task (not running yet)
    UEZTaskCreate(
        (T_uezTaskFunction)uEZPlatformStartup, 
        "Startup", 
        UEZ_TASK_STACK_BYTES( 1024 ),
        0, 
        UEZ_PRIORITY_NORMAL, 
        &G_startupTask);

    // Run everything
    UEZSystemMainLoop();
}

/*---------------------------------------------------------------------------*
 * Routine:  SystemInit
 *---------------------------------------------------------------------------*/
/**
 *  SystemInit is the very earliest configuration of the system.
 *      It is called when there is only a simple start up stack and nothing
 *      else running in the system.  It is very delicate and should only
 *      do a the listed tasks below.  Once these tasks are done, the
 *      C code will be initialized (zero memory, copy constants, and run).
 *
 *  @return        void
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  SystemInit();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void SystemInit(void)
{  
    UEZBSP_Pre_PLL_SystemInit(); // uEZ 2.11
    UEZBSP_PLLConfigure();
    UEZBSP_CPU_PinConfigInit(); // uEZ 2.11
    UEZBSP_RAMInit();
    UEZBSP_ROMInit();
    UEZBSP_Post_SystemInit(); // uEZ 2.11.1
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZBSPInit.c
 *-------------------------------------------------------------------------*/
