/*-------------------------------------------------------------------------*
 * File:  FDICmd.c
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
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
 /**
 *    @addtogroup FDICmd
 *  @{
 *  @brief     FDI Command Console
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    Provides basic control of a serial port in the form of input
 *     through lines (with some editing).  All output can be directed
 *     through the console commands (ConsolePrintf).
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <uEZ.h>
#include <uEZStream.h>
#include "FDICmd.h"

/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/
typedef struct {
    T_uezDevice iStream;
    T_uezTask iTask;

    // Current line being built by the console
    char iCmd[FDICMD_MAX_LINE_LENGTH+1];
    char iLastCmd[FDICMD_MAX_LINE_LENGTH+1];

    // Current length of line being built by console
    TUInt32 iCmdLen;

    // Flag to note if prompt should be shown on the next call to ConsoleUpdate().
    TBool iNeedPrompt;

    // Semaphore to block single access to the console command processor
    T_uezSemaphore iSemCmdProcess;

    // Simple flags to determine if we are/have stopping/stopped
    volatile TBool iStop;
    volatile TBool iStopped;

    // Pointer to a list of console commands
    const T_consoleCmdEntry *iCommandTable;
} T_FDICmdWorkspace;

static void strupr(char *string)
{
    while (*string)  {
        *string = toupper(*string);
        string++;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdUValue
 *---------------------------------------------------------------------------*/
/**
 *  Convert the given string into a 32-bit unsigned value.  If the prefix
 *      is "0x" or "$", use hex, otherwise parse as a decimal value.
 *
 *	@param [in] 	*aArg			String with argument to convert
 *  
 *	@return			TUInt32			Value of string   
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt32 FDICmdUValue(const char *aArg)
{
    TBool isHex=0;
    TUInt32 v=0;

    if ((aArg[0] == '0') && (aArg[1] == 'x')) {
        aArg += 2;
        isHex = 1;
    } else if (aArg[0] == '$') {
        aArg++;
        isHex = 1;
    }
    if (isHex)
        sscanf(aArg, "%x", &v);
    else
        sscanf(aArg, "%d", &v);

    return v;
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdSendString
 *---------------------------------------------------------------------------*/
/**
 *  Output string of characters to the default console.
 *
 *	@param [in]		*aWorkspace		Pointer to workspace
 *                          
 *	@param [in]		*aString		Character string to output.
 *
 *	@return			T_uezError		UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FDICmdSendString(void *aWorkspace, const char *aString)
{
    T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)aWorkspace;
    char c;
    TUInt32 numWritten;
    T_uezError error = UEZ_ERROR_NONE;

    // Go through the line character by character
    // and convert \n into \r\n for proper standard conversion and
    // ignore extra \r's
    while ((c=*aString++)!=0)  {
        if (c == '\n') {
            // Convert \n into \r\n
            error = UEZStreamWrite(
                p->iStream,
                "\r\n",
                2,
                &numWritten,
                UEZ_TIMEOUT_INFINITE);
        } else if (c == '\r') {
            // Ignore \r
        } else {
            // Output all other characters individually
            error = UEZStreamWrite(
                p->iStream,
                &c,
                1,
                &numWritten,
                UEZ_TIMEOUT_INFINITE);
        }
        if (error)
            return error;
    }
    // Flush the output and wait for the string to all go out
    return UEZStreamFlush(p->iStream);
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdPrintf
 *---------------------------------------------------------------------------*/
/**
 *  General command similar to printf but directs the output to the
 *      current console device.
 *
 *	@param [in] 	*aWorkspace		Pointer to workspace
 *                          
 *	@param [in]		aFormat			printf formatted text
 *
 *	@return			T_uezError		UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FDICmdPrintf(void *aWorkspace, const char *aFormat, ...)
{
    char buffer[FDICMD_PRINTF_BUFFER_SIZE];
    va_list marker;

    // Convert to a string for output
    va_start(marker, aFormat);
    vsprintf(buffer, aFormat, marker);
    va_end(marker);

    // Output string
    return FDICmdSendString(aWorkspace, buffer);
}

/*---------------------------------------------------------------------------*
 * Routine:  IFDICmdBuildCmd
 *---------------------------------------------------------------------------*/
/**
 *  User entered a character.  Add the character to the end of the line,
 *      or process the special control character.  If time to process the
 *      command, return TRUE.
 *
 *  @param [in]    *aWorkspace	Pointer to workspace
 *
 *  @param [in]    c            Character to process on command line
 *
 *  @return        TBool     	TRUE if cmd ready (end of line char) else FALSE
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static TBool IFDICmdBuildCmd(void *aWorkspace, char c)
{
    T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)aWorkspace;
    char echo[2];

    // Read one character at a time
    // Put into the string
    if ((c == '\r') || (c == '\n')) {
        // End of line entered, return here
        FDICmdSendString(aWorkspace, "\r\n");
        return ETrue;
    } else if (c == '\b') {
        // Backspace hit
        if (p->iCmdLen > 0) {
            // Overwrite last character with a space if backing up
            FDICmdSendString(aWorkspace, "\b \b");
            p->iCmdLen--;
            p->iCmd[p->iCmdLen] = '\0';
        }
    } else {
        // Add if we have room, else do nothing
        if (p->iCmdLen < FDICMD_MAX_LINE_LENGTH) {
            // Show we are adding the character (echo)
            echo[0] = c;
            echo[1] = 0;
            FDICmdSendString(aWorkspace, echo);
            p->iCmd[p->iCmdLen++] = c;
            p->iCmd[p->iCmdLen] = '\0';
        }
    }
    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  IFindCommand
 *---------------------------------------------------------------------------*/
/**
 *  Find a command (by string name) in the command table
 *      that matches the given name (case sensitive).
 *
 *  @param [in]    *aWorkspace		Pointer to workspace
 *
 *  @param [in]    *aCmd   			Command name to find
 *
 *  @return        T_consoleCmd   	Pointer to command or 0
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_consoleCmd IFindCommand(void *aWorkspace, const char *aCmd)
{
    T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)aWorkspace;
    const T_consoleCmdEntry *p_node = p->iCommandTable;
    while (p_node->iName)  {
        if (strcmp(p_node->iName, aCmd) == 0)
            return p_node->iFunction;
        p_node++;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  FDICmdPrintCmds
 *---------------------------------------------------------------------------*/
/**
 *  Print command list
 *
 *  @param [in]    *aWorkspace		Pointer to workspace
 *
 *  @return        T_consoleCmd  	Pointer to command or 0
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_consoleCmd FDICmdPrintCmds(void *aWorkspace)
{
    T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)aWorkspace;
    const T_consoleCmdEntry *p_node = p->iCommandTable;
    while (p_node->iName)  {
		FDICmdPrintf(aWorkspace, "%s\r\n", p_node->iName);
        p_node++;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdProcessCmd
 *---------------------------------------------------------------------------*/
/**
 *   A command was entered in the form of a line of characters.
 *      Break the line down to it's component elements, look for a command,
 *      and if found, run it.  Otherwise, report the command not found.
 *
 *	@param [in] 	*aWorkspace		Pointer to workspace
 *                          
 *	@param [in]		*aCmd			Command line to process
 *
 *	@return			int				Result code from processing command.     
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
int FDICmdProcessCmd(void *aWorkspace, const char *aCmd)
{
    char *argv[FDICMD_MAX_PARAMETERS];
    char line[FDICMD_MAX_LINE_LENGTH+1];
    int argc = 0;
    char *p;
    int quote = 0;
    int started = 0;
    int result = -1;
    T_consoleCmd cmd;

    // Copy the line, we'll be modifying it
    strcpy(line, aCmd);

    // Parse the line into parameters
    p = line;
    while (*p)  {
        if (quote)  {
            // If inside a quote, just keep going
            if (*p == '\"')  {
                // Ending quote, zero it out and finish up this param
                quote = 0;
                *p = '\0';
                argc++;
                started = 0;
                // Stop here if too many parameters
                if (argc >= FDICMD_MAX_PARAMETERS)
                    break;
            }
        } else {
            if (isspace(*p))  {
                // Got a space
                // Zero out spaces as we move
                // to the next line
                *p = '\0';
                if (started)  {
                    // We are done with this parameter,
                    // move to the next
                    argc++;
                    started = 0;
                    // Stop here if too many parameters
                    if (argc >= FDICMD_MAX_PARAMETERS)
                        break;
                }
            } else {
                // Not a space, keep moving past it
                if (*p == '\"') {
                    // If we see a quote, note it, and skip over it
                    quote = 1;
                    p++;
                }
                if (!started)  {
                    // Found the start of a parameter
                    started = 1;
                    argv[argc] = p;
                }
            }
        }
        p++;
    }

    // Finish out last parameter
    if (started)  {
        argc++;
    }

    // Do we have any arguments?
    if (argc)  {
        // Find the matching command (case insensitive)
        strupr(argv[0]);
        cmd = IFindCommand(aWorkspace, argv[0]);
        if (cmd)  {
            // Run the command
            result = cmd(aWorkspace, argc, argv);
        } else {
            FDICmdPrintf(aWorkspace, "Unknown command: %s\n", argv[0]);
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdTask
 *---------------------------------------------------------------------------*/
/**
 *  Sits on the stream and waits for incoming characters.
 *      Build a command line using the received characters.
 *      If [ENTER] is pressed, process the command.
 *      If just [ENTER] is pressed, then use the last command.
 *
 *  @param [in]    aMyTask				Current Task
 *
 *  @param [in]    *aWorkspace    		Pointer to workspace 
 *
 *  @return        TUInt32				returns 0 currently
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt32 FDICmdTask(T_uezTask aMyTask, void *aWorkspace)
{
    T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)aWorkspace;
    char c;
    TUInt32 numRead;

    // Run until told to stop
    while (!p->iStop)  {
        if (p->iNeedPrompt)  {
            p->iNeedPrompt = EFalse;
            p->iCmdLen = 0;
            FDICmdSendString(p, "\n# ");
        }

        // Try to get a character (wait up to 50 ms)
        if (UEZStreamRead(p->iStream, &c, 1, &numRead, 50) != UEZ_ERROR_TIMEOUT) {

            if(c=='\0') {
                // Do Nothing
            }
            else if(c=='?') {
                // If '?' is received, print a the list of commands
				FDICmdSendString(aWorkspace, "\r\n\r\n");
				FDICmdPrintCmds(aWorkspace);
				p->iCmd[p->iCmdLen] = '\0';
				FDICmdPrintf(aWorkspace, "\r\n# %s", p->iCmd);
			}
			else if (IFDICmdBuildCmd(aWorkspace, c)) {
				// Stuff a character into the line
				
                // Only process lines with length
                if (p->iCmdLen)  {
                    // Store the last command
                    memcpy(p->iLastCmd, p->iCmd, sizeof(p->iLastCmd));
                    FDICmdProcessCmd(aWorkspace, p->iCmd);
                } else {
                    // User hit enter on a blank line.
                    // Is there a last command waiting?
                    if (p->iLastCmd[0])  {
                        // Yes, blank lines repeat the last command.
                        FDICmdPrintf(aWorkspace, "REPEAT: %s\n", p->iLastCmd);
                        memcpy(p->iCmd, p->iLastCmd, sizeof(p->iLastCmd));
                        FDICmdProcessCmd(aWorkspace, p->iCmd);
                    }
                }
                p->iNeedPrompt = ETrue;
            }
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdStart
 *---------------------------------------------------------------------------*/
/**
 *  Initialize a task to use the given stream as a console.
 *
 *  @param [in]    *aStream  			Open and configured stream device
 *
 *  @param [in]    **aWorkspace    		Pointer to receive workspace pointer
 *                                     	to be passed to FDICmdStop().  Pass 0
 *                                     	if not needed.
 *  @param [in]    aStackSizeBytes    	Stack size in bytes.  All commands
 *                                     	share the same stack.  Set this to
 *                                     	the maximum size stack.
 *  @param [in]    *aCommandTable 		Pointer to an array of commands.
 *
 *  @return        T_uezError       	Error code. UEZ_ERROR_NOT_OPEN if
 *                                     	stream is not open or 0.
 *                                     	UEZ_ERROR_OUT_OF_MEMORY if out of
 *                                     	memory.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FDICmdStart(
     T_uezDevice aStream,
     void **aWorkspace,
     TUInt32 aStackSizeBytes,
     const T_consoleCmdEntry *aCommandTable)
{
    T_FDICmdWorkspace *p;
    T_uezError error;

    // Make sure we have a stream opened
    if (!aStream)
        return UEZ_ERROR_NOT_OPEN;

    // Allocate the workspace
    p = UEZMemAlloc(sizeof(*p));
    if (!p)
        return UEZ_ERROR_OUT_OF_MEMORY;

    // Initialize the default parameters
    p->iStream = aStream;
    p->iCommandTable = aCommandTable;
    p->iCmdLen = 0;
    p->iNeedPrompt = ETrue;
    p->iLastCmd[0] = '\0';
    p->iSemCmdProcess = 0;
    p->iStop = EFalse;
    p->iStopped = EFalse;

    // Create a semaphore to ensure one command
    // and a time is processed
    error = UEZSemaphoreCreateBinary(&p->iSemCmdProcess);
    if (!error) {
        // Semaphore for command processing created
        // Create the task
        error = UEZTaskCreate(
            FDICmdTask,
            "CMD",
            aStackSizeBytes,
            p,
            UEZ_PRIORITY_NORMAL,
            &p->iTask);
    }

    // If any error, task is not running and we need to clean up
    if (error) {
        // Get rid of the semaphore
        if (p->iSemCmdProcess)
            UEZSemaphoreDelete(p->iSemCmdProcess);
        // Remove the memory used
        UEZMemFree(p);
        p = 0;
    }

    // Return the workspace pointer (if any)
    if (aWorkspace)
        *aWorkspace = (void *)p;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdStop
 *---------------------------------------------------------------------------*/
/**
 *  Use to stop a FDI Cmd externally.  *CURRENTLY NOT SUPPORTED!*
 *
 *  @param [in]   *aWorkspace       FDICmd Workspace currently in use.
 *
 *  @return       T_uezError    	Error code. UEZ_ERROR_NOT_SUPPORTED 
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FDICmdStop(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  FDICmdReadLine
 *---------------------------------------------------------------------------*/
/**
 *  Read line from command prompt
 *
 *	@param [in] 	*aWorkspace		Pointer to workspace
 *                          
 *	@param [in]		*aLine			Pointer to line to read
 *
 *	@param [in] 	aMaxLength		max lenght of line to read
 *                          
 *	@param [in] 	aTimeout		timeout time
 *                          
 *	@param [in]		aEcho			If true echos onto screen
 *
 *	@return			T_uezError		UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError FDICmdReadLine(
        void *aWorkspace,
        char *aLine,
        TUInt32 aMaxLength,
        TUInt32 aTimeout,
        TBool aEcho)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 numRead;
    TUInt32 len = 0;
    TUInt8 c;
    char echo[2];
    T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)aWorkspace;

    while (len+1 < aMaxLength) {
        // Read one character at a time
        error = UEZStreamRead(p->iStream, &c, 1, &numRead, aTimeout);

        // Stop on an error or timeout
        if (error)
            break;

        // Put into the string, stripping the final \r or \n
        if ((c == '\r') || (c == '\n')) {
            // End of line, stop here
            if (aEcho)
                FDICmdSendString(aWorkspace, "\r\n");
            break;
        } else if (c == '\b') {
            // Backspace hit
            if (len > 0) {
                // Overwrite last character with a space if backing up
                if (aEcho)
                    FDICmdSendString(aWorkspace, "\b \b");
                len--;
                aLine[len] = '\0';
            }
        } else {
            // Add if we have room, else do nothing
            if (p->iCmdLen < FDICMD_MAX_LINE_LENGTH) {
                // Show we are adding the character (echo)
                if (aEcho) {
                    echo[0] = c;
                    echo[1] = 0;
                    FDICmdSendString(aWorkspace, echo);
                }
                aLine[len++] = c;
                aLine[len] = '\0';
            }
        }
    }

    return error;
}
/** @} */

/*-------------------------------------------------------------------------*
 * End of File:  FDICmd.c
 *-------------------------------------------------------------------------*/
