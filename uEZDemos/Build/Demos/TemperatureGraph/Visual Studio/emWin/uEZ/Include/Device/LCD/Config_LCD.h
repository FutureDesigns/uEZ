/*-------------------------------------------------------------------------*
 * File:  Config_LCD.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ LCD Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _CONFIG_LCD_H_
#define _CONFIG_LCD_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

 /*-------------------------------------------------------------------------*
 * LCD Configurations:
 *-------------------------------------------------------------------------*/
/**
 *  @file   /Include/Device/LCD/Config_LCD.h
 *  @brief  uEZ LCD Device Interface
 *
 * The uEZ LCD Device Interface
 * Each LCD configuration has a specific identifier.  These identifiers
 * are known only at compile time.
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
#ifdef __cplusplus
extern "C" {
#endif


#define LCD_CONFIG_CUSTOM                           0
#define LCD_CONFIG_TOSHIBA_QVGA_LCDC_REV2           1
#define LCD_CONFIG_TOSHIBA_QVGA_LCDC_3_5            2
#define LCD_CONFIG_OKAYA_QVGA_LCDC_3_5              3
#define LCD_CONFIG_OKAYA_VGA_LCDC_3_5               4
#define LCD_CONFIG_OKAYA_QVGA_3x5_LCDC_3_5          5
#define LCD_CONFIG_SEIKO_43WQW1T                    6
#define LCD_CONFIG_SEIKO_70WVW2T                    7
#define LCD_CONFIG_NEC_VGA                          8
#define LCD_CONFIG_OPTREX_T55596GD                  9
#define LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1      10
#define LCD_CONFIG_TIANMA_TM047NBH01                11
#define LCD_CONFIG_TIANMA_TM043NBH02                12
#define LCD_CONFIG_HITACHI_TX11D06VM2APA            13
#define LCD_CONFIG_TIANMA_QVGA_LCDC_3_5             14
#define LCD_CONFIG_DIGITAL_IMAGE_FG050720DSSWDG01   15
#define LCD_CONFIG_TIANMA_TM070RBHG04               16
#define LCD_CONFIG_NEWHAVEN_NHD43480272MF           17
#define LCD_CONFIG_SHARP_LQ043T3DG02_GFX            18
#define LCD_CONFIG_SHARP_LQ043T3DG01                19
#define LCD_CONFIG_SHARP_LQ043T1DG28                20
#define LCD_CONFIG_KOE_TX13D06VM2BAA                21
#define LCD_CONFIG_SHARP_LQ042T5DZ11                22
#define LCD_CONFIG_SHARP_LQ104V1DG28                23

#define LCD_RES_VGA                         1
#define LCD_RES_QVGA                        2
#define LCD_RES_480x272                     3
#define LCD_RES_WVGA                        4

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_NEWHAVEN_NHD43480272MF)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_NewHaven_NHD43480272MF_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
/**
 * Configure PWM of backlight settings (fast control)
 */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   180000
#endif


#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_KOE_TX13D06VM2BAA)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH      UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_KOE_TX13D06VM2BAA_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           800
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_WVGA
/**
 * Configure PWM of backlight settings
 */
    #define UEZ_LCD_BACKLIGHT_PERIOD_COUNT  (PROCESSOR_OSCILLATOR_FREQUENCY/40000)
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD  UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x5000  //6000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON  0x00  //40000//
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x5000  //2000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON   (UEZ_LCD_BACKLIGHT_PERIOD_COUNT/4)//0x3000  //???
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF  UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x2400  //???


    #define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        700   //0x2000
    #define DEFAULT_TOUCHSCREEN_LOW_LEVEL         200   //0x1000

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x0C04,                     0x204C,                     1 }, \
        { 0,    0x6F2A,                     0x2134,                     1 }, \
        { 0,    0x0C58,                     0x6012,                     1 }, \
        { 0,    0x6F2C,                     0x5DBA,                     1 }, \
        { 0,    0x3C6A,                     0x4098,                     1 },
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TIANMA_TM070RBHG04)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH      UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Tianma_TM070RBHG04_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           800
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_WVGA
/**
 * Configure PWM of backlight settings
 */
    #define UEZ_LCD_BACKLIGHT_PERIOD_COUNT  (PROCESSOR_OSCILLATOR_FREQUENCY/40000)
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD  UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x5000  //6000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON  0x00  //40000//
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x5000  //2000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON   (UEZ_LCD_BACKLIGHT_PERIOD_COUNT/4)//0x3000  //???
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF  UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x2400  //???


    #define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        700   //0x2000
    #define DEFAULT_TOUCHSCREEN_LOW_LEVEL         200   //0x1000

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x0C04,                     0x204C,                     1 }, \
        { 0,    0x6F2A,                     0x2134,                     1 }, \
        { 0,    0x0C58,                     0x6012,                     1 }, \
        { 0,    0x6F2C,                     0x5DBA,                     1 }, \
        { 0,    0x3C6A,                     0x4098,                     1 },
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SEIKO_43WQW1T)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_SEIKO_43WQW1T_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   0x3000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0x0000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  0x3000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0x2400
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   0x3000
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SEIKO_70WVW2T)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH      UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_SEIKO_70WVW2T_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           800
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_WVGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_PERIOD_COUNT  (PROCESSOR_OSCILLATOR_FREQUENCY/40000)
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD  UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x5000  //6000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON  0x00  //40000//
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x5000  //2000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON   (UEZ_LCD_BACKLIGHT_PERIOD_COUNT/4)//0x3000  //???
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF  UEZ_LCD_BACKLIGHT_PERIOD_COUNT//0x2400  //???

	/**
	 * The Seiko 7" display needs different touchscreen sensitivity levels
	 */
    #define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        700   //0x2000
    #define DEFAULT_TOUCHSCREEN_LOW_LEVEL         200   //0x1000

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x6F1C,                     0x620D,                     1 }, \
        { 0,    0x118B,                     0x628A,                     1 }, \
        { 0,    0x6F01,                     0x1BF4,                     1 }, \
        { 0,    0x11D7,                     0x1BFB,                     1 }, \
        { 0,    0x4036,                     0x3F62,                     1 },
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TOSHIBA_QVGA_LCDC_REV2)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH      UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Toshiba_LTA057A347F_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           320
    #define UEZ_LCD_DISPLAY_HEIGHT          240
    #define UEZ_DEFAULT_LCD                 LCD_RES_QVGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   0x3000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0x3000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  0x0000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0x3000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   0x2400
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TOSHIBA_QVGA_LCDC_3_5)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
//    #define UEZ_LCD_DEFAULT_COLOR_DEPTH  UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Toshiba_LTA057A347F_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           320
    #define UEZ_LCD_DISPLAY_HEIGHT          240
    #define UEZ_DEFAULT_LCD                 LCD_RES_QVGA
    /**
     * Configure PWM of backlight settings
     */
#if 0
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   90
#else
#if 0
#define UEZ_LCD_BACKLIGHT_FULL_PERIOD   5000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   1000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  5000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    2500
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   5000
#else
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    10000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  170000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   180000
#endif
#endif
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_OKAYA_QVGA_LCDC_3_5)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_OKAYA_RV320240T_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           320
    #define UEZ_LCD_DISPLAY_HEIGHT          240
    #define UEZ_DEFAULT_LCD                 LCD_RES_QVGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   90
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_OKAYA_VGA_LCDC_3_5)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_OKAYA_RV640480T_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           640
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_VGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   90

   /**
    * The Okaya VGA display needs different touchscreen sensitivity levels
    */
    #define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        0x4000
    #define DEFAULT_TOUCHSCREEN_LOW_LEVEL         0x1800
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_OKAYA_QVGA_3x5_LCDC_3_5)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_OKAYA_RH320240T_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           320
    #define UEZ_LCD_DISPLAY_HEIGHT          240
    #define UEZ_DEFAULT_LCD                 LCD_RES_QVGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   90000
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG02_GFX)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_SHARP_LQ043T3DG02_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    #define UEZ_DEFAULT_LCD_RES_480x272     1
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   180000
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG01)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_SHARP_LQ043T1DG01_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   180000

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x13f6,                     0x58ec,                     1 }, \
        { 0,    0x6aa3,                     0x58f0,                     1 }, \
        { 0,    0x12d8,                     0x22cd,                     1 }, \
        { 0,    0x6616,                     0x1f62,                     1 }, \
        { 0,    0x3f47,                     0x3de0,                     1 },
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T1DG28)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_SHARP_LQ043T1DG28_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   18000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  18000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   18000

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x13f6,                     0x58ec,                     1 }, \
        { 0,    0x6aa3,                     0x58f0,                     1 }, \
        { 0,    0x12d8,                     0x22cd,                     1 }, \
        { 0,    0x6616,                     0x1f62,                     1 }, \
        { 0,    0x3f47,                     0x3de0,                     1 },
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_NEC_VGA)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_NEC_NL6448BC_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           640
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_VGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  135
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   135

    // Not sure what the A-Dec NEC Display Needs yet
    //#define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        0x4000
    //#define DEFAULT_TOUCHSCREEN_LOW_LEVEL         0x1800
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_OPTREX_T55596GD)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_OPTREX_T_5596GD035JU_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           320
    #define UEZ_LCD_DISPLAY_HEIGHT          240
    #define UEZ_DEFAULT_LCD                 LCD_RES_QVGA

    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  135
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   135

    // Not sure what the A-Dec NEC Display Needs yet
    //#define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        0x4000
    //#define DEFAULT_TOUCHSCREEN_LOW_LEVEL         0x1800
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT//UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Inteltronic_LMIX0560NTN53V1_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           640
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_VGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_PERIOD_COUNT  (PROCESSOR_OSCILLATOR_FREQUENCY/300)//300HZ
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   UEZ_LCD_BACKLIGHT_PERIOD_COUNT
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  UEZ_LCD_BACKLIGHT_PERIOD_COUNT
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   UEZ_LCD_BACKLIGHT_PERIOD_COUNT

    // Not sure what the Display Needs yet
    //#define DEFAULT_TOUCHSCREEN_HIGH_LEVEL        0x4000
    //#define DEFAULT_TOUCHSCREEN_LOW_LEVEL         0x1800

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x6D64,                     0x62F8,                     1 }, \
        { 0,    0x0F3A,                     0x63D7,                     1 }, \
        { 0,    0x6BFD,                     0x1B4B,                     1 }, \
        { 0,    0x0F6D,                     0x1880,                     1 }, \
        { 0,    0x3CC8,                     0x3C98,                     1 },
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TIANMA_TM047NBH01)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Tianma_TM047NBH01_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   0x0F00 // 0x0780
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0x0F00 // 0x0780
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  0x00C0
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TIANMA_TM043NBH02)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Tianma_TM043NBH02_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   0x0F00 // 0x0780
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0x0F00 // 0x0780
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  0x00C0
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_HITACHI_TX11D06VM2APA)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_I15_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Hitachi_TX11D06VM2APA_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   180000
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TIANMA_QVGA_LCDC_3_5)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH      UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_Tianma_TM035NBH02_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           320
    #define UEZ_LCD_DISPLAY_HEIGHT          240
    #define UEZ_DEFAULT_LCD                 LCD_RES_QVGA
    /**
     * Configure PWM of backlight settings
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   0x0F00 // 0x0780
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON   0x0F00 // 0x0780
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  0x00C0
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_DIGITAL_IMAGE_FG050720DSSWDG01)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_DigitalImage_FG050720DSSWDG01_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           640
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_VGA
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   180000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  180000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   180000
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ042T5DZ11)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_SHARP_LQ042T5DZ11_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           480
    #define UEZ_LCD_DISPLAY_HEIGHT          272
    #define UEZ_DEFAULT_LCD                 LCD_RES_480x272
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   18000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  18000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   18000

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x13f6,                     0x58ec,                     1 }, \
        { 0,    0x6aa3,                     0x58f0,                     1 }, \
        { 0,    0x12d8,                     0x22cd,                     1 }, \
        { 0,    0x6616,                     0x1f62,                     1 }, \
        { 0,    0x3f47,                     0x3de0,                     1 },
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ104V1DG28)
    #define UEZ_LCD_DEFAULT_COLOR_DEPTH     UEZLCD_COLOR_DEPTH_16_BIT
    #define UEZ_LCD_INTERFACE_ARRAY         LCD_SHARP_LQ104V1DG28_InterfaceArray
    #define UEZ_LCD_DISPLAY_WIDTH           640
    #define UEZ_LCD_DISPLAY_HEIGHT          480
    #define UEZ_DEFAULT_LCD                 LCD_RES_VGA
    /**
     * Configure PWM of backlight settings (fast control)
     */
    #define UEZ_LCD_BACKLIGHT_FULL_PERIOD   18000
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_ON    0
    #define UEZ_LCD_BACKLIGHT_FULL_PWR_OFF  18000
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_ON     0
    #define UEZ_LCD_BACKLIGHT_LOW_PWR_OFF   18000

    #define UEZ_LCD_TOUCHSCREEN_DEFAULT_CALIBRATION \
        { 0,    0x13f6,                     0x58ec,                     1 }, \
        { 0,    0x6aa3,                     0x58f0,                     1 }, \
        { 0,    0x12d8,                     0x22cd,                     1 }, \
        { 0,    0x6616,                     0x1f62,                     1 }, \
        { 0,    0x3f47,                     0x3de0,                     1 },
#endif

#ifndef UEZ_LCD_COLOR_DEPTH
    #define UEZ_LCD_COLOR_DEPTH                 UEZ_LCD_DEFAULT_COLOR_DEPTH
#endif

#ifndef UEZ_DEFAULT_LCD
    #define UEZ_DEFAULT_LCD                     LCD_RES_QVGA
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_VGA)
    #define UEZ_DEFAULT_LCD_RES_VGA             1
#else
    #define UEZ_DEFAULT_LCD_RES_VGA             0
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_QVGA)
    #define UEZ_DEFAULT_LCD_RES_QVGA            1
#else
    #define UEZ_DEFAULT_LCD_RES_QVGA            0
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define UEZ_DEFAULT_LCD_RES_480x272         1
#else
    #define UEZ_DEFAULT_LCD_RES_480x272         0
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_WVGA)
    #define UEZ_DEFAULT_LCD_RES_WVGA            1
#else
    #define UEZ_DEFAULT_LCD_RES_WVGA            0
#endif

#ifdef __cplusplus
}
#endif

#endif // _CONFIG_LCD_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_LCD.h
 *-------------------------------------------------------------------------*/
