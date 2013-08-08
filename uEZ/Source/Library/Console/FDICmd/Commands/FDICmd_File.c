/*-------------------------------------------------------------------------*
 * File:  FDICmd_File.c
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
#include <uEZFile.h>
#include <uEZMemory.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include "FDICmd_File.h"

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdFileTestWrite
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      FILETESTWRITE <filename> <size> <numTimes>
 *
 *  Where,
 *      <filename> Name of test file, usually including 0:/ or 1:/ prefix
 *      <size>     Size of file to write for testing
 *      <size block> Size of memory block to use per write instruction
 *                      (NOTE: This is memory limited)
 *      <numTimes> Number of times to execute this test
 *
 *  A file is written with a semi-random pattern of bytes, then read to verify
 *  the same pattern of bytes.  This process is repeated a number of times with
 *  output as it goes.  The final result will be shown.
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
int UEZCmdFileTestWrite(void *aWorkspace, int argc, char *argv[])
{
    T_uezDevice file;
    TUInt32 size;
    TUInt32 v;
    TUInt32 i;
    TUInt32 numTimes = 0;
    TUInt8 *dataOut;
    TUInt8 *dataIn;
    TUInt32 count = 0;
    const char *filename = argv[1];
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 numBytes;
    TUInt32 numWritten;
    TUInt32 numRead;
    TUInt32 numToProcess;
    TUInt32 blockSize;

    if (argc == 5) {
        numTimes = FDICmdUValue(argv[4]);
        blockSize = FDICmdUValue(argv[3]);
        size = FDICmdUValue(argv[2]);
        // Allocate a place to store buffers
        dataIn = UEZMemAlloc(blockSize);
        dataOut = UEZMemAlloc(blockSize);
        if (dataIn && dataOut) {
            for (count = 0; count < numTimes; count++) {
                FDICmdPrintf(aWorkspace, "Test #%d Write\n", count+1);
                // Open up the file for writing
                file = 0;
                error = UEZFileOpen(filename, FILE_FLAG_WRITE, &file);
                if (error) {
                    FDICmdPrintf(aWorkspace,
                        "File %s cannot be open for writing!\n", filename);
                    error = UEZ_ERROR_NOT_FOUND;
                    break;
                }

                // Initialize the semi-random value
                v = count;

                numToProcess = size;
                while (numToProcess) {
                    numBytes = numToProcess;
                    if (numBytes > blockSize)
                        numBytes = blockSize;

                    // Fill the buffer with semi-random data
                    for (i = 0; i < numBytes; i++) {
                        // Store a 16-bit value element
                        if (i & 1) {
                            dataOut[i] = (v >> 0) & 0xFF;
                        } else {
                            dataOut[i] = (v >> 8) & 0xFF;
                        }
                        if (i & 4) {
                            v ^= count * 7;
                        } else {
                            v += i;
                        }
                    }

                    // Write the data waiting to go out
                    error = UEZFileWrite(file, dataOut, numBytes, &numWritten);
                    if (error) {
                        FDICmdPrintf(aWorkspace, "FAIL: File Write Error\n");
                        break;
                    }
                    numToProcess -= numBytes;
                }
                if (error)
                    break;

                UEZFileClose(file);

                // Open up the file for reading/verifying
                file = 0;
                error = UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file);
                if (error) {
                    FDICmdPrintf(aWorkspace,
                        "File %s cannot be open for reading!\n", filename);
                    error = UEZ_ERROR_NOT_FOUND;
                    break;
                }

                FDICmdPrintf(aWorkspace, "Test #%d Verify\n", count+1);
                // Initialize the semi-random value
                v = count;

                numToProcess = size;
                while (numToProcess) {
                    numBytes = numToProcess;
                    if (numBytes > blockSize)
                        numBytes = blockSize;

                    // Fill the buffer with semi-random data
                    for (i = 0; i < numBytes; i++) {
                        // Store a 16-bit value element
                        if (i & 1) {
                            dataOut[i] = (v >> 0) & 0xFF;
                        } else {
                            dataOut[i] = (v >> 8) & 0xFF;
                        }
                        if (i & 4) {
                            v ^= count * 7;
                        } else {
                            v += i;
                        }
                    }

                    // Read in the data
                    error = UEZFileRead(file, dataIn, numBytes, &numRead);
                    if (error) {
                        FDICmdPrintf(aWorkspace, "FAIL: File Read Error\n");
                        break;
                    }

                    // Now verify the data read in matches the data written out
                    for (i = 0; i < numBytes; i++) {
                        // Determine if there is a mismatch and where
                        if (dataIn[i] != dataOut[i]) {
                            FDICmdPrintf(aWorkspace,
                                "FAIL: Mismatch at offset %d (0x%02X != 0x%02X)\n",
                                (size - numToProcess) + i, dataIn[i],
                                dataOut[i]);
                            error = UEZ_ERROR_MISMATCH;
                            break;
                        }
                    }
                    if (error)
                        break;

                    numToProcess -= numBytes;
                }
                if (error)
                    break;

                UEZFileClose(file);
            }
            if (file) {
                UEZFileClose(file);
                file = 0;
            }
        } else {
            FDICmdSendString(aWorkspace, "FAIL: Out of memory to run test\n");
        }
        if (dataOut) {
            UEZMemFree(dataOut);
            dataOut = 0;
        }
        if (dataIn) {
            UEZMemFree(dataIn);
            dataOut = 0;
        }
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: FILETESTWRITE <filename> <size> <mem block size> <numTimes>\n");
    }

    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  FDICmd_File.c
 *-------------------------------------------------------------------------*/
