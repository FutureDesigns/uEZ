/*-------------------------------------------------------------------------*
 * File:  LPC546xx_Flexcomm.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

void LPC546xx_Flexcomm_6_Require(const T_LPC546xx_FlexcommPins *aPins)
{
    T_halWorkspace *p_lcd;

    static const T_LPC546xx_ICON_ConfigList pinsVD0[] = {
            {GPIO_P3_6,  IOCON_D_DEFAULT(2)}, // LCD_VD0
    };


    // Register LCD Controller
    HALInterfaceRegister("LCDController",
            (T_halInterface *)&LCDController_LPC546xx_Interface, 0, &p_lcd);

    LPC546xx_ICON_ConfigPinOrNone(aPins->iPWR, pinsPWR,
            ARRAY_COUNT(pinsPWR));

}
/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_Flexcomm.c
 *-------------------------------------------------------------------------*/
