/*-------------------------------------------------------------------------*
 * File:  uEZPlatform.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the uEZGUI-4357-70WVN
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

 /**
 *    @addtogroup Platform_uEZGUI_4357_70WVN
 *  @{
 *  @brief     uEZ Platform file for the uEZGUI_4357_70WVN
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZLicense.txt
 *
 *    uEZ Platform file for the uEZGUI-435750WVN
 *
 * @par Example code:
 * // example description
 * @par
 * @code
 *
 * @endcode
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
#include <Source/Processor/NXP/LPC43xx/LPC43xx_USBDeviceController.h>
#include <Source/Devices/Accelerometer/Freescale/MMA7455/Freescale_MMA7455.h>
#include <Source/Devices/Accelerometer/ST/LIS3DH/ST_LIS3DH_I2C.h>
#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/CRC/Generic/CRC_Generic.h>
#include <Source/Devices/AudioAmp/NXP/TDA8551_T/AudioAmp_TDA8551T.h>
#include <Source/Devices/AudioAmp/Wolfson/WM8731/AudioAmp_WM8731.h>
#include <Source/Devices/AudioAmp/TI/LM48100/AudioAmp_LM48100.h>
#if (UEZ_ENABLE_AUDIO_CODEC == 1)
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#endif
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Source/Devices/CRC/Software/CRC_Software.h>
#include <Source/Devices/DAC/Generic/DAC_Generic.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM_Generic_I2C.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM16_Generic_I2C.h>
#include <Source/Devices/EEPROM/NXP/LPC43xx/EEPROM_NXP_LPC43xx.h>
#include <Source/Devices/Flash/NXP/LPC43xx/LPC43xx_IAP.h>
#include <Source/Devices/Flash/NXP/LPC_SPIFI_M4F/Flash_NXP_LPC_SPIFI_M4.h>
#include <Source/Devices/GPDMA/Generic/Generic_GPDMA.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include <Source/Devices/I2C/Generic/Generic_I2C.h>
#include <Source/Devices/I2S/Generic/Generic_I2S.h>
#include <Source/Devices/LCD/Innolux/AT070TN84/Innolux_AT070TN84.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <Source/Devices/MassStorage/LPCUSBLib/LPCUSBLib_MS.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SD_MMC.h>
#include <Source/Devices/Stream/LPCUSBLib/SerialHost/Stream_LPCUSBLib_SerialHost.h>
#include <Source/Devices/Network/GainSpan/Network_GainSpan.h>
#include <Source/Devices/Network/ESPWROOM32/Network_ESPWROOM32.h>
#if (UEZ_ENABLE_TCPIP_STACK == 1)
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#endif
#include <Source/Devices/PWM/Generic/Generic_PWM.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/Stream/SWO/Stream_SWO_CortexM.h>
#include <Source/Devices/Serial/LPCUSBLib/SerialHost_FTDI/Stream_LPCUSBLib_SerialHost_FTDI.h>
#include <Source/Devices/SPI/Generic/Generic_SPI.h>
#include <Source/Devices/Temperature/NXP/LM75A/Temperature_LM75A.h>
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/ToneGenerator/Generic/Timer/ToneGenerator_Generic_Timer.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/Touchscreen/Newhaven/FT5306DE4/FT5306DE4TouchScreen.h>
#include <Source/Devices/USBHost/Generic/Generic_USBHost.h>
#include <Source/Devices/Watchdog/Generic/Watchdog_Generic.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_RTC.h>
#include <Source/Library/Web/BasicWeb/BasicWEB.h>
#include <Source/Library/FileSystem/FATFS/uEZFileSystem_FATFS.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Memory/MemoryTest/MemoryTest.h>
#include <Source/Library/StreamIO/StdInOut/StdInOut.h>
#include <Source/Processor/NXP/LPC43xx/uEZProcessor_LPC43xx.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_ADCBank.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_DAC.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_EMAC.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_EMC_Static.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPDMA.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPIO.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_I2C.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_I2S.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_LCDController.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_SD_MMC.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_PWM.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_PLL.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_SDRAM.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Serial.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_SSP.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Timer.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_USBDeviceController.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_USBHost.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Watchdog.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_UtilityFuncs.h>
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
#include <uEZAudioMixer.h>
#include <uEZTimer.h>
#include <Source/Devices/GPIO/I2C/PCA9532/GPIO_PCA9532.h>
#include <Source/uEZSystem/uEZHandles.h>

#include <Config_Build.h>
#include <Device/LCD/Config_LCD.h>

#include <Source/Devices/Flash/NXP/LPC_SPIFI_M4F/Flash_NXP_LPC_SPIFI_M4.h>

extern int32_t MainTask(void);

#ifdef CORE_M0 // Enable RITIMER interrupt.
static void IRITimerProcessIRQ(void);
void RITIMER_OR_WWDT_IRQHandler(void);
#endif

#define SD_LOW_POWER_SUPP   (0) // set to 1 to enable the power pin on SD card
#define USE_TIMER_SYSTICK   (0) // TODO in some demos the timer gets disabled somehow during boot, probably wrong prio set. Not fully implemented

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define CONFIG_MEMORY_TEST_ON_SDRAM     0
#define LPC4357_LCD_ADDRESS_GUARD       0 // set to 1 in case you have suspect code that can set LCD start less than base address.

#ifndef ENABLE_LCD_HSYNC_VSYNC_PINS
#define ENABLE_LCD_HSYNC_VSYNC_PINS     1 // must set to 1 to enable the HYSNC/VSYNC, otherwise pines are set to low
#endif

#ifndef ENABLE_LCD_DE_PIN
#define ENABLE_LCD_DE_PIN               1 // must set to 1 to enable the DE pin, otherwise the pin is set to input
#endif

#ifndef LCD_BACKLIGHT_FREERTOS_TIMER
#define LCD_BACKLIGHT_FREERTOS_TIMER    1 // set to 0 here and in library project Config_Build.h to force using MCU timer peripheral for LCD backlight delay.
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_uezTask G_mainTask;
static TUInt8 G_ms0_driveNum = 0;
static TUInt32 G_USBHostDriveNumber = 0xFF;

#if (USE_TIMER_SYSTICK == 1)
T_uezDevice G_sysTickTimer;
T_uezTimerCallback G_sysTickCallback;
#endif

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

// See Users Manual UM10503 Section 6.6 Code Read Protection. If you set these to certain bytes JTAG will be disabled.
// In uEZ 2.13 all demo appications will set CRP1 bits high. (no protection)
// However, bootloader projects will also set CRP1 bits high. (no protection)
// If you change this here then rebuild both bootloader and application will set the same number still.
// If you modified CRP1 in the application, then flash older bootloader on top of it the number will get reset to 0xFFFFFFFF.

// We recommend to set the number in the bootloader only, then use the below define to disable it in application.
// So then application won't put anything in the start of flash, but will set the CRP2 in the second flash bank.
// If you did try to set the number in the application, it may not be able to flash it as page size is 512 bytes. (flashing may erase part of bootloader)
// So is very important to avoid including the CRP1 section in both a bootloader project and application project.

#ifdef CORE_M4
#if (DO_NOT_INCLUDE_LPC43XX_CODE_READ_PROTECTION_1 == 1)
    // Create this define set to 1 in application code to allow for only setting CRP1 in a bootloader.
#else
UEZ_PUT_SECTION(".crp1", static const TUInt32 G_LPC43XX_CRP1 = 0xFFFFFFFF);
TUInt32 *_crp1ptr = (TUInt32 * const)&G_LPC43XX_CRP1;
#endif

#ifndef BBL_BASE_ADDRESS // Don't include anything in the second flash section in a bootloader application.
// We want to fill or put some kind of data in bank b at the very start of it to make sure that image generation is correct.
UEZ_PUT_SECTION(".fillFlashB0", static const TUInt32 G_LPC43XX_CRP2_FILL = 0x6C6C6946);
//TUInt32 *_crp2fillptr  = (TUInt32 * const)&G_LPC43XX_CRP2_FILL;
UEZ_PUT_SECTION(".crp2", static const TUInt32 G_LPC43XX_CRP2 = 0xFFFFFFFF);
TUInt32 *_crp2ptr  = (TUInt32 * const)&G_LPC43XX_CRP2;
#endif
#endif

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

// Note that uEZBSP Delays are only accurate for around level 2 or medium optimization.
// With no optimization it will be about double the time. So we will always optimize them.
// For IAR application projects set the optimization level of this file to medium or high.

#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 204000000)
//#elif ( PROCESSOR_OSCILLATOR_FREQUENCY == 200000000) // TODO not validated yet.
#else
    #error "1 microSecond delay not defined for CPU speed"
#endif

// Note: To use a non-default max MCU frequency, can redefine UEZBSPDelay1US here.
// UEZBSDelay functions are now weakly defined in library for default frequency.
#if 0 // example of redefinition
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_MED,
void UEZBSPDelay1US(void)
{
#ifdef CORE_M4
/* 	Measured to 1.004 microseconds GPIO switch time: (RTOS off)
 *  -   Based on release build with high optimization speed setting, inline
 *      small functions enabled, with GPIO on/off verified with a scope.
 *      Use the following code for direct GPIO switching (example GPIO_P5_3):
 *          LPC_GPIO_PORT->SET[5] = 8; 
 *          UEZBSPDelay1US();        
 *          LPC_GPIO_PORT->CLR[5] = 8; 
 */
#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 204000000)
    nops50();
    nops50();
    nops50();
    nops10();
    nops10();
    nops10();
    nops10();
    nop();
    nop();
    nop();
    nop();
#elif ( PROCESSOR_OSCILLATOR_FREQUENCY == 200000000) // TODO not validated yet.
    nops50();
    nops50();
    nops50();
    nops10();
    nops10();
    nops10();
    nops10();
    nop();
#else
    #error "1 microSecond delay not defined for CPU speed"
#endif
#endif

#ifdef CORE_M0 // 
// 	Measured to 1.005s from gcc build UEZBSPDelayMS(1000) call.
#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 204000000)
    nops50();
    nops50();
    nops50();
    nops10();
    nops10();
    nops10();
    nops10();
#elif ( PROCESSOR_OSCILLATOR_FREQUENCY == 200000000) // TODO not validated yet.
    nops50();
    nops50();
    nops50();
    nops10();
    nops10();
    nops10();
    nops5();
    nop();
    nop();
#else
    #error "1 microSecond delay not defined for CPU speed"
#endif
#endif
})
#endif

#ifdef CORE_M0
void RITIMER_OR_WWDT_IRQHandler(void)
{
    IRQ_START();
    IRITimerProcessIRQ();
    IRQ_END()
    ;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPSDRAMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the external SDRAM.
 *---------------------------------------------------------------------------*/
void UEZBSP_RAMInit(void)
{
#ifdef CORE_M4

//#if (UEZGUI_4357_70WVN_REV == 1) // future use placeholder

#if 0 // Testing CAS2 mode with -75E timings. Several parts support up to 133MHz.
    static const T_LPC43xx_SDRAM_Configuration sdramConfig_TODO_75E = {
            UEZBSP_SDRAM_BASE_ADDR,
            UEZBSP_SDRAM_SIZE,
            SDRAM_CAS_2, 
            SDRAM_RAS_2, 
            LPC43xx_SDRAM_CLKOUT1,
            SDRAM_CLOCK_FREQUENCY,
            64, // ms
            4096, // cycles
            SDRAM_CYCLES(18), // tRP precharge to activate period - All used parts 18.
            SDRAM_CYCLES(42), // tRAS min // Used parts 42/45. Same register value up to 49
            SDRAM_CYCLES(70), // tXSR min // 69-78 = same cycles, no used parts list SREX. Original part (not available) used 67
            SDRAM_CYCLES(18), // tAPR/tRCD Active Command to Read Command // 20+ = same cycles, no parts used < 18
            SDRAM_CLOCKS(5), // tDAL Input Dat to ACT/REF (tCK units) // On existing parts this stays the same as long as CAS 3 is used.
            SDRAM_CYCLES(12), // tDPL/tWR = write recovery time = tCK + 1(depends on rise time) // Used parts have less than <=14 cycles
            SDRAM_CYCLES(60), // tRC Row Cycle Time or Command Period // 59-68 are same same number, so some parts that are less have same regiser stting
            SDRAM_CYCLES(60), // Refresh Cycle Time tRFC/tREF  // 59-68 are same same number, so some parts that are less have same regiser stting
            SDRAM_CYCLES(70), // tXSR Exit Self-Refresh Time // 69-78 = same cycles.  Original part (not available) used 67
            SDRAM_CYCLES(12), // tRRD Row active to Row Active (or command period) // no parts use > 19, so same register setting
            SDRAM_CLOCKS(2) // tMRD Mode Register Set (to command) (tCK units) // Some parts are 14, but 16/2CK is worst case
            };
    LPC43xx_SDRAM_Init_32BitBus(&sdramConfig_TODO_75E);
#endif

#if 0 // Testing CAS2 mode with -6 timings. Some parts only support up to 100Mhz in spec across temperature range.
    static const T_LPC43xx_SDRAM_Configuration sdramConfig_IS42S32800J_6 = {
            UEZBSP_SDRAM_BASE_ADDR,
            UEZBSP_SDRAM_SIZE,
            SDRAM_CAS_2, 
            SDRAM_RAS_2, 
            LPC43xx_SDRAM_CLKOUT1,
            SDRAM_CLOCK_FREQUENCY,
            64, // ms
            4096, // cycles
            SDRAM_CYCLES(18), // tRP precharge to activate period - All used parts 18.
            SDRAM_CYCLES(42), // tRAS min // Used parts 42/45. Same register value up to 49
            SDRAM_CYCLES(70), // tXSR min // 69-78 = same cycles, no used parts list SREX. Original part (not available) used 67
            SDRAM_CYCLES(18), // tAPR/tRCD Active Command to Read Command // 20+ = same cycles, no parts used < 18
            SDRAM_CLOCKS(5), // tDAL Input Dat to ACT/REF (tCK units) // On existing parts this stays the same as long as CAS 3 is used.
            SDRAM_CYCLES(12), // tDPL/tWR = write recovery time = tCK + 1(depends on rise time) // Used parts have less than <=14 cycles
            SDRAM_CYCLES(60), // tRC Row Cycle Time or Command Period // 59-68 are same same number, so some parts that are less have same regiser stting
            SDRAM_CYCLES(60), // Refresh Cycle Time tRFC/tREF  // 59-68 are same same number, so some parts that are less have same regiser stting
            SDRAM_CYCLES(70), // tXSR Exit Self-Refresh Time // 69-78 = same cycles.  Original part (not available) used 67
            SDRAM_CYCLES(12), // tRRD Row active to Row Active (or command period) // no parts use > 19, so same register setting
            SDRAM_CLOCKS(2) // tMRD Mode Register Set (to command) (tCK units) // Some parts are 14, but 16/2CK is worst case
            };
    LPC43xx_SDRAM_Init_32BitBus(&sdramConfig_IS42S32800J_6);
#endif

#if 1 // Optimal timing mode to support -6 parts in CAS3 only. // 6 ns is slowest speed rating actually loaded.
    static const T_LPC43xx_SDRAM_Configuration sdramConfig_IS42S32800J_6 = {
            UEZBSP_SDRAM_BASE_ADDR,
            UEZBSP_SDRAM_SIZE,
            SDRAM_CAS_3, 
            SDRAM_RAS_3, 
            LPC43xx_SDRAM_CLKOUT1,
            SDRAM_CLOCK_FREQUENCY,
            64, // ms
            4096, // cycles
            SDRAM_CYCLES(18), // tRP precharge to activate period - All used parts 18.
            SDRAM_CYCLES(42), // tRAS min // Used parts 42/45. Same register value up to 49
            SDRAM_CYCLES(70), // tXSR min // 69-78 = same cycles, no used parts list SREX. Original part (not available) used 67
            SDRAM_CYCLES(18), // tAPR/tRCD Active Command to Read Command // 20+ = same cycles, no parts used < 18
            SDRAM_CLOCKS(5), // tDAL Input Dat to ACT/REF (tCK units) // On existing parts this stays the same as long as CAS 3 is used.
            SDRAM_CYCLES(12), // tDPL/tWR = write recovery time = tCK + 1(depends on rise time) // Used parts have less than <=14 cycles
            SDRAM_CYCLES(60), // tRC Row Cycle Time or Command Period // 59-68 are same same number, so some parts that are less have same regiser stting
            SDRAM_CYCLES(60), // Refresh Cycle Time tRFC/tREF  // 59-68 are same same number, so some parts that are less have same regiser stting
            SDRAM_CYCLES(70), // tXSR Exit Self-Refresh Time // 69-78 = same cycles.  Original part (not available) used 67
            SDRAM_CYCLES(12), // tRRD Row active to Row Active (or command period) // no parts use > 19, so same register setting
            SDRAM_CLOCKS(2) // tMRD Mode Register Set (to command) (tCK units) // Some parts are 14, but 16/2CK is worst case
            };
    LPC43xx_SDRAM_Init_32BitBus(&sdramConfig_IS42S32800J_6);
#endif

//#else // future use placeholder

// This unit never had the old Micron SDRAM part loaded. It is no longer available for purchase.

//#endif

#if CONFIG_MEMORY_TEST_ON_SDRAM
    MemoryTest(UEZBSP_SDRAM_BASE_ADDR, UEZBSP_SDRAM_SIZE);
#endif
#endif

#ifdef CORE_M0
 // SDRAM is already init once we boot secondary core
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_ROMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the NOR Flash, QSPI, etc. 
 *      To use any RAM here set it with UEZ_PUT_SECTION(".IRAM", variable);
 *---------------------------------------------------------------------------*/
void UEZBSP_ROMInit(void)
{
#ifdef CORE_M4
  // Dummy code to keep various memory location declarations from optimizing out
#if (DO_NOT_INCLUDE_LPC43XX_CODE_READ_PROTECTION_1 == 1)
#else
#if (defined __clang__)
#else
   _crp1ptr = _crp1ptr;
#endif
#endif
#ifndef BBL_BASE_ADDRESS // Don't include anything in the second flash section in a bootloader application.
#if (defined __clang__)
#else
   _crp2ptr = _crp2ptr;
#endif
#endif
#endif

#ifdef CORE_M0

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
#ifdef CORE_M4
    const T_LPC43xx_PLL_Frequencies freq = {
            OSCILLATOR_CLOCK_FREQUENCY,

            // Run PLL0 at 48 MHz
            48000000,

            // Run PLL1
            PROCESSOR_OSCILLATOR_FREQUENCY,

            // Use the main oscillator of 12 MHz as a source
            LPC43xx_CLKSRC_SELECT_MAIN_OSCILLATOR,

            // Use PLL0 for the CCLK, PCLK, and EMC Clk source (before dividers)
            LPC43xx_CPU_CLOCK_SELECT_PLL_CLK,

            // Use PPL1 (alt) for the USB
            LPC43xx_USB_CLOCK_SELECT_ALT_PLL_CLK,

            // CPU Clock is PLL0 / 1 or 204 MHz / 1 = 204 MHz
            1,

            // PCLK is PLL0 / 2, or 204 MHz
            1,

            // EMC runs at PLL0 / 2, or 102 MHz
            1,

            // USB Clock = PLL1 / 1 (48 MHz / 1)
            1,

            LPC43xx_CLOCK_OUT_SELECT_CPU,
            1,
            EFalse, }; // no clock out by default

    LPC43xx_PLL_SetFrequencies(&freq);
#endif
#ifdef CORE_M0

#endif
}


void UEZBSP_HEARTBEAT_TOGGLE(void)
{ // Per manual can read set register to determine the set state.
    if((LPC_GPIO_PORT->SET[0] & (1 << 11)) == (1 << 11)){ // if on
      //LPC_GPIO1->CLR |= (1 << 13); // set off
      LPC_GPIO_PORT->CLR[0] |= 1 << 11;// off
    } else {
     // LPC_GPIO1->SET |= (1 << 13); // set on
      LPC_GPIO_PORT->SET[0] |= (1 << 11); // set on
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
 *      To use any RAM here set it with UEZ_PUT_SECTION(".IRAM", variable);
 *---------------------------------------------------------------------------*/
void UEZBSP_Pre_PLL_SystemInit(void)
{
#ifdef CORE_M4
    // Turn off LED before init clocks. 
    // Then it will only start blinking after RTOS
    // TODO need to test this code. It most likely doesn't work.
    LPC_SCU->SFSP1_4 = (0x3 << 3) | 0;
    LPC_GPIO_PORT->DIR[0] |= (1<<11);
    LPC_GPIO_PORT->CLR[0] |= 1 << 11;// off

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

#endif
#ifdef CORE_M0

#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_CPU_PinConfigInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Immediately configure the port pins
 *      Called after PLL is working at rated speed
  *      To use any RAM here set it with UEZ_PUT_SECTION(".IRAM", variable);
 *---------------------------------------------------------------------------*/
void UEZBSP_CPU_PinConfigInit(void)
{
#ifdef CORE_M4
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

#endif
#ifdef CORE_M0

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
 *      Safe to access SDRAM here.
 *---------------------------------------------------------------------------*/
void UEZBSP_Post_SystemInit(void)
{
#ifdef CORE_M4
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

#if (defined __GNUC__) // GCC
extern unsigned char __usbhostmem_start__[];
extern unsigned char __usbhostmem_end__[];
  // clear usbhostmem every boot
  memset((void *)__usbhostmem_start__, 0x0,
   __usbhostmem_end__-__usbhostmem_start__);
#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR
#pragma section = ".usbhostmem"
// TODO test and verify on IAR
memset((void *)__section_begin(".usbhostmem"), 0x0,
   __section_size(".usbhostmem"));
#elif (defined __CC_ARM) // ARM RealView Compiler
#else
  #error "Early memory clear not implemented yet for this compiler."
#endif

#endif
#ifdef CORE_M0

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
 #ifdef CORE_M4
    UEZPlatform_Timer1_Require();
 #endif
 #ifdef CORE_M0
    UEZPlatform_Timer3_Require();
 #endif
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
  if (UEZTimerOpen(
#ifdef CORE_M4
                   "Timer1",
#endif
#ifdef CORE_M0
                   "Timer3",
#endif
                   &G_StatsTimer) == UEZ_ERROR_NONE) {
    G_StatsTimerCallback.iTimer = G_StatsTimer;
    G_StatsTimerCallback.iMatchRegister = 1;
    G_StatsTimerCallback.iTriggerSem = UEZ_NULL_HANDLE;
    G_StatsTimerCallback.iCallback = RtosStatsTimerCallback;
    if (UEZTimerSetupRegularInterval(G_StatsTimer, 1,
            (PROCESSOR_OSCILLATOR_FREQUENCY) / (20*1000),
            &G_StatsTimerCallback) == UEZ_ERROR_NONE) {
      error = UEZTimerSetTimerMode(G_StatsTimer, TIMER_MODE_CLOCK);
      error = UEZTimerReset(G_StatsTimer);
 #ifdef CORE_M4
    InterruptSetPriority(TIMER1_IRQn, INTERRUPT_PRIORITY_LOW);
 #endif
 #ifdef CORE_M0
    InterruptSetPriority(M0_TIMER3_IRQn, INTERRUPT_PRIORITY_LOW);
 #endif
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
    // Ensure interrupts are turned off
    portDISABLE_INTERRUPTS();

    // We already set the GPIO into GPIO mode above
    //LPC_SCU->SFSP1_4 = (0x3 << 3) | 0;
    //LPC_GPIO_PORT->DIR[0] |= (1<<11);

    LPC_GPIO_PORT->CLR[0] |= 1 << 11; 
    // Blink our status led in a pattern, forever

    count = 0;
    while (1) {
        LPC_GPIO_PORT->SET[0] |= 1 << 11; // on
        for (i = 0; i < 15000000; i++)
            nop();
        //__nop();//asm ( "nop" );
        LPC_GPIO_PORT->CLR[0] |= 1 << 11;// off
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
 * Routine:  UEZPlatform_ADC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_ADC0_Require();
    ADCBank_Generic_Create("ADC0", "ADC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC1 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC1_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_ADC1_Require();
    ADCBank_Generic_Create("ADC1", "ADC1");
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
    LPC43xx_ADC0_0_Require(GPIO_NONE);//Uses dedicated pin
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
    LPC43xx_GPIO2_Require();

    LPC43xx_ADC0_1_Require(GPIO_P2_1);
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
    LPC43xx_GPIO7_Require();

    LPC43xx_ADC0_2_Require(GPIO_P7_22);
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
    LPC43xx_GPIO3_Require();

    LPC43xx_ADC0_3_Require(GPIO_P3_13);
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
    LPC43xx_GPIO3_Require();

    LPC43xx_ADC0_4_Require(GPIO_P3_12);
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
    LPC43xx_GPIO5_Require();

    LPC43xx_ADC0_5_Require(GPIO_NONE);//Uses dedicated pin
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
    LPC43xx_ADC0_6_Require(GPIO_NONE);//Uses dedicated pin
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
    LPC43xx_ADC0_7_Require(GPIO_NONE);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC1_0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC1_0 channel on the ADC1 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC1_0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO7_Require();

    LPC43xx_ADC1_0_Require(GPIO_P7_24);
    UEZPlatform_ADC1_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC1_3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC1_3 channel on the ADC1 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC1_3_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO7_Require();

    LPC43xx_ADC1_3_Require(GPIO_P7_20);
    UEZPlatform_ADC1_Require();
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

    LPC43xx_I2C0_Require(); // This one has only 1 pin set (at least supported in driver)
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

    LPC43xx_I2C1_Require(GPIO_P5_3, GPIO_P5_4);
    I2C_Generic_Create("I2C1", "I2C1", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2S_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2S drivers for talking to the expansion board.
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2S_Require(void)
{
    static const T_LPC43xx_I2S_Settings settings = {
            GPIO_P5_9,      // I2S_RX_SDA
            GPIO_PZ_4_P6_0, // I2S_RX_SCK
            GPIO_NONE,      // I2S_RX_WS
            GPIO_NONE,      // I2S_RX_MCLK
            GPIO_P4_14,     // I2S_TX_SDA
            GPIO_PZ_1_P3_0, // I2S_TX_SCK
            GPIO_P4_13,     // I2S_TX_WS
            GPIO_NONE,      // I2S_TX_MCLK
    };
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO4_Require();
    LPC43xx_GPIO5_Require();
    LPC43xx_GPIOZ_Require();

    LPC43xx_I2S_Require(&settings);
    Generic_I2S_Create("I2S", "I2S");
}

void UEZPlatform_GPDMA1_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_GPDMA1_Require();
    GPDMA_Generic_Create("GPDMA1", "GPDMA1");
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
 * Routine:  UEZPlatform_SD_MMC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SD MMC Peripheral
 *---------------------------------------------------------------------------*/
void UEZPlatform_SD_MMC_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO6_Require();
    LPC43xx_GPIOZ_Require();

    // Using the defaults for now
    const T_LPC43xx_SD_MMC_Pins pins = {
        GPIO_P6_3,      // DAT0
        GPIO_P6_4,      // DAT1
        GPIO_P6_5,      // DAT2
        GPIO_P6_6,      // DAT3
        GPIO_PZ_Z_PC_0, // CLK // Note that this pin isn't a GPIO and can't be pulled low using normal GPIO routine.
        GPIO_P6_9,      // CMD
        GPIO_P6_7,      // Card Detect
        GPIO_NONE,      // Write Protect Detect (no switch on microSD cards)
        GPIO_P6_15,     // Power Output Control, High On
    };
#if (defined SD_LOW_POWER_SUPP && SD_LOW_POWER_SUPP == 1)
#else
#endif        
    LPC43xx_SD_MMC_Require(&pins);
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

    Software_CRC_Create("CRC0");
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
            0, // backlight register (PWM0_1)
            UEZ_LCD_BACKLIGHT_FULL_PERIOD,
            UEZ_LCD_BACKLIGHT_FULL_PWR_ON,
            UEZ_LCD_BACKLIGHT_FULL_PWR_OFF,
            GPIO_P3_11,
            EFalse
    };
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO2_Require();
    LPC43xx_GPIO3_Require();

    LPC43xx_PWM0_A_Require(GPIO_P2_0);
    Backlight_Generic_PWMControlled_Create("Backlight", &settings);
}

#if(UEZ_PROCESSOR == NXP_LPC4357)
#if (DISABLE_BACKLIGHT_PWM_UEZ_API == 1)
typedef struct {
        TVUInt32  iCON;                   /*!< (@ 0x400A0000) PWM Control read address        */
        TVUInt32  iCON_SET;               /*!< (@ 0x400A0004) PWM Control set address         */
        TVUInt32  iCON_CLR;               /*!< (@ 0x400A0008) PWM Control clear address       */
        TVUInt32  iCAPCON;                /*!< (@ 0x400A000C) Capture Control read address    */
        TVUInt32  iCAPCON_SET;            /*!< (@ 0x400A0010) Capture Control set address     */
        TVUInt32  iCAPCON_CLR;            /*!< (@ 0x400A0014) Event Control clear address     */
        TVUInt32  iTC0;                   /*!< (@ 0x400A0018) Timer Counter register          */
        TVUInt32  iTC1;                   /*!< (@ 0x400A001C) Timer Counter register          */
        TVUInt32  iTC2;                   /*!< (@ 0x400A0020) Timer Counter register          */
        TVUInt32  iLIM0;                  /*!< (@ 0x400A0024) Limit register                  */
        TVUInt32  iLIM1;                  /*!< (@ 0x400A0028) Limit register                  */
        TVUInt32  iLIM2;                  /*!< (@ 0x400A002C) Limit register                  */
        TVUInt32  iMAT0;                  /*!< (@ 0x400A0030) Match register                  */
        TVUInt32  iMAT1;                  /*!< (@ 0x400A0034) Match register                  */
        TVUInt32  iMAT2;                  /*!< (@ 0x400A0038) Match register                  */
        TVUInt32  iDT;                    /*!< (@ 0x400A003C) Dead time register              */
        TVUInt32  iCCP;                   /*!< (@ 0x400A0040) Communication Pattern register  */
        TVUInt32  iCAP0;                  /*!< (@ 0x400A0044) Capture register                */
        TVUInt32  iCAP1;                  /*!< (@ 0x400A0048) Capture register                */
        TVUInt32  iCAP2;                  /*!< (@ 0x400A004C) Capture register                */
        TVUInt32  iINTEN;                 /*!< (@ 0x400A0050) Interrupt Enable read address   */
        TVUInt32  iINTEN_SET;             /*!< (@ 0x400A0054) Interrupt Enable set address    */
        TVUInt32  iINTEN_CLR;             /*!< (@ 0x400A0058) Interrupt Enable clear address  */
        TVUInt32  iCNTCON;                /*!< (@ 0x400A005C) Count Control read address      */
        TVUInt32  iCNTCON_SET;            /*!< (@ 0x400A0060) Count Control set address       */
        TVUInt32  iCNTCON_CLR;            /*!< (@ 0x400A0064) Count Control clear address     */
        TVUInt32  iINTF;                  /*!< (@ 0x400A0068) Interrupt flags read address    */
        TVUInt32  iINTF_SET;              /*!< (@ 0x400A006C) Interrupt flags set address     */
        TVUInt32  iINTF_CLR;              /*!< (@ 0x400A0070) Interrupt flags clear address   */
        TVUInt32  iCAP_CLR;               /*!< (@ 0x400A0074) Capture clear address           */
} T_LPC43xx_PWM_Registers;

T_LPC43xx_PWM_Registers *G_LPC43XX_PWM_register_Base = (T_LPC43xx_PWM_Registers *)0x400A0000;
#define PWM_MATCH_REGISTER 0

#undef UEZLCDBacklight
T_uezError UEZLCDBacklight(T_uezDevice aLCDDevice, TUInt32 aLevel) 
{
 (void) aLCDDevice;

  G_LPC43XX_PWM_register_Base->iINTEN_CLR = (1 << (1 + (PWM_MATCH_REGISTER * 4)));
  G_LPC43XX_PWM_register_Base->iCAPCON_SET = (1 << (18 + (PWM_MATCH_REGISTER)));
  G_LPC43XX_PWM_register_Base->iMAT0 = (aLevel*20); // only need to adjust level here
    
  return UEZ_ERROR_NONE;
} 
#endif 
#endif

void LCD_Pin_Power_Off(T_uezGPIOPortPin portPin)
{
    TUInt32 value;
    UEZGPIOOutput(portPin); 
    UEZGPIOSetMux(portPin, (portPin >> 8) >= 5 ? 4 : 0);
    value = ((portPin >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(portPin, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOClear(portPin);
}
void LCD_Power_Down(void)
{
    // Need to set all connected signals low to power down the LCD and stop current leakage.
    LCD_Pin_Power_Off(GPIO_PZ_3_P4_7); // LCD_DCLK
    LCD_Pin_Power_Off(GPIO_P2_6);      // LCD_ENAB_M
    LCD_Pin_Power_Off(GPIO_P2_5);      // LCD_FP
    LCD_Pin_Power_Off(GPIO_P3_14);     // LCD_LP
    // Color signals
    LCD_Pin_Power_Off(GPIO_P2_3);      // LCD_VD2
    LCD_Pin_Power_Off(GPIO_P2_2);      // LCD_VD3
    LCD_Pin_Power_Off(GPIO_P4_7);      // LCD_VD4
    LCD_Pin_Power_Off(GPIO_P4_6);      // LCD_VD5
    LCD_Pin_Power_Off(GPIO_P4_5);      // LCD_VD6
    LCD_Pin_Power_Off(GPIO_P4_4);      // LCD_VD7
    // no VD8 or VD9
    LCD_Pin_Power_Off(GPIO_P5_14);     // LCD_VD10
    LCD_Pin_Power_Off(GPIO_P5_13);     // LCD_VD11
    LCD_Pin_Power_Off(GPIO_P4_3);      // LCD_VD12
    LCD_Pin_Power_Off(GPIO_P5_26);     // LCD_VD13
    LCD_Pin_Power_Off(GPIO_P5_25);     // LCD_VD14
    LCD_Pin_Power_Off(GPIO_P5_24);     // LCD_VD15
    // no VD16 or VD17
    LCD_Pin_Power_Off(GPIO_P3_10);     // LCD_VD18
    LCD_Pin_Power_Off(GPIO_P3_9);      // LCD_VD19
    LCD_Pin_Power_Off(GPIO_P5_23);     // LCD_VD20
    LCD_Pin_Power_Off(GPIO_P5_22);     // LCD_VD21
    LCD_Pin_Power_Off(GPIO_P5_21);     // LCD_VD22
    LCD_Pin_Power_Off(GPIO_P5_20);     // LCD_VD23
    // Per NHD 5" or NHD 7" hold supply low > 5ms with some timing margin
    // Tpdt - Time of the voltage of supply being below 0.3V: Min: 5ms
    UEZTaskDelay(7); // Note: Can take a whole second to reach 0V, but not needed for spec.
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_LCD_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD device driver and its backlight
 *---------------------------------------------------------------------------*/
void UEZPlatform_LCD_Require(void)
{  
#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
    LPC43xx_GPIO2_Require(); // called in minimal require
#endif
    LPC43xx_GPIO3_Require();
#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
    LPC43xx_GPIO4_Require(); // called in minimal require
    LPC43xx_GPIO5_Require(); // called in minimal require
    LPC43xx_GPIOZ_Require(); // called in minimal require
#endif
    LCD_Power_Down();
    const T_LPC43xx_LCDController_Pins pins = {
            GPIO_NONE,  // LCD_PWR -- GPIO controlled
            GPIO_PZ_3_P4_7,  // LCD_DCLK
#if (ENABLE_LCD_DE_PIN == 1)
            GPIO_P2_6,  // LCD_ENAB_M
#else
            GPIO_NONE,  // DE Pin Not Used
#endif
            
#if (ENABLE_LCD_HSYNC_VSYNC_PINS == 1)
            GPIO_P2_5,  // LCD_FP
#else
            GPIO_NONE,  // VSYNC Not Used
#endif
            GPIO_NONE,  // LCD_LE
#if (ENABLE_LCD_HSYNC_VSYNC_PINS == 1)
            GPIO_P3_14,  // LCD_LP
#else
            GPIO_NONE,  // HSYNC Not Used
#endif
            {
                GPIO_NONE,  // LCD_VD0
                GPIO_NONE,  // LCD_VD1
                GPIO_P2_3,  // LCD_VD2
                GPIO_P2_2,  // LCD_VD3
                GPIO_P4_7,  // LCD_VD4
                GPIO_P4_6,  // LCD_VD5
                GPIO_P4_5,  // LCD_VD6
                GPIO_P4_4,  // LCD_VD7

                GPIO_NONE,  // LCD_VD8
                GPIO_NONE,  // LCD_VD9
                GPIO_P5_14, // LCD_VD10
                GPIO_P5_13,  // LCD_VD11
                GPIO_P4_3,  // LCD_VD12
                GPIO_P5_26,  // LCD_VD13
                GPIO_P5_25,  // LCD_VD14
                GPIO_P5_24,  // LCD_VD15

                GPIO_NONE,  // LCD_VD16
                GPIO_NONE,  // LCD_VD17
                GPIO_P3_10,  // LCD_VD18
                GPIO_P3_9,  // LCD_VD19
                GPIO_P5_23,  // LCD_VD20
                GPIO_P5_22,  // LCD_VD21
                GPIO_P5_21,  // LCD_VD22
                GPIO_P5_20,  // LCD_VD23
            },

            GPIO_NONE,  // LCD_CLKIN
            GPIO_P3_15, //GPIO Power Control
            EFalse,
            0,

    };
    T_halWorkspace *p_lcdc;
    T_uezDeviceWorkspace *p_lcd;
    
    // Below we disable some code bloat when building a bootloader.
#if (DISABLE_BACKLIGHT_PWM_UEZ_API == 1)
    // No backlight device or workspace created.
#else
    T_uezDevice backlight;
    T_uezDeviceWorkspace *p_backlight = 0;
#endif

#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
    // Don't use LPC timer driver. It is only used for 167 ms turn on/off on this LPC.
#else

 #if (LCD_BACKLIGHT_FREERTOS_TIMER==1)
    // We no longer require wasting the peripheral timer for backlight turn on delay
 #else
    UEZPlatform_Timer0_Require();
 #endif

#endif

#if (ENABLE_LCD_DE_PIN == 0)
      // set DE pin to input mode
      TUInt32 valueDE = ((GPIO_P2_6 >> 8) & 0x7) >= 5 ? 4 : 0;
      UEZGPIOSetMux(GPIO_P2_6, (GPIO_P2_6 >> 8) >= 5 ? 4 : 0);
      UEZGPIOControl(GPIO_P2_6, GPIO_CONTROL_SET_CONFIG_BITS, valueDE);
      //UEZGPIOClear(GPIO_P2_6);
      //UEZGPIOSet(GPIO_P2_6);
      //UEZGPIOOutput(GPIO_P2_6);
      UEZGPIOInput(GPIO_P2_6);
      UEZGPIOLock(GPIO_P2_6);
#endif

#if (ENABLE_LCD_HSYNC_VSYNC_PINS == 0)
      // set VSYNC pin low
      TUInt32 valueHV = ((GPIO_P2_5 >> 8) & 0x7) >= 5 ? 4 : 0;
      UEZGPIOSetMux(GPIO_P2_5, (GPIO_P2_5 >> 8) >= 5 ? 4 : 0);
      UEZGPIOControl(GPIO_P2_5, GPIO_CONTROL_SET_CONFIG_BITS, valueHV);
      UEZGPIOClear(GPIO_P2_5);    
      UEZGPIOOutput(GPIO_P2_5);
      UEZGPIOLock(GPIO_P2_5); 
      // set HSYNC pin low
      valueHV = ((GPIO_P3_14 >> 8) & 0x7) >= 5 ? 4 : 0;
      UEZGPIOSetMux(GPIO_P3_14, (GPIO_P3_14 >> 8) >= 5 ? 4 : 0);
      UEZGPIOControl(GPIO_P3_14, GPIO_CONTROL_SET_CONFIG_BITS, valueHV);
      UEZGPIOClear(GPIO_P3_14);    
      UEZGPIOOutput(GPIO_P3_14);
      UEZGPIOLock(GPIO_P3_14);
#endif
   
#if(UEZ_PROCESSOR == NXP_LPC4357)
#if (DISABLE_BACKLIGHT_PWM_UEZ_API == 1)    
   // setup PWM output pin on P4_0/GPIO2[0]/MCOA0/ match register 0
   UEZGPIOControl(GPIO_P2_0, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(1));
                    
  // Stop the timer, otherwise we cannot change it
   G_LPC43XX_PWM_register_Base->iCON_CLR = (1 << (0 + (PWM_MATCH_REGISTER * 8)));

   G_LPC43XX_PWM_register_Base->iLIM0 = 5100; // Set the master match limit register (value for 100% backlight)
    
   G_LPC43XX_PWM_register_Base->iTC0 = 0; // Reset the counter

   G_LPC43XX_PWM_register_Base->iCON_SET = (1 << (1 + (PWM_MATCH_REGISTER * 8)));
   G_LPC43XX_PWM_register_Base->iCON_SET = (1 << (2 + (PWM_MATCH_REGISTER * 8)));

    // Let the counter run
   G_LPC43XX_PWM_register_Base->iCON_SET = (1 << (0 + (PWM_MATCH_REGISTER * 8)));
                   
    // Set to single output mode
   G_LPC43XX_PWM_register_Base->iCON_CLR = (1 << (1 + (PWM_MATCH_REGISTER * 8)));

   UEZLCDBacklight(0, 0); // sets initial match register to backlight 0
   
   // Enable output
   G_LPC43XX_PWM_register_Base->iCON_SET = (1 << (0 + (PWM_MATCH_REGISTER * 8)));
      
   TUInt32 value = ((GPIO_P3_11 >> 8) & 0x7) >= 5 ? 4 : 0;
   UEZGPIOOutput(GPIO_P3_11); 
   UEZGPIOSetMux(GPIO_P3_11, (GPIO_P3_11 >> 8) >= 5 ? 4 : 0);
   UEZGPIOControl(GPIO_P3_11, GPIO_CONTROL_SET_CONFIG_BITS, value);
   UEZGPIOClear(GPIO_P3_11); // enables power to backlight driver
#else
    UEZPlatform_Backlight_Require();
#endif
#endif    

    LPC43xx_LCDController_Require(&pins);
    extern const T_uezDeviceInterface *UEZ_LCD_INTERFACE_ARRAY[];

    // Need to register LCD device and use existing LCD Controller
    UEZDeviceTableRegister(
            "LCD",
            (T_uezDeviceInterface *)UEZ_LCD_INTERFACE_ARRAY[UEZ_LCD_COLOR_DEPTH],
            0,
            &p_lcd);

    HALInterfaceFind("LCDController", &p_lcdc);
    
#if (DISABLE_BACKLIGHT_PWM_UEZ_API == 1)
    
    ((DEVICE_LCD *)(p_lcd->iInterface))->Configure(
            p_lcd,
            (HAL_LCDController **)p_lcdc,
            LCD_DISPLAY_BASE_ADDRESS,
            (DEVICE_Backlight **) 0);
#else
    UEZDeviceTableFind("Backlight", &backlight);
    UEZDeviceTableGetWorkspace(backlight, (T_uezDeviceWorkspace **)&p_backlight);
    ((DEVICE_LCD *)(p_lcd->iInterface))->Configure(
            p_lcd,
            (HAL_LCDController **)p_lcdc,
            LCD_DISPLAY_BASE_ADDRESS,
            (DEVICE_Backlight **)p_backlight);
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Touchscreen_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_Touchscreen_Require(char *aName)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO0_Require();

    UEZPlatform_I2C0_Require();
#if 1
    //PCAP IRQ, P6_6, GPIO0[5]
    UEZGPIOControl(GPIO_P0_5, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P0_5, (GPIO_P0_5 >> 8) >= 5 ? 4 : 0);
    UEZGPIOInput(GPIO_P0_5);
    // This enables input buffer, which is required for input mode operation, at least with GPIO mode.
    // TODO verify if interrupt actually works. I cannot find in UM that ISR requires this.
    UEZGPIOControl(GPIO_P0_5, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0);
#endif

    Touchscreen_FT5306DE4_Create(aName, "I2C0", GPIO_P0_5, GPIO_P5_15);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Flash0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Flash0 device driver for the external QSPI Flash
 *---------------------------------------------------------------------------*/
void UEZPlatform_Flash0_Require(void)
{
    DEVICE_CREATE_ONCE();
    Flash_NXP_LPC_SPIFI_M4_Create("Flash0");
    
#ifdef CORE_M4
    InterruptSetPriority(SPIFI_IRQn, INTERRUPT_PRIORITY_NORMAL);
#endif
    // No ISR on M0 cores!
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
    Flash_NXP_LPC43xx_Create("IAP");
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
    static const T_LPC43xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            GPIO_NONE,      // T0_CAP[2]
            GPIO_NONE,      // T0_CAP[3]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC43xx_Timer0_Require(&settings);
    Timer_Generic_Create("Timer0", "Timer0");
}


#if (USE_TIMER_SYSTICK == 1) // switch RTOS tick to Timer 0 if possible.
// For re-implementing the system tick here we need to have these defines to allow falling back to it.
#ifndef configSYSTICK_CLOCK_HZ
	#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
	/* Ensure the SysTick is clocked at the same frequency as the core. */
	#define portNVIC_SYSTICK_CLK_BIT	( 1UL << 2UL )
#else
	/* The way the SysTick is clocked is not modified in case it is not the same
	as the core. */
	#define portNVIC_SYSTICK_CLK_BIT	( 0 )
#endif
#define portNVIC_SYSTICK_CTRL_REG			( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG			( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
/* ...then bits in the registers. */

#define portNVIC_SYSTICK_INT_BIT			( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT			( 1UL << 0UL )

extern void xPortSysTickHandler(void);

void TickTimerCallback (struct _T_uezTimerCallback *aCallbackData)
{
    xPortSysTickHandler();
}

void vPortSetupTimerInterrupt(void) {
  UEZPlatform_Timer0_Require();

  T_uezError error = UEZTimerOpen("Timer0", &G_sysTickTimer);
  if (!error) {
      G_sysTickCallback.iTimer = G_sysTickTimer;
      G_sysTickCallback.iMatchRegister = 0;
      G_sysTickCallback.iTriggerSem = UEZ_NULL_HANDLE;
      G_sysTickCallback.iCallback = TickTimerCallback;
      G_sysTickCallback.iData = 0;
    // Set the time for the Timer interval, second param is match register 0 and must match 0 above.
    error = UEZTimerSetupRegularInterval(G_sysTickTimer, 0, (PROCESSOR_OSCILLATOR_FREQUENCY/1000), &G_sysTickCallback);
    if (error == UEZ_ERROR_NONE) {      
      error = UEZTimerSetTimerMode(G_sysTickTimer, TIMER_MODE_CLOCK); // Set clock mode for timer
      if (error == UEZ_ERROR_NONE) {
        /*InterruptUnregister(TIMER0_IRQn);
        InterruptRegister(
        TIMER0_IRQn,
        (TISRFPtr)TimerCallback,
        INTERRUPT_PRIORITY_LOWEST,
        "TimerISR");*/
        
        error = UEZTimerReset(G_sysTickTimer); 
        if (error == UEZ_ERROR_NONE) {
          error = UEZTimerEnable(G_sysTickTimer);
          if (error == UEZ_ERROR_NONE) { // We have actual timer running and can exit. Otherwise setup systick.
            return;
          }
        }
      }
    }
  }

  /* Configure SysTick to interrupt at the requested rate. */
  portNVIC_SYSTICK_LOAD_REG = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
  portNVIC_SYSTICK_CTRL_REG = (portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT);
}

#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer1 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer1_Require(void)
{
    static const T_LPC43xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T1_CAP[0]
            GPIO_NONE,      // T1_CAP[1]
            GPIO_NONE,      // T1_CAP[2]
            GPIO_NONE,      // T1_CAP[3]
            },
            {
            GPIO_NONE,      // T1_MAT[0]
            GPIO_NONE,      // T1_MAT[1]
            GPIO_NONE,      // T1_MAT[2]
            GPIO_NONE,      // T1_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC43xx_Timer1_Require(&settings);
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
    static const T_LPC43xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T2_CAP[0]
            GPIO_NONE,      // T2_CAP[1]
            GPIO_NONE,      // T2_CAP[2]
            GPIO_NONE,      // T2_CAP[3]
            },
            {
            GPIO_NONE,      // T2_MAT[0]
            GPIO_NONE,      // T2_MAT[1]
            GPIO_NONE,      // T2_MAT[2]
            GPIO_NONE,      // T2_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC43xx_Timer2_Require(&settings);
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
    static const T_LPC43xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T3_CAP[0]
            GPIO_NONE,      // T3_CAP[1]
            GPIO_NONE,      // T3_CAP[2]
            GPIO_NONE,      // T3_CAP[3]
            },
            {
            GPIO_NONE,      // T3_MAT[0]
            GPIO_NONE,      // T3_MAT[1]
            GPIO_NONE,      // T3_MAT[2]
            GPIO_NONE,      // T3_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC43xx_Timer3_Require(&settings);
    Timer_Generic_Create("Timer3", "Timer3");
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

   LPC43xx_PWM0_Require();

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

   LPC43xx_PWM1_Require();

   PWM_Generic_Create("PWM1", "PWM1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM2 driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM2_Require(void)
{
    DEVICE_CREATE_ONCE();

   LPC43xx_PWM2_Require();

   PWM_Generic_Create("PWM2", "PWM2");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_DAC0_Offboard_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the DAC0 device driver
 *      NOTE: Goes to P2.4
 *---------------------------------------------------------------------------*/
void UEZPlatform_DAC0_Offboard_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_DAC0_Require(GPIO_P2_4); // gpio require is inside routine
    DAC_Generic_Create("DAC0", "DAC0");
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_DAC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the DAC0 device driver
 *      NOTE: Goes to ADC0/DAC0 pin
 *---------------------------------------------------------------------------*/
void UEZPlatform_DAC0_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_DAC0_Require(GPIO_NONE);
    DAC_Generic_Create("DAC0", "DAC0");
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

    UEZPlatform_I2C0_Require();
    AudioAmp_LM48110_Create("Amp0", "I2C0", 0x1F);
    UEZAudioAmpOpen("Amp0", &G_Amp);
    UEZAudioAmpSetLevel(G_Amp, UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110);
    G_Amp_In_Use = AUDIO_AMP_LM48110;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI50WVN_AudioMixerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
static T_uezError UEZGUI50WVN_AudioMixerCallback(
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
                if(G_Amp_In_Use == AUDIO_AMP_LM48110) { // enforce maximum volume based on AMP
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110 /255);
                } else if(G_Amp_In_Use == AUDIO_AMP_WOLFSON) {
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL /255);
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
    UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, &UEZGUI50WVN_AudioMixerCallback);
    
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
    
    UEZPlatform_AudioMixer_Require(); // will mute the amp
    //UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    //UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    
    UEZPlatform_DAC0_Require();    // DAC output, currently this generates noise so we want to mute first.
    //UEZPlatform_DAC0_Offboard_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM_I2C_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal EEPROM in the LPC43xx
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_LPC43xx_Require(void)
{
    DEVICE_CREATE_ONCE();
    EEPROM_NXP_LPC43xx_Create("EEPROM0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM_I2C_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the external EEPROM over I2C.
 *      MAC address EEPROM, this EEPROM should not be used by the application
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_I2C_Require(void)
{
    const T_EEPROMConfig eeprom_config = {EEPROM_CONFIG_R1EX24512ASAS0A};

    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    EEPROM16_Generic_I2C_Create("EEPROM1", "I2C0", 0xA0>>1, &eeprom_config);
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
    LPC43xx_GPIO7_Require();
    LPC43xx_UART0_Require(GPIO_P7_24, GPIO_P7_25, GPIO_NONE, GPIO_NONE);
    UEZPlatform_Console_FullDuplex_UART_Require("UART0", aWriteBufferSize,
            aReadBufferSize);
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
#if 1
    LPC43xx_GPIO5_Require();
    LPC43xx_UART3_Require(GPIO_P5_3, GPIO_P5_4, GPIO_NONE, GPIO_NONE, GPIO_NONE);
#else
    LPC43xx_GPIO4_Require();
    LPC43xx_GPIO5_Require();
    LPC43xx_UART3_Require(GPIO_P4_15, GPIO_P5_17, GPIO_NONE, GPIO_NONE, GPIO_NONE);
#endif
    UEZPlatform_Console_FullDuplex_UART_Require("UART3", aWriteBufferSize,
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
    // UART0 on P7.24/P7.25
    LPC43xx_GPIO7_Require();
    LPC43xx_UART0_Require(GPIO_P7_24, GPIO_P7_25, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART0", "UART0",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    LPC43xx_GPIO6_Require();
    LPC43xx_UART1_Require(GPIO_P6_12, GPIO_P6_13, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART1", "UART1",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART2.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    LPC43xx_GPIO4_Require();
    LPC43xx_UART2_Require(GPIO_P4_8, GPIO_P4_9, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART2", "UART2",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART3.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
#if 0
    /* XBEE, Primary Expansion connector */
    LPC43xx_GPIO5_Require();
    LPC43xx_UART3_Require(GPIO_P5_3, GPIO_P5_4, GPIO_NONE, GPIO_NONE, GPIO_NONE);
#elif 0
    /* Secondary Expansion Connector */
    LPC43xx_GPIO4_Require();
    LPC43xx_GPIO5_Require();
    LPC43xx_UART3_Require(GPIO_P4_15, GPIO_P5_17, GPIO_NONE, GPIO_NONE, GPIO_NONE);
#elif 1
    /* Pmod Connector */
    LPC43xx_GPIO7_Require();
    LPC43xx_UART3_Require(GPIO_P7_17, GPIO_P7_18, GPIO_NONE, GPIO_NONE, GPIO_NONE);
#endif

    Serial_Generic_FullDuplex_Stream_Create("UART3", "UART3",
            aWriteBufferSize, aReadBufferSize);
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

    LPC43xx_GPIO7_Require();
    LPC43xx_GPIOZ_Require();
    LPC43xx_SSP0_Require(GPIO_PZ_8_PF_0, GPIO_P7_17, GPIO_P7_18, GPIO_NONE);

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
    LPC43xx_GPIO0_Require();
    LPC43xx_GPIO7_Require();
    LPC43xx_GPIOZ_Require();
    LPC43xx_SSP1_Require(GPIO_PZ_0_P1_19, GPIO_P7_20, GPIO_P0_1, GPIO_NONE);

    SPI_Generic_Create("SSP1", "SSP1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Speaker_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Speaker device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_Speaker_Require(void)
{
    const T_ToneGenerator_Generic_Timer_Settings settings = {
            "Timer3",
            3, // MAT3[3]
            GPIO_P5_19,
            4, // 0 mux = GPIO
            1, // 3 mux = T3_MAT0
    };

    DEVICE_CREATE_ONCE();
    UEZPlatform_Timer3_Require();
    ToneGenerator_Generic_Timer_Create("Speaker", &settings);
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
    
    // Come up with some mechanism (non-volatile storage, or LPC OTP, or QSPI OTP, etc)
    // to load a unique calibration value for each LPC43XX internal RTC. This is the only
    // way to get passable accuracy, otherwise you may be off a whole minute in a month.
    TUInt32 aPerUnitRtcCalibrationValue = 0;
    LPC43xx_RTC_Require(ETrue, aPerUnitRtcCalibrationValue);
    RTC_Generic_Create("RTC", "RTC");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_IRTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal RTC device driver.  Not compatible with the
 *      external RTC.
 *---------------------------------------------------------------------------*/
void UEZPlatform_RTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_IRTC_Require();
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

    UEZPlatform_I2C0_Require();
    Temp_NXP_LM75A_Create("Temp0", "I2C0", 0x92 >> 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Accel0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Accelerometer 0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Accel0_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();

    ST_Accelo_LIS3DH_I2C_Create("Accel0", "I2C0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EMAC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the EMAC controller for networking.
 *---------------------------------------------------------------------------*/
void UEZPlatform_EMAC_Require(void)
{
    // This EMAC is RMII (less pins)// Due to RMII require, struct will be different from 4088.
    const T_LPC43xx_EMAC_Settings emacSettings = {
            GPIO_P0_1,          // ENET_TX_ENn      = PIN_P0_1_ENET_TXEN
            GPIO_NONE,          // ENET_TX_TXD[3]   = not used for RMII
            GPIO_NONE,          // ENET_TX_TXD[2]   = not used for RMII
            GPIO_P0_15,         // ENET_TX_TXD[1]   = PIN_P1_20_ENET_TXD1
            GPIO_P0_13,         // ENET_TX_TXD[0]   = PIN_P1_18_ENET_TXD0
            GPIO_NONE,          // ENET_TX_ER       = not used for RMII
            GPIO_PZ_0_P1_19,    // ENET_TX_CLK      = PIN_P1_19_ENET_REFCLK (input into LPC)
            GPIO_P0_3,          // ENET_RX_DV       = ENET_RX_DV is proper mode for RMII
            GPIO_NONE,          // ENET_RXD[3]      = not used for RMII
            GPIO_NONE,          // ENET_RXD[2]      = not used for RMII
            GPIO_P0_0,          // ENET_RXD[1]      = PIN_P0_0_ENET_RXD1
            GPIO_P0_2,          // ENET_RXD[0]      = PIN_P1_15_ENET_RXD0
            GPIO_P6_8,          // ENET_RX_ER       = PIN_P6_8_ENET_RX_ER
            GPIO_NONE,          // ENET_RX_CLK      = not used for RMII
            GPIO_NONE,          // ENET_COL         = not used for RMII
            GPIO_NONE,          // ENET_CRS         = not used for RMII
            GPIO_P6_0,          // ENET_MDC         = PIN_P6_0_ENET_MDC
            GPIO_P0_12,         // ENET_MDIO        = PIN_P1_17_ENET_MDIO
    };

    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO0_Require();
    LPC43xx_GPIO6_Require();
    LPC43xx_GPIOZ_Require();

    LPC43xx_EMAC_RMII_Require(&emacSettings);
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

#if (UEZ_ENABLE_TCPIP_STACK == 1)
    // Wired network needs an EMAC
    UEZPlatform_EMAC_Require();
    // Create the network driver for talking to lwIP on a wired
    // network.
    Network_lwIP_Create("WiredNetwork0");
#endif
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
  return 52000000UL;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MCI_TransferMode
 *---------------------------------------------------------------------------*
 * Description:
 *      For SD/MCI mode select 1-bit or 4-bit mode data transfer.
 *---------------------------------------------------------------------------*/
TUInt32 UEZPlatform_MCI_TransferMode(void)
{
  return UEZ_MCI_BUS_4BIT_WIDE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS1_SD_MMC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS1 using MMC SDCard drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS1_SD_MMC_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_SD_MMC_Require();
    MassStorage_SDCard_SD_MMC_Create("MS1", "SD_MMC");
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

    UEZPlatform_MS1_SD_MMC_Require();
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS1", &ms1);
    UEZDeviceTableGetWorkspace(ms1, (T_uezDeviceWorkspace **)&p_ms1);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortA_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port on USB1, off board USB
 *      P9_5 Connected to MIC2025-2YM, active low power enable
 
 * PPWR-> VBUS drive signal (towards external charge pump or power management unit);
 * indicates that VBUS must be driven (active HIGH).
 * Add a pull-down resistor to disable the power switch at reset. This signal has
 * opposite polarity compared to the USB_PPWR used on other NXP LPC parts.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortA_Require(void)
{
#if (UEZ_ENABLE_USB_HOST_STACK == 1)
    extern void LPCUSBLib_USB1_IRQHandler(void);
    DEVICE_CREATE_ONCE();

    G_USBHostDriveNumber = 1;
#if 1
    LPC43xx_GPIO5_Require();
    UEZGPIOSetPull(GPIO_P5_5, GPIO_PULL_NONE); // disable pulls on the offboard USB1_VBUS pin. We probably aren't using this pin in Host mode.

    // If the expansion board USB1H_PPWR is active high you can use the USB mode.
    // Otherwise we MUST set GPIO low to enable power on existing boards.
    //UEZGPIOSetMux(GPIO_P5_18, 2); // Turn on P9_5 USB1_PPWR USB1 peripheral mode
    UEZGPIOControl(GPIO_P5_18, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P5_18, (GPIO_P5_18 >> 8) >= 5 ? 4 : 0); // Turn on GPIO mode
    UEZGPIOClear(GPIO_P5_18);
    UEZGPIOOutput(GPIO_P5_18);
    // We can take apparently over 2.5ms for power to rise with a flash drive connected,
    // so delay to make sure it is fully on first to match other port.
    UEZTaskDelay(5);
#endif
    
    // Note: We don't have GPIO power control in the USB software, so a manual
    // power reset should be done here. It seems this isn't ever necessary.

    InterruptRegister(USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
        INTERRUPT_PRIORITY_NORMAL, "USB1");
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortB_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port on USB0, on board USB
 *      P6_3 Connected to MIC2025-1YM, active high power enable
 *
 * PPWR-> VBUS drive signal (towards external charge pump or power management unit);
 * indicates that VBUS must be driven (active HIGH).
 * Add a pull-down resistor to disable the power switch at reset. This signal has
 * opposite polarity compared to the USB_PPWR used on other NXP LPC parts.

 * In a standard EHCI controller design, the EHCI host controller driver detects a Full speed
 * (FS) or Low speed (LS) device by noting if the port enable bit is set after the port reset
 * operation. The port enable will only be set in a standard EHCI controller implementation
 * after the port reset operation and when the host and device negotiate a High-Speed
 * connection (i.e. Chirp completes successfully). Since this controller has an embedded
 * Transaction Translator, the port enable will always be set after the port reset operation
 * regardless of the result of the host device chirp result and the resulting port speed will be
 * indicated by the PSPD field in PORTSC1 (see Section 25.6.15).
 *
 *  Per documentation we would need special handling to allow for proper speed detection
 *  so currently we will force full-speed mode until future HW/SW work is performed.
 *  Existing HW may not be suitable for proper operation at high-speed mode.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortB_Require(void)
{
#if (UEZ_ENABLE_USB_HOST_STACK == 1)
    extern void LPCUSBLib_USB0_IRQHandler(void);
    DEVICE_CREATE_ONCE();

    G_USBHostDriveNumber = 0;
#if 1
    LPC43xx_GPIO3_Require();

    // GPIO operation, not needed here.
    //T_uezGPIOPortPin currentPortPin = GPIO_P3_2;        
    //UEZGPIOSet(currentPortPin);
    //UEZGPIOOutput(currentPortPin);
    //UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0

    // USB0_VBUS isn't a GPIO pin so we can't change the pull resistor or any function change. Has a unique internal pull-down.
    UEZGPIOSetMux(GPIO_P3_2, 1); // Turn on P6_3 USB0_PPWR USB 0 peripheral mode func 1
#endif
    InterruptRegister(USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
        INTERRUPT_PRIORITY_NORMAL, "USB0");
#endif
}

void UEZPlatform_MS0_Connected(void *aWorkspace) // called from IUpdate
{
    PARAM_NOT_USED(aWorkspace);
    TUInt32 driveNum = G_ms0_driveNum;
    T_uezDevice ms0;
    T_uezDeviceWorkspace *p_ms0;

#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
#else
    //printf("UEZGUI MS0 Connected drive %u\n", driveNum);
#endif
    UEZDeviceTableFind("MS0", &ms0);
    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
    // Reregister the device (doing a mount)
    FATFS_RegisterMassStorageDevice(driveNum, (DEVICE_MassStorage **)p_ms0);
    // USB drive not actually initialized yet until we read a file!
}

void UEZPlatform_MS0_Disconnected(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    TUInt32 driveNum = G_ms0_driveNum;

#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
#else
    //printf("UEZGUI MS0 Disconnected drive %u\n", driveNum);
#endif
    // Unregister the device (doing a unmount)
    FATFS_UnregisterMassStorageDevice(driveNum);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS0 using USBHost flash drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS0_Require(TUInt32 aUSBNumber)
{
    PARAM_NOT_USED(aUSBNumber);
#if (UEZ_ENABLE_USB_HOST_STACK == 1)
    T_MassStorage_LPCUSBLib_Callbacks ms0Callbacks = {
            UEZPlatform_MS0_Connected,
            UEZPlatform_MS0_Disconnected
    };
    DEVICE_CREATE_ONCE();

    MassStorage_LPCUSBLib_Create("MS0", aUSBNumber, &ms0Callbacks, 0);
#endif
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

    if(G_USBHostDriveNumber == 0xFF){
        UEZFailureMsg("Must Require Port A or B before the FlashDrive\n");
    }

    // Now put the MS0 in the filesystem using the USB
    UEZPlatform_MS0_Require(G_USBHostDriveNumber);
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS0", &ms0);
    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms0);
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
void UEZPlatform_System_Reset(void){
    // By default use HW reset pin on this board.
    // Use low level code to save code space and allow for calling outside of API.
    LPC_SCU->SFSPE_14 = 0x4; //PIN_HW_RESET
    LPC_GPIO_PORT->CLR[7] |= (1<<14); // set low
    LPC_GPIO_PORT->DIR[7] |= (1<<14); // set output
    
    /* From ES_LPC435x/3x/2x/1x Rev 6.8:    Problem:
     * On the LPC43xx, PERIPH_RST is not functional. CMSIS call NVIC_SystemReset() uses
     * PERIPH_RST internally and is also non-functional.
     * Work-around:
     * There is no work-around. To reset the entire chip, use the CORE_RST instead of using
     * CMSIS call NVIC_SystemReset() or PERIPH_RST.*/
}

void UEZPlatform_TouchscreenReplay_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_Touchscreen_Require("TouchscreenReal");
    //Touchscreen_Replay_Create("Touchscreen", "TouchscreenReal");
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
    LPC43xx_Watchdog_Require();
    Watchdog_Generic_Create("Watchdog", "Watchdog");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_WirelessNetwork0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wireless connections.
 *---------------------------------------------------------------------------*/
#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
#else
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
        GPIO_P7_16, // iSPIChipSelect = WIFI_CSN = P2.23
        GPIO_P5_12, // iDataReadyIO = WIFI_INTR
        GPIO_NONE,  // iProgramMode = P1.6_MCIDAT0_PWM0.4
        GPIO_NONE,  // iSPIMode = WIFI_MODE
        GPIO_NONE,  // iSRSTn = WIFI_SRSTn AND'd with RESET_OUTn
        GPIO_NONE,  // iWIFIFactoryRestore = WIFI_FAC_RST
        };

    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO5_Require();
    LPC43xx_GPIO7_Require();

    //Set CS to GPIO Outpit
    UEZGPIOControl(GPIO_P7_16, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P7_16, 4);
    UEZGPIOSet(GPIO_P7_16);
    UEZGPIOOutput(GPIO_P7_16);

    //Set int32_t INTR pin to an input.
    UEZGPIOControl(GPIO_P5_12, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P5_12, 4);
    UEZGPIOInput(GPIO_P5_12);

    UEZPlatform_SSP0_Require();
    Network_GainSpan_Create("WirelessNetwork0", &spi_settings);
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_WirelessNetwork1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wireless connections.
 *---------------------------------------------------------------------------*/
#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
#else
void UEZPlatform_WirelessNetwork1_Require(void)
{
    const T_ESPWROOM32_Network_UARTSettings uart_settings = 
    {
        // UART Settings
        "UART3",// UART Name
        GPIO_P7_0, // Peripheral Reset
    };

    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO7_Require();

    //Set Reset to GPIO Output
    UEZGPIOControl(GPIO_P7_0, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P7_0, 4);
    UEZGPIOSet(GPIO_P7_0);
    UEZGPIOOutput(GPIO_P7_0);

    UEZPlatform_FullDuplex_UART3_Require(1024, 1024);

    Network_ESPWROOM32_Create("WirelessNetwork1", &uart_settings);
}
#endif

#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
#else
void UEZPlatform_USBHost_USB0_Serial_Require(void)
{
#if (UEZ_ENABLE_USB_HOST_STACK == 1)
    UEZGPIOSetMux(GPIO_P3_2, 1); // Turn on USB0_PPWR USB 0  
    Stream_LPCUSBLib_SerialHost_Create("SerialHost", 1); // USB0
    Stream_LPCUSBLib_SerialHost_FTDI_Create("USB0Serial", 0, 1024, 1024); // USB0
#endif  
}

void UEZPlatform_USBHost_USB1_Serial_Require(void)
{
#if (UEZ_ENABLE_USB_HOST_STACK == 1)
    UEZGPIOSetMux(GPIO_P5_18, 2); // Turn on USB0_PPWR USB1
    Stream_LPCUSBLib_SerialHost_FTDI_Create("USB1Serial", 1, 1024, 1024); // USB1
#endif  
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ButtonBoard_Require
 *---------------------------------------------------------------------------*/
/**
 *  Setup the I2C GPIO drivers for talking to the button board
 */
/*---------------------------------------------------------------------------*/
/*void UEZPlatform_ButtonBoard_Require(void)
{
    static const T_GPIOKeypadAssignment keyAssignment[] = {
        { 0, KEY_ENTER },
        { 1, KEY_ARROW_LEFT },
        { 2, KEY_ARROW_RIGHT },
        { 3, KEY_ARROW_DOWN },
        { 4, KEY_ARROW_UP },
        { 0, 0 },
    };

    DEVICE_CREATE_ONCE();
    UEZPlatform_I2C0_Require();
    GPIO_PCF8574T_Create("GPIO:PCF8574T", UEZ_GPIO_PORT_EXT1, "I2C0", 0x48>>1);
    Keypad_Generic_GPIO_Create("BBKeypad", UEZ_GPIO_PORT_EXT1, keyAssignment, 5,
        KEYPAD_LOW_TRUE_SIGNALS, 
        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 7), 
        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 6));
}*/

// Don't enable console anymore in minimal requires, so that we can use this for GUI only bootloader, etc.
void UEZPlatform_Minimal_Require(void)
{
#ifdef CORE_M4
#if (DISABLE_FEATURES_FOR_BOOTLOADER==1)
    //LPC43xx_GPIO3_Require();
    //UEZPlatform_CRC0_Require(); // Adds a few extra bytes if we require it later in code.
    //UEZPlatform_Flash0_Require(); // currently this won't fit in BBL

    // TODO could read normal QSPI or OTP region here, but will be too much space for any bootloader.

    //LPC43xx_GPIO0_Require();
    //LPC43xx_GPIO2_Require();
    //LPC43xx_GPIO4_Require();
    //LPC43xx_GPIO5_Require();
    //LPC43xx_GPIOZ_Require();
#else
    LPC43xx_GPIO3_Require();
    UEZPlatform_CRC0_Require();
    UEZPlatform_Flash0_Require();

    // TODO could read normal QSPI or OTP region here.

    LPC43xx_GPIO0_Require();
    //LPC43xx_GPIO1_Require();
    LPC43xx_GPIO2_Require();
    LPC43xx_GPIO4_Require();
    LPC43xx_GPIO5_Require();
    LPC43xx_GPIO6_Require();
    LPC43xx_GPIO7_Require();
    LPC43xx_GPIOZ_Require();
#endif
#endif

#ifdef CORE_M0
    // TODO
    LPC43xx_GPIO0_Require(); // heartbeat LED
#endif
}

void UEZPlatform_Standard_Require(void)
{
    UEZPlatform_Minimal_Require();

#ifdef CORE_M4
    UEZPlatform_Console_Expansion_Require(1024, 1024); // USART0 console
    //UEZPlatform_Console_ALT_PWR_COM_Require(1024, 1024); // USART3 console
    
    //UEZPlatform_IRTC_Require(); // Only init RTC after main task to prevent boot stall.

    UEZPlatform_I2C0_Require();

    UEZPlatform_LCD_Require();
    UEZPlatform_Touchscreen_Require("Touchscreen");

    //5v Monitor
    UEZPlatform_ADC0_7_Require();

#if (LCD_BACKLIGHT_FREERTOS_TIMER==1)
    //UEZPlatform_Timer0_Require(); // this is now unused and can be used freely for application software
#else
    UEZPlatform_Timer0_Require(); // LCD backlight timeouts
#endif
    //UEZPlatform_Timer1_Require(); // not used in normal demo,  runtime stats M4
    //UEZPlatform_Timer2_Require(); // dac audio for video player/wav playback
    UEZPlatform_Timer3_Require(); // tone generator, runtime stats M0

    UEZPlatform_EEPROM_LPC43xx_Require();
    UEZPlatform_Temp0_Require();
    UEZPlatform_Accel0_Require();
    
    UEZPlatform_Audio_Require();

    #if (UEZ_ENABLE_LOOPBACK_TEST == 1)
    UEZPlatform_Prepare_GPIOs_For_Loopback();
    #endif
#endif

#ifdef CORE_M0
    // TODO
    /* Note that calling some of these will reset the peripheral that was running on M4.
     * So even if most of the drivers will work, you want to avoid calling the same require on
     * both parts, and instead get the existing initalized data structures (and register them)
     * so that the normal uEZ API calls can be used on both cores. It would be simpler to use
     * each peripheral on only 1 core at a time, to avoid issues. uEZGPIO should have no issues. */
#endif
}

TBool UEZPlatform_Host_Port_B_Detect(void)
{
    TBool IsDevice;

#if 1
    UEZGPIOSetMux(GPIO_P3_2, 0);
    UEZGPIOSet(GPIO_P3_2); // disable MIC2025
    UEZGPIOOutput(GPIO_P3_2);

    IsDevice = (LPC_USB0->OTGSC & (1 << 8))? ETrue : EFalse;

    if(!IsDevice){
        UEZGPIOClear(GPIO_P3_2);// enable MIC2025
    }
#endif

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

#include "Config_App.h"

// To get both T_pixelColor and RGB def must include the SimpleUI_Types here.
// We need to get pixel color type size here from simpleui types, but not in assembler.
// In some applications we cannot include this in Config_App.h so also have it here.
#if (COMPILER_TYPE == IAR)
#ifdef __ICCARM__  
     //Ensure the #include is only used by the compiler, and not the assembler.
     #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
#endif
#else 
   #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
#endif

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
    return PROCESSOR_OSCILLATOR_FREQUENCY;
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
        r.iADCChannel = 7;
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
            if(reading > 400) {
              reading -= 72; // Board specific offset
            }
        }
    }
    UEZADCClose(adc);
    return reading;
}

T_pixelColor SUICallbackRGBConvert(int32_t r, int32_t g, int32_t b)
{
    return RGB(r, g, b);
}

TUInt32 UEZPlatform_GetBaseAddress(void)
{
    return LCD_DISPLAY_BASE_ADDRESS;
}
#if INCLUDE_EMWIN
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

void SUICallbackSetLCDBase(void *aAddress)
{
#if (LPC4357_LCD_ADDRESS_GUARD == 1)
    if((TUInt32)aAddress < LCD_DISPLAY_BASE_ADDRESS) {
      aAddress = (void *)LCD_DISPLAY_BASE_ADDRESS; // On this LPC LCD will eventualy crash if less than SDRAM start address!
    }
#endif
    LPC_LCD->UPBASE = (TUInt32)aAddress;
}

TUInt32 SUICallbackGetLCDBase(void)
{
    return LPC_LCD->UPBASE;
}

void WriteByteInFrameBufferWithAlpha(UNS_32 aAddr, COLOR_T aPixel, T_swimAlpha aAlpha)
{
    static COLOR_T mask =(COLOR_T)(~((1<<10)|(1<<5)|(1<<0)));
    COLOR_T *p = (COLOR_T *)aAddr;

    switch (aAlpha) {
        case SWIM_ALPHA_50:
            *p = (((*p & mask)>>1) + ((aPixel & mask)>>1));
            break;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  vMainMPUFaultHandler
 *---------------------------------------------------------------------------*/
/**
 *  Determine which stack was in use when the MPU fault occurred and extract
    the stacked PC.
 *
 *  @return         int32_t
 */
/*---------------------------------------------------------------------------*/
void vMainMPUFaultHandler( unsigned long * pulFaultRegisters ) {
    PARAM_NOT_USED(pulFaultRegisters);
#ifdef CORE_M4
unsigned long ulStacked_pc = 0UL;
    ( void ) ulStacked_pc;
    __asm volatile
    (
        "   tst lr, #4          \n"
        "   ite eq              \n"
        "   mrseq r0, msp       \n" /* The code that generated the exception was using the main stack. */
        "   mrsne r0, psp       \n" /* The code that generated the exception was using the process stack. */
        "   ldr r0, [r0, #24]   \n" /* Extract the value of the stacked PC. */
        "   str r0, [sp]        \n" /* Store the value of the stacked PC into ulStacked_pc. */
    );

    for( ;; );     /* Inspect ulStacked_pc to locate the offending instruction. */
    //UEZPlatform_System_Reset();
#endif
#ifdef CORE_M0
// TODO create appropriate ASM for these cores
unsigned long ulStacked_pc = 0UL;
    ( void ) ulStacked_pc;
    __asm
    (
    ""
    );

    for( ;; );     /* Inspect ulStacked_pc to locate the offending instruction. */
    //UEZPlatform_System_Reset();
#endif
#ifdef CORE_M0SUB
// TODO
#endif
}

#include "chip.h"

#if (LPC43XX_ENABLE_M0_CORES == 1) // interrupt / workspaces between cores
#ifdef CORE_M4
T_LPC43xx_M0_APP_Workspace G_M0_APP_Workspace;
#endif
#ifdef CORE_M0
T_LPC43xx_M4_CORE_Workspace G_M4_CORE_Workspace;
#endif
#ifdef CORE_M0SUB
// TODO
#endif

// Register and intializae the interrupt. Untested so far.
void uEZPlatform_Register_InterCore_WS(void)
{
#ifdef CORE_M4
    // register the interrupt that we get from M0 APP core.
    LPC43xx_InterCore_InitializeWorkspace(&G_M0_APP_Workspace);
    LPC43xxSetupInterCoreInterrupts();
#endif
#ifdef CORE_M0
    // register the interrupt that we get from M4 core.
    LPC43xx_InterCore_InitializeWorkspace(&G_M4_CORE_Workspace);
    LPC43xxSetupInterCoreInterrupts();
#endif
#ifdef CORE_M0SUB // TODO 
#endif
}
#endif

void uEZPlatform_Start_Additonal_Cores(void)
{
#if (LPC43XX_ENABLE_M0_CORES == 1)
#if 0 // Enable one of the PMOD Connector GPIOs to be able to scope capture it toggling in second core.
    TUInt32 value;
    UEZGPIOOutput(GPIO_P7_17);
    UEZGPIOClear(GPIO_P7_17);
    UEZGPIOSetMux(GPIO_P7_17, (GPIO_P7_17 >> 8) >= 5 ? 4 : 0);
    value = ((GPIO_P7_17 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(GPIO_P7_17, GPIO_CONTROL_SET_CONFIG_BITS, value);
    
    UEZGPIOOutput(GPIO_P7_18); 
    UEZGPIOClear(GPIO_P7_18);
    UEZGPIOSetMux(GPIO_P7_18, (GPIO_P4_3 >> 8) >= 5 ? 4 : 0);
    value = ((GPIO_P7_18 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(GPIO_P7_18, GPIO_CONTROL_SET_CONFIG_BITS, value);
#endif
    LPC_RITIMER->CTRL = 0x1; // disable RITIMER

    uEZPlatform_Register_InterCore_WS();

    // It appears that holding reset is required before clearing it.
    Chip_RGU_TriggerReset(RGU_RESET_CTRL1_M0APP_RST_Pos+32);
    Chip_Clock_EnableOpts(CLK_M4_M0APP, true, true, 1); // enable the clock branch if not enabled    
    
    // Start M0 core(s)
    Chip_CREG_SetM0AppMemMap(0x1A070000); // Wherever your M0 project flash starts you should point to that address here.
    Chip_RGU_ClearReset(RGU_M0APP_RST); // Must trigger reset above, before clearing it here.
    LPC_RGU->RESET_STATUS3 &= ~(RGU_RESET_STATUS3_M0APP_RST_Msk); // clear reset status
    if((LPC_RGU->RESET_ACTIVE_STATUS1 & RGU_RESET_ACTIVE_STATUS1_M0APP_RST_Msk) == RGU_RESET_ACTIVE_STATUS1_M0APP_RST_Msk) {
        // M0 came out of reset
    }
    
#if 0 // M0Sub starting
    Chip_CREG_SetM0AppMemMap(0x1B070000);
    Chip_RGU_ClearReset(RGU_M0SUB_RST); //
    LPC_RGU->RESET_STATUS0 &= ~(RGU_RESET_STATUS0_M0SUB_RST_Msk); // clear reset status
    if((LPC_RGU->RESET_ACTIVE_STATUS0 & RGU_RESET_ACTIVE_STATUS0_M0SUB_RST_Msk) == RGU_RESET_ACTIVE_STATUS0_M0SUB_RST_Msk) {
        // M0 Sub came out of reset
    }
#endif

#endif
}

#ifdef CORE_M0 // Enable RITIMER interrupt.
#include "FreeRTOS.h" // Can Get FreeRTOS config settings to configure tick rate or some other settings.
extern void xPortSysTickHandler( void );

#if (SEGGER_ENABLE_SYSTEM_VIEW == 1) // For SEGGER System View Time stamping
// Make sure tick suppression is off, so that normal ticks are used and a small divider is used for higher resolution.
#define SYSVIEW_TIMESTAMP_FREQ  (configCPU_CLOCK_HZ) // Frequency of the timestamp. Must match SEGGER_SYSVIEW_GET_TIMESTAMP in SEGGER_SYSVIEW_FreeRTOS.c

/*********************************************************************
*
*       SEGGER_SYSVIEW_X_GetTimestamp()
*
* Function description
*   Returns the current timestamp in cycles using the system tick
*   count and the SysTick counter.
*   All parameters of the SysTick have to be known.
*
* Return value
*   The current timestamp.
*
* Additional information
*   SEGGER_SYSVIEW_X_GetTimestamp is always called when interrupts are
*   disabled. Therefore locking here is not required.
*/
U32 SEGGER_SYSVIEW_X_GetTimestamp_Port_Specific(void) {
  U32 TickCount = SEGGER_SYSVIEW_TickCnt;
  U32 Cycles = LPC_RITIMER->COUNTER;
  
  if (NVIC_GetPendingIRQ(M0_RITIMER_OR_WWDT_IRQn)) {   // Check if timer interrupt pending ...
    Cycles = LPC_RITIMER->COUNTER; // Interrupt pending, re-read timer and adjust result
    TickCount++;
  }
  return ((SYSVIEW_TIMESTAMP_FREQ/configTICK_RATE_HZ) * TickCount) + Cycles;
}
#endif

static void IRITimerProcessIRQ(void)
{
#if 0 // Enable one of the PMOD Connector GPIOs to be able to scope capture it toggling in second core.
    if((LPC_GPIO_PORT->SET[7] & (1 << 17)) == (1 << 17)){ // if on
      LPC_GPIO_PORT->CLR[7] |= 1 << 17;// off
    } else {
      LPC_GPIO_PORT->SET[7] |= (1 << 17); // set on
    }
#endif    
    LPC_RITIMER->CTRL |= RITIMER_CTRL_RITINT_Msk; // Must clear interrupt every time for proper reload and timing.
    // Run RTOS tick here.
    xPortSysTickHandler();
}

// Even though the header file lists a SysTick on M0 it doesn't exist, so use RITIMER here instead. 
void vPortSetupTimerInterrupt( void )
{
    // This simple timer only has 4 registers
    LPC_RITIMER->CTRL = 0x1; // disable
    LPC_RITIMER->COUNTER = 0; // default value
    LPC_RITIMER->MASK = 0; // default value, don't use the mask for additional interrupt triggers
    LPC_RITIMER->COMPVAL = 204000; // value to count to for 1ms tick rate.
    // enable rollover back to 0 on compare match, disable timer when debugger attached, enable timer, enable int
    // Since we auto reload, we ONLY need to set the RITIMER_CTRL_RITINT_Msk every ISR to clear it, we don't need to
    // do an additional reload. (unless we wanted to dynamicaly change tick timing)
    LPC_RITIMER->CTRL = RITIMER_CTRL_RITINT_Msk | RITIMER_CTRL_RITENCLR_Msk | RITIMER_CTRL_RITENBR_Msk | RITIMER_CTRL_RITEN_Msk;

    InterruptEnable(M0_RITIMER_OR_WWDT_IRQn); // Must call this nvic enable to actually enable the ISR.
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  main
 *---------------------------------------------------------------------------*/
/**
 *  The main() routine in UEZ is only a stub that is used to start
 *      the whole UEZ system.  UEZBSP_Startup() is immediately called.
 *
 *  @return         int32_t
 */
/*---------------------------------------------------------------------------*/
int32_t main(void)
{
  // To be able to check RTOS check we need to finish SystemInit and actually clear/init RAM first.
  // Starting here we can check it.

  // At start we are in PD0_SLEEP0_HW_ENA = 0x1 mode and second/third cores are being held in RESET.
  // Note that some low power modes on these parts have issues that FDI didn't test/workaround.
  // So make sure to thoroughly read the manual and errata to put either M4 or M0 in a sleep mode.

#ifdef CORE_M4
    // We want to call this before starting M0, so that full init takes place. (GPIO, PLL, SDRAM, etc)
    UEZBSP_Startup();
#endif
#ifdef CORE_M0
  #if 0 // blink LED with no timer or RTOS
      while (1) {
          UEZBSPDelayMS(1000);
          UEZBSP_HEARTBEAT_TOGGLE();
      }
  #else
      UEZBSP_Startup(); // start RTOS
  #endif
#endif
#ifdef CORE_M0SUB
    while (1) {
        UEZBSPDelayMS(1000);
        UEZBSP_HEARTBEAT_TOGGLE();
    }
#endif

    while (1) {
    } // never should get here
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/

