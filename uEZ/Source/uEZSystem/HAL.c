/*-------------------------------------------------------------------------*
 * File:  HAL.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ Hardware Abstraction Layer interface routines.
 *
 * Implementation Notes:
 *      HAL interfaces are registered to the system by a unique
 *      name.  As they are registered, a workspace is created for the
 *      interface and initialized.
 *
 *      Devices that need to access a HAL then call HALInterfaceFindp()
 *      with the unique name.  All future calls then go through the
 *      retrieved interface.
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
#include <Config.h>
#include <uEZ.h>
#include <uEZMemory.h>
#include <HAL/HAL.h>
#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#ifndef MAX_NUM_HAL_WORKSPACES
#define MAX_NUM_HAL_WORKSPACES      50
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    T_halWorkspace *iWorkspace;
    const char * iName;
} T_halEntry;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_halEntry G_halWorkspaces[MAX_NUM_HAL_WORKSPACES];
static TUInt32 G_halNumRegistered=0;
static TUInt32 G_halWorkspaceAllocated=0;

/*---------------------------------------------------------------------------*
 * Routine:  HALInterfaceInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the HAL Interface tables.
 *---------------------------------------------------------------------------*/
void HALInterfaceInit(void)
{
    G_halNumRegistered = 0;
    G_halWorkspaceAllocated = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  HALInterfaceRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Register a new HAL interface and setup a workspace for it.
 * Inputs:
 *      const char * const aName  -- Unique identifier for HAL
 *      T_halInterface *aInterface -- Interface to the HAL
 *      T_uezInterface *aInterfaceHandle -- Pointer to recently created
 *                              interface's handle.
 *      T_halWorkspace **aWorkspace -- Pointer to pointer to recently
 *                              created workspace.
 * Outputs:
 *      T_uezError               -- Error code
 *---------------------------------------------------------------------------*/
T_uezError HALInterfaceRegister(
                const char * const aName,
                T_halInterface *aInterface,
                T_uezInterface *aInterfaceHandle,
                T_halWorkspace **aWorkspace)
{
    T_uezError error;
    T_halWorkspace *workspace;
    TUInt32 index;

    // Check if there is room in the table
    if (G_halNumRegistered >= MAX_NUM_HAL_WORKSPACES)
        return UEZ_ERROR_OUT_OF_HANDLES;

    // Room for a handle
    index = G_halNumRegistered;
    G_halWorkspaces[index].iWorkspace = 0;
    G_halWorkspaces[index].iName = aName;

    // Create the workspace
    workspace = (T_halWorkspace *)UEZMemAllocPermanent(aInterface->iWorkspaceSize);
    G_halWorkspaceAllocated += aInterface->iWorkspaceSize;
    if (workspace == 0)  {
        // Did not get the workspace, delete the handle
        // and report out of memory
        return UEZ_ERROR_OUT_OF_MEMORY;
    }
    // Reset the workspace to a junk state
    memset(workspace, 0xCC, aInterface->iWorkspaceSize);

    // Got a handle and workspace.
    // Let's initialize the workspace
    workspace->iInterface = (void *)aInterface;
    error = aInterface->InitializeWorkspace((void *)workspace);
    if (error)  {
        // If we got an error, delete the handle and workspace
        UEZMemFree((void *)workspace);
        return error;
    }

    // Record information about this driver in the table
    G_halWorkspaces[index].iWorkspace = workspace;

    if (aWorkspace)
        *aWorkspace = workspace;
    if (aInterfaceHandle)   // handle is same as workspace
        *aInterfaceHandle = (void *)workspace;

    // Allow another interface to be registered
    G_halNumRegistered++;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  HALInterfaceGetWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Register a new HAL interface and setup a workspace for it.
 * Inputs:
 *      T_uezInterface aInterface -- Interface to the HAL
 *      T_halWorkspace **aWorkspace -- Pointer to pointer to interfaces'
 *                              workspace.
 * Outputs:
 *      T_uezError               -- Error code
 *---------------------------------------------------------------------------*/
T_uezError HALInterfaceGetWorkspace(
            T_uezInterface aInterface,
            T_halWorkspace **aWorkspace)
{
    // Just pass back a pointer to the workspace
    *aWorkspace = (void *)aInterface;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  HALInterfaceFind
 *---------------------------------------------------------------------------*
 * Description:
 *      Find a HAL interface by name and return it's workspace.
 * Inputs:
 *      T_uezInterface aInterface -- Interface to the HAL
 *      T_halWorkspace **aWorkspace -- Pointer to pointer to interfaces'
 *                              workspace.
 * Outputs:
 *      T_uezError               -- Error code
 *---------------------------------------------------------------------------*/
T_uezError HALInterfaceFind(
            const char * const aName,
            T_halWorkspace **aWorkspace)
{
    TUInt32 index;
#ifndef NDEBUG
    char buffer [50];
#endif

    // Search until we hit the end
    for (index=0; index<G_halNumRegistered; index++)  {
        if (strcmp(aName, G_halWorkspaces[index].iName)==0)  {
            // We found a match.
            *aWorkspace = G_halWorkspaces[index].iWorkspace;
            return UEZ_ERROR_NONE;
        }
    }

    // Did not find it, report not found
    *aWorkspace = 0;

#ifndef NDEBUG
    sprintf (buffer, "HAL %s not found!", aName);
    UEZFailureMsg(buffer);
#endif

    return UEZ_ERROR_NOT_FOUND;
}

/*-------------------------------------------------------------------------*
 * File:  HAL.c
 *-------------------------------------------------------------------------*/

