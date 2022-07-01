/*-------------------------------------------------------------------------*
 * File:  FDICmd_I2C.c
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
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include "FDICmd_AudioDAC.h"
#include <math.h>

#ifndef M_PI
#define M_PI    3.141592654
#endif

#ifndef FDICMD_AUDIODAC_BIT_SIZE
#define FDICMD_AUDIODAC_BIT_SIZE  8
#endif

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdAudioDAC
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      AUDIODAC (frequency) (sample rate) (ms to play) (timer)
 *
 *  Where,
 *      (frequency) is the Hz of sinewave to create
 *      (sample rate) is the Hz of the sample to create (usually 22100)
 *      (ms) is the length to play the tone
 *      (timer) Timer to use for playing (usually Timer0)
 *
 *  A 16-bit sample is created in memory, played, and then removed from memory.
 *  If there is not enough memory, a memory failure will be reported.
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
int32_t UEZCmdAudioDAC(void *aWorkspace, int32_t argc, char *argv[])
{
    T_uezError error;
    TUInt32 ms;
    TUInt32 frequency;
    TUInt32 sampleRate;
    TUInt32 numBytes;
    TUInt32 numSamples;
#if (FDICMD_AUDIODAC_BIT_SIZE==8)
    TUInt8 *p_sample;
#else
    TUInt16 *p_sample;
#endif
    TUInt32 i;
    double r;
    double v;

    if (argc == 5) {
        // Get the frequency
        frequency = FDICmdUValue(argv[1]);
        if ((frequency < 1) || (frequency > 100000)) {
            FDICmdPrintf(aWorkspace, "FAIL: Frequency must be 1 to 100000\n");
            return UEZ_ERROR_INVALID_PARAMETER;
        }

        // Get the sample rate
        sampleRate = FDICmdUValue(argv[2]);
        if ((sampleRate < 1000) || (sampleRate > 100000)) {
            FDICmdPrintf(aWorkspace, "FAIL: Sample rate must be 1000 to 100000.  Usually 22100\n");
            return UEZ_ERROR_INVALID_PARAMETER;
        }

        // Get the length of time
        ms = FDICmdUValue(argv[3]);

        // Set which timer to use
        error = UEZDACWAVConfig(argv[4]);
        if (error) {
            FDICmdPrintf(aWorkspace, "FAIL: Timer error %d\n", error);
            return UEZ_ERROR_INVALID_PARAMETER;
        }

        // Calculate the needed amount of memory
        // Number of samples:
        numSamples = sampleRate * ms / 1000;
#if (FDICMD_AUDIODAC_BIT_SIZE==8)
        // 8-bit samples.  Memory needed
        numBytes = numSamples;
#else
        // 16-bit samples.  Memory needed
        numBytes = numSamples * 2;
#endif
        // Try to allocate the needed memory
        p_sample = UEZMemAlloc(numBytes);
        if (p_sample) {
            // Got the memory!
            // Create the sample mathematically
            for (i=0; i<numSamples; i++) {
                // Normalize i into a number of seconds based on sample rate
                r = i;
                r /= sampleRate;

                // r now equals seconds into sample
                // For sin() we need 2*PI per cycle
                r *= 2 * M_PI;

                // We now have a 1 Hz signal, but we need X Hz
                // Calculate the frequency value
                r *= frequency;

                // Now get the sinusoidal position for that
                v = sin(r);

#if (FDICMD_AUDIODAC_BIT_SIZE==8)
                // This -1.0 .. 1.0 value now needs to be a 0 to 65535 value
                // First make it a -127 to 127 value
                v *= 127;

                // Now add 127 to offset it and get 0 to 254
                v += 127;

                // Store this value in memory
                p_sample[i] = (TUInt8)v;
#else
                // This -1.0 .. 1.0 value now needs to be a 0 to 65535 value
                // First make it a -32767 to 32767 value
                v *= 32767;

                // Now add 32767 to offset it and get 0 to 65534
                v += 32767;

                // Store this value in memory
                p_sample[i] = (TUInt16)v;
#endif

                // Next!
            }

            // The sample is now created.  Let's play it
            UEZDACWAVPlayBuffer((TUInt8 *)p_sample, numBytes);

            // Wait for it to end
            while (UEZDACWAVGetStatus()) {
                UEZTaskDelay(1);
            }

            // Done, free the wave and end
            UEZMemFree(p_sample);
        } else {
            FDICmdPrintf(aWorkspace, "FAIL: Not enough memory to create sample\n");
        }
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nAUDIODAC <frequency> <sample rate> <ms to play> <timer driver>\n");
    }

    return UEZ_ERROR_NONE;
}


/*-------------------------------------------------------------------------*
 * End of File:  FDICmd_I2C.c
 *-------------------------------------------------------------------------*/
