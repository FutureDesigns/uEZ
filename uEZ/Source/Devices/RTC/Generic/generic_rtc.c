/*-------------------------------------------------------------------------*
 * File:  generic_rtc.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a RTC that merely communicates with
 *      a simple RTC HAL driver implementation.
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

#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include "Generic_RTC.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_RTC *iDevice;
    T_uezSemaphore iSem;
    HAL_RTC **iRTC;
} T_RTC_Generic_Workspace;


/*---------------------------------------------------------------------------*
 * Routine:  RTC_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace to connect to generic RTC HAL driver.
 *      Automatically connects to the default "RTC" HAL driver.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const char *aHALName        -- Name of HAL device (usually "RTC")
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_RTC_Generic_Workspace *p = (T_RTC_Generic_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_Generic_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current RTC clock reading.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_Generic_Get(
        void *aWorkspace, 
        T_uezTimeDate *aTimeDate)
{
    T_uezError error;
    T_RTC_Generic_Workspace *p = (T_RTC_Generic_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iRTC)->Get(p->iRTC, aTimeDate);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_Generic_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current RTC clock.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const T_uezTimeDate *aTimeDate -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_Generic_Set(
        void *aWorkspace, 
        const T_uezTimeDate *aTimeDate)
{
    T_uezError error;
    T_RTC_Generic_Workspace *p = (T_RTC_Generic_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iRTC)->Set(p->iRTC, aTimeDate);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_Generic_Validate
 *---------------------------------------------------------------------------*
 * Description:
 *      Normally, validate the current RTC date and time for integrity
 *      if the power has been out.  For generic usage, we'll always assume
 *      that the RTC is NOT valid and set it to the given setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const T_uezTimeDate *aTimeDate -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code, always UEZ_ERROR_INVALID
 *---------------------------------------------------------------------------*/
static T_uezError RTC_Generic_Validate(
        void *aWorkspace, 
        const T_uezTimeDate *aTimeDate)
{
    T_RTC_Generic_Workspace *p = (T_RTC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // If the HAL driver has a Validate command, go ahead and use that
    // otherwise, assume its invalid
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if ((*p->iRTC)->Validate) {
        error = (*p->iRTC)->Validate(p->iRTC, aTimeDate);
        UEZSemaphoreRelease(p->iSem);
    } else {
        UEZSemaphoreRelease(p->iSem);
        RTC_Generic_Set(aWorkspace, aTimeDate);
        error = UEZ_ERROR_INVALID;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_Generic_SetClockOutHz
 *---------------------------------------------------------------------------*
 * Description:
 *      Not supported.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aHertz -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_Generic_SetClockOutHz(
        void *aWorkspace,
        TUInt32 aHertz)
{
    // By default, this is generically not supported.
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link Generic RTC to HAL hardware device
 * Inputs:
 *      void *aW                    -- Workspace
 *      HAL_RTC **aRTC              -- HAL hardware to link to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
void RTC_Generic_Configure(void *aWorkspace, HAL_RTC **aRTC)
{
    T_RTC_Generic_Workspace *p = (T_RTC_Generic_Workspace *)aWorkspace;

    p->iRTC = aRTC;
}

T_uezError RTC_Generic_Create(const char *aName, const char *aHALName)
{
    HAL_RTC **p_rtchal;
    T_uezDeviceWorkspace *p_rtcdev;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&RTC_Generic_Interface, 0, &p_rtcdev);
    HALInterfaceFind(aHALName, (T_halWorkspace **)&p_rtchal);
    RTC_Generic_Configure(p_rtcdev, (HAL_RTC **)p_rtchal);
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_RTC RTC_Generic_Interface = {
	{
	    // Common device interface
	    "RTC:Generic",
	    0x0100,
	    RTC_Generic_InitializeWorkspace,
	    sizeof(T_RTC_Generic_Workspace),
	},
	
    // Functions
    RTC_Generic_Get,
    RTC_Generic_Set,
    RTC_Generic_Validate,
    RTC_Generic_SetClockOutHz
} ;

/*-------------------------------------------------------------------------*
 * End of File:  generic_rtc.c
 *-------------------------------------------------------------------------*/
