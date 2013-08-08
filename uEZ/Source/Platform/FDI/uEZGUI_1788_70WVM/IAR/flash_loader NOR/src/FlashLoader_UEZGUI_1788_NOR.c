/*************************************************************************
 *
 *   Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : FlashLoader_UEZGUI_1788.c
 *    Description : Flash loader driver for LPC1788.
 *    History :
 *    1. Date        : 31.1.2008 ã.
 *       Author      : Stoyan Choynev
 *       Description : initial verstion
 *
 *    Revision: Derivative version
 **************************************************************************/

/** include files **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <intrinsics.h>

#include "FlashLoader_UEZGUI_1788_NOR.h"
#include "framework/flash_loader.h" // The flash loader framework API declarations.
#include <uEZ.h>
#include <uEZFlash.h>
#include "Flash_S29GL064N90.h"
#include "LPC1788_BSP.h"

#ifdef DEBUG
//#include "uart.h"
#endif

T_Flash_S29GL064N90_Workspace G_flash;

//==============================================
//__root int Dummy @ ".DUMMY";

// The CPU clock speed (CCLK), the default value is used if no clock option is found.
//__no_init static int clock;

#if USE_ARGC_ARGV
/** private functions **/
static const char* FlFindOption(char* option, int with_value, int argc, char const* argv[])
{
    int i;

    for (i = 0; i < argc; i++)
    {
        if (strcmp(option, argv[i]) == 0)
        {
            if (with_value)
            {
                if (i + 1 < argc)
                return argv[i + 1]; // The next argument is the value.
                else
                return 0; // The option was found but there is no value to return.
            }
            else
            {
                return argv[i]; // Return the flag argument itself just to get a non-zero pointer.
            }
        }
    }
    return 0;
}
#endif // USE_ARGC_ARGV
void SystemInit(void)
{
    LPC1788_BSP_PinConfigInit();
    LPC1788_BSP_PLLConfigure();
    LPC1788_BSP_SDRAMInit();
    LPC1788_BSP_InitializeNORFlash();
}

/** public functions **/
#if USE_ARGC_ARGV
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
        uint32_t link_address, uint32_t flags,
        int argc, char const *argv[])
#else
uint32_t FlashInit(
        void *base_of_flash,
        uint32_t image_size,
        uint32_t link_address,
        uint32_t flags)
#endif
{
    extern void Flash_S29GL064N90_16Bit_Configure(
            T_Flash_S29GL064N90_Workspace *aWorkspace,
            void *aBaseAddr);
    SystemInit();

#ifdef DEBUG
    LPC1788_BSP_UartInit();

    printf("\r\n====================");
    printf("\r\nFLASH CONFIGURE: base_of_flash=0x%x", (unsigned int)base_of_flash);
    printf("\r\nFLASH CONFIGURE: image_size=%u", image_size);
    printf("\r\nFLASH CONFIGURE: link_address=%u", link_address);
    printf("\r\nFLASH CONFIGURE: flags=%u", flags);
#if USE_ARGC_ARGV
    printf("\r\nFLASH CONFIGURE: argc=%u", argc);
    printf("\r\nFLASH CONFIGURE: argv=0x%x",(unsigned int)(*argv));
    
    for(int i = 0; i < argc; i++)
    {
        printf("\r\n");
        printf(argv[i]);
    }
#endif
#endif

    // The CPU clock speed (CCLK), the default value is used if no clock option is found.
    //clock = CCLK;
#if USE_ARGC_ARGV
    const char* str;
    str = FlFindOption("--clock", 1, argc, argv);

    if (str)
    {
        clock = strtoul(str, 0, 0);
    }
#ifdef DEBUG
    printf("\r\nclock =%d",clock);
#endif

#ifdef DEBUG
    //Init Uart0 115200bps 8-N-1 @ 12MHz oscilator clock
    printf("\r\n====================");
    printf("\r\nFLASH INIT");
#endif
#endif // USE_ARGC_ARGV
#if __CORE__ == __ARM6M__
    *(unsigned int*)0x40048000 = 2;
#endif
    Flash_S29GL064N90_16Bit_InitializeWorkspace(&G_flash);
    //  Flash_S29GL064N90_16Bit_Configure(&G_flash, (void *)0x80000000);
    Flash_S29GL064N90_16Bit_Open(&G_flash);

#if 1 // testing
    //Flash_S29GL064N90_16Bit_Write(&G_flash, 0, "Hello...", 8);
    //Flash_S29GL064N90_16Bit_Close(&G_flash);
#endif 
    return 0;
}

uint32_t FlashSignoff(void)
{
#ifdef DEBUG
    printf("\r\n====================");
    printf("\r\nSIGNOFF");
#endif
    Flash_S29GL064N90_16Bit_Close(&G_flash);
    return 0;
}

/*************************************************************************
 * Function Name: FlashWrite
 * Parameters: block base address, offet in block, data size, ram buffer
 *             pointer
 * Return:
 *
 * Description
 *
 *************************************************************************/
uint32_t FlashWrite(
        void *block_start,
        uint32_t offset_into_block,
        uint32_t count,
        char const *buffer)
{
    //  int ret;
    T_uezError error;
    //  unsigned int  Size ;
    //  unsigned long cmd[5];
    //  unsigned long status[3];

#ifdef DEBUG
    printf("\r\n====================");
    printf("\r\nFLASH WRITE: block_start=%#x, offset=%#x, count=%#x, buffer=%#x",
            block_start, offset_into_block, count, buffer);
#endif

    /*
     cmd[0] = CMD_COPY_RAM_TO_FLASH;
     cmd[1] = (unsigned long)block_start + offset_into_block;
     cmd[2] = (unsigned long)buffer;
     cmd[3] = Size;
     cmd[4] = clock;
     */

#ifdef DEBUG
    printf("\r\nWrite Page: dest addr=%#x, source addres=%#x, size=%#x",
            ((unsigned long)block_start + offset_into_block) & 0x0FFFFFFF, 
            (unsigned long)buffer, count);
#endif

    error = Flash_S29GL064N90_16Bit_Write((void *)&G_flash,
            ((unsigned long)block_start + offset_into_block) & 0x0FFFFFFF,
            (TUInt8 *)buffer, count);
    //ret = ExecuteCommand(cmd, status);

    if (error != UEZ_ERROR_NONE) {
#ifdef DEBUG
        printf("\r\nCMD_COPY_RAM_TO_FLASH failed!\r\n error:%u", error);
#endif
        //message CMD_COPY_RAM_TO_FLASH failed.
        return ((uint32_t) - 1);
    }

    //      if(count > cmd[3])
    //          count -= cmd[3];
    //      else
    //          count = 0;

    //      offset_into_block += count;
    //      buffer += count;
    //    }
    //    while(count);

#ifdef DEBUG
    printf("\r\n====================");
    printf("\r\nEXIT");
#endif
    return 0;
}
/*************************************************************************
 * Function Name: FlashErase
 * Parameters:  Block Address, Block Size
 *
 * Return:
 *
 * Description:
 *
 *************************************************************************/
uint32_t FlashErase(void *block_start, uint32_t block_size)
{
    T_uezError error;
    //  int ret;
    //  int start_sector;
    //  int end_sector;
    //  unsigned long cmd[4];
    //  unsigned long status[3];

#ifdef DEBUG
    printf("\r\n====================");
    printf("\r\nFLASH ERASE: block_start=%#x, block_size=%#x",
            block_start, block_size);
#endif

    /*
     // Erase sector.
     cmd[0] = CMD_ERASE_SECTORS;
     cmd[1] = start_sector;
     cmd[2] = end_sector;
     cmd[3] = clock;
     */

#ifdef DEBUG
    //    printf("\r\nErase Sectors %u - %u", cmd[1], cmd[2]);
#endif

    error = Flash_S29GL064N90_16Bit_BlockErase(&G_flash,
            (((unsigned int)block_start) & 0x1FFFFFFF), block_size);

    if (error != UEZ_ERROR_NONE) {
#ifdef DEBUG
        printf("\r\nCMD_ERASE_SECTORS failed!\r\n error:%u", error);
#endif
        //message CMD_ERASE_SECTORS failed.
        return ((uint32_t) - 1);
    }

#ifdef DEBUG
    printf("\r\n====================");
    printf("\r\nEXIT");
#endif
    return 0;
}
