/*-------------------------------------------------------------------------*
 * File:  Glyph_uEZ_0.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
 
#ifndef __GLYPH_UEZ_H__
#define __GLYPH_UEZ_H__

/******************************************************************************
Includes “Glyph Include”
******************************************************************************/
#include "..\glyph_api.h"
 
/******************************************************************************
Prototypes for the Glyph Communications API
******************************************************************************/
void Glyph_uEZ_CommandSend(int8_t cCommand) ;
void Glyph_uEZ_DataSend(int8_t cData) ;
T_glyphError Glyph_uEZ_Open(T_glyphHandle aHandle);
void Glyph_uEZ_Config(void) ;
void Glyph_uEZ_Start(void) ;
void Glyph_uEZ_End(void) ;
void Glyph_uEZ_TransferOutWords(int16_t sLowWord, int16_t sHighWord) ;
void Glyph_uEZ_TransferOutWord8Bit(uint8_t cData) ;

#endif
 
 
