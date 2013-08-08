/*-------------------------------------------------------------------------*
 * File:  UEZGainSpan.h
 *-------------------------------------------------------------------------*/
#ifndef UEZGAINSPAN_H_
#define UEZGAINSPAN_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <string.h>
#include <uEZ.h>
#include <uEZStream.h>
#include <uEZSPI.h>


/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
int UEZGUIGainSpan(void *aWorkspace, int argc, char *argv[], char *rxBuf);
void UEZGUIGainSpanProgram(void);

#endif // UEZGAINSPAN_H_
/*-------------------------------------------------------------------------*
 * End of File:  UEZGainSpan.h
 *-------------------------------------------------------------------------*/
