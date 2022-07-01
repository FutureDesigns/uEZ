/*-------------------------------------------------------------------------*
 * File:  Sharp_LQ040Y3DX80A.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Sharp LQ040Y3DX80A.
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
#include <uEZ.h>
#include <HAL/LCDController.h>
#include <HAL/GPIO.h>
#include <UEZSPI.h>
#include "Sharp_LQ040Y3DX80A.h"
#include <uEZDeviceTable.h>
#include <string.h>
#include <uEZGPIO.h>
#include "uEZPlatform.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        480
#define RESOLUTION_Y        800

#ifndef LCD_SHARP_LQ040Y3DX80A_FLIP_SCREEN_X
#define LCD_SHARP_LQ040Y3DX80A_FLIP_SCREEN_X       0
#endif

#ifndef LCD_SHARP_LQ040Y3DX80A_FLIP_SCREEN_Y
#define LCD_SHARP_LQ040Y3DX80A_FLIP_SCREEN_Y       0
#endif

#define LCD_CLOCK_RATE      20000000//25000000


/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_LCD *iHAL;
    TUInt32 iBaseAddress;
    int32_t aNumOpen;
    TUInt32 iBacklightLevel;
    HAL_LCDController **iLCDController;
    DEVICE_Backlight **iBacklight;
    const T_uezLCDConfiguration *iConfiguration;
    char iSPIBus[5];
    T_uezDevice iSPI;
    T_uezGPIOPortPin iCSGPIOPin;
    T_uezGPIOPortPin iResetGPIOPin;
    T_uezSemaphore iVSyncSem;
} T_LQ040Y3DX80AWorkspace;

typedef struct {
    TUInt16 iReg;
#define REG_END   0xFF
    TUInt8 iData;
} T_lcdSPICmd;


/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_LQ040Y3DX80A_settings;

static const T_LCDControllerSettings LCD_LQ040Y3DX80A_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    14,         /* Horizontal back porch */
    14,         /* Horizontal front porch */
    2,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    4,          /* Vertical back porch */
    2,          /* Vertical front porch */
    1,         /* VSYNC pulse width */
    272,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     // Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    LCD_CLOCK_RATE,
};

static const T_LCDControllerSettings LCD_LQ040Y3DX80A_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    30,         /* Horizontal back porch */
    16,         /* Horizontal front porch */
    2,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    3,          /* Vertical back porch */
    3,          /* Vertical front porch */
    1,         /* VSYNC pulse width */
    800,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     // Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    LCD_CLOCK_RATE,
//    2000000,
};

static const T_LCDControllerSettings LCD_LQ040Y3DX80A_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    16,         /* Horizontal back porch */
    10,         /* Horizontal front porch */
    2,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    3,          /* Vertical back porch */
    2,          /* Vertical front porch */
    2,         /* VSYNC pulse width */
    272,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     // Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    LCD_CLOCK_RATE,
};

static T_uezLCDConfiguration LCD_LQ040Y3DX80A_configuration_16Bit = {
    RESOLUTION_X,
    RESOLUTION_Y,
    UEZLCD_COLOR_DEPTH_16_BIT,
    UEZLCD_ORDER_RGB,
    0,
    1,
    RESOLUTION_X*2,
    RESOLUTION_X*2,
    256
};

static T_uezLCDConfiguration LCD_LQ040Y3DX80A_configuration_I15Bit = {
    RESOLUTION_X,
    RESOLUTION_Y,
    UEZLCD_COLOR_DEPTH_I15_BIT,
    UEZLCD_ORDER_RGB,
    0,
    1,
    RESOLUTION_X*2,
    RESOLUTION_X*2,
    256
};

static T_uezLCDConfiguration LCD_LQ040Y3DX80A_configuration_8Bit = {
    RESOLUTION_X,
    RESOLUTION_Y,
    UEZLCD_COLOR_DEPTH_8_BIT,
    UEZLCD_ORDER_RGB,
    0,
    1,
    RESOLUTION_X*1,
    RESOLUTION_X*1,
    256
};
static const T_lcdSPICmd G_lcdStartup[] = {
  // Panel Driving Control
        {0xFF00, 0xAA},
        {0xFF01, 0x55},
        {0xFF02, 0x25},
        {0xFF03, 0x01},
        {0xF300, 0x00},
        {0xF301, 0x32},
        {0xF302, 0x00},
        {0xF303, 0x38},
        {0xF304, 0x31},
        {0xF305, 0x08},
        {0xF306, 0x11},
        {0xF307, 0x00},
        {0xF000, 0x55},
        {0xF001, 0xAA},
        {0xF002, 0x52},
        {0xF003, 0x08},
        {0xF004, 0x00},
        {0xB000, 0x00},
        {0xB001, 0x05},
        {0xB002, 0x02},
        {0xB003, 0x05},
        {0xB004, 0x02},
        {0xB300, 0x00},
        {0xB600, 0x03},
        {0xB700, 0x70},
        {0xB701, 0x70},
        {0xB800, 0x00},
        {0xB801, 0x06},
        {0xB802, 0x06},
        {0xB805, 0x06},
        {0xBC00, 0x00},
        {0xBC01, 0xc8},
        {0xBC02, 0x00},
        {0xBD00, 0x01},
        {0xBD01, 0x84},
        {0xBD02, 0x06},
        {0xBD03, 0x50},
        {0xBD04, 0x00},
        {0xcc00, 0x03},
        {0xcc01, 0x01},
        {0xcc02, 0x06},
        {0xF000, 0x55},
        {0xF001, 0xAA},
        {0xF002, 0x52},
        {0xF003, 0x08},
        {0xF004, 0x01},
        {0xB000, 0x05},
        {0xB001, 0x05},
        {0xB002, 0x05},
        {0xB100, 0x05},
        {0xB101, 0x05},
        {0xB102, 0x05},
        {0xB200, 0x03},
        {0xB201, 0x03},
        {0xB202, 0x03},
        {0xB800, 0x25},
        {0xB801, 0x25},
        {0xB802, 0x25},
        {0xB300, 0x0b},
        {0xB301, 0x0b},
        {0xB302, 0x0b},
        {0xB900, 0x34},
        {0xB901, 0x34},
        {0xB902, 0x34},
        {0xBF00, 0x01},
        {0xB500, 0x08},
        {0xB501, 0x08},
        {0xB502, 0x08},
        {0xBA00, 0x24},
        {0xBA01, 0x24},
        {0xBA02, 0x24},
        {0xB400, 0x2D},
        {0xB401, 0x2D},
        {0xB402, 0x2D},
        {0xBC00, 0x00},
        {0xBC01, 0x68},
        {0xBC02, 0x00},
        {0xBD00, 0x00},
        {0xBD01, 0x7C},
        {0xBD02, 0x00},
        {0xBE00, 0x00},
        {0xBE01, 0x40},
        {0xF000, 0x55},
        {0xF001, 0xAA},
        {0xF002, 0x52},
        {0xF003, 0x01},
        {0xD000, 0x0B},
        {0xD001, 0x14},
        {0xD002, 0x0C},
        {0xD003, 0x0E},
        {0xD100, 0x00},
        {0xD101, 0x37},
        {0xD102, 0x00},
        {0xD103, 0x4A},
        {0xD104, 0x00},
        {0xD105, 0x6F},
        {0xD106, 0x00},
        {0xD107, 0x8D},
        {0xD108, 0x00},
        {0xD109, 0xAD},
        {0xD10A, 0x00},
        {0xD10B, 0xDF},
        {0xD10C, 0x01},
        {0xD10D, 0x11},
        {0xD10E, 0x01},
        {0xD10F, 0x58},
        {0xD110, 0x01},
        {0xD111, 0x76},
        {0xD112, 0x01},
        {0xD113, 0xA6},
        {0xD114, 0x01},
        {0xD115, 0xCD},
        {0xD116, 0x02},
        {0xD117, 0x0E},
        {0xD118, 0x02},
        {0xD119, 0x46},
        {0xD11A, 0x02},
        {0xD11B, 0x48},
        {0xD11C, 0x02},
        {0xD11D, 0x78},
        {0xD11E, 0x02},
        {0xD11F, 0xAC},
        {0xD120, 0x02},
        {0xD121, 0xCD},
        {0xD122, 0x02},
        {0xD123, 0xFD},
        {0xD124, 0x03},
        {0xD125, 0x1F},
        {0xD126, 0x03},
        {0xD127, 0x4B},
        {0xD128, 0x03},
        {0xD129, 0x69},
        {0xD12A, 0x03},
        {0xD12B, 0x8E},
        {0xD12C, 0x03},
        {0xD12D, 0xA5},
        {0xD12E, 0x03},
        {0xD12F, 0xCD},
        {0xD130, 0x03},
        {0xD131, 0xF1},
        {0xD132, 0x03},
        {0xD133, 0xF1},
        {0xD200, 0x00},
        {0xD201, 0x37},
        {0xD202, 0x00},
        {0xD203, 0x4A},
        {0xD204, 0x00},
        {0xD205, 0x6F},
        {0xD206, 0x00},
        {0xD207, 0x8D},
        {0xD208, 0x00},
        {0xD209, 0xAD},
        {0xD20A, 0x00},
        {0xD20B, 0xDF},
        {0xD20C, 0x01},
        {0xD20D, 0x11},
        {0xD20E, 0x01},
        {0xD20F, 0x58},
        {0xD210, 0x01},
        {0xD211, 0x76},
        {0xD212, 0x01},
        {0xD213, 0xA6},
        {0xD214, 0x01},
        {0xD215, 0xCD},
        {0xD216, 0x02},
        {0xD217, 0x0E},
        {0xD218, 0x02},
        {0xD219, 0x46},
        {0xD21A, 0x02},
        {0xD21B, 0x48},
        {0xD21C, 0x02},
        {0xD21D, 0x78},
        {0xD21E, 0x02},
        {0xD21F, 0xAC},
        {0xD220, 0x02},
        {0xD221, 0xCD},
        {0xD222, 0x02},
        {0xD223, 0xFD},
        {0xD224, 0x03},
        {0xD225, 0x1F},
        {0xD226, 0x03},
        {0xD227, 0x4B},
        {0xD228, 0x03},
        {0xD229, 0x69},
        {0xD22A, 0x03},
        {0xD22B, 0x8E},
        {0xD22C, 0x03},
        {0xD22D, 0xA5},
        {0xD22E, 0x03},
        {0xD22F, 0xCD},
        {0xD230, 0x03},
        {0xD231, 0xF1},
        {0xD232, 0x03},
        {0xD233, 0xF1},
        {0xD300, 0x00},
        {0xD301, 0x37},
        {0xD302, 0x00},
        {0xD303, 0x4A},
        {0xD304, 0x00},
        {0xD305, 0x6F},
        {0xD306, 0x00},
        {0xD307, 0x8D},
        {0xD308, 0x00},
        {0xD309, 0xAD},
        {0xD30A, 0x00},
        {0xD30B, 0xDF},
        {0xD30C, 0x01},
        {0xD30D, 0x11},
        {0xD30E, 0x01},
        {0xD30F, 0x58},
        {0xD310, 0x01},
        {0xD311, 0x76},
        {0xD312, 0x01},
        {0xD313, 0xA6},
        {0xD314, 0x01},
        {0xD315, 0xCD},
        {0xD316, 0x02},
        {0xD317, 0x0E},
        {0xD318, 0x02},
        {0xD319, 0x46},
        {0xD31A, 0x02},
        {0xD31B, 0x48},
        {0xD31C, 0x02},
        {0xD31D, 0x78},
        {0xD31E, 0x02},
        {0xD31F, 0xAC},
        {0xD320, 0x02},
        {0xD321, 0xCD},
        {0xD322, 0x02},
        {0xD323, 0xFD},
        {0xD324, 0x03},
        {0xD325, 0x1F},
        {0xD326, 0x03},
        {0xD327, 0x4B},
        {0xD328, 0x03},
        {0xD329, 0x69},
        {0xD32A, 0x03},
        {0xD32B, 0x8E},
        {0xD32C, 0x03},
        {0xD32D, 0xA5},
        {0xD32E, 0x03},
        {0xD32F, 0xCD},
        {0xD330, 0x03},
        {0xD331, 0xF1},
        {0xD332, 0x03},
        {0xD333, 0xF1},
        {0xD400, 0x00},
        {0xD401, 0x37},
        {0xD402, 0x00},
        {0xD403, 0x4A},
        {0xD404, 0x00},
        {0xD405, 0x6F},
        {0xD406, 0x00},
        {0xD407, 0x8D},
        {0xD408, 0x00},
        {0xD409, 0xAD},
        {0xD40A, 0x00},
        {0xD40B, 0xDF},
        {0xD40C, 0x01},
        {0xD40D, 0x11},
        {0xD40E, 0x01},
        {0xD40F, 0x58},
        {0xD410, 0x01},
        {0xD411, 0x76},
        {0xD412, 0x01},
        {0xD413, 0xA6},
        {0xD414, 0x01},
        {0xD415, 0xCD},
        {0xD416, 0x02},
        {0xD417, 0x0E},
        {0xD418, 0x02},
        {0xD419, 0x46},
        {0xD41A, 0x02},
        {0xD41B, 0x48},
        {0xD41C, 0x02},
        {0xD41D, 0x78},
        {0xD41E, 0x02},
        {0xD41F, 0xAC},
        {0xD420, 0x02},
        {0xD421, 0xCD},
        {0xD422, 0x02},
        {0xD423, 0xFD},
        {0xD424, 0x03},
        {0xD425, 0x1F},
        {0xD426, 0x03},
        {0xD427, 0x4B},
        {0xD428, 0x03},
        {0xD429, 0x69},
        {0xD42A, 0x03},
        {0xD42B, 0x8E},
        {0xD42C, 0x03},
        {0xD42D, 0xA5},
        {0xD42E, 0x03},
        {0xD42F, 0xCD},
        {0xD430, 0x03},
        {0xD431, 0xF1},
        {0xD432, 0x03},
        {0xD433, 0xF1},
        {0xD500, 0x00},
        {0xD501, 0x37},
        {0xD502, 0x00},
        {0xD503, 0x4A},
        {0xD504, 0x00},
        {0xD505, 0x6F},
        {0xD506, 0x00},
        {0xD507, 0x8D},
        {0xD508, 0x00},
        {0xD509, 0xAD},
        {0xD50A, 0x00},
        {0xD50B, 0xDF},
        {0xD50C, 0x01},
        {0xD50D, 0x11},
        {0xD50E, 0x01},
        {0xD50F, 0x58},
        {0xD510, 0x01},
        {0xD511, 0x76},
        {0xD512, 0x01},
        {0xD513, 0xA6},
        {0xD514, 0x01},
        {0xD515, 0xCD},
        {0xD516, 0x02},
        {0xD517, 0x0E},
        {0xD518, 0x02},
        {0xD519, 0x46},
        {0xD51A, 0x02},
        {0xD51B, 0x48},
        {0xD51C, 0x02},
        {0xD51D, 0x78},
        {0xD51E, 0x02},
        {0xD51F, 0xAC},
        {0xD520, 0x02},
        {0xD521, 0xCD},
        {0xD522, 0x02},
        {0xD523, 0xFD},
        {0xD524, 0x03},
        {0xD525, 0x1F},
        {0xD526, 0x03},
        {0xD527, 0x4B},
        {0xD528, 0x03},
        {0xD529, 0x69},
        {0xD52A, 0x03},
        {0xD52B, 0x8E},
        {0xD52C, 0x03},
        {0xD52D, 0xA5},
        {0xD52E, 0x03},
        {0xD52F, 0xCD},
        {0xD530, 0x03},
        {0xD531, 0xF1},
        {0xD532, 0x03},
        {0xD533, 0xF1},
        {0xD600, 0x00},
        {0xD601, 0x37},
        {0xD602, 0x00},
        {0xD603, 0x4A},
        {0xD604, 0x00},
        {0xD605, 0x6F},
        {0xD606, 0x00},
        {0xD607, 0x8D},
        {0xD608, 0x00},
        {0xD609, 0xAD},
        {0xD60A, 0x00},
        {0xD60B, 0xDF},
        {0xD60C, 0x01},
        {0xD60D, 0x11},
        {0xD60E, 0x01},
        {0xD60F, 0x58},
        {0xD610, 0x01},
        {0xD611, 0x76},
        {0xD612, 0x01},
        {0xD613, 0xA6},
        {0xD614, 0x01},
        {0xD615, 0xCD},
        {0xD616, 0x02},
        {0xD617, 0x0E},
        {0xD618, 0x02},
        {0xD619, 0x46},
        {0xD61A, 0x02},
        {0xD61B, 0x48},
        {0xD61C, 0x02},
        {0xD61D, 0x78},
        {0xD61E, 0x02},
        {0xD61F, 0xAC},
        {0xD620, 0x02},
        {0xD621, 0xCD},
        {0xD622, 0x02},
        {0xD623, 0xFD},
        {0xD624, 0x03},
        {0xD625, 0x1F},
        {0xD626, 0x03},
        {0xD627, 0x4B},
        {0xD628, 0x03},
        {0xD629, 0x69},
        {0xD62A, 0x03},
        {0xD62B, 0x8E},
        {0xD62C, 0x03},
        {0xD62D, 0xA5},
        {0xD62E, 0x03},
        {0xD62F, 0xCD},
        {0xD630, 0x03},
        {0xD631, 0xF1},
        {0xD632, 0x03},
        {0xD633, 0xF1},
        {0xF000, 0x55},
        {0xF001, 0xAA},
        {0xF002, 0x52},
        {0xF003, 0x08},
        {0xF004, 0x00},
        {0xB400, 0x10},
        {0x3A00, 0x77},
        {0xB101, 0x00},
        {0x1100, 0x00},
        {0x2900, 0x00},
    { REG_END, 0 }
};

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_LCD LCD_SHARP_LQ040Y3DX80A_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ040Y3DX80A LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ040Y3DX80A_InitializeWorkspace_16Bit(void *aW)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ040Y3DX80A_configuration_16Bit;
    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ040Y3DX80A LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ040Y3DX80A_InitializeWorkspace_I15Bit(void *aW)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ040Y3DX80A_configuration_I15Bit;
    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ040Y3DX80A LCD.
 * Inputs:
 *      void *aW                    -- Particular  workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ040Y3DX80A_InitializeWorkspace_8Bit(void *aW)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ040Y3DX80A_configuration_8Bit;
    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

static T_uezError SPIWriteCmd(
        T_LQ040Y3DX80AWorkspace *p,
        const T_lcdSPICmd *aCmd)
{
    SPI_Request r;
    TUInt8 cmd[4];
    TUInt8 data[3];
    T_uezError error = UEZ_ERROR_NONE;

#define LCD_SPI_WRITE       (0<<7)
#define LCD_SPI_READ        (1<<7)
#define LCD_SPI_CMD         (0<<6)
#define LCD_SPI_DATA        (1<<6)
#define LCD_SPI_HIGH        (1<<5)
#define LCD_SPI_LOW         (0<<5)


    cmd[0] = LCD_SPI_WRITE|LCD_SPI_CMD|LCD_SPI_HIGH;
    cmd[1] = (aCmd->iReg & 0xFF00) >> 8;
    //cmd[2] = (aCmd->iReg & 0x00FF);
    r.iNumTransfers = 2;
    r.iBitsPerTransfer = 8;
    r.iRate = 1000; // spec says 12 MHz, but we'll start slow at 1 MHz
    r.iClockOutPolarity = ETrue; // rising edge
    r.iClockOutPhase = ETrue;
    r.iCSGPIOPort = GPIO_TO_HAL_PORT(p->iCSGPIOPin);
    r.iCSGPIOBit = GPIO_TO_PIN_BIT(p->iCSGPIOPin);
    r.iCSPolarity = EFalse;
    r.iDataMISO = cmd;
    r.iDataMOSI = cmd;
    error = UEZSPITransferPolled(p->iSPI, &r);

    if(!error){
        cmd[0] = LCD_SPI_WRITE|LCD_SPI_CMD|LCD_SPI_LOW;
        cmd[1] = (aCmd->iReg & 0x00FF);
        error = UEZSPITransferPolled(p->iSPI, &r);
    }

    if (!error) {
        data[0] = LCD_SPI_WRITE|LCD_SPI_DATA|LCD_SPI_LOW;
        data[1] = aCmd->iData;
        //data[2] = aCmd->iData & 0xFF;
        r.iNumTransfers = 2;
        r.iDataMOSI = data;
        r.iDataMISO = data;
        error = UEZSPITransferPolled(p->iSPI, &r);
    }

    return error;
}

static T_uezError ISPIWriteCommands(
        T_LQ040Y3DX80AWorkspace *p,
        const T_lcdSPICmd *aCmd)
{
    T_uezError error = UEZ_ERROR_NONE;
    while (aCmd->iReg != REG_END) {
        error = SPIWriteCmd(p, aCmd);
        if (error)
            break;
        aCmd++;
    }
    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  ISPIConfigure
 *---------------------------------------------------------------------------*
 * Description:
 *      program lcd with SPI commands
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If programming is successful, returns
 *                                  UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError ISPIConfigure(T_LQ040Y3DX80AWorkspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    if(strcmp(p->iSPIBus, "\0") == 0){
        //See LCD_LQ040Y3DX80A_Create to fix.
        UEZFailureMsg("SPI Bus not Configured");
    }
    error = UEZSPIOpen(p->iSPIBus, &p->iSPI);
    if (!error) {
            if (!error) {
                UEZTaskDelay(1);
                error = ISPIWriteCommands(p, G_lcdStartup);
            }
        UEZSPIClose(p->iSPI);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the LCD screen.  If this is the first open, initialize the
 *      screen.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is opened, returns
 *                                  UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_Open(void *aW)
{
  T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
  HAL_LCDController **plcdc;
  T_uezError error = UEZ_ERROR_NONE;
  
  p->aNumOpen++;
  
  if (p->aNumOpen == 1) {
    UEZGPIOOutput(p->iResetGPIOPin);
    
    plcdc = p->iLCDController;
    if (!error) {
      switch (p->iConfiguration->iColorDepth) {
      case UEZLCD_COLOR_DEPTH_8_BIT:
        LCD_LQ040Y3DX80A_settings = LCD_LQ040Y3DX80A_params8bit;
        break;
      default:
      case UEZLCD_COLOR_DEPTH_16_BIT:
        LCD_LQ040Y3DX80A_settings = LCD_LQ040Y3DX80A_params16bit;
        break;
      case UEZLCD_COLOR_DEPTH_I15_BIT:
        LCD_LQ040Y3DX80A_settings = LCD_LQ040Y3DX80A_paramsI15bit;
        break;
      }
      LCD_LQ040Y3DX80A_settings.iBaseAddress = p->iBaseAddress;
      
      // Toggle the rest pin and hold SHUT H until programming done
      UEZGPIOSet(p->iResetGPIOPin);
      UEZGPIOClear(p->iResetGPIOPin);
      UEZTaskDelay(10); // delay in ms, min 1 ms delay
      UEZGPIOSet(p->iResetGPIOPin);
      UEZTaskDelay(200);
      
      // start DOTCLK immediately
      error = (*plcdc)->Configure(plcdc, &LCD_LQ040Y3DX80A_settings);
      // start HSYNC and VSYNC (same as UEZLCDOn(lcd))
      (*p->iLCDController)->On(p->iLCDController);
      
      // program registers, configure the LCD over the SPI port
      error = ISPIConfigure(p);
      
      UEZTaskDelay(10); // min 5ms delay
      // go to normal mode from sleep mode by clearing SHUT
      UEZTaskDelay(10); // delay before turning backlight on, min 10 frame
      (*p->iBacklight)->On(p->iBacklight); // turn backlight on 
    }
  }
  return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_Close(void *aW)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ040Y3DX80A_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;
    p->iBacklight = aBacklight;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_GetInfo(void *aW, T_uezLCDConfiguration *aConfiguration)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_GetFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Returns a pointer to the frame memory in the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 aFrame          -- Index to frame (0 based)
 *      void **aFrameBuffer     -- Pointer to base address
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_ShowFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Makes the passed frame the actively viewed frame on the LCD
 *      (if not already).
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 aFrame          -- Index to frame (0 based)
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_On(void *aW)
{
    // Turn back on to the remembered level
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->On(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_Off(void *aW)
{
    // Turn off, but don't remember the level
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    (*p->iLCDController)->Off(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->Off(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_SetBacklightLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on or off the backlight.  A value of 0 is off and values of 1
 *      or higher is higher levels of brightness (dependent on the LCD
 *      display).  If a display is on/off only, this value will be 1 or 0
 *      respectively.
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 aLevel          -- Level of backlight
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the backlight intensity level is
 *                                  invalid, returns UEZ_ERROR_OUT_OF_RANGE.
 *                                  Returns UEZ_ERROR_NOT_SUPPORTED if
 *                                  no backlight for LCD.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;
    TUInt16 level;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    // Limit the backlight level
    if (aLevel > p->iConfiguration->iNumBacklightLevels)
        aLevel = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    p->iBacklightLevel = aLevel;

    // Scale backlight to be 0 - 0xFFFF
    level = (aLevel * 0xFFFF) / p->iConfiguration->iNumBacklightLevels;

    return (*p->iBacklight)->SetRatio(p->iBacklight, level);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_GetBacklightLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Returns the current backlight level as well as the number of
 *      maximum light levels available.
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 *aLevel          -- Level of backlight returned
 *      TUInt32 *aNumLevels      -- Number of levels maximum
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the backlight intensity level is
 *                                  invalid, returns UEZ_ERROR_OUT_OF_RANGE.
 *                                  Returns UEZ_ERROR_NOT_SUPPORTED if
 *                                  no backlight for LCD.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_SetPaletteColor
 *---------------------------------------------------------------------------*
 * Description:
 *      Change the color of a palette entry given the red, green, blue
 *      component of a particular color index.  The color components are
 *      expressed in full 16-bit values at a higher resolution than
 *      the hardware can usually perform.  The color is down shifted to what
 *      the hardware can handle.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aColorIndex         -- Index to palate entry
 *      TUInt16 aRed                -- Red value
 *      TUInt16 aGreen              -- Green value
 *      TUInt16 aBlue               -- Blue value
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the index is invalid, returns
 *                                  UEZ_ERROR_OUT_OF_RANGE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_LQ040Y3DX80A_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_WaitForVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for a vertical sync be enabling the vertical sync callback
 *      and waiting on a semaphore.
 * Inputs:
 *      void *aWorkspace -- LCD workspace
 *      TUInt32 aTimeout -- Timeout in ms or UEZ_TIMEOUT_INFINITE
 * Outputs:
 *      T_uezError -- UEZ_ERROR_TIMEOUT or UEZ_ERROR_OK.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ040Y3DX80A_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_LQ040Y3DX80AWorkspace *p = (T_LQ040Y3DX80AWorkspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_LQ040Y3DX80A_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ040Y3DX80A_WaitForVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Routine to configure additional hardware requirements.
 * Inputs:
 *      void *aWorkspace -- LCD workspace
 *      const char* -- SPI or SSP bus the LCD on on (ex. "SSP0")
 *
 *---------------------------------------------------------------------------*/
void LCD_LQ040Y3DX80A_Create(const char *aName,
                            char* aSPIBus,
                            T_uezGPIOPortPin aResetPin,
                            T_uezGPIOPortPin aSPICSPin)
{
    T_LQ040Y3DX80AWorkspace *p;

    T_uezDevice lcd;
    T_uezDeviceWorkspace *p_lcd;

    UEZDeviceTableFind(aName, &lcd);
    UEZDeviceTableGetWorkspace(lcd, &p_lcd);

    p = (T_LQ040Y3DX80AWorkspace *)p_lcd;

    strcpy(p->iSPIBus, aSPIBus);
    p->iCSGPIOPin = aSPICSPin;
    p->iResetGPIOPin = aResetPin;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_SHARP_LQ040Y3DX80A_Interface_16Bit = {
    {
        // Common interface
        "LCD:SHARP_LQ040Y3DX80A:16Bit",
        0x0100,
        LCD_LQ040Y3DX80A_InitializeWorkspace_16Bit,
        sizeof(T_LQ040Y3DX80AWorkspace),
    },

    // Functions
    LCD_LQ040Y3DX80A_Open,
    LCD_LQ040Y3DX80A_Close,
    LCD_LQ040Y3DX80A_Configure,
    LCD_LQ040Y3DX80A_GetInfo,
    LCD_LQ040Y3DX80A_GetFrame,
    LCD_LQ040Y3DX80A_ShowFrame,
    LCD_LQ040Y3DX80A_On,
    LCD_LQ040Y3DX80A_Off,
    LCD_LQ040Y3DX80A_SetBacklightLevel,
    LCD_LQ040Y3DX80A_GetBacklightLevel,
    LCD_LQ040Y3DX80A_SetPaletteColor,

    // v2.04
    LCD_LQ040Y3DX80A_WaitForVerticalSync,
};

const DEVICE_LCD LCD_SHARP_LQ040Y3DX80A_Interface_I15Bit = {
    {
        // Common interface
        "LCD:SHARP_LQ040Y3DX80A:I15Bit",
        0x0100,
        LCD_LQ040Y3DX80A_InitializeWorkspace_I15Bit,
        sizeof(T_LQ040Y3DX80AWorkspace),
    },

    // Functions
    LCD_LQ040Y3DX80A_Open,
    LCD_LQ040Y3DX80A_Close,
    LCD_LQ040Y3DX80A_Configure,
    LCD_LQ040Y3DX80A_GetInfo,
    LCD_LQ040Y3DX80A_GetFrame,
    LCD_LQ040Y3DX80A_ShowFrame,
    LCD_LQ040Y3DX80A_On,
    LCD_LQ040Y3DX80A_Off,
    LCD_LQ040Y3DX80A_SetBacklightLevel,
    LCD_LQ040Y3DX80A_GetBacklightLevel,
    LCD_LQ040Y3DX80A_SetPaletteColor,

    // v2.04
    LCD_LQ040Y3DX80A_WaitForVerticalSync,
};

const DEVICE_LCD LCD_SHARP_LQ040Y3DX80A_Interface_8Bit = {
    {
        // Common interface
        "LCD:SHARP_LQ040Y3DX80A:8Bit",
        0x0100,
        LCD_LQ040Y3DX80A_InitializeWorkspace_8Bit,
        sizeof(T_LQ040Y3DX80AWorkspace),
    },

    // Functions
    LCD_LQ040Y3DX80A_Open,
    LCD_LQ040Y3DX80A_Close,
    LCD_LQ040Y3DX80A_Configure,
    LCD_LQ040Y3DX80A_GetInfo,
    LCD_LQ040Y3DX80A_GetFrame,
    LCD_LQ040Y3DX80A_ShowFrame,
    LCD_LQ040Y3DX80A_On,
    LCD_LQ040Y3DX80A_Off,
    LCD_LQ040Y3DX80A_SetBacklightLevel,
    LCD_LQ040Y3DX80A_GetBacklightLevel,
    LCD_LQ040Y3DX80A_SetPaletteColor,

    // v2.04
    LCD_LQ040Y3DX80A_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_SHARP_LQ040Y3DX80A_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ040Y3DX80A_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ040Y3DX80A_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ040Y3DX80A_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  Sharp_LQ040Y3DX80A.c
 *-------------------------------------------------------------------------*/
