/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corp. and is
* only intended for use with Renesas products.  

* No other uses are authorized.

* This software is owned by Renesas Electronics Corp. and is 
* protected under the applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
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
* File Name     : Flash_API_RX600.h
* Version       : 1.42
* Device(s)     : R5F562N8
* Tool-Chain    : Renesas RX Standard Toolchain 1.0.1
* OS            : None
* H/W Platform  : YRDKRX62N
* Description   : File declares Flash API prototypes and has device specific
*                 information need for Flash API use.
* Limitations   : None
*******************************************************************************
* History : DD.MMM.YYYY   Version    Description
*         : 21.Dec.2009   1.00       First Release
*         : 13.Jan.2010   1.10       Made function names and variables RAPI compliant
*         : 29.Apr.2010   1.20       Added support for RX621/N Group
*         : 26.May.2010   1.30       Added support for RX62T Group
*         : 28.Jul.2010   1.41       Fixed bug (in C file) when performing a blank 
*                                    check on an entire data flash block.  Also 
*                                    declared functions not in the API as 'static'.
*          : 23.Aug.2010   1.42      Updated source to raise the coding standard, to
*                                    meet GSCE & RDK standards.
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>
#include <uEZ.h>
#include <uEZProcessor.h>
/******************************************************************************
* Configuration Settings
******************************************************************************/


//#ifndef  ICLK_FREQUENCY
//    #define     ICLK_FREQUENCY  96  
//#endif

/*******************************************
 * SPECIFY YOUR PERIPHERAL CLOCK SPEED     *
 *******************************************
 * You need to specify the speed of your Peripheral clock (PCLK)
 * so that the correct programming speed can be calculated
 * for communicating to the flash controller
 * The value should be in MHz
 * Alternatively, you could define your Clock Speed in your project settings
 **************************************************************************/
/* Max Peripheral Clock for RX600 is 50MHz, but USB needs 48MHz */
//#ifndef PCLK_FREQUENCY
//	#define PCLK_FREQUENCY 48  
//#endif 

/******************************************************************************
 * ENABLE_ROM_PROGRAMMING
 ******************************************************************************
 * Set this to either '0' or '1'
 * When set to '1', code is included to program the User ROM program area of
 * Flash. Since this code must be executed from within RAM, the sections
 * 'PFRAM'(ROM) and 'RPFRAM'(RAM) must be added to the linker settings. Also
 * the linker option '-rom=PFRAM=RPFRAM' must be added. Finally, the 
 * initialization of the 'RPFRAM' must be added to the 'dbsct.c' file as such:
 *     { __sectop("PFRAM"), __secend("PFRAM"), __sectop("RPFRAM") }
 *
 **************************************************************************/
#define ENABLE_ROM_PROGRAMMING  0

/******************************************************************************
Function Prototypes
******************************************************************************/
void R_Exit_Programming(void);
void R_Flash_Reset(void);
uint8_t R_Flash_Init(void);
T_uezError R_Flash_Status_Check(uint32_t wait_cnt);
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
uint8_t R_FlashErase( uint8_t block );

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
uint8_t R_FlashWrite( uint32_t flash_addr,
                      uint32_t buffer_addr,
                      uint16_t bytes );


/* Data Flash Only Functions */

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
void R_FlashDataAreaAccess(uint16_t read_en_mask, uint16_t write_en_mask);

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
uint8_t R_FlashDataAreaBlankCheck(uint32_t address, uint8_t size);



/******************************************************************************
* Definitions - Use 'Configuration Settings' above to select these.
******************************************************************************/
/* Define read mode macro */
#define READ_MODE 0
/* Define ROM PE mode macro */
#define ROM_PE_MODE 1
/* Define data flash PE mode macro */
#define FLD_PE_MODE 2
/* 'size' parameter for R_FlashDataAreaBlankCheck */
#define BLANK_CHECK_8_BYTE       0    
/* 'size' parameter for R_FlashDataAreaBlankCheck */
#define BLANK_CHECK_ENTIRE_BLOCK 1  

/* This flag is used for setting/clearing the I bit in the PSW */
#define I_FLAG    (0x00010000)    

/* Below is memory information that is specific to device families (RX610,
   RX62N, etc...).  While memory sizes will differ between family members,
   the memory layout should remain the same */
   
/* Do not edit below this line unless you are editting for another RX with */
/* a different memory layout                                               */


    /*  According to HW Manual the Max Programming Time for 256 bytes (ROM)
        is 12ms.  This is with a PCLK of 50MHz. The calculation below
        calculates the number of ICLK ticks needed for the timeout delay.
        The 12ms number is adjusted linearly depending on the PCLK frequency.
    */
    #define    WAIT_MAX_ROM_WRITE \
            ((int32_t)(12000 * (50.0/48.0))*96.0)

    /*  According to HW Manual the Max Programming Time for 128 bytes
        (Data Flash) is 5ms.  This is with a PCLK of 50MHz. The calculation
        below calculates the number of ICLK ticks needed for the timeout delay.
        The 12ms number is adjusted linearly depending on the PCLK frequency.
    */
    #define    WAIT_MAX_DF_WRITE \
            ((int32_t)(5000 * (50.0/48.0))*96.0)


    /* FCU-RAM address define */
    /* FCU F/W Store Address */
    #define FCU_PRG_TOP        0xFEFFE000
    /* FCU RAM Address */
    #define FCU_RAM_TOP        0x007F8000
    /* FCU RAM Size */
    #define FCU_RAM_SIZE    0x2000


    /* User ROM Block Area           Size: Start Addr -   End Addr */
    #define BLOCK_0     0        /*    4KB: 0xFFFFF000 - 0xFFFFFFFF */
    #define BLOCK_1     1        /*    4KB: 0xFFFFE000 - 0xFFFFEFFF */
    #define BLOCK_2     2        /*    4KB: 0xFFFFD000 - 0xFFFFDFFF */
    #define BLOCK_3     3        /*    4KB: 0xFFFFC000 - 0xFFFFCFFF */
    #define BLOCK_4     4        /*    4KB: 0xFFFFB000 - 0xFFFFBFFF */
    #define BLOCK_5     5        /*    4KB: 0xFFFFA000 - 0xFFFFAFFF */
    #define BLOCK_6     6        /*    4KB: 0xFFFF9000 - 0xFFFF9FFF */
    #define BLOCK_7     7        /*    4KB: 0xFFFF8000 - 0xFFFF8FFF */
    #define BLOCK_8     8        /*   16KB: 0xFFFF4000 - 0xFFFF7FFF */
    #define BLOCK_9     9        /*   16KB: 0xFFFF0000 - 0xFFFF3FFF */
    #define BLOCK_10    10        /*   16KB: 0xFFFEC000 - 0xFFFEFFFF */
    #define BLOCK_11    11        /*   16KB: 0xFFFE8000 - 0xFFFEBFFF */
    #define BLOCK_12    12        /*   16KB: 0xFFFE4000 - 0xFFFE7FFF */
    #define BLOCK_13    13        /*   16KB: 0xFFFE0000 - 0xFFFE3FFF */
    #define BLOCK_14    14        /*   16KB: 0xFFFDC000 - 0xFFFDFFFF */
    #define BLOCK_15    15        /*   16KB: 0xFFFD8000 - 0xFFFDBFFF */
    #define BLOCK_16    16        /*   16KB: 0xFFFD4000 - 0xFFFD7FFF */
    #define BLOCK_17    17        /*   16KB: 0xFFFD0000 - 0xFFFD3FFF */
    #define BLOCK_18    18        /*   16KB: 0xFFFCC000 - 0xFFFCFFFF */
    #define BLOCK_19    19        /*   16KB: 0xFFFC8000 - 0xFFFCBFFF */
    #define BLOCK_20    20        /*   16KB: 0xFFFC4000 - 0xFFFC7FFF */
    #define BLOCK_21    21        /*   16KB: 0xFFFC0000 - 0xFFFC3FFF */
    #define BLOCK_22    22        /*   16KB: 0xFFFBC000 - 0xFFFBFFFF */
    #define BLOCK_23    23        /*   16KB: 0xFFFB8000 - 0xFFFBBFFF */
    #define BLOCK_24    24        /*   16KB: 0xFFFB4000 - 0xFFFB7FFF */
    #define BLOCK_25    25        /*   16KB: 0xFFFB0000 - 0xFFFB3FFF */
    #define BLOCK_26    26        /*   16KB: 0xFFFAC000 - 0xFFFAFFFF */
    #define BLOCK_27    27        /*   16KB: 0xFFFA8000 - 0xFFFABFFF */
    #define BLOCK_28    28        /*   16KB: 0xFFFA4000 - 0xFFFA7FFF */
    #define BLOCK_29    29        /*   16KB: 0xFFFA0000 - 0xFFFA3FFF */
    #define BLOCK_30    30        /*   16KB: 0xFFF9C000 - 0xFFF9FFFF */
    #define BLOCK_31    31        /*   16KB: 0xFFF98000 - 0xFFF9BFFF */
    #define BLOCK_32    32        /*   16KB: 0xFFF94000 - 0xFFF97FFF */
    #define BLOCK_33    33        /*   16KB: 0xFFF90000 - 0xFFF93FFF */
    #define BLOCK_34    34        /*   16KB: 0xFFF8C000 - 0xFFF8FFFF */
    #define BLOCK_35    35        /*   16KB: 0xFFF88000 - 0xFFF8BFFF */
    #define BLOCK_36    36        /*   16KB: 0xFFF84000 - 0xFFF87FFF */
    #define BLOCK_37    37        /*   16KB: 0xFFF80000 - 0xFFF83FFF */

    /* Data Flash Block Area         Size: Start Addr -   End Addr */
    #define BLOCK_DB0    38        /*    2KB: 0x00100000 - 0x001007FF */
    #define BLOCK_DB1    39        /*    2KB: 0x00100800 - 0x00100FFF */
    #define BLOCK_DB2    40        /*    2KB: 0x00101000 - 0x001017FF */
    #define BLOCK_DB3    41        /*    2KB: 0x00101800 - 0x00101FFF */
    #define BLOCK_DB4    42        /*    2KB: 0x00102000 - 0x001027FF */
    #define BLOCK_DB5    43        /*    2KB: 0x00102800 - 0x00102FFF */
    #define BLOCK_DB6    44        /*    2KB: 0x00103000 - 0x001037FF */
    #define BLOCK_DB7    45        /*    2KB: 0x00103800 - 0x00103FFF */
    #define BLOCK_DB8    46        /*    2KB: 0x00104000 - 0x001047FF */
    #define BLOCK_DB9    47        /*    2KB: 0x00104800 - 0x00104FFF */
    #define BLOCK_DB10   48        /*    2KB: 0x00105000 - 0x001057FF */
    #define BLOCK_DB11   49        /*    2KB: 0x00105800 - 0x00105FFF */
    #define BLOCK_DB12   50        /*    2KB: 0x00106000 - 0x001067FF */
    #define BLOCK_DB13   51        /*    2KB: 0x00106800 - 0x00106FFF */
    #define BLOCK_DB14   52        /*    2KB: 0x00107000 - 0x001077FF */
    #define BLOCK_DB15   53        /*    2KB: 0x00107800 - 0x00107FFF */

    /*  Bottom of DF Area */
    #define DF_ADDRESS      0x00100000
    /* Used for getting DF block */
    #define DF_MASK         0xFFFFF800
    /* Used for getting 8 byte boundary in DF block when
       doing blank checking */
    #define DF_BLOCK_SIZE   0x00000800
    /* Defines how many DF blocks are on this part */
    #define DF_NUM_BLOCKS   16
    /* Defines how many ROM blocks are on this part */
    #define ROM_NUM_BLOCKS  38
    /*  Bottom of User Flash Area */
    #define ROM_PE_ADDR     0x00F80000                                                  

    /* Array of flash addresses used for writing */
    #define DEFINE_ADDR_LOOKUP const uint32_t block_addresses[54] = { \
            /* Caution. ID CODE(FFFFFFA0-FFFFFFAF) is excluded. */ \
            0x00FFF000,  /* EB00 */ \
            0x00FFE000,  /* EB01 */ \
            0x00FFD000,  /* EB02 */ \
            0x00FFC000,  /* EB03 */ \
            0x00FFB000,  /* EB04 */ \
            0x00FFA000,  /* EB05 */ \
            0x00FF9000,  /* EB06 */ \
            0x00FF8000,  /* EB07 */ \
            0x00FF4000,  /* EB08 */ \
            0x00FF0000,  /* EB09 */ \
            0x00FEC000,  /* EB10 */ \
            0x00FE8000,  /* EB11 */ \
            0x00FE4000,  /* EB12 */ \
            0x00FE0000,  /* EB13 */ \
            0x00FDC000,  /* EB14 */ \
            0x00FD8000,  /* EB15 */ \
            0x00FD4000,  /* EB16 */ \
            0x00FD0000,  /* EB17 */ \
            0x00FCC000,  /* EB18 */ \
            0x00FC8000,  /* EB19 */ \
            0x00FC4000,  /* EB20 */ \
            0x00FC0000,  /* EB21 */ \
            0x00FBC000,  /* EB22 */ \
            0x00FB8000,  /* EB23 */ \
            0x00FB4000,  /* EB24 */ \
            0x00FB0000,  /* EB25 */ \
            0x00FAC000,  /* EB26 */ \
            0x00FA8000,  /* EB27 */ \
            0x00FA4000,  /* EB28 */ \
            0x00FA0000,  /* EB29 */ \
            0x00F9C000,  /* EB30 */ \
            0x00F98000,  /* EB31 */ \
            0x00F94000,  /* EB32 */ \
            0x00F90000,  /* EB33 */ \
            0x00F8C000,  /* EB34 */ \
            0x00F88000,  /* EB35 */ \
            0x00F84000,  /* EB36 */ \
            0x00F80000,  /* EB37 */ \
            0x00100000,  /* DB00 */ \
            0x00100800,  /* DB01 */ \
            0x00101000,  /* DB02 */ \
            0x00101800,  /* DB03 */ \
            0x00102000,  /* DB04 */ \
            0x00102800,  /* DB05 */ \
            0x00103000,  /* DB06 */ \
            0x00103800,  /* DB07 */ \
            0x00104000,  /* DB08 */ \
            0x00104800,  /* DB09 */ \
            0x00105000,  /* DB10 */ \
            0x00105800,  /* DB11 */ \
            0x00106000,  /* DB12 */ \
            0x00106800,  /* DB13 */ \
            0x00107000,  /* DB14 */ \
            0x00107800}; /* DB15 */       

    /* The max User ROM size on the RX621/N is 512KB */
    #define ROM_SIZE            524288
    /* The Data Flash size on the RX621/N is 32KB */
    #define DF_SIZE             32768

    /*  According to HW Manual the Max Erasure Time for a 16kB block is
        around 300ms.  This is with a PCLK of 50MHz. The calculation below
        calculates the number of ICLK ticks needed for the timeout delay.
        The 300ms number is adjusted linearly depending on the PCLK frequency.
    */
    #define WAIT_MAX_ERASE \
            ((int32_t)(300000 * (50.0/48.0))*96.0)     
    
