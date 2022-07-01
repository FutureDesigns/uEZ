/*-------------------------------------------------------------------------*
 * File:  FDICmd_WiFiPrg.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZI2C.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include <uEZPlatform.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdWiFiPrg
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      WiFiPrg (wavefile)
 *
 *  Where,
 *      (wavfile) Name of file to play
 *
 *  Plays a 16-bit 22 kHz WAV file out the I2S (or whatever is hooked up to
 *  UEZWiFiPrg).
 *
 *  @param [in]     *aWorkspace     FDICmd workspace, needed for outputs
 *
 *  @param [in]     argc            Number of parsed arguments
 *
 *  @param [in]     *argv[]         Parsed arguments (strings)
 *
 *  @return         TUInt32         0 if success, non-zero for error.
 */
/*---------------------------------------------------------------------------*/
int UEZCmdWiFiPrg(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 1) {
        FDICmdPrintf(aWorkspace, "PASS: Entering WiFi pass-through programming mode...\n");
        UEZPlatform_WiFiProgramMode();
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nUSAGE: WiFiPrg\n");
    }

    return UEZ_ERROR_NONE;
}

int UEZCmdWiFiRun(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 1) {
        FDICmdPrintf(aWorkspace, "PASS: Entering WiFi pass-through run mode...\n");
        UEZPlatform_WiFiRunMode();
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nUSAGE: WiFiPrg\n");
    }
    return 0;
}


/*-------------------------------------------------------------------------*
 * End of File:  FDICmd_WiFiPrg.c
 *-------------------------------------------------------------------------*/
