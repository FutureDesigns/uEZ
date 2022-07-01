/*-------------------------------------------------------------------------*
 * File:  uEZFlash.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Flash Interface
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
 *  @addtogroup uEZFlash
 *  @{
 *  @brief     uEZ Flash Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *  The uEZ Flash interface.
 *
 *  The uEZ system supplies routines to synchronize accesses to flash devices.
 *  A flash device is defined as a non-volatile memory grouped into blocks that
 *  must be erased before they can be programmed. Routines have been provided
 *  to get the size, read, write, and erase the flash device.
 *
 *  @par Example code:
 *  Example Routine to read and write data into flash
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 *
 *  TUInt32 FlashTest(void)
 *  {
 *      T_uezDevice flash;
 *      TUInt8 aBuffer[0x10] = { 200 };
 *
 *      if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *          // The device opened properly
 *          // offset = 0x100, number of bytes = 0x10
 *          // Erase flash before writing
 *          // NOTE: May erase more than size given depending on erase rules
 *          UEZFlashBlockErase(flash, 0x100, 0x10);
 *
 *          // Write data from aBuffer into flash
 *          UEZFlashWrite(flash, 0x100, aBuffer, 0x10);
 *
 *          // Read data from flash into aBuffer
 *          UEZFlashRead(flash, 0x100, aBuffer, 0x10);
 *
 *          // Now data is stored in aBuffer
 *          if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *              // error closing the device
 *              return 1;
 *          }
 *      } else {
 *          // an error occurred opening the device
 *          return 1;
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
#include <Config.h>
#include <uEZFlash.h>
#include <Device/Flash.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the Flash bus.
 *
 *  @param [in]    *aName         Pointer to name of Flash bus (usually
 *                                      "Flashx" where x is bus number)
 *  @param [in]    *aDevice     Pointer to device handle to be returned
 *
 *  @return        T_uezError      If the device is opened, returns
 *                              UEZ_ERROR_NONE. If the device cannot be found,
 *                              returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // The device opened properly
 *  } else {
 *      // An error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashOpen(
            const char * const aName,
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->Open(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the Flash bus.
 *
 *  @param [in]    aDevice         Device handle of Flash to close
 *
 *  @return        T_uezError    If the device is successfully closed, returns
 *                              UEZ_ERROR_NONE. If the device handle is bad, 
 *                              returns UEZ_ERROR_HANDLE_INVALID.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // The device opened properly
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->Close(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashRead
 *---------------------------------------------------------------------------*/
/**
 *  Read bytes out of the Flash.
 *
 *  @param [in]    aDevice         Device handle
 *
 *  @param [in]    aOffset         Byte Offset address into flash to read
 *
 *  @param [in]    *aBuffer        Pointer to buffer to receive data
 *
 *  @param [in]    aNumBytes    Number of bytes to read
 *
 *  @return        T_uezError      Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *  TUInt8 aBuffer[0x10];
 *
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // The device opened properly
 *      // Read data into the buffer
 *      UEZFlashRead(flash,0x100, aBuffer, 0x10);
 *
 *      // Now data is stored in aBuffer
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // Error closing the device
 *      }
 *  } else {
 *      // An error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashRead(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->Read(p, aOffset, aBuffer, aNumBytes);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write bytes into the Flash. NOTE: You must erase the flash before
 *      writing to it.
 *
 *  @param [in]    aDevice        Device handle
 *
 *  @param [in]    aOffset        Byte Offset into flash to write
 *
 *  @param [in]    *aBuffer       Pointer to buffer of data
 *
 *  @param [in]    aNumBytes       Number of bytes to write
 *
 *  @return        T_uezError     Returns UEZ_ERROR_BAD_ALIGNMENT if offset or
 *                              number of bytes is not on word boundary.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *  TUInt8 aBuffer[0x10] = {200}; // contains data to write to flash
 *
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // The device opened properly
 *      // Erase first, then write.
 *      // NOTE: May erase more than size given depending on erase rules
 *      UEZFlashBlockErase(flash, 0x100, 0x10);
 *      UEZFlashWrite(flash,0x100, aBuffer ,0x10);
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // Error closing the device
 *      }
 *  } else {
 *      // An error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashWrite(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes)
{
    T_uezError error=UEZ_ERROR_NONE;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->Write(p, aOffset, aBuffer, aNumBytes);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashBlockErase
 *---------------------------------------------------------------------------*/
/**
 *  Erase one ore more blocks in the given location
 *
 *  @param [in]    aDevice      Device handle
 *
 *  @param [in]    aOffset        Offset to begin erase
 *
 *  @param [in]    aNumBytes    Number of bytes to write
 *
 *  @return        T_uezError    Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // The device opened properly
 *      UEZFlashBlockErase(flash, 0x100, 0x10);
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // Error closing the device
 *      }
 *  } else {
 *      // An error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashBlockErase(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            TUInt32 aNumBytes)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->BlockErase(p, aOffset, aNumBytes);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashChipErase
 *---------------------------------------------------------------------------*/
/**
 *  Erase the complete chip.
 *
 *  @param [in]    aDevice      Device handle
 *
 *  @return        T_uezError      Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // The device opened properly
 *      UEZFlashChipErase(flash);
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // Error closing the device
 *      }
 *  } else {
 *      // An error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashChipErase(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->ChipErase(p);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashQueryReg
 *---------------------------------------------------------------------------*/
/**
 *  Query the chip for the given register.
 *
 *  @param [in]    aDevice          Device handle
 *
 *  @param [in]    aReg                Register to query
 *
 *  @param [in]    *aValue           Place to store value returned
 *
 *  @return        T_uezError        Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *  TUInt32 aValue;
 *
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // The device opened properly
 *      // Query register
 *      if (UEZFlashQueryReg(aDevice, 1, &aValue) == UEZ_ERROR_NONE) {
 *          // value stored in aValue now
 *      }
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashQueryReg(
            T_uezDevice aDevice,
            TUInt32 aReg,
            TUInt32 *aValue)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->QueryReg(p, aReg, aValue);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashGetChipInfo
 *---------------------------------------------------------------------------*/
/**
 *  Get information about the chip size,
 *
 *  @param [in]    *aDevice     Device handle
 *
 *  @param [in]    *aInfo          Pointer to structure to receive info
 *
 *  @return        T_uezError      Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *  T_FlashChipInfo aInfo;
 *  TUInt32 iNumBytesLow;
 *  TUInt32 iNumBytesHigh; // if bigger than 4 Gig
 *  TUInt8 iBitWidth;   // 8, 16, or 32
 *  TUInt32 iNumRegions;
 *  T_FlashChipRegion iRegions[FLASH_CHIP_MAX_TRACKED_REGIONS];
 *
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // the device opened properly
 *
 *      if (UEZFlashGetChipInfo(aDevice, &aInfo) == UEZ_ERROR_NONE) {
 *          iNumBytesLow = aInfo->iNumBytesLow;
 *          iNumBytesHigh = aInfo->iNumBytesHigh;
 *          iBitWidth = aInfo->iBitWidth;
 *          iNumRegions = aInfo->iNumRegions;
 *      }
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashGetChipInfo(
            T_uezDevice aDevice,
            T_FlashChipInfo *aInfo)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->GetChipInfo(p, aInfo);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZFlashGetBlockInfo
 *---------------------------------------------------------------------------*/
/**
 *  Get information about the block
 *
 *  @param [in]    aDevice      Device handle
 *
 *  @param [in]    aOffset         Byte offset into flash
 *
 *  @param [in]    *aBlockInfo  Pointer to structure to receive info
 *
 *  @return        T_uezError   Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZFlash.h>
 * 
 *  T_uezDevice flash;
 *  T_FlashBlockInfo aBlockInfo;
 *  TUInt32 offset;
 *  TUInt32 size;
 *
 *  if (UEZFlashOpen("Flash0", &flash) == UEZ_ERROR_NONE) {
 *      // the device opened properly
 *
 *      if (UEZFlashGetBlockInfo(flash, 0xFF, &aBlockInfo) == UEZ_ERROR_NONE) {
 *          offset = aBlockInfo->iOffset;
 *          size = aBlockInfo->iSize;
 *      }
 *      if (UEZFlashClose(flash) != UEZ_ERROR_NONE) {
 *          // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZFlashGetBlockInfo(
            T_uezDevice aDevice,
            TUInt32 aOffset,
            T_FlashBlockInfo *aBlockInfo)
{
    T_uezError error;
    DEVICE_Flash **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->GetBlockInfo(p, aOffset, aBlockInfo);

    return error;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZFlash.c
 *-------------------------------------------------------------------------*/
