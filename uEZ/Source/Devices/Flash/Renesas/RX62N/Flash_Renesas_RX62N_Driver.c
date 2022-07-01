/*-------------------------------------------------------------------------*
 * File:  Flash_Renesas_RX62N_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Flash for Spansion S29GL064N90
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <string.h>
#include "flash_data.h"
#include "flash_api_rx600.h"
#include "Flash_Renesas_RX62N.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/*******************************************************************************
* Global Variables
*******************************************************************************/
/* Imported global variable, current mode of operation */
extern TUInt8 current_mode;

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RX_FLASH_PE_QUERY_ADDR		(0x007FC450)
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Flash *iDevice;
    T_uezSemaphore iSem;
    TUInt32 *iBaseAddr;		//<<---- Made changes here (was (volatile)TUInt16)
    TUInt32 iNumOpen;
    TBool iIsProgramMode;
    TBool iIsChipInfoReady;
    T_FlashChipInfo iChipInfo;
} T_Flash_Renesas_RX62N_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static T_uezError IEnsureChipInfo(void *aWorkspace);

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for Data Flash. For the Renesas RX62N, we allow the MCU to
 * 		access the data flash area.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    p->iBaseAddr = 0;
    p->iNumOpen = 0;
    p->iIsProgramMode = EFalse;
    p->iIsChipInfoReady = EFalse;
	// Grant Read Access here
	R_FlashDataAreaAccess(0xFFFF, 0x0000);
	return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IExitProgramMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit programming mode
 * Inputs:
 *      T_Flash_S29GL064N90_Workspace *p -- Workspace
 *---------------------------------------------------------------------------*/
static void IExitProgramMode(T_Flash_Renesas_RX62N_Workspace *p)
{
    if (p->iIsProgramMode) {
        // If we are in programming mode, exit it
    	UEZTaskDelay(1);
        p->iIsProgramMode = EFalse;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the flash device. Initialize the flash 
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_Open(void *aWorkspace)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
	/* Declare erase operation result container variable */
    p->iNumOpen++;
    if (p->iNumOpen == 1) {
        // If the first time, assume we were in some programming mode
        // and need to reset and Re-enable access to the Flash Data area
        p->iIsProgramMode = ETrue;
        IExitProgramMode(p);
		
    }
	// Grant full access if we want to open and operate on flash
	R_FlashDataAreaAccess(0xFFFF, 0xFFFF);
	
	current_mode = READ_MODE;			//<<---- Since we want to program the data flash only
    	
    return ((T_uezError)UEZ_ERROR_NONE);
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the flash device.  For the Renesas RX62N, the flash area access
 *		needs to be closed using IExitProgramMode.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_Close(void *aWorkspace)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
    if (p->iNumOpen) {
        p->iNumOpen--;
        if (p->iNumOpen == 0){
            IExitProgramMode(p);
		}
		//Grant Read Access Here
		R_FlashDataAreaAccess(0xFFFF, 0x0000);
		current_mode = READ_MODE;
        return UEZ_ERROR_NONE;
    } else {
        return UEZ_ERROR_NOT_OPEN;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read bytes out of the Flash.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte Offset address into flash to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_Read(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
	//TUInt8* Temp;
    // Make sure its not in a programming mode.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
	
	IExitProgramMode(p);
	current_mode = READ_MODE;

    // The memory is now accessible, copy the data over
	// Simple copy with check to see if the requested area is within the data flash
	if((void *)(((volatile TUInt8 *)p->iBaseAddr)+aOffset)>=(void*)DF_ADDRESS && 
		(void *)(((volatile TUInt8 *)p->iBaseAddr)+aOffset)<=(void*)(DF_ADDRESS+DF_NUM_BLOCKS*DF_BLOCK_SIZE))
	{
		memcpy(aBuffer, (void *)(((volatile TUInt8 *)p->iBaseAddr)+aOffset), aNumBytes);
	}
	else{
        UEZSemaphoreRelease(p->iSem);
		return UEZ_ERROR_OUT_OF_RANGE;
	}

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IWaitStatusDoneOrError
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for the status to show that the there is an error OR that
 *      the operation has completed.
 * Inputs:
 *      T_Flash_Renesas_RX62N_Workspace *p -- Workspace
 *      TUInt32 aWordOffset         -- Offset of last write/program/etc.	//<<---- Not used, but kept to maintaing syntax with UEZ!
 * Outputs:
 *      T_uezError                  -- Error code.  Returns
 *                                      UEZ_ERROR_INTERNAL_ERROR if error.
 *---------------------------------------------------------------------------*/
static T_uezError IWaitStatusDoneOrError(
        T_Flash_Renesas_RX62N_Workspace *p,
        TUInt32 aWordOffset)
{
    TUInt32 wait_cnt;

	/* Set the wait counter with timeout value */
        wait_cnt = WAIT_MAX_DF_WRITE;

        return(R_Flash_Status_Check(wait_cnt));
		    
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write bytes into the Flash
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte Offset into flash to write
 *      TUInt8 *aBuffer             -- Pointer to buffer of data
 *      TUInt32 aNumBytes           -- Number of bytes to write
 * Outputs:
 *      T_uezError                  -- Error code.  Returns
 *                                      UEZ_ERROR_BAD_ALIGNMENT if offset
 *                                      or number of bytes
 *                                      is not on word boundary.
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_Write(
        void *aWorkspace,
        TUInt32 aOffset,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
	TUInt8 padBuffer[256];
	TUInt8 ret = 0;
	TUInt8* flash_ptr = (TUInt8*)(p->iBaseAddr) + aOffset;
	// Let's do it
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
	
	memset(padBuffer, 0x00, 256);
	memcpy((void*)padBuffer, (void*)aBuffer, aNumBytes);
    while (1) {
    	/* Check if number of bytes is greater than 256 */
        if (aNumBytes > 256)
        {
            /* Number of bytes to write too high, set error flag to 1 */
            error = UEZ_ERROR_READ_WRITE_ERROR;
            break;
        }
        /* Check if number of bytes to write is a multiple of 8 */
        else if (aNumBytes % 8u)
        {
            /* Pad the data to write so it makes up to the nearest multiple of 8 */
            aNumBytes += 8u - (aNumBytes % 8u);
        }
	

    	// Passing the both the addresses, total offset and buffer(from where we want to write), as a value!!
    	error = (T_uezError)R_FlashWrite(((TUInt32)(p->iBaseAddr)+aOffset), (TUInt32)padBuffer,(TUInt16)aNumBytes ); //<<---- Check for error here
    
    	if(error) {
    		error = UEZ_ERROR_READ_WRITE_ERROR;
            break;
        }
	
    	ret = memcmp(aBuffer, flash_ptr, aNumBytes);
    	if(ret!=0) {
    		error = UEZ_ERROR_READ_WRITE_ERROR;
            break;
        }
        break;
    }
	UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_BlockErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase one ore more blocks in the given location
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to write
 *      TUInt8 *aBuffer             -- Pointer to buffer of data
 *      TUInt32 aNumBytes           -- Number of bytes to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_BlockErase(
        void *aWorkspace,
        TUInt32 aBlockNum,
        TUInt32 aNumBytes)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
    
    T_uezError error = UEZ_ERROR_NONE;
    
	if(aNumBytes!= DF_BLOCK_SIZE)
	{
		error = UEZ_ERROR_NOT_ENOUGH_DATA;
		
	}else{
	
    	IEnsureChipInfo(aWorkspace);

 	   	// Let's do it
    	UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    	error = (T_uezError)R_FlashErase((TUInt8)aBlockNum);		//<<---- Check for Error Here

    	// Go ahead and force out of program mode
    	IExitProgramMode(p);

    	// Done erasing, resume normal control
    	UEZSemaphoreRelease(p->iSem);
	}

    return error;
}



/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_ChipErase
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the complete chip
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_ChipErase(void *aWorkspace)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Let's do it
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Remember that we are in program mode
    p->iIsProgramMode = ETrue;

    Erase_FlashData();				//<<---- Calling from flash_data.c

    // Wait for it to finish (or error)
    error = IWaitStatusDoneOrError(p, 0);

    // Go ahead and force out of program mode
    IExitProgramMode(p);

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_QueryReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Query the chip for the given register (word based address).
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aReg                -- Register to query
 *      TUInt32 *aValue             -- Place to store value returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_QueryReg(
        void *aWorkspace,
        TUInt32 aReg,
        TUInt32 *aValue)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
	
    // Let's do it
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // For RX62N, read the value from the address
    
    *aValue = *(TUInt32*)aReg;

    // Note we are in program/query mode (and need to exit first)
    p->iIsProgramMode = ETrue;

    // Done erasing, resume normal control
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IEnsureChipInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the chip size
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_FlashChipInfo *aInfo      -- Pointer to structure to receive info
 * Outputs:
 *      T_uezError                   -- Error code.  Returns UEZ_ERROR_NOT_FOUND
 *                                      if flash information is not found
 *                                      (in this case, missing QRY header)
 *---------------------------------------------------------------------------*/
static T_uezError IEnsureChipInfo(void *aWorkspace)
{
    TUInt32 value;

    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;

    if (!p->iIsChipInfoReady) {
        // See if we get the proper header
        Flash_Renesas_RX62N_QueryReg(aWorkspace, RX_FLASH_PE_QUERY_ADDR , &value);	//<<---- Read FENTRYR and see if Key and Data flash mode are setup
        if (value != (TUInt32)(0x00FF00FF))			
            return UEZ_ERROR_NOT_FOUND;
        
        // Fill in the size and width
        //Flash_Renesas_RX62N_QueryReg(aWorkspace, CMD16_REG_DEVICE_SIZE, &value);
        p->iChipInfo.iNumBytesLow = DF_SIZE;
        p->iChipInfo.iNumBytesHigh = 0;
        
        //Flash_Renesas_RX62N_QueryReg(aWorkspace, CMD16_REG_BIT_WIDTH, &value);
        //p->iChipInfo.iBitWidth = 8*value;
		p->iChipInfo.iBitWidth = 8*1;			// Bit Width can be 8 or 128 bytes
		
        // Fill in the region info
        //Flash_Renesas_RX62N_QueryReg(aWorkspace, CMD16_REG_NUM_REGIONS, &value);
        p->iChipInfo.iNumRegions = 16;
		
        p->iIsChipInfoReady = ETrue;
    }
    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_GetChipInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the chip size
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_FlashChipInfo *aInfo      -- Pointer to structure to receive info
 * Outputs:
 *      T_uezError                   -- Error code.  Returns UEZ_ERROR_NOT_FOUND
 *                                      if flash information is not found
 *                                      (in this case, missing QRY header)
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_GetChipInfo(
        void *aWorkspace,
        T_FlashChipInfo *aInfo)
{
    T_uezError error;
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;

    // Ensure it is loaded in memory
    error = IEnsureChipInfo(aWorkspace);
    if (error)
        return error;

    // Copy the information over
    *aInfo = p->iChipInfo;

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_GetBlockInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the block
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aOffset             -- Byte offset into flash
 *      T_FlashChipInfo *aInfo      -- Pointer to structure to receive info
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Flash_Renesas_RX62N_GetBlockInfo(
        void *aWorkspace,
        TUInt32 aOffset,
        T_FlashBlockInfo *aBlockInfo)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;
	(void*)p;					// Getting rid of warning
    IEnsureChipInfo(aWorkspace);

    aBlockInfo->iOffset = (aOffset - DF_ADDRESS)/DF_BLOCK_SIZE;		//<<---- Made changes here!! (Need a faster way to calculate block number)
    // TBD: Right now we only support region 0!
    aBlockInfo->iSize = DF_BLOCK_SIZE;

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the flash chip's location
 * Inputs:
 *      void *aW                    -- Workspace
 *      void *aBaseAddr             -- Base address of this chip in
 *                                     processor memory map space
 *---------------------------------------------------------------------------*/
void Flash_Renesas_RX62N_Configure(
        void *aWorkspace,
        void *aBaseAddr)
{
    T_Flash_Renesas_RX62N_Workspace *p = (T_Flash_Renesas_RX62N_Workspace *)aWorkspace;

    p->iBaseAddr = aBaseAddr;		//<<---- Made Changes here!//<<---- Undone
}

/*---------------------------------------------------------------------------*
 * Routine:  Flash_Renesas_RX62N_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the flash chip's location
 * Inputs:
 *      void *aW                    -- Workspace
 *      void *aBaseAddr             -- Base address of this chip in
 *                                     processor memory map space
 *---------------------------------------------------------------------------*/
void Flash_Renesas_RX62N_Create(const char *aName)
{
	T_uezDeviceWorkspace *p_DeviceWorkspace;
	T_Flash_Renesas_RX62N_Workspace *p_FlashWorkspace;
	
	// Setup the NOR flash
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&Flash_Renesas_RX62N_Interface,
        0, &p_DeviceWorkspace);

	p_FlashWorkspace = (T_Flash_Renesas_RX62N_Workspace *)p_DeviceWorkspace;
    p_FlashWorkspace->iBaseAddr = (void *)RENESAS_FLASH_BASE_ADDR;		//<<---- Made Changes here!//<<---- Undone
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Flash Flash_Renesas_RX62N_Interface = {
	{
		// Common device interface
	    "Flash:Renesas:RX62N:16Bit",
	    0x0100,										//<<---- What is the 0x100 stand for??
	    Flash_Renesas_RX62N_InitializeWorkspace,
	    sizeof(T_Flash_Renesas_RX62N_Workspace),
	},
	
    // Functions
    Flash_Renesas_RX62N_Open,
    Flash_Renesas_RX62N_Close,
    Flash_Renesas_RX62N_Read,
    Flash_Renesas_RX62N_Write,
    Flash_Renesas_RX62N_BlockErase,
    Flash_Renesas_RX62N_ChipErase,
    Flash_Renesas_RX62N_QueryReg,
    Flash_Renesas_RX62N_GetChipInfo,
    Flash_Renesas_RX62N_GetBlockInfo,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  Flash_Renesas_RX62N_driver.c
 *-------------------------------------------------------------------------*/
