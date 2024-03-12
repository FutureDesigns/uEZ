/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
* C-file generated by                                                *
*                                                                    *
*        Bitmap Converter (NXP) for emWin V5.48d.                    *
*        Compiled Sep 13 2018, 15:45:46                              *
*                                                                    *
*        (c) 1998 - 2018 Segger Microcontroller GmbH                 *
*                                                                    *
**********************************************************************
*                                                                    *
* Source file: logo                                                  *
* Dimensions:  160 * 72                                              *
* NumColors:   9                                                     *
*                                                                    *
**********************************************************************
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP bmlogo;

/*********************************************************************
*
*       Palette
*
*  Description
*    The following are the entries of the palette table.
*    The entries are stored as a 32-bit values of which 24 bits are
*    actually used according to the following bit mask: 0xBBGGRR
*
*    The lower   8 bits represent the Red   component.
*    The middle  8 bits represent the Green component.
*    The highest 8 bits represent the Blue  component.
*/
static GUI_CONST_STORAGE GUI_COLOR _Colorslogo[] = {
#if (GUI_USE_ARGB == 0)
  0x000000, 0x33268C, 0x36298D, 0x3A2D90,
  0x3F3293, 0x201B46, 0x392F80, 0x31296F,
  0x0F0D21
#else
  0xFF000000, 0xFF8C2633, 0xFF8D2936, 0xFF902D3A,
  0xFF93323F, 0xFF461B20, 0xFF802F39, 0xFF6F2931,
  0xFF210D0F
#endif

};

static GUI_CONST_STORAGE GUI_LOGPALETTE _Pallogo = {
  9,  // Number of entries
  0,  // No transparency
  &_Colorslogo[0]
};

static GUI_CONST_STORAGE unsigned char _aclogo[] = {
  /* RLE: 1325 Pixels @ 000,000 */ 254, 0x00, 254, 0x00, 254, 0x00, 254, 0x00, 254, 0x00, 55, 0x00, 
  /* RLE: 001 Pixels @ 045,008 */ 1, 0x06, 
  /* RLE: 024 Pixels @ 046,008 */ 24, 0x01, 
  /* RLE: 009 Pixels @ 070,008 */ 9, 0x00, 
  /* RLE: 031 Pixels @ 079,008 */ 31, 0x01, 
  /* RLE: 001 Pixels @ 110,008 */ 1, 0x03, 
  /* RLE: 029 Pixels @ 111,008 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 140,008 */ 12, 0x01, 
  /* RLE: 048 Pixels @ 152,008 */ 48, 0x00, 
  /* RLE: 001 Pixels @ 040,009 */ 1, 0x02, 
  /* RLE: 029 Pixels @ 041,009 */ 29, 0x01, 
  /* RLE: 009 Pixels @ 070,009 */ 9, 0x00, 
  /* RLE: 036 Pixels @ 079,009 */ 36, 0x01, 
  /* RLE: 025 Pixels @ 115,009 */ 25, 0x00, 
  /* RLE: 011 Pixels @ 140,009 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 151,009 */ 1, 0x04, 
  /* RLE: 044 Pixels @ 152,009 */ 44, 0x00, 
  /* RLE: 001 Pixels @ 036,010 */ 1, 0x05, 
  /* RLE: 033 Pixels @ 037,010 */ 33, 0x01, 
  /* RLE: 009 Pixels @ 070,010 */ 9, 0x00, 
  /* RLE: 038 Pixels @ 079,010 */ 38, 0x01, 
  /* RLE: 001 Pixels @ 117,010 */ 1, 0x05, 
  /* RLE: 022 Pixels @ 118,010 */ 22, 0x00, 
  /* RLE: 011 Pixels @ 140,010 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 151,010 */ 43, 0x00, 
  /* RLE: 001 Pixels @ 034,011 */ 1, 0x02, 
  /* RLE: 034 Pixels @ 035,011 */ 34, 0x01, 
  /* RLE: 009 Pixels @ 069,011 */ 9, 0x00, 
  /* RLE: 041 Pixels @ 078,011 */ 41, 0x01, 
  /* RLE: 020 Pixels @ 119,011 */ 20, 0x00, 
  /* RLE: 012 Pixels @ 139,011 */ 12, 0x01, 
  /* RLE: 041 Pixels @ 151,011 */ 41, 0x00, 
  /* RLE: 001 Pixels @ 032,012 */ 1, 0x02, 
  /* RLE: 036 Pixels @ 033,012 */ 36, 0x01, 
  /* RLE: 009 Pixels @ 069,012 */ 9, 0x00, 
  /* RLE: 042 Pixels @ 078,012 */ 42, 0x01, 
  /* RLE: 019 Pixels @ 120,012 */ 19, 0x00, 
  /* RLE: 011 Pixels @ 139,012 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 150,012 */ 1, 0x05, 
  /* RLE: 039 Pixels @ 151,012 */ 39, 0x00, 
  /* RLE: 039 Pixels @ 030,013 */ 39, 0x01, 
  /* RLE: 008 Pixels @ 069,013 */ 8, 0x00, 
  /* RLE: 001 Pixels @ 077,013 */ 1, 0x05, 
  /* RLE: 043 Pixels @ 078,013 */ 43, 0x01, 
  /* RLE: 001 Pixels @ 121,013 */ 1, 0x06, 
  /* RLE: 016 Pixels @ 122,013 */ 16, 0x00, 
  /* RLE: 001 Pixels @ 138,013 */ 1, 0x08, 
  /* RLE: 011 Pixels @ 139,013 */ 11, 0x01, 
  /* RLE: 039 Pixels @ 150,013 */ 39, 0x00, 
  /* RLE: 039 Pixels @ 029,014 */ 39, 0x01, 
  /* RLE: 009 Pixels @ 068,014 */ 9, 0x00, 
  /* RLE: 045 Pixels @ 077,014 */ 45, 0x01, 
  /* RLE: 016 Pixels @ 122,014 */ 16, 0x00, 
  /* RLE: 012 Pixels @ 138,014 */ 12, 0x01, 
  /* RLE: 038 Pixels @ 150,014 */ 38, 0x00, 
  /* RLE: 040 Pixels @ 028,015 */ 40, 0x01, 
  /* RLE: 009 Pixels @ 068,015 */ 9, 0x00, 
  /* RLE: 046 Pixels @ 077,015 */ 46, 0x01, 
  /* RLE: 015 Pixels @ 123,015 */ 15, 0x00, 
  /* RLE: 011 Pixels @ 138,015 */ 11, 0x01, 
  /* RLE: 037 Pixels @ 149,015 */ 37, 0x00, 
  /* RLE: 001 Pixels @ 026,016 */ 1, 0x04, 
  /* RLE: 041 Pixels @ 027,016 */ 41, 0x01, 
  /* RLE: 008 Pixels @ 068,016 */ 8, 0x00, 
  /* RLE: 001 Pixels @ 076,016 */ 1, 0x04, 
  /* RLE: 046 Pixels @ 077,016 */ 46, 0x01, 
  /* RLE: 014 Pixels @ 123,016 */ 14, 0x00, 
  /* RLE: 001 Pixels @ 137,016 */ 1, 0x04, 
  /* RLE: 011 Pixels @ 138,016 */ 11, 0x01, 
  /* RLE: 036 Pixels @ 149,016 */ 36, 0x00, 
  /* RLE: 001 Pixels @ 025,017 */ 1, 0x06, 
  /* RLE: 019 Pixels @ 026,017 */ 19, 0x01, 
  /* RLE: 001 Pixels @ 045,017 */ 1, 0x05, 
  /* RLE: 030 Pixels @ 046,017 */ 30, 0x00, 
  /* RLE: 011 Pixels @ 076,017 */ 11, 0x01, 
  /* RLE: 017 Pixels @ 087,017 */ 17, 0x00, 
  /* RLE: 001 Pixels @ 104,017 */ 1, 0x04, 
  /* RLE: 019 Pixels @ 105,017 */ 19, 0x01, 
  /* RLE: 013 Pixels @ 124,017 */ 13, 0x00, 
  /* RLE: 012 Pixels @ 137,017 */ 12, 0x01, 
  /* RLE: 036 Pixels @ 149,017 */ 36, 0x00, 
  /* RLE: 015 Pixels @ 025,018 */ 15, 0x01, 
  /* RLE: 001 Pixels @ 040,018 */ 1, 0x05, 
  /* RLE: 035 Pixels @ 041,018 */ 35, 0x00, 
  /* RLE: 011 Pixels @ 076,018 */ 11, 0x01, 
  /* RLE: 021 Pixels @ 087,018 */ 21, 0x00, 
  /* RLE: 001 Pixels @ 108,018 */ 1, 0x02, 
  /* RLE: 015 Pixels @ 109,018 */ 15, 0x01, 
  /* RLE: 013 Pixels @ 124,018 */ 13, 0x00, 
  /* RLE: 011 Pixels @ 137,018 */ 11, 0x01, 
  /* RLE: 036 Pixels @ 148,018 */ 36, 0x00, 
  /* RLE: 013 Pixels @ 024,019 */ 13, 0x01, 
  /* RLE: 001 Pixels @ 037,019 */ 1, 0x04, 
  /* RLE: 037 Pixels @ 038,019 */ 37, 0x00, 
  /* RLE: 001 Pixels @ 075,019 */ 1, 0x03, 
  /* RLE: 010 Pixels @ 076,019 */ 10, 0x01, 
  /* RLE: 024 Pixels @ 086,019 */ 24, 0x00, 
  /* RLE: 001 Pixels @ 110,019 */ 1, 0x02, 
  /* RLE: 013 Pixels @ 111,019 */ 13, 0x01, 
  /* RLE: 012 Pixels @ 124,019 */ 12, 0x00, 
  /* RLE: 001 Pixels @ 136,019 */ 1, 0x03, 
  /* RLE: 011 Pixels @ 137,019 */ 11, 0x01, 
  /* RLE: 035 Pixels @ 148,019 */ 35, 0x00, 
  /* RLE: 012 Pixels @ 023,020 */ 12, 0x01, 
  /* RLE: 001 Pixels @ 035,020 */ 1, 0x02, 
  /* RLE: 039 Pixels @ 036,020 */ 39, 0x00, 
  /* RLE: 011 Pixels @ 075,020 */ 11, 0x01, 
  /* RLE: 025 Pixels @ 086,020 */ 25, 0x00, 
  /* RLE: 013 Pixels @ 111,020 */ 13, 0x01, 
  /* RLE: 012 Pixels @ 124,020 */ 12, 0x00, 
  /* RLE: 011 Pixels @ 136,020 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 147,020 */ 1, 0x02, 
  /* RLE: 034 Pixels @ 148,020 */ 34, 0x00, 
  /* RLE: 012 Pixels @ 022,021 */ 12, 0x01, 
  /* RLE: 001 Pixels @ 034,021 */ 1, 0x03, 
  /* RLE: 040 Pixels @ 035,021 */ 40, 0x00, 
  /* RLE: 011 Pixels @ 075,021 */ 11, 0x01, 
  /* RLE: 026 Pixels @ 086,021 */ 26, 0x00, 
  /* RLE: 012 Pixels @ 112,021 */ 12, 0x01, 
  /* RLE: 012 Pixels @ 124,021 */ 12, 0x00, 
  /* RLE: 011 Pixels @ 136,021 */ 11, 0x01, 
  /* RLE: 035 Pixels @ 147,021 */ 35, 0x00, 
  /* RLE: 011 Pixels @ 022,022 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 033,022 */ 1, 0x03, 
  /* RLE: 040 Pixels @ 034,022 */ 40, 0x00, 
  /* RLE: 001 Pixels @ 074,022 */ 1, 0x03, 
  /* RLE: 011 Pixels @ 075,022 */ 11, 0x01, 
  /* RLE: 026 Pixels @ 086,022 */ 26, 0x00, 
  /* RLE: 012 Pixels @ 112,022 */ 12, 0x01, 
  /* RLE: 011 Pixels @ 124,022 */ 11, 0x00, 
  /* RLE: 001 Pixels @ 135,022 */ 1, 0x03, 
  /* RLE: 011 Pixels @ 136,022 */ 11, 0x01, 
  /* RLE: 034 Pixels @ 147,022 */ 34, 0x00, 
  /* RLE: 012 Pixels @ 021,023 */ 12, 0x01, 
  /* RLE: 041 Pixels @ 033,023 */ 41, 0x00, 
  /* RLE: 011 Pixels @ 074,023 */ 11, 0x01, 
  /* RLE: 027 Pixels @ 085,023 */ 27, 0x00, 
  /* RLE: 012 Pixels @ 112,023 */ 12, 0x01, 
  /* RLE: 011 Pixels @ 124,023 */ 11, 0x00, 
  /* RLE: 012 Pixels @ 135,023 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 147,023 */ 33, 0x00, 
  /* RLE: 001 Pixels @ 020,024 */ 1, 0x06, 
  /* RLE: 011 Pixels @ 021,024 */ 11, 0x01, 
  /* RLE: 042 Pixels @ 032,024 */ 42, 0x00, 
  /* RLE: 011 Pixels @ 074,024 */ 11, 0x01, 
  /* RLE: 027 Pixels @ 085,024 */ 27, 0x00, 
  /* RLE: 012 Pixels @ 112,024 */ 12, 0x01, 
  /* RLE: 011 Pixels @ 124,024 */ 11, 0x00, 
  /* RLE: 011 Pixels @ 135,024 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 146,024 */ 1, 0x03, 
  /* RLE: 033 Pixels @ 147,024 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 020,025 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 031,025 */ 43, 0x00, 
  /* RLE: 010 Pixels @ 074,025 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 084,025 */ 1, 0x02, 
  /* RLE: 027 Pixels @ 085,025 */ 27, 0x00, 
  /* RLE: 012 Pixels @ 112,025 */ 12, 0x01, 
  /* RLE: 011 Pixels @ 124,025 */ 11, 0x00, 
  /* RLE: 011 Pixels @ 135,025 */ 11, 0x01, 
  /* RLE: 034 Pixels @ 146,025 */ 34, 0x00, 
  /* RLE: 011 Pixels @ 020,026 */ 11, 0x01, 
  /* RLE: 042 Pixels @ 031,026 */ 42, 0x00, 
  /* RLE: 011 Pixels @ 073,026 */ 11, 0x01, 
  /* RLE: 028 Pixels @ 084,026 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 112,026 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 124,026 */ 10, 0x00, 
  /* RLE: 012 Pixels @ 134,026 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 146,026 */ 33, 0x00, 
  /* RLE: 001 Pixels @ 019,027 */ 1, 0x02, 
  /* RLE: 010 Pixels @ 020,027 */ 10, 0x01, 
  /* RLE: 043 Pixels @ 030,027 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 073,027 */ 11, 0x01, 
  /* RLE: 028 Pixels @ 084,027 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 112,027 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 124,027 */ 10, 0x00, 
  /* RLE: 011 Pixels @ 134,027 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 145,027 */ 1, 0x04, 
  /* RLE: 033 Pixels @ 146,027 */ 33, 0x00, 
  /* RLE: 010 Pixels @ 019,028 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 029,028 */ 1, 0x02, 
  /* RLE: 043 Pixels @ 030,028 */ 43, 0x00, 
  /* RLE: 010 Pixels @ 073,028 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 083,028 */ 1, 0x03, 
  /* RLE: 028 Pixels @ 084,028 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 112,028 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 124,028 */ 10, 0x00, 
  /* RLE: 011 Pixels @ 134,028 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 145,028 */ 33, 0x00, 
  /* RLE: 001 Pixels @ 018,029 */ 1, 0x02, 
  /* RLE: 010 Pixels @ 019,029 */ 10, 0x01, 
  /* RLE: 043 Pixels @ 029,029 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 072,029 */ 11, 0x01, 
  /* RLE: 029 Pixels @ 083,029 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 112,029 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 124,029 */ 9, 0x00, 
  /* RLE: 012 Pixels @ 133,029 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 145,029 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 018,030 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 029,030 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 072,030 */ 11, 0x01, 
  /* RLE: 028 Pixels @ 083,030 */ 28, 0x00, 
  /* RLE: 001 Pixels @ 111,030 */ 1, 0x03, 
  /* RLE: 011 Pixels @ 112,030 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 123,030 */ 1, 0x05, 
  /* RLE: 009 Pixels @ 124,030 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 133,030 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 144,030 */ 1, 0x06, 
  /* RLE: 032 Pixels @ 145,030 */ 32, 0x00, 
  /* RLE: 001 Pixels @ 017,031 */ 1, 0x05, 
  /* RLE: 010 Pixels @ 018,031 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 028,031 */ 1, 0x02, 
  /* RLE: 042 Pixels @ 029,031 */ 42, 0x00, 
  /* RLE: 001 Pixels @ 071,031 */ 1, 0x06, 
  /* RLE: 010 Pixels @ 072,031 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 082,031 */ 1, 0x03, 
  /* RLE: 028 Pixels @ 083,031 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 111,031 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 123,031 */ 9, 0x00, 
  /* RLE: 001 Pixels @ 132,031 */ 1, 0x06, 
  /* RLE: 011 Pixels @ 133,031 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 144,031 */ 33, 0x00, 
  /* RLE: 045 Pixels @ 017,032 */ 45, 0x01, 
  /* RLE: 009 Pixels @ 062,032 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 071,032 */ 11, 0x01, 
  /* RLE: 029 Pixels @ 082,032 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 111,032 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 123,032 */ 9, 0x00, 
  /* RLE: 012 Pixels @ 132,032 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 144,032 */ 33, 0x00, 
  /* RLE: 045 Pixels @ 017,033 */ 45, 0x01, 
  /* RLE: 009 Pixels @ 062,033 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 071,033 */ 11, 0x01, 
  /* RLE: 029 Pixels @ 082,033 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 111,033 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 123,033 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 132,033 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 143,033 */ 33, 0x00, 
  /* RLE: 001 Pixels @ 016,034 */ 1, 0x02, 
  /* RLE: 045 Pixels @ 017,034 */ 45, 0x01, 
  /* RLE: 008 Pixels @ 062,034 */ 8, 0x00, 
  /* RLE: 001 Pixels @ 070,034 */ 1, 0x04, 
  /* RLE: 010 Pixels @ 071,034 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 081,034 */ 1, 0x04, 
  /* RLE: 028 Pixels @ 082,034 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 110,034 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 122,034 */ 9, 0x00, 
  /* RLE: 001 Pixels @ 131,034 */ 1, 0x04, 
  /* RLE: 011 Pixels @ 132,034 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 143,034 */ 33, 0x00, 
  /* RLE: 045 Pixels @ 016,035 */ 45, 0x01, 
  /* RLE: 009 Pixels @ 061,035 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 070,035 */ 11, 0x01, 
  /* RLE: 029 Pixels @ 081,035 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 110,035 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 122,035 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 131,035 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 142,035 */ 1, 0x02, 
  /* RLE: 033 Pixels @ 143,035 */ 33, 0x00, 
  /* RLE: 045 Pixels @ 016,036 */ 45, 0x01, 
  /* RLE: 009 Pixels @ 061,036 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 070,036 */ 11, 0x01, 
  /* RLE: 029 Pixels @ 081,036 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 110,036 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 122,036 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 131,036 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 142,036 */ 33, 0x00, 
  /* RLE: 045 Pixels @ 015,037 */ 45, 0x01, 
  /* RLE: 001 Pixels @ 060,037 */ 1, 0x03, 
  /* RLE: 008 Pixels @ 061,037 */ 8, 0x00, 
  /* RLE: 001 Pixels @ 069,037 */ 1, 0x03, 
  /* RLE: 010 Pixels @ 070,037 */ 10, 0x01, 
  /* RLE: 029 Pixels @ 080,037 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 109,037 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 121,037 */ 9, 0x00, 
  /* RLE: 001 Pixels @ 130,037 */ 1, 0x03, 
  /* RLE: 011 Pixels @ 131,037 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 142,037 */ 33, 0x00, 
  /* RLE: 045 Pixels @ 015,038 */ 45, 0x01, 
  /* RLE: 009 Pixels @ 060,038 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 069,038 */ 11, 0x01, 
  /* RLE: 029 Pixels @ 080,038 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 109,038 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 121,038 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 130,038 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 141,038 */ 1, 0x02, 
  /* RLE: 032 Pixels @ 142,038 */ 32, 0x00, 
  /* RLE: 001 Pixels @ 014,039 */ 1, 0x05, 
  /* RLE: 045 Pixels @ 015,039 */ 45, 0x01, 
  /* RLE: 009 Pixels @ 060,039 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 069,039 */ 11, 0x01, 
  /* RLE: 028 Pixels @ 080,039 */ 28, 0x00, 
  /* RLE: 001 Pixels @ 108,039 */ 1, 0x03, 
  /* RLE: 011 Pixels @ 109,039 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 120,039 */ 1, 0x02, 
  /* RLE: 009 Pixels @ 121,039 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 130,039 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 141,039 */ 33, 0x00, 
  /* RLE: 045 Pixels @ 014,040 */ 45, 0x01, 
  /* RLE: 001 Pixels @ 059,040 */ 1, 0x03, 
  /* RLE: 008 Pixels @ 060,040 */ 8, 0x00, 
  /* RLE: 001 Pixels @ 068,040 */ 1, 0x02, 
  /* RLE: 010 Pixels @ 069,040 */ 10, 0x01, 
  /* RLE: 029 Pixels @ 079,040 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 108,040 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 120,040 */ 9, 0x00, 
  /* RLE: 001 Pixels @ 129,040 */ 1, 0x02, 
  /* RLE: 011 Pixels @ 130,040 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 141,040 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 014,041 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 025,041 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 068,041 */ 11, 0x01, 
  /* RLE: 029 Pixels @ 079,041 */ 29, 0x00, 
  /* RLE: 012 Pixels @ 108,041 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 120,041 */ 9, 0x00, 
  /* RLE: 011 Pixels @ 129,041 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 140,041 */ 1, 0x03, 
  /* RLE: 033 Pixels @ 141,041 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 014,042 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 025,042 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 068,042 */ 11, 0x01, 
  /* RLE: 028 Pixels @ 079,042 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 107,042 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 119,042 */ 10, 0x00, 
  /* RLE: 011 Pixels @ 129,042 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 140,042 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 013,043 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 024,043 */ 1, 0x05, 
  /* RLE: 042 Pixels @ 025,043 */ 42, 0x00, 
  /* RLE: 001 Pixels @ 067,043 */ 1, 0x02, 
  /* RLE: 010 Pixels @ 068,043 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 078,043 */ 1, 0x03, 
  /* RLE: 028 Pixels @ 079,043 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 107,043 */ 12, 0x01, 
  /* RLE: 009 Pixels @ 119,043 */ 9, 0x00, 
  /* RLE: 001 Pixels @ 128,043 */ 1, 0x02, 
  /* RLE: 011 Pixels @ 129,043 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 140,043 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 013,044 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 024,044 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 067,044 */ 11, 0x01, 
  /* RLE: 028 Pixels @ 078,044 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 106,044 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 118,044 */ 10, 0x00, 
  /* RLE: 012 Pixels @ 128,044 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 140,044 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 013,045 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 024,045 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 067,045 */ 11, 0x01, 
  /* RLE: 028 Pixels @ 078,045 */ 28, 0x00, 
  /* RLE: 012 Pixels @ 106,045 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 118,045 */ 10, 0x00, 
  /* RLE: 011 Pixels @ 128,045 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 139,045 */ 1, 0x05, 
  /* RLE: 032 Pixels @ 140,045 */ 32, 0x00, 
  /* RLE: 001 Pixels @ 012,046 */ 1, 0x03, 
  /* RLE: 010 Pixels @ 013,046 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 023,046 */ 1, 0x04, 
  /* RLE: 042 Pixels @ 024,046 */ 42, 0x00, 
  /* RLE: 011 Pixels @ 066,046 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 077,046 */ 1, 0x03, 
  /* RLE: 027 Pixels @ 078,046 */ 27, 0x00, 
  /* RLE: 012 Pixels @ 105,046 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 117,046 */ 10, 0x00, 
  /* RLE: 012 Pixels @ 127,046 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 139,046 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 012,047 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 023,047 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 066,047 */ 11, 0x01, 
  /* RLE: 027 Pixels @ 077,047 */ 27, 0x00, 
  /* RLE: 001 Pixels @ 104,047 */ 1, 0x03, 
  /* RLE: 012 Pixels @ 105,047 */ 12, 0x01, 
  /* RLE: 010 Pixels @ 117,047 */ 10, 0x00, 
  /* RLE: 012 Pixels @ 127,047 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 139,047 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 012,048 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 023,048 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 066,048 */ 11, 0x01, 
  /* RLE: 027 Pixels @ 077,048 */ 27, 0x00, 
  /* RLE: 012 Pixels @ 104,048 */ 12, 0x01, 
  /* RLE: 011 Pixels @ 116,048 */ 11, 0x00, 
  /* RLE: 011 Pixels @ 127,048 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 138,048 */ 33, 0x00, 
  /* RLE: 001 Pixels @ 011,049 */ 1, 0x02, 
  /* RLE: 010 Pixels @ 012,049 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 022,049 */ 1, 0x05, 
  /* RLE: 042 Pixels @ 023,049 */ 42, 0x00, 
  /* RLE: 001 Pixels @ 065,049 */ 1, 0x06, 
  /* RLE: 010 Pixels @ 066,049 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 076,049 */ 1, 0x04, 
  /* RLE: 026 Pixels @ 077,049 */ 26, 0x00, 
  /* RLE: 012 Pixels @ 103,049 */ 12, 0x01, 
  /* RLE: 001 Pixels @ 115,049 */ 1, 0x04, 
  /* RLE: 010 Pixels @ 116,049 */ 10, 0x00, 
  /* RLE: 001 Pixels @ 126,049 */ 1, 0x07, 
  /* RLE: 011 Pixels @ 127,049 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 138,049 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 011,050 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 022,050 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 065,050 */ 11, 0x01, 
  /* RLE: 026 Pixels @ 076,050 */ 26, 0x00, 
  /* RLE: 001 Pixels @ 102,050 */ 1, 0x02, 
  /* RLE: 012 Pixels @ 103,050 */ 12, 0x01, 
  /* RLE: 011 Pixels @ 115,050 */ 11, 0x00, 
  /* RLE: 012 Pixels @ 126,050 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 138,050 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 011,051 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 022,051 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 065,051 */ 11, 0x01, 
  /* RLE: 025 Pixels @ 076,051 */ 25, 0x00, 
  /* RLE: 013 Pixels @ 101,051 */ 13, 0x01, 
  /* RLE: 012 Pixels @ 114,051 */ 12, 0x00, 
  /* RLE: 011 Pixels @ 126,051 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 137,051 */ 33, 0x00, 
  /* RLE: 001 Pixels @ 010,052 */ 1, 0x02, 
  /* RLE: 010 Pixels @ 011,052 */ 10, 0x01, 
  /* RLE: 043 Pixels @ 021,052 */ 43, 0x00, 
  /* RLE: 001 Pixels @ 064,052 */ 1, 0x06, 
  /* RLE: 010 Pixels @ 065,052 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 075,052 */ 1, 0x06, 
  /* RLE: 024 Pixels @ 076,052 */ 24, 0x00, 
  /* RLE: 013 Pixels @ 100,052 */ 13, 0x01, 
  /* RLE: 012 Pixels @ 113,052 */ 12, 0x00, 
  /* RLE: 001 Pixels @ 125,052 */ 1, 0x07, 
  /* RLE: 011 Pixels @ 126,052 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 137,052 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 010,053 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 021,053 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 064,053 */ 11, 0x01, 
  /* RLE: 022 Pixels @ 075,053 */ 22, 0x00, 
  /* RLE: 001 Pixels @ 097,053 */ 1, 0x05, 
  /* RLE: 014 Pixels @ 098,053 */ 14, 0x01, 
  /* RLE: 013 Pixels @ 112,053 */ 13, 0x00, 
  /* RLE: 012 Pixels @ 125,053 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 137,053 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 010,054 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 021,054 */ 43, 0x00, 
  /* RLE: 011 Pixels @ 064,054 */ 11, 0x01, 
  /* RLE: 019 Pixels @ 075,054 */ 19, 0x00, 
  /* RLE: 001 Pixels @ 094,054 */ 1, 0x04, 
  /* RLE: 016 Pixels @ 095,054 */ 16, 0x01, 
  /* RLE: 014 Pixels @ 111,054 */ 14, 0x00, 
  /* RLE: 011 Pixels @ 125,054 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 136,054 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 009,055 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 020,055 */ 43, 0x00, 
  /* RLE: 001 Pixels @ 063,055 */ 1, 0x02, 
  /* RLE: 046 Pixels @ 064,055 */ 46, 0x01, 
  /* RLE: 014 Pixels @ 110,055 */ 14, 0x00, 
  /* RLE: 001 Pixels @ 124,055 */ 1, 0x02, 
  /* RLE: 011 Pixels @ 125,055 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 136,055 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 009,056 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 020,056 */ 43, 0x00, 
  /* RLE: 046 Pixels @ 063,056 */ 46, 0x01, 
  /* RLE: 015 Pixels @ 109,056 */ 15, 0x00, 
  /* RLE: 012 Pixels @ 124,056 */ 12, 0x01, 
  /* RLE: 032 Pixels @ 136,056 */ 32, 0x00, 
  /* RLE: 001 Pixels @ 008,057 */ 1, 0x05, 
  /* RLE: 010 Pixels @ 009,057 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 019,057 */ 1, 0x02, 
  /* RLE: 043 Pixels @ 020,057 */ 43, 0x00, 
  /* RLE: 045 Pixels @ 063,057 */ 45, 0x01, 
  /* RLE: 016 Pixels @ 108,057 */ 16, 0x00, 
  /* RLE: 011 Pixels @ 124,057 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 135,057 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 008,058 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 019,058 */ 43, 0x00, 
  /* RLE: 001 Pixels @ 062,058 */ 1, 0x02, 
  /* RLE: 043 Pixels @ 063,058 */ 43, 0x01, 
  /* RLE: 001 Pixels @ 106,058 */ 1, 0x02, 
  /* RLE: 016 Pixels @ 107,058 */ 16, 0x00, 
  /* RLE: 001 Pixels @ 123,058 */ 1, 0x02, 
  /* RLE: 011 Pixels @ 124,058 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 135,058 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 008,059 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 019,059 */ 43, 0x00, 
  /* RLE: 043 Pixels @ 062,059 */ 43, 0x01, 
  /* RLE: 018 Pixels @ 105,059 */ 18, 0x00, 
  /* RLE: 011 Pixels @ 123,059 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 134,059 */ 1, 0x03, 
  /* RLE: 032 Pixels @ 135,059 */ 32, 0x00, 
  /* RLE: 001 Pixels @ 007,060 */ 1, 0x06, 
  /* RLE: 010 Pixels @ 008,060 */ 10, 0x01, 
  /* RLE: 001 Pixels @ 018,060 */ 1, 0x03, 
  /* RLE: 043 Pixels @ 019,060 */ 43, 0x00, 
  /* RLE: 041 Pixels @ 062,060 */ 41, 0x01, 
  /* RLE: 020 Pixels @ 103,060 */ 20, 0x00, 
  /* RLE: 011 Pixels @ 123,060 */ 11, 0x01, 
  /* RLE: 033 Pixels @ 134,060 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 007,061 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 018,061 */ 43, 0x00, 
  /* RLE: 040 Pixels @ 061,061 */ 40, 0x01, 
  /* RLE: 021 Pixels @ 101,061 */ 21, 0x00, 
  /* RLE: 012 Pixels @ 122,061 */ 12, 0x01, 
  /* RLE: 033 Pixels @ 134,061 */ 33, 0x00, 
  /* RLE: 011 Pixels @ 007,062 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 018,062 */ 43, 0x00, 
  /* RLE: 038 Pixels @ 061,062 */ 38, 0x01, 
  /* RLE: 023 Pixels @ 099,062 */ 23, 0x00, 
  /* RLE: 011 Pixels @ 122,062 */ 11, 0x01, 
  /* RLE: 001 Pixels @ 133,062 */ 1, 0x03, 
  /* RLE: 032 Pixels @ 134,062 */ 32, 0x00, 
  /* RLE: 001 Pixels @ 006,063 */ 1, 0x04, 
  /* RLE: 011 Pixels @ 007,063 */ 11, 0x01, 
  /* RLE: 043 Pixels @ 018,063 */ 43, 0x00, 
  /* RLE: 034 Pixels @ 061,063 */ 34, 0x01, 
  /* RLE: 001 Pixels @ 095,063 */ 1, 0x04, 
  /* RLE: 026 Pixels @ 096,063 */ 26, 0x00, 
  /* RLE: 011 Pixels @ 122,063 */ 11, 0x01, 
  /* RLE: 1307 Pixels @ 133,063 */ 254, 0x00, 254, 0x00, 254, 0x00, 254, 0x00, 254, 0x00, 37, 0x00, 
  0
};  // 1040 bytes for 11520 pixels

GUI_CONST_STORAGE GUI_BITMAP bmlogo = {
  160, // xSize
  72, // ySize
  160, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  _aclogo,  // Pointer to picture data (indices)
  &_Pallogo,  // Pointer to palette
  GUI_DRAW_RLE8
};

/*************************** End of file ****************************/