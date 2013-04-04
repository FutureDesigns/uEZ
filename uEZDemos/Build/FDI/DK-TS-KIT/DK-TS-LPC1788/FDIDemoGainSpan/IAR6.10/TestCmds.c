/*-------------------------------------------------------------------------*
 * File:  UEZGUITestCmds.c
 *-------------------------------------------------------------------------*
 * Description:
 *      UEZGUI Tester command console
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2010 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 * 	-   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 * 	-   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 	-   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * 	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * 	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * 	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * 	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * 	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * 	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * 	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZStream.h>
#include <uEZSPI.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
//#include "FuncTestFramework.h"
//#include "FunctionalTest.h"
#include "AppDemo.h"
#include <HAL/GPIO.h>
#include <uEZToneGenerator.h>
#include <Device/ToneGenerator.h>
#include "NVSettings.h"
#include <uEZToneGenerator.h>
#if UEZ_ENABLE_AUDIO_AMP
#include <uEZAudioAmp.h>
#endif
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
#include <uEZLCD.h>
#include "UEZGainSpan.h"

int UEZGUICmdGainSpan(void *aWorkspace, int argc, char *argv[]);

#include <Source/Library/Network/GainSpan/CmdLib/AtCmdLib.h>

int UEZGUICmdGainSpan(void *aWorkspace, int argc, char *argv[])
{
  // todo: Not Finished - Here for future reference
  char messageRxBuf[400] = {0};
  static TBool init_gainspan = EFalse;
  if (!init_gainspan) {
    //GainSpan_SPI_Start();
    init_gainspan = ETrue;
  }
  UEZGUIGainSpan(aWorkspace, argc, argv, messageRxBuf);

  return 0;
}



