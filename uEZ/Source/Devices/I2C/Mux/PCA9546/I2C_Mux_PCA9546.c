/*-------------------------------------------------------------------------*
 * File:  I2C_Mux_PCA9546.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/I2CBus.h>
#include <uEZDeviceTable.h>
#include <uEZMemory.h>
#include "I2C_Mux_PCA9546.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezSemaphore iSem;
    TUInt8 iMuxLane;
} T_I2C_Mux_PCA9546_Group;

typedef struct {
    const DEVICE_I2C_BUS *iDevice;
    DEVICE_I2C_BUS **iI2CBus;
    TUInt8 iMuxAddr;
    TUInt8 iMuxLane;
    T_I2C_Mux_PCA9546_Group *iGroup;
} T_I2C_Mux_PCA9546_Workspace;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Mux_PCA9546_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a I2C Bus called I2C0 and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError II2C_Mux_PCA9546_InitializeWorkspace(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Mux_PCA9546_ProcessRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Do two I2C requests (a write followed by a read) or only one
 *      (write or read) per the request.  Block until the request is
 *      processed or times out.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      I2C_Request *aRequest        -- I2C request being used.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Mux_PCA9546_ProcessRequest(void *aWorkspace, I2C_Request *aRequest)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_I2C_Mux_PCA9546_Workspace *p = (T_I2C_Mux_PCA9546_Workspace *)aWorkspace;
    T_I2C_Mux_PCA9546_Group *pg = p->iGroup;
    I2C_Request r;
    TUInt8 lane;

    // Allow only one request at a time for the whole mux group
    UEZSemaphoreGrab(pg->iSem, UEZ_TIMEOUT_INFINITE);

    // Are we changing lanes?
    if (pg->iMuxLane != p->iMuxLane) {
        // Need to tell the mux to change lanes
        r.iAddr = p->iMuxAddr;
        r.iReadData = 0;
        r.iReadLength = 0;
        r.iReadTimeout = 0;
        r.iSpeed = aRequest->iSpeed;
        lane = (1<<p->iMuxLane);
        r.iWriteData = &lane;
        r.iWriteLength = 1;
        r.iWriteTimeout = 1000;

        error = (*(p->iI2CBus))->ProcessRequest(p->iI2CBus, &r);

        // We are now on a different lane, remember it
        pg->iMuxLane = p->iMuxLane;
    }

    if (!error) {
        // Now pass the request to the I2C driver
        error = (*(p->iI2CBus))->ProcessRequest(p->iI2CBus, aRequest);
    }

    // Done with this request, allow the next one to occur for the group.
    UEZSemaphoreRelease(pg->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Mux_PCA9546_SlaveStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Setups
 * Inputs:
 *      void *p -- I2C Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Mux_PCA9546_SlaveStart(
    void *aWorkspace,
    const T_I2CSlaveSetup *aSetup)
{
    PARAM_NOT_USED(aWorkspace);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Mux_PCA9546_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enables the I2C channel.
 * Inputs:
 *      void *p -- I2C Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Mux_PCA9546_Enable(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Mux_PCA9546_Disable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables the I2C channel.
 * Inputs:
 *      void *p -- I2C Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError I2C_Mux_PCA9546_Disable(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);

    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError I2C_Mux_PCA9546_SlaveWaitForEvent(
    void *aWorkspace,
    T_I2CSlaveEvent *aEvent,
    TUInt32 *aLength,
    TUInt32 aTimeout)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aEvent);
    PARAM_NOT_USED(aLength);
    PARAM_NOT_USED(aTimeout);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  I2C_Mux_PCA9546_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a I2C MUX driver for doing data MUX transfers through a
 *      mux on the I2C driver.  Each lane of the mux will look like its
 *      own I2C bus.  Each lane of the mux share a group handle.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aI2CBus -- Name of I2C Bus this mux is on
 *      TUInt8 aI2CMuxAddr -- Address of mux itself
 *      TUInt8 aI2CMuxLane -- Index of lane (0-3)
 *      void **aGroupWorkspace -- Common group identifier.  Pass 0 for
 *                      first group and then pass the identifier/pointer
 *                      for all next buses on the mux.
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
void I2C_Mux_PCA9546_Create(
    const char *aName,
    const char *aI2CBus,
    TUInt8 aI2CMuxAddr,
    TUInt8 aI2CMuxLane,
    void **aGroupWorkspace)
{
    T_I2C_Mux_PCA9546_Workspace *p;
    T_I2C_Mux_PCA9546_Group *pg;
    T_uezDevice i2c;

    pg = *aGroupWorkspace;

    // If non exists, create the group
    if (!pg) {
        // Create a group
        pg = (T_I2C_Mux_PCA9546_Group *)UEZMemAlloc(sizeof(T_I2C_Mux_PCA9546_Group));
        if (!pg) {
            UEZFailureMsg("Cannot alloc PCA9546 Group!");
            return;
        }
        *aGroupWorkspace = pg;

        // Create the semaphore for the whole group
        UEZSemaphoreCreateBinary(&pg->iSem);

        // No mux lane chosen yet
        pg->iMuxLane = 0xFF;
    } else {
        // Look up the group
        pg = (T_I2C_Mux_PCA9546_Group *)(*aGroupWorkspace);
    }

    // Setup a generic I2C driver
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&I2CBus_Mux_PCA9546_Interface, 0, (T_uezDeviceWorkspace **)&p);

    // Find the I2C driver that has the mux
    UEZDeviceTableFind(aI2CBus, &i2c);

    // Link this device driver to the I2C bus
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p->iI2CBus);

    p->iMuxAddr = aI2CMuxAddr;
    p->iMuxLane = aI2CMuxLane;
    p->iGroup = pg;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_I2C_BUS I2CBus_Mux_PCA9546_Interface = { {
// Common device interface
    "I2C:Mux:PCA9546",
    0x0100,
    II2C_Mux_PCA9546_InitializeWorkspace,
    sizeof(T_I2C_Mux_PCA9546_Workspace), },

// Functions
    I2C_Mux_PCA9546_ProcessRequest,

    // uEZ v1.10 Functions: (Not used in this driver!)
    I2C_Mux_PCA9546_SlaveStart,
    I2C_Mux_PCA9546_SlaveWaitForEvent,
    I2C_Mux_PCA9546_Enable,
    I2C_Mux_PCA9546_Disable };

/*-------------------------------------------------------------------------*
 * End of File:  I2C_Mux_PCA9546.c
 *-------------------------------------------------------------------------*/
