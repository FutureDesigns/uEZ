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

#include "FlashLoader_UEZGUI_1788.h"
#include "framework/flash_loader.h" // The flash loader framework API declarations.

#ifdef DEBUG
  #include "uart.h"
#endif
/** local definitions **/
typedef struct SLayout {
  int sectors;
  int size;
} Layout;

//==============================================
//                512k flash, CortexM3
// LPC1788
//==============================================
const Layout flashLayout[] =
{
  {16,  4096},
  {14, 32768},
  { 0,     0}
};

const int allowedWriteSizes[] =
{
  4096,
  1024,
  512,
  256,
  0
};

//__root int Dummy @ ".DUMMY";

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/
static int ExecuteCommand(unsigned long* cmd, unsigned long* status);
static int CalculateSector(unsigned long addr);
/** public data **/

/** private data **/
const static IAP iap_entry = (IAP)kIAPentry; // MCU flash firmware interface function.
// The CPU clock speed (CCLK), the default value is used if no clock option is found.
__no_init static int clock;

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

static int ExecuteCommand(unsigned long* cmd, unsigned long* status)
{
  int ret;

#ifdef DEBUG
  printf("\r\niap_entry=%#x",iap_entry);
#endif

  for (;;)
  {
    iap_entry(cmd, status);
    ret = status[0];
    if (ret != STATUS_BUSY)
    {
      return ret;
    }
    // Try again if busy.
  }
}

// Returns the flash sector number for a given flash address.
// Returns -1 if the address is outside the flash.
static int CalculateSector(unsigned long addr)
{
  int i;
  int j;
  int sector = 0;
  unsigned long current = 0;

  for (i = 0; flashLayout[i].sectors; i++)
  {
    for (j = 0; j < flashLayout[i].sectors; j++)
    {
      if (addr < current + flashLayout[i].size)
      {
        return sector;
      }
      sector++;
      current += flashLayout[i].size;
    }
  }
  return -1;
}

/** public functions **/
#if USE_ARGC_ARGV
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[])
#else
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags)
#endif
{
#ifdef DEBUG
  UartInit(UART0_BASE);

  printf("\r\n====================");
  printf("\r\nFLASH CONFIGURE: base_of_flash=0x%x", (unsigned int)base_of_flash);
  printf("\r\nFLASH CONFIGURE: image_size=%u", image_size);
  printf("\r\nFLASH CONFIGURE: link_address=%u", link_address);
  printf("\r\nFLASH CONFIGURE: flags=%u", flags);
  printf("\r\nFLASH CONFIGURE: argc=%u", argc);
  printf("\r\nFLASH CONFIGURE: argv=0x%x",(unsigned int)(*argv));

  for(int i = 0; i < argc ; i++)
  {
    printf("\r\n");
    printf(argv[i]);
  }
#endif

  // The CPU clock speed (CCLK), the default value is used if no clock option is found.
  clock = CCLK;
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
uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
  int ret;
  unsigned int  Size ;
  unsigned long cmd[5];
  unsigned long status[3];

#ifdef DEBUG
    printf("\r\n====================");
    printf("\r\nFLASH WRITE: block_start=%#x, offset=%#x, count=%#x, buffer=%#x",
                             block_start, offset_into_block, count, buffer);
#endif

    do
    {
      for (unsigned int i = 0; allowedWriteSizes[i]; i++)
      {//take the max. allowed size
        Size = allowedWriteSizes[i];
        if (count >= allowedWriteSizes[i])
        {
          break;
        }
      }

    #ifdef INTERNAL_CRC_CALC
      unsigned int Crc;
      Crc = 0;
      for(unsigned int i = (unsigned int) block_start + offset_into_block;
          i < 0x1C;
          i+=4)
      {
        Crc -= *(unsigned int*)((unsigned int)buffer+i);
      }
      if (Crc)
      {
        *(unsigned int*)((unsigned int)buffer+0x1C) = Crc;
      }
    #endif

      cmd[0] = CMD_PREPARE_SECTORS;
      cmd[1] = CalculateSector((unsigned long)block_start + offset_into_block);
      cmd[2] = CalculateSector((unsigned long)block_start + offset_into_block + Size-1);


#ifdef DEBUG
      printf("\r\nPrepare Sectors %u - %u",cmd[1], cmd[2]);
#endif
      ret = ExecuteCommand(cmd, status);

      if (ret != STATUS_CMD_SUCCESS)
      {
#ifdef DEBUG
        printf("\r\nCMD_PREPARE_SECTORS failed!\r\n error:%u", ret);
#endif
        //message CMD_PREPARE_SECTORS failed.
        return ((uint32_t)-1);
      }

      cmd[0] = CMD_COPY_RAM_TO_FLASH;
      cmd[1] = (unsigned long)block_start + offset_into_block;
      cmd[2] = (unsigned long)buffer;
      cmd[3] = Size;
      cmd[4] = clock;

#ifdef DEBUG
      printf("\r\nWrite Page: dest addr=%#x, source addres=%#x, size=%#x",
                              cmd[1], cmd[2], cmd[3]);
#endif

      ret = ExecuteCommand(cmd, status);

      if (ret != STATUS_CMD_SUCCESS)
      {
#ifdef DEBUG
        printf("\r\nCMD_COPY_RAM_TO_FLASH failed!\r\n error:%u", ret);
#endif
        //message CMD_COPY_RAM_TO_FLASH failed.
        return ((uint32_t)-1);
      }

      if(count > cmd[3]) count -= cmd[3];
      else count = 0;

      offset_into_block += cmd[3];
      buffer += cmd[3];
    }
    while(count);

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
uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
  int ret;
  int start_sector;
  int end_sector;
  unsigned long cmd[4];
  unsigned long status[3];

#ifdef DEBUG
    printf("\r\n====================");
    printf("\r\nFLASH ERASE: block_start=%#x, block_size=%#x",
                             block_start, block_size);
#endif

    start_sector = CalculateSector((unsigned long)block_start);
    end_sector = CalculateSector((unsigned long)block_start+(block_size-1));

    // Prepare sector for erase.
    cmd[0] = CMD_PREPARE_SECTORS;
    cmd[1] = start_sector;
    cmd[2] = end_sector;

#ifdef DEBUG
    printf("\r\nPrapare Sectors %u - %u",cmd[1], cmd[2]);
#endif

    ret = ExecuteCommand(cmd, status);

   if (ret != STATUS_CMD_SUCCESS)
    {
#ifdef DEBUG
      printf("\r\nCMD_PREPARE_SECTORS failed!\r\n error:%u", ret);
#endif
      //message CMD_PREPARE_SECTORS failed.
      return((uint32_t)-1);
    }

    // Erase sector.
    cmd[0] = CMD_ERASE_SECTORS;
    cmd[1] = start_sector;
    cmd[2] = end_sector;
    cmd[3] = clock;

#ifdef DEBUG
    printf("\r\nErase Sectors %u - %u", cmd[1], cmd[2]);
#endif

    ret = ExecuteCommand(cmd, status);

    if (ret != STATUS_CMD_SUCCESS)
    {
#ifdef DEBUG
      printf("\r\nCMD_ERASE_SECTORS failed!\r\n error:%u", ret);
#endif
      //message CMD_ERASE_SECTORS failed.
      return((uint32_t)-1);
    }

#ifdef DEBUG
  printf("\r\n====================");
  printf("\r\nEXIT");
#endif
    return 0;
}
