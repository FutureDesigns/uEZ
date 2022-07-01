/*-------------------------------------------------------------------------*
 * File:  CRC_Generic.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a CRC that merely communicates with
 *      a simple CRC HAL driver implementation.
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
#include "CRC_Generic.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_CRC *iDevice;
    T_uezSemaphore iSem;
    HAL_CRC **iCRC;
} T_CRC_Generic_Workspace;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
#define IGrab()     UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()  UEZSemaphoreRelease(p->iSem)

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace to connect to generic CRC HAL driver.
 *      Automatically connects to the default "CRC" HAL driver.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const char *aHALName        -- Name of HAL device (usually "CRC")
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError CRC_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_CRC_Generic_Workspace *p = (T_CRC_Generic_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine: CRC_Generic_Interface_SetComputationType
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets computation mode using either CCITT, CRC16, or CRC32 polynom
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      T_uezCRCComputationType aType  -- Type of CRC computations to do
 * Outputs:
 *      T_uezError                     -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_SetComputationType(
        void *aWorkspace,
        T_uezCRCComputationType aType)
{
    T_CRC_Generic_Workspace *p = (T_CRC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iCRC)->SetComputationType(p->iCRC, aType);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine: CRC_Generic_Interface_Open
 *---------------------------------------------------------------------------*
 * Description:
 *    
 * Inputs:
 *      void *aWorkspace               -- Workspace
 * Outputs:
 *      T_uezError                     -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_Open(void *aWorkspace)
{
    // Doesn't do anything in this implementation
    PARAM_NOT_USED(aWorkspace);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: CRC_Generic_Interface_Close
 *---------------------------------------------------------------------------*
 * Description:
 *    
 * Inputs:
 *      void *aWorkspace               -- Workspace
 * Outputs:
 *      T_uezError                     -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_Close(void *aWorkspace)
{
    // Doesn't do anything in this implementation
    PARAM_NOT_USED(aWorkspace);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_Interface_AddModifier
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a modifier to the algorithm.  If the modifier already has been
 *      added, do nothing.
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      T_uezCRCModifier aModifier -- Type of modifier to add
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_AddModifier(
        void *aWorkspace,
        T_uezCRCModifier aModifier)
{
    T_CRC_Generic_Workspace *p = (T_CRC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iCRC)->AddModifier(p->iCRC, aModifier);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_Interface_RemoveModifier
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a modifier to the algorithm.  If the modifier already has been
 *      added, do nothing.
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      T_uezCRCModifier aModifier -- Type of modifier to add
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_RemoveModifier(
        void *aWorkspace,
        T_uezCRCModifier aModifier)
{
    T_CRC_Generic_Workspace *p = (T_CRC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iCRC)->RemoveModifier(p->iCRC, aModifier);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_Interface_SetSeed
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the seed (usually 0) of a new calculation.
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      TUInt32 aSeed                -- Seed value
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_SetSeed(void *aWorkspace, TUInt32 aSeed)
{
    T_CRC_Generic_Workspace *p = (T_CRC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iCRC)->SetSeed(p->iCRC, aSeed);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_Interface_ReadChecksum
 *---------------------------------------------------------------------------*
 * Description:
 *      Return current checksum reading
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      TUInt32 *aValue -- Place to store checksum
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_ReadChecksum(void *aWorkspace, TUInt32 *aValue)
{
    T_CRC_Generic_Workspace *p = (T_CRC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iCRC)->ReadChecksum(p->iCRC, aValue);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_Interface_ComputeData
 *---------------------------------------------------------------------------*
 * Description:
 *      Pass in a set of data to compute a checksum.  This can be a single
 *      value or a group of data.  The size of the data elements are passed
 *      as well to tell if computations on small data (bytes) or larger
 *      chunks (words).
 * Inputs:
 *      void *aWorkspace               -- Workspace
 *      void *aData                    -- Pointer to data array
 *      T_uezCRCDataElementSize aSize  -- Size of each element
 *      TUInt32 aNumDataElements       -- Number of data elements to process
 * Outputs:
 *      T_uezError                     -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Interface_ComputeData(
        void *aWorkspace,
        void *aData,
        T_uezCRCDataElementSize aSize,
        TUInt32 aNumDataElements)
{
    T_CRC_Generic_Workspace *p = (T_CRC_Generic_Workspace *)aWorkspace;
    T_uezError error;

    // Pass on the request
    IGrab();
    error = (*p->iCRC)->ComputeData(p->iCRC, aData, aSize, aNumDataElements);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  CRC_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create the CRC Device driver and link it to an existing CRC HAL Driver.
 * Inputs:
 *      const char *aName           -- Name of new CRC generic device driver
 *      const char *aHALDriverName  -- HAL Device driver name to link to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError CRC_Generic_Create(const char *aName, const char *aHALDriverName)
{
    T_CRC_Generic_Workspace *p;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&CRC_Generic_Interface, 0,
            (T_uezDeviceWorkspace **)&p);
    return HALInterfaceFind(aHALDriverName, (T_halWorkspace **)&p->iCRC);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_CRC CRC_Generic_Interface = {
        {
        // Common device interface
                "CRC:Generic",
                0x0100,
                CRC_Generic_InitializeWorkspace,
                sizeof(T_CRC_Generic_Workspace), },

        /* High level functions */
        CRC_Generic_Interface_Open,
        CRC_Generic_Interface_Close,

        /* Advanced functions: */
        CRC_Generic_Interface_SetComputationType,
        CRC_Generic_Interface_AddModifier,
        CRC_Generic_Interface_RemoveModifier,
        CRC_Generic_Interface_SetSeed,
        CRC_Generic_Interface_ComputeData,
        CRC_Generic_Interface_ReadChecksum };

/*-------------------------------------------------------------------------*
 * End of File:  CRC_Generic.c
 *-------------------------------------------------------------------------*/
