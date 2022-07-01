/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  NXP Semiconductors USA, Inc.  whose
registered  office  is  situated  at 411 E. Plumeria Drive, San  Jose,
CA 95134, USA  solely for  the  purposes  of  creating  libraries  for
NXPs M0, M3/M4 and  ARM7/9 processor-based  devices,  sublicensed  and
distributed under the terms and conditions of the NXP End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              NXP Semiconductors, 1109 McKay Dr, M/S 76, San Jose, CA 95131, USA
Licensed SEGGER software: emWin
License number:           GUI-00186
License model:            emWin License Agreement, dated August 20th 2011 and Amendment, dated October 19th 2017
Licensed platform:        NXP's ARM 7/9, Cortex-M0, M3, M4, M7, A7
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2011-08-19 - 2018-09-02
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : GUI_Private.h
Purpose     : GUI internal declarations
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_PRIVATE_H
#define GUI_PRIVATE_H

#include "GUI.h"
#include "LCD_Protected.h"
#include "GUI_Debug.h"
#if GUI_WINSUPPORT
  #include "WM_GUI.h"
#endif

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defaults for config switches
*
**********************************************************************

  The config switches below do not affect the interface in GUI.h and
  are therefor not required to be in GUI.h.
*/

/* Short address area.
   For  most compilers, this is "near" or "__near"
   We do not use this except for some CPUs which we know to always have some
   near memory, because the GUI_Context and some other data will be declared
   to be in this short address (near) memory area as it has a major effect
   on performance.
   Please define in GUIConf.h (if you want to use it)
*/
#ifndef GUI_SADDR
  #define GUI_SADDR
#endif

#ifndef GUI_DEFAULT_FONT
  #define GUI_DEFAULT_FONT    &GUI_Font6x8
#endif

#ifndef GUI_DEFAULT_CURSOR
  #define GUI_DEFAULT_CURSOR  &GUI_CursorArrowM
#endif

#ifndef GUI_DEFAULT_BKCOLOR
  #define GUI_DEFAULT_BKCOLOR GUI_BLACK
#endif

#ifndef GUI_DEFAULT_COLOR
  #define GUI_DEFAULT_COLOR   GUI_WHITE
#endif

/*********************************************************************
*
*       Angles
*
**********************************************************************
*/
#define GUI_45DEG  512
#define GUI_90DEG  (2 * GUI_45DEG)
#define GUI_180DEG (4 * GUI_45DEG)
#define GUI_360DEG (8 * GUI_45DEG)

/*********************************************************************
*
*       Locking checks
*
**********************************************************************
*/
#if defined (WIN32) && defined (_DEBUG) && GUI_OS
  #define GUI_ASSERT_LOCK()    GUITASK_AssertLock()
  #define GUI_ASSERT_NO_LOCK() GUITASK_AssertNoLock()
  void GUITASK_AssertLock(void);
  void GUITASK_AssertNoLock(void);
#else
  #define GUI_ASSERT_LOCK()
  #define GUI_ASSERT_NO_LOCK()
#endif

/*********************************************************************
*
*       Division tables
*
**********************************************************************
*/
extern const U8 GUI__aConvert_15_255[(1 << 4)];
extern const U8 GUI__aConvert_31_255[(1 << 5)];
extern const U8 GUI__aConvert_63_255[(1 << 6)];
extern const U8 GUI__aConvert_255_15[(1 << 8)];
extern const U8 GUI__aConvert_255_31[(1 << 8)];
extern const U8 GUI__aConvert_255_63[(1 << 8)];

/*********************************************************************
*
*       Usage internals
*
**********************************************************************
*/
typedef GUI_HMEM GUI_USAGE_Handle;
typedef struct tsUSAGE_APIList tUSAGE_APIList;
typedef struct GUI_Usage GUI_USAGE;
#define GUI_USAGE_h GUI_USAGE_Handle

typedef GUI_USAGE_h tUSAGE_CreateCompatible(GUI_USAGE * p);
typedef void        tUSAGE_AddPixel        (GUI_USAGE * p, int32_t x, int32_t y);
typedef void        tUSAGE_AddHLine        (GUI_USAGE * p, int32_t x0, int32_t y0, int32_t len);
typedef void        tUSAGE_Clear           (GUI_USAGE * p);
typedef void        tUSAGE_Delete          (GUI_USAGE_h h);
typedef int32_t         tUSAGE_GetNextDirty    (GUI_USAGE * p, int32_t * pxOff, int32_t yOff);
#define GUI_USAGE_LOCK_H(h) ((GUI_USAGE *)GUI_LOCK_H(h))

void GUI_USAGE_DecUseCnt(GUI_USAGE_Handle  hUsage);

GUI_USAGE_Handle GUI_USAGE_BM_Create(int32_t x0, int32_t y0, int32_t xsize, int32_t ysize, int32_t Flags);
void    GUI_USAGE_Select(GUI_USAGE_Handle hUsage);
void    GUI_USAGE_AddRect(GUI_USAGE * pUsage, int32_t x0, int32_t y0, int32_t xSize, int32_t ySize);
#define GUI_USAGE_AddPixel(p, x,y)            p->pAPI->pfAddPixel(p,x,y)
#define GUI_USAGE_AddHLine(p,x,y,len)         p->pAPI->pfAddHLine(p,x,y,len)
#define GUI_USAGE_Clear(p)                    p->pAPI->pfClear(p)
#define GUI_USAGE_Delete(p)                   p->pAPI->pfDelete(p)
#define GUI_USAGE_GetNextDirty(p,pxOff, yOff) p->pAPI->pfGetNextDirty(p,pxOff, yOff)

struct tsUSAGE_APIList {
  tUSAGE_AddPixel         * pfAddPixel;
  tUSAGE_AddHLine         * pfAddHLine;
  tUSAGE_Clear            * pfClear;
  tUSAGE_CreateCompatible * pfCreateCompatible;
  tUSAGE_Delete           * pfDelete;
  tUSAGE_GetNextDirty     * pfGetNextDirty;
} ;

struct GUI_Usage {
  I16P x0, y0, XSize, YSize;
  const tUSAGE_APIList * pAPI;
  I16 UseCnt;
};

/*********************************************************************
*
*       GUI_MEMDEV
*
**********************************************************************
*/
#if GUI_SUPPORT_MEMDEV

typedef struct {
  GUI_DEVICE * pDevice;
  I16P                   x0, y0, XSize, YSize;
  unsigned               BytesPerLine;
  unsigned               BitsPerPixel;
  GUI_HMEM               hUsage;
} GUI_MEMDEV;

#define      GUI_MEMDEV_LOCK_H(h) ((GUI_MEMDEV *)GUI_LOCK_H(h))

void         GUI_MEMDEV__CopyFromLCD (GUI_MEMDEV_Handle hMem);
void         GUI_MEMDEV__GetRect     (GUI_RECT * pRect);
unsigned     GUI_MEMDEV__Color2Index (LCD_COLOR Color);
LCD_COLOR    GUI_MEMDEV__Index2Color (int32_t Index);
uint32_t GUI_MEMDEV__GetIndexMask(void);
void         GUI_MEMDEV__SetAlphaCallback(unsigned(* pcbSetAlpha)(U8));

GUI_MEMDEV_Handle GUI_MEMDEV__CreateFixed(int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, int32_t Flags,
                                          const GUI_DEVICE_API     * pDeviceAPI,
                                          const LCD_API_COLOR_CONV * pColorConvAPI);

void              GUI_MEMDEV__DrawSizedAt        (GUI_MEMDEV_Handle hMem, int32_t xPos, int32_t yPos, int32_t xSize, int32_t ySize);
GUI_MEMDEV_Handle GUI_MEMDEV__GetEmptyCopy32     (GUI_MEMDEV_Handle hMem, int32_t * pxSize, int32_t * pySize, int32_t * pxPos, int32_t * pyPos);
void              GUI_MEMDEV__ReadLine           (int32_t x0, int32_t y, int32_t x1, LCD_PIXELINDEX * pBuffer);
void              GUI_MEMDEV__WriteToActiveAlpha (GUI_MEMDEV_Handle hMem,int32_t x, int32_t y);
void              GUI_MEMDEV__WriteToActiveAt    (GUI_MEMDEV_Handle hMem,int32_t x, int32_t y);
void              GUI_MEMDEV__WriteToActiveOpaque(GUI_MEMDEV_Handle hMem,int32_t x, int32_t y);
void            * GUI_MEMDEV__XY2PTR             (int32_t x,int32_t y);
void            * GUI_MEMDEV__XY2PTREx           (GUI_MEMDEV * pDev, int32_t x,int32_t y);
void              GUI_MEMDEV__BlendColor32       (GUI_MEMDEV_Handle hMem, U32 BlendColor, U8 BlendIntens);

unsigned GUI__AlphaPreserveTrans(int32_t OnOff);

extern unsigned GUI_MEMDEV__TimePerFrame;

#define GUI_TIME_PER_FRAME (GUI_TIMER_TIME)GUI_MEMDEV__TimePerFrame

#define GUI_POS_AUTO -4095   /* Position value for auto-pos */

#endif

/*********************************************************************
*
*       LCD_HL_ level defines
*
**********************************************************************
*/
#define LCD_HL_DrawHLine             GUI_pContext->pLCD_HL->pfDrawHLine
#define LCD_HL_DrawPixel             GUI_pContext->pLCD_HL->pfDrawPixel

/*********************************************************************
*
*       Helper functions
*
**********************************************************************
*/
#define GUI_ZEROINIT(Obj) GUI__MEMSET(Obj, 0, sizeof(Obj))
int32_t  GUI_cos(int32_t angle);
int32_t  GUI_sin(int32_t angle);
extern const U32 GUI_Pow10[10];

/* Multi-touch */
void GUI_MTOUCH__ManagePID(int32_t OnOff);

/* Anti-aliased drawing */
int32_t  GUI_AA_Init       (int32_t x0, int32_t x1);
int32_t  GUI_AA_Init_HiRes (int32_t x0, int32_t x1);
void GUI_AA_Exit       (void);
I16  GUI_AA_HiRes2Pixel(int32_t HiRes);

void GL_DrawCircleAA_HiRes(int32_t x0, int32_t y0, int32_t r);
void GL_FillCircleAA_HiRes (int32_t x0, int32_t y0, int32_t r);
void GL_FillEllipseAA_HiRes(int32_t x0, int32_t y0, int32_t rx, int32_t ry);

void GUI_AA__DrawCharAA2(int32_t x0, int32_t y0, int32_t XSize, int32_t YSize, int32_t BytesPerLine, const U8 * pData);
void GUI_AA__DrawCharAA4(int32_t x0, int32_t y0, int32_t XSize, int32_t YSize, int32_t BytesPerLine, const U8 * pData);
void GUI_AA__DrawCharAA8(int32_t x0, int32_t y0, int32_t XSize, int32_t YSize, int32_t BytesPerLine, const U8 * pData);

/* Alpha blending helper functions */
#define GUI_ALPHABLENDING_DONE  (1 << 0)

int32_t      GUI__GetAlphaBuffer    (U32 ** ppCurrent, U32 ** ppConvert, U32 ** ppData, int32_t * pVXSizeMax);
int32_t      GUI__AllocAlphaBuffer  (int32_t AllocDataBuffer);
U32    * GUI__DoAlphaBlending   (int32_t x, int32_t y, U32 * pData, int32_t xSize, tLCDDEV_Index2Color * pfIndex2Color_DEV, int32_t * pDone);
unsigned GUI__SetAlphaBufferSize(int32_t xSize);

/* System independent font routines */
int32_t        GUI_SIF__GetCharDistX       (U16P c, int32_t * pSizeX);
void       GUI_SIF__GetFontInfo        (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
char       GUI_SIF__IsInFont           (const GUI_FONT * pFont, U16 c);
const U8 * GUI_SIF__GetpCharInfo       (const GUI_FONT * pFont, U16P c, unsigned SizeOfCharInfo);
int32_t        GUI_SIF__GetNumCharAreas    (const GUI_FONT * pFont);
int32_t        GUI_SIF__GetCharDistX_ExtFrm(U16P c, int32_t * pSizeX);
void       GUI_SIF__GetFontInfo_ExtFrm (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
char       GUI_SIF__IsInFont_ExtFrm    (const GUI_FONT * pFont, U16 c);
int32_t        GUI_SIF__GetCharInfo_ExtFrm (U16P c, GUI_CHARINFO_EXT * pInfo);
void       GUI_SIF__ClearLine_ExtFrm   (const char * s, int32_t Len);

/* External binary font routines */
int32_t        GUI_XBF__GetOff       (const GUI_XBF_DATA * pXBF_Data, unsigned c, U32 * pOff);
int32_t        GUI_XBF__GetOffAndSize(const GUI_XBF_DATA * pXBF_Data, unsigned c, U32 * pOff, U16 * pSize);
int32_t        GUI_XBF__GetCharDistX (U16P c, int32_t * pSizeX);
void       GUI_XBF__GetFontInfo  (const GUI_FONT * pFont, GUI_FONTINFO * pInfo);
char       GUI_XBF__IsInFont     (const GUI_FONT * pFont, U16 c);
int32_t        GUI_XBF__GetCharInfo  (U16P c, GUI_CHARINFO_EXT * pInfo);
void       GUI_XBF__ClearLine    (const char * s, int32_t Len);

/* Conversion routines */
void GUI_AddHex     (U32 v, U8 Len, char ** ps);
void GUI_AddBin     (U32 v, U8 Len, char ** ps);
void GUI_AddDecMin  (I32 v, char ** ps);
void GUI_AddDecShift(I32 v, U8 Len, U8 Shift, char ** ps);
long GUI_AddSign    (long v, char ** ps);
int32_t  GUI_Long2Len   (I32 v);

#define GUI_UC__GetCharSize(sText)  GUI_pUC_API->pfGetCharSize(sText)
#define GUI_UC__GetCharCode(sText)  GUI_pUC_API->pfGetCharCode(sText)

int32_t   GUI_UC__CalcSizeOfChar   (U16 Char);
U16   GUI_UC__GetCharCodeInc   (const char ** ps);
int32_t   GUI_UC__NumChars2NumBytes(const char * s, int32_t NumChars);
int32_t   GUI_UC__NumBytes2NumChars(const char * s, int32_t NumBytes);

int32_t  GUI__GetLineNumChars  (const char * s, int32_t MaxNumChars);
int32_t  GUI__GetNumChars      (const char * s);
int32_t  GUI__GetOverlap       (U16 Char);
int32_t  GUI__GetLineDistX     (const char * s, int32_t Len);
int32_t  GUI__GetFontSizeY     (void);
int32_t  GUI__HandleEOLine     (const char ** ps);
void GUI__InvertRectColors (int32_t x0, int32_t y0, int32_t x1, int32_t y1);
void GUI__DispLine         (const char * s, int32_t Len, const GUI_RECT * pr);
void GUI__AddSpaceHex      (U32 v, U8 Len, char ** ps);
void GUI__CalcTextRect     (const char * pText, const GUI_RECT * pTextRectIn, GUI_RECT * pTextRectOut, int32_t TextAlign);

void GUI__ClearTextBackground(int32_t xDist, int32_t yDist);

int32_t  GUI__WrapGetNumCharsDisp       (const char * pText, int32_t xSize, GUI_WRAPMODE WrapMode);
int32_t  GUI__WrapGetNumCharsToNextLine (const char * pText, int32_t xSize, GUI_WRAPMODE WrapMode);
int32_t  GUI__WrapGetNumBytesToNextLine (const char * pText, int32_t xSize, GUI_WRAPMODE WrapMode);
//void GUI__memset    (U8  * p, U8 Fill, int32_t NumBytes);
void GUI__memset16  (U16 * p, U16 Fill, int32_t NumWords);
int32_t  GUI__strlen    (const char * s);
int32_t  GUI__strcmp    (const char * s0, const char * s1);
int32_t  GUI__strcmp_hp (GUI_HMEM hs0, const char * s1);

/* Get cursor position */
int32_t  GUI__GetCursorPosX     (const char * s, int32_t Index, int32_t MaxNumChars);
int32_t  GUI__GetCursorPosChar  (const char * s, int32_t x, int32_t NumCharsToNextLine);
U16  GUI__GetCursorCharacter(const char * s, int32_t Index, int32_t MaxNumChars, int32_t * pIsRTL);

/* Arabic support (tbd) */
U16  GUI__GetPresentationForm     (U16 Char, U16 Next, U16 Prev, int32_t * pIgnoreNext, const char * s);
int32_t  GUI__IsArabicCharacter       (U16 c);

/* BiDi support */
int32_t  GUI__BIDI_Log2Vis           (const char * s, int32_t NumChars, char * pBuffer, int32_t BufferSize);
int32_t  GUI__BIDI_GetCursorPosX     (const char * s, int32_t NumChars, int32_t Index);
int32_t  GUI__BIDI_GetCursorPosChar  (const char * s, int32_t NumChars, int32_t x);
U16  GUI__BIDI_GetLogChar        (const char * s, int32_t NumChars, int32_t Index);
int32_t  GUI__BIDI_GetCharDir        (const char * s, int32_t NumChars, int32_t Index);
int32_t  GUI__BIDI_IsNSM             (U16 Char);
U16  GUI__BIDI_GetCursorCharacter(const char * s, int32_t Index, int32_t MaxNumChars, int32_t * pIsRTL);
int32_t  GUI__BIDI_GetWordWrap       (const char * s, int32_t xSize, int32_t * pxDist);
int32_t  GUI__BIDI_GetCharWrap       (const char * s, int32_t xSize);

#if (GUI_USE_BIDI2)

#define GUI__BIDI_Log2Vis            GUI__BIDI2_Log2Vis
#define GUI__BIDI_GetCursorPosX      GUI__BIDI2_GetCursorPosX
#define GUI__BIDI_GetCursorPosChar   GUI__BIDI2_GetCursorPosChar
#define GUI__BIDI_GetLogChar         GUI__BIDI2_GetLogChar
#define GUI__BIDI_GetCharDir         GUI__BIDI2_GetCharDir
#define GUI__BIDI_IsNSM              GUI__BIDI2_IsNSM
#define GUI__BIDI_GetCursorCharacter GUI__BIDI2_GetCursorCharacter
#define GUI__BIDI_GetWordWrap        GUI__BIDI2_GetWordWrap
#define GUI__BIDI_GetCharWrap        GUI__BIDI2_GetCharWrap
#define GUI__BIDI_SetBaseDir         GUI__BIDI2_SetBaseDir
#define GUI__BIDI_GetBaseDir         GUI__BIDI2_GetBaseDir

int32_t  GUI__BIDI_Log2Vis           (const char * s, int32_t NumChars, char * pBuffer, int32_t BufferSize);
int32_t  GUI__BIDI_GetCursorPosX     (const char * s, int32_t NumChars, int32_t Index);
int32_t  GUI__BIDI_GetCursorPosChar  (const char * s, int32_t NumChars, int32_t x);
U16  GUI__BIDI_GetLogChar        (const char * s, int32_t NumChars, int32_t Index);
int32_t  GUI__BIDI_GetCharDir        (const char * s, int32_t NumChars, int32_t Index);
int32_t  GUI__BIDI_IsNSM             (U16 Char);
U16  GUI__BIDI_GetCursorCharacter(const char * s, int32_t Index, int32_t MaxNumChars, int32_t * pIsRTL);
int32_t  GUI__BIDI_GetWordWrap       (const char * s, int32_t xSize, int32_t * pxDist);
int32_t  GUI__BIDI_GetCharWrap       (const char * s, int32_t xSize);
void GUI__BIDI_SetBaseDir        (int32_t Dir);
int32_t  GUI__BIDI_GetBaseDir        (void);

#else

#define GUI__BIDI_SetBaseDir
#define GUI__BIDI_GetBaseDir

#endif

const char * GUI__BIDI_Log2VisBuffered(const char * s, int32_t * pMaxNumChars, int32_t Mode);

extern int32_t GUI__BIDI_Enabled;

extern int32_t (* _pfGUI__BIDI_Log2Vis         )(const char * s, int32_t NumChars, char * pBuffer, int32_t BufferSize);
extern int32_t (* _pfGUI__BIDI_GetCursorPosX   )(const char * s, int32_t NumChars, int32_t Index);
extern int32_t (* _pfGUI__BIDI_GetCursorPosChar)(const char * s, int32_t NumChars, int32_t x);
extern U16 (* _pfGUI__BIDI_GetLogChar      )(const char * s, int32_t NumChars, int32_t Index);
extern int32_t (* _pfGUI__BIDI_GetCharDir      )(const char * s, int32_t NumChars, int32_t Index);
extern int32_t (* _pfGUI__BIDI_IsNSM           )(U16 Char);

/* BiDi-related function pointers */
extern const char * (* GUI_CharLine_pfLog2Vis)(const char * s, int32_t * pMaxNumChars, int32_t Mode);

extern int32_t (* GUI__GetCursorPos_pfGetPosX)     (const char * s, int32_t MaxNumChars, int32_t Index);
extern int32_t (* GUI__GetCursorPos_pfGetPosChar)  (const char * s, int32_t MaxNumChars, int32_t x);
extern U16 (* GUI__GetCursorPos_pfGetCharacter)(const char * s, int32_t MaxNumChars, int32_t Index, int32_t * pIsRTL);

extern int32_t (* GUI__Wrap_pfGetWordWrap)(const char * s, int32_t xSize, int32_t * pxDist);
extern int32_t (* GUI__Wrap_pfGetCharWrap)(const char * s, int32_t xSize);

/* Proportional  font support */
const GUI_FONT_PROP * GUIPROP__FindChar(const GUI_FONT_PROP * pProp, U16P c);

/* Extended proportional font support */
const GUI_FONT_PROP_EXT * GUIPROP_EXT__FindChar(const GUI_FONT_PROP_EXT * pPropExt, U16P c);
void  GUIPROP_EXT__DispLine      (const char * s, int32_t Len);
void  GUIPROP_EXT__ClearLine     (const char * s, int32_t Len);
void  GUIPROP_EXT__SetfpClearLine(void (* fpClearLine)(const char * s, int32_t Len));

/* Reading data routines */
U16 GUI__Read16(const U8 ** ppData);
U32 GUI__Read32(const U8 ** ppData);

/* Virtual screen support */
void GUI__GetOrg(int32_t * px, int32_t * py);
void GUI__SetOrgHook(void(* pfHook)(int32_t x, int32_t y));

/* Timer support */
int32_t              GUI_TIMER__IsActive       (void);
GUI_TIMER_TIME   GUI_TIMER__GetPeriod      (void);
GUI_TIMER_HANDLE GUI_TIMER__GetFirstTimer  (PTR_ADDR * pContext);
GUI_TIMER_HANDLE GUI_TIMER__GetNextTimerLin(GUI_TIMER_HANDLE hTimer, PTR_ADDR * pContext);

/* Get function pointers for color conversion */
tLCDDEV_Index2Color * GUI_GetpfIndex2ColorEx(int32_t LayerIndex);
tLCDDEV_Color2Index * GUI_GetpfColor2IndexEx(int32_t LayerIndex);

int32_t GUI_GetBitsPerPixelEx(int32_t LayerIndex);

LCD_PIXELINDEX * LCD_GetpPalConvTable        (const LCD_LOGPALETTE * pLogPal);
LCD_PIXELINDEX * LCD_GetpPalConvTableUncached(const LCD_LOGPALETTE * pLogPal);
LCD_PIXELINDEX * LCD_GetpPalConvTableBM      (const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int32_t LayerIndex);

/* Setting a function for converting a color palette to an array of index values */
void GUI_SetFuncGetpPalConvTable(LCD_PIXELINDEX * (* pFunc)(const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int32_t LayerIndex));

/*********************************************************************
*
*       Format definitions used by streamed bitmaps
*
*   IMPORTANT: DO NOT CHANGE THESE VALUES!
*   THEY HAVE TO CORRESPOND TO THE DEFINITIONS WITHIN THE CODE OF THE BITMAPCONVERTER!
*/
#define GUI_STREAM_FORMAT_INDEXED    100 /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE4       6   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE8       7   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_565        8   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M565       9   /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_555        10  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M555       11  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE16      12  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLEM16     13  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_8888       16  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLE32      15  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_24         17  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_RLEALPHA   18  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_444_12     19  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M444_12    20  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_444_12_1   21  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M444_12_1  22  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_444_16     23  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M444_16    24  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_A555       25  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_AM555      26  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_A565       27  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_AM565      28  /* DO NOT CHANGE */
#define GUI_STREAM_FORMAT_M8888I     29  /* DO NOT CHANGE */

void GUI__ReadHeaderFromStream  (GUI_BITMAP_STREAM * pBitmapHeader, const U8 * pData);
void GUI__CreateBitmapFromStream(const GUI_BITMAP_STREAM * pBitmapHeader, const void * pData, GUI_BITMAP * pBMP, GUI_LOGPALETTE * pPAL, const GUI_BITMAP_METHODS * pMethods);

/* Cache management */
int32_t GUI__ManageCache  (int32_t Cmd);
int32_t GUI__ManageCacheEx(int32_t LayerIndex, int32_t Cmd);

/*********************************************************************
*
*       2d - GL
*
**********************************************************************
*/
void GL_DispChar         (U16 c);
void GL_DrawArc          (int32_t x0, int32_t y0, int32_t rx, int32_t ry, int32_t a0, int32_t a1);
void GL_DrawBitmap       (const GUI_BITMAP * pBM, int32_t x0, int32_t y0);
void GL_DrawCircle       (int32_t x0, int32_t y0, int32_t r);
void GL_DrawEllipse      (int32_t x0, int32_t y0, int32_t rx, int32_t ry, int32_t w);
void GL_DrawHLine        (int32_t y0, int32_t x0, int32_t x1);
void GL_DrawPolygon      (const GUI_POINT * pPoints, int32_t NumPoints, int32_t x0, int32_t y0);
void GL_DrawPoint        (int32_t x,  int32_t y);
void GL_DrawLine1        (int32_t x0, int32_t y0, int32_t x1, int32_t y1);
void GL_DrawLine1Ex      (int32_t x0, int32_t y0, int32_t x1, int32_t y1, unsigned * pPixelCnt);
void GL_DrawLineRel      (int32_t dx, int32_t dy);
void GL_DrawLineTo       (int32_t x,  int32_t y);
void GL_DrawLineToEx     (int32_t x,  int32_t y, unsigned * pPixelCnt);
void GL_DrawLine         (int32_t x0, int32_t y0, int32_t x1, int32_t y1);
void GL_DrawLineEx       (int32_t x0, int32_t y0, int32_t x1, int32_t y1, unsigned * pPixelCnt);
void GL_MoveTo           (int32_t x,  int32_t y);
void GL_FillCircle       (int32_t x0, int32_t y0, int32_t r);
void GL_FillCircleAA     (int32_t x0, int32_t y0, int32_t r);
void GL_FillEllipse      (int32_t x0, int32_t y0, int32_t rx, int32_t ry);
void GL_FillPolygon      (const GUI_POINT * pPoints, int32_t NumPoints, int32_t x0, int32_t y0);
void GL_SetDefault       (void);

/*********************************************************************
*
*       Replacement of memcpy() and memset()
*
**********************************************************************
*/
//
// Configurable function pointers
//
extern void * (* GUI__pfMemset)(void * pDest, int32_t Fill, size_t Cnt);
extern void * (* GUI__pfMemcpy)(void * pDest, const void * pSrc, size_t Cnt);

extern int32_t    (* GUI__pfStrcmp)(const char *, const char *);
extern size_t (* GUI__pfStrlen)(const char *);
extern char * (* GUI__pfStrcpy)(char *, const char *);
//
// Macros for typesave use of function pointers
//
#define GUI__MEMSET(pDest, Fill, Cnt) GUI__pfMemset((void *)(pDest), (int32_t)(Fill), (size_t)(Cnt))
#define GUI__MEMCPY(pDest, pSrc, Cnt) GUI__pfMemcpy((void *)(pDest), (const void *)(pSrc), (size_t)(Cnt))

/*********************************************************************
*
*       Callback pointers for dynamic linkage
*
**********************************************************************
Dynamic linkage pointers reduces configuration hassles.
*/
typedef int32_t  GUI_tfTimer(void);
typedef int32_t  WM_tfHandlePID(void);

/*********************************************************************
*
*       Text rotation
*
**********************************************************************
*/
extern GUI_RECT  GUI_RectDispString; /* Used by LCD_Rotate...() and GUI_DispStringInRect() */

/*********************************************************************
*
*       Flag for setting transparency for 'EXT' fonts
*
**********************************************************************
*/
extern U8 GUI__CharHasTrans;

/*********************************************************************
*
*       Multitasking support
*
**********************************************************************
*/
extern int32_t GUITASK__EntranceCnt;

/*********************************************************************
*
*       Bitmap related functions
*
**********************************************************************
*/

int32_t       GUI_GetBitmapPixelIndex(const GUI_BITMAP * pBMP, unsigned x, unsigned y);
GUI_COLOR GUI_GetBitmapPixelColor(const GUI_BITMAP * pBMP, unsigned x, unsigned y);
int32_t       GUI_GetBitmapPixelIndexEx(int32_t BitsPerPixel, int32_t BytesPerLine, const U8 * pData, unsigned x, unsigned y);

void      GUI__DrawBitmap16bpp (int32_t x0, int32_t y0, int32_t xsize, int32_t ysize, const U8 * pPixel, const LCD_LOGPALETTE * pLogPal, int32_t xMag, int32_t yMag, tLCDDEV_Index2Color * pfIndex2Color, const LCD_API_COLOR_CONV * pColorConvAPI);
void      GUI__DrawBitmapA16bpp(int32_t x0, int32_t y0, int32_t xSize, int32_t ySize, const U8 * pPixel, const LCD_LOGPALETTE * pLogPal, int32_t xMag, int32_t yMag, tLCDDEV_Index2Color * pfIndex2Color);
void      GUI__SetPixelAlpha   (int32_t x, int32_t y, U8 Alpha, LCD_COLOR Color);
LCD_COLOR GUI__MixColors       (LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
void      GUI__MixColorsBulk   (U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens);

extern const GUI_UC_ENC_APILIST GUI_UC_None;

/*********************************************************************
*
*       LCDDEV_L0_xxx
*
**********************************************************************
*/
#define LCDDEV_L0_Color2Index         GUI__apDevice[GUI_pContext->SelLayer]->pColorConvAPI->pfColor2Index
#define LCDDEV_L0_Index2Color         GUI__apDevice[GUI_pContext->SelLayer]->pColorConvAPI->pfIndex2Color

#define LCDDEV_L0_DrawBitmap          GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawBitmap
#define LCDDEV_L0_DrawHLine           GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawHLine
#define LCDDEV_L0_DrawVLine           GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawVLine
#define LCDDEV_L0_DrawPixel           GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfDrawPixel
#define LCDDEV_L0_FillRect            GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfFillRect
#define LCDDEV_L0_GetPixel            GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetPixel
#define LCDDEV_L0_GetRect             GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetRect
#define LCDDEV_L0_GetPixelIndex       GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetPixelIndex
#define LCDDEV_L0_SetPixelIndex       GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfSetPixelIndex
#define LCDDEV_L0_XorPixel            GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfXorPixel
#define LCDDEV_L0_GetDevFunc          GUI__apDevice[GUI_pContext->SelLayer]->pDeviceAPI->pfGetDevFunc

void LCD_ReadRect  (int32_t x0, int32_t y0, int32_t x1, int32_t y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);
void GUI_ReadRect  (int32_t x0, int32_t y0, int32_t x1, int32_t y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);
void GUI_ReadRectEx(int32_t x0, int32_t y0, int32_t x1, int32_t y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);

void LCD_ReadRectNoClip(int32_t x0, int32_t y0, int32_t x1, int32_t y1, LCD_PIXELINDEX * pBuffer, GUI_DEVICE * pDevice);

/*********************************************************************
*
*       Internal color management
*
**********************************************************************
*/
typedef struct {
  void         (* pfSetColor)   (LCD_COLOR Index);
  void         (* pfSetBkColor) (LCD_COLOR Index);
  LCD_DRAWMODE (* pfSetDrawMode)(LCD_DRAWMODE dm);
} LCD_SET_COLOR_API;

extern const LCD_SET_COLOR_API * LCD__pSetColorAPI;

#define LCD__SetBkColorIndex(Index) (*GUI_pContext->LCD_pBkColorIndex = Index)
#define LCD__SetColorIndex(Index)   (*GUI_pContext->LCD_pColorIndex   = Index)
#define LCD__GetBkColorIndex()      (*GUI_pContext->LCD_pBkColorIndex)
#define LCD__GetColorIndex()        (*GUI_pContext->LCD_pColorIndex)

/* The following 2 defines are only required for compatibility to older versions of the TTF library */
#define LCD_BKCOLORINDEX (*GUI_pContext->LCD_pBkColorIndex)
#define LCD_COLORINDEX   (*GUI_pContext->LCD_pColorIndex)

/*********************************************************************
*
*       EXTERNs for GL_CORE
*
**********************************************************************
*/
extern const GUI_FONT * GUI__pFontDefault;
extern GUI_COLOR        GUI__ColorDefault;
extern GUI_COLOR        GUI__BkColorDefault;

extern GUI_SADDR GUI_CONTEXT * GUI_pContext;

extern GUI_DEVICE * GUI__apDevice[GUI_NUM_LAYERS];

//
// Function pointer for converting a palette containing a color array into an index array
//
extern LCD_PIXELINDEX * (* GUI_pfGetpPalConvTable)(const LCD_LOGPALETTE * pLogPal, const GUI_BITMAP * pBitmap, int32_t LayerIndex);

//
// Function pointer for mixing up 2 colors
//
extern LCD_COLOR (* GUI__pfMixColors)(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);

//
// Function pointer for mixing up arrays of colors
//
extern void (* GUI__pfMixColorsBulk)(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens);

//
// Function pointer for mixing color and gamma values
//
extern LCD_COLOR (* LCD_AA_pfMixColors16)(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);

//
// Function pointer for drawing alpha memory devices
//
extern GUI_DRAWMEMDEV_FUNC   * GUI__pfDrawAlphaMemdevFunc;
extern GUI_DRAWMEMDEV_FUNC   * GUI__pfDrawM565MemdevFunc;

//
// Function pointer for drawing alpha bitmaps
//
extern GUI_DRAWBITMAP_FUNC * GUI__pfDrawAlphaBitmapFunc;
extern GUI_DRAWBITMAP_FUNC * GUI__pfDrawM565BitmapFunc;

extern U8 GUI__DrawStreamedBitmap;

//
// API list to be used for MultiBuffering
//
extern const GUI_MULTIBUF_API    GUI_MULTIBUF_APIList;
extern const GUI_MULTIBUF_API    GUI_MULTIBUF_APIListMasked;
extern const GUI_MULTIBUF_API_EX GUI_MULTIBUF_APIListEx;

#ifdef  GL_CORE_C
  #define GUI_EXTERN
#else
  #define GUI_EXTERN extern
#endif

GUI_EXTERN   void (* GUI_pfExecAnimations)(void);
GUI_EXTERN   int32_t  (* GUI_pfUpdateSoftLayer)(void);

#ifdef WIN32
  GUI_EXTERN void (* GUI_pfSoftlayerGetPixel)(int32_t x, int32_t y, void * p);
#endif

GUI_EXTERN void (* GUI_pfHookMTOUCH)(const GUI_MTOUCH_STATE * pState);

GUI_EXTERN const GUI_UC_ENC_APILIST * GUI_pUC_API; /* Unicode encoding API */

GUI_EXTERN GUI_SADDR char             GUI_DecChar;
GUI_EXTERN           GUI_tfTimer    * GUI_pfTimerExec;
GUI_EXTERN           WM_tfHandlePID * WM_pfHandlePID;
GUI_EXTERN   void (* GUI_pfDispCharStyle)(U16 Char);
GUI_EXTERN   void (* GUI_pfDispCharLine)(int32_t x0);

GUI_EXTERN           int32_t GUI__BufferSize; // Required buffer size in pixels for alpha blending and/or antialiasing
GUI_EXTERN           int32_t GUI_AA__ClipX0;  // x0-clipping value for AA module

GUI_EXTERN           I8  GUI__aNumBuffers[GUI_NUM_LAYERS]; // Number of buffers used per layer
GUI_EXTERN           U8  GUI__PreserveTrans;
GUI_EXTERN           U8  GUI__IsInitialized;

GUI_EXTERN           U8  GUI__NumLayersInUse;
GUI_EXTERN           U32 GUI__LayerMask;

#if GUI_SUPPORT_ROTATION
  GUI_EXTERN const tLCD_APIList * GUI_pLCD_APIList; /* Used for rotating text */
#endif

GUI_EXTERN I16 GUI_OrgX, GUI_OrgY;

#undef GUI_EXTERN

#if defined(__cplusplus)
}
#endif

#endif /* GUI_PRIVATE_H */

/*************************** End of file ****************************/
