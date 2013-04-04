/*-------------------------------------------------------------------------*
 * File:  PWM.c
 *-------------------------------------------------------------------------*
 * Description:
 *      DEVICE implementation of the Generic PWMs Interface.
 * Implementation:
 *      Two PWMs are created, PWM0 and PWM1.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
#include "Config.h"
#include <uEZDeviceTable.h>
#include <uEZTypes.h>
#include <uEZRTOS.h>
#include <HAL/PWM.h>
#include "Generic_PWM.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_PWM *iDEVICE;

    T_uezSemaphore iSem;
    HAL_PWM **iPWM;

//??    TUInt32 iPeriod;
} T_PWM_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a PWM Bus and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError PWM_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_PWM_Generic_Workspace *p = (T_PWM_Generic_Workspace *)aW;

    // Create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_SetMaster
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to start -- chip select enabled for device
 * Outputs:
 *      void *aWorkspace        -- Workspace for PWM
 *      TUInt32 aPeriod         -- Number of Fpclk intervals per period
 *      TUInt8 aMatchRegister   -- Which register is used for master counter
 *---------------------------------------------------------------------------*/
static T_uezError PWM_Generic_SetMaster(
        void *aWorkspace, 
        TUInt32 aPeriod,
        TUInt8 aMatchRegister)
{
    T_PWM_Generic_Workspace *p = (T_PWM_Generic_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iPWM)->SetMaster(p->iPWM, aPeriod, aMatchRegister);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_SetMatchRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure one of the several match registers for this PWM bank.
 * Outputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Index to match register (0-7)
 *      TUInt32 aMatchPoint     -- Number of PWM cycles until match
 *      TBool aDoInterrupt      -- ETrue if want an interrupt, else EFalse
 *                                  (NOTE: Interrupts currently not 
 *                                  implemented)
 *      TBool aDoCounterReset   -- ETrue if counter for this PWM bank is
 *                                  to be reset on match.
 *      TBool aDoStop           -- ETrue if counter is to be stopped
 *                                  when match occurs.
 *---------------------------------------------------------------------------*/
static T_uezError PWM_Generic_SetMatchRegister(
        void *aWorkspace, 
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{
    T_PWM_Generic_Workspace *p = (T_PWM_Generic_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iPWM)->SetMatchRegister(
                p->iPWM, 
                aMatchRegister, 
                aMatchPoint, 
                aDoInterrupt, 
                aDoCounterReset, 
                aDoStop);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_EnableSingleEdgeOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where PWM switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError PWM_Generic_EnableSingleEdgeOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_PWM_Generic_Workspace *p = (T_PWM_Generic_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iPWM)->EnableSingleEdgeOutput(
                p->iPWM, 
                aMatchRegister);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_DisableOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where PWM switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError PWM_Generic_DisableOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_PWM_Generic_Workspace *p = (T_PWM_Generic_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iPWM)->DisableOutput(
                p->iPWM, 
                aMatchRegister);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_SetMatchCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Enables a callback routine for a Match Register
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *		TUInt8 aMatchRegister	-- The match register
 *      void *aCallback   		-- Pointer to the callback method
 *      void *aCallbackWorkspace-- Workspace for callback
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError PWM_Generic_SetMatchCallback(
        void *aWorkspace,
		TUInt8 aMatchRegister,
		void (*aCallback)(void *aCallbackWorkspace),
        void *aCallbackWorkspace)
{
    T_PWM_Generic_Workspace *p = (T_PWM_Generic_Workspace *)aWorkspace;

    return (*p->iPWM)->SetMatchCallback(
                p->iPWM,
                aMatchRegister,
				aCallback,
                aCallbackWorkspace);
}


/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a PWM device to ensure single accesses at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError PWM_Generic_Configure(void *aW, const char *aPWMHALName)
{
    T_PWM_Generic_Workspace *p = (T_PWM_Generic_Workspace *)aW;

    // Link to the PWM device or report an error when tried
    return HALInterfaceFind(aPWMHALName, (T_halWorkspace **)&p->iPWM);
}

/*---------------------------------------------------------------------------*
 * Routine:  PWM_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a PWM device and connect to the given PWM HAL driver.
 * Inputs:
 *      const char *aName -- Name of PWM driver to create
 *      const char *aPWMHALName -- HAL driver to connect to
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError PWM_Generic_Create(const char *aName, const char *aPWMHALName)
{
    T_uezDeviceWorkspace *p_pwm;

    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&PWM_Generic_Interface,
            0,
            &p_pwm);

    return PWM_Generic_Configure(p_pwm, aPWMHALName);
}

/*---------------------------------------------------------------------------*
 * DEVICE_PWM Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_PWM PWM_Generic_Interface = {
	{
	    "NXP:Generic:PWM",
	    0x0100,
	    PWM_Generic_InitializeWorkspace,
	    sizeof(T_PWM_Generic_Workspace),
	},
	
    PWM_Generic_SetMaster,
    PWM_Generic_SetMatchRegister,
    PWM_Generic_EnableSingleEdgeOutput,
    PWM_Generic_DisableOutput,
	PWM_Generic_SetMatchCallback,
};

/*-------------------------------------------------------------------------*
 * End of File:  PWM.c
 *-------------------------------------------------------------------------*/

