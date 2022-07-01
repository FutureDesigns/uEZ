/*-------------------------------------------------------------------------*
 * File: EnterNameScreen.c
 *-------------------------------------------------------------------------*/
/** @addtogroup EnterNameScreen
 * @{
 *     @brief Implements Layout and functionality of EnterNameScreen.c.
 * 
 *     How it works in detail goes here ....
 */
/*-------------------------------------------------------------------------*/
//Includes needed by emWin
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include "string.h"
#include <stdio.h>

//uEZ and Application Includes
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Graphics/Graphics.h"
#include "Fonts/Fonts.h"
#include "../Bowlers.h"
#include "Audio.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_TITLE_TEXT           (GUI_ID_USER + 0x01)
#define ID_BACKSPACE_TEXT       (GUI_ID_USER + 0x02)
#define ID_BACKSPACE_BUTTON     (GUI_ID_USER + 0x03)
#define ID_OK_BUTTON            (GUI_ID_USER + 0x04)
#define ID_NAME_EDIT_BOX        (GUI_ID_USER + 0x05)
#define ID_BACK_BUTTON          (GUI_ID_USER + 0x06)

#define ID_Q_BUTTON             (GUI_ID_USER + 0x08)
#define ID_W_BUTTON             (GUI_ID_USER + 0x09)
#define ID_E_BUTTON             (GUI_ID_USER + 0x0A)
#define ID_R_BUTTON             (GUI_ID_USER + 0x0B)
#define ID_T_BUTTON             (GUI_ID_USER + 0x0C)
#define ID_Y_BUTTON             (GUI_ID_USER + 0x0D)
#define ID_U_BUTTON             (GUI_ID_USER + 0x0E)
#define ID_I_BUTTON             (GUI_ID_USER + 0x0F)
#define ID_O_BUTTON             (GUI_ID_USER + 0x10)
#define ID_P_BUTTON             (GUI_ID_USER + 0x11)

#define ID_A_BUTTON             (GUI_ID_USER + 0x12)
#define ID_S_BUTTON             (GUI_ID_USER + 0x13)
#define ID_D_BUTTON             (GUI_ID_USER + 0x14)
#define ID_F_BUTTON             (GUI_ID_USER + 0x15)
#define ID_G_BUTTON             (GUI_ID_USER + 0x16)
#define ID_H_BUTTON             (GUI_ID_USER + 0x17)
#define ID_J_BUTTON             (GUI_ID_USER + 0x18)
#define ID_K_BUTTON             (GUI_ID_USER + 0x19)
#define ID_L_BUTTON             (GUI_ID_USER + 0x1A)

#define ID_Z_BUTTON             (GUI_ID_USER + 0x1B)
#define ID_X_BUTTON             (GUI_ID_USER + 0x1C)
#define ID_C_BUTTON             (GUI_ID_USER + 0x1D)
#define ID_V_BUTTON             (GUI_ID_USER + 0x1E)
#define ID_B_BUTTON             (GUI_ID_USER + 0x1F)
#define ID_N_BUTTON             (GUI_ID_USER + 0x20)
#define ID_M_BUTTON             (GUI_ID_USER + 0x21)
#define ID_SHIFT_BUTTON         (GUI_ID_USER + 0x23)
#define ID_SPACE_BUTTON         (GUI_ID_USER + 0x22)

#define ID_EDIT_BOX             (GUI_ID_USER + 0x24)


#define SPACING                 (10)
#define TITLE_TEXT_YSIZE        (35)

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)  //Code added by IMM - Used to define the size of the window for spacing out buttons in the layout
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT) //Code added by IMM - Used to define the size of the window for spacing out buttons in the layout

#define BACKSPACE_TEXT_XSIZE      (BACKSPACE_BUTTON_XSIZE)
#define BACKSPACE_TEXT_YSIZE      (BACKSPACE_BUTTON_XSIZE)
#define BACKSPACE_TEXT_YPOS       (SPACING * 2)

#define BACKSPACE_BUTTON_XSIZE    (95)
#define BACKSPACE_BUTTON_YSIZE    (95)
#define BACKSPACE_BUTTON_YPOS     (SPACING + BACKSPACE_TEXT_YPOS + SPACING)

#define BACKSPACE_XPOS            ((UEZ_LCD_DISPLAY_WIDTH/4) - (BACKSPACE_BUTTON_XSIZE/2))

#define OK_BUTTON_XSIZE         (135)
#define OK_BUTTON_YSIZE         (125)
#define OK_BUTTON_XPOS          (((UEZ_LCD_DISPLAY_WIDTH/4) + (UEZ_LCD_DISPLAY_WIDTH/2)) - (OK_BUTTON_XSIZE/2))
#define OK_BUTTON_YPOS          (SPACING*2)

/*  Code Removed by IMM - Replacing cancel button with ID_BACK_BUTTON
#define CANCEL_BUTTON_XSIZE     (128)
#define CANCEL_BUTTON_YSIZE     (45)
#define CANCEL_BUTTON_XPOS      (SPACING * 2)
#define CANCEL_BUTTON_YPOS      (UEZ_LCD_DISPLAY_HEIGHT - SPACING - CANCEL_BUTTON_YSIZE)
*/

#define BACK_BUTTON_XSIZE       ((WINDOW_XSIZE/6))
#define BACK_BUTTON_YSIZE       ((WINDOW_YSIZE/8))
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - SPACING - BACK_BUTTON_YSIZE)   

#define ROW_BUTTON_XSIZE        (78)
#define ROW_BUTTON_YSIZE        (74)

#define ROW_1_XPOS(n)           (1 + ( n * (ROW_BUTTON_XSIZE + 2)))
#define ROW_1_YPOS              (UEZ_LCD_DISPLAY_HEIGHT - SPACING - (4 * (ROW_BUTTON_YSIZE + 2)))

#define ROW_2_XPOS(n)           (1 + (ROW_BUTTON_XSIZE/2) + ( n * (ROW_BUTTON_XSIZE + 2)))
#define ROW_2_YPOS              (UEZ_LCD_DISPLAY_HEIGHT - SPACING - (3 * (ROW_BUTTON_YSIZE + 2)))

#define ROW_3_XPOS(n)           (1 + (ROW_BUTTON_XSIZE) + ( n * (ROW_BUTTON_XSIZE + 2)))
#define ROW_3_YPOS              (UEZ_LCD_DISPLAY_HEIGHT - SPACING - (2 * (ROW_BUTTON_YSIZE + 2)))

#define SHIFT_BUTTON_XSIZE       (150)
#define SHIFT_BUTTON_YSIZE       (ROW_BUTTON_YSIZE)

#define SPACE_BAR_XSIZE         (4 * (ROW_BUTTON_XSIZE + 2))
#define SPACE_BAR_YSIZE         (ROW_BUTTON_YSIZE)
#define SPACE_BAR_XPOS          (1 + (3 * (ROW_BUTTON_XSIZE + 2)))
#define SPACE_BAR_YPOS          (UEZ_LCD_DISPLAY_HEIGHT - SPACING - (1 * (ROW_BUTTON_YSIZE + 2)))

#define MAPPING_UPPER           (0)
#define MAPPING_LOWER           (1)

#define EDIT_BOX_XSIZE          (UEZ_LCD_DISPLAY_WIDTH - (BACKSPACE_XPOS + BACKSPACE_BUTTON_XSIZE) -  (UEZ_LCD_DISPLAY_WIDTH - OK_BUTTON_XPOS) - ( 2 * SPACING))
#define EDIT_BOX_YSIZE          (UEZ_LCD_DISPLAY_HEIGHT - TITLE_TEXT_YSIZE - (5 * (ROW_BUTTON_YSIZE + 2) - (SPACING *3)))
#define EDIT_BOX_XPOS           ( BACKSPACE_XPOS + BACKSPACE_BUTTON_XSIZE + SPACING)
#define EDIT_BOX_YPOS           (TITLE_TEXT_YSIZE + SPACING)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleCancel(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleOK(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleBackspace(WM_MESSAGE * pMsg, int aNCode, int aID);// Changed by IMM - Modified into a backspace for the name entry. Need to rename
static TBool IHandleCharacter(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleSpace(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleShift(WM_MESSAGE * pMsg, int aNCode, int aID);
static void ISetKeyboardMapping(WM_MESSAGE *pMsg, int aMapping);
static void IUpdateFields(WM_MESSAGE * pMsg);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iEnterNameScreenDialog[] = {
     //Function                 Name            ID                  XP                       YP     				    XS                       YS
    { WINDOW_CreateIndirect     , ""            , ID_WINDOW           , 0                     , 0 		                , UEZ_LCD_DISPLAY_WIDTH     , UEZ_LCD_DISPLAY_HEIGHT  , 0, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_TITLE_TEXT       , 0                     , 0                       , UEZ_LCD_DISPLAY_WIDTH     , TITLE_TEXT_YSIZE        , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { TEXT_CreateIndirect       , ""            , ID_BACKSPACE_TEXT   , BACKSPACE_XPOS        , BACKSPACE_TEXT_YPOS     , BACKSPACE_TEXT_XSIZE      , BACKSPACE_TEXT_YSIZE    , TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BACKSPACE_BUTTON , BACKSPACE_XPOS        , BACKSPACE_BUTTON_YPOS   , BACKSPACE_BUTTON_XSIZE    , BACKSPACE_BUTTON_YSIZE  , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_OK_BUTTON        , OK_BUTTON_XPOS        , OK_BUTTON_YPOS          , OK_BUTTON_XSIZE           , OK_BUTTON_YSIZE         , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_BACK_BUTTON      , BACK_BUTTON_XPOS      , BACK_BUTTON_YPOS        , BACK_BUTTON_XSIZE         , BACK_BUTTON_YSIZE       , 0, 0, 0},    //Code modified by IMM - Changed ID_CANCEL_BUTTON to ID_MENU_BUTTON and changed size and position variables

    { BUTTON_CreateIndirect     , ""            , ID_Q_BUTTON         , ROW_1_XPOS(0)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_W_BUTTON         , ROW_1_XPOS(1)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_E_BUTTON         , ROW_1_XPOS(2)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_R_BUTTON         , ROW_1_XPOS(3)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_T_BUTTON         , ROW_1_XPOS(4)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_Y_BUTTON         , ROW_1_XPOS(5)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_U_BUTTON         , ROW_1_XPOS(6)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_I_BUTTON         , ROW_1_XPOS(7)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_O_BUTTON         , ROW_1_XPOS(8)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_P_BUTTON         , ROW_1_XPOS(9)         , ROW_1_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},

    { BUTTON_CreateIndirect     , ""            , ID_A_BUTTON         , ROW_2_XPOS(0)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_S_BUTTON         , ROW_2_XPOS(1)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_D_BUTTON         , ROW_2_XPOS(2)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_F_BUTTON         , ROW_2_XPOS(3)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_G_BUTTON         , ROW_2_XPOS(4)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_H_BUTTON         , ROW_2_XPOS(5)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_J_BUTTON         , ROW_2_XPOS(6)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_K_BUTTON         , ROW_2_XPOS(7)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_L_BUTTON         , ROW_2_XPOS(8)         , ROW_2_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},

    { BUTTON_CreateIndirect     , ""            , ID_Z_BUTTON         , ROW_3_XPOS(0)         , ROW_3_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_X_BUTTON         , ROW_3_XPOS(1)         , ROW_3_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_C_BUTTON         , ROW_3_XPOS(2)         , ROW_3_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_V_BUTTON         , ROW_3_XPOS(3)         , ROW_3_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_B_BUTTON         , ROW_3_XPOS(4)         , ROW_3_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_N_BUTTON         , ROW_3_XPOS(5)         , ROW_3_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_M_BUTTON         , ROW_3_XPOS(6)         , ROW_3_YPOS              , ROW_BUTTON_XSIZE          , ROW_BUTTON_YSIZE        , 0, 0, 0},

    { BUTTON_CreateIndirect     , ""            , ID_SHIFT_BUTTON   , ROW_3_XPOS(7)           , ROW_3_YPOS              , SHIFT_BUTTON_XSIZE        , SHIFT_BUTTON_YSIZE      , 0, 0, 0},
    { BUTTON_CreateIndirect     , ""            , ID_SPACE_BUTTON   , SPACE_BAR_XPOS          , SPACE_BAR_YPOS          , SPACE_BAR_XSIZE           , SPACE_BAR_YSIZE         , 0, 0, 0},

    { EDIT_CreateIndirect       , ""            , ID_EDIT_BOX       , EDIT_BOX_XPOS         , EDIT_BOX_YPOS         , EDIT_BOX_XSIZE            , EDIT_BOX_YSIZE        , EDIT_CF_HCENTER | EDIT_CF_VCENTER, 0, 0},
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping EnterNameScreenMapping[] = {
    { ID_WINDOW         ,""                     , GUI_BLACK         , GUI_BLACK , &MAGELLAN_FONT_SMALL  , LAFSetupWindow, 0},
    { ID_TITLE_TEXT     , "enter bowler x name" , GUI_TRANSPARENT   , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupText  , 0},
    { ID_BACKSPACE_TEXT , "bumpers"             , GUI_TRANSPARENT   , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupText  , 0},
    { ID_BACKSPACE_BUTTON , ""                  , GUI_TRANSPARENT   , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleBackspace},
    { ID_OK_BUTTON      , ""                    , GUI_TRANSPARENT   , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleOK},
    { ID_BACK_BUTTON    , "Back"                , GUI_GRAY          , GUI_BLACK , &MAGELLAN_FONT_LARGE  , LAFSetupButton, IHandleCancel},    //Code modified by IMM - Changed ID_CANCEL_BUTTON to ID_MENU_BUTTON and changed font and text color

    { ID_Q_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_W_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_E_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_R_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_T_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_Y_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_U_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_I_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_O_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_P_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},

    { ID_A_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_S_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_D_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_F_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_G_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_H_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_J_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_K_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_L_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},

    { ID_Z_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_X_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_C_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_V_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_B_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_N_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_M_BUTTON       , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleCharacter},
    { ID_SHIFT_BUTTON   , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleShift},

    { ID_SPACE_BUTTON   , ""                    , GUI_GRAY          , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupButton, IHandleSpace},
    { ID_EDIT_BOX       , ""                    , GUI_TRANSPARENT   , GUI_WHITE , &MAGELLAN_FONT_MEDIUM , LAFSetupEditBox  , 0},
    {0},
};

typedef struct {
        int iID;
        const char iChar;
        const char *iText;
}T_keyboardMapping;

static T_keyboardMapping G_keyboardMappingUppercase[] = {
    { ID_Q_BUTTON   , 'Q', "Q" },
    { ID_W_BUTTON   , 'W', "W" },
    { ID_E_BUTTON   , 'E', "E" },
    { ID_R_BUTTON   , 'R', "R" },
    { ID_T_BUTTON   , 'T', "T" },
    { ID_Y_BUTTON   , 'Y', "Y" },
    { ID_U_BUTTON   , 'U', "U" },
    { ID_I_BUTTON   , 'I', "I" },
    { ID_O_BUTTON   , 'O', "O" },
    { ID_P_BUTTON   , 'P', "P" },

    { ID_A_BUTTON   , 'A', "A" },
    { ID_S_BUTTON   , 'S', "S" },
    { ID_D_BUTTON   , 'D', "D" },
    { ID_F_BUTTON   , 'F', "F" },
    { ID_G_BUTTON   , 'G', "G" },
    { ID_H_BUTTON   , 'H', "H" },
    { ID_J_BUTTON   , 'J', "J" },
    { ID_K_BUTTON   , 'K', "K" },
    { ID_L_BUTTON   , 'L', "L" },

    { ID_Z_BUTTON   , 'Z', "Z" },
    { ID_X_BUTTON   , 'X', "X" },
    { ID_C_BUTTON   , 'C', "C" },
    { ID_V_BUTTON   , 'V', "V" },
    { ID_B_BUTTON   , 'B', "B" },
    { ID_N_BUTTON   , 'N', "N" },
    { ID_M_BUTTON   , 'M', "M" },
    
    { ID_SHIFT_BUTTON   , ' ', "SHIFT" },  //Code Added by IMM - Added "Shift" text to the button that was created for shift
    { ID_SPACE_BUTTON   , ' ', "SPACE" },
};

static T_keyboardMapping G_keyboardMappingLowercase[] = {
    { ID_Q_BUTTON   , 'q', "q" },
    { ID_W_BUTTON   , 'w', "w" },
    { ID_E_BUTTON   , 'e', "e" },
    { ID_R_BUTTON   , 'r', "r" },
    { ID_T_BUTTON   , 't', "t" },
    { ID_Y_BUTTON   , 'y', "y" },
    { ID_U_BUTTON   , 'u', "u" },
    { ID_I_BUTTON   , 'i', "i" },
    { ID_O_BUTTON   , 'o', "o" },
    { ID_P_BUTTON   , 'p', "p" },

    { ID_A_BUTTON   , 'a', "a" },
    { ID_S_BUTTON   , 's', "s" },
    { ID_D_BUTTON   , 'd', "d" },
    { ID_F_BUTTON   , 'f', "f" },
    { ID_G_BUTTON   , 'g', "g" },
    { ID_H_BUTTON   , 'h', "h" },
    { ID_J_BUTTON   , 'j', "j" },
    { ID_K_BUTTON   , 'k', "k" },
    { ID_L_BUTTON   , 'l', "l" },

    { ID_Z_BUTTON   , 'z', "z" },
    { ID_X_BUTTON   , 'x', "x" },
    { ID_C_BUTTON   , 'c', "c" },
    { ID_V_BUTTON   , 'v', "v" },
    { ID_B_BUTTON   , 'b', "b" },
    { ID_N_BUTTON   , 'n', "n" },
    { ID_M_BUTTON   , 'm', "m" },

    { ID_SHIFT_BUTTON   , ' ', "SHIFT" },  //Code Added by IMM - Added "Shift" text to the button that was created for shift
    { ID_SPACE_BUTTON   , ' ', "SPACE" },
};

static T_keyboardMapping *G_keyboardCurrentMapping = G_keyboardMappingUppercase;

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;
static TUInt8 G_BowlerNumber = 0;
static WM_HWIN G_Win = 0;
static char G_Name[MAX_NAME_SIZE+1];
static TUInt8 G_NumChars = 0;

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/** 
 *      
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static void ISetButtonIcons(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;

    BUTTON_SetSkinFlexProps(&unPressedButtonProps, BUTTON_SKINFLEX_PI_ENABLED);
    BUTTON_SetSkinFlexProps(&pressedButtonProps, BUTTON_SKINFLEX_PI_PRESSED);
    BUTTON_SetSkinFlexProps(&unPressedButtonProps, BUTTON_SKINFLEX_PI_FOCUSSED);
    BUTTON_SetSkinFlexProps(&unPressedButtonProps, BUTTON_SKINFLEX_PI_DISABLED);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_OK_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmOK, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmOK, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmOK, 0, 0);

    /*  Code Removed by IMM - Code was used to draw the cancel button, but we are now using a button generated by emWIN
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BACK_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmCancel, 0, 0);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmCancel, 1, 1);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmCancel, 0, 0);
   */
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BACKSPACE_BUTTON);
    BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmBackspace, 0, 0);//Edited by IMM - Changed the bitmap from &bmBumper to &bmBackspace.
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED,   &bmBackspace, 1, 1);//Edited by IMM - Changed the bitmap from &bmBumper to &bmBackspace.
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED,  &bmBackspace, 0, 0);//Edited by IMM - Changed the bitmap from &bmBumper to &bmBackspace.

#if 0
    for( i = ID_Q_BUTTON; i < ID_SPACE_BUTTON; i ++){
        hItem = WM_GetDialogItem(pMsg->hWin, i);
        BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmKeypadKey, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmKeypadKeyPressed, 1, 1);
        BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmKeypadKey, 0, 0);
    }
#endif

    //Set spacebar
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleShift(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        if(G_keyboardCurrentMapping == G_keyboardMappingUppercase){
            G_keyboardCurrentMapping = G_keyboardMappingLowercase;
            ISetKeyboardMapping(pMsg, MAPPING_LOWER);
        } else {
            G_keyboardCurrentMapping = G_keyboardMappingUppercase;
            ISetKeyboardMapping(pMsg, MAPPING_UPPER);
        }
    } else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleSpace(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        if ( G_NumChars < MAX_NAME_SIZE){
            G_Name[G_NumChars] = ' ';
            G_NumChars++;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BOX);
            EDIT_SetText(hItem, G_Name);
        }
    }else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID			ID of widget that caused the event.
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleCancel(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        GUI_EndDialog(G_Win, 0);
#if MAGELLAN_DEBUG
        printf("Edit Name Canceled\n");
#endif
    }else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleOK(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        GUI_EndDialog(G_Win, 1);
#if MAGELLAN_DEBUG
        printf("Edit Name Closed\n");
#endif
    }else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleCharacter(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    WM_HWIN hItem;
    T_keyboardMapping *p;

    if (aNCode == WM_NOTIFICATION_RELEASED) {
        if ( G_NumChars < MAX_NAME_SIZE){
            p = G_keyboardCurrentMapping + (aID - (GUI_ID_USER + 0x08));
            if(G_keyboardCurrentMapping == G_keyboardMappingUppercase){
                G_keyboardCurrentMapping = G_keyboardMappingLowercase;
                ISetKeyboardMapping(pMsg, MAPPING_LOWER);
            }
            G_Name[G_NumChars] = p->iChar;
            G_NumChars++;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BOX);
            EDIT_SetText(hItem, G_Name);
        }
    }else if ( aNCode == WM_NOTIFICATION_CLICKED){
        ButtonClick();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/**
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID         ID of widget that caused the event.
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleBackspace(WM_MESSAGE * pMsg, int aNCode, int aID)
{
  WM_HWIN hItem;
  
    if (aNCode == WM_NOTIFICATION_RELEASED) {
//Code added by IMM - Implementation of backspace into the
//                    name text line.
//*******************************************************
        if ( G_NumChars != 0){
            G_NumChars--;
            G_Name[G_NumChars] = ' ';
 
            hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BOX);
            EDIT_SetText(hItem, G_Name);
            }
//***********************************************************            

    }else if ( aNCode == WM_NOTIFICATION_CLICKED){
        BeepError();
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine: IUpdateFields
 *---------------------------------------------------------------------------*/
/** Update all the fields on the screen.
 *      
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void IUpdateFields(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    char message[45];

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TITLE_TEXT);
    if( G_BowlerNumber != 0 ){
        sprintf(message, "enter bowler %01d name", G_BowlerNumber);
        TEXT_SetText(hItem, message);
    }
}

/*---------------------------------------------------------------------------*
 * Routine: ISetKeyboardMapping
 *---------------------------------------------------------------------------*/
/** Update all the fields on the screen.
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                          to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void ISetKeyboardMapping(WM_MESSAGE *pMsg, int aMapping)
{
    WM_HWIN hItem;
    TUInt32 i;
    T_keyboardMapping *p;

    if ( aMapping == MAPPING_UPPER){
        G_keyboardCurrentMapping = G_keyboardMappingUppercase;
    } else if (aMapping == MAPPING_LOWER){
        G_keyboardCurrentMapping = G_keyboardMappingLowercase;
    }

    p = G_keyboardCurrentMapping;
    for( i = ID_Q_BUTTON; i < ID_M_BUTTON + 1; i++){
        hItem= WM_GetDialogItem(pMsg->hWin, i);
        BUTTON_SetText(hItem, p->iText);
        p++;
    }
    
//Code added by IMM - Sets the text of the shift button to
//                    said shift button. Holds the shift button
//                    down when using upper case charachters.
//*******************************************************
    hItem= WM_GetDialogItem(pMsg->hWin, ID_SHIFT_BUTTON);
    BUTTON_SetText(hItem, p->iText);
    
    if ( aMapping == MAPPING_UPPER){
        BUTTON_SetPressed(hItem, 1);//Hold down the shift button
    } else if (aMapping == MAPPING_LOWER){
        BUTTON_SetPressed(hItem, 0);//Release the shift button
    }
    
    p++;
//********************************************************    
    hItem= WM_GetDialogItem(pMsg->hWin, ID_SPACE_BUTTON);
    BUTTON_SetText(hItem, p->iText);
}

/*---------------------------------------------------------------------------*
 * Routine:	_EnterNameScreen.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _EnterNameScreenDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;
    WM_HWIN hItem;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, EnterNameScreenMapping);
        ISetButtonIcons(pMsg);
        ISetKeyboardMapping(pMsg, MAPPING_UPPER);
        G_Win = pMsg->hWin;

        hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_BOX);
        EDIT_EnableBlink(hItem, 500, 1);
        EDIT_SetMaxLen(hItem, 15);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(EnterNameScreenMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
	case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_POST_PAINT:
        break;
    case WM_APP_GAINED_FOCUS:
#if MAGELLAN_DEBUG
        printf("Enter Name Active\n");
#endif
    	G_Active = ETrue;
    	break;
    case WM_APP_LOST_FOCUS:
#if MAGELLAN_DEBUG
        printf("Enter Name Inactive\n");
#endif
    	G_Active = EFalse;
    	break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:	EnterNameScreen_Create
 *---------------------------------------------------------------------------*/
/** Create the EnterNameScreen. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
TUInt32 EnterNameScreen(char *aName, TUInt8 aBowler)
{
    TUInt32 result;

#if MAGELLAN_DEBUG
    printf("Edit Name Opened\n");
#endif

    G_BowlerNumber = aBowler;
    G_NumChars = 0;
    memset((void*)&G_Name, 0, 10);
    result = GUI_ExecDialogBox(_iEnterNameScreenDialog, GUI_COUNTOF(_iEnterNameScreenDialog), &_EnterNameScreenDialog, 0,0,0);
    if(result){
        strcpy(aName, (char*)&G_Name);
    }
    G_Win = 0;
    return result;
}

void EnterNameScreenClose()
{
    if( G_Active){
#if MAGELLAN_DEBUG
        printf("Edit Name Closed Externally\n");
#endif
        GUI_EndDialog(G_Win, 0);
    }
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    EnterNameScreen.c
 *-------------------------------------------------------------------------*/
