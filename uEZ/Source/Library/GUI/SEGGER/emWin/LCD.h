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
File        : LCD.h
Purpose     : Declares LCD interface functions
----------------------------------------------------------------------
*/

#ifndef LCD_H
#define LCD_H

#include "GUI_ConfDefaults.h" /* Used for GUI_CONST_STORAGE */
#include "Global.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Basic type defines
*
* The follwing are defines for types used in the LCD-driver and the
* GUI layers on top of that. Since "C" does not provide data types of
* fixed length which are identical on all platforms, this is done here.
* For most 16/32 controllers, the settings will work fine. However, if
* you have similar defines in other sections of your program, you might
* want to change or relocate these defines, e.g. in a TYPE.h file.
*/
#define I16P I16              /*   signed 16 bits OR MORE ! */
#define U16P U16              /* unsigned 16 bits OR MORE ! */

/*********************************************************************
*
*       Settings for windows simulation
*
* Some settings in the configuration may conflict with the values required
* in the Simulation. This is why we ignore the target settings for data
* types and use the correct settings for the simulation.
* (U32 could be defined as long, which would yield a 64 bit type on
* the PC)
*/
#ifdef WIN32
  #pragma warning( disable : 4244 )  // Disable warning messages in simulation
  #pragma warning( disable : 4761 )  // Disable warning "integral size mismatch in argument; conversion supplied"
#endif

/*********************************************************************
*
*       Constants
*/
#define LCD_ERR0 (0x10)
#define LCD_ERR_CONTROLLER_NOT_FOUND (LCD_ERR0+1)
#define LCD_ERR_MEMORY               (LCD_ERR0+2)

/*********************************************************************
*
*       Drawing modes
*/
#define LCD_DRAWMODE_NORMAL (0)
#define LCD_DRAWMODE_XOR    (1<<0)
#define LCD_DRAWMODE_TRANS  (1<<1)
#define LCD_DRAWMODE_REV    (1<<2)

/*********************************************************************
*
*       Typedefs
*/
typedef int32_t LCD_DRAWMODE;
typedef U32 LCD_COLOR;

/*********************************************************************
*
*       Data structures
*/
typedef struct { I16P x,y; } GUI_POINT;
typedef struct { I16 x0,y0,x1,y1; } LCD_RECT;

typedef struct {
  int32_t              NumEntries;
  char             HasTrans;
  const LCD_COLOR * pPalEntries;
} LCD_LOGPALETTE;

/* This is used for the simulation only ! */
typedef struct {
  int32_t x,y;
  unsigned char KeyStat;
} LCD_tMouseState;

typedef struct {
  int32_t               NumEntries;
  const LCD_COLOR * pPalEntries;
} LCD_PHYSPALETTE;

/*********************************************************************
*
*       LCD_L0_... color conversion
*/
typedef LCD_COLOR      tLCDDEV_Index2Color  (LCD_PIXELINDEX Index);
typedef LCD_PIXELINDEX tLCDDEV_Color2Index  (LCD_COLOR Color);
typedef LCD_PIXELINDEX tLCDDEV_GetIndexMask (void);

typedef void tLCDDEV_Index2ColorBulk(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex);
typedef void tLCDDEV_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex);

/*********************************************************************
*
*       Color conversion API tables
*/
typedef struct {
  tLCDDEV_Color2Index  * pfColor2Index;
  tLCDDEV_Index2Color  * pfIndex2Color;
  tLCDDEV_GetIndexMask * pfGetIndexMask;
  int32_t NoAlpha;
  tLCDDEV_Color2IndexBulk * pfColor2IndexBulk;
  tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk;
} LCD_API_COLOR_CONV;

extern const LCD_API_COLOR_CONV LCD_API_ColorConv_0;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_2;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_4;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_5;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_8;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1_24;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_2;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_4;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_5;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_6;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_1616I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_111;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_222;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_233;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_323;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_332;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_444_12;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_444_12_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_444_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_555;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_565;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_556;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_655;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_666;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_666_9;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_822216;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_84444;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8666;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8666_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_88666I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_8888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M111;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M1555I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M222;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M233;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M323;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M332;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M4444I;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M444_12;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M444_12_1;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M444_16;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M555;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M565;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M556;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M655;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M666;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M666_9;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M8565;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M8888;
extern const LCD_API_COLOR_CONV LCD_API_ColorConv_M8888I;

#define GUICC_0         &LCD_API_ColorConv_0
#define GUICC_1         &LCD_API_ColorConv_1
#define GUICC_1_2       &LCD_API_ColorConv_1_2
#define GUICC_1_4       &LCD_API_ColorConv_1_4
#define GUICC_1_5       &LCD_API_ColorConv_1_5
#define GUICC_1_8       &LCD_API_ColorConv_1_8
#define GUICC_1_16      &LCD_API_ColorConv_1_16
#define GUICC_1_24      &LCD_API_ColorConv_1_24
#define GUICC_2         &LCD_API_ColorConv_2
#define GUICC_4         &LCD_API_ColorConv_4
#define GUICC_5         &LCD_API_ColorConv_5
#define GUICC_6         &LCD_API_ColorConv_6
#define GUICC_8         &LCD_API_ColorConv_8
#define GUICC_16        &LCD_API_ColorConv_16
#define GUICC_1616I     &LCD_API_ColorConv_1616I
#define GUICC_111       &LCD_API_ColorConv_111
#define GUICC_222       &LCD_API_ColorConv_222
#define GUICC_233       &LCD_API_ColorConv_233
#define GUICC_323       &LCD_API_ColorConv_323
#define GUICC_332       &LCD_API_ColorConv_332
#define GUICC_M4444I    &LCD_API_ColorConv_M4444I
#define GUICC_444_12    &LCD_API_ColorConv_444_12
#define GUICC_444_12_1  &LCD_API_ColorConv_444_12_1
#define GUICC_444_16    &LCD_API_ColorConv_444_16
#define GUICC_555       &LCD_API_ColorConv_555
#define GUICC_565       &LCD_API_ColorConv_565
#define GUICC_556       &LCD_API_ColorConv_556
#define GUICC_655       &LCD_API_ColorConv_655
#define GUICC_666       &LCD_API_ColorConv_666
#define GUICC_666_9     &LCD_API_ColorConv_666_9
#define GUICC_822216    &LCD_API_ColorConv_822216
#define GUICC_84444     &LCD_API_ColorConv_84444
#define GUICC_8666      &LCD_API_ColorConv_8666
#define GUICC_8666_1    &LCD_API_ColorConv_8666_1
#define GUICC_88666I    &LCD_API_ColorConv_88666I
#define GUICC_888       &LCD_API_ColorConv_888
#define GUICC_8888      &LCD_API_ColorConv_8888
#define GUICC_M111      &LCD_API_ColorConv_M111
#define GUICC_M1555I    &LCD_API_ColorConv_M1555I
#define GUICC_M222      &LCD_API_ColorConv_M222
#define GUICC_M233      &LCD_API_ColorConv_M233
#define GUICC_M323      &LCD_API_ColorConv_M323
#define GUICC_M332      &LCD_API_ColorConv_M332
#define GUICC_M444_12   &LCD_API_ColorConv_M444_12
#define GUICC_M444_12_1 &LCD_API_ColorConv_M444_12_1
#define GUICC_M444_16   &LCD_API_ColorConv_M444_16
#define GUICC_M555      &LCD_API_ColorConv_M555
#define GUICC_M565      &LCD_API_ColorConv_M565
#define GUICC_M556      &LCD_API_ColorConv_M556
#define GUICC_M655      &LCD_API_ColorConv_M655
#define GUICC_M666      &LCD_API_ColorConv_M666
#define GUICC_M666_9    &LCD_API_ColorConv_M666_9
#define GUICC_M8565     &LCD_API_ColorConv_M8565
#define GUICC_M888      &LCD_API_ColorConv_M888
#define GUICC_M8888     &LCD_API_ColorConv_M8888
#define GUICC_M8888I    &LCD_API_ColorConv_M8888I

void GUICC_M1555I_SetCustColorConv(tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M565_SetCustColorConv  (tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M4444I_SetCustColorConv(tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M888_SetCustColorConv  (tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);
void GUICC_M8888I_SetCustColorConv(tLCDDEV_Color2IndexBulk * pfColor2IndexBulk, tLCDDEV_Index2ColorBulk * pfIndex2ColorBulk);

/*********************************************************************
*
*       Compatibility defines for older versions
*/
#define GUI_COLOR_CONV_1    GUICC_1
#define GUI_COLOR_CONV_2    GUICC_2
#define GUI_COLOR_CONV_4    GUICC_4
#define GUI_COLOR_CONV_8666 GUICC_8666
#define GUI_COLOR_CONV_888  GUICC_888
#define GUI_COLOR_CONV_8888 GUICC_8888
#define GUI_COLOR_CONV_565  GUICC_565
#define GUI_COLOR_CONV_M565 GUICC_M565

/*********************************************************************
*
*       LCDDEV function table
*
*  Below the routines which need to in an LCDDEV routine table are
*  defined. All of these routines have to be in the low-level driver
*  (LCD_L0) or in the memory device which can be used to replace the
*  driver.
*  The one exception to this is the SetClipRect routine, which would
*  be identical for all drivers and is therefor contained in the
*  level above (LCD).
*/
typedef void         tLCDDEV_DrawPixel    (int32_t x, int32_t y);
typedef void         tLCDDEV_DrawHLine    (int32_t x0, int32_t y0,  int32_t x1);
typedef void         tLCDDEV_DrawVLine    (int32_t x , int32_t y0,  int32_t y1);
typedef void         tLCDDEV_FillRect     (int32_t x0, int32_t y0, int32_t x1, int32_t y1);
typedef uint32_t tLCDDEV_GetPixelIndex(int32_t x, int32_t y);
typedef void         tLCDDEV_SetPixelIndex(int32_t x, int32_t y, int32_t ColorIndex);
typedef void         tLCDDEV_XorPixel     (int32_t x, int32_t y);
typedef void         tLCDDEV_FillPolygon  (const GUI_POINT * pPoints, int32_t NumPoints, int32_t x0, int32_t y0);
typedef void         tLCDDEV_FillPolygonAA(const GUI_POINT * pPoints, int32_t NumPoints, int32_t x0, int32_t y0);
typedef void         tLCDDEV_GetRect      (LCD_RECT * pRect);
typedef int32_t          tLCDDEV_Init         (void);
typedef void         tLCDDEV_On           (void);
typedef void         tLCDDEV_Off          (void);
typedef void         tLCDDEV_SetLUTEntry  (U8 Pos, LCD_COLOR color);
typedef void *       tLCDDEV_GetDevFunc   (int32_t Index);
typedef I32          tLCDDEV_GetDevProp   (int32_t Index);
typedef void         tLCDDEV_SetOrg       (int32_t x, int32_t y);

/*********************************************************************
*
*       Memory device API tables
*/
typedef struct GUI_DEVICE     GUI_DEVICE;
typedef struct GUI_DEVICE_API GUI_DEVICE_API;

typedef void tLCDDEV_DrawBitmap   (int32_t x0, int32_t y0, int32_t xsize, int32_t ysize,
                       int32_t BitsPerPixel, int32_t BytesPerLine,
                       const U8 * pData, int32_t Diff,
                       const void * pTrans);   /* Really LCD_PIXELINDEX, but is void to avoid compiler warnings */
#define GUI_MEMDEV_APILIST_1  &GUI_MEMDEV_DEVICE_1
#define GUI_MEMDEV_APILIST_8  &GUI_MEMDEV_DEVICE_8
#define GUI_MEMDEV_APILIST_16 &GUI_MEMDEV_DEVICE_16
#define GUI_MEMDEV_APILIST_32 &GUI_MEMDEV_DEVICE_32

/*********************************************************************
*
*       Defines for device capabilities
*
* The following is the list of device capabilities which can, but do
* not have to be implemented in the driver. This way the driver can be
* enhanced in the future without affecting the driver interface,
* keeping older drivers compatible.
* More DevCaps can always be added in the future, as older drivers
* are guaranteed to return 0 for all unimplemented features or queries.
*
* The values below define the legal parameters to the LCD_GetDeviceCaps
* and the LCD_GetpCapFunc routines.
*/

#define LCD_DEVCAP_XSIZE             0x01    /* Quest horiz. res. of display */
#define LCD_DEVCAP_YSIZE             0x02    /* Quest vert. res. of display */
#define LCD_DEVCAP_VXSIZE            0x03    /* Quest vert. res. of virtual disp.*/
#define LCD_DEVCAP_VYSIZE            0x04    /* Quest vert. res. of virtual disp.*/
#define LCD_DEVCAP_XORG              0x05    /* X-origin ... usually 0 */
#define LCD_DEVCAP_YORG              0x06    /* Y-origin ... usually 0 */
#define LCD_DEVCAP_CONTROLLER        0x07    /* LCD Controller (Numerical) */
#define LCD_DEVCAP_BITSPERPIXEL      0x08    /* Bits per pixel ... 1/2/4/8 */
#define LCD_DEVCAP_NUMCOLORS         0x09    /* Quest number of colors */
#define LCD_DEVCAP_XMAG              0x0A
#define LCD_DEVCAP_YMAG              0x0B
#define LCD_DEVCAP_MIRROR_X          0x0C
#define LCD_DEVCAP_MIRROR_Y          0x0D
#define LCD_DEVCAP_SWAP_XY           0x0E
#define LCD_DEVCAP_SWAP_RB           0x0F

int32_t LCD_GetXSizeMax(void);
int32_t LCD_GetYSizeMax(void);
int32_t LCD_GetVXSizeMax(void);
int32_t LCD_GetVYSizeMax(void);
int32_t LCD_GetBitsPerPixelMax(void);
void LCD_SetDisplaySize(int32_t xSizeDisplay, int32_t ySizeDisplay);
int32_t LCD_GetXSizeDisplay(void);
int32_t LCD_GetYSizeDisplay(void);

int32_t LCD_GetXSizeEx          (int32_t LayerIndex);
int32_t LCD_GetYSizeEx          (int32_t LayerIndex);
int32_t LCD_GetVXSizeEx         (int32_t LayerIndex);
int32_t LCD_GetVYSizeEx         (int32_t LayerIndex);
int32_t LCD_GetBitsPerPixelEx   (int32_t LayerIndex);
U32 LCD_GetNumColorsEx      (int32_t LayerIndex);
int32_t LCD_GetXMagEx           (int32_t LayerIndex);
int32_t LCD_GetYMagEx           (int32_t LayerIndex);
int32_t LCD_GetMirrorXEx        (int32_t LayerIndex);
int32_t LCD_GetMirrorYEx        (int32_t LayerIndex);
int32_t LCD_GetSwapXYEx         (int32_t LayerIndex);
int32_t LCD_GetReversLUTEx      (int32_t LayerIndex);
int32_t LCD_GetPhysColorsInRAMEx(int32_t LayerIndex);

int32_t LCD_GetXSize            (void);
int32_t LCD_GetYSize            (void);
int32_t LCD_GetVXSize           (void);
int32_t LCD_GetVYSize           (void);
int32_t LCD_GetBitsPerPixel     (void);
U32 LCD_GetNumColors        (void);
int32_t LCD_GetXMag             (void);
int32_t LCD_GetYMag             (void);
int32_t LCD_GetMirrorX          (void);
int32_t LCD_GetMirrorY          (void);
int32_t LCD_GetSwapXY           (void);
int32_t LCD_GetReversLUT        (void);
int32_t LCD_GetPhysColorsInRAM  (void);

I32 LCD__GetBPP      (U32 IndexMask);
I32 LCD__GetBPPDevice(U32 IndexMask);

tLCDDEV_Index2Color * LCD_GetpfIndex2ColorEx(int32_t LayerIndex);
tLCDDEV_Color2Index * LCD_GetpfColor2IndexEx(int32_t LayerIndex);

tLCDDEV_Color2Index * LCD_GetpfColor2Index(void);

int32_t LCD_GetNumLayers(void);

LCD_COLOR * LCD_GetPalette   (void);
LCD_COLOR * LCD_GetPaletteEx (int32_t LayerIndex);
void      * LCD_GetVRAMAddr  (void);
void      * LCD_GetVRAMAddrEx(int32_t LayerIndex);

void (* LCD_GetDevFunc(int32_t LayerIndex, int32_t Item))(void);

/*********************************************************************
*
*       Runtime rotation of drivers
*/
int32_t LCD_ROTATE_AddDriver  (const GUI_DEVICE_API * pDriver);
int32_t LCD_ROTATE_AddDriverEx(const GUI_DEVICE_API * pDeviceAPI, int32_t LayerIndex);
int32_t LCD_ROTATE_DecSel     (void);
int32_t LCD_ROTATE_DecSelEx   (int32_t LayerIndex);
int32_t LCD_ROTATE_IncSel     (void);
int32_t LCD_ROTATE_IncSelEx   (int32_t LayerIndex);
int32_t LCD_ROTATE_SetCallback(void (* pCbOnConfig)(GUI_DEVICE *, int32_t, int32_t), int32_t LayerIndex);
int32_t LCD_ROTATE_SetSel     (int32_t Index);
int32_t LCD_ROTATE_SetSelEx   (int32_t Index, int32_t LayerIndex);

/*********************************************************************
*
*       Values for requesting and setting function pointers (display driver)
*/
                                       /* Request of a function pointer for... */
#define LCD_DEVFUNC_READRECT      0x01 /* ...reading a rectangular display area */
#define LCD_DEVFUNC_SETALPHA      0x02 /* ...setting the alpha blending factor */
#define LCD_DEVFUNC_SETPOS        0x03 /* ...setting the layer position */
#define LCD_DEVFUNC_GETPOS        0x04 /* ...getting the layer position */
#define LCD_DEVFUNC_SETSIZE       0x05 /* ...setting the layer size */
#define LCD_DEVFUNC_SETVIS        0x06 /* ...setting the visibility of a layer */
#define LCD_DEVFUNC_24BPP         0x07 /* ...drawing 24bpp bitmaps */
#define LCD_DEVFUNC_NEXT_PIXEL    0x08 /* ...drawing a bitmap pixel by pixel */
#define LCD_DEVFUNC_SET_VRAM_ADDR 0x09 /* ...setting the VRAM address */
#define LCD_DEVFUNC_SET_VSIZE     0x0A /* ...setting the VRAM size */
#define LCD_DEVFUNC_SET_SIZE      0x0B /* ...setting the display size */
#define LCD_DEVFUNC_INIT          0x0C /* ...initializing the display controller */
#define LCD_DEVFUNC_CONTROLCACHE  0x0D /* ...controlling the cache */
#define LCD_DEVFUNC_ON            0x0E /* ...switching the display on */
#define LCD_DEVFUNC_OFF           0x0F /* ...switching the display off */
#define LCD_DEVFUNC_SETLUTENTRY   0x10 /* ...setting a LUT entry */
#define LCD_DEVFUNC_FILLPOLY      0x11 /* ...filling a polygon */
#define LCD_DEVFUNC_FILLPOLYAA    0x12 /* ...filling an antialiased polygon */
#define LCD_DEVFUNC_ALPHAMODE     0x13 /* ...setting the alpha blending mode */
#define LCD_DEVFUNC_CHROMAMODE    0x14 /* ...setting the chroma blending mode */
#define LCD_DEVFUNC_CHROMA        0x15 /* ...setting the chroma values */
#define LCD_DEVFUNC_SETFUNC       0x16 /* ...setting a function pointer */
#define LCD_DEVFUNC_REFRESH       0x17 /* ...refreshing the display */
#define LCD_DEVFUNC_SETRECT       0x18 /* ...setting the drawing rectangle */
                                       /* Setting a function pointer for... */
#define LCD_DEVFUNC_FILLRECT      0x19 /* ...filling a rectangular area */
#define LCD_DEVFUNC_DRAWBMP_1BPP  0x20 /* ...drawing a 1bpp bitmap */
#define LCD_DEVFUNC_COPYBUFFER    0x21 /* ...copying complete frame buffers */
#define LCD_DEVFUNC_SHOWBUFFER    0x22 /* ...shows the given buffer */
#define LCD_DEVFUNC_COPYRECT      0x23 /* ...filling a rectangular area */
#define LCD_DEVFUNC_DRAWBMP_16BPP 0x24 /* ...drawing a 16bpp bitmap */
#define LCD_DEVFUNC_DRAWBMP_8BPP  0x25 /* ...drawing a 8bpp bitmap */
#define LCD_DEVFUNC_READPIXEL     0x26 /* ...reading a pixel index */
#define LCD_DEVFUNC_READMPIXELS   0x27 /* ...reading multiple pixel indices */
#define LCD_DEVFUNC_DRAWBMP_32BPP 0x28 /* ...drawing a 32bpp bitmap */
#define LCD_DEVFUNC_SET_BUFFERPTR 0x29 /* ...setting an array of buffer pointers */
#define LCD_DEVFUNC_EXIT          0x30 /* ...free memory and shut down controller */

/*********************************************************************
*
*       Values for requesting function pointers (memory devices)
*/
                                          /* Request of a function pointer for... */
#define MEMDEV_DEVFUNC_WRITETOACTIVE 0x16 /* ...writing the memory device */

/*********************************************************************
*
*       Values for requesting data
*/
                                       /* Request pointer to... */
#define LCD_DEVDATA_MEMDEV        0x01 /* ...default memory device API */
#define LCD_DEVDATA_PHYSPAL       0x02 /* ...physical palette */
#define LCD_DEVDATA_VRAMADDR      0x03 /* ...VRAM address */

/*********************************************************************
*
*       Structures for passing data to LCD_X_DisplayDriver()
*/
typedef struct {
  void * pVRAM;
} LCD_X_SETVRAMADDR_INFO;

typedef struct {
  int32_t xPos, yPos;
} LCD_X_SETORG_INFO;

typedef struct {
  LCD_COLOR Color;
  U8 Pos;
} LCD_X_SETLUTENTRY_INFO;

typedef struct {
  int32_t xSize, ySize;
} LCD_X_SETSIZE_INFO;

typedef struct {
  int32_t xPos, yPos;
  int32_t xLen, yLen;
  int32_t BytesPerPixel;
  U32 Off;
} LCD_X_SETPOS_INFO;

typedef struct {
  int32_t Alpha;
} LCD_X_SETALPHA_INFO;

typedef struct {
  int32_t OnOff;
} LCD_X_SETVIS_INFO;

typedef struct {
  int32_t AlphaMode;
} LCD_X_SETALPHAMODE_INFO;

typedef struct {
  int32_t ChromaMode;
} LCD_X_SETCHROMAMODE_INFO;

typedef struct {
  LCD_COLOR ChromaMin;
  LCD_COLOR ChromaMax;
} LCD_X_SETCHROMA_INFO;

typedef struct {
  int32_t Index;
} LCD_X_SHOWBUFFER_INFO;

/*********************************************************************
*
*       Commands for LCD_X_DisplayDriver()
*/
#define LCD_X_INITCONTROLLER 0x01 /* Initializing the display controller */
#define LCD_X_SETVRAMADDR    0x02 /* Setting the video RAM address */
#define LCD_X_SETORG         0x03 /* Setting the origin within a layer */
#define LCD_X_SETLUTENTRY    0x04 /* Setting an entry of the LUT */
#define LCD_X_ON             0x05 /* Switching the display on */
#define LCD_X_OFF            0x06 /* Switching the display off */
#define LCD_X_SETSIZE        0x07 /* Setting the layer size */
#define LCD_X_SETPOS         0x08 /* Setting the layer position */
#define LCD_X_SETVIS         0x09 /* Setting the visibility of a layer */
#define LCD_X_SETALPHA       0x0A /* Setting the alpha value of the layer */
#define LCD_X_SETALPHAMODE   0x0B /* Setting the alpha blending mode */
#define LCD_X_SETCHROMAMODE  0x0C /* Setting the chroma blending mode */
#define LCD_X_SETCHROMA      0x0D /* Setting the chroma values */
#define LCD_X_SHOWBUFFER     0x0E /* Switching to the given buffer */

int32_t  LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData);
void LCD_X_Config(void);

/*********************************************************************
*
*       Get/Set layer properties
*/
int32_t  LCD_GetPosEx       (int32_t LayerIndex, int32_t * pxPos, int32_t * pyPos);
int32_t  LCD_OffEx          (int32_t LayerIndex);
int32_t  LCD_OnEx           (int32_t LayerIndex);
int32_t  LCD_RefreshEx      (int32_t LayerIndex);
int32_t  LCD_SetAlphaEx     (int32_t LayerIndex, int32_t Alpha);
int32_t  LCD_SetAlphaModeEx (int32_t LayerIndex, int32_t AlphaMode);
int32_t  LCD_SetBufferPtrEx (int32_t LayerIndex, void ** pBufferPTR);
int32_t  LCD_SetChromaEx    (int32_t LayerIndex, LCD_COLOR ChromaMin, LCD_COLOR ChromaMax);
int32_t  LCD_SetChromaModeEx(int32_t LayerIndex, int32_t ChromaMode);
int32_t  LCD_SetDevFunc     (int32_t LayerIndex, int32_t IdFunc, void (* pDriverFunc)(void));
int32_t  LCD_SetLUTEntryEx  (int32_t LayerIndex, U8 Pos, LCD_COLOR Color);
int32_t  LCD_SetPosEx       (int32_t LayerIndex, int32_t xPos, int32_t yPos);
int32_t  LCD_SetSizeEx      (int32_t LayerIndex, int32_t xSize, int32_t ySize);
int32_t  LCD_SetVisEx       (int32_t LayerIndex, int32_t OnOff);
int32_t  LCD_SetVRAMAddrEx  (int32_t LayerIndex, void * pVRAM);
int32_t  LCD_SetVSizeEx     (int32_t LayerIndex, int32_t xSize, int32_t ySize);

int32_t  LCD_GetPos         (int32_t * pxPos, int32_t * pyPos);
int32_t  LCD_Off            (void);
int32_t  LCD_On             (void);
int32_t  LCD_Refresh        (void);
int32_t  LCD_SetAlpha       (int32_t Alpha);
int32_t  LCD_SetAlphaMode   (int32_t AlphaMode);
int32_t  LCD_SetBufferPtr   (void ** pBufferPTR);
int32_t  LCD_SetChroma      (LCD_COLOR ChromaMin, LCD_COLOR ChromaMax);
int32_t  LCD_SetChromaMode  (int32_t ChromaMode);
int32_t  LCD_SetLUTEntry    (U8 Pos, LCD_COLOR Color);
void LCD_SetOrg         (int32_t xOrg, int32_t yOrg);
int32_t  LCD_SetPos         (int32_t xPos, int32_t yPos);
int32_t  LCD_SetSize        (int32_t xSize, int32_t ySize);
int32_t  LCD_SetVis         (int32_t OnOff);
int32_t  LCD_SetVRAMAddr    (void * pVRAM);
int32_t  LCD_SetVSize       (int32_t xSize, int32_t ySize);

/*********************************************************************
*
*       NEXT_PIXEL API support
*/
typedef struct {
  int32_t  (* pfStart)   (int32_t x0, int32_t y0, int32_t x1, int32_t y1);
  void (* pfSetPixel)(LCD_PIXELINDEX PixelIndex);
  void (* pfNextLine)(void);
  void (* pfEnd)     (void);
} LCD_API_NEXT_PIXEL;

LCD_API_NEXT_PIXEL * LCD_GetNextPixelAPI(void);

/*********************************************************************
*
*      LCD_CLIP function table
*/
typedef void tLCD_HL_DrawHLine    (int32_t x0, int32_t y0,  int32_t x1);
typedef void tLCD_HL_DrawPixel    (int32_t x0, int32_t y0);

typedef struct {
  tLCD_HL_DrawHLine * pfDrawHLine;
  tLCD_HL_DrawPixel * pfDrawPixel;
} tLCD_HL_APIList;

void LCD_DrawHLine(int32_t x0, int32_t y0,  int32_t x1);
void LCD_DrawPixel(int32_t x0, int32_t y0);
void LCD_DrawVLine(int32_t x,  int32_t y0,  int32_t y1);


/*********************************************************************
*
*       Declarations for LCD_
*/
void LCD_SetClipRectEx(const LCD_RECT * pRect);
void LCD_SetClipRectMax(void);

/* Get device capabilities (0 if not supported) */
I32  LCD_GetDevCap  (int32_t Index);
I32  LCD_GetDevCapEx(int32_t LayerIndex, int32_t Index);

/* Initialize LCD using config-parameters */
int32_t LCD_Init(void);
int32_t LCD_InitColors(void);
int32_t LCD_InitEx(GUI_DEVICE * pDevice, int32_t ClearScreen);
int32_t LCD_ExitEx(int32_t LayerIndex);

void LCD_SetBkColor   (LCD_COLOR Color); /* Set background color */
void LCD_SetColor     (LCD_COLOR Color); /* Set foreground color */
void LCD_SetPixelIndex(int32_t x, int32_t y, int32_t ColorIndex);

/* Palette routines (Not available on all drivers) */
void LCD_InitLUT(void);
void LCD_SetLUTEx(int32_t LayerIndex, const LCD_PHYSPALETTE * pPalette);
void LCD_SetLUT  (const LCD_PHYSPALETTE * pPalette);

LCD_DRAWMODE LCD_SetDrawMode  (LCD_DRAWMODE dm);
void LCD_SetColorIndex(unsigned PixelIndex);
void LCD_SetBkColorIndex(unsigned PixelIndex);
void LCD_FillRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
typedef void tLCD_SetPixelAA(int32_t x, int32_t y, U8 Intens);

void LCD_SetPixelAA4_Trans  (int32_t x, int32_t y, U8 Intens);
void LCD_SetPixelAA4_NoTrans(int32_t x, int32_t y, U8 Intens);

void LCD_SetPixelAA8_Trans  (int32_t x, int32_t y, U8 Intens);
void LCD_SetPixelAA8_NoTrans(int32_t x, int32_t y, U8 Intens);

void LCD_AA_EnableGamma(int32_t OnOff);
void LCD_AA_SetGamma   (U8 * pGamma);
void LCD_AA_GetGamma   (U8 * pGamma);

LCD_COLOR    LCD_AA_MixColors16 (LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
LCD_COLOR    LCD_AA_MixColors256(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
LCD_COLOR    LCD_MixColors256   (LCD_COLOR Color, LCD_COLOR BkColor, unsigned Intens);
LCD_COLOR    LCD_GetPixelColor(int32_t x, int32_t y);     /* Get RGB color of pixel */
uint32_t LCD_GetPixelIndex(int32_t x, int32_t y);
int32_t          LCD_GetBkColorIndex (void);
int32_t          LCD_GetColorIndex (void);
#if (GUI_USE_ARGB)
U32          LCD_AA_SetOrMask(U32 OrMask);
#else
U32          LCD_AA_SetAndMask(U32 AndMask);
#endif

/* Configuration */
int32_t  LCD_SetMaxNumColors(unsigned MaxNumColors);
int32_t  LCD_GetMaxNumColors(void);
void LCD__SetPaletteConversionHook(void (* pfPaletteConversionHook)(const LCD_LOGPALETTE * pLogPal));

/*********************************************************************
*
*       Optional support for rotation
*/
#if GUI_SUPPORT_ROTATION

typedef void tLCD_DrawBitmap(int32_t x0, int32_t y0, int32_t xsize, int32_t ysize,
                             int32_t xMul, int32_t yMul, int32_t BitsPerPixel, int32_t BytesPerLine,
                             const U8 * pPixel, const void * pTrans);
typedef void tRect2TextRect (LCD_RECT * pRect);

struct tLCD_APIList_struct {
  tLCD_DrawBitmap   * pfDrawBitmap;
  tRect2TextRect    * pfRect2TextRect;
  tRect2TextRect    * pfTransformRect;
};

typedef struct tLCD_APIList_struct tLCD_APIList;

extern tLCD_APIList LCD_APIListCCW;
extern tLCD_APIList LCD_APIListCW;
extern tLCD_APIList LCD_APIList180;

#define GUI_ROTATION   tLCD_APIList
#define GUI_ROTATE_CCW &LCD_APIListCCW
#define GUI_ROTATE_CW  &LCD_APIListCW
#define GUI_ROTATE_180 &LCD_APIList180
#define GUI_ROTATE_0   0

tLCD_SetPixelAA * LCD__GetPfSetPixel(int32_t BitsPerPixel);

#endif

/*********************************************************************
*
*       Physical color access, internally used only
*/
void LCD__SetPhysColor(U8 Pos, LCD_COLOR Color);

/*********************************************************************
*
*       Cache control
*/
#define LCD_CC_UNLOCK (0)    /* Default mode: Cache is transparent */
#define LCD_CC_LOCK   (1)    /* Cache is locked, no write operations */
#define LCD_CC_FLUSH  (2)    /* Flush cache, do not change mode */

int32_t LCD_ControlCache  (int32_t Cmd);
int32_t LCD_ControlCacheEx(int32_t LayerIndex, int32_t Cmd);

/*********************************************************************
*
*       Color conversion
*/
LCD_PIXELINDEX   LCD_Color2Index     (LCD_COLOR Color);
LCD_COLOR        LCD_Index2Color     (int32_t Index);
LCD_COLOR        LCD_Index2ColorEx   (int32_t i, unsigned LayerIndex);

/*********************************************************************
*
*       LCD_X_...
*/
unsigned char LCD_X_Read00(void);
unsigned char LCD_X_Read01(void);
void LCD_X_Write00 (unsigned char c);
void LCD_X_Write01 (unsigned char c);
void LCD_X_WriteM01(unsigned char * pData, int32_t NumBytes);

#if defined(__cplusplus)
  }
#endif

#endif /* LCD_H */

/*************************** End of file ****************************/
