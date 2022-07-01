/*-------------------------------------------------------------------------*
 * File:  uEZCANGeneric.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic stream interface to a CAN device.
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <Device/CAN.h>
#include <HAL/CAN.h>
#include "Generic_CAN.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_CAN *iDevice;
    T_uezSemaphore iSem;
    HAL_CAN **iCAN;
    TUInt32 iNumOpen;
} T_CAN_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  CAN_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a generic CAN workspace.  Configure will setup the next
 *      step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CAN_Generic_InitializeWorkspace(void *aW)
{
    T_CAN_Generic_Workspace *p = (T_CAN_Generic_Workspace *)aW;
    
    p->iNumOpen = 0;

    // Then create a semaphore to limit the number of accessors
    return UEZSemaphoreCreateBinary(&p->iSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  CAN_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the generic configuration with the given queue sizes and
 *      link to HAL CAN device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      HAL_CAN **aCAN      -- CAN Device to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CAN_Generic_Configure(
            T_uezDeviceWorkspace *aWorkspace, 
            HAL_CAN **aCAN)
{
    T_CAN_Generic_Workspace *p = (T_CAN_Generic_Workspace *)aWorkspace;

    // Record CAN device
    p->iCAN = aCAN;

    return (*p->iCAN)->Configure((void *)p->iCAN, (void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  CAN_Generic_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the CAN port (activating it if first user).
 * Inputs:
 *      void *aWorkspace          -- This CAN generic workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CAN_Generic_Open(void *aWorkspace)
{
    T_CAN_Generic_Workspace *p = (T_CAN_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // If the last user, deactivate
    p->iNumOpen++;
    if (p->iNumOpen == 1)
        error = (*p->iCAN)->Activate((T_halWorkspace *)p->iCAN);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  CAN_Generic_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the CAN port (deactivating it if last user to close).
 * Inputs:
 *      void *aWorkspace          -- This CAN generic workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CAN_Generic_Close(void *aWorkspace)
{
    // Decrement use count.  Are we done?
    T_CAN_Generic_Workspace *p = (T_CAN_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // If the last user, deactivate
    p->iNumOpen--;
    if (p->iNumOpen == 0)
        error = (*p->iCAN)->Deactivate((T_halWorkspace *)p->iCAN);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  CAN_Generic_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the CAN port or timeout trying.  If a timeout,
 *      the number of bytes written is reported.
 * Inputs:
 *      void *aWorkspace          -- This CAN generic workspace
 *      TUInt8 aID                -- ID of the remote device
 *      TUInt8 *aData             -- Data to send
 *      TUInt32 aDataLen          -- Number of bytes to send
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_TIMEOUT is returned
 *                                   if timeout occurs trying to write
 *                                   the full amount and not enough room was
 *                                   available.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError CAN_Generic_Write(
            void *aWorkspace,
            TUInt8 aID,
            TUInt8 *aData,
            TUInt8 aDataLen)
{
    // Decrement use count.  Are we done?
    T_CAN_Generic_Workspace *p = (T_CAN_Generic_Workspace *)aWorkspace;

    return (*p->iCAN)->Send((T_halWorkspace *)p->iCAN, aID, aData, aDataLen);;
}

/*---------------------------------------------------------------------------*
 * Routine:  CAN_Generic_Control
 *---------------------------------------------------------------------------*
 * Description:
 *      Command to configure the port or do handshaking via the stream
 *      interface.  For the uEZ Generic CAN driver, only
 *      STREAM_CONTROL_SET_CAN_SETTINGS and 
 *      STREAM_CONTROL_GET_CAN_SETTINGS are currently provided.
 * Inputs:
 *      void *aWorkspace          -- This CAN generic workspace
 *      TUInt32 aControl          -- Command to execute
 *      void *aControlData        -- Data passed in with the command
 * Outputs:
 *      T_uezError                 -- Error code specific to command.
 *                                   If the command is unhandled, error code
 *                                   UEZ_ERROR_ILLEGAL_OPERATION is returned.
 *---------------------------------------------------------------------------*/
T_uezError CAN_Generic_Control(
            void *aWorkspace, 
            TUInt32 aControl, 
            void *aControlData)
{
    T_CAN_Generic_Workspace *p = (T_CAN_Generic_Workspace *)aWorkspace;
    HAL_CAN *ps = *(p->iCAN);

    // Process the command
    switch (aControl)  {
        case CAN_CONTROL_SET_SETTINGS:
            return ps->SetSettings(p->iCAN, (T_CAN_Settings *)aControlData);
        case CAN_CONTROL_GET_SETTINGS:   
            return ps->GetSettings(p->iCAN, (T_CAN_Settings *)aControlData);
        default:
            return UEZ_ERROR_ILLEGAL_OPERATION;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  CAN_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a full duplex CAN stream to a HAL CAN device and create
 *      the read and write buffers.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aCANHALName -- Name of low level CAN HAL driver
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError CAN_Generic_Create(
        const char *aName,
        const char *aCANHALName)
{
    T_halWorkspace *p_CAN_HAL;
    T_uezDeviceWorkspace *p_CAN;

    // Create CAN stream device and link to HAL_CAN driver
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&CAN_Generic_Interface, 0,
            &p_CAN);
    HALInterfaceFind(aCANHALName, (T_halWorkspace **)&p_CAN_HAL);
    return CAN_Generic_Configure(p_CAN, (HAL_CAN **)p_CAN_HAL);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_CAN CAN_Generic_Interface = {
	{
	    // Generic device interface
	    "CAN:Generic",
	    0x0100,
	    CAN_Generic_InitializeWorkspace,
	    sizeof(T_CAN_Generic_Workspace),
	},
	
    // Functions
    CAN_Generic_Open,
    CAN_Generic_Close,
    CAN_Generic_Control,
    CAN_Generic_Write,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  uEZCANGeneric.c
 *-------------------------------------------------------------------------*/
