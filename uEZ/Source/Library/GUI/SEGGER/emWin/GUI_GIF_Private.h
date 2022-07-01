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
File        : GUI_GIF_Private.h
Purpose     : Private header file for GUI_GIF... functions
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_GIF_PRIVATE_H
#define GUI_GIF_PRIVATE_H

#include "GUI_Private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MAX_NUM_LWZ_BITS 12

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/* Context structure */
typedef struct {
  /* Required for getting input */
  unsigned            NumBytesInBuffer;     /* Remaining bytes in buffer */
  const U8          * pBuffer;              /* Pointer into buffer for reading data */
  GUI_GET_DATA_FUNC * pfGetData;            /* Function pointer */
  void              * pParam;               /* Parameter pointer passed to function */
  U32                 Off;                  /* Data pointer */
  /* Decompression data */
  U8    aBuffer[258];                       /* Input buffer for data block */
  short aCode  [(1 << MAX_NUM_LWZ_BITS)];   /* This array stores the LZW codes for the compressed strings */
  U8    aPrefix[(1 << MAX_NUM_LWZ_BITS)];   /* Prefix character of the LZW code. */
  U8    aDecompBuffer[3000];                /* Decompression buffer. The higher the compression, the more bytes are needed in the buffer. */
  U8 *  sp;                                 /* Pointer into the decompression buffer */
  int32_t   CurBit;
  int32_t   LastBit;
  int32_t   GetDone;
  int32_t   LastByte;
  int32_t   ReturnClear;
  int32_t   CodeSize;
  int32_t   SetCodeSize;
  int32_t   MaxCode;
  int32_t   MaxCodeSize;
  int32_t   ClearCode;
  int32_t   EndCode;
  int32_t   FirstCode;
  int32_t   OldCode;
  /* Palette buffer */
  GUI_COLOR aColorTable[256];
} GUI_GIF_CONTEXT;

typedef struct {
  int32_t XPos;
  int32_t YPos;
  int32_t XSize;
  int32_t YSize;
  int32_t Flags;
  int32_t NumColors;
} IMAGE_DESCRIPTOR;

/* Default parameter structure for reading data from memory */
typedef struct {
  const U8 * pFileData;
  U32   FileSize;
} GUI_GIF_PARAM;

typedef int32_t  DRAW_FROM_DATABLOCK(GUI_GIF_CONTEXT * pContext, IMAGE_DESCRIPTOR * pDescriptor, int32_t x0, int32_t y0, int32_t Transparency, int32_t Disposal, int32_t Num, int32_t Denom);
typedef void CLEAR_UNUSED_PIXELS(int32_t x0, int32_t y0, IMAGE_DESCRIPTOR * pDescriptor, GUI_GIF_IMAGE_INFO * pInfo, int32_t Num, int32_t Denom);

/*********************************************************************
*
*       Private data
*
**********************************************************************
*/
extern const int32_t GUI_GIF__aInterlaceOffset[4];
extern const int32_t GUI_GIF__aInterlaceYPos[4];

/*********************************************************************
*
*       Interface
*
**********************************************************************
*/
int32_t  GUI_GIF__ReadData(GUI_GIF_CONTEXT * pContext, unsigned NumBytes, const U8 ** ppData, unsigned StartOfFile);
int32_t  GUI_GIF__GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off);
int32_t  GUI_GIF__DrawFromFilePointer(GUI_GIF_CONTEXT * pContext, int32_t x0, int32_t y0, int32_t Index, int32_t Num, int32_t Denom, DRAW_FROM_DATABLOCK pfDrawFromDataBlock, CLEAR_UNUSED_PIXELS pfClearUnusedPixels);
void GUI_GIF__InitLZW(GUI_GIF_CONTEXT * pContext, int32_t InputCodeSize);
int32_t  GUI_GIF__GetNextByte(GUI_GIF_CONTEXT * pContext);

#endif /* GUI_GIF_PRIVATE_H */
