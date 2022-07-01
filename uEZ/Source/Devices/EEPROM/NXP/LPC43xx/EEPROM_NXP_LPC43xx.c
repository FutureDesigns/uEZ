/*-------------------------------------------------------------------------*
 * File:  EEPROM_NXP_LPC43xx_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the EEPROM for the LPC43xx
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <HAL/Interrupt.h>
#include "EEPROM_NXP_LPC43xx.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/** EEPROM start address */
#define EEPROM_START                    (0x20040000)
/** EEPROM byes per page */
#define EEPROM_PAGE_SIZE                (128)
/**The number of EEPROM pages. The last page is not writable.*/
#define EEPROM_PAGE_NUM                 (128)
/** Get the eeprom address */
#define EEPROM_ADDRESS(page, offset)     (EEPROM_START + (EEPROM_PAGE_SIZE * (page)) + offset)
#define EEPROM_CLOCK_DIV                 1500000
#define EEPROM_READ_WAIT_STATE_VAL       0x58
#define EEPROM_WAIT_STATE_VAL            0x232

#define LPC43xx_EEPROM_PAGE_SIZE         EEPROM_PAGE_SIZE

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_EEPROM *iDevice;
    T_uezSemaphore iSem;
    TUInt8 iPage[EEPROM_PAGE_SIZE];
    T_EEPROMConfig iConfig;
    TBool iInitialized;
    T_uezSemaphore iIRQReadySem;
    volatile int end_of_prog;
    volatile int end_of_rdwr;
} T_EEPROM_NXP_LPC43xx_Workspace;

/* EEPROM_CMD */
#define READ8				0
#define READ16				1
#define READ32				2
#define WRITE8				3
#define WRITE16				4
#define WRITE32				5
#define ERASE_PROG			6
#define RD_PFETCH			(1<<3)

#define END_OF_RDWR			26
#define END_OF_PROG			28

static T_EEPROM_NXP_LPC43xx_Workspace *G_lpc43xxEEPROMWorkspace;

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_NXP_LPC43xx_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for LPC43xx.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError EEPROM_NXP_LPC43xx_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_EEPROM_NXP_LPC43xx_Workspace *p = (T_EEPROM_NXP_LPC43xx_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    // Setup some standard settings in case they are not setup
    p->iConfig.iCommSpeed = 100; // not really needed
    p->iConfig.iPageSize = LPC43xx_EEPROM_PAGE_SIZE;
    p->iConfig.iWriteTime = 25;
    p->iInitialized = EFalse;

    // Record the global
    G_lpc43xxEEPROMWorkspace = p;

    return error;
}

static void IProcessIRQ(T_EEPROM_NXP_LPC43xx_Workspace *p)
{
    TUInt32 status = LPC_EEPROM->INTSTAT;

    if (status & ((0x1 << END_OF_RDWR) | (0x1 << END_OF_PROG))) {
        if (status & (0x1 << END_OF_RDWR))
            p->end_of_rdwr = 1;
        if (status & (0x1 << END_OF_PROG))
            p->end_of_prog = 1;
        LPC_EEPROM->INTSTATCLR = status;
    }
}

IRQ_ROUTINE(IFlashIRQHandler)
{
    IRQ_START();
    IProcessIRQ(G_lpc43xxEEPROMWorkspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  IInitialize
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the EEPROM workspace.
 * Inputs:
 *      T_EEPROM_NXP_LPC43xx_Workspace *p -- Workspace
 *---------------------------------------------------------------------------*/
static void IInitialize(T_EEPROM_NXP_LPC43xx_Workspace *p)
{
    uint32_t v;

    LPC_EEPROM->PWRDWN = 0x0;

    v = PROCESSOR_OSCILLATOR_FREQUENCY / 1500000;
    LPC_EEPROM->CLKDIV = v;
    v = LPC_EEPROM->CLKDIV;

    LPC_EEPROM->INTENSET = ((0x1 << END_OF_RDWR) | (0x1 << END_OF_PROG));

    UEZSemaphoreCreateBinary(&p->iIRQReadySem);

    p->iInitialized = ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_NXP_LPC43xx_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read bytes out of the EEPROM.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to read
 *      TUInt8 *aBuffer             -- Pointer to buffer to receive data
 *      TUInt32 aNumBytes           -- Number of bytes to read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError EEPROM_NXP_LPC43xx_Read(
        void *aWorkspace,
        TUInt32 aAddress,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_EEPROM_NXP_LPC43xx_Workspace *p = (T_EEPROM_NXP_LPC43xx_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (!p->iInitialized)
        IInitialize(p);

    //LPC4357 allows for direct memory access to the EEPROM
    memcpy((void*)aBuffer, (void*)(EEPROM_START + aAddress), aNumBytes);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

static void GetPageStartandBytes(const TUInt32 aWriteAddress, TUInt32 *aAddress, TUInt32 *aBytesRemaining)
{
    TUInt32 page = 0;

    if(aWriteAddress % EEPROM_PAGE_SIZE){
        //not on a page boundary
        page = (aWriteAddress - EEPROM_START) / (EEPROM_PAGE_SIZE);
        *aAddress = ((EEPROM_PAGE_SIZE * page) + EEPROM_START);
        *aBytesRemaining = EEPROM_PAGE_SIZE - (aWriteAddress - ((EEPROM_PAGE_SIZE * page) + EEPROM_START));
    } else {
        //on a page boundary
        *aAddress = aWriteAddress;
        *aBytesRemaining = EEPROM_PAGE_SIZE;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_NXP_LPC43xx_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write bytes into the EEPROM
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aAddress            -- Base address into device to write
 *      TUInt8 *aBuffer             -- Pointer to buffer of data
 *      TUInt32 aNumBytes           -- Number of bytes to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError EEPROM_NXP_LPC43xx_Write(
        void *aWorkspace,
        TUInt32 aAddress,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_EEPROM_NXP_LPC43xx_Workspace *p = (T_EEPROM_NXP_LPC43xx_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 size = aNumBytes;
    TUInt32 len;
    TUInt32 timeOut = 0xFFFF;
    TUInt32 startPageAddress;
    TUInt32 writeToAddress = EEPROM_START + aAddress;
    TUInt32 bytesInPage;
    TUInt32 bytesWritten = 0;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (!p->iInitialized){
        IInitialize(p);
    }

    if((aAddress + aNumBytes) > (EEPROM_START + (EEPROM_PAGE_SIZE * EEPROM_PAGE_NUM))){
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    GetPageStartandBytes(writeToAddress, &startPageAddress, &bytesInPage);

    while (size) {
        //Read page
        memcpy((void*)p->iPage, (void*)startPageAddress, EEPROM_PAGE_SIZE);

        //Modify Page
        if(size <= bytesInPage){
            memcpy((void*)(p->iPage + (EEPROM_PAGE_SIZE - bytesInPage)), (void*)aBuffer, size);
            size -= size;
        } else {
            len = bytesInPage;
            memcpy((void*)(p->iPage + (EEPROM_PAGE_SIZE - bytesInPage)), (void*)(aBuffer), len);
            size -= len;
            bytesWritten += len;
            aBuffer += len;
        }
        //Write back
        memcpy((void*)startPageAddress, p->iPage, EEPROM_PAGE_SIZE);
        GetPageStartandBytes(writeToAddress + bytesWritten, &startPageAddress, &bytesInPage);

        LPC_EEPROM->CMD = 6;

        while((LPC_EEPROM->INTSTAT & (1<<2)) == 0){
            timeOut--;
            if(timeOut == 0){
                error = UEZ_ERROR_TIMEOUT;
                break;
            }
        }
        LPC_EEPROM->INTSTATCLR = 1<<2;             // Clear Program int status bit

        // Wait the appropriate amount of time
        UEZTaskDelay(p->iConfig.iWriteTime);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError EEPROM_NXP_LPC43xx_SetConfiguration(
        void *aWorkspace,
        const T_EEPROMConfig *aConfig)
{
    T_EEPROM_NXP_LPC43xx_Workspace *p = (T_EEPROM_NXP_LPC43xx_Workspace *)aWorkspace;
    p->iConfig = *aConfig;
    return UEZ_ERROR_NONE;
}

void EEPROM_NXP_LPC43xx_Create(const char *aName)
{
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&EEPROM_NXP_LPC43xx_Interface,
            0,
            0);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_EEPROM EEPROM_NXP_LPC43xx_Interface = {
	{
		// Common device interface
	    "EEPROM:NXP:LPC43xx",
	    0x0100,
	    EEPROM_NXP_LPC43xx_InitializeWorkspace,
	    sizeof(T_EEPROM_NXP_LPC43xx_Workspace),
	},

    // Functions
    EEPROM_NXP_LPC43xx_SetConfiguration,
    EEPROM_NXP_LPC43xx_Read,
    EEPROM_NXP_LPC43xx_Write,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  EEPROM_NXP_LPC43xx_driver.c
 *-------------------------------------------------------------------------*/
