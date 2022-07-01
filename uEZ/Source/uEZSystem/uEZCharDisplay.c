/*-------------------------------------------------------------------------*
 * File:  uEZCharDisplay.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ CharDisplay API
 * Implementation:
 *      Most of the commands are thin and map directly to the 
 *      API of the CharDisplay device drivers.
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
/**
 *    @addtogroup uEZCharDisplay
 *  @{
 *  @brief     uEZ Char Display Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ Character Display interface.
 *    The uEZ system supplies routines to synchronize accesses to do a character display. A
 *    character display is a grid of characters. The current implementation assumes a left to
 *    right orientation and characters are considered fixed width with a limited number of
 *    columns and rows. The underlying communication and timing is hidden at this level.
 *
 *  @par Example code:
 *  Example task to display a row of text on the screen
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   TUInt32 textRowDisplay(T_uezTask aMyTask, void *aParams)
 *   {
 *       T_uezDevice CD;
 *       if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *           // the device opened properly
 *
 *           UEZCharDisplayClearScreen(CD);
 *           // draw "TEST" on 2nd row, 3rd column
 *           UEZCharDisplayDrawString(CD,1,2,"TEST");
 *           if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *               // error closing the device
 *           }
 *       } else {
 *           // an error occurred opening the device
 *       }
 *       return 0;
 *  }
 * @endcode
 */
#include "Config.h"
#include "uEZCharDisplay.h"
#include "Device/CharDisplay.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open access to the character display (and turn on). Access is not mutually
 *   exclusive to other tasks, but is thread protected.
 *
 *  @param [in]  *aName 	 Pointer to name of CharDisplay display 
 *                           (usually "CharDisplay")
 *  @param [in]  *aDevice    Pointer to device handle to be returned
 *
 *  @return      T_uezError  If the device is opened, returns UEZ_ERROR_NONE.
 *                           If not found returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayOpen(
            const char * const aName, 
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Open((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the CharDisplay display.  May turn off CharDisplay if 
 *      no one else is using the CharDisplay.
 *
 *  @param [in]    aDevice 		Device handle of CharDisplay to close
 *
 *  @return        T_uezError   If the device is successfully closed, returns
 * 								UEZ_ERROR_NONE.  If the device handle is bad, 
 *                              returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *           // error closing the device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayDrawChar
 *---------------------------------------------------------------------------*/
/**
 *  Draw a single character on the character display at the given column and
 *  row. If a column or row is specified outside the range of the
 *  device, it is not drawn.
 *
 *  @param [in]    aDevice     	Handle to opened CharDisplay device.
 *
 *  @param [in]    aRow       	Row on display (top to bottom, 0 based)
 *
 *  @param [in]	   aColumn    	Column on display (left to right, 0 based)
 *
 *  @param [in]	   aChar    	Character to draw
 *
 *  @return        T_uezError 	If successful, returns UEZ_ERROR_NONE.
 *                              Else, returns error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *       // draw character "A" on 2nd row, 3rd column
 *       UEZCharDisplayDrawChar(CD,1,2,(TUInt32)'A');
 *       if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *           // error closing the device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayDrawChar(
            T_uezDevice aDevice, 
            TUInt32 aRow,
            TUInt32 aColumn,
            TUInt32 aChar)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->DrawChar((void *)p, aColumn, aRow, aChar);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayDrawString
 *---------------------------------------------------------------------------*/
/**
 *  Draw a string of zero terminated characters from left to right at the given
 *  location on the display.
 *
 *  @param [in]    aDevice   	Handle to opened CharDisplay device.
 *
 *  @param [in]    aRow     	Row on display (top to bottom, 0 based)
 *
 *  @param [in]    aColumn    	Column on display (left to right, 0 based)
 *
 *  @param [in]    *aChar      	ASCII string to draw
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE.
 *                              Else, returns error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *       // draw "TEST" on 2nd row, 3rd column
 *       UEZCharDisplayDrawString(CD,1,2,"TEST");
 *       if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *           // error closing the device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayDrawString(
            T_uezDevice aDevice, 
            TUInt32 aRow,
            TUInt32 aColumn,
            const char *aChar)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Just draw all the characters
    while (*aChar) {
        error = UEZCharDisplayDrawChar(aDevice, aRow, aColumn++, *(aChar++));
        if (error)
            break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayControl
 *---------------------------------------------------------------------------*/
/**
 *  Process command specific to the underlying device. The exact operation of
 *   this command is device specific.
 *
 *  @param [in]    	aDevice			Handle to opened CharDisplay device.
 *
 *  @param [in]    	aControl        Control being processed   
 *
 *  @param [in]		*aControlData   Data for control
 *
 *  @return        T_uezError      	If successful, returns UEZ_ERROR_NONE.
 *                                 	Otherwise returns error code specific to
 *                                 	command.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   TUInt32 controlData = {0x20}; // custom control data
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *       UEZCharDisplayControl(CD,0x10,controlData); // 0x10 is custom control command
 *       if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *           // error closing the device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayControl(
            T_uezDevice aDevice,
            TUInt32 aControl,
            void *aControlData)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    if ((*p)->Control)
        return (*p)->Control((void *)p, aControl, aControlData);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayClearScreen
 *---------------------------------------------------------------------------*/
/**
 *  Clears the character display with empty/space characters.
 *
 *  @param [in]    aDevice      Handle to opened CharDisplay device.
 *
 *  @return        T_uezError	If successful, returns UEZ_ERROR_NONE.
 *                              Otherwise returns error code specific to
 *                              command.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *       UEZCharDisplayClearScreen(CD);
 *       if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *           // error closing the device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayClearScreen(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    if ((*p)->ClearScreen)
        return (*p)->ClearScreen((void *)p);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayGetSize
 *---------------------------------------------------------------------------*/
/**
 *  Gets the size of the display in the number of visible rows and columns
 *  that are available on the display.
 *
 *  @param [in]    aDevice     		Handle to opened CharDisplay device
 *
 *  @param [in]    *aNumRows     	Returned number of rows
 *
 *  @param [in]    *aNumColumns   	Returned number of columns
 *
 *  @return        T_uezError  		If successful, returns UEZ_ERROR_NONE.
 *                                 	Otherwise returns error code specific to
 *                                 	command.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   TUInt32 aNumRows;
 *   TUInt32 aNumColumns;
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *       UEZCharDisplayGetSize(CD, &aNumRows, &aNumColumns);
 *       if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *           // error closing the device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayGetSize(
                T_uezDevice aDevice,
                TUInt32 *aNumRows,
                TUInt32 *aNumColumns)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    const T_charDisplayInfo *p_info;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    p_info = (*p)->GetInfo((void *)p);
    if (aNumRows)
        *aNumRows = p_info->iNumRows;
    if (aNumColumns)
        *aNumColumns = p_info->iNumColumns;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCharDisplayClearRow
 *---------------------------------------------------------------------------*/
/**
 *  Clears a single line of characters on the screen by drawing the given
 *  character across all the columns of a single row.
 *
 *  @param [in]    aDevice      Handle to opened CharDisplay device
 *
 *  @param [in]    aRow         Row to be cleared
 *
 *  @param [in]    aChar        Character to use
 *
 *  @return        T_uezError	If successful, returns UEZ_ERROR_NONE. 
 *                              Otherwise returns error code specific to
 *                              command.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCharDisplay.h>
 *
 *   T_uezDevice CD;
 *   if (UEZCharDisplayOpen("CharDisplay", &CD) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *       // draw "TEST" on 2nd row, 3rd column
 *       UEZCharDisplayDrawString(CD,1,2,"TEST");
 *       // Clear "TEST" line from screen
 *       UEZCharDisplayClearRow(CD,1," ");
 *       if (UEZCharDisplayClose(CD) != UEZ_ERROR_NONE) {
 *           // error closing the device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCharDisplayClearRow(
                T_uezDevice aDevice,
                TUInt32 aRow,
                TUInt32 aChar)
{
    T_uezError error;
    DEVICE_CharDisplay **p;
    TUInt32 i;
    const T_charDisplayInfo *p_info;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    p_info = (*p)->GetInfo((void *)p);
    for (i=0; i<p_info->iNumColumns; i++) {
        error = UEZCharDisplayDrawChar(aDevice, aRow, i, aChar);
        if (error)
            break;
    }

    return error;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZCharDisplay.c
 *-------------------------------------------------------------------------*/
