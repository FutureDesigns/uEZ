/*-------------------------------------------------------------------------*
 * File:  uEZDeviceTable.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ Device Table used to track all devices in the system.
 *
 * Implementation Notes:
 *      Each device is an entry in the handle table.  The handle stores
 *      three data items: workspace, device interface, and name.
 *      The workspace holds the specific implementation data for the
 *      device. The device interface is the structure for that device.
 *      The name is a static string that gives the device a unique
 *      identifier (for finding later).
 *      NOTE: It is assumed that a user of a device understands the
 *      interface associated with that device.
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
 *    @addtogroup uEZDeviceTable
 *  @{
 *  @brief     uEZ Device Table Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Device Table interface.
 *
 * @par Example code:
 * Example task to find device
 * @par
 * @code
 *  #include <uEZ.h>
 *  #include <uEZDevice.h>
 *
 *   TUInt32 SPIfind(T_uezTask aMyTask, void *aParams)
 *   {
 *       T_uezDevice SPI;
 *       if (UEZDeviceTableFind("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *           // found device handle and stored in SPI
 *       }
 *       return 0;
 *   }
 * @endcode
 */
#include <string.h>
#include <stdio.h>
#include <Config.h>
#include <uEZ.h>
#include <Source/uEZSystem/uEZHandles.h>
#include <uEZMemory.h>
#include <uEZDevice.h>

static TUInt32 G_deviceWorkspaceAllocated=0;

/*---------------------------------------------------------------------------*
 * Routine:  UEZDeviceTableRegister
 *---------------------------------------------------------------------------*/
/**
 *  Register a device with the given interface and handle.  Creates
 *      a workspace for the device.
 *
 * @param [in] *aName  		  Unique identifier for device
 *
 * @param [in] *aInterface 	  Interface to the device
 *
 * @param [in] *aDeviceHandle Pointer to recently created device's handle.
 *
 * @param [in] **aWorkspace   Pointer to pointer to recently created workspace.
 *
 * @return    T_uezError     Error code
 * @par Example Code:
 * @code
 *  #include <uEZ.h>
 *  #include <uEZDeviceTable.h>
 *  #include <Generic_SPI.h>
 *
 *   T_uezDeviceWorkspace *p_ssp0;
 *   // Initialize the SPI bus 0 device
 *   // and link to the SPI0 HAL driver
 *   UEZDeviceTableRegister(
 *           "SPI0",
 *           (T_uezDeviceInterface *)&SPIBus_Generic_Interface,
 *           0,
 *           &p_ssp0);
 * @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDeviceTableRegister(
                const char * const aName,
                T_uezDeviceInterface *aInterface,
                T_uezDevice *aDeviceHandle,
                T_uezDeviceWorkspace **aWorkspace)
{
    T_uezError error;
    T_uezDeviceWorkspace *workspace;
    T_uezDevice dev;

    error = uEZHandleAlloc(&dev);
    if (error)
        return error;

    // Got a handle.
    // Create the workspace
    workspace = (T_uezDeviceWorkspace *)UEZMemAllocPermanent(aInterface->iWorkspaceSize);
    if (workspace == 0)  {
        // Did not get the workspace, delete the handle
        // and report out of memory
        uEZHandleFree(dev);
        return UEZ_ERROR_OUT_OF_MEMORY;
    }
    // Reset the workspace to a junk state
    memset(workspace, 0xCC, aInterface->iWorkspaceSize);
    G_deviceWorkspaceAllocated += aInterface->iWorkspaceSize;

    // Got a handle and workspace.
    // Let's initialize the workspace
    workspace->iInterface = (void *)aInterface;
    error = aInterface->InitializeWorkspace((void *)workspace);
    if (error)  {
        // If we got an error, delete the handle and workspace
        UEZMemFree((void *)workspace);
        uEZHandleFree(dev);
        return error;
    }

    // Record information about this driver in the table
    uEZHandleSet(
        dev,
        UEZ_HANDLE_DEVICE,
        (TUInt32)workspace,
        (TUInt32)aInterface,
        (TUInt32)aName);

    if (aWorkspace)
        *aWorkspace = workspace;
    if (aDeviceHandle)
        *aDeviceHandle = dev;

    return UEZ_ERROR_NONE;
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZDeviceTableIsRegistered
 *---------------------------------------------------------------------------*/
/**
 *  Returns if the given device name is registered in the Device Table.
 *
 *  @param [in]    *aName 			Pointer to device name to find.
 *
 *  @return        T_uezError   	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDeviceTable.h>
 *
 *   if (UEZDeviceTableIsRegistered("SPI0") == ETrue) {
 *       // device is registered
 *   }
 * @endcode
 */
/*---------------------------------------------------------------------------*/
TBool UEZDeviceTableIsRegistered(const char * const aName)
{
    T_uezError error;
    TUInt32 index = UEZ_HANDLE_FIND_START;
    char *handleName;
    T_uezDevice device;

    // Search until we hit the end
    do {
        error = uEZHandleFindOfType(UEZ_HANDLE_DEVICE, &device, &index);
        if (error == UEZ_ERROR_NONE)  {
            // We found a match of type.
            // Does the name match?
            uEZHandleGet(device, 0, 0, 0, (TUInt32 *)&handleName);
            if (strcmp(handleName, aName) == 0)
                return ETrue;
        }
    } while (error == UEZ_ERROR_NONE);

    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZDeviceTableGetWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Register a device with the given interface and handle.  Creates
 *  a workspace for the device.
 *
 *  @param [in]    aDevice          Device with workspace to get
 *
 *  @param [in]    **aWorkspace 	Pointer to pointer to returned
 *                                  workspace.
 *  @return        T_uezError     	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZSPI.h>
 *  #include <uEZDeviceTable.h>
 *
 *  T_uezDevice flash;
 *  DEVICE_Flash **p_flash;
 *
 *  UEZDeviceTableFind("Flash0", &flash);
 *  if (UEZDeviceTableGetWorkspace(flash, (T_uezDeviceWorkspace **)&p_flash)
 *      = UEZ_ERROR_NONE) {
 *      // found workspace and can be accessed by p_flash
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDeviceTableGetWorkspace(
            T_uezDevice aDevice,
            T_uezDeviceWorkspace **aWorkspace)
{
    TUInt32 type = UEZ_HANDLE_FREE;

    uEZHandleGet(aDevice, &type, (TUInt32 *)aWorkspace, 0, 0);
    if ((type & UEZ_HANDLE_TYPE_MASK) != UEZ_HANDLE_DEVICE) {
        *aWorkspace = 0;
        return UEZ_ERROR_HANDLE_INVALID;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZDeviceTableFind
 *---------------------------------------------------------------------------*/
/**
 *  Search device table for existing device and return device handle.
 *
 *  @param [in]    *aName 			Pointer to device name to find.
 *
 *  @param [in]    *aDevice 		Pointer to device to return
 *
 *  @return        T_uezError   	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDeviceTable.h>
 *
 *   T_uezDevice SPI;
 *   if (UEZDeviceTableFind("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *       // found device handle and stored in SPI
 *   }
 * @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDeviceTableFind(const char * const aName, T_uezDevice *aDevice)
{
    T_uezError error;
    TUInt32 index = UEZ_HANDLE_FIND_START;
    char *handleName;
#ifndef NDEBUG
    static char errorString[50];
#endif

    // Search until we hit the end
    *aDevice = UEZ_NULL_HANDLE;
    do {
        error = uEZHandleFindOfType(UEZ_HANDLE_DEVICE, aDevice, &index);
        if (error == UEZ_ERROR_NONE)  {
            // We found a match of type.
            // Does the name match?
            uEZHandleGet(*aDevice, 0, 0, 0, (TUInt32 *)&handleName);
            if (strcmp(handleName, aName) == 0)
                return UEZ_ERROR_NONE;
        }
    } while (error == UEZ_ERROR_NONE);

#ifndef NDEBUG
    sprintf(errorString, "%s device not found!", aName);
    UEZFailureMsg(errorString);
#endif

    return UEZ_ERROR_NOT_FOUND;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZDeviceTableRegisterAlias
 *---------------------------------------------------------------------------*/
/**
 *  Registers another name for an existing device.
 *
 * @param [in] *aExistingName  Unique identifier for existing device
 *
 * @param [in] *aAliasName     Unique identifier for device alias
 *
 * @return    T_uezError     Error code
 * @par Example Code:
 * @code
 *  #include <uEZ.h>
 *  #include <uEZDevice.h>
 *
 *   // Register the SPI0 driver also under the name of EXP-SPI
 *   UEZDeviceTableRegisterAlias(
 *           "SPI0",
 *           "EXP-SPI");
 * 
 * @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDeviceTableRegisterAlias(
                const char * const aExistingName,
                const char * const aAliasName)
{
    T_uezError error;
    T_uezDeviceWorkspace *workspace;
    T_uezDeviceInterface *interface;
    T_uezDevice dev;
    T_uezDevice devAlias;
    TUInt32 type;

    // Find the existing device
    error = UEZDeviceTableFind(aExistingName, &dev);
    if (error)
        return error;

    // Get a new handle.
    error = uEZHandleAlloc(&devAlias);
    if (error)
        return error;

    error = uEZHandleGet(dev, &type, (TUInt32 *)&workspace, (TUInt32 *)&interface, 0 /* name */);
    if (error)
        return error;

    uEZHandleSet(devAlias, type, (TUInt32)workspace, (TUInt32)interface, (TUInt32)aAliasName);

    return UEZ_ERROR_NONE;
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZDeviceTable.c
 *-------------------------------------------------------------------------*/
