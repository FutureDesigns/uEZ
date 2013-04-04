/*-------------------------------------------------------------------------*
 * File:    Keyboard.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Generic Keyboard for emWin.
 *-------------------------------------------------------------------------*/
#include "GUI.h"
#include "BUTTON.h"
#include "DIALOG.h"
#include "WM.h"
#include "GUI_FontIntern.h"
#include "EDIT.h"
#include <uEZ.h>
#include "string.h"
#include "LookAndFeel.h"
#include "Keyboard.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
//IDs used by emWin to identify objects
#define ID_WINDOW_0        (GUI_ID_USER + 0x00)
#define ID_BUTTON_Q        (GUI_ID_USER + 0x01)
#define ID_BUTTON_W        (GUI_ID_USER + 0x02)
#define ID_BUTTON_E        (GUI_ID_USER + 0x03)
#define ID_BUTTON_R        (GUI_ID_USER + 0x04)
#define ID_BUTTON_T        (GUI_ID_USER + 0x05)
#define ID_BUTTON_Y        (GUI_ID_USER + 0x06)
#define ID_BUTTON_U        (GUI_ID_USER + 0x07)
#define ID_BUTTON_I        (GUI_ID_USER + 0x08)
#define ID_BUTTON_O        (GUI_ID_USER + 0x09)
#define ID_BUTTON_P        (GUI_ID_USER + 0x0a)
#define ID_BUTTON_A        (GUI_ID_USER + 0x0B)
#define ID_BUTTON_S        (GUI_ID_USER + 0x0C)
#define ID_BUTTON_D        (GUI_ID_USER + 0x0D)
#define ID_BUTTON_F        (GUI_ID_USER + 0x0E)
#define ID_BUTTON_G        (GUI_ID_USER + 0x0F)
#define ID_BUTTON_H        (GUI_ID_USER + 0x10)
#define ID_BUTTON_J        (GUI_ID_USER + 0x11)
#define ID_BUTTON_K        (GUI_ID_USER + 0x12)
#define ID_BUTTON_L        (GUI_ID_USER + 0x13)
#define ID_BUTTON_SHIFT    (GUI_ID_USER + 0x14)
#define ID_BUTTON_Z        (GUI_ID_USER + 0x15)
#define ID_BUTTON_X        (GUI_ID_USER + 0x16)
#define ID_BUTTON_C        (GUI_ID_USER + 0x17)
#define ID_BUTTON_V        (GUI_ID_USER + 0x18)
#define ID_BUTTON_B        (GUI_ID_USER + 0x19)
#define ID_BUTTON_N        (GUI_ID_USER + 0x1A)
#define ID_BUTTON_M        (GUI_ID_USER + 0x1B)
#define ID_BUTTON_BACK     (GUI_ID_USER + 0x1C)
#define ID_BUTTON_SYMBOL   (GUI_ID_USER + 0x1D)
#define ID_BUTTON_COMMA    (GUI_ID_USER + 0x1E)
#define ID_BUTTON_SPACE    (GUI_ID_USER + 0x1F)
#define ID_BUTTON_PERIOD   (GUI_ID_USER + 0x20)
#define ID_BUTTON_ENTER    (GUI_ID_USER + 0x21)
#define ID_TEXT_BOX        (GUI_ID_USER + 0x22)
#define ID_BUTTON_CANCEL   (GUI_ID_USER + 0x23)
#define ID_MESSAGE         (GUI_ID_USER + 0x24)

//Max number of Characters allowed to be entered
#define MAX_NUMBER_OF_CHARS 85

//Max number of buttons in a row
#define KEYBOARD_BUTTON_PER_ROW       10

#if (UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING      5
#else
#define SPACING      3     //WQVGA
#endif

//Button sizes based on screen size
#define KEYBOARD_BUTTON_WIDTH         ((UEZ_LCD_DISPLAY_WIDTH-(KEYBOARD_BUTTON_PER_ROW * (SPACING + 1)))/KEYBOARD_BUTTON_PER_ROW)
#define KEYBOARD_BUTTON_HEIGHT        (KEYBOARD_BUTTON_WIDTH * 52)/70  //made up ration may need adjusting

//Text filed height
#define KEYBOARD_G_textBox_HEIGHT       (KEYBOARD_BUTTON_HEIGHT)

#define KEYBOARD_X_POS                (0)
#define KEYBOARD_Y_POS                (UEZ_LCD_DISPLAY_HEIGHT - ((4 * KEYBOARD_BUTTON_HEIGHT) + (5 * SPACING) + KEYBOARD_G_textBox_HEIGHT))
#define KEYBOARD_X_SIZE               (UEZ_LCD_DISPLAY_WIDTH)
#define KEYBOARD_Y_SIZE               (4 * KEYBOARD_BUTTON_HEIGHT) + (5 * SPACING + KEYBOARD_G_textBox_HEIGHT)

//Default colors for keyboard
#define KEYBOARD_BACKGROUND_COLOR   GUI_WHITE
#define KEY_BACKGROUND_COLOR        GUI_GRAY

//Default font for text
#if (UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define KEYBOARD_DEFAULT_FONT GUI_Font24_ASCII
#else
#define KEYBOARD_DEFAULT_FONT GUI_Font13_ASCII
#endif
       
/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IKeyboardHandleShift(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IKeyboardHandleBack(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IKeyboardHandleEnter(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IKeyboardHandleCancel(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IKeyboardHandleSymbol(WM_MESSAGE * pMsg, int aNCode, int aID);
void KeyboardClose(void);

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static TBool G_enteringData;
static char G_textBox[MAX_NUMBER_OF_CHARS+1];
static TUInt8 G_textBoxIndex;
static TBool G_isActive = EFalse;
static char* G_message;
static WM_HWIN G_window;
static TUInt8 G_showKeys;
static TUInt16 G_maxKeys;

//Dialog structure for emWin
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {      //XP                                                                                                          YP                       XS                                                                                                          YS
  { WINDOW_CreateIndirect   , "Keyboard"    , ID_WINDOW_0       , KEYBOARD_X_POS                                                                                            , KEYBOARD_Y_POS - 20   , KEYBOARD_X_SIZE                                                       , KEYBOARD_Y_SIZE + 20      , 0, 0, 0},
  { TEXT_CreateIndirect     , ""            , ID_MESSAGE        , KEYBOARD_X_POS                                                                                            , 0                     , KEYBOARD_X_SIZE                                                       , 20                        , 0, 0, 0},
  { EDIT_CreateIndirect     , ""            , ID_TEXT_BOX       , KEYBOARD_X_POS + (SPACING/2)                                                                              , 20                    , UEZ_LCD_DISPLAY_WIDTH - SPACING - (KEYBOARD_BUTTON_WIDTH*2) - (SPACING/2) , KEYBOARD_G_textBox_HEIGHT , 0, 0, 0},
  { BUTTON_CreateIndirect   , "Cancel"      , ID_BUTTON_CANCEL  , KEYBOARD_X_POS + (SPACING/2) + (UEZ_LCD_DISPLAY_WIDTH - SPACING - (KEYBOARD_BUTTON_WIDTH*2) - (SPACING))    , 20                    , (KEYBOARD_BUTTON_WIDTH* 2)                                   , KEYBOARD_G_textBox_HEIGHT , 0, 0, 0},
//Row 1                                      XP                                                                               YP                                                                                            XS                     YS
  { BUTTON_CreateIndirect, "Q", ID_BUTTON_Q, KEYBOARD_X_POS + (SPACING * 1)  + (KEYBOARD_BUTTON_WIDTH * 0) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "W", ID_BUTTON_W, KEYBOARD_X_POS + (SPACING * 2)  + (KEYBOARD_BUTTON_WIDTH * 1) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "E", ID_BUTTON_E, KEYBOARD_X_POS + (SPACING * 3)  + (KEYBOARD_BUTTON_WIDTH * 2) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "R", ID_BUTTON_R, KEYBOARD_X_POS + (SPACING * 4)  + (KEYBOARD_BUTTON_WIDTH * 3) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "T", ID_BUTTON_T, KEYBOARD_X_POS + (SPACING * 5)  + (KEYBOARD_BUTTON_WIDTH * 4) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "Y", ID_BUTTON_Y, KEYBOARD_X_POS + (SPACING * 6)  + (KEYBOARD_BUTTON_WIDTH * 5) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "U", ID_BUTTON_U, KEYBOARD_X_POS + (SPACING * 7)  + (KEYBOARD_BUTTON_WIDTH * 6) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "I", ID_BUTTON_I, KEYBOARD_X_POS + (SPACING * 8)  + (KEYBOARD_BUTTON_WIDTH * 7) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "O", ID_BUTTON_O, KEYBOARD_X_POS + (SPACING * 9)  + (KEYBOARD_BUTTON_WIDTH * 8) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "P", ID_BUTTON_P, KEYBOARD_X_POS + (SPACING * 10) + (KEYBOARD_BUTTON_WIDTH * 9) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 1) + (KEYBOARD_BUTTON_HEIGHT * 0)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
//Row 2                                      XP                                                                                                                                       YP                                                                                            XS                     YS
  { BUTTON_CreateIndirect, "A", ID_BUTTON_A, KEYBOARD_X_POS + (SPACING * 1)  + (KEYBOARD_BUTTON_WIDTH * 0) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "S", ID_BUTTON_S, KEYBOARD_X_POS + (SPACING * 2)  + (KEYBOARD_BUTTON_WIDTH * 1) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "D", ID_BUTTON_D, KEYBOARD_X_POS + (SPACING * 3)  + (KEYBOARD_BUTTON_WIDTH * 2) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "F", ID_BUTTON_F, KEYBOARD_X_POS + (SPACING * 4)  + (KEYBOARD_BUTTON_WIDTH * 3) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "G", ID_BUTTON_G, KEYBOARD_X_POS + (SPACING * 5)  + (KEYBOARD_BUTTON_WIDTH * 4) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "H", ID_BUTTON_H, KEYBOARD_X_POS + (SPACING * 6)  + (KEYBOARD_BUTTON_WIDTH * 5) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "J", ID_BUTTON_J, KEYBOARD_X_POS + (SPACING * 7)  + (KEYBOARD_BUTTON_WIDTH * 6) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "K", ID_BUTTON_K, KEYBOARD_X_POS + (SPACING * 8)  + (KEYBOARD_BUTTON_WIDTH * 7) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "L", ID_BUTTON_L, KEYBOARD_X_POS + (SPACING * 9)  + (KEYBOARD_BUTTON_WIDTH * 8) + (SPACING + (KEYBOARD_BUTTON_WIDTH/2)), KEYBOARD_G_textBox_HEIGHT + (SPACING * 2) + (KEYBOARD_BUTTON_HEIGHT * 1)+20, KEYBOARD_BUTTON_WIDTH, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
//Row 3                                                     XP                                                                                                                                       YP                                                                                            XS                     YS
  { BUTTON_CreateIndirect, "Shift"  , ID_BUTTON_SHIFT   , KEYBOARD_X_POS + (SPACING * 1)  + (KEYBOARD_BUTTON_WIDTH * 0)                                                            , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH+ (KEYBOARD_BUTTON_WIDTH/2) + SPACING, KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "Z"      , ID_BUTTON_Z       , KEYBOARD_X_POS + (SPACING * 2)  + (KEYBOARD_BUTTON_WIDTH * 1) +  (KEYBOARD_BUTTON_WIDTH/2) + SPACING    , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH                                                      , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "X"      , ID_BUTTON_X       , KEYBOARD_X_POS + (SPACING * 3)  + (KEYBOARD_BUTTON_WIDTH * 2) + (KEYBOARD_BUTTON_WIDTH/2) + SPACING     , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH                                                      , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "C"      , ID_BUTTON_C       , KEYBOARD_X_POS + (SPACING * 4)  + (KEYBOARD_BUTTON_WIDTH * 3) + (KEYBOARD_BUTTON_WIDTH/2) + SPACING     , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH                                                      , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "V"      , ID_BUTTON_V       , KEYBOARD_X_POS + (SPACING * 5)  + (KEYBOARD_BUTTON_WIDTH * 4) + (KEYBOARD_BUTTON_WIDTH/2) + SPACING     , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH                                                      , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "B"      , ID_BUTTON_B       , KEYBOARD_X_POS + (SPACING * 6)  + (KEYBOARD_BUTTON_WIDTH * 5) + (KEYBOARD_BUTTON_WIDTH/2) + SPACING     , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH                                                      , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "N"      , ID_BUTTON_N       , KEYBOARD_X_POS + (SPACING * 7)  + (KEYBOARD_BUTTON_WIDTH * 6) + (KEYBOARD_BUTTON_WIDTH/2) + SPACING     , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH                                                      , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "M"      , ID_BUTTON_M       , KEYBOARD_X_POS + (SPACING * 8)  + (KEYBOARD_BUTTON_WIDTH * 7) + (KEYBOARD_BUTTON_WIDTH/2) + SPACING     , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH                                                      , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
  { BUTTON_CreateIndirect, "BACK"   , ID_BUTTON_BACK    , KEYBOARD_X_POS + (SPACING * 9)  + (KEYBOARD_BUTTON_WIDTH * 8) + (KEYBOARD_BUTTON_WIDTH/2) + SPACING     , KEYBOARD_G_textBox_HEIGHT + (SPACING * 3) + (KEYBOARD_BUTTON_HEIGHT * 2)+20, KEYBOARD_BUTTON_WIDTH + (KEYBOARD_BUTTON_WIDTH/2)                          , KEYBOARD_BUTTON_HEIGHT, 0, 0, 0 },
//Row 4                                                     XP                                                                             YP                                                                                         XS                                                              YS
  { BUTTON_CreateIndirect, "Symbol", ID_BUTTON_SYMBOL   , KEYBOARD_X_POS + (SPACING * 1)  + (KEYBOARD_BUTTON_WIDTH * 0) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 4) + (KEYBOARD_BUTTON_HEIGHT * 3)+20, (2 * KEYBOARD_BUTTON_WIDTH) + SPACING        , KEYBOARD_BUTTON_HEIGHT     , 0, 0, 0 },
  { BUTTON_CreateIndirect, ",", ID_BUTTON_COMMA         , KEYBOARD_X_POS + (SPACING * 3)  + (KEYBOARD_BUTTON_WIDTH * 2) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 4) + (KEYBOARD_BUTTON_HEIGHT * 3)+20, KEYBOARD_BUTTON_WIDTH                                         , KEYBOARD_BUTTON_HEIGHT     , 0, 0, 0 },
  { BUTTON_CreateIndirect, " ", ID_BUTTON_SPACE         , KEYBOARD_X_POS + (SPACING * 4)  + (KEYBOARD_BUTTON_WIDTH * 3) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 4) + (KEYBOARD_BUTTON_HEIGHT * 3)+20, (4 * KEYBOARD_BUTTON_WIDTH) + (3 * SPACING)  , KEYBOARD_BUTTON_HEIGHT     , 0, 0, 0 },
  { BUTTON_CreateIndirect, ".", ID_BUTTON_PERIOD        , KEYBOARD_X_POS + (SPACING * 8)  + (KEYBOARD_BUTTON_WIDTH * 7) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 4) + (KEYBOARD_BUTTON_HEIGHT * 3)+20, KEYBOARD_BUTTON_WIDTH                                         , KEYBOARD_BUTTON_HEIGHT     , 0, 0, 0 },
  { BUTTON_CreateIndirect, "ENTER", ID_BUTTON_ENTER     , KEYBOARD_X_POS + (SPACING * 9)  + (KEYBOARD_BUTTON_WIDTH * 8) , KEYBOARD_G_textBox_HEIGHT + (SPACING * 4) + (KEYBOARD_BUTTON_HEIGHT * 3)+20, (2 * KEYBOARD_BUTTON_WIDTH) + SPACING        , KEYBOARD_BUTTON_HEIGHT     , 0, 0, 0 },
};

//Mapping for keys on the keyboard
typedef struct {
  int iID;
  const int iChar;
  const char *iTextOnButton;
  const TUInt8 iType;
    TBool (*iCommand)(WM_MESSAGE * pMsg, int aID, int aNCode);
} T_keyboardMapping;

//Uppercase mapping for keys
static T_keyboardMapping G_keyboardMappingUppercase[] = {
  { ID_BUTTON_Q, 'Q', "Q", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_W, 'W', "W", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_E, 'E', "E", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_R, 'R', "R", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_T, 'T', "T", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_Y, 'Y', "Y", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_U, 'U', "U", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_I, 'I', "I", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_O, 'O', "O", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_P, 'P', "P", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_A, 'A', "A", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_S, 'S', "S", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_D, 'D', "D", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_F, 'F', "F", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_G, 'G', "G", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_H, 'H', "H", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_J, 'J', "J", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_K, 'K', "K", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_L, 'L', "L", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_Z, 'Z', "Z", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_X, 'X', "X", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_C, 'C', "C", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_V, 'V', "V", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_B, 'B', "B", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_N, 'N', "N", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_M, 'M', "M", KEYBOARD_USE_UPPER_CASE, 0},
  { ID_BUTTON_SHIFT, 0, "Shift", 0, IKeyboardHandleShift },
  { ID_BUTTON_BACK, 0, "BACK", 0, IKeyboardHandleBack },
  { ID_BUTTON_COMMA, ',', ",", KEYBOARD_USE_SYMBOLS, 0 },
  { ID_BUTTON_SPACE, ' ', " ", KEYBOARD_USE_SPACE, 0 },
  { ID_BUTTON_PERIOD, '.', ".", KEYBOARD_USE_SYMBOLS, 0 },
  { ID_BUTTON_ENTER, 0, "ENTER", 0, IKeyboardHandleEnter },
  { ID_BUTTON_SYMBOL, 0, "Symbol", 0, IKeyboardHandleSymbol },
  { ID_BUTTON_CANCEL, 0, "Cancel", 0, IKeyboardHandleCancel },
  { 0 },
};

//Lower case mapping for keys.
static T_keyboardMapping G_keyboardMappingLowercase[] = {
  { ID_BUTTON_Q, 'q', "q", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_W, 'w', "w", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_E, 'e', "e", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_R, 'r', "r", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_T, 't', "t", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_Y, 'y', "y", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_U, 'u', "u", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_I, 'i', "i", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_O, 'o', "o", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_P, 'p', "p", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_A, 'a', "a", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_S, 's', "s", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_D, 'd', "d", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_F, 'f', "f", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_G, 'g', "g", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_H, 'h', "h", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_J, 'j', "j", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_K, 'k', "k", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_L, 'l', "l", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_Z, 'z', "z", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_X, 'x', "x", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_C, 'c', "c", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_V, 'v', "v", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_B, 'b', "b", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_N, 'n', "n", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_M, 'm', "m", KEYBOARD_USE_LOWER_CASE, 0},
  { ID_BUTTON_SHIFT, 0, "Shift", 0, IKeyboardHandleShift },
  { ID_BUTTON_BACK, 0, "BACK", 0, IKeyboardHandleBack },
  { ID_BUTTON_COMMA, ',', ",", KEYBOARD_USE_SYMBOLS, 0 },
  { ID_BUTTON_SPACE, ' ', " ", KEYBOARD_USE_SPACE, 0 },
  { ID_BUTTON_PERIOD, '.', ".", KEYBOARD_USE_SYMBOLS, 0 },
  { ID_BUTTON_ENTER, 0, "ENTER", 0, IKeyboardHandleEnter },
  { ID_BUTTON_SYMBOL, 0, "Symbol", 0, IKeyboardHandleSymbol },
  { ID_BUTTON_CANCEL, 0, "Cancel", 0, IKeyboardHandleCancel },
  { 0 },
};

//Number and symbol mapping for keyboard.
static T_keyboardMapping G_keyboardMappingSymbol[] = {
  { ID_BUTTON_Q, '1', "1", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_W, '2', "2", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_E, '3', "3", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_R, '4', "4", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_T, '5', "5", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_Y, '6', "6", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_U, '7', "7", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_I, '8', "8", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_O, '9', "9", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_P, '0', "0", KEYBOARD_USE_DIGITS, 0},
  { ID_BUTTON_A, '!', "!", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_S, '@', "@", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_D, '#', "#", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_F, '$', "$", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_G, '%', "%", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_H, '&', "&", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_J, '*', "*", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_K, '?', "?", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_L, '/', "/", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_Z, ':', ":", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_X, '_', "_", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_C, '"', "\"", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_V, '(', "(", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_B, ')', ")", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_N, '-', "-", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_M, '+', "+", KEYBOARD_USE_SYMBOLS, 0},
  { ID_BUTTON_SHIFT, ';', ";", KEYBOARD_USE_SYMBOLS, 0 },
  { ID_BUTTON_BACK, 0, "BACK", 0, IKeyboardHandleBack },
  { ID_BUTTON_COMMA, ',', ",", KEYBOARD_USE_SYMBOLS, 0 },
  { ID_BUTTON_SPACE, ' ', " ", KEYBOARD_USE_SPACE, 0 },
  { ID_BUTTON_PERIOD, '.', ".", KEYBOARD_USE_SYMBOLS, 0 },
  { ID_BUTTON_ENTER, 0, "ENTER", 0, IKeyboardHandleEnter },
  { ID_BUTTON_SYMBOL, 0, "Letters", 0, IKeyboardHandleSymbol },
  { ID_BUTTON_CANCEL, 0, "Cancel", 0, IKeyboardHandleCancel },
  { 0 },
};

//default mapping is upper case
static T_keyboardMapping *G_keypadCurrentMapping = G_keyboardMappingUppercase;

/*---------------------------------------------------------------------------*
 * Routine:     setNewButtonText
 *---------------------------------------------------------------------------*
 * Description:
 *      Applies the current mapping to the keys.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event received to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static void setNewButtonText(WM_MESSAGE * pMsg, int aNCode, int aID)
{
     T_keyboardMapping *p = G_keypadCurrentMapping;
     WM_HWIN hitem;
         
     while(p->iID){
        hitem = WM_GetDialogItem(pMsg->hWin, p->iID);
        BUTTON_SetText(hitem, p->iTextOnButton);
        if ((p->iType == 0) || (p->iType & G_showKeys)) {
            WM_EnableWindow(hitem);
        } else {
            WM_DisableWindow(hitem);
        }
        p++;
     }
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeyboardHandleShift
 *---------------------------------------------------------------------------*
 * Description:
 *      Toggles the character map from upper to lower case letters.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event received to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeyboardHandleShift(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if(G_keypadCurrentMapping == G_keyboardMappingLowercase)
        G_keypadCurrentMapping = G_keyboardMappingUppercase;
    else
        G_keypadCurrentMapping = G_keyboardMappingLowercase;
    
    setNewButtonText(pMsg,aNCode, aID);
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeyboardHandleSymbol
 *---------------------------------------------------------------------------*
 * Description:
 *      Toggles the character map from letters to symbols.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event received to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeyboardHandleSymbol(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (G_keypadCurrentMapping == G_keyboardMappingSymbol)
        G_keypadCurrentMapping = G_keyboardMappingUppercase;
    else
        G_keypadCurrentMapping = G_keyboardMappingSymbol;
    setNewButtonText(pMsg,aNCode, aID);
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeyboardHandleBack
 *---------------------------------------------------------------------------*
 * Description:
 *      Handles the backspace key, erase the last letter in the list.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event recived to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeyboardHandleBack(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if(G_textBoxIndex){
        G_textBoxIndex--;
        G_textBox[G_textBoxIndex] = '\0';
        EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_BOX), G_textBox); 
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeyboardHandleEnter
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit the dialog passing the current string back.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event recived to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeyboardHandleEnter(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    G_enteringData = EFalse;
    G_textBox[G_textBoxIndex+1] = '\0';
    GUI_EndDialog(pMsg->hWin, 1);
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeyboardHandleCancel
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit the dialog and do not pass current string back.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event recived to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeyboardHandleCancel(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    KeyboardClose();
    return ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:     IKeyboardHandleEvent
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic event handler.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *      int aNCode -- Type of event recived to process.
 *      int aID    -- not used.
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
static TBool IKeyboardHandleEvent(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    TBool handled = EFalse;
    T_keyboardMapping *p = G_keypadCurrentMapping;
    WM_HWIN hItem;
    
    /* Find the code in the table */
    while (p->iID) {
        /* Is this the ID we want? */
        if (p->iID == aID) {
            if (aNCode == WM_NOTIFICATION_CLICKED) {
            } else if (aNCode == WM_NOTIFICATION_RELEASED){
                /* If this button has a special command, go process it */
                if (p->iCommand) {
                    /* Call the command associated with this ID */
                    handled = (p->iCommand)(pMsg, aNCode, aID);
                } else {
                    /* No command, do the standard code */
                    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BOX);
                    if (G_textBoxIndex < G_maxKeys){
                        G_textBox[G_textBoxIndex] = p->iChar;
                        G_textBox[G_textBoxIndex + 1] = '\0';
                        G_textBoxIndex++;
                        EDIT_SetText(hItem, G_textBox);
                    }
                }
            }
            break;
        }
        p++;
    }  
    return handled;
}
      
/*---------------------------------------------------------------------------*
 * Routine:     _cbDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events for the active window.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    int Id, NCode;
    T_keyboardMapping *p = G_keypadCurrentMapping;

    switch (pMsg->MsgId) {
        case WM_INIT_DIALOG:
            //
            // Initialization of 'Window'
            //
            hItem = pMsg->hWin;
            G_window = hItem;
            WINDOW_SetBkColor(hItem, KEYBOARD_BACKGROUND_COLOR);
            //BUTTON_SetDefaultBkColor(KEY_BACKGROUND_COLOR, BUTTON_CI_UNPRESSED);
            EDIT_SetDefaultFont(&KEYBOARD_DEFAULT_FONT);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_BOX);
            EDIT_SetMaxLen(hItem, MAX_NUMBER_OF_CHARS);
            EDIT_SetText(hItem, G_textBox);
            EDIT_SetFont(hItem, &KEYBOARD_DEFAULT_FONT);
            G_textBoxIndex = EDIT_GetNumChars(hItem);
            hItem = WM_GetDialogItem(pMsg->hWin, ID_MESSAGE);
            TEXT_SetFont(hItem, &KEYBOARD_DEFAULT_FONT);
            TEXT_SetTextColor(hItem, GUI_BLACK);
            TEXT_SetBkColor(hItem, KEYBOARD_BACKGROUND_COLOR);
            TEXT_SetText(hItem, G_message);
            TEXT_SetTextAlign(hItem, GUI_TA_TOP);
            while (p->iID) {
                hItem = WM_GetDialogItem(pMsg->hWin, p->iID);
                BUTTON_SetFont(hItem, &KEYBOARD_DEFAULT_FONT);
                BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLACK);
                BUTTON_SetTextColor(hItem, BUTTON_CI_PRESSED, GUI_BLACK);
                BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED,
                    KEY_BACKGROUND_COLOR);
                BUTTON_SetBkColor(hItem, BUTTON_CI_PRESSED, GUI_LIGHTGRAY);
                //BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
                p++;
            }
            WM_MakeModal(pMsg->hWin);
            setNewButtonText(pMsg, 0, 0);
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            if (!IKeyboardHandleEvent(pMsg, NCode, Id)) {
                // Special cases go here
                // Nothing happens otherwise
            }
        case WM_POST_PAINT:
            // Make sure the all presses up til now are gone
            TouchscreenClearPresses();
            break;
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:     KeyboardClose
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the dialog.
 *---------------------------------------------------------------------------*/
void KeyboardClose(void)
{
    if (G_isActive) {
        G_enteringData = EFalse;
        G_textBox[0] = '\0';
        G_textBoxIndex = 0;
        GUI_EndDialog(G_window, 0);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  keyboard
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the keyboard dialog and stay here until closed.
 * Inputs:
 *      char* aData -- string to hold current value and to pass back new value.
 *      char* message -- String to tell the type of data being entered.
 *      TUInt16 aNumChars -- number of expected characters to return.
 *      TUInt8 aShowKeys -- integer to disable keys, used for serial number.
 * Outputs:
 *    TUInt16 -- return code of how dialog closes.
 *---------------------------------------------------------------------------*/
TUInt16 Keyboard(char* aData, char* message, TUInt16 aNumChars, TUInt8 aShowKeys)
{
	TUInt8 returnCode;
	
    G_enteringData = ETrue;

    G_showKeys = aShowKeys;
    G_maxKeys = aNumChars;

    if (!G_isActive) {
        G_isActive = ETrue;
        G_keypadCurrentMapping = G_keyboardMappingUppercase;
        memcpy((void*)G_textBox, (void*)aData, MAX_NUMBER_OF_CHARS);
        G_message = message;
        while (G_enteringData) {
            returnCode = GUI_ExecDialogBox(_aDialogCreate,
                GUI_COUNTOF(_aDialogCreate), &_cbDialog, 0, 0, 0);
        }
        G_isActive = EFalse;

        if (returnCode) {
            memcpy((void*)aData, (void*)G_textBox, G_textBoxIndex + 1);
            return G_textBoxIndex;
        }
    }

    return 0; //Text entry canceled
}

