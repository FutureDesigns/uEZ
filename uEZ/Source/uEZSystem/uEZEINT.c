/*-------------------------------------------------------------------------*
 * File:  uEZEINT.c
 *-------------------------------------------------------------------------*
 * Description:
 *     External Interrupt Interface
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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
/**
 *  @addtogroup uEZEINT
 *  @{
 *  @brief     uEZ EINT Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ EINT interface.
 *
 *  @par Example code:
 *  Example task to TODO
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <Types/EINT.h>
 *  #include <uEZEINT.h>
 *  //TODO
 * @endcode
 */
#include "Config.h"
#include "Device/ExternalInterrupt.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>
#include <Types/EINT.h>
#include <uEZEINT.h>



/*---------------------------------------------------------------------------*
 * Routine:  UEZEINTOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the EINT bank and setup a channel.
 *
 *  @param [in]    *aName 				Pointer to name of EINT bank (usually
 *                                      "EINTx" where x is bank number)
 *  @param [in]    *aDevice 			Pointer to device handle to be returned
 *
 *  @param [in]    aChannel				Channel number
 *
 *  @param [in]    aTrigger 			Trigger mode
 *
 *  @param [in]    aPriority			Priority level
 *
 *  @param [in]    aCallBackFunc 		Call back funtion
 * 
 *  @param [out]   *aCallbackWorkspace 	Call back workspace
 *
 *  @return        T_uezError   If the device is opened, 
 *                              returns UEZ_ERROR_NONE.  If the device cannot 
 *                              be found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Types/EINT.h>
 *  #include <uEZEINT.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEINTOpen(const char *const aName,
                       T_uezDevice *aDevice,
                       TUInt32 aChannel,
                       T_EINTTrigger aTrigger,
                       T_EINTPriority aPriority,
                       EINT_Callback aCallBackFunc,
                       void *aCallbackWorkspace)
{
    T_uezError error;
    DEVICE_ExternalInterrupt **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Set((void *)p, 
                     aChannel, 
                     aTrigger, 
                     aCallBackFunc, 
                     aCallbackWorkspace,
                     aPriority, 
                     aName);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEINTClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the EINT bank.
 *
 *  @param [in]    aDevice 		Device handle of EINT to close
 *
 *  @return        T_uezError   If the device is successfully closed, returns
 *                              UEZ_ERROR_NONE.  If the device handle is bad,
 *                              returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Types/EINT.h>
 *  #include <uEZEINT.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEINTClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_ExternalInterrupt **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEINTClear
 *---------------------------------------------------------------------------*/
/**
 *  Clears the settings for an EINT channel
 *
 *  @param [in]    aDevice 		Device handle of EINT to close
 *
 *  @param [in]    aChannel     Channel Number
 *
 *  @return        T_uezError   If the EINT channel is successfully cleared, 
 *                              returns UEZ_ERROR_NONE.  If the device handle
 *                              is bad, returns	UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Types/EINT.h>
 *  #include <uEZEINT.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEINTClear(T_uezDevice aDevice, TUInt32 aChannel)
{
    T_uezError error;
    DEVICE_ExternalInterrupt **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Clear((void *)p, aChannel);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEINTEnable
 *---------------------------------------------------------------------------*/
/**
 *  Enables an EINT channel
 *
 *  @param [in]    aDevice 		Device handle of EINT to close
 *
 *  @param [in]    aChannel     Channel Number
 *
 *  @return        T_uezError  	If the EINT channel is successfully enabled, 
 *                             	returns UEZ_ERROR_NONE.  If the device handle
 *                              is bad, returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Types/EINT.h>
 *  #include <uEZEINT.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEINTEnable(T_uezDevice aDevice, TUInt32 aChannel)
{
    T_uezError error;
    DEVICE_ExternalInterrupt **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Enable((void *)p, aChannel);
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZEINTDisable
 *---------------------------------------------------------------------------*/
/**
 *  Disables an EINT channel
 *
 *  @param [in]    aDevice 		Device handle of EINT to close
 *
 *  @param [in]    aChannel   	Channel Number
 *
 *  @return        T_uezError 	If the EINT channel is successfully disabled, 
 *                             	returns UEZ_ERROR_NONE.  If the device handle
 *                              is bad, returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Types/EINT.h>
 *  #include <uEZEINT.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEINTDisable(T_uezDevice aDevice, TUInt32 aChannel)
{
    T_uezError error;
    DEVICE_ExternalInterrupt **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Disable((void *)p, aChannel);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZEINT.c
 *-------------------------------------------------------------------------*/
