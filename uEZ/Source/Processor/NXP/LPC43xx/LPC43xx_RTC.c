/*-------------------------------------------------------------------------*
* File:  RTC.c
*-------------------------------------------------------------------------*
* Description:
*      HAL implementation of the LPC43xx RTC Interface.
* Implementation:
*      Direct access to RTC registers is sufficient here.
*-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(R) distribution.  See the included
* uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_RTC.h>
#include <LPC43xx.h>
#include <uEZTimeDate.h>
#include "LPC43xx_UtilityFuncs.h"

// Note that it is important to only ever write to LPC_RTC-> registers in 
// this file. Then it should be ok to task delay during RTC initialization 
// without fear of writing to the registers when they shouldn't be written
// too. It should be safe to read from the registers at any time.
// For example to check if RTC is enabled or read the day of the week.

/*---------------------------------------------------------------------------*
* Constants:
*---------------------------------------------------------------------------*/
#define RTC_VALID_MARKER        0xAA55DCCD

#define RTC_ENABLE_CALIBRATION  1 // If passed in aCalibrationValue is 0 it won't have any affect.
#define RTC_ENABLE_GPIO_MEASURE 0 // Set to 1 to use dual GPIOs on PMOD to measure startup timing.

#if (RTC_ENABLE_CALIBRATION == 1)
#define RTC_CCR_BITMASK         (0x00000003) // CCR register mask
#else
#define RTC_CCR_BITMASK         (0x00000013) // CCR register mask
#endif

// Alarm timer is really a separate peripheral and needs a separate driver.
// At the moment we don't have any driver code to properly set it up, configure interrupts, etc.
// When enabled it was observed that we were triggering interrupts and getting output from GPIOs.

// If a driver is written separately, it can turn on the CREG0 bit after RTC full init, then wait 2 seconds.
// Then the registers for LPC_ATIMER-> can be cleared/reset, etc and DOWNCOUNTER could be used.
#ifndef LPC43XX_ENABLE_ALARM_TIMER
#define LPC43XX_ENABLE_ALARM_TIMER 0 // If 1 enable the old software to enable 32k output to Alarm and enable using DOWNCOUNTER 
#endif

// Use Julian Day calculation to find the day of the week, although the LPC has a built-in register for this.
// DAY_OF_WEEK(BUILD_DAY,BUILD_MONTH,BUILD_YEAR)

/*---------------------------------------------------------------------------*
* Types:
*---------------------------------------------------------------------------*/
typedef struct {
  const HAL_RTC *iHAL;
  TBool iExternalClock;
} T_LPC43xx_RTC_Workspace;

/*---------------------------------------------------------------------------*
* Routine:  LPC43xx_RTC_InitializeWorkspace
*---------------------------------------------------------------------------*
* Description:
*      Setup the LPC43xx RTC workspace.
* Inputs:
*      void *aW                    -- Particular RTC workspace
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_InitializeWorkspace(void *aWorkspace)
{
  T_LPC43xx_RTC_Workspace *p = (T_LPC43xx_RTC_Workspace *)aWorkspace;
  
  // Be default, use external clock
  p->iExternalClock = ETrue;
  
  return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC43xx_RTC_Get
*---------------------------------------------------------------------------*
* Description:
*      Get the current date and time.
* Inputs:
*      void *aW                     -- RTC workspace
*      T_uezTimeDate *aTimeDate     -- Time and date of the current time
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
  TUInt32 d, t;

  if ((LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) == RTC_CCR_CLKEN_Msk) { // RTC is enabled

    // Read the time and date, but check to see if it is just
    // changing.  If it just changes, then read again.  We don't
    // want to be caught reading between 11:59:59 and 0:00:00.
    do {
      t = LPC_RTC->CTIME0;
      d = LPC_RTC->CTIME1;
    } while ((t != LPC_RTC->CTIME0) || (d != LPC_RTC->CTIME1));
    // These 3 CTIME registers are read only.

    // Now extract the consolidated registers into the structure
    // in a more useful form.
    aTimeDate->iTime.iSecond = (t >> 0) & 63;
    aTimeDate->iTime.iMinute = (t >> 8) & 63;
    aTimeDate->iTime.iHour = (t >> 16) & 31;
    aTimeDate->iDate.iDay = d & 31;
    aTimeDate->iDate.iMonth = (d >> 8) & 15;
    aTimeDate->iDate.iYear = (d >> 16) & 4095; // 0 - 4095
  } else { // RTC not enabled yet, return build time instead.
    aTimeDate->iTime.iSecond = G_aBuildTimeDate.iTime.iSecond;
    aTimeDate->iTime.iMinute = G_aBuildTimeDate.iTime.iMinute;
    aTimeDate->iTime.iHour = G_aBuildTimeDate.iTime.iHour;
    aTimeDate->iDate.iDay = G_aBuildTimeDate.iDate.iDay;
    aTimeDate->iDate.iMonth = G_aBuildTimeDate.iDate.iMonth;
    aTimeDate->iDate.iYear = G_aBuildTimeDate.iDate.iYear;
  }

  return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC43xx_RTC_Set
*---------------------------------------------------------------------------*
* Description:
*      Set the current date and time.
* Inputs:
*      void *aW                     -- RTC workspace
*      T_uezTimeDate *aTimeDate     -- Time and date of the current time
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
  PARAM_NOT_USED(aWorkspace);       // T_LPC43xx_RTC_Workspace *p = (T_LPC43xx_RTC_Workspace *)aWorkspace;
  uint32_t ccrValue = LPC_RTC->CCR; // read current CCR value
  T_uezDate *date = (T_uezDate *)&aTimeDate->iDate;
  TUInt32 delayWrite = 1100; // 2300;
  // Note that we cannot use CTIME0, CTIME1, and CTIME2 registers to set the time. Those are read only!

  // TODO can we optimize this? Need to ask NXP.
  // Since we must take up to 7 seconds to set the date and time, do seconds first
  // Always update seconds number
  if (ccrValue & RTC_CCR_CLKEN_Msk) { // Temporarily disable Clock Control Register
    LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
  }
  LPC_RTC->SEC = aTimeDate->iTime.iSecond;
  LPC_RTC->CCR = ccrValue;  // Restore Clock Control Register to old setting
  UEZTaskDelay(delayWrite); // It appears that you must wait for 1 second between writes!

  if (LPC_RTC->MIN != aTimeDate->iTime.iMinute) { // if we already have the correct minute set, then don't set it
    if (LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) {       // Temporarily disable Clock Control Register
      LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
    }
    LPC_RTC->MIN = aTimeDate->iTime.iMinute;
    LPC_RTC->CCR = ccrValue;  // Restore Clock Control Register to old setting
    UEZTaskDelay(delayWrite); // It appears that you must wait for 1 second between writes!
  }

  if (LPC_RTC->HRS != aTimeDate->iTime.iHour) { // if we already have the correct hour set, then don't set it
    if (LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) {     // Temporarily disable Clock Control Register
      LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
    }
    LPC_RTC->HRS = aTimeDate->iTime.iHour;
    LPC_RTC->CCR = ccrValue;  // Restore Clock Control Register to old setting
    UEZTaskDelay(delayWrite); // It appears that you must wait for 1 second between writes!
  }

  if (LPC_RTC->DOW != UEZDateGetDayOfWeek(date)) { // if we already have the correct DOW set, then don't set it
    if (LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) {        // Temporarily disable Clock Control Register
      LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
    }
    LPC_RTC->DOW = UEZDateGetDayOfWeek(date); // Is it required to write DOW?
    LPC_RTC->CCR = ccrValue;                  // Restore Clock Control Register to old setting
    UEZTaskDelay(delayWrite);                 // It appears that you must wait for 1 second between writes!
  }

  if (LPC_RTC->DOM != aTimeDate->iDate.iDay) { // if we already have the correct day set, then don't set it
    if (LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) {    // Temporarily disable Clock Control Register
      LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
    }
    LPC_RTC->DOM = aTimeDate->iDate.iDay;
    LPC_RTC->CCR = ccrValue;  // Restore Clock Control Register to old setting
    UEZTaskDelay(delayWrite); // It appears that you must wait for 1 second between writes!
  }

  if (LPC_RTC->MONTH != aTimeDate->iDate.iMonth) { // if we already have the correct month set, then don't set it
    if (LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) {        // Temporarily disable Clock Control Register
      LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
    }
    LPC_RTC->MONTH = aTimeDate->iDate.iMonth;
    LPC_RTC->CCR = ccrValue;  // Restore Clock Control Register to old setting
    UEZTaskDelay(delayWrite); // It appears that you must wait for 1 second between writes!
  }

  if (LPC_RTC->YEAR != aTimeDate->iDate.iYear) { // if we already have the correct year set, then don't set it
    if (LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) {      // Temporarily disable Clock Control Register
      LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
    }
    LPC_RTC->YEAR = aTimeDate->iDate.iYear;
    LPC_RTC->CCR = ccrValue;  // Restore Clock Control Register to old setting
    UEZTaskDelay(delayWrite); // It appears that you must wait for 1 second between writes!
  }

  return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC43xx_RTC_Configure
*---------------------------------------------------------------------------*
* Description:
*      Configure for internal or external clock and turn on.
* Inputs:
*      void *aW                     -- RTC workspace
*      TBool aIsExternalClock       -- Internal or external clock config.
*      TUInt16 aPrescaleInteger     -- Prescalar integer (13-bit) when
*                                      internal config
*      TUInt16 aPrescaleFraction    -- Prescalar fraction (15-bit) when
*                                      internal
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_Configure(void *aWorkspace, TBool aIsExternalClock, TUInt32 aCalibrationValue)
{
  T_LPC43xx_RTC_Workspace *p = (T_LPC43xx_RTC_Workspace *)aWorkspace;
  p->iExternalClock = aIsExternalClock;

#if (RTC_ENABLE_GPIO_MEASURE == 1) // Enable one of the PMOD Connector GPIOs to be able to scope capture pins for test
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

  //int32_t int_status; // TODO track interrupt status
  // TODO cleanup more
  
  // Output RTC to CLK3 for testing, currently don't leave this on as we use CLKOUT
  //LPC_CGU->BASE_OUT_CLK = (0); // change base OUT clock to 32K, might need to power down first?
  // Change P12 CLK3 to output clock mode
  // disable input glitch filter, enable input buffer, high speed slew rate, disable pull-up, disable pull-down, select 32KHz
  //LPC_SCU->SFSCLK_3 = ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4)) | 1; // CLK3, set register to 0xF1
  
  LPC_CREG->CREG0 &= ~((1 << 3) | (1 << 2));	/* Reset 32Khz oscillator, enable 32K power */

  // Enable RTC Clock

  /* Per errata ES_LPC435x/3x/2x/1x -> Section 4.1 IBAT.1: We must set alarm and sample to inactive
   * to improve backup time; Set ALARMCTRL to Inactive 0x3.
   * Set SAMPLECTRL to Inactive 0x3 (only listed as Reserved in manual).
   * The setting will persist on power down until VBAT runs out, so we just need to set it on every boot. */

  // Enable 32 kHz & 1 kHz on osc32k and release reset, set errata workaround for ALARMCTRL and SAMPLECTRL for first rev chips only.
  //LPC_CREG->CREG0 |= (1 << 1) | (1 << 0) | (CREG_CREG0_ALARMCTRL_Msk) | (CREG_CREG0_SAMPLECTRL_Msk);
  
  // Note that we seemed to be getting inconsistent ALARM pin output results. The test point is now gone on newer boards.
  // So it is probably best to leave that unused feature turned off by default. It could be enabled in separate driver for ALARM.
    
#if (LPC43XX_ENABLE_ALARM_TIMER == 1)
  // Enable 32 kHz & 1 kHz on osc32k and release reset
  LPC_CREG->CREG0 |= (1 << 1) | (1 << 0); // alarm timer requires 32k output to be enabled
#else
  //1khz output only, no errata workaround (for Rev A chip or newer), disable 32K output to alarm (RTC still works)
  LPC_CREG->CREG0 |= (1 << 0);
#endif

  // Next wait for 2 seconds to allow for it to come on. If we enable the alarm timer, we can use the down count register.
  // Otherwise use another method to wait for 2 seconds.
#if (LPC43XX_ENABLE_ALARM_TIMER == 1)  
  LPC_ATIMER->DOWNCOUNTER = 2048; // 2-Second delay after enabling RTC clock per datasheet
  while (LPC_ATIMER->DOWNCOUNTER) {
    UEZTaskDelay(128);
  }
#else
    UEZTaskDelay(2048); // delay for 2 seconds
#endif
  
  // If RTC was previously enabled we MUST enable clock before we return.
  if((LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) != 0) {
	// RTC was previously initialized, so we are finished turning it on. The time should still be set.
#if (RTC_ENABLE_GPIO_MEASURE == 1)
  LPC_GPIO_PORT->SET[7] |= (1 << 17);
  LPC_GPIO_PORT->CLR[7] |= (1 << 18);
#endif
    return UEZ_ERROR_NONE;
  }

  // The RTC wasn't enabled. We lost power and need to re-initalize the whole thing.
#if (RTC_ENABLE_GPIO_MEASURE == 1)
  LPC_GPIO_PORT->SET[7] |= (1 << 17);
  LPC_GPIO_PORT->CLR[7] |= (1 << 18);
#endif
  
#if 0
  // HITEX RG: disable interrupts to stop FreeRTOS or anything else disturbing
  // critical time while RTC peripheral is initialised
  //int_status = 
  //__disable_interrupt();  
  portDISABLE_INTERRUPTS(); //TODO verifiy if this has same functionality as __disable_interrupt 
  //uint32_t currentInterrupt = NVIC_GetActive(IRQn_Type IRQn)
#endif
  
  // Disable RTC
  LPC_RTC->CCR = (LPC_RTC->CCR & ~RTC_CCR_CLKEN_Msk) & RTC_CCR_BITMASK;

#if (RTC_ENABLE_CALIBRATION == 1)
  // RTC_CCR_CLKEN_Msk is set to 0 above.
#else
  // Disable Calibration
  LPC_RTC->CCR |= RTC_CCR_CCALEN_Msk;
#endif
  
  // Note that this reset procedure is the time consuming part, probably to allow the 1.5s delay for various registers to be cleared to 0.
  // If we initialize all of the registers maybe the reset could be skipped to save time.
  
  // Reset RTC clock
  LPC_RTC->CCR |= RTC_CCR_CTCRST_Msk;
  while (!(LPC_RTC->CCR & RTC_CCR_CTCRST_Msk)) { // per manual should be able to task delay here as long as we don't allow register writes
    UEZTaskDelay(10); // specifically resetting the RTC has a long delay
  } 
  // Finish resetting RTC clock
  LPC_RTC->CCR = (LPC_RTC->CCR & ~RTC_CCR_CTCRST_Msk) & RTC_CCR_BITMASK; // CTCRST should already be 0?
  while (LPC_RTC->CCR & RTC_CCR_CTCRST_Msk) { // check that it is zero?
    UEZTaskDelay(1); 
  }
  // The reset is now finished.
  
  // Clear counter increment and alarm interrupts (shouldn't matter if we don't use alarm interrupt, maybe it was on previously)
  LPC_RTC->ILR = RTC_ILR_RTCCIF_Msk | RTC_ILR_RTCALF_Msk; // should be quick
  //while (LPC_RTC->ILR != 0) {} // This register is write only, we cannot read it!
  
  // Clear all register to be default
  LPC_RTC->CIIR = 0x00; // Counter Increment Interrupt Register
  LPC_RTC->AMR = 0xFF; // Alarm Mask Register

#if (RTC_ENABLE_CALIBRATION == 1)
  LPC_RTC->CALIBRATION = aCalibrationValue; // forward direction to add 1 second every 46526 seconds.
#else
  LPC_RTC->CALIBRATION = 0x00;
#endif

  // TODO is it ever possible to set time here while not running without CPU stall?
    
#if (RTC_ENABLE_GPIO_MEASURE == 1)
      LPC_GPIO_PORT->CLR[7] |= 1 << 17;// off
      LPC_GPIO_PORT->SET[7] |= (1 << 18); // set on
#endif

#if 0
  //if (!int_status) {
    //__enable_interrupt();     // Re-enable interrupts if they were enabled before    
    portENABLE_INTERRUPTS(); //TODO verifiy if this has same functionality as __enable_interrupt 
    __ISB(); // Enable servicing of interrupts that we just missed.
  //}
#endif
  
  // Enable RTC
  LPC_RTC->CCR |= RTC_CCR_CLKEN_Msk; // Note that you should not repeatedly write this bit!
  while (!(LPC_RTC->CCR & RTC_CCR_CLKEN_Msk)) { // wait for enable to continue
    UEZTaskDelay(1); // 
  }

  //LPC43xx_RTC_Set(aWorkspace, &G_aBuildTimeDate); //

  return UEZ_ERROR_NONE;    
}

/*---------------------------------------------------------------------------*
* Routine:  LPC43xx_RTC_Validate
*---------------------------------------------------------------------------*
* Description:
*      Check if the RTC values  have gone corrupt.  If they have gone
*      corrupt reset the time to given time and report it invalid.
*      Otherwise, leave everything alone.
* Inputs:
*      void *aWorkspace             -- RTC workspace
*      T_uezTimeDate *aTimeDate     -- time to use if invalid
* Outputs:
*      T_uezError                   -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_Validate(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
  T_uezTimeDate time;
  T_uezError error = UEZ_ERROR_NONE;
  
  // What's the current time?
  LPC43xx_RTC_Get(aWorkspace, &time);
  
  // Is any part of it invalid?
  if ((time.iTime.iHour >= 24) || (time.iTime.iMinute >= 60)
      || (time.iTime.iSecond >= 60) || (time.iDate.iMonth == 0)
        || (time.iDate.iMonth > 12) || (time.iDate.iDay == 0)
          || (time.iDate.iDay > 31) || (time.iDate.iYear > 9999))
    error = UEZ_ERROR_INVALID;
  
  if (error == UEZ_ERROR_INVALID) {
    // Reset the RTC
    LPC43xx_RTC_Set(aWorkspace, aTimeDate);
  }
  
  return error;
}
/*---------------------------------------------------------------------------*
* HAL Interface tables:
*---------------------------------------------------------------------------*/
const HAL_RTC LPC43xx_RTC_Interface = {
  {
    "LPC43xx:RTC",
    0x0100,
    LPC43xx_RTC_InitializeWorkspace,
    sizeof(T_LPC43xx_RTC_Workspace),
  },
  
  LPC43xx_RTC_Get,
  LPC43xx_RTC_Set,
  LPC43xx_RTC_Validate, };

/*---------------------------------------------------------------------------*
* Requirement routines:
*---------------------------------------------------------------------------*/
void LPC43xx_RTC_Require(TBool aIsExternalClock, TUInt32 aCalibrationValue)
{
  T_halWorkspace *p_rtc;
  
  HAL_DEVICE_REQUIRE_ONCE();
  HALInterfaceRegister(
                       "RTC",
                       (T_halInterface *)&LPC43xx_RTC_Interface,
                       0,
                       &p_rtc);
  LPC43xx_RTC_Configure(p_rtc, aIsExternalClock, aCalibrationValue);
}

/*-------------------------------------------------------------------------*
* End of File:  RTC.c
*-------------------------------------------------------------------------*/
