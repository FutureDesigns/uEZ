/*-------------------------------------------------------------------------*
 * File:  PictureFame_Main.c
 *-------------------------------------------------------------------------*
 * Description:
 *      <DESCRIPTION>
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include "string.h"
#include <uEZ.h>
#include <UEZFile.h>
#include "WM.h"

#define IMAGE_BUFFER_SIZE           200000

#pragma section Images
static TUInt8 _G_Image[IMAGE_BUFFER_SIZE];
#pragma section

/*---------------------------------------------------------------------------*
 * Routine: PictureFame
 *---------------------------------------------------------------------------*/
/**
 *
 *  @return                 The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN PictureFame()
{
    T_uezFile file;
    char filename[50];
	TUInt32 read;
	TUInt32 *p_Image;

    sprintf( filename, "1:/images/WQSLID05.bmp" );

    if(UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE){
		p_Image = (TUInt32 *)_G_Image;

		if(p_Image){
            UEZFileRead(file, p_Image, IMAGE_BUFFER_SIZE, &read);
        }
        UEZFileClose(file);
    } else {
        UEZFailureMsg("Image Not Found");
    }

    GUI_BMP_Draw(p_Image, 0, 0);

	// Stop here
	while(1);

	return 1;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    PictureFame_Main.c
 *-------------------------------------------------------------------------*/
