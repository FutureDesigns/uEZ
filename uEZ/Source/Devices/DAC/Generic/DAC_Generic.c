/*-------------------------------------------------------------------------*
 * File:  DAC_Generic.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Generic DAC.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include <HAL/DAC.h>
#include "DAC_Generic.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_DAC *iDevice;
    T_uezSemaphore iSem;
    HAL_DAC **iDAC;
} T_DAC_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define IGrab()     UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()  UEZSemaphoreRelease(p->iSem)

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a DAC and semaphore to ensure single accesses
 *      at all times.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link the DAC Device Driver to a DAC HAL driver.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const char *aDACHALName     -- HAL Device driver name
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_Configure(void *aW, const char *aDACHALName)
{
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aW;
    T_uezError error;

    // Access the DAC device
    error = HALInterfaceFind(aDACHALName, (T_halWorkspace **)&p->iDAC);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write value to the DAC.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 aValue               -- Value to be written to DAC
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_Write(void *aWorkspace, TUInt32 aValue)
{
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iDAC)->Write(p->iDAC, aValue);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_WriteMilliVolt
 *---------------------------------------------------------------------------*
 * Description:
 *      Write value to the DAC in millivolts.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 aMilliVolts          -- Value in millivolts to be written to DAC
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_WriteMilliVolts(void *aWorkspace, TUInt32 aMilliVolts)
{
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iDAC)->WriteMilliVolts(p->iDAC, aMilliVolts);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read value from the DAC.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 *aValue              -- Value to be written to.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_Read(void *aWorkspace, TUInt32 *aValue)
{
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iDAC)->Read(p->iDAC, aValue);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read value from the DAC in milliVolts.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TUInt32 *aMilliVolts         -- Value in mV to be written to.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_ReadMilliVolts(void *aWorkspace, TUInt32 * aMilliVolts)
{
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iDAC)->ReadMilliVolts(p->iDAC, aMilliVolts);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_SetBias
 *---------------------------------------------------------------------------*
 * Description:
 *      Set BIAS.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      TBool aBias                  -- Value to determine if BIAS should be set.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_SetBias(void *aWorkspace, TBool aBias)
{
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iDAC)->SetBias(p->iDAC, aBias);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  DAC_Generic_SetVRef
 *---------------------------------------------------------------------------*
 * Description:
 *      Set internal voltage reference.
 * Inputs:
 *      void *aW                     -- DAC workspace
 *      const TUInt32 aVRef          -- Set VRef in millivolts.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_SetVRef(void *aWorkspace, const TUInt32 aVRef)
{
    T_DAC_Generic_Workspace *p = (T_DAC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iDAC)->SetVRef(p->iDAC, aVRef);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Creation routines:
 *---------------------------------------------------------------------------*/
T_uezError DAC_Generic_Create(const char *aName, const char *aHALName)
{
    T_uezDeviceWorkspace *p_dac0;

    // Initialize the DAC0 device
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&DAC_Generic_Interface, 0, &p_dac0);
    return DAC_Generic_Configure(p_dac0, aHALName);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_DAC DAC_Generic_Interface = {
        {
                "DAC:Generic",
                0x0106,
                DAC_Generic_InitializeWorkspace,
                sizeof(T_DAC_Generic_Workspace), },

        DAC_Generic_Read,
        DAC_Generic_ReadMilliVolts,
        DAC_Generic_Write,
        DAC_Generic_WriteMilliVolts,
        DAC_Generic_SetBias,
        DAC_Generic_SetVRef };

/*-------------------------------------------------------------------------*
 * End of File:  DAC_Generic.c
 *-------------------------------------------------------------------------*/
