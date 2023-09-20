/*-------------------------------------------------------------------------*
 * File:  lpc2478_I2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Generic LPC2478 I2C Bus.
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
#include <uEZTypes.h>
#include <uEZDeviceTable.h>
#include <HAL/HAL.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include <HAL/I2SBus.h>
#include <uEZRTOS.h>
#include <HAL/I2SBus.h>
#include "Generic_I2S.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_I2S_BUS *iDevice;
    T_uezSemaphore iSem;
    HAL_I2S **iI2S;
} T_Generic_i2s_Workspace;
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_Generic_i2s_Workspace *G_Generic_i2s_Workspace;
extern const HAL_I2S G_Generic_I2S;

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError Generic_I2S_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;

    T_Generic_i2s_Workspace *p = (T_Generic_i2s_Workspace *)aWorkspace;
    G_Generic_i2s_Workspace = p;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    return error;
}
/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError Generic_I2S_Stop(void *aWorkspace)
{
    T_Generic_i2s_Workspace *p = (T_Generic_i2s_Workspace *)aWorkspace;

    return (*p->iI2S)->Stop(p->iI2S);
}
/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError Generic_I2S_Start(void *aWorkspace)//transmit low functions ?
{
    T_Generic_i2s_Workspace *p = (T_Generic_i2s_Workspace*)aWorkspace;

    return (*p->iI2S)->Start(p->iI2S);
}
/*---------------------------------------------------------------------------*
 * Routine: Config
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
T_uezError Generic_I2S_config_TX(
    void *aWorkspace,
    HAL_I2S_Callback_Transmit_Low aTransmitLowCallback)
{
    T_Generic_i2s_Workspace *p = (T_Generic_i2s_Workspace *)aWorkspace;

    G_Generic_i2s_Workspace = p;

    return (*p->iI2S)->Configure(aWorkspace, aTransmitLowCallback);
}

/*---------------------------------------------------------------------------*
 * Create function:
 *---------------------------------------------------------------------------*/
T_uezError Generic_I2S_Create(const char *aName, const char *aHALI2SName)
{
    T_Generic_i2s_Workspace *p;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&G_Generic_I2S_Interface, 0,
            (T_uezDeviceWorkspace **)&p);

    // Access the I2C device and link in or return an error
    HALInterfaceFind(aHALI2SName, (T_halWorkspace **)&p->iI2S);
    G_Generic_i2s_Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

const DEVICE_I2S_BUS G_Generic_I2S_Interface = {
    {
    "Audio:Generic:I2S",
    0x0100,
    Generic_I2S_InitializeWorkspace,
    sizeof(T_Generic_i2s_Workspace),
    },

    Generic_I2S_config_TX,
    Generic_I2S_Start,
    Generic_I2S_Stop, };

/*-------------------------------------------------------------------------*
 * End of File:  lpc2478_I2C.c
 *-------------------------------------------------------------------------*/
