/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corp. and is
* only intended for use with Renesas products.  

* No other uses are authorized.

* This software is owned by Renesas Electronics Corp. and is 
* protected under the applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WAWRRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH
* WARRANTIES ARE EXPRESSLY DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER 
* RENESAS ELECTRONICS CORP. NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR AND DIRECT, INDIRECT, SPECIAL, INCIDENTAL
* OR COSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE,
* EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE
* POSSIBILITIES OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue availability of this software.
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
*******************************************************************************/
/* Copyright (C) 2010. Renesas Electronics Corp., All Rights Reserved         */
/******************************************************************************
* File Name     : Flash_API_RX600.c
* Version       : 1.42
* Device(s)     : R5F562N8
* Tool-Chain    : Renesas RX Standard Toolchain 1.0.1
* OS            : None
* H/W Platform  : YRDKRX62N
* Description   : Flash programming for the RX600 Group
* Limitations   : None
*******************************************************************************
* History : DD.MMM.YYYY   Version    Description
*         : 21.Dec.2009   1.00       First Release
*         : 13.Jan.2010   1.10       Made function names and variables RAPI compliant
*         : 11.Feb.2010   1.20       Fixed other RAPI issues and fixed I flag issue
*         : 29.Apr.2010   1.30       Added support for RX621/N Group. Moved most
*                                    device specific data to header file.
*         : 26.May.2010   1.40       Added support for RX62T Group
*         : 28.Jul.2010   1.41       Fixed bug when performing a blank check on an
*                                    entire data flash block.  Also declared 
*                                    functions not in the API as 'static'.
*          : 23.Aug.2010   1.42      Updated source to raise the coding standard, to
*                                    meet GSCE & RDK standards.
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Intrinsic functions of MCU */
#include <machine.h>
#include <uEZPlatform.h>

// NOTE: DO NOT OPTIMIZE THIS CODE with the Renesas RX Toolset!

/* Function prototypes and device specific info needed for Flash API */
#include "Flash_API_RX600.h"
#include "iodefine.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
/*  The number of ICLK ticks needed for 35us delay are calculated below */
#define    WAIT_tRESW (35*96.0)                    
/*  The number of ICLK ticks needed for 10us delay are calculated below */
#define    WAIT_t10USEC (10*96.0)        
/* This macro was defined in the Flash_API_RXxxx.h file and will 
   create an array of flash addresses called block_addresses.  It is
   used for getting the write addresses of flash blocks */
DEFINE_ADDR_LOOKUP

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* Signals whether FCU firmware has been transferred to the FCU RAM 
   0 : No, 1 : Yes */
uint8_t fcu_transfer_complete = 0;

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Valid values are 'READ_MODE','ROM_PE_MODE' or 'FLD_PE_MODE' */
uint8_t current_mode;         
/* Stores whether interrupts were enabled when flash operation was called */
static uint8_t psw_i_bit;             
/* Flash intialisation function prototype */
static uint8_t _Flash_Init(void);
/* Enter PE mode function prototype */
static uint8_t _Enter_PE_Mode( uint32_t flash_addr, uint16_t bytes );
/* Exit PE mode function prototype */
static void _Exit_PE_Mode(void);
/* ROM write function prototype */
static uint8_t _ROM_Write(uint32_t address, uint32_t data);
/* Data flash write function prototype */
static uint8_t _DataFlash_Write(uint32_t address, uint32_t data, uint8_t size);
/* Data flash status clear function prototype */
static void _DataFlash_Status_Clr(void);
/* Notify peripheral clock function prototype */
static uint8_t _Notify_Peripheral_Clock(volatile uint8_t * flash_addr);
/* FCU reset function prototype */
static void    _Flash_Reset(void);

/******************************************************************************
* Function Name: _Flash_Init
* Description  : Initializes the FCU peripheral block.
    NOTE: This function does not have to execute from in RAM.
* Arguments    : none
* Return Value : 
        0x00 = Operation Successful
        0x01 = Operation Failed
******************************************************************************/
static uint8_t _Flash_Init(void)
{
    /* Declare source and destination pointers */
    uint32_t * src, * dst;

    /* Declare iteration counter variable */
    uint16_t i;

    /* Disable FCU interrupts */
    FLASH.FRDYIE.BIT.FRDYIE = 0;

    /* Disable FCU interrupts in FCU block */
    FLASH.FAEINT.BIT.ROMAEIE = 0;
    FLASH.FAEINT.BIT.CMDLKIE = 0;
    FLASH.FAEINT.BIT.DFLAEIE = 0;
    FLASH.FAEINT.BIT.DFLRPEIE = 0;
    FLASH.FAEINT.BIT.DFLWPEIE = 0;

    /* Disable FCU interrupts in ICU */

    /* Disable flash interface error (FIFERR) */
    IPR(FCU, FIFERR) = 0;
    IEN(FCU, FIFERR) = 0;
    /* Disable flash ready interrupt (FRDYI) */
    IPR(FCU, FRDYI) = 0;
    IEN(FCU, FRDYI) = 0;

    /* Transfer Firmware to the FCU RAM. To use FCU commands, the FCU firmware
        must be stored in the FCU RAM. */

    /* Before writing data to the FCU RAM, clear FENTRYR to stop the FCU. */
    if (FLASH.FENTRYR.WORD != 0x0000)
    {
        /* Disable the FCU from accepting commands - Clear both the
           FENTRY0(ROM) and FENTRYD(Data Flash) bits to 0 */
        FLASH.FENTRYR.WORD = 0xAA00;
    }

    /* Enable the FCU RAM */
    FLASH.FCURAME.WORD = 0xC401;

    /* Copies the FCU firmware to the FCU RAM.
       Source: H'FEFFE000 to H'FF00000 (FCU firmware area)
       Destination: H'007F8000 to H'007FA000 (FCU RAM area) */
    /* Set source pointer */
    src = (uint32_t *)FCU_PRG_TOP;

    /* Set destination pointer */
    dst = (uint32_t *)FCU_RAM_TOP;

    /* Iterate for loop to copy the FCU firmware */
    for ( i=0; i<(FCU_RAM_SIZE/4); i++)
    {
        /* Copy data from the source to the destination pointer */
        *dst = *src;

        /* Increment the source and destination pointers */
        src++;
        dst++;
    }

    /* FCU firmware transfer complete, set the flag to 1 */
    fcu_transfer_complete = 1;

    /* Return 0, no errors */
    return 0;
}
/******************************************************************************
End of function  _Flash_Init
******************************************************************************/

/******************************************************************************
* Function Name: _DataFlash_Status_Clr
* Description  : Clear the status of the Data Flash operation.
    NOTE: This function does not have to execute from in RAM.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void _DataFlash_Status_Clr(void)
{
    /* Declare temporaty pointer */
    volatile uint8_t *ptrb;

    /* Set pointer to Data Flash to issue a FCU command if needed */
    ptrb = (uint8_t *)(DF_ADDRESS);

    /* Check to see if an error has occurred with the FCU.  If set, then
       issue a status clear command to bring the FCU out of the 
       command-locked state */
    if (FLASH.FSTATR0.BIT.ILGLERR == 1)
    {
        /* FASTAT must be set to 0x10 before the status clear command
           can be successfully issued  */
        if (FLASH.FASTAT.BYTE != 0x10)
        {
            /* Set the FASTAT register to 0x10 so that a status clear
               command can be issued */
            FLASH.FASTAT.BYTE = 0x10;
        }
    }

    /* Issue a status clear command to the FCU */
    *ptrb = 0x50;
}
/******************************************************************************
End of function  _DataFlash_Status_Clr
******************************************************************************/

/******************************************************************************
* Function Name: _DataFlash_Write
* Description  : Write either 8-bytes of 128-bytes to Data Flash area.
    NOTE: This function does not have to execute from in RAM.
* Arguments    :
    address
        The address (in the Data Flash programming area) to write the data to
    data
        The address of the data to write
    size
        The size of the data to write. Must be set to either '8' or '128'
* Return Value : 
        0x00 = Operation Successful
        0x01 = Operation Failed
******************************************************************************/
static uint8_t _DataFlash_Write(uint32_t address, uint32_t data, uint8_t size)
{
    /* Declare wait counter variable */
    int32_t wait_cnt;

    /* Define loop iteration count variable */
    uint8_t n = 0;

    /* Check data size is valid */
    if ((size == 128) || (size == 8))
    {
        /* Perform bit shift */
        size = size >> 1;

        /* Send command to data flash area */
        *(volatile uint8_t *)DF_ADDRESS = 0xE8;

        /* Specify data transfer size to data flash area */
        *(volatile uint8_t *)DF_ADDRESS = size;     

        /* Iterate through the number of data bytes */
        while (n++ < size)
        {
            /* Copy data from source address to destination area */
            *(volatile uint16_t *)address = *(uint16_t *)data;

            /* Increment data address by two bytes */
            data += 2;
        }

        /* Write the final FCU command for programming */
        *(volatile uint8_t *)(DF_ADDRESS) = 0xD0;

        /* Set the wait counter with timeout value */
        wait_cnt = WAIT_MAX_DF_WRITE;

        /* Check if FCU has completed its last operation */
        while (FLASH.FSTATR0.BIT.FRDY == 0)
        {
            /* Decrement the wait counter */
            wait_cnt--;

            /* Check if the wait counter has reached zero */
            if (wait_cnt == 0)
            {
                /* Maximum time for writing a block has passed, 
                   operation failed, reset FCU */
                _Flash_Reset();

                /* Return 1, operation failure */
                return 1;
            }
        }

        /* Check for illegal command or programming errors */
        if ((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR == 1))
        {
            /* Return 1, operation failure */
            return 1;    
        }
    }
    /* Data size is invalid */
    else
    {
        /*Return 1, operation failure */
        return 1;
    }

    /* Return 0, operation success */
    return 0;
}
/******************************************************************************
End of function  _DataFlash_Write
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashDataAreaBlankCheck
* Description  : 
    NOTE: This function does not have to execute from in RAM.
* Arguments    :
    address
        The address to check if is blank.
        If the parameter 'size'=='BLANK_CHECK_8_BYTE', this should be set to an
        8-byte address boundary.
        If the parameter 'size'=='BLANK_CHECK_ENTIRE_BLOCK', this should be set
        to a defined Data Block Number ('BLOCK_DB0', 'BLOCK_DB1', etc...)
    size
        This specifies if you are checking an 8-byte location, or an entire
        block. You must set this to either 'BLANK_CHECK_8_BYTE' or
        'BLANK_CHECK_ENTIRE_BLOCK'.
* Return Value : 
        0x00 = Blank
        0x01 = Not Blank
        0x02 = Operation Failed
******************************************************************************/
uint8_t R_FlashDataAreaBlankCheck(uint32_t address, uint8_t size)
{
    /* Declare data flash pointer */
    volatile uint8_t *ptrb;

    /* Declare result container variable */
    uint8_t result;

    /* Set current FCU mode to Data Flash PE Mode */
    current_mode = FLD_PE_MODE;

    /* Enter Data Flash PE mode in the FCU */
    if ( _Enter_PE_Mode( address, 0 ) != 0)
    {
        /* FCU failed to enter PE mode, set mode to READ */
        current_mode = READ_MODE;

        /* Return 2, operation failure */
        return 2;
    }

    /* Set  bit FRDMD (bit 4) in FMODR to 1 */
    FLASH.FMODR.BIT.FRDMD = 1;

    /* Check if 8 byte size has been passed */
    if (size == BLANK_CHECK_8_BYTE)
    {
        /* Set data flash pointer to beginning of the memory block */
        ptrb = (uint8_t *)(address & DF_MASK);    

        /* Check if the next 8 bytes are blank
           Bits BCADR to the address of the 8-byte location
           to check (only bits A3-A12).
           Set bit BCSIZE in EEPBCCNT to 0. */
        FLASH.DFLBCCNT.WORD = (uint16_t)(address & (DF_BLOCK_SIZE-8));
    }
    /* Check entire data block */
    else
    {
        /* Any address in the erasure block */
        ptrb = (uint8_t *)address;

        /* Check if the entire block is blank
           Set bit BCSIZE in EEPBCCNT to 1.
           Bits BCADR are not used, set to '0'. */
        FLASH.DFLBCCNT.WORD = 0x0001;
    }

    /* Send commands to FCU */
    *ptrb = 0x71;
    *ptrb = 0xD0;

    /* Check 'FRDY' bit */
    while ( FLASH.FSTATR0.BIT.FRDY == 0 )
    {
        /* Wait till ready */
    }

    /* Reset the FRDMD bit back to 0 */
    FLASH.FMODR.BIT.FRDMD = 0x00;

    /* Check if the 'ILGERR' was set during the command */
    if (FLASH.FSTATR0.BIT.ILGLERR == 1)    /* Check 'ILGERR' bit */
    {
        /* Take the FCU out of PE mode */
        _Exit_PE_Mode( );

        /* Set the FCU mode to READ */
        current_mode = READ_MODE;

        /* Return 2, operation failure */
        return 2;
    }

    /* (Read the 'BCST' bit (bit 0) in the 'DFLBCSTAT' register
       0=blank, 1=not blank */
    result = FLASH.DFLBCSTAT.BIT.BCST;    

    /* Take the FCU out of PE mode */
    _Exit_PE_Mode( );

    /* Set the current mode to READ */
    current_mode = READ_MODE;

    /* Return 1/0 (Not Blank/Blank), operation successful */
    return( result );
}
/******************************************************************************
End of function  R_FlashDataAreaBlankCheck
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashDataAreaAccess
* Description  : 
    This function is used to allow read and program permissions to the Data
    Flash areas.
    NOTE: This function does not have to execute from in RAM.
* Arguments    : 
    read_en_mask
      Bitmasked value. Bits 0-3 represents each Data Blocks 0-3 (respectively).
          '0'=no Read access. '1'=Allows Read by CPU
    write_en_mask
      Bitmasked value. Bits 0-3 represents each Data Blocks 0-3 (respectively).
          '0'=no Erase/Write access. '1'=Allows Erase/Write by FCU
* Return Value : none
******************************************************************************/
void R_FlashDataAreaAccess(uint16_t read_en_mask, uint16_t write_en_mask)
{


    /* Set Read access for the Data Flash blocks DB0-DB7 */
    FLASH.DFLRE0.WORD = 0x2D00 | (read_en_mask & 0x00FF);

    /* Set Read access for the Data Flash blocks DB8-DB15 */
    FLASH.DFLRE1.WORD = 0xD200 | ((read_en_mask >> 8) & 0x00FF);

    /* Set Erase/Program access for the Data Flash blocks DB0-DB7 */
    FLASH.DFLWE0.WORD = 0x1E00 |  (write_en_mask & 0x00FF);

    /* Set Erase/Program access for the Data Flash blocks DB8-DB15 */
    FLASH.DFLWE1.WORD = 0xE100 |  ((write_en_mask >> 8) & 0x00FF);
}

#if ENABLE_ROM_PROGRAMMING
    #pragma section FRAM
#endif
/******************************************************************************
End of function  R_FlashDataAreaAccess
******************************************************************************/

/******************************************************************************
* Function Name: _ROM_Write
* Description  : Write 256 bytes to ROM Area Flash.
    NOTE: This function MUST execute from in RAM.
* Arguments    : none
* Return Value : 
        0x00 = Operation Successful
        0x01 = Operation Failed
******************************************************************************/
static uint8_t _ROM_Write(uint32_t address, uint32_t data)
{
    /* Declare iteration loop count variable */
    uint8_t i;

    /* Declare wait counter variable */
    int32_t wait_cnt;

    /* Write the FCU Program command */
    *(volatile uint8_t *)address = 0xE8;    
    *(volatile uint8_t *)address = 0x80;

    /* Write 256 bytes into flash, 16-bits at a time */
    for (i = 0; i < 128; i++)
    {
        /* Copy data from source address to destination ROM */
        *(volatile uint16_t *)address = *(uint16_t *)data;

        /* Increment destination address by 2 bytes */
        data += 2;
    }

    /* Write the final FCU command for programming */
    *(volatile uint8_t *)address = 0xD0;

    /* Set timeout wait counter value */
    wait_cnt = WAIT_MAX_ROM_WRITE;

    /* Wait until FCU operation finishes, or a timeout occurs */
    while (FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if (wait_cnt == 0)
        {
            /* Maximum timeout duration for writing to ROM has elapsed - 
               assume operation failure and reset the FCU */
            _Flash_Reset();

            /* Return 1, operation failure */
            return 1;
        }
    }

    /* Check for illegal command or programming errors */
    if ((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR  == 1))
    {
        /* Return 1, operation failure */
        return 1;
    }

    /* Return 0, operation successful */
    return 0;
}
/******************************************************************************
End of function  _ROM_Write
******************************************************************************/

/******************************************************************************
* Function Name: _Enter_PE_Mode
* Description  : 
    NOTE: This function MUST execute from in RAM for 'ROM Area' programming,
    but if you are ONLY doing Data Flash programming, this function can reside
    and execute in Flash.
* Arguments    :     bytes
    The number of bytes you are writing (if you are writing).  This argument
    is used to determine if you are crossing the 1MB boundary in ROM.  If you
    do cross this boundary then FENTRY0 and FENTRY1 both need to be set.
* Return Value : 
        0x00 = Operation Successful
        0x01 = Operation Failed
******************************************************************************/
static uint8_t _Enter_PE_Mode( uint32_t flash_addr, uint16_t bytes )
{
    /* FENTRYR must be 0x0000 before bit FENTRY0 or FENTRYD can be set to 1 */
    FLASH.FENTRYR.WORD = 0xAA00;

    /* Execute null operations to allow FENTRYR register to settle */
    nop();
    nop();

    /* Check if FCU mode is set to ROM PE */
    if ( current_mode == ROM_PE_MODE)
    {
        /* See if interrupts were already disabled,    so as not to enable them
           when leaving. Store interrupt enabled flag locally */
        if ((get_psw() & I_FLAG) == 0)
        {
            psw_i_bit = 0;
        } else
        {
            psw_i_bit = 1;
        }

        /* Disable Interrupts because the interrupt vector table may not
            be accessible */
        set_psw(get_psw() & (~I_FLAG));

        /* Check which partition of flash this address is in */
        if (flash_addr >= 0x00F00000)
        {
            /* Enter ROM PE mode for addresses 0xFFF00000 - 0xFFFFFFFF */
            FLASH.FENTRYR.WORD = 0xAA01;
        } else
        {
            /* Enter ROM PE mode for addresses 0xFFE00000 - 0xFFEFFFFF */
            FLASH.FENTRYR.WORD = 0xAA02;
        }

    }
    /* Check if FCU mode is set to data flash PE */
    else if ( current_mode == FLD_PE_MODE )
    {
        /* Set FENTRYD bit(Bit 7) and FKEY (B8-15 = 0xAA) */
        FLASH.FENTRYR.WORD = 0xAA80;    

        /*  First clear the FCU's status before doing Data Flash programming.
            This is to clear out any previous errors that may have occured.
            For example, if you attempt to read the Data Flash area 
            before you make it readable using R_FlashDataAreaAccess(). */
        _DataFlash_Status_Clr();

    }
    /* Catch-all for invalid FCU mode */
    else
    {
        /* Invalid value of 'current_mode' */
        return 1;
    }

    /* Enable Write/Erase of ROM/Data Flash */
    FLASH.FWEPROR.BYTE = 0x01;

    /* Check for FCU error */
    if (        (FLASH.FSTATR0.BIT.ILGLERR == 1)
                ||    (FLASH.FSTATR0.BIT.ERSERR  == 1)
                ||    (FLASH.FSTATR0.BIT.PRGERR  == 1)
                ||    (FLASH.FSTATR1.BIT.FCUERR  == 1))
    {
        /* Enable Interrupts if I flag was set initially (ROM PE Mode only) */
        if (psw_i_bit == 1)
        {
            /* Restore original interrupt enable flag status */
            set_psw(get_psw() | (I_FLAG));
        }

        /* Return 1, operation failure */
        return 1;     
    }

    /* Return 0, operation successful */
    return 0;
}
/******************************************************************************
End of function _Enter_PE_Mode
******************************************************************************/

/******************************************************************************
* Function Name: _Exit_PE_Mode
* Description  : 
    NOTE: This function MUST execute from in RAM for 'ROM Area' programming,
    but if you are ONLY doing Data Flash programming, this function can reside
    and execute in Flash.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void _Exit_PE_Mode( void )
{
    /* Declare wait timer count variable */
    int32_t    wait_cnt;

    /* Declare address pointer */
    volatile uint8_t *pAddr;

    /* Set wait timer count duration */     
    wait_cnt = WAIT_MAX_ERASE;

    /* Iterate while loop whilst FCU operation is in progress */
    while (FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement wait timer count variable */
        wait_cnt--;                            

        /* Check if wait timer count value has reached zero */
        if (wait_cnt == 0)
        {
            /* Timeout duration has elapsed, assuming operation failure and
               resetting the FCU */
            _Flash_Reset();            

            /* Break from the while loop prematurely */
            break;
        }
    }

    /* Check FSTATR0 and execute a status register clear command if needed */
    if (        (FLASH.FSTATR0.BIT.ILGLERR == 1) 
                ||  (FLASH.FSTATR0.BIT.ERSERR  == 1) 
                ||  (FLASH.FSTATR0.BIT.PRGERR  == 1))
    {

        /* Clear ILGLERR */
        if (FLASH.FSTATR0.BIT.ILGLERR == 1)
        {
            /* FASTAT must be set to 0x10 before the status clear command
               can be successfully issued  */
            if (FLASH.FASTAT.BYTE != 0x10)
            {
                /* Set the FASTAT register to 0x10 so that a status clear
                    command can be issued */
                FLASH.FASTAT.BYTE = 0x10;
            }
        }

        /* Issue a status register clear command to clear all error bits */
        if ( current_mode == ROM_PE_MODE )
        {
            /* Set address pointer to ROM area */
            pAddr = (uint8_t *)ROM_PE_ADDR;

            /* Send status clear command to FCU */
            *pAddr = 0x50;
        } else
        {
            /* Set address pointer to data flash area */
            pAddr = (uint8_t *)DF_ADDRESS;

            /* Send status clear command to FCU */
            *pAddr = 0x50;
        }
    }

    /* Enter ROM Read mode */
    FLASH.FENTRYR.WORD = 0xAA00;                

    /* Flash write/erase disabled */
    FLASH.FWEPROR.BYTE = 0x02;

    /* Restore the imask value (ROM PE Mode only) */
    if ( current_mode == ROM_PE_MODE )
    {
        /* Enable Interrupts if I flag was set initially (ROM PE Mode only) */
        if (psw_i_bit == 1)
        {
            /* Restore original interrupt enable flag status */
            set_psw(get_psw() | (I_FLAG));
        }
    }

}
/******************************************************************************
End of function  _Exit_PE_Mode
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashErase
* Description  : Erases an entire flash block.
    NOTE: This function MUST execute from in RAM for 'ROM Area' programming,
    but if you are ONLY doing Data Flash programming, this function can reside
    and execute in Flash.

* Arguments    : 
    block
        The block number to erase (BLOCK_0, BLOCK_1, etc...)
* Return Value : 
        0x00 = Operation Successful
        0x01 = Operation Failed
******************************************************************************/
uint8_t R_FlashErase (uint8_t block)
{
    /* Declare address pointer */
    volatile uint8_t *pAddr;

    /* Declare erase operation result container variable */
    uint8_t result = 0;

    /* Declare timer wait count variable */
    int32_t wait_cnt;

    /* Erase Command Address */
    pAddr = (volatile uint8_t *)(block_addresses[ block ]);

    /* Check for FCU operation to complete */
    if ( !fcu_transfer_complete )
    {
        /* Initialise the FCU, and store operation resilt in result variable */
        result = _Flash_Init();    

        /* Check if FCU initialisation was successful */
        if ( result != 0x00 )
        {
            /* FCU initialisiation failed - return 1, operation failure */
            return 1;
        }
    }

    /* Make sure we are not in the middle of another operation */
    if ( current_mode != READ_MODE )
    {
        /* Currently in the middle of doing something else */
        return 1;     
    }

    /* Do we want to erase a Data Flash block or ROM block? */
    if ( block >= BLOCK_DB0 )
    {
        /* Set current FCU mode to data flash PE */
        current_mode = FLD_PE_MODE;
    } else
    {
        /* Set current FCU mode to ROM PE */
        current_mode = ROM_PE_MODE;
    }

    /* Enter ROM PE mode, check if operation successful */
    if ( _Enter_PE_Mode((uint32_t)pAddr, 0) != 0)
    {
        /* Enter PE mode operation failed, set current mode to READ */
        current_mode = READ_MODE;

        /* Return 1, operation failure */
        return 1;
    }

    /* Inform FCU of peripheral clock speed, check if operation is succesful */
    if ( _Notify_Peripheral_Clock(pAddr) != 0)
    {
        /* Notify operation failed, set FCU mode to READ */
        current_mode = READ_MODE;

        /* Return 1, operation failure */
        return 1;
    }

    /* Cancel the ROM Protect feature 
       NOTE: If you are using block locking feature to protect your data,
       remove the following line */
    FLASH.FPROTR.WORD = 0x5501;

    /* Send the FCU Command */
    *pAddr = 0x20;
    *pAddr = 0xD0;

    /* Set the wait counter timeout duration */
    wait_cnt = WAIT_MAX_ERASE;

    /* Wait while FCU operation is in progress */
    while (FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if (wait_cnt == 0)
        {

            /* Maximum timeout duration for erasing a ROM block has 
               elapsed, assuming operation failure - reset the FCU */
            _Flash_Reset();

            /* Return 1, operation failure */
            return 1;
        }
    }

    /* Check if erase operation was successful by checking 
       bit 'ERSERR' (bit5) and 'ILGLERR' (bit 6) of register 'FSTATR0' */
    /* Check FCU error */
    if ((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.ERSERR  == 1))
    {
        result = 1;
    }

    /* Leave Program/Erase Mode */
    _Exit_PE_Mode();

    /* Set FCU mode to READ */
    current_mode = READ_MODE;

    /* Return erase result, operation succesful */
    return result;
}
/******************************************************************************
End of function  R_FlashErase
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashWrite
* Description  : Writes bytes into flash.
    NOTE: This function MUST execute from in RAM for 'ROM Area' programming,
    but if you are ONLY doing Data Flash programming, this function can reside
    and execute in Flash.

* Arguments    : 
    flash_addr
        Flash address location to write to. This address must be on a 256 byte
        boundary.
    buffer_addr
        Address location of data buffer to write into flash.
    bytes
        The number of bytes to write. You must always pass a multiple of 256
        bytes for programming.
* Return Value : 
        0x00 = Operation Successful
        0x01 = Operation Failed
        0x02 = (ROM AREA) Number of bytes is not divisible by 256
        0x03 = (ROM AREA) Flash address not on a 256 byte boundary
        0x04 = (Data Flash AREA) Number of bytes not divisible by 8
        0x05 = (Data Flash AREA) Flash address not on 8 byte boundary
        0x06 = Invalid address
        0x07 = (ROM AREA) Cannot write over 1MB boundary at 0xFFF00000
******************************************************************************/
uint8_t R_FlashWrite ( uint32_t flash_addr, uint32_t buffer_addr,
                       uint16_t bytes )
{
    /* Declare result container and number of bytes to write variables */
    uint8_t result = 0, num_byte_to_write;

    /* Take off upper byte since for programming/erase addresses for ROM are 
        the same as read addresses except upper byte is masked off to 0's.
        Data Flash addresses are not affected. */
    flash_addr &= 0x00FFFFFF;

    /* ROM area or Data Flash area - Check for DF first */  
    if ( flash_addr >= DF_ADDRESS && flash_addr < DF_ADDRESS + DF_SIZE )
    {
        /* Check if the number of bytes were passed is a multiple of 8 */
        if ( bytes & 0x07 )
        {
            /* Return 4, number of bytes not a multiple of 8 */
            return 4;
        }
        /* Check for an address on a 8 byte page boundary. */
        if ( (uint16_t)flash_addr & 0x07)
        {
            /* Return 5, address not on an 8 byte boundary */
            return 5;
        }
    }
    /* Check for ROM area */
    else if (    (flash_addr >= ROM_PE_ADDR)
                 &&    (flash_addr < ROM_PE_ADDR + ROM_SIZE) )
    {
        /* Check if the number of bytes were passed is a multiple of 256 */
        if ( bytes & 0xFF )
        {
            /* Return 2, number of bytes not a multiple of 256 */
            return 2;
        }

        /* Check for an address on a 256 byte page */
        if ( (uint16_t)flash_addr & 0xFF)
        {
            /* Return 3, address not on a 256 byte boundary */
            return 3;    
        }

        /* Check for passing over 1MB boundary at 0xFFF00000 */
        if (        (flash_addr < (0x00F00000))
                    &&    ((flash_addr + bytes) > 0x00F00000) )
        {
            /* Return 7, cannot write across a 1MB boundary */
            return 7;
        }
    } else
    {
        /* Return 6, invalid flash address */
        return 6;
    }

    /* If FCU firmware has already been transferred to FCU RAM,
        no need to do it again */
    if ( !fcu_transfer_complete )
    {
        /* If FCU firmware has not been transferred,
            then transfer from FCU ROM to FCU RAM. */
        result = _Flash_Init();    
    }

    /* Make sure we are not in the middle of another operation */
    if ( current_mode != READ_MODE )
    {
        /* Flash operation in progress - return 1, operation failure */
        return 1;
    }

    /* Do we want to program a DF area or ROM area? */
    if ( flash_addr < block_addresses[ROM_NUM_BLOCKS-1] )
    {
        /* Set current FCU mode to data flash PE */
        current_mode = FLD_PE_MODE;
    } else
    {
        /* Set FCU to ROM PE mode */
        current_mode = ROM_PE_MODE;
    }

    /* Enter PE mode, check if operation is successful */
    if ( _Enter_PE_Mode( flash_addr, bytes ) != 0)
    {
        /* Mode change failure, set FCU mode to READ */
        current_mode = READ_MODE;

        /* Return 1, operation failure */
        return 1;
    }

    /* Cancel the ROM Protect feature 
       NOTE: If you are using block locking feature to protect your data,
       remove the following line */
    FLASH.FPROTR.WORD = 0x5501;

    /* Check if FCU mode is set to data flash PE */
    if ( current_mode == FLD_PE_MODE )
    {
        /* Iterate while there are still bytes remaining to write */
        while ( bytes )
        {
            /* Check if address is on a 128 byte boundary, and there are
               at least 128 bytes remaining to write */
            if ( (bytes >= 128) && (((uint8_t)flash_addr & 0x7F) == 0x00))
            {
                /* Set the next data transfer size to be 128 */
                num_byte_to_write = 128;
            } else
            {    /* Set the next data transfer size to be 8 */
                num_byte_to_write = 8;
            }

            /* Call the Programming function, store the operation status in the
               container variable, result */
            result = _DataFlash_Write( flash_addr, buffer_addr,
                                       num_byte_to_write);

            /* Check the container variable result for errors */
            if ( result != 0x00 )
            {
                /* Data flash write error detected, break from flash write
                   while loop prematurely */
                break;
            }

            /* Increment the flash address and the buffer address by the size
               of the transfer thats just completed */
            flash_addr += num_byte_to_write;
            buffer_addr += num_byte_to_write;

            /* Decrement the number of bytes remaining by the size of the last
               flash write */
            bytes -= num_byte_to_write;
        }
    }
    /* Catch-all - FCU mode must be ROM PE */
    else
    {
        /* Iterate loop while there are still bytes remaining to write */
        while ( bytes )
        {
            /* Call the Programming function */
            result = _ROM_Write(flash_addr, buffer_addr);

            /* Check for errors */
            if ( result != 0x00 )
            {
                /* Stop programming */
                break;    
            }

            /* Increment the flash address and the buffer address by the size
               of the transfer thats just completed */
            flash_addr += 256;
            buffer_addr += 256;

            /* Decrement the number of bytes remaining by the size of the last
               flash write */        
            bytes -= 256;
        }
    }

    /* Leave Program/Erase Mode */
    _Exit_PE_Mode();

    /* Set current FCU mode to READ */
    current_mode = READ_MODE;

    /* Return flash programming result */
    return result;
}
/******************************************************************************
End of function  R_FlashWrite
******************************************************************************/

/******************************************************************************
* Function Name: _Notify_Peripheral_Clock
* Description  : Notifies FCU or Peripheral Clock (PCLK)
    NOTE: This function MUST execute from in RAM for 'ROM Area' programming,
    but if you are ONLY doing Data Flash programming, this function can reside
    and execute in Flash.
* Arguments    : 
    flash_addr
        Flash address you will be erasing or writing to
* Return Value : 
        0x00 = Operation Successful
        0x01 = Operation Failed
******************************************************************************/
static uint8_t _Notify_Peripheral_Clock(volatile uint8_t * flash_addr)
{
    /* Declare wait counter variable */
    int32_t wait_cnt;

    /* Notify Peripheral Clock(PCK) */
    /* Set frequency of PCK */
    FLASH.PCKAR.WORD = PCLK_FREQUENCY/1000000;            

    /* Execute Peripheral Clock Notification Commands */
    *flash_addr = 0xE9;            
    *flash_addr = 0x03;
    *(uint16_t far *)flash_addr = 0x0F0F;
    *(uint16_t far *)flash_addr = 0x0F0F;
    *(uint16_t far *)flash_addr = 0x0F0F;
    *flash_addr = 0xD0;

    /* Set timeout wait duration */
    wait_cnt = WAIT_t10USEC;

    /* Check FRDY */
    while (FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement wait counter */
        wait_cnt--;                                

        /* Check if wait counter has reached zero */
        if (wait_cnt == 0)
        {
            /* Timeout duration elapsed, assuming operation failure - Reset
               FCU */
            _Flash_Reset();                        

            /* Return 1, operation failure  */
            return 1;
        }
    }

    /* Check ILGLERR */
    if (FLASH.FSTATR0.BIT.ILGLERR == 1)
    {
        /* Return 1, operation failure*/
        return 1;
    }

    /* Return 1, operation success */
    return 0;
}
/******************************************************************************
End of function _Notify_Peripheral_Clock
******************************************************************************/

/******************************************************************************
* Function Name: _Flash_Reset
* Description  : Reset the FCU.
    NOTE: This function MUST execute from in RAM for 'ROM Area' programming,
    but if you are ONLY doing Data Flash programming, this function can reside
    and execute in Flash.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void _Flash_Reset(void)
{
    /* Declare wait counter variable */
    int32_t    wait_cnt;

    /* Reset the FCU */
    FLASH.FRESETR.WORD = 0xCC01;

    /* Give FCU time to reset */
    wait_cnt = WAIT_tRESW;

    /* Wait until the timer reaches zero */
    while (wait_cnt != 0)
    {
        /* Decrement the timer count each iteration */
        wait_cnt--;
    }

    /* FCU is not reset anymore */
    FLASH.FRESETR.WORD = 0xCC00;
}
/******************************************************************************
End of function  _Flash_Reset
******************************************************************************/
void R_Exit_Programming(void){
	_Exit_PE_Mode();
}
void R_Flash_Reset(void){
	_Flash_Reset();
}	
uint8_t R_Flash_Init(void){
	return(_Flash_Init());
}
T_uezError R_Flash_Status_Check(uint32_t wait_cnt){
	/* Check if FCU has completed its last operation. If not, wait till timeout*/
        while (FLASH.FSTATR0.BIT.FRDY == 0)
        {
            /* Decrement the wait counter */
            wait_cnt--;

            /* Check if the wait counter has reached zero */
            if (wait_cnt == 0)
            {
                /* Maximum time for writing a block has passed, 
                   operation failed, reset FCU */
                R_Flash_Reset();

                /* Return 1, operation failure */
                return UEZ_ERROR_INTERNAL_ERROR;
            }
        }

        /* Check for illegal command or programming errors */
        if ((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR == 1))
        {
            /* Return 1, operation failure */
            return UEZ_ERROR_INTERNAL_ERROR;
        }
		return UEZ_ERROR_NONE;	
}
