/*-------------------------------------------------------------------------*
 * File:  ADC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Generic ADC Bank.
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
#include <HAL/ADCBank.h>
#include "Generic_ADC.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_ADCBank *iDevice;
    T_uezSemaphore iSem;
    HAL_ADCBank **iADCBank;
    T_uezSemaphore iPending;
} T_ADC_Generic_Workspace;

static void IADC_Generic_CaptureComplete(
            void *aWorkspace, 
            ADC_RequestSingle *aRequest);

static const T_adcBankCallbackInterface G_ADC_Generic_CallbackAPI = {
    IADC_Generic_CaptureComplete,
};

/*---------------------------------------------------------------------------*
 * Routine:  ADC_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a ADC Bank and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void IADC_Generic_CaptureComplete(
            void *aWorkspace, 
            ADC_RequestSingle *aRequest)
{
    T_ADC_Generic_Workspace *p = (T_ADC_Generic_Workspace *)aWorkspace;

    // Release the waiting caller
    _isr_UEZSemaphoreRelease(p->iPending );
}

/*---------------------------------------------------------------------------*
 * Routine:  ADC_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a ADC Bank and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ADC_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_ADC_Generic_Workspace *p = (T_ADC_Generic_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    // Create a binary semaphore that is released when data comes in
    error = UEZSemaphoreCreateBinary(&p->iPending);
    UEZSemaphoreGrab(p->iPending, 0);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ADC_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link the ADCBank Device Driver to a ADCBank HAL driver.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const char *aADCHALName     -- HAL Device driver name
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ADC_Generic_Configure(void *aW, const char *aADCHALName)
{
    T_ADC_Generic_Workspace *p = (T_ADC_Generic_Workspace *)aW;
    T_uezError error;

    // Access the ADCBank device
    error = HALInterfaceFind(aADCHALName, (T_halWorkspace **)&p->iADCBank);
    if (error)
        return error;

    // Hook up the callback function
    error = (*p->iADCBank)->Configure(
            p->iADCBank, 
            p, 
            &G_ADC_Generic_CallbackAPI);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ADC_Generic_RequestSingle
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a single ADC transfer using polling methods.  Will block until
 *      the ADC is completed.  Also protects multiple tasks from using
 *      the same resource.
 * Inputs:
 *      void *aW                    -- Workspace
 *      ADC_RequestSingle *aRequest -- ADC request being used.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ADC_Generic_RequestSingle(
              void *aWorkspace, 
              ADC_RequestSingle *aRequest)
{
    T_uezError error;
    T_ADC_Generic_Workspace *p = (T_ADC_Generic_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, 500);

    // Put in the request and when the ISR calls the callback, the
    // semaphore will be released.
    error = (*p->iADCBank)->RequestSingle(p->iADCBank, aRequest);

    // Wait until it is released (if not already)
    UEZSemaphoreGrab(p->iPending, 500);

    // Data is ready, but may disappear soon.  Grab what we need

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SPI_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a generic thread safe interface to the SPI HAL driver
 *      for handling SPI requests.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aSerialHALName -- Name of low level SPI HAL driver
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError ADCBank_Generic_Create(
        const char *aName,
        const char *aADCHALName)
{
    T_uezDeviceWorkspace *p_adc0;

    // Initialize the ADC0 device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&ADCBank_Generic_Interface,
            0,
            &p_adc0);
    return ADC_Generic_Configure(p_adc0, aADCHALName);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ADCBank ADCBank_Generic_Interface = {
    // Common device interface
    {
	    "ADC:Generic",
	    0x0100,
	    ADC_Generic_InitializeWorkspace,
	    sizeof(T_ADC_Generic_Workspace),
	},
	
    // Functions
    ADC_Generic_RequestSingle,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  ADC.c
 *-------------------------------------------------------------------------*/
