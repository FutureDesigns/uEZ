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
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include "FDICmd_I2C.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdI2CProbe
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      I2CPROBE (bus)
 *
 *  Where (bus) is the number used in the bus name (e.g., 0 for I2C0).
 *  A list of devices found are shown, or a failure error.
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
int32_t UEZCmdI2CProbe(void *aWorkspace, int32_t argc, char *argv[])
{
    T_uezDevice i2c;
    T_uezError error;
    TUInt8 addr;
    TUInt8 data[10];
    char busName[] = "I2C0";

    if (argc == 2) {
        busName[3] = argv[1][0];
        error = UEZI2COpen(busName, &i2c);
        if (error) {
            FDICmdPrintf(aWorkspace, "I2C Bus %s not found!\n", busName);
            return UEZ_ERROR_NOT_FOUND;
        }

        // Probe all 127 addresses
        FDICmdPrintf(aWorkspace, "Probing all 127 I2C addresses on %s:\n",
            busName);
        for (addr = 1; addr <= 127; addr++) {
            // Give it 1 second each (way too much time)
            error = UEZI2CRead(i2c, addr, 100, data, 1, 100);
            if (error == UEZ_ERROR_NONE) {
                FDICmdPrintf(aWorkspace, "  Device @ 0x%02X\n", addr);
            } else if (error == UEZ_ERROR_TIMEOUT) {
                FDICmdPrintf(aWorkspace, "  Timeout at device 0x%02X!", addr);
                break;
            }
        }

        UEZI2CClose(i2c);
        if (addr >= 127) {
            FDICmdSendString(aWorkspace, "PASS: OK\n");
        }
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nI2CPROBE <bus number>\n");
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdI2CWrite
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      I2CWRITE (bus) (7-bit slave addr) (byte1) (byte2) (byte3) ... (byteN)
 *
 *  Where,
 *      (bus) is the number used in the bus name (e.g., 0 for I2C0).
 *      (7-bit slave addr) is the 7-bit I2C address of the device.
 *      (byteX) is the bytes to write
 *
 *  The bytes are written and a final status is reported.  Only up to 128
 *  bytes can be written at a time.
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
int32_t UEZCmdI2CWrite(void *aWorkspace, int32_t argc, char *argv[])
{
    T_uezDevice i2c;
    T_uezError error;
    TUInt8 addr;
    TUInt32 v;
    TUInt8 data[128];
    char busName[] = "I2C0";
    int32_t i;

    if (argc >= 4) {
        busName[3] = argv[1][0];

        // Open up the bus
        error = UEZI2COpen(busName, &i2c);
        if (error) {
            FDICmdPrintf(aWorkspace, "I2C Bus %s not found!\n", busName);
            return UEZ_ERROR_NOT_FOUND;
        }

        // Get the address
        addr = FDICmdUValue(argv[2]);
        if (addr >= 128) {
            FDICmdPrintf(aWorkspace, "Address must be 7-bit (0-127)\n");
            UEZI2CClose(i2c);
            return UEZ_ERROR_INVALID_PARAMETER;
        }

        // Setup the data to be written
        for (i = 3; (i < argc) && (i < sizeof(data)); i++) {
            v = FDICmdUValue(argv[i]);
            if (v >= 256) {
                FDICmdPrintf(aWorkspace,
                    "Values in write must be bytes (0-255).  %d is out of range.\n",
                    v);
                UEZI2CClose(i2c);
                return UEZ_ERROR_INVALID_PARAMETER;
            }
            data[i-3] = v;
        }

        // Write the data
        FDICmdPrintf(aWorkspace, "Writing to bus %s, addr 0x%02X:\n",
            busName, addr);

        // For now, assume 100 kHz is valid
        error = UEZI2CWrite(i2c, addr, 100, data, argc-3, 1000);
        if (error == UEZ_ERROR_NONE) {
            FDICmdSendString(aWorkspace, "PASS: OK\n");
        } else if (error == UEZ_ERROR_NAK) {
            FDICmdSendString(aWorkspace, "FAIL: NAK\n");
        } else if (error == UEZ_ERROR_TIMEOUT) {
            FDICmdSendString(aWorkspace, "FAIL: Timeout!\n");
        } else {
            FDICmdPrintf(aWorkspace, "FAIL: Error #%d\n", error);
        }

        UEZI2CClose(i2c);
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nI2CWRITE <bus number> <addr> [<byte> ...]\n");
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCmdI2CRead
 *---------------------------------------------------------------------------*/
/**
 *  Process the given command:
 *      I2CREAD (bus) (7-bit slave addr) (num bytes read)
 *
 *  Where,
 *      (bus) is the number used in the bus name (e.g., 0 for I2C0).
 *      (7-bit slave addr) is the 7-bit I2C address of the device.
 *      (num bytes read) is the number of bytes to read back by this command.
 *
 *  The bytes are shown formatted in 16 columns.  Only up to 128 bytes can
 *  be read with this command.
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
int32_t UEZCmdI2CRead(void *aWorkspace, int32_t argc, char *argv[])
{
    T_uezDevice i2c;
    T_uezError error;
    TUInt8 addr;
    TUInt32 num;
    TUInt8 data[128];
    char busName[] = "I2C0";
    int32_t i;

    if (argc == 4) {
        busName[3] = argv[1][0];

        // Open up the bus
        error = UEZI2COpen(busName, &i2c);
        if (error) {
            FDICmdPrintf(aWorkspace, "I2C Bus %s not found!\n", busName);
            return UEZ_ERROR_NOT_FOUND;
        }

        // Get the address
        addr = FDICmdUValue(argv[2]);
        if (addr >= 128) {
            FDICmdPrintf(aWorkspace, "Address must be 7-bit (0-127)\n");
            UEZI2CClose(i2c);
            return UEZ_ERROR_INVALID_PARAMETER;
        }

        // Get the number of bytes to read
        num = FDICmdUValue(argv[3]);
        if (num > sizeof(data)) {
            FDICmdPrintf(aWorkspace, "Out of space. Limited read buffer of %d bytes\n", sizeof(data));
            UEZI2CClose(i2c);
            return UEZ_ERROR_INVALID_PARAMETER;
        }

        // Write the data
        FDICmdPrintf(aWorkspace, "Reading bus %s, addr 0x%02X, %d bytes:\n",
            busName, addr, num);

        // For now, assume 100 kHz is valid
        error = UEZI2CRead(i2c, addr, 100, data, num, 1000);
        if (error == UEZ_ERROR_NONE) {
            for (i=0; i<num; i++) {
                FDICmdPrintf(aWorkspace, "%02X ", data[i]);
                if ((i%15)==7)
                    FDICmdPrintf(aWorkspace, "- ");
                if ((i%15)==15)
                    FDICmdPrintf(aWorkspace, "\n");
            }
            if ((num%15)!=15)
                FDICmdPrintf(aWorkspace, "\n");
            FDICmdSendString(aWorkspace, "PASS: OK\n");
        } else if (error == UEZ_ERROR_NAK) {
            FDICmdSendString(aWorkspace, "FAIL: NAK\n");
        } else if (error == UEZ_ERROR_TIMEOUT) {
            FDICmdSendString(aWorkspace, "FAIL: Timeout!\n");
        } else {
            FDICmdPrintf(aWorkspace, "FAIL: Error #%d\n", error);
        }

        UEZI2CClose(i2c);
    } else {
        FDICmdSendString(aWorkspace,
            "FAIL: Incorrect parameters.\nI2CREAD <bus number> <addr> <num>\n");
    }

    return UEZ_ERROR_NONE;
}


/*-------------------------------------------------------------------------*
 * End of File:  FDICmd_I2C.c
 *-------------------------------------------------------------------------*/
