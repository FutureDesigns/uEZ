/*-------------------------------------------------------------------------*
 * File:  uEZPlatform.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the uEZGUI-4088-43WQR
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZGUI-4088-43WQN
 *  @{
 *  @brief     uEZGUI-4088-43WQN platform
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZGUI-4088-43WQN platform interface.
*/
#include <Config.h>
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <Device/LCD.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include <HAL/EMAC.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_USBDeviceController.h>
#include <Source/Devices/Accelerometer/Freescale/MMA7455/Freescale_MMA7455.h>
#include <Source/Devices/Accelerometer/ST/LIS3DH/ST_LIS3DH_I2C.h>
#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/AudioAmp/TI/LM48100/AudioAmp_LM48100.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Source/Devices/CRC/Generic/CRC_Generic.h>
#include <Source/Devices/DAC/Generic/DAC_Generic.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM_Generic_I2C.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM16_Generic_I2C.h>
#include <Source/Devices/EEPROM/NXP/LPC17xx_40xx/EEPROM_NXP_LPC17xx_40xx.h>
#include <Source/Devices/Flash/Spansion/S29GL/Flash_S29GL064N90_16bit.h>
#include <Source/Devices/Flash/NXP/LPC17xx_40xx/LPC17xx_40xx_IAP.h>
#include <Source/Devices/GPDMA/Generic/Generic_GPDMA.h>
#include <Source/Devices/GPIO/I2C/PCF8574T/GPIO_PCF8574T.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include <Source/Devices/I2C/Generic/Generic_I2C.h>
#include <Source/Devices/I2S/Generic/Generic_I2S.h>
#include <Source/Devices/LCD/Sharp/LQ043T1DG28/Sharp_LQ043T1DG28.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SPI.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_MCI.h>
#include <Source/Devices/MassStorage/USB_MS/USB_MS.h>
#include <Source/Devices/Network/GainSpan/Network_GainSpan.h>
#if (UEZ_ENABLE_TCPIP_STACK == 1)
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#endif
#include <Source/Devices/PWM/Generic/Generic_PWM.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/Serial/RS485/Generic/Generic_RS485.h>
#include <Source/Devices/SPI/Generic/Generic_SPI.h>
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/Temperature/NXP/LM75A/Temperature_LM75A.h>
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/Touchscreen/Generic/FourWireTouchResist/FourWireTouchResist_TS.h>
#include <Source/Devices/Touchscreen/Newhaven/FT5306DE4/FT5306DE4TouchScreen.h>
#include <Source/Devices/USBDevice/NXP/LPC17xx_40xx/LPC17xx_40xx_USBDevice.h>
#include <Source/Devices/USBHost/Generic/Generic_USBHost.h>
#include <Source/Devices/Watchdog/Generic/Watchdog_Generic.h>
#include <Source/Devices/Keypad/GPIO/Keypad_Generic_GPIO.h>
#include <Source/Devices/GPIO/I2C/PCF8574T/GPIO_PCF8574T.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_RTC.h>
#include <Source/Library/Web/BasicWeb/BasicWEB.h>
#include <Source/Library/FileSystem/FATFS/uEZFileSystem_FATFS.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Memory/MemoryTest/MemoryTest.h>
#include <Source/Library/StreamIO/StdInOut/StdInOut.h>
#include <Source/Processor/NXP/LPC17xx_40xx/uEZProcessor_LPC17xx_40xx.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_ADCBank.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_CRC.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_DAC.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_EMAC.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_EMC_Static.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_GPDMA.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_GPIO.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_I2C.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_I2S.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_LCDController.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_PWM.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_PLL.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_SDRAM.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_Serial.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_SSP.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_Timer.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_USBDeviceController.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_USBHost.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_Watchdog.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_MCI.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_UtilityFuncs.h>
#include <uEZADC.h>
#include <uEZAudioAmp.h>
#include <uEZBSP.h>
#include <uEZCRC.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include <uEZFile.h>
#include <uEZI2C.h>
#include <uEZMemory.h>
#include <uEZNetwork.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZStream.h>
#include <uEZPlatform.h>
#include <uEZTimer.h>
#include <Source/uEZSystem/uEZHandles.h>
#include <uEZPlatformAPI.h>
#include <uEZAudioMixer.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>

#include <Source/Devices/Flash/NXP/LPC_SPIFI_M4F/Flash_NXP_LPC_SPIFI_M4.h>
    
extern int32_t MainTask(void);

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NOR_FLASH_BASE_ADDR             0x80000000
#define PLATFORM_USES_NOR_FLASH         (0)//off for standard product
#define CONFIG_MEMORY_TEST_ON_SDRAM     0

#ifndef USING_43WQN_BA_REV1
#define USING_43WQN_BA_REV1             1 // set to 1 for 1.X revisions for I2C power fix
#endif

#ifndef UEZGUI_EXP_BRK_OUT
#define UEZGUI_EXP_BRK_OUT              0
#endif

#ifndef ENABLE_LCD_HSYCN_VSYNC
#define ENABLE_LCD_HSYCN_VSYNC          1 // must set to 1 to enable the HYSNC/VSYNC, otherwise pines are set to low
#endif

#ifndef LCD_BACKLIGHT_FREERTOS_TIMER
#define LCD_BACKLIGHT_FREERTOS_TIMER    1 // set to 0 here and in library project Config_Build.h to force using MCU timer peripheral for LCD backlight delay.
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_uezTask G_mainTask;

/*---------------------------------------------------------------------------*
 * Memory placement section:
 *---------------------------------------------------------------------------*/
//Allocate general purpose frames memory
#if (MAX_NUM_FRAMES > 0)
UEZ_PUT_SECTION(".frames", static TUInt8 _framesMemory [LCD_FRAMES_SIZE]);
TUInt8 *_framesMemoryptr = _framesMemory;
#else
UEZ_PUT_SECTION(".frames", static TUInt8 _framesMemory [4]);
TUInt8 *_framesMemoryptr = _framesMemory;
#endif

// See Users Manual UM10562 Section 38.6 Code Read Protection. If you set these to certain bytes JTAG will be disabled.
// In uEZ 2.13 all demo appications will set CRP1 bits high. (no protection)
// However, bootloader projects will also set CRP1 bits high. (no protection)
// If you change this here then rebuild both bootloader and application will set the same number still.
// If you modified CRP1 in the application, then flash the original bootloader on top of it the number will get reset to 0xFFFFFFFF. (no protection)

// For this LPC there is only one of these numbers. If you are using the uEZ bootloader then the CRP will be set in the
// the bootloader and it should NOT be generated in ANY application project output. If you do not have a bootloader then
// the CRP should be included in the application .hex output with the chosen number.

#if (DO_NOT_INCLUDE_LPC17XX40XX_CODE_READ_PROTECTION_1 == 1)
    // Create this define set to 1 in application code to allow for only setting CRP1 in a bootloader.
#else
UEZ_PUT_SECTION(".crp1", static const TUInt32 G_LPC17XX40XX_CRP1 = 0xFFFFFFFF);
TUInt32 *_crp1ptr = (TUInt32 * const)&G_LPC17XX40XX_CRP1;
#endif

/* On LPC1788 we want to keep the same tested AHB memory map even if 
 * MCI card or USB host isn't used. So create dummy memory in AHB for it.
 * It isn't a problem if Ethernet is turned off, so no array for it. */
#if (UEZ_ENABLE_USB_HOST_STACK != 1)
UEZ_PUT_SECTION(".usbhostmem", static TUInt8 G_usbHostMemory[8*1024]);
volatile TUInt8 *_usbMemoryptr = G_usbHostMemory;
#else // memory should already exist
#endif
// On-board MCI card should always be enabled on this uEZGUI.
  
/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

// Note that uEZBSP Delays are only accurate for around level 2 or medium optimization.
// With no optimization it will be about double the time. So we will always optimize them.
// For IAR application projects set the optimization level of this file to medium or high.
#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 120000000)
//#elif ( PROCESSOR_OSCILLATOR_FREQUENCY == 72000000) // TODO not validated yet.
#else
    #error "1 microSecond delay not defined for CPU speed"
#endif

// Note: To use a non-default max MCU frequency, can redefine UEZBSPDelay1US here.
// UEZBSDelay functions are now weakly defined in library for default frequency.
#if 0 // example for slower clock version of delay function
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_MED,
void UEZBSPDelay1US(void)
{
#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 72000000) // TODO not validated yet.
    nops50();
    nops10();
    nop();
    nop();
    nop();
#else
    #error "1 microSecond delay not defined for CPU speed"
#endif
})
#endif

#if (defined __GNUC__) // GCC
extern unsigned char __heap_start__[];
extern unsigned char __HEAPSIZE__[];
#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR
// Must declare these pragmas before below section placement if IAR (or to use section macro)
#pragma section = "RAM_region"
#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPSDRAMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the external SDRAM.
 *---------------------------------------------------------------------------*/
void UEZBSP_RAMInit(void)
{   // 7 ns is slowest speed rating supported/used. Driver only supports CAS 2. Some numbers would change for CAS 3.
    static const T_LPC17xx_40xx_SDRAM_Configuration sdramConfig_IS45S32400F_7 = { 
            UEZBSP_SDRAM_BASE_ADDR,
            UEZBSP_SDRAM_SIZE,
            SDRAM_CAS_2, // only 2 supported, this number ignored
            SDRAM_RAS_2, // only 2 supported, this number ignored
            LPC17xx_40xx_SDRAM_CLKOUT0,
            SDRAM_CLOCK_FREQUENCY,
            64, // ms
            4096, // cycles
            SDRAM_CYCLES(20), // tRP precharge to activate period - All used parts 18/20, which is same cycle number
            SDRAM_CYCLES(45), // tRAS min // Used parts 42/45. Same register value up to 49
            SDRAM_CYCLES(70), // tXSR min // 67-83 = same cycles, no parts used < 67, no used parts list SREX
            SDRAM_CYCLES(20), // tAPR/tRCD Active Command to Read Command // 17-33 = same cycles, no parts used < 18
            SDRAM_CLOCKS(4),  // tDAL Input Dat to ACT/REF (tCK units) // On existing parts this stays the same as long as CAS 2 is used.
            SDRAM_CLOCKS(2), // tWR = write recovery time = tCK + 1(depends on rise time) // Used parts have less than 20 cycles, 2 tCK is worst case.
            SDRAM_CYCLES(65), // tRC Row Cycle Time or Command Period // 50-66 are same same number
            SDRAM_CYCLES(64), // Refresh Cycle Time tRFC/tREF  // 50-66 are same same number
            SDRAM_CYCLES(70), // tXSR Exit Self-Refresh Time // 67-83 = same cycles, no parts used < 67
            SDRAM_CYCLES(14), // tRRD Row active to Row Active (or command period) // no parts use > 17, so same register setting
            SDRAM_CLOCKS(2)}; // tMRD Mode Register Set (to command) (tCK units) // Some parts are 14, but 16/2CK is worst case
    LPC17xx_40xx_SDRAM_Init_32BitBus(&sdramConfig_IS45S32400F_7);
    
#if CONFIG_MEMORY_TEST_ON_SDRAM
    MemoryTest(UEZBSP_SDRAM_BASE_ADDR, UEZBSP_SDRAM_SIZE);
#endif

// We shouldn't do the toolchain auto memory clear in SDRAM until after it is intialized.
// Currently in IAR/Crossworks projects we do it correctly after SDRAM init.
// Leave this here as example in case ASM memory clear isn't used or additional SDRAM memory clear needs to be performed.
#if (defined __GNUC__) // GCC
// In Crossworks we clear bss and heap in thumb_crt0.s after this function is called, so no need to do it here.
//memset((uint32_t*)__heap_start__, 0, // clear section by name example
//       (uint32_t )__HEAPSIZE__);
// We can also clear bss memory in RTOS mem clear function. We don't need to clear frames or emWin as that is handled in later routines.
#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR
// In IAR we currently clear SDRAM using __iar_program_start AFTER PLL and SDRAM intialized.
//memset((uint32_t*)__section_begin("RAM_region"), 0, // clear section by name example
//       (uint32_t )__section_size("RAM_region"));
#endif
//UEZBSP_CLEAR_RTOS_RAM();
//memset((uint32_t*)UEZBSP_SDRAM_BASE_ADDR, 0, // clear whole SDRAM using platform defines example
//     (uint32_t )UEZBSP_SDRAM_SIZE);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_ROMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the NOR Flash
 *---------------------------------------------------------------------------*/
void UEZBSP_ROMInit(void)
{
  // Dummy code to keep various memory location declarations from optimizing out
#if (DO_NOT_INCLUDE_LPC17XX40XX_CODE_READ_PROTECTION_1 == 1)
#else
   _crp1ptr = _crp1ptr;
#endif

#if PLATFORM_USES_NOR_FLASH
#if 1
    const T_LPC17xx_40xx_EMC_Static_Configuration norFlash_M29W128G = {
            UEZBSP_NOR_FLASH_BASE_ADDRESS,
            16*1024*1024,  // stock is 8 Megs, but setup for 16 Megs
            EMC_STATIC_MEMORY_WIDTH_16_BITS,
            EFalse,

            EMC_STATIC_CYCLES(0),
            EMC_STATIC_CYCLES(90 + 18),
            EMC_STATIC_CYCLES(25),
            EMC_STATIC_CYCLES(0),
            EMC_STATIC_CYCLES(90 + 4.9),

            1, };
    LPC17xx_40xx_EMC_Static_Init(&norFlash_M29W128G);
#else
    const T_LPC17xx_40xx_EMC_Static_Configuration norSlowFlash_M29W128G = {
        UEZBSP_NOR_FLASH_BASE_ADDRESS,
        EMC_STATIC_MEMORY_WIDTH_16_BITS,
        EFalse,

        0x0F,
        0x0F,
        0x1F,
        0x0F,
        0x1F,
        0x0F,
    };
    LPC17xx_40xx_EMC_Static_Init(&norSlowFlash_M29W128G);
#endif
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_PLLConfigure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the PLL
 *---------------------------------------------------------------------------*/
void UEZBSP_PLLConfigure(void)
{
#if (PROCESSOR_OSCILLATOR_FREQUENCY == 72000000)
    const T_LPC17xx_40xx_PLL_Frequencies freq_CPU72MHz_Peripheral60Mhz_USB48MHz = {
            12000000,

            // Run PLL0 at 120 MHz
            PROCESSOR_OSCILLATOR_FREQUENCY,

            // Run PLL1 at 48 MHz
            48000000,

            // Use the main oscillator of 12 MHz as a source
            LPC17xx_40xx_CLKSRC_SELECT_MAIN_OSCILLATOR,

            // Use PLL0 for the CCLK, PCLK, and EMC Clk source (before dividers)
            LPC17xx_40xx_CPU_CLOCK_SELECT_PLL_CLK,

            // Use PPL1 (alt) for the USB
            LPC17xx_40xx_USB_CLOCK_SELECT_ALT_PLL_CLK,

            // CPU Clock is PLL0 / 1 or 120 MHz / 1 = 120 MHz
            1,

            // PCLK is PLL0 / 2, or 60 MHz
            1,

            // EMC runs at PLL0 / 2, or 60 MHz
            1,

            // USB Clock = PLL1 / 1 (48 MHz / 1)
            1,

            // CLKOUT is on the EMC CLK and at 60 MHz (60 MHz / 1)
            LPC17xx_40xx_CLOCK_OUT_SELECT_CPU,
            1,
            ETrue, };

    LPC17xx_40xx_PLL_SetFrequencies(&freq_CPU72MHz_Peripheral60Mhz_USB48MHz);

#else // 120000000 MHz

    const T_LPC17xx_40xx_PLL_Frequencies freq_CPU120MHz_Peripheral60Mhz_USB48MHz = {
            12000000,

            // Run PLL0 at 120 MHz
            PROCESSOR_OSCILLATOR_FREQUENCY,

            // Run PLL1 at 48 MHz
            48000000,

            // Use the main oscillator of 12 MHz as a source
            LPC17xx_40xx_CLKSRC_SELECT_MAIN_OSCILLATOR,

            // Use PLL0 for the CCLK, PCLK, and EMC Clk source (before dividers)
            LPC17xx_40xx_CPU_CLOCK_SELECT_PLL_CLK,

            // Use PPL1 (alt) for the USB
            LPC17xx_40xx_USB_CLOCK_SELECT_ALT_PLL_CLK,

            // CPU Clock is PLL0 / 1 or 120 MHz / 1 = 120 MHz
            1,

            // PCLK is PLL0 / 2, or 60 MHz
            2,

            // EMC runs at PLL0 / 2, or 60 MHz
            2,

            // USB Clock = PLL1 / 1 (48 MHz / 1)
            1,

            // CLKOUT is on the EMC CLK and at 60 MHz (60 MHz / 1)
            LPC17xx_40xx_CLOCK_OUT_SELECT_CPU,
            1,
            ETrue, };

    LPC17xx_40xx_PLL_SetFrequencies(&freq_CPU120MHz_Peripheral60Mhz_USB48MHz);

#endif
}

void UEZBSP_HEARTBEAT_TOGGLE(void)
{ // Per manual can read set register to determine the set state.
    if((LPC_GPIO1->SET & (1 << 13)) == (1 << 13)){ // if on
      LPC_GPIO1->CLR |= (1 << 13); // set off
    } else {
      LPC_GPIO1->SET |= (1 << 13); // set on
    }
}

void UEZBSP_CLEAR_RTOS_RAM(void)
{
#if (defined __GNUC__) // GCC
extern unsigned char __bss_start__[];
extern unsigned char __bss_end__[];
  // clear BSS so RTOS stuff is actualy 0.
//  memset((void *)".bss", 0x0,
  memset((void *)__bss_start__, 0x0,
   __bss_end__-__bss_start__);
#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR
// TODO create new section for test variable that is outside kernel block
#pragma section = "__kernel_functions_block__"
//#pragma section = "_END"
// TODO test and verify on IAR
memset((void *)__section_begin("__kernel_functions_block__"), 0x0,
   __section_size("__kernel_functions_block__"));
#elif (defined __CC_ARM) // ARM RealView Compiler
#else
  #error "Early memory clear not implemented yet for this compiler."
#endif

}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_Pre_PLL_SystemInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Earliest platform init function
 *      Can call before PLL comes on. For example to set LED initial state.
 *---------------------------------------------------------------------------*/
void UEZBSP_Pre_PLL_SystemInit(void) {
  // PIO1_13 initial state is pull up mode.
  // Turn off LED before init clocks. 
  // Then it will only start blinking after RTOS

  // Configure status led to be fully in our control
  // Make P1.13 be a GPIO pin
  LPC_GPIO1->PIN &= (uint32_t)(~(3 << 3));
  LPC_GPIO1->CLR |= (1 << 13); // off
  // and an output pin
  LPC_GPIO1->DIR |= (1 << 13);
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
    switch(G_hardwareTest.iTestMode){
    case HARDWARE_TEST_CRYSTAL:
    
    /*
    for(uint8_t i = 0; i < 10; i++){
      UEZBSP_HEARTBEAT_TOGGLE();
      UEZBSPDelayMS(100);
      UEZBSP_HEARTBEAT_TOGGLE();
      UEZBSPDelayMS(100);
     }*/

      UEZPlatform_INIT_LOW_LEVEL_UART_DEFAULT_CLOCK();
      COM_Send("\r\n\n", 3);
      COM_Send("CRYSTAL Mode\n", 13);
      COM_Send("Turning On Oscillators...\n", 26);
    break;
    case HARDWARE_TEST_SDRAM:
      UEZBSP_CLEAR_RTOS_RAM();
      UEZPlatform_INIT_LOW_LEVEL_UART_DEFAULT_CLOCK();
      COM_Send("\r\n\n", 3);
      COM_Send("SDRAM Mode\n", 11);
      COM_Send("Turning On Oscillators...\n", 26);
    break;
    case HARDWARE_TEST_I2C:
    

      UEZPlatform_INIT_LOW_LEVEL_UART_DEFAULT_CLOCK();
      COM_Send("\r\n\n", 3);
      COM_Send("I2C Mode\n", 9);
      COM_Send("Turning On Oscillators...\n", 26);
    break;
    case HARDWARE_TEST_AUDIO:
    

    break;
    case HARDWARE_TEST_NORMAL:
    default:
    
    break;
  }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_CPU_PinConfigInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Immediately configure the port pins
 *      Called after PLL is working at rated speed
 *---------------------------------------------------------------------------*/
void UEZBSP_CPU_PinConfigInit(void)
{
    // Place any pin configuration that MUST be initially here (at power up
    // but before even SDRAM is initialized)
    // Can chage LED state here if troubleshooting SDRAM or want CLK OK signal 
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
  switch(G_hardwareTest.iTestMode){
    case HARDWARE_TEST_CRYSTAL:    
      UEZPlatform_INIT_LOW_LEVEL_UART();
      COM_Send("Oscillators Enabled...\n", 23);
/*    for(uint8_t i = 0; i < 10; i++) {
      UEZBSP_HEARTBEAT_TOGGLE();
        UEZBSPDelayMS(100);
        
      UEZBSP_HEARTBEAT_TOGGLE();
        UEZBSPDelayMS(100);
      }*/
      // TODO output clock on a pin somewhere?
      // TODO try to measure clocks using timer to verify accuracy?
    
      COM_Send("...OK\n", 6);
    
    break;
    case HARDWARE_TEST_SDRAM:
      UEZPlatform_INIT_LOW_LEVEL_UART();
      COM_Send("Oscillators Enabled...\n", 23);
      COM_Send("Turning on SDRAM...\n", 20);

    // TODO print serial port that RAM test is about to begin
      //COM_Send("TEST BEGING\n", 12);
    
    break;
    case HARDWARE_TEST_I2C:
      UEZPlatform_INIT_LOW_LEVEL_UART();
      COM_Send("Oscillators Enabled...\n", 23);
      COM_Send("Turning on SDRAM...\n", 20);
    
    break;
    case HARDWARE_TEST_AUDIO:
    
    break;
    case HARDWARE_TEST_NORMAL:
    default:
    
    break;
  }
#endif
}

#if (defined __GNUC__) // GCC
//(uint32_t*)&.frames)));

#define RAMTEST_START_ADDRESS ((const uint32_t) &".frames")
#define RAMTEST_LENGTH (const uint32_t)(((uint32_t)UEZBSP_SDRAM_SIZE) - RAMTEST_START_ADDRESS)

#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR
// Must declare these pragmas before below section placement if IAR
#pragma section = ".frames"
#define RAMTEST_START_ADDRESS &".frames"
#define RAMTEST_LENGTH (UEZBSP_SDRAM_SIZE-RAMTEST_START_ADDRESS)

#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_Post_SystemInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Perform any tests before things such as zero mem are performed.
 *      Called after external RAM and ROM are setup. (end of SystemInit)
 *---------------------------------------------------------------------------*/
void UEZBSP_Post_SystemInit(void)
{
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
  switch(G_hardwareTest.iTestMode){
    case HARDWARE_TEST_CRYSTAL:
    
    break;
    case HARDWARE_TEST_SDRAM:
      COM_Send("SDRAM RUNNING\n", 14);
      for(uint16_t i = 0; i < G_hardwareTest.iTestIterations; i++) {// perform RAM test X times    
        COM_Send("SDRAM TESTING...\n", 17);
        
        UEZBSP_HEARTBEAT_TOGGLE();
        MemoryTest(UEZBSP_SDRAM_BASE_ADDR, UEZBSP_SDRAM_SIZE);
        
        UEZBSP_HEARTBEAT_TOGGLE();
        COM_Send("SDRAM PASS\n", 11);
        UEZBSPDelayMS(100);
      }

    break;
    case HARDWARE_TEST_I2C:
    
    break;
    case HARDWARE_TEST_AUDIO:
    
    break;
    case HARDWARE_TEST_NORMAL:
    default:
    
    break;
  }
#endif

#if (UEZ_ENABLE_USB_HOST_STACK != 1)
  // Keep various memory location declarations from optimizing out
  _usbMemoryptr = _usbMemoryptr;
#else // memory should already exist
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_InterruptsReset
 *---------------------------------------------------------------------------*
 * Description:
 *      Do the first initialization of the interrupts.
 *---------------------------------------------------------------------------*/
void UEZBSP_InterruptsReset(void)
{
    InterruptsReset();
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
  switch(G_hardwareTest.iTestMode){
    case HARDWARE_TEST_CRYSTAL:
    
    break;
    case HARDWARE_TEST_SDRAM:
    
    break;
    case HARDWARE_TEST_I2C:
    
    break;
    case HARDWARE_TEST_AUDIO:
    
    break;
    case HARDWARE_TEST_NORMAL:
    default:
    
    break;
  }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  uEZPlatformInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the board with all the proper settings.
 *      Registers all peripherals specific to this board.
 *---------------------------------------------------------------------------*/
void uEZPlatformInit(void)
{
    // Do any initialiation necessary before the RTOS is started
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
  switch(G_hardwareTest.iTestMode){
    case HARDWARE_TEST_CRYSTAL:
    
    break;
    case HARDWARE_TEST_SDRAM:
    
    break;
    case HARDWARE_TEST_I2C:
    
    break;
    case HARDWARE_TEST_AUDIO:
    
    break;
    case HARDWARE_TEST_NORMAL:
    default:
    
    break;
  }
#endif

#if (configGENERATE_RUN_TIME_STATS == 1)
    UEZPlatform_Timer1_Require();
#endif
}

#if (configGENERATE_RUN_TIME_STATS == 1)
T_uezDevice G_StatsTimer;
T_uezTimerCallback G_StatsTimerCallback;
volatile configRUN_TIME_COUNTER_TYPE G_RunTimeStatsCounter;

void RtosStatsTimerCallback(T_uezTimerCallback *aWorkspace)
{
   PARAM_NOT_USED(aWorkspace);
   G_RunTimeStatsCounter++;
}
 
configRUN_TIME_COUNTER_TYPE UEZBSP_GetGetRunTimeStatsCounter(void)
{
  return G_RunTimeStatsCounter;
}

T_uezError UEZBSP_ConfigureTimerForRunTimeStats(void)
{
  T_uezError error = UEZ_ERROR_HANDLE_INVALID;
  G_RunTimeStatsCounter = 0;
  if (UEZTimerOpen("Timer1", &G_StatsTimer) == UEZ_ERROR_NONE) {
    G_StatsTimerCallback.iTimer = G_StatsTimer;
    G_StatsTimerCallback.iMatchRegister = 1;
    G_StatsTimerCallback.iTriggerSem = UEZ_NULL_HANDLE;
    G_StatsTimerCallback.iCallback = RtosStatsTimerCallback;
    if (UEZTimerSetupRegularInterval(G_StatsTimer, 1,
            (PROCESSOR_OSCILLATOR_FREQUENCY) / (20*1000),
            &G_StatsTimerCallback) == UEZ_ERROR_NONE) {
      error = UEZTimerSetTimerMode(G_StatsTimer, TIMER_MODE_CLOCK);
      error = UEZTimerReset(G_StatsTimer);
      InterruptSetPriority(TIMER1_IRQn, INTERRUPT_PRIORITY_LOW);
      UEZTimerEnable(G_StatsTimer);
    } else { // an error occurred opening Timer
    }
  }
  return error;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_FatalError
 *---------------------------------------------------------------------------*
 * Description:
 *      A fatal error in uEZ will come here last.  We must turn off
 *      interrupts and put the platform in a halted state with a blinking
 *      LED.
 * Inputs:
 *      int32_t aErrorCode -- Blink the LED a number of times equal to
 *          the error code.
 *---------------------------------------------------------------------------*/
void UEZBSP_FatalError(int32_t aErrorCode)
{
    register TUInt32 i;
    register TUInt32 count;
    
#if (DISABLE_FEATURES_FOR_BOOTLOADER == 1)
#else
//#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
    COM_Send("BSP_FATAL_ERROR!\n", 17);
//#endif
#endif
    // Ensure interrupts are turned off
    portDISABLE_INTERRUPTS();

    // Configure status led to be fully in our control
    // Make P1.13 be a GPIO pin
    LPC_GPIO1->PIN &= (uint32_t) (~(3 << 3));
    // and an output pin
    LPC_GPIO1->DIR |= (1 << 13);

    // Blink our status led in a pattern, forever
    count = 0;
    while (1) {
        LPC_GPIO1->SET |= (1 << 13); // on
        for (i = 0; i < 15000000; i++)
            nop();
        //__nop();//asm ( "nop" );
        LPC_GPIO1->CLR |= (1 << 13); // off
        for (i = 0; i < 15000000; i++)
            nop();
        //__nop();//asm ( "nop" );
        count++;
        if (count >= (uint32_t) aErrorCode) {
            // Long pause
            for (i = 0; i < 80000000; i++)
                nop();
            //__nop();//asm ( "nop" );
            count = 0;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C0_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the I2C0 exists in the HAL level
    LPC17xx_40xx_GPIO5_Require();
    LPC17xx_40xx_I2C0_Require(GPIO_P5_2, GPIO_P5_3);
    I2C_Generic_Create("I2C0", "I2C0", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C1 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C1_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the I2C1 exists in the HAL level
    LPC17xx_40xx_GPIO2_Require();
    LPC17xx_40xx_I2C1_Require(GPIO_P2_14, GPIO_P2_15);
    I2C_Generic_Create("I2C1", "I2C1", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C1_ALT_COM_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C1 device driver on the alternate set of pins
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C1_ALT_COM_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the I2C1 exists in the HAL level
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_I2C1_Require(GPIO_P0_0, GPIO_P0_1);
    I2C_Generic_Create("I2C1", "I2C1", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C2 device driver.  Usually only used when connecting
 *      to the external header.
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C2_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the I2C2 exists in the HAL level
    LPC17xx_40xx_GPIO4_Require();
    LPC17xx_40xx_I2C2_Require(GPIO_P4_20, GPIO_P4_21);
    I2C_Generic_Create("I2C2", "I2C2", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Temp0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Temperature0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Temp0_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C1_Require();
    Temp_NXP_LM75A_Create("Temp0", "I2C1", 0x92 >> 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Accel0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Accelerometer 0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Accel0_Require(void)
{
    T_uezDevice I2C;
    T_uezError error;
    I2C_Request r;
    TUInt8 dataIn;
    TUInt8 dataOut = 0x0F;

    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C1_Require();

    // Detect which accelerometer is loaded

    UEZI2COpen("I2C1", &I2C);
    r.iAddr = 0x18;
    r.iSpeed = 400; //kHz
    r.iWriteData = &dataOut;
    r.iWriteLength = 1; // send 1 byte
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = &dataIn;
    r.iReadLength = 1; // read 1 bytes
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

    error = UEZI2CTransaction(I2C, &r);

    if(!error && (dataIn == 0x33)) {
        ST_Accelo_LIS3DH_I2C_Create("Accel0", "I2C1");
    } else {
        Accelerometer_Freescale_MMA7455_I2C_Create("Accel0", "I2C1",
            MMA7455_I2C_ADDR);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_GPDMAx_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the General Purpose DMA drivers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_GPDMA0_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA0_Require();
    GPDMA_Generic_Create("GPDMA0", "GPDMA0");
}

void UEZPlatform_GPDMA1_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA1_Require();
    GPDMA_Generic_Create("GPDMA1", "GPDMA1");
}

void UEZPlatform_GPDMA2_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA2_Require();
    GPDMA_Generic_Create("GPDMA2", "GPDMA2");
}

void UEZPlatform_GPDMA3_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA3_Require();
    GPDMA_Generic_Create("GPDMA3", "GPDMA3");
}

void UEZPlatform_GPDMA4_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA4_Require();
    GPDMA_Generic_Create("GPDMA4", "GPDMA4");
}

void UEZPlatform_GPDMA5_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA5_Require();
    GPDMA_Generic_Create("GPDMA5", "GPDMA5");
}

void UEZPlatform_GPDMA6_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA6_Require();
    GPDMA_Generic_Create("GPDMA6", "GPDMA6");
}

void UEZPlatform_GPDMA7_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPDMA7_Require();
    GPDMA_Generic_Create("GPDMA7", "GPDMA7");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART0_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART0 on P0.2/P0.3
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART0_Require(GPIO_P0_2, GPIO_P0_3);
    Serial_Generic_FullDuplex_Stream_Create("UART0", "UART0",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_UART1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the raw UART1 driver.  No stream is attached.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_UART1_Require(void)
{
    // UART1 on P0.15/P0.16
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART1_Require(GPIO_P0_15, GPIO_P0_16, GPIO_NONE, GPIO_NONE,
            GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART1.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART1 on P0.15/P0.16
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART1_Require(GPIO_P0_15, GPIO_P0_16, GPIO_NONE, GPIO_NONE,
            GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART1", "UART1",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART2_Require
 *---------------------------------------------------------------------------*/
/**
 *  Setup the full duplex UART using UART2.
 *
 *  @param [in]     aWriteBufferSize    Size in bytes of outgoing buffer
 *  @param [in]     aReadBufferSize     Size in bytes of incoming buffer
 */
/*---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART0 on P0.10/P0.11
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART2_Require(GPIO_P0_10, GPIO_P0_11);
    Serial_Generic_FullDuplex_Stream_Create("UART2", "UART2",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART0 on P0.2/P0.3
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART3_Require(GPIO_P0_0, GPIO_P0_1);
    Serial_Generic_FullDuplex_Stream_Create("UART3", "UART3",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART4_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the full duplex UART using UART4.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART4_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART1 on P5.3(RXD)/P5.4(TXD)
    LPC17xx_40xx_GPIO5_Require();
    LPC17xx_40xx_UART4_Require(GPIO_P5_4, GPIO_P5_3);
    Serial_Generic_FullDuplex_Stream_Create("UART4", "UART4",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART.
 * Inputs:
 *      const char *aHALSerialName -- Name of UART serial driver to use
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    DEVICE_CREATE_ONCE();
    // NOTE: This require routine does NOT require the HAL driver
    // automatically!
    Serial_Generic_FullDuplex_Stream_Create("Console", aHALSerialName,
            aWriteBufferSize, aReadBufferSize);
    // Set standard output to console
    T_uezDevice stdInOut;
    UEZStreamOpen("Console", &stdInOut); // open device
    StdinRedirect(stdInOut); // set stdIn in library StdInOut.c
    StdoutRedirect(stdInOut); // set stdOut in library StdInOut.c
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_HalfDuplex_UART_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a half duplex UART.
 * Inputs:
 *      const char *aHALSerialName -- Name of UART serial driver to use
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *      T_uezGPIOPortPin aDriveEnablePortPin -- GPIO for drive enable
 *      TBool aDriveEnablePolarity -- ETrue for high true, else EFalse
 *      TUInt32 aDriveEnableReleaseTime -- Milliseconds to wait before
 *          changing drive enable direction.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_HalfDuplex_UART_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime)
{
    DEVICE_CREATE_ONCE();
    // NOTE: This require routine does NOT require the HAL driver
    // automatically!
    // NOTE: This require routine does NOT require the GPIO driver
    // needed for the GPIO drive enable port pin!
    Serial_Generic_HalfDuplex_Stream_Create("Console", aHALSerialName,
            aWriteBufferSize, aReadBufferSize, aDriveEnablePortPin,
            aDriveEnablePolarity, aDriveEnableReleaseTime);
    // Set standard output to console
    T_uezDevice stdInOut;
    UEZStreamOpen("Console", &stdInOut); // open device
    StdinRedirect(stdInOut); // set stdIn in library StdInOut.c
    StdoutRedirect(stdInOut); // set stdOut in library StdInOut.c
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART0_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART0 on P0.2/P0.3
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART0_Require(GPIO_P0_2, GPIO_P0_3);
    UEZPlatform_Console_FullDuplex_UART_Require("UART0", aWriteBufferSize,
            aReadBufferSize);
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART1.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART1 on P0.15/P0.16
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART1_Require(GPIO_P0_15, GPIO_P0_16, GPIO_NONE, GPIO_NONE,
            GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
    UEZPlatform_Console_FullDuplex_UART_Require("UART1", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART2.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    PARAM_NOT_USED(aWriteBufferSize);
    PARAM_NOT_USED(aReadBufferSize);
    // UART2 on P0.10/P0.11
    LPC17xx_40xx_GPIO0_Require();
    //LPC17xx_40xx_UART2_Require(GPIO_P0_10, GPIO_P0_11);
    //UEZPlatform_Console_FullDuplex_UART_Require("UART2", aWriteBufferSize,
    //        aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART3.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART3 on P0.0/P0.1
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_UART3_Require(GPIO_P0_0, GPIO_P0_1);
    UEZPlatform_Console_FullDuplex_UART_Require("UART3", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_HalfDuplex_RS485_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the half-duplex RS485 driver for UART1 as the uEZGUI Console.
 * Inputs:
 *      char *aHALSerialName, -- Serial Port Name (
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *      T_uezGPIOPortPin aDriveEnablePortPin -- drive enable GPIO pin
 *      TBool aDriveEnablePolarity -- Drive enable polarity (ETrue = HIGH true)
 *      TUInt32 aDriveEnableReleaseTime -- Drive enable release time (in ms)
 *      T_uezGPIOPortPin aReceiveEnablePortPin -- Receive enable GPIO pin
 *      TBool aReceiveEnablePolarity -- Receive enable polarity (EFalse = LOW true)
 *      TUInt32 aReceiveEnableReleaseTime -- Receive enable release time (in ms)
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_HalfDuplex_RS485_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime,
        T_uezGPIOPortPin aReceiveEnablePortPin,
        TBool aReceiveEnablePolarity,
        TUInt32 aReceiveEnableReleaseTime)
{
    T_RS485_GenericHalfDuplex_Settings aSettings;
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_GPIO0_Require(); // UART1 on P0.15/P0.16
    LPC17xx_40xx_UART1_Require(GPIO_P0_15, GPIO_P0_16, GPIO_NONE, GPIO_NONE,
            GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
  
    aSettings.iSerialName = aHALSerialName; 
    aSettings.iQueueSendSize = aWriteBufferSize;
    aSettings.iQueueReceiveSize = aReadBufferSize;
    aSettings.iDriveEnable = aDriveEnablePortPin;
    aSettings.iDriveEnablePolarity = aDriveEnablePolarity;
    aSettings.iDriveEnableReleaseTime = aDriveEnableReleaseTime;
    aSettings.iReceiveEnable = aReceiveEnablePortPin;
    aSettings.iReceiveEnablePolarity = aReceiveEnablePolarity;
    aSettings.aReceiveEnableReleaseTime = aReceiveEnableReleaseTime;

    RS485_GenericHalfDuplex_Create("Console", &aSettings);
    // Set standard output to console
    T_uezDevice stdInOut;
    UEZStreamOpen("Console", &stdInOut); // open device
    StdinRedirect(stdInOut); // set stdIn in library StdInOut.c
    StdoutRedirect(stdInOut); // set stdOut in library StdInOut.c
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_ISPHeader_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console to be a full duplex UART on the ISP header
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_ISPHeader_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // Standard ISP header is on UART0
    UEZPlatform_Console_FullDuplex_UART0_Require(aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_Expansion_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console to be a full duplex UART on the expansion header
 *      NOTE: On expansion header
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_Expansion_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // Standard Expansion board serial console is on UART0
    UEZPlatform_Console_FullDuplex_UART0_Require(aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_ALT_PWR_COM_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console to be a full duplex UART on the ALT PWR COM header
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_ALT_PWR_COM_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // Standard Expansion board serial console is on UART3
    UEZPlatform_Console_FullDuplex_UART3_Require(aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SSP0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SSP0 to be a SPI driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_SSP0_Require(void)
{
    DEVICE_CREATE_ONCE();

    // SCK0 is P2.22, MISO0 is P2.26, MOSI0 is P2.27, SSEL0 is not used
    LPC17xx_40xx_GPIO2_Require();
    LPC17xx_40xx_SSP0_Require(GPIO_P2_22, GPIO_P2_26, GPIO_P2_27, GPIO_NONE);

    SPI_Generic_Create("SSP0", "SSP0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SSP1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SSP1 to be a SPI driver.
 *      NOTE: On expansion header
 *---------------------------------------------------------------------------*/
void UEZPlatform_SSP1_Require(void)
{
    DEVICE_CREATE_ONCE();

    // SCK1 is P0.7, MISO1 is P0.8, MOSI1 is P0.9, SSEL1 is P0.6
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_SSP1_Require(GPIO_P0_7, GPIO_P0_8, GPIO_P0_9, GPIO_P0_6);

    SPI_Generic_Create("SSP1", "SSP1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_Require();
    ADCBank_Generic_Create("ADC0", "ADC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_0 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_0_Require(GPIO_P0_23);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_1 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_1_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_1_Require(GPIO_P0_24);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_2 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_2_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_2_Require(GPIO_P0_25);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_3 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_3_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_3_Require(GPIO_P0_26);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_4_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_4 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_4_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_4_Require(GPIO_P1_30);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_5_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_5 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_5_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_5_Require(GPIO_P1_31);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_6_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_6 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_6_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_6_Require(GPIO_P0_12);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_7_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_7 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_7_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_ADC0_7_Require(GPIO_P0_13);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_DAC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the DAC0 device driver
 *      NOTE: Goes to P0.26_AD03_AOUT_RD3
 *---------------------------------------------------------------------------*/
void UEZPlatform_DAC0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_DAC0_Require(GPIO_P0_26);
    DAC_Generic_Create("DAC0", "DAC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Flash0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Flash0 device driver for the external NOR Flash
 *---------------------------------------------------------------------------*/
void UEZPlatform_Flash0_Require(void)
{
    DEVICE_CREATE_ONCE();
#if PLATFORM_USES_NOR_FLASH
    Flash_S29GL064N90_16Bit_Create("Flash0", NOR_FLASH_BASE_ADDR);
#else
    Flash_NXP_LPC_SPIFI_M4_Create("Flash0");
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_IAP_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the IAP flash device driver for the internal flash
 *---------------------------------------------------------------------------*/
void UEZPlatform_IAP_Require(void)
{
    DEVICE_CREATE_ONCE();
    Flash_NXP_LPC17xx_40xx_Create("IAP");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Backlight_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Backlight device driver for the LCD
 *---------------------------------------------------------------------------*/
void UEZPlatform_Backlight_Require(void)
{
    const T_backlightGenericPWMSettings settings = {
            "PWM0",
            0, // master register
            1, // backlight register (PWM0_1)
            UEZ_LCD_BACKLIGHT_FULL_PERIOD,
            UEZ_LCD_BACKLIGHT_FULL_PWR_ON,
            UEZ_LCD_BACKLIGHT_FULL_PWR_OFF,
            GPIO_NONE,
            EFalse
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_PWM0_1_Require(GPIO_P1_2);
    LPC17xx_40xx_GPIO4_Require();
    UEZGPIOLock(GPIO_P4_31);
    UEZGPIOOutput(GPIO_P4_31);
    
    //UEZGPIOClear(GPIO_P1_2);
    //UEZGPIOOutput(GPIO_P1_2);
    Backlight_Generic_PWMControlled_Create("Backlight", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_LCD_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD device driver and its backlight
 *---------------------------------------------------------------------------*/
void UEZPlatform_LCD_Require(void)
{
    const T_LPC17xx_40xx_LCDController_Pins pins = {
            GPIO_NONE,  // LCD_PWR -- GPIO controlled
            GPIO_P2_2,  // LCD_DCLK
            GPIO_P2_4,  // LCD_ENAB_M
#if (ENABLE_LCD_HSYCN_VSYNC == 1)
            GPIO_P2_3,  // LCD_FP
#else
            GPIO_NONE,  // VSYNC Not Used
#endif
            GPIO_NONE,  // LCD_LE
#if (ENABLE_LCD_HSYCN_VSYNC == 1)
            GPIO_P2_5,  // LCD_LP
#else
            GPIO_NONE,  // HSYNC Not Used
#endif

            {
            GPIO_NONE,  // LCD_VD0
            GPIO_NONE,  // LCD_VD1
            GPIO_P4_28, // LCD_VD2
            GPIO_P4_29, // LCD_VD3
            GPIO_P2_6,  // LCD_VD4
            GPIO_P0_10,  // LCD_VD5
            GPIO_P2_8,  // LCD_VD6
            GPIO_P2_9,  // LCD_VD7

            GPIO_NONE,  // LCD_VD8
            GPIO_NONE,  // LCD_VD9
            GPIO_P1_20, // LCD_VD10
            GPIO_P1_21, // LCD_VD11
            GPIO_P1_22, // LCD_VD12
            GPIO_P1_23, // LCD_VD13
            GPIO_P1_24, // LCD_VD14
            GPIO_P1_25, // LCD_VD15

            GPIO_NONE,  // LCD_VD16
            GPIO_NONE,  // LCD_VD17
            GPIO_P2_12, // LCD_VD18
            GPIO_P2_13, // LCD_VD19
            GPIO_P1_26, // LCD_VD20
            GPIO_P1_27, // LCD_VD21
            GPIO_P1_28, // LCD_VD22
            GPIO_P1_29, // LCD_VD23
            },

            GPIO_NONE,  // LCD_CLKIN

#if USING_43WQN_BA_REV1 // Make sure that power to I2C devices cannot be turned off on this revision
            GPIO_NONE,
#else       
            GPIO_P2_0, // P2.0 is power pin, GPIO controlled
#endif
            EFalse,
            0,
    };
    T_halWorkspace *p_lcdc;
    T_uezDeviceWorkspace *p_lcd;
    T_uezDevice backlight;
    T_uezDeviceWorkspace *p_backlight;
    extern const T_uezDeviceInterface *UEZ_LCD_INTERFACE_ARRAY[];

    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_GPIO1_Require();
    LPC17xx_40xx_GPIO2_Require();
    LPC17xx_40xx_GPIO4_Require();
#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
    // Don't use LPC timer driver. It is only used for 167 ms turn on/off on this LPC.
#else

 #if (LCD_BACKLIGHT_FREERTOS_TIMER==1)
    // We no longer require wasting the peripheral timer for backlight turn on delay
 #else
    UEZPlatform_Timer0_Require();
 #endif

#endif

#if (ENABLE_LCD_HSYCN_VSYNC == 0) // set VSYNC pin low
      UEZGPIOSetMux(GPIO_P2_3, 0);
      UEZGPIOClear(GPIO_P2_3);    
      UEZGPIOOutput(GPIO_P2_3);
      UEZGPIOLock(GPIO_P2_3); 
#endif
#if (ENABLE_LCD_HSYCN_VSYNC == 0) // set HSYNC pin low            
      UEZGPIOSetMux(GPIO_P2_5, 0);
      UEZGPIOClear(GPIO_P2_5);    
      UEZGPIOOutput(GPIO_P2_5);
      UEZGPIOLock(GPIO_P2_5); 
#endif

    LPC17xx_40xx_LCDController_Require(&pins);
    UEZPlatform_Backlight_Require();

    // Need to register LCD device and use existing LCD Controller
    UEZDeviceTableRegister(
            "LCD",
            (T_uezDeviceInterface *)UEZ_LCD_INTERFACE_ARRAY[UEZ_LCD_COLOR_DEPTH],
            0,
            &p_lcd);

    HALInterfaceFind("LCDController", &p_lcdc);
    UEZDeviceTableFind("Backlight", &backlight);
    UEZDeviceTableGetWorkspace(backlight, (T_uezDeviceWorkspace **)&p_backlight);
    ((DEVICE_LCD *)(p_lcd->iInterface))->Configure(
            p_lcd,
            (HAL_LCDController **)p_lcdc,
            LCD_DISPLAY_BASE_ADDRESS,
            (DEVICE_Backlight **)p_backlight);

//TODO: add create routine for the newhaven display
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Speaker_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Speaker device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_Speaker_Require(void)
{
    const T_ToneGenerator_Generic_PWM_Settings settings = {
            "PWM1",
            2, // PWM1[2]
            GPIO_P2_1,
            0, // 0 mux = GPIO
            1, // 1 mux = PWM1_2
    };
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_PWM1_2_Require(GPIO_P2_1);

    // Speaker is on PWM1[2]
    ToneGenerator_Generic_PWM_Create("Speaker", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_IRTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal RTC device driver.  Not compatible with the
 *      external RTC.
 *---------------------------------------------------------------------------*/
void UEZPlatform_IRTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_RTC_Require(ETrue);
    RTC_Generic_Create("RTC", "RTC");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_IRTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the external RTC device driver.  Not compatible with the
 *      internal RTC.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ERTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    RTC_PCF8563_Create("RTC", "I2C0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_RTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RTC device driver and check it for validity
 *---------------------------------------------------------------------------*/
void UEZPlatform_RTC_Require(void)
{
    T_uezDevice rtcDev;
    DEVICE_RTC **p_rtcDev;
    const T_uezTimeDate resetTD = {
        {   0, 0, 0}, // 12:00 midnight
        {   1, 1, 2009}, // Jan 1, 2009
    };
    T_uezError error;

    // Choose one:
    UEZPlatform_IRTC_Require();
    //UEZPlatform_ERTC_Require();

    // Make sure the RTC has valid data
    printf("Checking RTC ...\n");
    UEZDeviceTableFind("RTC", &rtcDev);
    UEZDeviceTableGetWorkspace(rtcDev, (T_uezDeviceWorkspace **)&p_rtcDev);
    error = (*p_rtcDev)->Validate(p_rtcDev, &resetTD);
    if (error == UEZ_ERROR_INVALID) {
        printf("RTC was invalid, reset to 01/01/2009 12:00:00\n");
    } else if (error != UEZ_ERROR_NONE) {
        printf("RTC error #%d!  Could not check validity!\n", error);
    } else {
        printf("RTC OK\n");
    }

#if 0 // Test code for doing Clock out
    // Make sure the clock out is disabled
    error = (*p_rtcDev)->SetClockOutHz(p_rtcDev, 32768);
    if (error == UEZ_ERROR_NONE) {
        printf("RTC Clock out disabled\n");
    } else {
        printf("RTC Clock out error #%d!\n", error);
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM_I2C_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the external EEPROM over I2C.
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_I2C_Require(void)
{
    const T_EEPROMConfig eeprom_config = {EEPROM_CONFIG_PCA24S08};

    DEVICE_CREATE_ONCE();
    UEZPlatform_I2C1_Require();
    EEPROM16_Generic_I2C_Create("EEPROM0", "I2C1", 0xA0>>1, &eeprom_config);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM_I2C_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal EEPROM in the LPC17xx_40xx
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_LPC17xx_40xx_Require(void)
{
    DEVICE_CREATE_ONCE();
    EEPROM_NXP_LPC17xx_40xx_Create("EEPROM0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the default EEPROM0 driver (usually internal LPC17xx_40xx)
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM0_Require(void)
{
    // Pick one of these two for the type of EEPROM
    UEZPlatform_EEPROM_LPC17xx_40xx_Require();
    //UEZPlatform_EEPROM_I2C_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Watchdog_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal watchdog.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Watchdog_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_Watchdog_Require();
    Watchdog_Generic_Create("Watchdog", "Watchdog");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortA_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port A (expansion board).
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortA_Require(void)
{
    const T_LPC17xx_40xx_USBHost_Settings portASettings = {
            GPIO_P0_29,     // USB_D+1          = P0.29_USB1_DP_EINT0
            GPIO_P0_30,     // USB_D-1          = P0.30_USB1_DM_EINT1
            GPIO_NONE,      // USB_CONNECT1n
            GPIO_NONE,      // USB_UP_LED1
            GPIO_NONE,      // USB_INT1n
            GPIO_NONE,      // USB_SCL1
            GPIO_NONE,      // USB_SDA1
            GPIO_NONE,      // USB_TX_E
            GPIO_NONE,      // USB_TX_DP
            GPIO_NONE,      // USB_TX_DM
            GPIO_NONE,      // USB_RCV
            GPIO_NONE,      // USB_RX_DP
            GPIO_NONE,      // USB_RX_DM
            GPIO_NONE,      // USB_LS1
            GPIO_NONE,      // USB_SSPND1
            GPIO_P1_19,     // USB_PPWR1        = USB1H_PPWR
            GPIO_NONE,      // USB_PWRD1        = USB1H_PWRD
            GPIO_NONE,      // USB_OVRCR1       = USB1H_OVC
            GPIO_NONE,      // USB_HSTEN1
    };
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_USBHost_PortA_Require(&portASettings);
    USBHost_Generic_Create("USBHost", "USBHost:PortA");
    // NOTE: USB1H_PWRD is on GPIO_P4_26 and is manually controlled
    // NOTE: USB1H_OVR  is on GPIO_P4_27 and is manually controlled
    LPC17xx_40xx_GPIO4_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortB_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port B.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortB_Require(void)
{
//    GPIO_P1_30,     // USB_VBUS2        = USB2_VBUS
//    GPIO_P0_31,     // USB_D+2          = USB2_DP
//    GPIO_NONE,      // USB_D-2          = USB2_DM (not a port pin)
//    GPIO_P0_14,     // USB_CONNECT2n    = USB2_CON
//    GPIO_P0_13,     // USB_UP_LED2      = P0.13_USB2_UPLED_AD0.7
    const T_LPC17xx_40xx_USBHost_Settings portBSettings = {
            GPIO_P0_31,     // USB_D+2          = P0.29_USB1_DP_EINT0
            GPIO_NONE,       // USB_D-2          = P0.30_USB1_DM_EINT1
            GPIO_P0_14,      // USB_CONNECT2n
            GPIO_P0_13,      // USB_UP_LED2
            GPIO_NONE,      // USB_INT2n
            GPIO_NONE,      // USB_SCL2
            GPIO_NONE,      // USB_SDA2
            GPIO_NONE,      // USB_TX_E
            GPIO_NONE,      // USB_TX_DP
            GPIO_NONE,      // USB_TX_DM
            GPIO_NONE,      // USB_RCV
            GPIO_NONE,      // USB_RX_DP
            GPIO_NONE,      // USB_RX_DM
            GPIO_NONE,      // USB_LS2
            GPIO_NONE,      // USB_SSPND2
            GPIO_NONE,      // USB_PPWR
            GPIO_NONE,      // USB_PWRD2
            GPIO_NONE, // GPIO_P0_27, (cannot go here)     // USB_OVRCR2       = USB2H_OVC
            GPIO_NONE,      // USB_HSTEN2
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_USBHost_PortB_Require(&portBSettings);
    USBHost_Generic_Create("USBHost", "USBHost:PortB");
    // NOTE: USB1H_PWRD is on GPIO_P4_26 and is manually controlled
    // NOTE: USB1H_OVR  is on GPIO_P4_27 and is manually controlled
    //LPC17xx_40xx_GPIO4_Require();
    // P1.30 needs to be VBUS (USB controlled)
    UEZGPIOSetMux(GPIO_P1_30, 2); // disabled
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_Require(void)
{
    UEZPlatform_USBHost_PortA_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FileSystem_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the File System using FATFS
 *---------------------------------------------------------------------------*/
void UEZPlatform_FileSystem_Require(void)
{
    T_uezDevice dev_fs;

    DEVICE_CREATE_ONCE();

    FileSystem_FATFS_Create("FATFS");
    UEZDeviceTableFind("FATFS", &dev_fs);
    UEZFileSystemInit();
    UEZFileSystemMount(dev_fs, "/");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS0 using USBHost flash drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS0_Require(void)
{
    DEVICE_CREATE_ONCE();
    MassStorage_USB_Create("MS0", "USBHost");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MCI_DefaultFreq
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the bootup max frequency for SD/MCI/SPI mode data transfer.
 *      Existing drivers will divide down till <= this frequency.
 *---------------------------------------------------------------------------*/
TUInt32 UEZPlatform_MCI_DefaultFreq(void)
{
  return 15000000UL;
  //return 7500000UL;
  //return 3750000UL;
  //return 1875000UL;
  //return 937500UL;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MCI_TransferMode
 *---------------------------------------------------------------------------*
 * Description:
 *      For SD/MCI mode select 1-bit or 4-bit mode data transfer.
 *---------------------------------------------------------------------------*/
TUInt32 UEZPlatform_MCI_TransferMode(void)
{
  //return UEZ_MCI_BUS_1BIT_WIDE; // 1-bit mode
  return UEZ_MCI_BUS_4BIT_WIDE; // 4-bit mode
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MCI_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the MCI Peripheral
 *---------------------------------------------------------------------------*/
void UEZPlatform_MCI_Require(void)
{
    const T_LPC17xx_40xx_MCI_Pins pins = {
        GPIO_P1_6,   // DAT0
        GPIO_P1_7,   // DAT1
        GPIO_P1_11,  // DAT2
        GPIO_P1_12,  // DAT3
        GPIO_P0_19,  // CLK
        GPIO_P0_20,  // CMD
        GPIO_NONE,   // GPIO_P1_3, // Card Detect
        GPIO_NONE,   // Write Protect Detect
    };

	DEVICE_CREATE_ONCE();
    UEZPlatform_GPDMA1_Require();
    LPC17xx_40xx_MCI_Require(&pins, "GPDMA1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS1 using SDCard drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS1_MCI_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_MCI_Require();
    MassStorage_SDCard_MCI_Create("MS1", "MCI", "GPDMA1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SDCard_Drive_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the high speed SDCard drive using MS1 on the given drive number
 *---------------------------------------------------------------------------*/
void UEZPlatform_SDCard_Drive_Require(TUInt8 aDriveNum)
{
    T_uezDevice ms1;
    T_uezDeviceWorkspace *p_ms1;

    DEVICE_CREATE_ONCE();

    UEZPlatform_MS1_MCI_Require();
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS1", &ms1);
    UEZDeviceTableGetWorkspace(ms1, (T_uezDeviceWorkspace **)&p_ms1);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms1);
}

void UEZPlatform_SDCard_MCI_Drive_Require(TUInt8 aDriveNum)
{
    UEZPlatform_SDCard_Drive_Require(aDriveNum);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioCodec_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioCodec_Require(void)
{
    T_uezDevice p_ac;
    DEVICE_AudioCodec **ac;

    DEVICE_CREATE_ONCE();

    AudioCodec_WM8731_ADXL345_Create("AudioCodec0", "I2C2");
    UEZDeviceTableFind("AudioCodec0", &p_ac);
    UEZDeviceTableGetWorkspace(p_ac, (T_uezDeviceWorkspace **)&ac);

    (*ac)->UseConfiguration((void*)ac, 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBFlash_Drive_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Flash drive using MS0 on the given drive number
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum)
{
    T_uezDevice ms0;
    T_uezDeviceWorkspace *p_ms0;

    DEVICE_CREATE_ONCE();

    // Now put the MS0 in the filesystem using the USB
    UEZPlatform_MS0_Require();
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS0", &ms0);
    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms0);
}

static T_uezDevice G_Amp;
static TUInt8 G_Amp_In_Use = AUDIO_AMP_NONE;
/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioAmp_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Audio Amp driver and set the default level
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioAmp_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C1_Require();
    AudioAmp_LM48110_Create("AMP0", "I2C1", 0x1F);
    UEZAudioAmpOpen("AMP0", &G_Amp);
    UEZAudioAmpMute(G_Amp); // mute before level to avoid noise until needed
    UEZAudioAmpSetLevel(G_Amp, UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110);
    G_Amp_In_Use = AUDIO_AMP_LM48110;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI43WQN_AudioMixerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
static T_uezError UEZGUI43WQN_AudioMixerCallback(
        T_uezAudioMixerOutput aChangedOutput,
            TBool aMute,
            TUInt8 aLevel)
{
    T_uezError error = UEZ_ERROR_NONE;

    switch(aChangedOutput){
        case  UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER:
            if(G_Amp){
                if(aMute){
                    error = UEZAudioAmpMute(G_Amp);
                } else {
                    error = UEZAudioAmpUnMute(G_Amp);
                } 
                // This insures we scale the 0-255 input volume to the correct maximum volume for the onboard speaker.
                if(G_Amp_In_Use == AUDIO_AMP_TDA8551) { // enforce maximum volume based on AMP
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_TDA8551 /255);
                } else if(G_Amp_In_Use == AUDIO_AMP_WOLFSON) {
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL /255);
                } else if(G_Amp_In_Use == AUDIO_AMP_LM48110) {
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110 /255);
                } else {
                    aLevel = 0; // NEED TO DEFINE AMP AND MAX SAFE VOLUME LEVEL!
                }
                error = UEZAudioAmpSetLevel(G_Amp, aLevel);
            }
        // Example Offboard speaker callback case
        /*case  UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER:
            if(G_Amp){
                if(aMute){
                    error = UEZAudioAmpMute(G_Amp);
                } else {
                    error = UEZAudioAmpUnMute(G_Amp);
                }
                error = UEZAudioAmpSetLevel(G_Amp, aLevel);
            }*/
            break;
        default:
            break;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioMixer_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Audio Mixer driver and callback function
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioMixer_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_AudioAmp_Require();
    UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, &UEZGUI43WQN_AudioMixerCallback);
    
    // To seperately control offboard speaker jack uncomment this line.
    //UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER, &UEZGUI70WVT_AudioMixerCallback);
    
    UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER); // mute here to avoid the "beep" sound from turning on too fast.

    // Set all 5 volume levels from platform file
    // First set master volume
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_MASTER, UEZ_DEFAULT_AUDIO_LEVEL); // master volume
    // Next set onboard speaker based on AMP that is loaded. Callback will handle this automatically.
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, UEZ_DEFAULT_AUDIO_LEVEL);
    
    // Uncomment these lines to set the other 3 volumes if needed.
    //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER, UEZ_DEFAULT_OFFBOARD_SPEAKER_AUDIO_LEVEL); // We call this AFTER onboard speaker, so it will set the current volume.
    //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_HEADPHONES, UEZ_DEFAULT_ONBOARD_HEADPHONES_AUDIO_LEVEL);
    //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_HEADPHONES, UEZ_DEFAULT_OFFBOARD_HEADPHONES_AUDIO_LEVEL);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Audio_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the audio system
 *---------------------------------------------------------------------------*/
void UEZPlatform_Audio_Require(void)
{    
    // Init audio outputs before amp so amp doesn't get noise/undefined signal.
    UEZPlatform_Speaker_Require(); // PWM output for basic tones
    UEZPlatform_DAC0_Require();    // DAC output
    
    UEZPlatform_AudioMixer_Require(); // UEZPlatform_AudioAmp_Require();
    //UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    //UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_CRC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the CRC0 drivers for doing CRC calculations
 *---------------------------------------------------------------------------*/
void UEZPlatform_CRC0_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_CRC0_Require();
    CRC_Generic_Create("CRC0", "CRC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM0 drivers
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM0_Require(void)
{
    DEVICE_CREATE_ONCE();

   LPC17xx_40xx_PWM0_Require();
   // Current UEZGUI does not use these, but may need one later
   //LPC17xx_40xx_PWM0_1_Require(pin?);
   //LPC17xx_40xx_PWM0_2_Require(pin?);
   //LPC17xx_40xx_PWM0_3_Require(pin?);
   //LPC17xx_40xx_PWM0_4_Require(pin?);
   //LPC17xx_40xx_PWM0_5_Require(pin?);
   //LPC17xx_40xx_PWM0_6_Require(pin?);
   //LPC17xx_40xx_PWM0_CAP0_Require(pin?);
   PWM_Generic_Create("PWM0", "PWM0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM1 driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM1_Require(void)
{
    DEVICE_CREATE_ONCE();

   LPC17xx_40xx_PWM1_Require();
   // Current UEZGUI does not use these, but may need one later
   //LPC17xx_40xx_PWM1_1_Require(pin?);
   //LPC17xx_40xx_PWM1_2_Require(pin?);
   //LPC17xx_40xx_PWM1_3_Require(pin?);
   //LPC17xx_40xx_PWM1_4_Require(pin?);
   //LPC17xx_40xx_PWM1_5_Require(pin?);
   //LPC17xx_40xx_PWM1_6_Require(pin?);
   //LPC17xx_40xx_PWM1_CAP0_Require(pin?);
   //LPC17xx_40xx_PWM1_CAP1_Require(pin?);
   PWM_Generic_Create("PWM1", "PWM1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Touchscreen_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_Touchscreen_Require(void)
{
    //T_uezDevice ts;
    DEVICE_CREATE_ONCE();
    UEZPlatform_I2C1_Require();

    Touchscreen_FT5306DE4_Create("Touchscreen", "I2C1", GPIO_P2_19, GPIO_P1_18);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Touchscreen_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_TouchscreenRT_Require(void)
{
    T_uezDevice ts;

    const TS_FourWireTouchResist_Configuration tsConfig = {
            // XPlus ADC = ADC0.0 / P0.23
            { "ADC0",   0,  GPIO_P0_23,     0,      1 },
            // XMinus GPIO = P2.19
            { GPIO_P2_19 },
            // YPlus ADC = ADC0.1 / P0.24
            { "ADC0",   1,  GPIO_P0_24,     0,      1 },
            // YMinus GPIO = P1.18
            { GPIO_P1_18 },
            // Use IRQs?
            ETrue,
            // Need input buffer disabled when doing A/Ds?
            ETrue,
    };

    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_GPIO1_Require();
    LPC17xx_40xx_GPIO2_Require();
    UEZPlatform_ADC0_0_Require();
    UEZPlatform_ADC0_1_Require();

    Touchscreen_FourWireTouchResist_Create("Touchscreen", &tsConfig);
    UEZDeviceTableFind("Touchscreen", &ts);
    UEZTSSetTouchDetectSensitivity(ts, 0x4000, 0x6000);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBDevice_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USBDevice controller.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBDevice_Require(void)
{
    const T_LPC17xx_40xx_USBDevice_Settings portBSettings = {
            GPIO_P1_30,     // USB_VBUS2        = USB2_VBUS
            GPIO_P0_31,     // USB_D+2          = USB2_DP
            GPIO_NONE,      // USB_D-2          = USB2_DM (not a port pin)
            GPIO_P0_14,     // USB_CONNECT2n    = USB2_CON
            GPIO_P0_13,     // USB_UP_LED2      = P0.13_USB2_UPLED_AD0.7
    };
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_USBDevice_PortB_Require(&portBSettings);
    USBDevice_LPC17xx_40xx_Create("USBDevice", "USBDeviceController");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EMAC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the EMAC controller for networking.
 *---------------------------------------------------------------------------*/
void UEZPlatform_EMAC_Require(void)
{
    // This EMAC is RMII (less pins) // Due to RMII require, struct will be different from 4357.
    const T_LPC17xx_40xx_EMAC_Settings emacSettings = {
            GPIO_P1_4,      // ENET_TX_ENn      = P1.4_ENET_TXEN
            GPIO_NONE,      // ENET_TX_TXD[3]   = not used for RMII
            GPIO_NONE,      // ENET_TX_TXD[2]   = not used for RMII
            GPIO_P1_1,      // ENET_TX_TXD[1]   = P1.1_ENET_TXD1
            GPIO_P1_0,      // ENET_TX_TXD[0]   = P1.0_ENET_TXD0
            GPIO_NONE,      // ENET_TX_ER       = not used for RMII
            GPIO_NONE,      // ENET_TX_CLK      = not used for RMII
            GPIO_NONE,      // ENET_RX_DV       = not used for RMII pin set to DV mode below
            GPIO_NONE,      // ENET_RXD[3]      = not used for RMII
            GPIO_NONE,      // ENET_RXD[2]      = not used for RMII
            GPIO_P1_10,     // ENET_RXD[1]      = P1.10_ENET_RXD1
            GPIO_P1_9,      // ENET_RXD[0]      = P1.9_ENET_RXD0
            GPIO_P1_14,     // ENET_RX_ER       = P1.14_ENET_RX_ER
            GPIO_P1_15,     // ENET_REFCLK      = P1.15_ENET_REFCLK
            GPIO_NONE,      // ENET_COL         = not used for RMII
            GPIO_P1_8,      // ENET_CRS         = P1.8_ENET_DV dual purpose pin will run in DV mode for RMII
            GPIO_P1_16,     // ENET_MDC         = P1.16_ENET_MDC
            GPIO_P1_17,     // ENET_MDIO        = P1.17_ENET_MDIO
    };
    DEVICE_CREATE_ONCE();

    LPC17xx_40xx_EMAC_RMII_Require(&emacSettings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer0 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer0_Require(void)
{
    static const T_LPC17xx_40xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            },
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_Timer0_Require(&settings);
    Timer_Generic_Create("Timer0", "Timer0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer1 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer1_Require(void)
{
    static const T_LPC17xx_40xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_Timer1_Require(&settings);
    Timer_Generic_Create("Timer1", "Timer1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer2 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer2_Require(void)
{
    static const T_LPC17xx_40xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_Timer2_Require(&settings);
    Timer_Generic_Create("Timer2", "Timer2");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer3 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer3_Require(void)
{
    static const T_LPC17xx_40xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_Timer3_Require(&settings);
    Timer_Generic_Create("Timer3", "Timer3");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_WiredNetwork0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wired connections.
 *---------------------------------------------------------------------------*/
void UEZPlatform_WiredNetwork0_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Wired network needs an EMAC
    UEZPlatform_EMAC_Require();

    // Create the network driver for talking to lwIP on a wired
    // network.
#if (UEZ_ENABLE_TCPIP_STACK == 1)
    Network_lwIP_Create("WiredNetwork0");
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_WirelessNetwork0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wireless connections.
 *---------------------------------------------------------------------------*/
#if (UEZ_ENABLE_TCPIP_STACK == 1)
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
void UEZPlatform_WirelessNetwork0_Require(void)
{
    // These SPI pins work for both the Application Header and the PMOD Header
    // for GainSpan Modules with SPI configured.
    const T_GainSpan_Network_SPISettings spi_settings = {
    // SPI Settings
        "SSP0",// SSP Name
        3000, // 3 MHz
        // Pins.
        GPIO_P2_23, // iSPIChipSelect = WIFI_CSN = P2.23
        GPIO_P2_11, // iDataReadyIO = WIFI_INTR 
        GPIO_NONE,  // iProgramMode = P1.6_MCIDAT0_PWM0.4
        GPIO_NONE,  // iSPIMode = WIFI_MODE 
        GPIO_NONE,  // iSRSTn = WIFI_SRSTn AND'd with RESET_OUTn
        GPIO_NONE,  // iWIFIFactoryRestore = WIFI_FAC_RST
        };

    DEVICE_CREATE_ONCE();

    //LPC17xx_40xx_GPIO1_Require();
    LPC17xx_40xx_GPIO2_Require();
    //LPC17xx_40xx_GPIO5_Require();
    UEZPlatform_SSP0_Require();
    Network_GainSpan_Create("WirelessNetwork0", &spi_settings);
}
#endif

// Need UART connection to Wifi module for this to work
// Modify UART pins for expansion board before use
void UEZPlatform_WiFiProgramMode(TBool runMode)
{    
#define GPIO_WIFI_FAC_RST       GPIO_P5_1
#define GPIO_WIFI_SRSTn         GPIO_P1_5
#define GPIO_WIFI_IRQ           GPIO_P2_11
#define GPIO_WIFI_MODE          GPIO_P1_6

    if(runMode == ETrue) {
        printf("GainSpan Run Mode ...\n");        
    } else {
        printf("GainSpan Programming Mode ...\n");
    }
    RTOS_ENTER_CRITICAL()    ;
    
    UEZGPIOSet(GPIO_WIFI_FAC_RST);          // WIFI_FAC_RST
    UEZGPIOSetMux(GPIO_WIFI_FAC_RST, 0);
    UEZGPIOOutput(GPIO_WIFI_FAC_RST);
    
    UEZGPIOClear(GPIO_WIFI_SRSTn);        // WIFI_SRSTn
    UEZGPIOSetMux(GPIO_WIFI_SRSTn, 0);
    UEZGPIOOutput(GPIO_WIFI_SRSTn);       // WIFI_SRSTn
    
    UEZGPIOInput(GPIO_WIFI_IRQ);       // WIFI IRQ

    if(runMode == ETrue) {
        UEZGPIOClear(GPIO_WIFI_MODE);        // WIFI RUN MODE
    } else {
        UEZGPIOSet(GPIO_WIFI_MODE);          // WIFI PROGRAM ON
    }
    UEZGPIOOutput(GPIO_WIFI_MODE);       
                
    UEZGPIOUnlock(GPIO_P0_2);       // 1788 TX
    UEZGPIOSetMux(GPIO_P0_2, 0);
    UEZGPIOOutput(GPIO_P0_2);

    UEZGPIOUnlock(GPIO_P0_3);       // 1788 RX
    UEZGPIOSetMux(GPIO_P0_3, 0);
    UEZGPIOInput(GPIO_P0_3);

    UEZGPIOUnlock(GPIO_P0_15);      // WIFI TX
    UEZGPIOSetMux(GPIO_P0_15, 0);
    UEZGPIOOutput(GPIO_P0_15);

    UEZGPIOUnlock(GPIO_P0_16);      // WIFI RX
    UEZGPIOSetMux(GPIO_P0_16, 0);
    UEZGPIOInput(GPIO_P0_16);
    
    UEZGPIOSet(GPIO_WIFI_SRSTn);          // WIFI_SRSTn

    CPUDisableInterrupts();
    while (1) {
        if (LPC_GPIO0->PIN & (1 << 3)) // 1788 RX = GPIO_P0_3
            LPC_GPIO0->SET = (1 << 15); // WIFI TX = GPIO_P0_15
        else
            LPC_GPIO0->CLR = (1 << 15);

        if (LPC_GPIO0->PIN & (1 << 16)) // WIFI RX = GPIO_P0_16
            LPC_GPIO0->SET = (1 << 2);  // 1788 TX = GPIO_P0_2
        else
            LPC_GPIO0->CLR = (1 << 2);  // 1788 TX = GPIO_P0_2
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2S_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2S drivers for talking to the expansion board.
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2S_Require(void)
{
    static const T_LPC17xx_40xx_I2S_Settings settings = {
            GPIO_P0_6,  // I2S_RX_SDA   = P0.6_I2SRX_SDA_SSEL1_MAT2.0
            GPIO_P0_4,  // I2S_RX_SCK   = P0.4_I2SRX_CLK_RD2_CAP2.0
            GPIO_P0_5,  // I2S_RX_WS    = P0.5_I2SRX_WS_TD2_CAP2.1
            GPIO_NONE,  // I2S_RX_MCLK  = not used
            GPIO_P0_9,  // I2S_TX_SDA   = P0.9_I2STX_SDA_MOSI1_MAT2.3
            GPIO_P0_7,  // I2S_TX_SCK   = P0.7_I2STX_CLK_SCK1_MAT2.1
            GPIO_P0_8,  // I2S_TX_WS    = P0.8_I2STX_WS_MISO1_MAT2.2
            GPIO_NONE,  // I2S_TX_MCLK  = not used
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_I2S_Require(&settings);
    Generic_I2S_Create("I2S", "I2S");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ButtonBoard_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the bitbanged I2C drivers for talking to the button board
 *---------------------------------------------------------------------------*/
void UEZPlatform_ButtonBoard_Require(void) {
    // Use one of the following to turn the buttonboard on:
    //BitBang_ButtonBoard_Start(GPIO_P0_0, GPIO_P0_1); //bitbang I2C
    //UEZPlatform_ButtonBoard_I2C1_Require(); // uncomment to use real I2C
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ButtonBoard_I2C1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the real I2C driver for talking to the button board
 *---------------------------------------------------------------------------*/
void UEZPlatform_ButtonBoard_I2C1_Require(void) {        
    static const T_GPIOKeypadAssignment keyAssignment[] = {
        { 0, KEY_ENTER },
        { 1, KEY_ARROW_LEFT },
        { 2, KEY_ARROW_RIGHT },
        { 3, KEY_ARROW_DOWN },
        { 4, KEY_ARROW_UP },
        { 0, 0 },
    };

    DEVICE_CREATE_ONCE();
    // I2C1 on ALT COM port, must disable normal I2C1 and require routine!
    UEZPlatform_I2C1_ALT_COM_Require(); 
    GPIO_PCF8574T_Create("GPIO:PCF8574T", UEZ_GPIO_PORT_EXT1, "I2C1", 0x48>>1);
    Keypad_Generic_GPIO_Create("BBKeypad", UEZ_GPIO_PORT_EXT1, keyAssignment, 5,
        KEYPAD_LOW_TRUE_SIGNALS, 
        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 7), 
        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 6));
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_System_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a board specific system reset. In some cases we have a pin that
 *      can trigger POR as if you pushed a physical reset button.
 *      This is necessary to insure a full hardware reset across all lines
 *      with minimum reset hold timing.
 *---------------------------------------------------------------------------*/
void UEZPlatform_System_Reset(void) {  
  // No HW reset on board
  //GPIO->CLR[UEZ_GPIO_PORT_FROM_PORT_PIN(PIN_HW_RESET)] |= (1 << UEZ_GPIO_PIN_FROM_PORT_PIN(PIN_HW_RESET)); //UEZGPIOClear(PIN_HW_RESET);
  //GPIO->DIR[UEZ_GPIO_PORT_FROM_PORT_PIN(PIN_HW_RESET)] |= (1 << UEZ_GPIO_PIN_FROM_PORT_PIN(PIN_HW_RESET)); //UEZGPIOOutput(PIN_HW_RESET);
  NVIC_SystemReset();
}



// Don't enable console anymore in minimal requires, so that we can use this for GUI only bootloader, etc.
void UEZPlatform_Minimal_Require(void)
{
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_GPIO2_Require();
      
#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
    //UEZPlatform_CRC0_Require(); // Adds a few extra bytes if we require it later in code.
    //UEZPlatform_Flash0_Require(); // currently this won't fit in BBL
    
    // TODO could read normal QSPI or OTP region here, but will be too much space for any bootloader.
#else
    UEZPlatform_CRC0_Require();
    UEZPlatform_Flash0_Require();
#endif
  
    /* TODO could load QSPI OTP data right here early to determine unit Rev, 
     * then adjust pins or other drivers/calibration as needed. (Not used at this time)
     * Enable CRC now in case we want to CRC the rev data that we read, or 
     * application CRC could be checked here before we start executing code from QSPI
     * When it comes to checksumming the application, the platfrom file would be a good spot to handle
     * start/end boundaries for checksum range(s). */

#if USING_43WQN_BA_REV1 // Make sure that power to I2C devices cannot be turned off on this revision
    // TODO set this pin state on earlier during boot
    static TBool init = EFalse;
    if (!init) {
      LPC17xx_40xx_GPIO2_Require();
      UEZGPIOSetMux(GPIO_P2_0, 0);
      UEZGPIOClear(GPIO_P2_0);    
      UEZGPIOOutput(GPIO_P2_0);
      UEZGPIOLock(GPIO_P2_0); 
      init = ETrue;
    }
#endif    

    LPC17xx_40xx_GPIO1_Require();
    LPC17xx_40xx_GPIO3_Require();
    LPC17xx_40xx_GPIO4_Require();
    LPC17xx_40xx_GPIO5_Require();
}

void UEZPlatform_Standard_Require(void)
{
    UEZPlatform_Minimal_Require();
    
    // Setup console immediately
#if UEZ_ENABLE_CONSOLE_ALT_PWR_COM
    UEZPlatform_Console_ALT_PWR_COM_Require(
        UEZ_CONSOLE_WRITE_BUFFER_SIZE,
        UEZ_CONSOLE_READ_BUFFER_SIZE);
#else
    UEZPlatform_Console_Expansion_Require(
        UEZ_CONSOLE_WRITE_BUFFER_SIZE,
        UEZ_CONSOLE_READ_BUFFER_SIZE);
#endif
        
// Example for console on UART 1 RS485 half duplex
// UEZPlatform_Console_HalfDuplex_RS485_Require("UART1",
// 1024, 256, GPIO_P0_22, ETrue, 2, GPIO_P0_17, EFalse, 2);

    UEZPlatform_LCD_Require();

    //UEZPlatform_I2C0_Require(); // not used on this board, will interfere with loopback test
    UEZPlatform_I2C1_Require();    
    //UEZPlatform_I2C2_Require(); // Cannot turn this on and run loopback test
    UEZPlatform_Temp0_Require();
    UEZPlatform_GPDMA0_Require();
    UEZPlatform_GPDMA1_Require();
    UEZPlatform_ADC0_0_Require();
    UEZPlatform_ADC0_1_Require();
    UEZPlatform_ADC0_2_Require();
    UEZPlatform_EEPROM0_Require();
    UEZPlatform_Accel0_Require();
    UEZPlatform_RTC_Require();
#if USE_RESISTIVE_TOUCH
    UEZPlatform_TouchscreenRT_Require();
#else
    UEZPlatform_Touchscreen_Require();    
#endif
    UEZPlatform_Audio_Require();
}

void UEZPlatform_Require(void)
{
    // Choose one:
    UEZPlatform_Standard_Require();
}

TBool UEZPlatform_Host_Port_B_Detect()
{
    TBool IsDevice;
 
    UEZGPIOLock(GPIO_P0_12);
    UEZGPIOSet(GPIO_P0_12); // disable MIC2025
    UEZGPIOOutput(GPIO_P0_12);
    UEZGPIOInput(GPIO_P0_28);
    UEZGPIOSetMux(GPIO_P0_28, 0);

    IsDevice = UEZGPIORead(GPIO_P0_28);

    if(!IsDevice){
        UEZGPIOClear(GPIO_P0_12);// enable MIC2025
    }
    return IsDevice;
}

TUInt16 UEZPlatform_LCDGetHeight(void)
{
    return UEZ_LCD_DISPLAY_HEIGHT;
}
TUInt16 UEZPlatform_LCDGetWidth(void)
{
    return UEZ_LCD_DISPLAY_WIDTH;
}

//#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
TUInt32 UEZPlatform_GetBaseAddress(void)
{
    // Keep various memory location declarations from optimizing out
    ((TUInt8 volatile *)_framesMemoryptr)[0] = _framesMemoryptr[0];
    return LCD_DISPLAY_BASE_ADDRESS;
}

TUInt32 UEZPlatform_LCDGetFrame(TUInt16 aFrameNum)
{
    return (TUInt32)FRAME(aFrameNum);
}
TUInt32 UEZPlatform_LCDGetFrameBuffer(void)
{
    return (TUInt32)LCD_FRAME_BUFFER;
}
TUInt32 UEZPlatform_LCDGetFrameSize(void)
{
    return FRAME_SIZE;
}
TUInt32 UEZPlatform_SerialGetDefaultBaud(void)
{
    return SERIAL_PORTS_DEFAULT_BAUD;
}
TUInt32 UEZPlatform_ProcessorGetFrequency(void)
{
    return PROCESSOR_OSCILLATOR_FREQUENCY;
}
TUInt32 UEZPlatform_GetPCLKFrequency(void)
{
#if (PROCESSOR_OSCILLATOR_FREQUENCY == 72000000)
    return 72000000;
#else
    return 60000000;
#endif
}
#if (INCLUDE_EMWIN == 1)
#include <Source/Library/GUI/SEGGER/emWin/LCD.h>
#include <Source/Library/GUI/SEGGER/emWin/GUIDRV_Lin.h>
const void* UEZPlatform_GUIColorConversion(void)
{
    return GUICC_M555;
}

const void *UEZPlatform_GUIDisplayDriver()
{
    return &GUIDRV_Lin_16_API;
}
#else
const void* UEZPlatform_GUIColorConversion(void)
{
    return 0;
}

const void *UEZPlatform_GUIDisplayDriver()
{
    return 0;
}
#endif

void *SUICallbackGetLCDBase(void)
{
    return (void *)(LPC_LCD->UPBASE);
}
void SUICallbackSetLCDBase(void *aAddress)
{
    LPC_LCD->UPBASE = (TUInt32)aAddress;
}

TUInt32 UEZPlatform_5V_Monitor_Get_Raw_Reading(void)
{    
    T_uezDevice adc;
    T_uezError error;
    ADC_RequestSingle r;
    TUInt32 reading = 0; // on any failure just return 0.
    // 0 isn't valid as we should have some voltage on a running unit, even on bad 5V rail.
     
    error = UEZADCOpen("ADC0", &adc);
    if (error == UEZ_ERROR_NONE) { // Take a reading
        r.iADCChannel = 2;
        r.iBitSampleSize = 10;
        r.iTrigger = ADC_TRIGGER_NOW;
        r.iCapturedData = &reading;
        error = UEZADCRequestSingle(adc, &r);
        if (error == UEZ_ERROR_NONE) {
            // Generate percentage based on perfect 5V input
            // divided by 2, but compared to a A/D value of 0 - 0x3FFF
            // with a reference voltage of 3.3V
            //   reading = (voltage / 5 V) * 0x3FF
            //   percent = 100 * (reading / (0x3FF * 3.3V/5V))
            //percent = reading * 100 / 0x307;
        }
    }
    UEZADCClose(adc);
    return reading;
}

T_pixelColor SUICallbackRGBConvert(int32_t r, int32_t g, int32_t b)
{
    //return RGB16(r, g, b);
    return RGB(r, g, b);
}

void WriteByteInFrameBufferWithAlpha(UNS_32 aAddr, COLOR_T aPixel, T_swimAlpha aAlpha)
{
    static COLOR_T mask = (uint16_t)(~((1<<10)|(1<<5)|(1<<0)));
    COLOR_T *p = (COLOR_T *)aAddr;

    switch (aAlpha) {
        case SWIM_ALPHA_50:
            *p = (((*p & mask)>>1) + ((aPixel & mask)>>1));
            break;
    }
}

void vMainMPUFaultHandler( unsigned long * pulFaultRegisters )
{
    PARAM_NOT_USED(pulFaultRegisters);
unsigned long ulStacked_pc = 0UL;

    ( void ) ulStacked_pc;

    /* Determine which stack was in use when the MPU fault occurred and extract
    the stacked PC. */
    __asm volatile
    (
        "   tst lr, #4          \n"
        "   ite eq              \n"
        "   mrseq r0, msp       \n" /* The code that generated the exception was using the main stack. */
        "   mrsne r0, psp       \n" /* The code that generated the exception was using the process stack. */
        "   ldr r0, [r0, #24]   \n" /* Extract the value of the stacked PC. */
        "   str r0, [sp]        \n" /* Store the value of the stacked PC into ulStacked_pc. */
    );

    /* Inspect ulStacked_pc to locate the offending instruction. */
    for( ;; );
}

/*---------------------------------------------------------------------------*
 * Routine:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      The main() routine in UEZ is only a stub that is used to start
 *      the whole UEZ system.  UEZBSP_Startup() is immediately called.
 * Outputs:
 *      int32_t -- not used, 0
 *---------------------------------------------------------------------------*/
int32_t main(void)
{
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
    switch(G_hardwareTest.iTestMode){
      case HARDWARE_TEST_CRYSTAL:
        COM_Send("Starting uEZ System and Tasks\n", 30);
      
      break;
      case HARDWARE_TEST_SDRAM:
        COM_Send("Starting uEZ System and Tasks\n", 30);
     /*for(uint16_t i = 0; i < 255; i++) {
        COM_Send("SDRAM TESTING...\n", 17);
        MemoryTest(UEZBSP_SDRAM_BASE_ADDR, UEZBSP_SDRAM_SIZE);
        COM_Send("SDRAM PASS\n", 11);
      }*/
      break;
      case HARDWARE_TEST_I2C:
        COM_Send("Starting uEZ System and Tasks\n", 30);
      
      break;
      case HARDWARE_TEST_AUDIO:
    
      break;
      case HARDWARE_TEST_NORMAL:
      default:
    
      break;
    }
#endif
    UEZBSP_Startup();
    while (1) {
    } // never should get here
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/
