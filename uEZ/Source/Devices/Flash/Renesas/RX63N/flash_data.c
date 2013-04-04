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
* File Name     : flash_data.c
* Version       : 1.1
* Device(s)     : R5F562N8
* Tool-Chain    : Renesas RX Standard Toolchain 1.0.1
* OS            : None
* H/W Platform  : YRDKRX62N
* Description   : Defines flash and ADC functions used in this sample.
* Limitations   : None
*******************************************************************************
* History : DD.MMM.YYYY   Version    Description
*         : 08.Oct.2010   1.00       First release
*         : 02.Dec.2010   1.10       Second YRDK release
*******************************************************************************/

/*******************************************************************************
* Project Includes
*******************************************************************************/
/* Provides standard string function definitions used in this file */
#include <string.h>
/* Defines flash API functions used in this file */
#include "Flash_API_RX600.h"
/* Declares prototypes for functions defined in this file */
#include "flash_data.h"

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
 /* Imported global variable, list of block addresses array */
extern uint32_t block_addresses(uint32_t block);
/* Flash write buffer array */
uint8_t gFlashWriteBuffer[16];
/* Flash write address location global variable */
uint32_t gFlashWriteAddr;

/*******************************************************************************
 * Local Function Prototypes
 *******************************************************************************/
/* ADC initialisation function prototype declaration */
static void InitADC_FlashData(void);

/*******************************************************************************
* Outline      : Init_FlashData
* Description  : This function initialises the MCU flash area, allowing it to be
*                 read and written to by user code. The function then calls the
*                 InitADC_FlashData function to configure the ADC unit used in
*                 the project. Finally the function erases the contents of the
*                 data flash.
* Argument     : none
* Return value : none
*******************************************************************************/
void Init_FlashData(void)
{
    /* Enable MCU access to the data flash area */
    R_FlashDataAreaAccess(0xFFFF, 0xFFFF);

    /* Initialise the ADC unit */
    InitADC_FlashData();

    /* Erase flash memory before writing to it */
    Erase_FlashData();
}

/*******************************************************************************
* Outline      : Erase_FlashData
* Description  : This function enters a for loop, and erases a block of data
*                 flash memory each iteration until all blocks have been erased.
* Argument     : none
* Return value : none
*******************************************************************************/
void Erase_FlashData(void)
{
    /* Declare flash API error flag */
    uint8_t ret = 0;

    /* Declare loop count and block count variables */
    uint32_t loop_counter = 0, block_count = BLOCK_DB0;

    uint32_t address = 0x00000000;
	
	R_FlashDataAreaAccess(0xFFFF, 0xFFFF);

    /* Initialise a for loop to erase each of the data flash blocks */
    for (loop_counter = 0; loop_counter < DF_NUM_BLOCKS; loop_counter++) {
        /* Fetch beginning address of DF block */
        address = block_addresses(BLOCK_DB0 + loop_counter);

        /* Erase data flash block */
        ret |= R_FlashErase(block_count);

        /* Check if data flash area is blank */
        ret |= R_FlashDataAreaBlankCheck(address, BLANK_CHECK_ENTIRE_BLOCK);

        /* Increment block counter */
        block_count++;
    }

    /* Check Blank Checking */
    ret |= R_FlashDataAreaBlankCheck(address, BLANK_CHECK_ENTIRE_BLOCK);

    R_Flash_Status_Check(WAIT_MAX_DF_WRITE);

    /* Halt in while loop when flash API errors detected */
    while (ret);
}

/*******************************************************************************
* Outline      : Write_FlashData
* Description  : This function writes the contents of gFlashWriteBuffer to the
*                 dataflash, at the location pointed by gFlashWriteAddr. If the
*                 number of bytes to write to flash is not a multiple of 8, the
*                 data is padded with null bytes (0x00) to make up to the 
*                 nearest multiple of 8.
* Argument     : none
* Return value : none
*******************************************************************************/
void Write_FlashData(void)
{
	uint16_t num_bytes;
	
    /* Declare flash API error flag */
    uint8_t ret = 0;

    /* Declare data padding array and loop counter */
    uint8_t pad_buffer[256];

    /* Declare pointer to flash write location */

    uint32_t * flash_ptr = (uint32_t *)gFlashWriteAddr;;

    /* Declare the number of bytes variable, and initialise with the number of
     bytes the variable gFlashWriteBuffer contains */
    num_bytes = sizeof(gFlashWriteBuffer); // sizeof(gFlashWriteBuffer);

    /* Clear the contents of the flash write buffer array */
    memset(pad_buffer, 0x00, 256);

    /* Copy contents of the write buffer to the padding buffer */
    memcpy((char*)pad_buffer, (char*)gFlashWriteBuffer, num_bytes);

    /* Check if number of bytes is greater than 256 */
    if (num_bytes > 256) {
        /* Number of bytes to write too high, set error flag to 1 */
        ret |= 1;
    }
    /* Check if number of bytes to write is a multiple of 8 */
    else if (num_bytes % 8u) {
        /* Pad the data to write so it makes up to the nearest multiple of 8 */
        num_bytes += 8u - (num_bytes % 8u);
    }

    /* Halt in while loop when flash API errors detected */
    while (ret)
        ;

    /* Write contents of write buffer to data flash */
    ret |= R_FlashWrite(gFlashWriteAddr, (uint32_t)pad_buffer, num_bytes);

    /* Compare memory locations to verify written data */
    ret |=     R_FlashWrite
               (
               gFlashWriteAddr,
               (uint32_t)pad_buffer,
               num_bytes
               );

	/* Compare memory locations to verify written data */    
    ret |= memcmp(gFlashWriteBuffer, flash_ptr, num_bytes);

    /* Halt in while loop when flash API errors detected */
    while(ret);
}
/*******************************************************************************
 * End of function Write_FlashData
 *******************************************************************************/

/*******************************************************************************
 * Outline      : InitADC_FlashData
 * Description  : This function configures the ADC unit for single shot operation
 *                 and sets the switch callback function to CB_Switch.
 * Argument     : none
 * Return value : none
 *******************************************************************************/
static void InitADC_FlashData(void)
{
    /* Set the initial flash write address to the start of block DB0 */
    gFlashWriteAddr = DF_START_ADDR;
}
/*******************************************************************************
 * End of function InitADC_FlashData
 *******************************************************************************/
