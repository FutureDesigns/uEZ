/*-------------------------------------------------------------------------*
 * File:  EEPROM_NXP_LPC1788_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the EEPROM for the LPC1788
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
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <HAL/Interrupt.h>
#include "EEPROM_NXP_LPC1788.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define LPC1788_EEPROM_PAGE_SIZE    64 // bytes

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_EEPROM *iDevice;
    T_uezSemaphore iSem;
    TUInt8 iPage[64];
    T_EEPROMConfig iConfig;
    TBool iInitialized;
    T_uezSemaphore iIRQReadySem;
    volatile int end_of_prog;
    volatile int end_of_rdwr;
} T_EEPROM_NXP_LPC1788_Workspace;

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

static T_EEPROM_NXP_LPC1788_Workspace *G_lpc1788EEPROMWorkspace;

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_NXP_LPC1788_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for LPC1788.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError EEPROM_NXP_LPC1788_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_EEPROM_NXP_LPC1788_Workspace *p = (T_EEPROM_NXP_LPC1788_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    // Setup some standard settings in case they are not setup
    p->iConfig.iCommSpeed = 100; // not really needed
    p->iConfig.iPageSize = 64;
    p->iConfig.iWriteTime = 25;
    p->iInitialized = EFalse;
    
    // Record the global
    G_lpc1788EEPROMWorkspace = p;

    return error;
}

static void IProcessIRQ(T_EEPROM_NXP_LPC1788_Workspace *p)
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
    IProcessIRQ(G_lpc1788EEPROMWorkspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  IInitialize
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the EEPROM workspace.
 * Inputs:
 *      T_EEPROM_NXP_LPC1788_Workspace *p -- Workspace
 *---------------------------------------------------------------------------*/
static void IInitialize(T_EEPROM_NXP_LPC1788_Workspace *p)
{
    uint32_t v;
    
    LPC_EEPROM->PWRDWN = 0x0;
    
    v = PROCESSOR_OSCILLATOR_FREQUENCY / 375000;
    LPC_EEPROM->CLKDIV = v;
    v = LPC_EEPROM->CLKDIV;
    
    v  = ((((PROCESSOR_OSCILLATOR_FREQUENCY / 1000000) * 15) / 1000) + 1);
    v |= (((((PROCESSOR_OSCILLATOR_FREQUENCY / 1000000) * 55) / 1000) + 1) << 8);
    v |= (((((PROCESSOR_OSCILLATOR_FREQUENCY / 1000000) * 35) / 1000) + 1) << 16);
    LPC_EEPROM->WSTATE = v;
    v = LPC_EEPROM->WSTATE;
    
    LPC_EEPROM->INTENSET = ((0x1 << END_OF_RDWR) | (0x1 << END_OF_PROG));
    
    /* Enable the KFLASH Interrupt */
    InterruptRegister(
        KFLASH_IRQn,
        IFlashIRQHandler,
        INTERRUPT_PRIORITY_NORMAL,
        "KFlash");
    InterruptEnable(KFLASH_IRQn);
    
    UEZSemaphoreCreateBinary(&p->iIRQReadySem);
    
    p->iInitialized = ETrue;
}

static void IReadPage(
        T_EEPROM_NXP_LPC1788_Workspace *p, 
        TUInt32 aAddress, 
        TUInt8 *aBuffer, 
        TUInt8 aSize)
{
    TUInt32 data, i;
    
    // Clear the interrupts
    LPC_EEPROM->INTSTATCLR = ((0x1 << END_OF_RDWR) | (0x1 << END_OF_PROG));

    // Reset the flag for ending read/write
//    p->end_of_rdwr = 0;
    
    // Start reading at the given address/offset
    LPC_EEPROM->ADDR = aAddress;
    
    // Determine the size of the read (0=8 bit, 1=16 bit, 2=32 bit)
    // And do a separate algorithm for each
    if (aSize == 0) {
        // Read bytes (only one used currently)
        LPC_EEPROM->CMD = RD_PFETCH|READ8; 
        for (i = 0; i < LPC1788_EEPROM_PAGE_SIZE; i++) {
            data = LPC_EEPROM->RDATA;
            aBuffer[i] = *(uint8_t *)&data;
        }
    } else if (aSize == 2) {
        // Read 32-bit words
        LPC_EEPROM->CMD = RD_PFETCH|READ32; 
        for (i = 0; i < (LPC1788_EEPROM_PAGE_SIZE/4); i++) {
            *((TUInt32 *)(aBuffer + (i * 4))) = LPC_EEPROM->RDATA;
        }
    } else {
        // Read 16-bit words
        LPC_EEPROM->CMD = RD_PFETCH|READ16; 
        for (i = 0; i < (LPC1788_EEPROM_PAGE_SIZE/2); i++) {
            data = LPC_EEPROM->RDATA;
            *((TUInt16 *)(aBuffer + (i * 2))) = *(TUInt16 *)&data;
        }
    }
}

static void IWritePage(
        T_EEPROM_NXP_LPC1788_Workspace *p,
        TUInt32 aAddress, 
        TUInt8 *aBuffer, 
        TUInt8 aSize)
{
    uint32_t data, i;
    
    LPC_EEPROM->INTSTATCLR = ((0x1 << END_OF_RDWR) | (0x1 << END_OF_PROG));
    
    if (aSize == 0) {
        LPC_EEPROM->CMD = WRITE8; 
    } else if (aSize == 2) {
        LPC_EEPROM->CMD = WRITE32; 
    } else {
        LPC_EEPROM->CMD = WRITE16; 
    }
    
    LPC_EEPROM->ADDR = aAddress;
    
    if (aSize == 0) {
        // Transfer in bytes (currently only one used)
	for (i = 0; i < LPC1788_EEPROM_PAGE_SIZE; i++) {
            *(TUInt8 *)&data = *(TUInt8 *)(aBuffer + i);
            LPC_EEPROM->WDATA = data;
	}
    } else if (aSize == 2) {
        // Transfer in 32-bit words
        for (i = 0; i < (LPC1788_EEPROM_PAGE_SIZE/4); i++) {
	    LPC_EEPROM->WDATA = *(TUInt32 *)(aBuffer + (i * 4));
	}
    } else {
        // Transfer in 16-bit words
        for (i = 0; i < (LPC1788_EEPROM_PAGE_SIZE/2); i++) {
	  *(TUInt16 *)&data = *(uint16_t *)(aBuffer + (i * 2));
	  LPC_EEPROM->WDATA = data;
	}
    }

    LPC_EEPROM->INTSTATCLR = ((0x1 << END_OF_RDWR) | (0x1 << END_OF_PROG));
    p->end_of_prog = 0;
    LPC_EEPROM->ADDR = aAddress;
    LPC_EEPROM->CMD = ERASE_PROG;
    while (p->end_of_prog == 0); // TBD: Needs timeout here!
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_NXP_LPC1788_Read
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
T_uezError EEPROM_NXP_LPC1788_Read(
        void *aWorkspace,
        TUInt32 aAddress,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_EEPROM_NXP_LPC1788_Workspace *p = (T_EEPROM_NXP_LPC1788_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 size = aNumBytes;
    TUInt32 len;
    const TUInt32 pageSize = LPC1788_EEPROM_PAGE_SIZE;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    
    if (!p->iInitialized)
        IInitialize(p);

    while (size) {
        // Determine number of bytes remaining in this page
        len = pageSize-((aAddress+pageSize)&(pageSize-1));
        // Clip to the remaining number of bytes
        if (len > size)
            len = size;

        // Read in a page
        IReadPage(p, aAddress&(~(LPC1788_EEPROM_PAGE_SIZE-1)), p->iPage, 0);
        
        // Copy over the part we need (offset by the address)
        memcpy(aBuffer, p->iPage+(aAddress & (LPC1788_EEPROM_PAGE_SIZE-1)), len);

        // Move everything up        
        aAddress += len;
        size -= len;
        aBuffer += len;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_NXP_LPC1788_Write
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
T_uezError EEPROM_NXP_LPC1788_Write(
        void *aWorkspace,
        TUInt32 aAddress,
        TUInt8 *aBuffer,
        TUInt32 aNumBytes)
{
    T_EEPROM_NXP_LPC1788_Workspace *p = (T_EEPROM_NXP_LPC1788_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 size = aNumBytes;
    TUInt32 len;
    const TUInt32 pageSize = LPC1788_EEPROM_PAGE_SIZE;
    TUInt32 addr;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (!p->iInitialized)
        IInitialize(p);

    while (size) {
        // Determine number of bytes remaining in this page
        len = pageSize-((aAddress+pageSize)&(pageSize-1));
        // Clip to the remaining number of bytes
        if (len > size)
            len = size;

        addr = aAddress & ~(LPC1788_EEPROM_PAGE_SIZE-1);
        // Read
        IReadPage(p, addr, p->iPage, 0);
        // Modify
        memcpy(p->iPage + (aAddress & (LPC1788_EEPROM_PAGE_SIZE-1)), aBuffer, len);
        // Write
        IWritePage(p, addr, p->iPage, 0);
        
        // Wait the appropriate amount of time
        UEZTaskDelay(p->iConfig.iWriteTime);

        aAddress += len;
        size -= len;
        aBuffer += len;
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError EEPROM_NXP_LPC1788_SetConfiguration(
        void *aWorkspace,
        const T_EEPROMConfig *aConfig)
{
    T_EEPROM_NXP_LPC1788_Workspace *p = (T_EEPROM_NXP_LPC1788_Workspace *)aWorkspace;
    p->iConfig = *aConfig;
    return UEZ_ERROR_NONE;
}

void EEPROM_NXP_LPC1788_Create(const char *aName)
{
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&EEPROM_NXP_LPC1788_Interface,
            0,
            0);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_EEPROM EEPROM_NXP_LPC1788_Interface = {
	{
		// Common device interface
	    "EEPROM:NXP:LPC1788",
	    0x0100,
	    EEPROM_NXP_LPC1788_InitializeWorkspace,
	    sizeof(T_EEPROM_NXP_LPC1788_Workspace),
	},
	
    // Functions
    EEPROM_NXP_LPC1788_SetConfiguration,
    EEPROM_NXP_LPC1788_Read,
    EEPROM_NXP_LPC1788_Write,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  EEPROM_NXP_LPC1788_driver.c
 *-------------------------------------------------------------------------*/
