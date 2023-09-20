/*-------------------------------------------------------------------------*
 * File:  AudioAmp_LM48110.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef AUDIOAMP_LM48110_H_
#define AUDIOAMP_LM48110_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Device/AudioAmp.h>
#include <Types/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef UEZ_OPTIONAL_VOL_CTRL_ENABLE
#define UEZ_OPTIONAL_VOL_CTRL_ENABLE 0	// CAHCT1G126 Volume control chip
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_AudioAmp AudioAmp_LM48110_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError AudioAmp_LM48110_InitializeWorkspace(void *aWorkSpace);
void AudioAmp_LM48110_Create(
        const char *aName,
        const char *aI2CBus,
        TUInt8 aMaxLevel);
		
#ifdef __cplusplus
}
#endif


#endif // AUDIOAMP_LM48110_H_
/*-------------------------------------------------------------------------*
 * End of File:  AudioAmp_LM48110.h
 *-------------------------------------------------------------------------*/
