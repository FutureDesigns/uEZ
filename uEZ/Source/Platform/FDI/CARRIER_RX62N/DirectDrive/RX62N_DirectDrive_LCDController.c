/*-------------------------------------------------------------------------*
 * File:  LCDController.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the H8SX RF1668 LCD Controller.
 * Implementation:
 *      Most of the configuration comes down to the single table passed
 *      in.  From there, we map SDRAM to the LCD.
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
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include "DirectLCD_Types.h"
#include "DirectLCD.h"
#include "DirectLCD_CNF.h"
#include "RX62N_UtilityFuncs.h"
#include "RX62N_DirectDrive_LCDController.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_LCDController *iHAL;
    TUInt32 iBaseAddress;
} T_workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const HAL_LCDController LCDController_RX62N_DirectDrive_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_DirectDrive_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this LCD Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_DirectDrive_InitializeWorkspace(void *aWorkspace)
{
    T_workspace *p = (T_workspace *)aWorkspace;
    p->iHAL = &LCDController_RX62N_DirectDrive_Interface;
    p->iBaseAddress = 0x400000;    // use default address
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_DirectDrive_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure and setup the LCD screen with the given settings
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      T_LCDControllerSettings *aSettings -- Pointer to settings to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_DirectDrive_Configure(
            void *aWorkspace,
            T_LCDControllerSettings *aSettings)
{
    LCDInit(aSettings);
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_DirectDrive_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Power on and enable the LCD.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_DirectDrive_On(void *aWorkspace)
{
    // Ensure the LCD is enabled and powered
// TBD: Should this delay be done different?
    UEZBSPDelayMS(200);

    LCDOn();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_DirectDrive_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Power off and disable the LCD.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_DirectDrive_Off(void *aWorkspace)
{
    // Ensure the LCD is enabled and powered
    LCDOff();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_DirectDrive_SetBacklightLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Control the backlight level.  For LPC247x, this is not a feature.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      TUInt32 aLevel            -- Level to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_DirectDrive_SetBacklightLevel(
                void *aWorkspace,
                TUInt32 aLevel)
{
#if 0
    if (aLevel)
        LCDOn();
    else
        LCDOff();
#endif
        
    // Not handled here in this LCD Controller
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_DirectDrive_SetBaseAddr
 *---------------------------------------------------------------------------*
 * Description:
 *      Changes the address in memory where the LCD controller gets it's
 *      pixels and if that change should occur synchronously.
 * Inputs:
 *      void *aWorkspace          -- LCD Controller's workspace
 *      TUInt32 aBaseAddress      -- New pixel memory base address
 *      TBool aSync               -- Wait for raster?
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_DirectDrive_SetBaseAddr(
            void *aWorkspace,
            TUInt32 aBaseAddress,
            TBool aSync)
{
	T_workspace *p = (T_workspace *)aWorkspace;
    PARAM_NOT_USED(aSync);

	p->iBaseAddress = aBaseAddress;
	LCDSetLineSource(0, FRAME_HEIGHT, (Raster_type *)p->iBaseAddress, FRAME_WIDTH);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_LCDController LCDController_RX62N_DirectDrive_Interface = {
    "RX62N:DirectDrive:LCDController",
    0x0100,
    RX62N_DirectDrive_InitializeWorkspace,
    sizeof(T_workspace),

    LCDCONTROLLER_FEATURE_POWER|
        LCDCONTROLLER_FEATURE_DYNAMIC_BASE_ADDR,

    RX62N_DirectDrive_Configure,
    RX62N_DirectDrive_On,
    RX62N_DirectDrive_Off,
    RX62N_DirectDrive_SetBacklightLevel,
    RX62N_DirectDrive_SetBaseAddr,
    0, // RX62N_DirectDrive_SetPaletteColor,
};

void RX62N_DirectDrive_LCDController_Require(T_uezGPIOPortPin *pins)
{
	
	HAL_DEVICE_REQUIRE_ONCE();
	HALInterfaceRegister(
        "LCDController",
        (T_halInterface *)&LCDController_RX62N_DirectDrive_Interface,
        0,
        0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
}

/*-------------------------------------------------------------------------*
 * End of File:  LCDController.c
 *-------------------------------------------------------------------------*/
