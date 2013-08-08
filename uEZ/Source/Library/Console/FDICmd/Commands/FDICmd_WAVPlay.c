/*-------------------------------------------------------------------------*
 * File:  FDICmd_I2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

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
#include <uEZ.h>
#include <uEZI2C.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include <uEZWAVFile.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdWAVPlay
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      WAVPlay <wavefile>
 *
 *  Where,
 *      <wavfile> Name of file to play
 *
 *  Plays a 16-bit 22 kHz WAV file out the I2S (or whatever is hooked up to
 *  UEZWAVPlay).
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
int UEZCmdWAVPlay(void *aWorkspace, int argc, char *argv[])
{
    T_uezError error;

    if (argc == 2) {
        UEZWAVConfig(48);
        error = UEZWAVPlayFile(argv[1]);
        if (error) {
            FDICmdPrintf(aWorkspace, "FAIL: Error %d\n", error);
            return error;
        } else {
            // Wait for it to start playing
            while (!UEZWAVGetStatus()) {
                UEZTaskDelay(1);
            }
            
            // Wait for it to end
            while (UEZWAVGetStatus()) {
                UEZTaskDelay(1);
            }
            UEZWAVCleanUp();
            FDICmdPrintf(aWorkspace, "PASS: OK\n");
        }
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nWAVPLAY <wav file>\n");
    }

    return UEZ_ERROR_NONE;
}


/*-------------------------------------------------------------------------*
 * End of File:  FDICmd_I2C.c
 *-------------------------------------------------------------------------*/
