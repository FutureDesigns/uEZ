/*********************************************************************
*            (c) 1998 - 2023 SEGGER Microcontroller GmbH             *
*        Solutions for real time microcontroller applications        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
* C-file generated by                                                *
*                                                                    *
*        Bitmap Converter (NXP) for emWin V6.32.                     *
*        Compiled Feb 16 2023, 09:23:17                              *
*                                                                    *
*        (c) 1998 - 2023 SEGGER Microcontroller GmbH                 *
*                                                                    *
**********************************************************************
*                                                                    *
* Source file: mqttLogo64                                            *
* Dimensions:  64 * 64                                               *
* NumColors:   185                                                   *
* NumBytes:    2574                                                  *
**********************************************************************
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP bmmqttLogo64;

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
static GUI_CONST_STORAGE GUI_COLOR _ColorsmqttLogo64[] = {
#if (GUI_USE_ARGB == 0)
  0x000000, 0x670068, 0xFAFDFB, 0xF7FAF8,
  0xF9FCFA, 0x0A0D0B, 0x52024E, 0x660067,
  0xF8FBF9, 0xF3F7F4, 0xF6F9F6, 0x676A68,
  0xBEC1BF, 0xD4D7D5, 0x1E001E, 0x656765,
  0x949795, 0x060004, 0x260029, 0x393B39,
  0x141614, 0x9C9F9D, 0xA8ABA9, 0x363937,
  0x6A1B6B, 0xC1C4C1, 0x202321, 0x690C66,
  0x474948, 0x555856, 0xACAFAD, 0x681969,
  0x282B29, 0xB5A7B6, 0xB1B4B2, 0xEAE5E8,
  0x303331, 0x6B1169, 0x6E246C, 0xA0A3A1,
  0xCAC4CE, 0xEFF2F0, 0x1A1C1B, 0x242725,
  0x6E2D6F, 0x6F2E70, 0x707371, 0x8B8E8C,
  0xA994A8, 0xAB96AA, 0xB8BCB9, 0xEBEFEC,
  0x6A0F68, 0x6D2B6D, 0x733570, 0x7E507D,
  0x606361, 0x616462, 0x717472, 0x7A7D7B,
  0x8F718F, 0x9E87A1, 0xB3A5B4, 0xBABDBB,
  0xBBBEBC, 0xCEC8D2, 0xDBDEDC, 0xDEDADD,
  0xE8EBE9, 0xEDE9EC, 0xF0F3F1, 0x120113,
  0x342935, 0x702F71, 0x743D74, 0x454846,
  0x494B49, 0x535553, 0x757876, 0x7F8280,
  0x896587, 0x957795, 0x838684, 0x939694,
  0x989C99, 0xA18AA4, 0xA5A8A6, 0xB8AAB9,
  0xC6C9C7, 0xD1CBD5, 0xD4CED8, 0xD2D5D3,
  0xD6DAD7, 0xD8DCD9, 0xE5E0E3, 0xE8E3E6,
  0x230021, 0x330830, 0x680069, 0x763F76,
  0x585B59, 0x7B4777, 0x794279, 0x78497C,
  0x7A4B7E, 0x636663, 0x6A6D6B, 0x7D807D,
  0x815A83, 0x8B6689, 0x917391, 0x9B7F97,
  0x919492, 0x969997, 0xA690A4, 0xA4A7A5,
  0xAAADAB, 0xAFB3B0, 0xBAACBC, 0xBDAFBE,
  0xC5B9C1, 0xCCD0CD, 0xD1C5CD, 0xD6D0DA,
  0xDFE2E0, 0xE0DBDE, 0xE3DFE2, 0xE7E2E5,
  0xE7EAE8, 0xECE7EA, 0xEEEAED, 0xEDF0ED,
  0xEEF1EF, 0x170017, 0x2C002D, 0x2C2E2C,
  0x3E333F, 0x560056, 0x61005E, 0x630260,
  0x743671, 0x753772, 0x47414A, 0x515452,
  0x5E605E, 0x7B4C7F, 0x757173, 0x787B79,
  0x7B7E7C, 0x80537F, 0x815480, 0x845D86,
  0x866088, 0x8D688B, 0x947694, 0x987A98,
  0x808381, 0x858886, 0x898C89, 0x9D8199,
  0x9E829A, 0x9C85A0, 0xB1A3B2, 0xB9ABBB,
  0xBCAEBD, 0xB5B8B6, 0xBFC3C0, 0xC0B2C1,
  0xC1B3C3, 0xC3B5C4, 0xC9BDC5, 0xCBBFC7,
  0xC7CAC8, 0xCDC1C9, 0xCFD2CF, 0xD0D3D1,
  0xD8D2DC, 0xDCD5DF, 0xE1E5E2, 0xE2E6E3,
  0xE9E4E7, 0xEAEEEB, 0xF0ECEF, 0xF5F0F3,
  0xFBFFFC
#else
  0xFF000000, 0xFF680067, 0xFFFBFDFA, 0xFFF8FAF7,
  0xFFFAFCF9, 0xFF0B0D0A, 0xFF4E0252, 0xFF670066,
  0xFFF9FBF8, 0xFFF4F7F3, 0xFFF6F9F6, 0xFF686A67,
  0xFFBFC1BE, 0xFFD5D7D4, 0xFF1E001E, 0xFF656765,
  0xFF959794, 0xFF040006, 0xFF290026, 0xFF393B39,
  0xFF141614, 0xFF9D9F9C, 0xFFA9ABA8, 0xFF373936,
  0xFF6B1B6A, 0xFFC1C4C1, 0xFF212320, 0xFF660C69,
  0xFF484947, 0xFF565855, 0xFFADAFAC, 0xFF691968,
  0xFF292B28, 0xFFB6A7B5, 0xFFB2B4B1, 0xFFE8E5EA,
  0xFF313330, 0xFF69116B, 0xFF6C246E, 0xFFA1A3A0,
  0xFFCEC4CA, 0xFFF0F2EF, 0xFF1B1C1A, 0xFF252724,
  0xFF6F2D6E, 0xFF702E6F, 0xFF717370, 0xFF8C8E8B,
  0xFFA894A9, 0xFFAA96AB, 0xFFB9BCB8, 0xFFECEFEB,
  0xFF680F6A, 0xFF6D2B6D, 0xFF703573, 0xFF7D507E,
  0xFF616360, 0xFF626461, 0xFF727471, 0xFF7B7D7A,
  0xFF8F718F, 0xFFA1879E, 0xFFB4A5B3, 0xFFBBBDBA,
  0xFFBCBEBB, 0xFFD2C8CE, 0xFFDCDEDB, 0xFFDDDADE,
  0xFFE9EBE8, 0xFFECE9ED, 0xFFF1F3F0, 0xFF130112,
  0xFF352934, 0xFF712F70, 0xFF743D74, 0xFF464845,
  0xFF494B49, 0xFF535553, 0xFF767875, 0xFF80827F,
  0xFF876589, 0xFF957795, 0xFF848683, 0xFF949693,
  0xFF999C98, 0xFFA48AA1, 0xFFA6A8A5, 0xFFB9AAB8,
  0xFFC7C9C6, 0xFFD5CBD1, 0xFFD8CED4, 0xFFD3D5D2,
  0xFFD7DAD6, 0xFFD9DCD8, 0xFFE3E0E5, 0xFFE6E3E8,
  0xFF210023, 0xFF300833, 0xFF690068, 0xFF763F76,
  0xFF595B58, 0xFF77477B, 0xFF794279, 0xFF7C4978,
  0xFF7E4B7A, 0xFF636663, 0xFF6B6D6A, 0xFF7D807D,
  0xFF835A81, 0xFF89668B, 0xFF917391, 0xFF977F9B,
  0xFF929491, 0xFF979996, 0xFFA490A6, 0xFFA5A7A4,
  0xFFABADAA, 0xFFB0B3AF, 0xFFBCACBA, 0xFFBEAFBD,
  0xFFC1B9C5, 0xFFCDD0CC, 0xFFCDC5D1, 0xFFDAD0D6,
  0xFFE0E2DF, 0xFFDEDBE0, 0xFFE2DFE3, 0xFFE5E2E7,
  0xFFE8EAE7, 0xFFEAE7EC, 0xFFEDEAEE, 0xFFEDF0ED,
  0xFFEFF1EE, 0xFF170017, 0xFF2D002C, 0xFF2C2E2C,
  0xFF3F333E, 0xFF560056, 0xFF5E0061, 0xFF600263,
  0xFF713674, 0xFF723775, 0xFF4A4147, 0xFF525451,
  0xFF5E605E, 0xFF7F4C7B, 0xFF737175, 0xFF797B78,
  0xFF7C7E7B, 0xFF7F5380, 0xFF805481, 0xFF865D84,
  0xFF886086, 0xFF8B688D, 0xFF947694, 0xFF987A98,
  0xFF818380, 0xFF868885, 0xFF898C89, 0xFF99819D,
  0xFF9A829E, 0xFFA0859C, 0xFFB2A3B1, 0xFFBBABB9,
  0xFFBDAEBC, 0xFFB6B8B5, 0xFFC0C3BF, 0xFFC1B2C0,
  0xFFC3B3C1, 0xFFC4B5C3, 0xFFC5BDC9, 0xFFC7BFCB,
  0xFFC8CAC7, 0xFFC9C1CD, 0xFFCFD2CF, 0xFFD1D3D0,
  0xFFDCD2D8, 0xFFDFD5DC, 0xFFE2E5E1, 0xFFE3E6E2,
  0xFFE7E4E9, 0xFFEBEEEA, 0xFFEFECF0, 0xFFF3F0F5,
  0xFFFCFFFB
#endif

};

static GUI_CONST_STORAGE GUI_LOGPALETTE _PalmqttLogo64 = {
  185,  // Number of entries
  0,    // No transparency
  (const LCD_COLOR *)&_ColorsmqttLogo64[0]
};

static GUI_CONST_STORAGE unsigned char _acmqttLogo64[] = {
  /* RLE: 402 Pixels @ 00,00 */ 254, 0x00, 148, 0x00, 
  /* ABS: 002 Pixels @ 18,06 */ 0, 2, 0x47, 0x60, 
  /* RLE: 008 Pixels @ 20,06 */ 8, 0x12, 
  /* ABS: 002 Pixels @ 28,06 */ 0, 2, 0x8E, 0x38, 
  /* RLE: 007 Pixels @ 30,06 */ 7, 0x0F, 
  /* ABS: 002 Pixels @ 37,06 */ 0, 2, 0x39, 0x48, 
  /* RLE: 005 Pixels @ 39,06 */ 5, 0x12, 
  /* ABS: 002 Pixels @ 44,06 */ 0, 2, 0x60, 0x85, 
  /* RLE: 035 Pixels @ 46,06 */ 35, 0x00, 
  /* ABS: 002 Pixels @ 17,07 */ 0, 2, 0x11, 0x89, 
  /* RLE: 009 Pixels @ 19,07 */ 9, 0x07, 
  /* ABS: 004 Pixels @ 28,07 */ 0, 4, 0x2C, 0x30, 0x43, 0x0A, 
  /* RLE: 006 Pixels @ 32,07 */ 6, 0x03, 
  /* ABS: 002 Pixels @ 38,07 */ 0, 2, 0x59, 0x37, 
  /* RLE: 005 Pixels @ 40,07 */ 5, 0x07, 
  /* ABS: 002 Pixels @ 45,07 */ 0, 2, 0x8B, 0x61, 
  /* RLE: 034 Pixels @ 47,07 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,08 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 011 Pixels @ 19,08 */ 11, 0x01, 
  /* ABS: 004 Pixels @ 30,08 */ 0, 4, 0x35, 0xA1, 0x43, 0x04, 
  /* RLE: 005 Pixels @ 34,08 */ 5, 0xB8, 
  /* ABS: 003 Pixels @ 39,08 */ 0, 3, 0x81, 0x9B, 0x1B, 
  /* RLE: 004 Pixels @ 42,08 */ 4, 0x01, 
  /* RLE: 001 Pixels @ 46,08 */ 1, 0x06, 
  /* RLE: 034 Pixels @ 47,08 */ 34, 0x00, 
  /* ABS: 007 Pixels @ 17,09 */ 0, 7, 0x48, 0xA0, 0x6F, 0x50, 0x4A, 0x2D, 0x18, 
  /* RLE: 008 Pixels @ 24,09 */ 8, 0x01, 
  /* ABS: 004 Pixels @ 32,09 */ 0, 4, 0x35, 0x30, 0x45, 0x02, 
  /* RLE: 004 Pixels @ 36,09 */ 4, 0xB8, 
  /* ABS: 007 Pixels @ 40,09 */ 0, 7, 0x09, 0x31, 0x1F, 0x01, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,09 */ 34, 0x00, 
  /* ABS: 010 Pixels @ 17,10 */ 0, 10, 0x1D, 0x0A, 0x04, 0x08, 0x0A, 0x23, 0x59, 0x21, 0x6D, 0x35, 
  /* RLE: 007 Pixels @ 27,10 */ 7, 0x01, 
  /* ABS: 003 Pixels @ 34,10 */ 0, 3, 0x68, 0xAD, 0x04, 
  /* RLE: 004 Pixels @ 37,10 */ 4, 0xB8, 
  /* ABS: 006 Pixels @ 41,10 */ 0, 6, 0x03, 0x3E, 0x18, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,10 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,11 */ 0, 2, 0x1D, 0x03, 
  /* RLE: 005 Pixels @ 19,11 */ 5, 0xB8, 
  /* ABS: 006 Pixels @ 24,11 */ 0, 6, 0x02, 0x08, 0x7E, 0x57, 0x37, 0x25, 
  /* RLE: 005 Pixels @ 30,11 */ 5, 0x01, 
  /* ABS: 003 Pixels @ 35,11 */ 0, 3, 0x18, 0x30, 0x29, 
  /* RLE: 004 Pixels @ 38,11 */ 4, 0xB8, 
  /* ABS: 005 Pixels @ 42,11 */ 0, 5, 0x03, 0x3E, 0x1F, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,11 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,12 */ 0, 2, 0x1D, 0x03, 
  /* RLE: 008 Pixels @ 19,12 */ 8, 0xB8, 
  /* ABS: 005 Pixels @ 27,12 */ 0, 5, 0x02, 0x46, 0x78, 0x95, 0x1B, 
  /* RLE: 004 Pixels @ 32,12 */ 4, 0x01, 
  /* ABS: 003 Pixels @ 36,12 */ 0, 3, 0x1B, 0x3C, 0x23, 
  /* RLE: 004 Pixels @ 39,12 */ 4, 0xB8, 
  /* ABS: 004 Pixels @ 43,12 */ 0, 4, 0x03, 0x30, 0x1B, 0x06, 
  /* RLE: 034 Pixels @ 47,12 */ 34, 0x00, 
  /* ABS: 005 Pixels @ 17,13 */ 0, 5, 0x1D, 0x09, 0x08, 0x04, 0x02, 
  /* RLE: 007 Pixels @ 22,13 */ 7, 0xB8, 
  /* ABS: 004 Pixels @ 29,13 */ 0, 4, 0x02, 0x84, 0x57, 0x49, 
  /* RLE: 005 Pixels @ 33,13 */ 5, 0x01, 
  /* ABS: 002 Pixels @ 38,13 */ 0, 2, 0x6C, 0x5E, 
  /* RLE: 004 Pixels @ 40,13 */ 4, 0xB8, 
  /* ABS: 003 Pixels @ 44,13 */ 0, 3, 0x46, 0x51, 0x06, 
  /* RLE: 034 Pixels @ 47,13 */ 34, 0x00, 
  /* ABS: 009 Pixels @ 17,14 */ 0, 9, 0x2B, 0x98, 0x6E, 0x55, 0x21, 0x28, 0x5F, 0x08, 0x02, 
  /* RLE: 005 Pixels @ 26,14 */ 5, 0xB8, 
  /* ABS: 004 Pixels @ 31,14 */ 0, 4, 0x02, 0x43, 0x99, 0x1B, 
  /* RLE: 004 Pixels @ 35,14 */ 4, 0x01, 
  /* ABS: 002 Pixels @ 39,14 */ 0, 2, 0x96, 0x5E, 
  /* RLE: 004 Pixels @ 41,14 */ 4, 0xB8, 
  /* ABS: 002 Pixels @ 45,14 */ 0, 2, 0x23, 0x65, 
  /* RLE: 034 Pixels @ 47,14 */ 34, 0x00, 
  /* ABS: 011 Pixels @ 17,15 */ 0, 11, 0x0E, 0x07, 0x01, 0x01, 0x01, 0x25, 0x2D, 0x6D, 0x77, 0x23, 0x04, 
  /* RLE: 005 Pixels @ 28,15 */ 5, 0xB8, 
  /* ABS: 003 Pixels @ 33,15 */ 0, 3, 0x29, 0x72, 0x1F, 
  /* RLE: 004 Pixels @ 36,15 */ 4, 0x01, 
  /* ABS: 002 Pixels @ 40,15 */ 0, 2, 0x6C, 0x23, 
  /* RLE: 004 Pixels @ 42,15 */ 4, 0xB8, 
  /* ABS: 002 Pixels @ 46,15 */ 0, 2, 0x32, 0x05, 
  /* RLE: 033 Pixels @ 48,15 */ 33, 0x00, 
  /* ABS: 002 Pixels @ 17,16 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 006 Pixels @ 19,16 */ 6, 0x01, 
  /* ABS: 005 Pixels @ 25,16 */ 0, 5, 0x1B, 0x36, 0x31, 0x7F, 0x02, 
  /* RLE: 004 Pixels @ 30,16 */ 4, 0xB8, 
  /* ABS: 003 Pixels @ 34,16 */ 0, 3, 0x03, 0x21, 0x18, 
  /* RLE: 004 Pixels @ 37,16 */ 4, 0x01, 
  /* ABS: 007 Pixels @ 41,16 */ 0, 7, 0x3C, 0x46, 0xB8, 0xB8, 0xB8, 0x19, 0x05, 
  /* RLE: 033 Pixels @ 48,16 */ 33, 0x00, 
  /* ABS: 002 Pixels @ 17,17 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 009 Pixels @ 19,17 */ 9, 0x01, 
  /* ABS: 003 Pixels @ 28,17 */ 0, 3, 0x8D, 0x76, 0x09, 
  /* RLE: 004 Pixels @ 31,17 */ 4, 0xB8, 
  /* ABS: 013 Pixels @ 35,17 */ 0, 13, 0x04, 0xA3, 0x18, 0x01, 0x01, 0x01, 0x1B, 0x31, 0x04, 0xB8, 0xB8, 0x19, 0x05, 
  /* RLE: 033 Pixels @ 48,17 */ 33, 0x00, 
  /* ABS: 006 Pixels @ 17,18 */ 0, 6, 0x48, 0x9F, 0x9A, 0x91, 0x2D, 0x1F, 
  /* RLE: 006 Pixels @ 23,18 */ 6, 0x01, 
  /* ABS: 019 Pixels @ 29,18 */ 0, 19, 0x34, 0x3C, 0x5E, 0x02, 0xB8, 0xB8, 0xB8, 0x08, 0x3E, 0x25, 0x01, 0x01, 0x01, 0x18, 0x28, 0x02, 0xB8, 0x19, 0x05, 
  /* RLE: 033 Pixels @ 48,18 */ 33, 0x00, 
  /* ABS: 009 Pixels @ 17,19 */ 0, 9, 0x1D, 0x0A, 0x08, 0x03, 0x81, 0x41, 0x72, 0x4A, 0x1B, 
  /* RLE: 005 Pixels @ 26,19 */ 5, 0x01, 
  /* ABS: 008 Pixels @ 31,19 */ 0, 8, 0x67, 0x5A, 0x02, 0xB8, 0xB8, 0xB8, 0x0A, 0x3D, 
  /* RLE: 004 Pixels @ 39,19 */ 4, 0x01, 
  /* ABS: 005 Pixels @ 43,19 */ 0, 5, 0x68, 0x45, 0xB8, 0x19, 0x05, 
  /* RLE: 033 Pixels @ 48,19 */ 33, 0x00, 
  /* ABS: 002 Pixels @ 17,20 */ 0, 2, 0x1D, 0x03, 
  /* RLE: 004 Pixels @ 19,20 */ 4, 0xB8, 
  /* ABS: 004 Pixels @ 23,20 */ 0, 4, 0x04, 0x45, 0x76, 0x63, 
  /* RLE: 005 Pixels @ 27,20 */ 5, 0x01, 
  /* ABS: 008 Pixels @ 32,20 */ 0, 8, 0x36, 0x41, 0x02, 0xB8, 0xB8, 0xB8, 0x33, 0x97, 
  /* RLE: 004 Pixels @ 40,20 */ 4, 0x01, 
  /* ABS: 004 Pixels @ 44,20 */ 0, 4, 0x31, 0x04, 0x19, 0x05, 
  /* RLE: 033 Pixels @ 48,20 */ 33, 0x00, 
  /* ABS: 002 Pixels @ 17,21 */ 0, 2, 0x1D, 0x03, 
  /* RLE: 006 Pixels @ 19,21 */ 6, 0xB8, 
  /* ABS: 004 Pixels @ 25,21 */ 0, 4, 0x02, 0x23, 0x55, 0x18, 
  /* RLE: 004 Pixels @ 29,21 */ 4, 0x01, 
  /* ABS: 002 Pixels @ 33,21 */ 0, 2, 0x8C, 0x7B, 
  /* RLE: 004 Pixels @ 35,21 */ 4, 0xB8, 
  /* ABS: 009 Pixels @ 39,21 */ 0, 9, 0x5A, 0x26, 0x01, 0x01, 0x01, 0x2C, 0x7D, 0x19, 0x05, 
  /* RLE: 033 Pixels @ 48,21 */ 33, 0x00, 
  /* ABS: 004 Pixels @ 17,22 */ 0, 4, 0x1D, 0x09, 0x04, 0x02, 
  /* RLE: 006 Pixels @ 21,22 */ 6, 0xB8, 
  /* ABS: 003 Pixels @ 27,22 */ 0, 3, 0x08, 0xA9, 0x2C, 
  /* RLE: 004 Pixels @ 30,22 */ 4, 0x01, 
  /* ABS: 007 Pixels @ 34,22 */ 0, 7, 0x67, 0x7F, 0xB8, 0xB8, 0xB8, 0x04, 0x30, 
  /* RLE: 004 Pixels @ 41,22 */ 4, 0x01, 
  /* ABS: 003 Pixels @ 45,22 */ 0, 3, 0x55, 0x0C, 0x05, 
  /* RLE: 033 Pixels @ 48,22 */ 33, 0x00, 
  /* ABS: 007 Pixels @ 17,23 */ 0, 7, 0x2B, 0x50, 0x6F, 0xA2, 0x41, 0x29, 0x02, 
  /* RLE: 004 Pixels @ 24,23 */ 4, 0xB8, 
  /* ABS: 003 Pixels @ 28,23 */ 0, 3, 0x04, 0x41, 0x36, 
  /* RLE: 004 Pixels @ 31,23 */ 4, 0x01, 
  /* ABS: 013 Pixels @ 35,23 */ 0, 13, 0x51, 0x09, 0xB8, 0xB8, 0xB8, 0x23, 0x4A, 0x01, 0x01, 0x01, 0x2D, 0x15, 0x05, 
  /* RLE: 033 Pixels @ 48,23 */ 33, 0x00, 
  /* ABS: 008 Pixels @ 17,24 */ 0, 8, 0x0E, 0x07, 0x01, 0x01, 0x1F, 0x66, 0x21, 0x82, 
  /* RLE: 004 Pixels @ 25,24 */ 4, 0xB8, 
  /* ABS: 013 Pixels @ 29,24 */ 0, 13, 0x02, 0x28, 0x26, 0x01, 0x01, 0x01, 0x25, 0xA8, 0x02, 0xB8, 0xB8, 0x02, 0x3E, 
  /* RLE: 004 Pixels @ 42,24 */ 4, 0x01, 
  /* ABS: 002 Pixels @ 46,24 */ 0, 2, 0x1F, 0x11, 
  /* RLE: 033 Pixels @ 48,24 */ 33, 0x00, 
  /* ABS: 002 Pixels @ 17,25 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 004 Pixels @ 19,25 */ 4, 0x01, 
  /* ABS: 024 Pixels @ 23,25 */ 0, 24, 0x62, 0x37, 0x7A, 0x04, 0xB8, 0xB8, 0xB8, 0x04, 0x57, 0x34, 0x01, 0x01, 0x01, 0x66, 0x45, 0xB8, 0xB8, 0xB8, 0x5F, 0x49, 0x01, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,25 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,26 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 006 Pixels @ 19,26 */ 6, 0x01, 
  /* ABS: 022 Pixels @ 25,26 */ 0, 22, 0x26, 0xA4, 0x04, 0xB8, 0xB8, 0xB8, 0x09, 0x6E, 0x01, 0x01, 0x01, 0x62, 0x21, 0x02, 0xB8, 0xB8, 0x04, 0x3D, 0x01, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,26 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,27 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 007 Pixels @ 19,27 */ 7, 0x01, 
  /* ABS: 021 Pixels @ 26,27 */ 0, 21, 0x18, 0xA7, 0x04, 0xB8, 0xB8, 0xB8, 0xB1, 0x26, 0x01, 0x01, 0x01, 0x65, 0x29, 0xB8, 0xB8, 0xB8, 0x7A, 0x1F, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,27 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,28 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 008 Pixels @ 19,28 */ 8, 0x01, 
  /* ABS: 020 Pixels @ 27,28 */ 0, 20, 0x2C, 0xB0, 0xB8, 0xB8, 0xB8, 0x08, 0x3D, 0x01, 0x01, 0x01, 0x25, 0x28, 0xB8, 0xB8, 0xB8, 0xB6, 0x36, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,28 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,29 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 009 Pixels @ 19,29 */ 9, 0x01, 
  /* ABS: 019 Pixels @ 28,29 */ 0, 19, 0x50, 0x09, 0xB8, 0xB8, 0xB8, 0x5A, 0x18, 0x01, 0x01, 0x01, 0x3D, 0x04, 0xB8, 0xB8, 0x08, 0x51, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,29 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,30 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 009 Pixels @ 19,30 */ 9, 0x01, 
  /* ABS: 019 Pixels @ 28,30 */ 0, 19, 0x25, 0xAB, 0xB8, 0xB8, 0xB8, 0x09, 0x37, 0x01, 0x01, 0x01, 0x63, 0xB7, 0xB8, 0xB8, 0x02, 0x21, 0x01, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,30 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,31 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 010 Pixels @ 19,31 */ 10, 0x01, 
  /* ABS: 018 Pixels @ 29,31 */ 0, 18, 0x3C, 0x08, 0xB8, 0xB8, 0x04, 0x31, 0x01, 0x01, 0x01, 0x26, 0x7D, 0xB8, 0xB8, 0xB8, 0x28, 0x34, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,31 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,32 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 010 Pixels @ 19,32 */ 10, 0x01, 
  /* ABS: 018 Pixels @ 29,32 */ 0, 18, 0x2D, 0xB4, 0xB8, 0xB8, 0xB8, 0xAA, 0x1B, 0x01, 0x01, 0x34, 0x28, 0xB8, 0xB8, 0xB8, 0x43, 0x26, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,32 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,33 */ 0, 2, 0x0E, 0x07, 
  /* RLE: 010 Pixels @ 19,33 */ 10, 0x01, 
  /* ABS: 018 Pixels @ 29,33 */ 0, 18, 0x1F, 0x59, 0xB8, 0xB8, 0xB8, 0x7B, 0x18, 0x01, 0x01, 0x01, 0x77, 0xB8, 0xB8, 0xB8, 0x5F, 0x2C, 0x01, 0x06, 
  /* RLE: 034 Pixels @ 47,33 */ 34, 0x00, 
  /* ABS: 002 Pixels @ 17,34 */ 0, 2, 0x47, 0x8A, 
  /* RLE: 011 Pixels @ 19,34 */ 11, 0x01, 
  /* ABS: 017 Pixels @ 30,34 */ 0, 17, 0x78, 0x02, 0xB8, 0xB8, 0x7E, 0x35, 0x01, 0x01, 0x01, 0x21, 0x02, 0xB8, 0xB8, 0x82, 0x49, 0x07, 0x06, 
  /* RLE: 035 Pixels @ 47,34 */ 35, 0x00, 
  /* RLE: 001 Pixels @ 18,35 */ 1, 0x86, 
  /* RLE: 011 Pixels @ 19,35 */ 11, 0x06, 
  /* ABS: 005 Pixels @ 30,35 */ 0, 5, 0x4F, 0x22, 0x22, 0x22, 0x27, 
  /* RLE: 004 Pixels @ 35,35 */ 4, 0x06, 
  /* ABS: 008 Pixels @ 39,35 */ 0, 8, 0x92, 0x75, 0x22, 0x22, 0x16, 0x88, 0x61, 0x47, 
  /* RLE: 213 Pixels @ 47,35 */ 213, 0x00, 
  /* ABS: 005 Pixels @ 04,39 */ 0, 5, 0x4C, 0x0F, 0x0F, 0x64, 0x14, 
  /* RLE: 007 Pixels @ 09,39 */ 7, 0x00, 
  /* ABS: 021 Pixels @ 16,39 */ 0, 21, 0x11, 0x8F, 0x0F, 0x0F, 0x4D, 0x05, 0x00, 0x00, 0x00, 0x14, 0x17, 0x3A, 0x2F, 0x2F, 0x3A, 0x17, 0x14, 0x00, 0x00, 0x05, 0x38, 
  /* RLE: 010 Pixels @ 37,39 */ 10, 0x0B, 
  /* ABS: 002 Pixels @ 47,39 */ 0, 2, 0x24, 0x17, 
  /* RLE: 010 Pixels @ 49,39 */ 10, 0x0B, 
  /* ABS: 002 Pixels @ 59,39 */ 0, 2, 0x0F, 0x14, 
  /* RLE: 006 Pixels @ 61,39 */ 6, 0x00, 
  /* ABS: 006 Pixels @ 03,40 */ 0, 6, 0x05, 0x40, 0x03, 0x03, 0x09, 0x4F, 
  /* RLE: 007 Pixels @ 09,40 */ 7, 0x00, 
  /* ABS: 021 Pixels @ 16,40 */ 0, 21, 0x39, 0x84, 0x03, 0x03, 0x5B, 0x1A, 0x00, 0x11, 0x4D, 0x32, 0x33, 0x08, 0x04, 0x04, 0x08, 0x33, 0x3F, 0x4D, 0x11, 0x24, 0x44, 
  /* RLE: 009 Pixels @ 37,40 */ 9, 0x03, 
  /* ABS: 004 Pixels @ 46,40 */ 0, 4, 0x0A, 0x4F, 0x2F, 0x0A, 
  /* RLE: 009 Pixels @ 50,40 */ 9, 0x03, 
  /* ABS: 002 Pixels @ 59,40 */ 0, 2, 0x09, 0x4B, 
  /* RLE: 006 Pixels @ 61,40 */ 6, 0x00, 
  /* ABS: 007 Pixels @ 03,41 */ 0, 7, 0x05, 0x0C, 0xB8, 0xB8, 0xB8, 0x0D, 0x2B, 
  /* RLE: 005 Pixels @ 10,41 */ 5, 0x00, 
  /* ABS: 011 Pixels @ 15,41 */ 0, 11, 0x14, 0x19, 0xB8, 0xB8, 0xB8, 0x0D, 0x1A, 0x11, 0x3B, 0x44, 0x02, 
  /* RLE: 006 Pixels @ 26,41 */ 6, 0xB8, 
  /* ABS: 005 Pixels @ 32,41 */ 0, 5, 0x02, 0x44, 0x3B, 0x24, 0x33, 
  /* RLE: 009 Pixels @ 37,41 */ 9, 0xB8, 
  /* ABS: 004 Pixels @ 46,41 */ 0, 4, 0x08, 0x9C, 0x2F, 0x04, 
  /* RLE: 009 Pixels @ 50,41 */ 9, 0xB8, 
  /* ABS: 002 Pixels @ 59,41 */ 0, 2, 0x03, 0x4B, 
  /* RLE: 006 Pixels @ 61,41 */ 6, 0x00, 
  /* ABS: 007 Pixels @ 03,42 */ 0, 7, 0x05, 0x0C, 0xB8, 0xB8, 0xB8, 0x08, 0x52, 
  /* RLE: 005 Pixels @ 10,42 */ 5, 0x00, 
  /* ABS: 046 Pixels @ 15,42 */ 0, 46, 0x0F, 0x09, 0xB8, 0xB8, 0xB8, 0x0D, 0x1A, 0x69, 0x33, 0xB8, 0xB8, 0x08, 0x42, 0x0C, 0x0C, 0x42, 0x08, 0xB8, 0xB8, 0xB5, 0x6A, 0x22, 0x0C, 0x0C, 0x19, 0x04, 0xB8, 0x02, 0x0D, 0x0C, 0x0C, 0x40, 0x38, 0x0B, 0x40, 
        0x0C, 0x0C, 0x5D, 0xB8, 0xB8, 0x04, 0x19, 0x0C, 0x0C, 0x3F, 0x24, 
  /* RLE: 006 Pixels @ 61,42 */ 6, 0x00, 
  /* ABS: 002 Pixels @ 03,43 */ 0, 2, 0x05, 0x0C, 
  /* RLE: 004 Pixels @ 05,43 */ 4, 0xB8, 
  /* ABS: 007 Pixels @ 09,43 */ 0, 7, 0x5C, 0x20, 0x00, 0x00, 0x00, 0x2A, 0x19, 
  /* RLE: 004 Pixels @ 16,43 */ 4, 0xB8, 
  /* ABS: 040 Pixels @ 20,43 */ 0, 40, 0x0D, 0x24, 0x79, 0xB8, 0xB8, 0x0A, 0x54, 0x20, 0x05, 0x05, 0x20, 0x71, 0x0A, 0xB8, 0xB8, 0x79, 0x1A, 0x05, 0x05, 0x4C, 0x03, 0xB8, 0x04, 0x15, 0x05, 0x05, 0x05, 0x00, 0x00, 0x05, 0x05, 0x05, 0x16, 0x02, 0xB8, 
        0x0A, 0x13, 0x05, 0x05, 0x05, 
  /* RLE: 007 Pixels @ 60,43 */ 7, 0x00, 
  /* ABS: 024 Pixels @ 03,44 */ 0, 24, 0x05, 0x0C, 0xB8, 0xB8, 0x04, 0xB8, 0x08, 0x9E, 0x00, 0x00, 0x00, 0x6A, 0x09, 0xB8, 0x08, 0xB8, 0xB8, 0x0D, 0x0B, 0x0A, 0xB8, 0x02, 0x3F, 0x14, 
  /* RLE: 004 Pixels @ 27,44 */ 4, 0x00, 
  /* ABS: 013 Pixels @ 31,44 */ 0, 13, 0x14, 0x3F, 0x02, 0xB8, 0x0A, 0x39, 0x00, 0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,44 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,44 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,44 */ 10, 0x00, 
  /* ABS: 023 Pixels @ 03,45 */ 0, 23, 0x05, 0x0C, 0xB8, 0x02, 0x0D, 0x08, 0xB8, 0x5D, 0x20, 0x00, 0x2A, 0x58, 0xB8, 0x02, 0x0D, 0x04, 0xB8, 0x0D, 0x27, 0x02, 0xB8, 0x08, 0x2E, 
  /* RLE: 006 Pixels @ 26,45 */ 6, 0x00, 
  /* ABS: 012 Pixels @ 32,45 */ 0, 12, 0x2E, 0x08, 0xB8, 0x02, 0x27, 0x00, 0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,45 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,45 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,45 */ 10, 0x00, 
  /* ABS: 023 Pixels @ 03,46 */ 0, 23, 0x05, 0x0C, 0xB8, 0x02, 0x22, 0x5D, 0xB8, 0x08, 0x2F, 0x00, 0x2E, 0x09, 0xB8, 0x80, 0x27, 0x04, 0xB8, 0x0D, 0x32, 0xB8, 0xB8, 0x09, 0x13, 
  /* RLE: 006 Pixels @ 26,46 */ 6, 0x00, 
  /* ABS: 012 Pixels @ 32,46 */ 0, 12, 0x13, 0x09, 0xB8, 0xB8, 0xA5, 0x00, 0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,46 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,46 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,46 */ 10, 0x00, 
  /* ABS: 023 Pixels @ 03,47 */ 0, 23, 0x05, 0x0C, 0xB8, 0x02, 0x1E, 0x9D, 0x08, 0xB8, 0x42, 0x17, 0xAC, 0xB8, 0x04, 0x27, 0x10, 0x04, 0xB8, 0x0D, 0x32, 0xB8, 0xB8, 0x09, 0x17, 
  /* RLE: 006 Pixels @ 26,47 */ 6, 0x00, 
  /* ABS: 012 Pixels @ 32,47 */ 0, 12, 0x17, 0x09, 0xB8, 0xB8, 0x32, 0x00, 0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,47 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,47 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,47 */ 10, 0x00, 
  /* ABS: 023 Pixels @ 03,48 */ 0, 23, 0x05, 0x0C, 0xB8, 0x02, 0x1E, 0x2B, 0x5B, 0xB8, 0x08, 0x16, 0x0A, 0xB8, 0xB3, 0x13, 0x10, 0x04, 0xB8, 0x0D, 0x56, 0x02, 0xB8, 0x03, 0x69, 
  /* RLE: 006 Pixels @ 26,48 */ 6, 0x00, 
  /* ABS: 012 Pixels @ 32,48 */ 0, 12, 0x39, 0x03, 0xB8, 0x02, 0x73, 0x00, 0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,48 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,48 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,48 */ 10, 0x00, 
  /* ABS: 024 Pixels @ 03,49 */ 0, 24, 0x05, 0x0C, 0xB8, 0x02, 0x1E, 0x00, 0x6B, 0x03, 0xB8, 0x0A, 0xB8, 0x04, 0x15, 0x11, 0x10, 0x04, 0xB8, 0x0D, 0x3A, 0x03, 0xB8, 0x02, 0x1E, 0x05, 
  /* RLE: 004 Pixels @ 27,49 */ 4, 0x00, 
  /* ABS: 013 Pixels @ 31,49 */ 0, 13, 0x05, 0x1E, 0x02, 0xB8, 0x03, 0x2E, 0x00, 0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,49 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,49 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,49 */ 10, 0x00, 
  /* ABS: 041 Pixels @ 03,50 */ 0, 41, 0x05, 0x0C, 0xB8, 0x02, 0x1E, 0x00, 0x1A, 0xAE, 0xB8, 0xB8, 0xB8, 0x7C, 0x24, 0x00, 0x10, 0x04, 0xB8, 0x0D, 0x17, 0x5C, 0xB8, 0xB8, 0x29, 0x94, 0x14, 0x00, 0x00, 0x14, 0x3B, 0x29, 0xB8, 0xB8, 0x5C, 0x20, 0x00, 
        0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,50 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,50 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,50 */ 10, 0x00, 
  /* ABS: 041 Pixels @ 03,51 */ 0, 41, 0x05, 0x0C, 0xB8, 0x02, 0x1E, 0x00, 0x00, 0x4E, 0x0A, 0xB8, 0x08, 0x10, 0x00, 0x00, 0x10, 0x04, 0xB8, 0x0D, 0x1A, 0x93, 0x09, 0xB8, 0xB8, 0x09, 0x58, 0x56, 0x73, 0x58, 0x09, 0xB8, 0xB8, 0x09, 0x3B, 0x00, 0x00, 
        0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,51 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,51 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,51 */ 10, 0x00, 
  /* ABS: 041 Pixels @ 03,52 */ 0, 41, 0x05, 0x0C, 0xB8, 0x02, 0x1E, 0x00, 0x00, 0x2A, 0x75, 0x7C, 0xA6, 0x20, 0x00, 0x00, 0x10, 0x04, 0xB8, 0x0D, 0x1A, 0x05, 0x10, 0x46, 0xB8, 0xB8, 0xB8, 0x02, 0x02, 0xB8, 0xB8, 0xB8, 0x09, 0x71, 0x05, 0x00, 0x00, 
        0x00, 0x1C, 0x03, 0xB8, 0x04, 0x15, 
  /* RLE: 008 Pixels @ 44,52 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,52 */ 0, 5, 0x16, 0x02, 0xB8, 0x0A, 0x13, 
  /* RLE: 010 Pixels @ 57,52 */ 10, 0x00, 
  /* ABS: 041 Pixels @ 03,53 */ 0, 41, 0x05, 0x40, 0x04, 0x08, 0x1E, 0x00, 0x00, 0x00, 0x14, 0x20, 0x2A, 0x00, 0x00, 0x00, 0x53, 0x03, 0x04, 0x5B, 0x1A, 0x00, 0x05, 0x3A, 0xAF, 0x0A, 0x04, 0x02, 0x02, 0x02, 0xB8, 0xB8, 0x09, 0x52, 0x11, 0x00, 0x00, 
        0x00, 0x4B, 0x0A, 0x04, 0x03, 0x54, 
  /* RLE: 008 Pixels @ 44,53 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,53 */ 0, 5, 0x56, 0x08, 0x04, 0x09, 0x17, 
  /* RLE: 010 Pixels @ 57,53 */ 10, 0x00, 
  /* ABS: 005 Pixels @ 03,54 */ 0, 5, 0x11, 0x2E, 0x10, 0x10, 0x0F, 
  /* RLE: 009 Pixels @ 08,54 */ 9, 0x00, 
  /* ABS: 027 Pixels @ 17,54 */ 0, 27, 0x1D, 0x53, 0x10, 0x6B, 0x14, 0x00, 0x00, 0x00, 0x1A, 0x90, 0x54, 0x74, 0x74, 0x27, 0x44, 0xB8, 0xB8, 0x83, 0x4E, 0x11, 0x00, 0x00, 0x2B, 0x70, 0x10, 0x53, 0x64, 
  /* RLE: 008 Pixels @ 44,54 */ 8, 0x00, 
  /* ABS: 005 Pixels @ 52,54 */ 0, 5, 0x38, 0x10, 0x10, 0x70, 0x2A, 
  /* RLE: 037 Pixels @ 57,54 */ 37, 0x00, 
  /* ABS: 007 Pixels @ 30,55 */ 0, 7, 0x11, 0x4E, 0x83, 0xB8, 0xB8, 0x80, 0x0B, 
  /* RLE: 058 Pixels @ 37,55 */ 58, 0x00, 
  /* ABS: 007 Pixels @ 31,56 */ 0, 7, 0x11, 0x52, 0xB2, 0x09, 0x09, 0x42, 0x4C, 
  /* RLE: 058 Pixels @ 38,56 */ 58, 0x00, 
  /* ABS: 006 Pixels @ 32,57 */ 0, 6, 0x11, 0x87, 0x17, 0x17, 0x17, 0x14, 
  /* RLE: 410 Pixels @ 38,57 */ 254, 0x00, 156, 0x00, 
  0
};  // 1802 bytes for 4096 pixels

GUI_CONST_STORAGE GUI_BITMAP bmmqttLogo64 = {
  64, // xSize
  64, // ySize
  64, // BytesPerLine
  GUI_COMPRESS_RLE8, // BitsPerPixel
  (const unsigned char *)_acmqttLogo64,  // Pointer to picture data (indices)
  &_PalmqttLogo64,  // Pointer to palette
  GUI_DRAW_RLE8
};

/*************************** End of file ****************************/