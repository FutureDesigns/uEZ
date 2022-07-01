/*-------------------------------------------------------------------------*
 * File:  HTTPServer.c
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup HTTPServer
 *  @{
 *  @brief     HTTP Server Implementation
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    HTTP Server Implementation
 *
 * @par Example code:
 * Example task to
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <uEZ.h>
#include <uEZFile.h>
#if ( RTOS == FreeRTOS)
#include <Source/RTOS/FreeRTOS/include/task.h>
#endif
#include "HTTPServer.h"
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>

#include <uEZINI.h>

#include "lwip/api.h"
#include "lwip/sockets.h"

//#define USE_VFILE // for virtual file system
#define ENABLE_HTML_PARSED_VARIABLES   1
#define ENABLE_HTML_QUESTION_IN_URL   0 // allow ? in URL

#ifdef USE_VFILE
extern T_uezError VFileOpen(
            const char * const aName, 
            TUInt32 aFlags,
            T_uezFile *aFile);

extern T_uezError VFileRead(
            T_uezFile aFile, 
            void *aBuffer, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesRead);

extern T_uezError VFileGetLength(T_uezFile aFile, TUInt32 *aLength);

extern T_uezError VFilePost(const char * aFileName,
              const char *aVarName,
              const char *aValue);

extern T_uezError VFileClose(T_uezFile aFile);
#else // use standard file api
#define VFileOpen UEZFileOpen
#define VFileRead UEZFileRead
#define VFileGetLength UEZFileGetLength
#define VFilePost UEZFilePost
#define VFileClose UEZFileClose
#endif

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
/* The size of the buffer in which the dynamic WEB page is created. */
#define MAX_PAGE_SIZE           1024

/* Standard GET response. */
#define HTTP_OK                 "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

#define  HTTP_MSG_NOT_FOUND \
        "<HTML>\r\n" \
        "<BODY>\r\n" \
        "<HEAD><TITLE>uEZ File not found</TITLE></HEAD>\r\n" \
        "<H1>uEZ File not found</H1>\r\n" \
        "The requested object does not exist on this server.\r\n" \
        "</BODY>\r\n" \
        "</HTML>\r\n"

#define  HTTP_MSG_FOUND \
        "<HTML>\r\n" \
        "<BODY>\r\n" \
        "<HEAD><TITLE>uEZ File found</TITLE></HEAD>\r\n" \
        "<H1>uEZ File found</H1>\r\n" \
        "The requested object does exist on this server.\r\n" \
        "</BODY>\r\n" \
        "</HTML>\r\n"

// when enabled prints information to console.
#ifndef DEBUG_HTTP_SERVER
#define DEBUG_HTTP_SERVER           	  0
#endif

// Stack size of the HTTP Server
#ifndef WEB_SERVER_STACK_SIZE
#define WEB_SERVER_STACK_SIZE  UEZ_TASK_STACK_BYTES(2048)
#endif

// Displayed version number of the HTTP Server
#ifndef WEB_SERVER_REPORT_VERSION
#define WEB_SERVER_REPORT_VERSION         0
#endif
		
#define MAX_LINE_LENGTH           256 // Max length of message to send/receive per packet
#define MAX_HTTP_VERSION          30  // Supported version of HTTP advertised by the server
#define MAX_HTTP_CONTENT_TYPE     120 // Type of content supplied from the HTTP Server
#define MAX_HTTP_CONTENT_BOUNDARY 80  // Content boundary of the HTTP Server
#define MAX_VAR_NAME_LENGTH       20  // Maximum length of variable name
#define HTTP_WRITE_BUFFER_SIZE    256 // Max length of write buffer size

#if DEBUG_HTTP_SERVER
	#define dprintf printf
#else
	#if (COMPILER_TYPE == RenesasRX)
    	void dprintf(const char * temp, ...);
		void dprintf(const char * temp, ...){
			; // do nothing	
		}
	#else
		#define dprintf(...)
	#endif
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct _T_httpStateVar {
    struct _T_httpStateVar *iNext;
    char *iName;
    char *iValue;
} T_httpStateVar;

typedef struct {
    int32_t iSocket; //Lwip socket, -1 if not active
    TUInt8 iReceiveBuffer[512];
    TUInt32 iReceiveIndex;
    TUInt32 iReceiveLength;
    TUInt32 iLength;
    TUInt8 iLine[MAX_LINE_LENGTH + 1];
    TUInt32 iFirstLineLength;
    TUInt8 iFirstLine[MAX_LINE_LENGTH + 1];
    TUInt8 iHTTPVersion[MAX_HTTP_VERSION + 1];
    TUInt8 iVarName[MAX_VAR_NAME_LENGTH + 1];
    TUInt32 iContentLength;
    T_HTTPCallbackGetVarFunc iGetFunc;
    T_HTTPCallbackSetVarFunc iSetFunc;
    T_httpStateVar *iVarList;
    TUInt8 iWriteBuffer[HTTP_WRITE_BUFFER_SIZE];
    TUInt32 iWriteLen;
    TUInt8 iContentType[MAX_HTTP_CONTENT_TYPE+1];
    TUInt8 iContentBoundary[MAX_HTTP_CONTENT_BOUNDARY+1];
    const char *iDrivePrefix;
} T_httpState;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static uint8_t writesTillDelay = 5;


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern void FuncTestPageHit(void);

static T_uezError IConvertErrorCode(err_t aLWIPError)
{
    switch (aLWIPError) {
        case ERR_OK: /* No error, everything OK. */
            return UEZ_ERROR_NONE;
        case ERR_MEM: /* Out of memory error.     */
            return UEZ_ERROR_OUT_OF_MEMORY;
        case ERR_BUF: /* Buffer error.            */
            return UEZ_ERROR_BUFFER_ERROR;
        case ERR_RTE: /* Routing problem.         */
            return UEZ_ERROR_INTERNAL_ERROR;
        case ERR_ABRT: /* Connection aborted.      */
            return UEZ_ERROR_ABORTED;
        case ERR_RST: /* Connection reset.        */
            return UEZ_ERROR_RESETED;
        case ERR_CLSD: /* Connection closed.       */
            return UEZ_ERROR_CLOSED;
        case ERR_CONN: /* Not connected.           */
            return UEZ_ERROR_COULD_NOT_CONNECT;
        case ERR_VAL: /* Illegal value.           */
            return UEZ_ERROR_ILLEGAL_VALUE;
        case ERR_ARG: /* Illegal argument.        */
            return UEZ_ERROR_ILLEGAL_ARGUMENT;
        case ERR_USE: /* Address in use.          */
            return UEZ_ERROR_ALREADY_EXISTS;
        case ERR_IF: /* Low-level netif error    */
            return UEZ_ERROR_INTERNAL_ERROR;
        case ERR_ISCONN: /* Already connected.       */
            return UEZ_ERROR_ALREADY_OPEN;
        case ERR_TIMEOUT: /* Timeout.                 */
            return UEZ_ERROR_TIMEOUT;
        case ERR_INPROGRESS: /* Operation in progress    */
            return UEZ_ERROR_BUSY;
    }
    return UEZ_ERROR_UNKNOWN;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPStateAlloc
 *---------------------------------------------------------------------------*/
/**
 *  Allocte a HTTP state block using the given connection
 *
 *  @param [in]    *aConn		Connection for this state to get data.
 *
 *  @param [in]    aNetwork		uEZ network device
 *
 *  @param [in]    aSocket		uEZ network socket
 *
 *  @return        *T_httpState Created state or 0 if out of memory.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_httpState *IHTTPStateAlloc(int32_t aSocket)
{
    T_httpState *p = UEZMemAlloc(sizeof(T_httpState));
    if (!p)
        return p;
    memset(p, 0xCC, sizeof(T_httpState));
    p->iSocket = aSocket;
    p->iLength = 0;
    p->iFirstLineLength = 0;
    p->iVarList = 0;
    p->iGetFunc = 0;
    p->iSetFunc = 0;
    p->iWriteLen = 0;
    return p;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPStateFree
 *---------------------------------------------------------------------------*/
/**
 *  Free the HTTP State previously allocated
 *
 *  @param [in]    *aState 	State to free
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IHTTPStateFree(T_httpState *aState)
{
    T_httpStateVar *p_var;
    T_httpStateVar *p_next;

    aState->iLength = 0;
    aState->iFirstLineLength = 0;

    // Release memory used by the HTTP state (if any)
    for (p_var = aState->iVarList; p_var; p_var = p_next) {
        p_next = p_var->iNext;
        UEZMemFree(p_var->iValue);
        UEZMemFree(p_var->iName);
        UEZMemFree(p_var);
    }
    aState->iVarList = 0;

    UEZMemFree(aState);
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPFindVar
 *---------------------------------------------------------------------------*/
/**
 *  Search for a match of variable names in the list of variables
 *      on this HTTP state.
 *      NOTE: Case is sensitive
 *
 *  @param [in]    *aState 			Pointer to state
 *
 *  @param [in]    *aName 			Name to search for
 *
 *  @return        T_httpStateVar*	Pointer to found match, else 0
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_httpStateVar *IHTTPFindVar(T_httpState *aState, const char *aName)
{
    T_httpStateVar *p_var;

    // Walk the list looking for a match and return it if found
    for (p_var = aState->iVarList; p_var; p_var = p_var->iNext) {
        if (strcmp(aName, p_var->iName) == 0)
            break;
    }

    return p_var;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPFreeVar
 *---------------------------------------------------------------------------*/
/**
 *  Remove the var from the linked list
 *
 *  @param [in]    *aState 		Pointer to state
 *
 *  @param [in]    *aVar 		Var to be deleted
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IHTTPFreeVar(T_httpState *aState, T_httpStateVar *aVar)
{
    T_httpStateVar *p_var;
    T_httpStateVar *p_prev = 0;

    // Walk the list looking for a match and if found, free it
    for (p_var = aState->iVarList; p_var; p_var = p_var->iNext) {
        if (p_var == aVar) {
            // Unlink it
            if (p_prev) {
                p_prev->iNext = p_var->iNext;
            } else {
                aState->iVarList = p_var->iNext;
            }

            // Delete its and fields and then itself
            UEZMemFree(p_var->iValue);
            UEZMemFree(p_var->iName);
            UEZMemFree(p_var);
            break;
        }
        p_prev = p_var;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  HTTPServerGetVar
 *---------------------------------------------------------------------------*/
/**
 *  Get a variable in the list of vars attached to this state
 *
 *	@param [in] *aHTTPState		HTTP session in progress
 *
 *	@param [out] *aVar 			Variable name to search for
 *
 *	@return		*char			Returns variable's data, or 0 if not found.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
const char *HTTPServerGetVar(void *aHTTPState, const char *aVar)
{
    T_httpState *p_state = (T_httpState *)aHTTPState;
    T_httpStateVar *p_var;

    // Determine if this var already exists.  If it does, replace
    // the previous definition.
    p_var = IHTTPFindVar(p_state, aVar);
    if (!p_var)
        return 0;
    return p_var->iValue;
}

/*---------------------------------------------------------------------------*
 * Routine:  HTTPServerSetVar
 *---------------------------------------------------------------------------*/
/**
 *   Set a variable in the list of vars attached to this state
 *
 *	@param [in] *aHTTPState	Pointer to state of HTTP server
 *
 *	@param [in]	*aVar		Poitner to variable
 *
 *	@param [in] *aValue		Pointer to value
 *
 *	@return		T_uezError	error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError HTTPServerSetVar(
    void *aHTTPState,
    const char *aVar,
    const char *aValue)
{
    T_httpState *p_state = (T_httpState *)aHTTPState;
    T_httpStateVar *p_var;
    TUInt32 fieldLen = strlen(aValue) + 1;
    char *field;

    // Determine if this var already exists.  If it does, replace
    // the previous definition.
    p_var = IHTTPFindVar(p_state, aVar);
    if (p_var) {
        // Free the old value
        UEZMemFree(p_var->iValue);
        p_var->iValue = 0;

        // Allocate the new field
        field = UEZMemAlloc(fieldLen);
        if (field) {
            // Got it, copy over the data and store
            strcpy(field, aValue);
            p_var->iValue = field;
        } else {
            // Failed to get it!  Get rid of this variable
            // and report an error
            IHTTPFreeVar(p_state, p_var);
            return UEZ_ERROR_OUT_OF_MEMORY;
        }
    } else {
        // New field, let's add it
        p_var = UEZMemAlloc(sizeof(T_httpStateVar));
        if (!p_var) {
            // We don't have enough memory to add a link
            return UEZ_ERROR_OUT_OF_MEMORY;
        }
        // We got the variable, now let's allocate
        // the name
        field = UEZMemAlloc(strlen(aVar) + 1);
        if (!field) {
            // COuld not allocate memory for the name
            // Fail after a bit of clean up
            UEZMemFree(p_var);
            return UEZ_ERROR_OUT_OF_MEMORY;
        }
        // Store the variable name here
        p_var->iName = field;
        strcpy(p_var->iName, aVar);

        // Now that we have a name, let's try to add to the data field
        field = UEZMemAlloc(fieldLen);
        if (!field) {
            // Not enough room, back out
            UEZMemFree(p_var->iName);
            UEZMemFree(p_var);
            return UEZ_ERROR_OUT_OF_MEMORY;
        }

        // Got the field too, store it here
        p_var->iValue = field;
        strcpy(p_var->iValue, aValue);

        // Add to the list
        p_var->iNext = p_state->iVarList;
        p_state->iVarList = p_var;
    }

    // Got here, it was successful
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPParameter
 *---------------------------------------------------------------------------*/
/**
 *  Look at a line of HTTP header text and parse out the 0-based
 *      nth parameter entry.
 *
 *  @param [in]    *aLine 		Line to parse
 *
 *  @param [in]    aLength 		Length of above line
 *
 *  @param [in]    aIndex 		Nth entry to get (0=first, 1=second, etc.)
 *
 *  @param [in]    aParameter 	Place to store parsed item.  Will be null string
 *              				in all cases where an item cannot be parsed.
 *  @return
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IHTTPParameter(
    const TUInt8 *aLine,
    TUInt32 aLength,
    TUInt32 aIndex,
    TUInt8 *aParameter)
{
    TUInt32 field = 0;
    TUInt32 i;
    TBool lastWasSpace = ETrue;

    // Filename is between two sets of spaces
    for (i = 0; i < aLength; i++) {
        if (isspace(aLine[i])) {
            // If this ends the field, increment to the next field
            if (!lastWasSpace) {
                field++;
                // Stop early if we got our parameter
                if (field > aIndex)
                    break;
            }
            lastWasSpace = ETrue;
        } else {
            // Got a non-space, copy it over if we are
            // on the 2nd parameter (1)
            if (field == aIndex)
                (*(aParameter++)) = aLine[i];
            lastWasSpace = EFalse;
        }
    }
    *aParameter = '\0';

}

static T_uezError IHTTPWriteFlush(T_httpState *aState, TBool aMoreToCome)
{
    T_uezError error;
    uint32_t start;
    int32_t sent;
    DEBUG_SV_Printf("FS");
    start = UEZTickCounterGet();
    dprintf("FlushStart: %d (%d)\n", start, aState->iWriteLen);
    UEZTaskDelay(1); // Will crash if flushstart debug code is not running, testing delay fix
    sent = send(aState->iSocket, aState->iWriteBuffer, aState->iWriteLen, 0);

    aState->iWriteLen = 0;
    dprintf("FlushEnd: %d\n", UEZTickCounterGet()-start);
    UEZTaskDelay(2); // Will crash if flushend debug code is not running, testing delay fix
    start--;
    DEBUG_SV_Printf("FE");
    if(sent < 0)
    {
        error = UEZ_ERROR_TIMEOUT;
    }
    else
    {
        error = UEZ_ERROR_NONE;
    }

    return error;
}

static T_uezError IHTTPWriteByte(T_httpState *aState, TUInt8 aByte)
{
    T_uezError error = UEZ_ERROR_NONE;

    // Flush if we are full (and reset write len)
    if (aState->iWriteLen >= HTTP_WRITE_BUFFER_SIZE)
        error = IHTTPWriteFlush(aState, ETrue);
    aState->iWriteBuffer[aState->iWriteLen++] = aByte;

    return error;
}

static T_uezError IHTTPWrite(
    T_httpState *aState,
    char *aData,
    TUInt32 aLength,
    TBool aMoreToCome)
{
    TUInt32 i;
    T_uezError error = UEZ_ERROR_NONE;
    writesTillDelay--;

    for (i=0; i<aLength; i++) {
        error = IHTTPWriteByte(aState, aData[i]);
        if (error)
            break;
    }

    // If this is the end, push it out with no more behind it
    if ((!error) && (!aMoreToCome))
        error = IHTTPWriteFlush(aState, EFalse);

    DEBUG_SV_Printf("WE");    
    if(writesTillDelay == 0) {
      //UEZTaskDelay(1);
      writesTillDelay = 5;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPReportError
 *---------------------------------------------------------------------------*/
/**
 *  Report an error over the browser of the given type.  No data other
 *      than the error is reported.
 *
 *  @param [in]    *aState 		State of HTTP request
 *
 *  @param [in]    aCode 		Status code to use in header
 *
 *  @param [in]    *aState 		Status string
 *
 *  @param [in]    *aText 		HTML text of error
 *
 *  @return        T_uezError 	Error code if any.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IHTTPReportError(
    T_httpState *aState,
    TUInt32 aCode,
    const char *aStatus,
    const char *aText)
{
    //    TUInt32 content_len = strlen(aText);
    char *line = (char *)aState->iFirstLine;

    sprintf(line, "%s %u %s\r\n", aState->iHTTPVersion, aCode, aStatus);

    IHTTPWrite(aState, line, strlen(line), ETrue);

    sprintf(line, "Content-Type: text/html\r\n");
    IHTTPWrite(aState, line, strlen(line), ETrue);

    sprintf(line, "Content-Length: %d\r\n\r\n", strlen(aText));
    IHTTPWrite(aState, line, strlen(line), ETrue);

    // Now send the error msg (out of ROM)
    IHTTPWrite(aState, (char *)aText, strlen(aText), EFalse);

    // Send back a bogus error to end this
    return UEZ_ERROR_STALL;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPOutputHeader
 *---------------------------------------------------------------------------*/
/**
 *  Create a HTTP response header.
 *
 *  @param [in]    *aState			State of HTTP request
 *
 *  @param [in]    aCode 			Status code to use in header, usually 200
 *
 *  @param [in]    *aState			Status string, usually "OK"
 *
 *  @param [in]    *aMIMEType 		Type of data reponse is to follow as
 *
 *  @param [in]    aContentLength 	Length in bytes of upcoming response
 *
 *  @return        T_uezError 		Error code if any.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IHTTPOutputHeader(
    T_httpState *aState,
    TUInt32 aCode,
    const char *aStatus,
    const char *aMIMEType,
    TUInt32 aContentLength)
{
    char *line = (char *)aState->iFirstLine;

    sprintf(line, "%s %u %s\r\n", aState->iHTTPVersion, aCode, aStatus);
    dprintf("Response: %s", line);
    IHTTPWrite(aState, line, strlen(line), ETrue);

    sprintf(line, "Content-Type: %s\r\n", aMIMEType);
    dprintf("Response: %s", line);
    IHTTPWrite(aState, line, strlen(line), ETrue);

    sprintf(line, "Content-Length: %u\r\n\r\n", aContentLength);
    dprintf("Response: %s", line);
    IHTTPWrite(aState, line, strlen(line), ETrue);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ICompareStringInsensitive
 *---------------------------------------------------------------------------*/
/**
 *  Do an insensitive compare of two strings
 *
 *  @param [in]    *aString1   First string to compare
 *
 *  @param [in]    *aString2   Second string to compare
 *
 *  @return        int32_t		   Sign represents direction of aString1 ? aString2
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static int32_t ICompareStringInsensitive(const char *aString1, const char *aString2)
{
    char c1, c2;
    int32_t v;

    do {
        c1 = *aString1++;
        c2 = *aString2++;
        /* The casts are necessary when pStr1 is shorter & char is signed */
        v = (TUInt8)tolower(c1) - (TUInt8)tolower(c2);
    } while ((v == 0) && (c1 != '\0') && (c2 != '\0'));

    return v;
}

/*---------------------------------------------------------------------------*
 * Routine:  IDetermineMimeTypeFromFilename
 *---------------------------------------------------------------------------*/
/**
 *  Based on the filename, determine the mime type.
 *
 *  @param [in]    *aFilename 	Filename in question
 *
 *  @return        char*		MIME string
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static const char *IDetermineMimeTypeFromFilename(const char *aFilename)
{
    typedef struct {
        const char *iPostfix;
        const char *iMimeType;
    } T_mimeType;

    const T_mimeType G_mimeTypes[] = {
      {"htm", "text/html" },
      {"html", "text/html" },
      {"jpg", "image/jpeg" },
      {"jpeg", "image/jpeg" },
      {"gif", "image/gif" },
      {"png", "image/png" },
      {"bmp", "image/bmp" },
      { "js", "text/javascript" },
      { "txt", "text/plain"},
      { "css", "text/css" },
      { "csv", "text/csv" },
      { "zip", "application/zip" },
      { "pdf", "application/pdf" },
      { "swf","application/x-shockwave-flash" },
      { "mp3", "audio/mpeg" },
      { "wav", "audio/wav" },
      { "ico", "image/vnd.microsoft.icon" },
      { "bin","application/octet-stream" },
      { 0, "application/octet-stream" }  // default, always at end
    };
    const char *p;
    const char *p_last;
    const T_mimeType *p_mime;

    // Find last period in the name (if any)
    p = aFilename;
    p_last = 0;
    do {
        p_last = p;
        p = strchr(p, '.');
        if (p)
            p++;
    } while (p);
    p = p_last;

    if (!p) {
        // No period?  Just make it this
        return "application/octet-stream";
    }

    p_mime = G_mimeTypes;
    do {
        if (ICompareStringInsensitive(p, p_mime->iPostfix) == 0)
            break;
        p_mime++;
    } while (p_mime->iPostfix);

    return p_mime->iMimeType;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPParseFileForVars
 *---------------------------------------------------------------------------*/
/**
 *  Parse the HTML file and parse it for variables that will be
 *      defined by the callback functions.  Variables are defined only
 *      once so they can be used more than once on a page but show the
 *      same result.
 *
 *  @param [in]    *aState  	State of the HTTP request and parsing.
 *
 *  @param [in]    aFile    	File to search for
 *
 *  @param [in]    *aLength   	Length of resulting file (used in
 *                                      Content-Length: of HTTP header)
 *
 *  @return        T_uezError 	Error if occurred.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IHTTPParseFileForVars(
    T_httpState *aState,
    T_uezFile aFile,
    TUInt32 *aLength)
{
    TUInt32 i;
    char name[MAX_VAR_NAME_LENGTH + 1];
    TUInt32 nameLen = 0;
    TBool inName = EFalse;
    TUInt32 newLength = 0; // 0 if outside, 1 if saw $, 2 if saw { and inside
    TUInt8 c;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 numRead;
    T_httpStateVar *p_var;

    for (i = 0; i < *aLength; i++) {
        // Put in c the next character
        error = VFileRead(aFile, &c, 1, &numRead);
        if (error != UEZ_ERROR_NONE)
            break;

        // Base on our state, what do we do?
        if (inName == 0) {
            if (c == '$') {
                inName = 1;
            } else {
                // Valid character, add it
                newLength++;
            }
        } else if (inName == 1) {
            // We got a $, do we get the {?
            if (c == '{') {
                inName = 2;
                // We are now officially inside a var name
                // Reset the name length
                nameLen = 0;
            } else {
                // didn't get the {, must have just been a rogue $
                newLength++;
                // Was this a '$'?
                if (c == '$') {
                    // try again
                } else {
                    // Nope, didn't start it.  Count this character
                    // and start over.
                    newLength++;
                    inName = 0;
                }
            }
        } else if (inName == 2) {
            // At this point, we've seen ${ already
            // Now look for } to end it.  Meanwhile, build a name
            if (c == '}') {
                // Found the end!
                // Let's go ahead and find this variable and get the data
                name[nameLen] = '\0';

                // Has this already been defined?
                if (!IHTTPFindVar(aState, name)) {
                    // Request the variable (it will then call HTTPServerSetVar
                    // on this state).
                    error = aState->iGetFunc(aState, name);
                    // If an error is reported, stop here
                    if (error)
                        break;
                }

                // Look up the new version of this
                // Now add length equal to the size of that variable
                p_var = IHTTPFindVar(aState, name);
                if ((p_var) && (p_var->iValue))
                    newLength += strlen(p_var->iValue);

                // Back to the main mode, looking
                inName = 0;
            } else {
                // The name is still growing.  Keep adding to it
                // (if we have not reached the limit, otherwise clip)
                if (nameLen < MAX_VAR_NAME_LENGTH) {
                    name[nameLen++] = c;
                }
            }
        }
    }
    // Note how long the final version is/will be.
    *aLength = newLength;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPOutputParsedFile
 *---------------------------------------------------------------------------*/
/**
 *  Parse the HTML for a second time, but this time output each character
 *      to the network and replace variables with their values.
 *
 *  @param [in]    *aState         	State of the HTTP request and parsing.
 *
 *  @param [in]    aFile            File to search for
 *
 *  @param [in]    aLength      	Length of resulting file (used in
 *                                      Content-Length: of HTTP header)
 *  @return        T_uezError     	Error if occurred
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IHTTPOutputParsedFile(
    T_httpState *aState,
    T_uezFile aFile,
    TUInt32 aLength)
{
    TUInt32 i;
    char name[MAX_VAR_NAME_LENGTH + 1];
    TUInt32 nameLen = 0;
    TBool inName = EFalse;
    TUInt8 c;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 numRead;
    T_httpStateVar *p_var;
    TUInt32 fileLen = 0;

    VFileGetLength(aFile, &fileLen);

    for (i = 0; (i < fileLen) && (error == UEZ_ERROR_NONE); i++) {
        // Put in c the next character
        error = VFileRead(aFile, &c, 1, &numRead);
        if (error != UEZ_ERROR_NONE)
            break;

        // Base on our state, what do we do?
        if (inName == 0) {
            if (c == '$') {
                inName = 1;
            } else {
                // Valid character, output it
                error = IHTTPWrite(aState, (char *)&c, 1, ETrue);
            }
        } else if (inName == 1) {
            // We got a $, do we get the {?
            if (c == '{') {
                inName = 2;
                // We are now officially inside a var name
                // Reset the name length
                nameLen = 0;
            } else {
                // didn't get the {, must have just been a rogue $
                // Output the rogue '$'
                IHTTPWrite(aState, "$", 1, ETrue);
                // Was this a '$'?
                if (c == '$') {
                    // try again
                } else {
                    // Nope, didn't start it.  Count this character
                    // and start over.
                    error = IHTTPWrite(aState, (char *)&c, 1, ETrue);
                    inName = 0;
                }
            }
        } else if (inName == 2) {
            // At this point, we've seen ${ already
            // Now look for } to end it.  Meanwhile, build a name
            if (c == '}') {
                // Found the end!
                // Let's go ahead and find this variable and get the data
                name[nameLen] = '\0';

                p_var = IHTTPFindVar(aState, name);
                // Has this already been defined?
                if (!p_var) {
                    // Missing variable!  Where'd it go?
                    break;
                }

                // Found the name, output it's value
                if (p_var->iValue)
                    error = IHTTPWrite(aState, p_var->iValue, strlen(p_var->iValue),
                        ETrue);

                // Back to the main mode, looking
                inName = 0;
            } else {
                // The name is still growing.  Keep adding to it
                // (if we have not reached the limit, otherwise clip)
                if (nameLen < MAX_VAR_NAME_LENGTH) {
                    name[nameLen++] = c;
                }
            }
        }
    }
    if (!error) {
        // End of the parsing, flush out the last bit
        //TODO: needed? error = IHTTPWriteFlush(aState, EFalse);
    } else {
        // Reset the write buffer, its invalid
        aState->iWriteLen = 0;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPGet
 *---------------------------------------------------------------------------*/
/**
 *  Get one character from the network, or wait.
 *
 *  @param [in]    *aState   	State of the HTTP request and parsing.
 *
 *  @return        T_uezError   Error if failed.  If no more data,
 *                                      returns UEZ_ERROR_NOT_ENOUGH_DATA.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IHTTPGet(T_httpState *aState)
{
#if (ENABLE_HTML_QUESTION_IN_URL == 1)
    uint16_t i; 
#endif
    T_uezFile file;
    TUInt32 len, totalLengthLeft;
    TBool moreToCome;
    TUInt8 *block = aState->iFirstLine;
    const char *mimeType;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 prefixLen;

    // Parse out the filename and store in the first line field
    // with a prefix (given in the iDrivePrefix)
    strcpy((char *)aState->iLine, aState->iDrivePrefix);
    prefixLen = strlen((char *)aState->iLine);
    IHTTPParameter(aState->iFirstLine, aState->iFirstLineLength, 1,
        aState->iLine + prefixLen /*length of "/httproot"*/);

    // Check to see if we are blank?  If so, go to the default index.htm file.
    if (strcmp((char *)aState->iLine + prefixLen, "/") == 0)
        strcpy((char *)aState->iLine + prefixLen, "/INDEX.HTM");
    // Get the version
    IHTTPParameter(aState->iFirstLine, aState->iFirstLineLength, 2,
        aState->iHTTPVersion);

    // Filename we need to get is now in aState->iFirstLine.
    // Let's see if the file can be opened
#if (ENABLE_HTML_QUESTION_IN_URL == 1) // added to  allow ? etc on url line
    for(i=0;(i<40)&&(*((char *)(aState->iLine+i))!=0);i++)
    if(*((char*)aState->iLine + i) =='?')
    {
     *((char *)aState->iLine + i) = 0;
     break;
    } // probably a better way to implement this such as ignoring ? in FS but only for end of file
#endif
    if (VFileOpen((char *)aState->iLine, FILE_FLAG_READ_ONLY, &file)
        == UEZ_ERROR_NONE) {
        len = 0;
        VFileGetLength(file, &len);
        totalLengthLeft = len; // total file length
        dprintf("\r\nFetching file: %s (size %d)\r\n", aState->iLine, len);
        // File exists!
        mimeType = IDetermineMimeTypeFromFilename((char *)aState->iLine);
        // If there is a function to parse types and this a HTML file
        // then we need to parse the data, getting values as found, and
        // determine the real length

#if (ENABLE_HTML_PARSED_VARIABLES == 1)
         if ((aState->iGetFunc) && (strcmp(mimeType, "text/html") == 0)) {
            error = IHTTPParseFileForVars(aState, file, &len);
            // Close the file and open again (if no errors)
            if (error == UEZ_ERROR_NONE) {
                VFileClose(file);
                // Reopen the file so it can be parsed
                error = VFileOpen((char *)aState->iLine, FILE_FLAG_READ_ONLY,
                    &file);
            }
        }
#endif
        if (error == UEZ_ERROR_NONE) {
            IHTTPOutputHeader(aState, 200, "OK", mimeType, len);
            // If there is a function to parse the vars in a HTML file,
            // we now use the previously parsed vars and data to output
#if (ENABLE_HTML_PARSED_VARIABLES == 1) 
            if ((aState->iGetFunc) && (strcmp(mimeType, "text/html") == 0)) {
                // Output the parsed data
                IHTTPOutputParsedFile(aState, file, len);
            } else {
#endif
              do {
                // Read a section
                if (VFileRead(file, block, MAX_LINE_LENGTH, &len)!= UEZ_ERROR_NONE){
                  break;
                }
                if(totalLengthLeft <= MAX_LINE_LENGTH){
                  moreToCome = EFalse;
                  // Write a section
                  if (IHTTPWrite(aState, (char *)block, totalLengthLeft, moreToCome) != UEZ_ERROR_NONE){
                    break;
                  }
                } else {
                  moreToCome = ETrue;
                  // Write a section
                  if (IHTTPWrite(aState, (char *)block, len, moreToCome) != UEZ_ERROR_NONE){
                    break;
                  }
                }
                totalLengthLeft -= len; // subtract how many bytes were read
              } while (moreToCome == ETrue);
            }
#if (ENABLE_HTML_PARSED_VARIABLES == 1) 
        }
#endif
        VFileClose(file);
    } else {
        // File does NOT exist
        IHTTPReportError(aState, 404, "Not Found", HTTP_MSG_NOT_FOUND);
    }

    // Stop here
    return UEZ_ERROR_STALL;
}

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPReadByte
 *---------------------------------------------------------------------------*/
/**
 *  Get one character from the network, or wait.
 *
 *  @param [in]   *aState  State of the HTTP request and parsing.
 *
 *  @param [in]   *aChar   Fetch one character from the current network buffer.
 *
 *  @return    T_uezError  Error if failed.  If no more data,
 *                                      returns UEZ_ERROR_NOT_ENOUGH_DATA.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IHTTPReadByte(T_httpState *aState, char *aChar)
{
    T_uezError error = UEZ_ERROR_NONE;
    //TUInt32 start = UEZTickCounterGet();
    TUInt32 len;
    int32_t recved = 0;

    while (aState->iReceiveIndex >= aState->iReceiveLength) {
        // Fetch more data
        len = 0;

        //TODO: May need to add socket time out here
        //Or use NO_WAIT and handle timeout
        recved = recv(aState->iSocket, aState->iReceiveBuffer,
                aState->iReceiveLength, 0);

        if (recved > 0) {
            // See if we got any data?
            // Got some data, prepare it for reading
            aState->iReceiveIndex = 0;
            aState->iReceiveLength = len;
            error = UEZ_ERROR_NONE;
            break;
        } else {
            // No more data
            return UEZ_ERROR_NOT_ENOUGH_DATA;
        }
    }

    // Got data (if no error)
    if (!error)
        *aChar = aState->iReceiveBuffer[aState->iReceiveIndex++];

    return error;
}


static T_uezError IHTTPProcessURLEncodedFormData(T_httpState *aState)
{
    TUInt32 i;
    char c=0;
    TUInt8 mode = 0;
    TUInt32 varNameLength = 0;
    TUInt32 dataLength = 0;
    TUInt8 ch;
    T_uezError error = UEZ_ERROR_NONE;

    for (i = 0; i <= aState->iContentLength; i++) {
        if (i == aState->iContentLength) {
            c = '&';
        } else {
            error = IHTTPReadByte(aState, &c);
            if (error != UEZ_ERROR_NONE) {
                dprintf("  Early end!\r\n");
                break;
            }
        }

        switch (mode) {
            case 0:
                // Mode: Building var name
                // Have we reached the equal yet?
                if (c == '=') {
                    // Name is an equal.  End of name
                    aState->iVarName[varNameLength] = '\0';
                    dataLength = 0;
                    mode = 1;
                } else {
                    // Store another letter in the variable name
                    if (varNameLength < MAX_VAR_NAME_LENGTH)
                        aState->iVarName[varNameLength++] = c;
                }
                break;
            case 1:
                // Building data, looking for a normal character or & for end.
                if (c == '+') {
                    // Plusses become spaces
                    c = ' ';
                }
                if (c == '%') {
                    // % becomes a hex value, change modes
                    mode = 2;
                } else if (c == '&') {
                    // End of variable data!
                    aState->iLine[dataLength] = '\0';

                    dprintf("  %s = %s\r\n", aState->iVarName, aState->iLine);

                    // Request the data be processed (if there is a handler)
                    if (aState->iSetFunc)
                        aState->iSetFunc(aState,
                            (const char *)aState->iVarName,
                            (const char *)aState->iLine);

                    dataLength = 0;
                    varNameLength = 0;
                    mode = 0;
                } else {
                    // Normal stuffing of data here
                    if (dataLength < MAX_LINE_LENGTH)
                        aState->iLine[dataLength++] = c;
                }
                break;
            case 2:
                // Working on first part of a hex character
                c = toupper(c);
                if (c >= 'A')
                    ch = 10 + (c - 'A');
                else
                    ch = c - '0';
                ch <<= 4;
                mode = 3;
                break;
            case 3:
                // Working on the second part of a hex character
                c = toupper(c);
                if (c >= 'A')
                    ch |= (10 + (c - 'A'));
                else
                    ch |= (c - '0');
                // Now stuff the character and go on
                mode = 0;
                // Normal stuffing of data here
                if (dataLength < MAX_LINE_LENGTH)
                    aState->iLine[dataLength++] = c;
                break;
        }
    }

    return error;
}

#if 0 // in development
static void IProcessMultipartFormDataStart(T_httpState *aState)
{
    extern void HTTPServerToApp_Start(void);
    dprintf("IProcessMultipartFormDataStart\n");
    HTTPServerToApp_Start();
}

static void IProcessMultipartFormDataLine(T_httpState *aState, char *aLine)
{
    dprintf("IProcessMultipartFormDataLine: %s\n", aLine);
}

static void IProcessMultipartFormDataEnd(T_httpState *aState)
{
    extern void HTTPServerToApp_End(void);
    dprintf("IProcessMultipartFormDataEnd\n");
    HTTPServerToApp_End();
}

static void IProcessMultipartFormDataAdd(T_httpState *aState, char aChar)
{
    extern void HTTPServerToApp_AddChar(char aChar);
//    dprintf("[%02X %c]", aChar, (isalnum(aChar)?aChar:'_'));
    HTTPServerToApp_AddChar(aChar);
}


static T_uezError IHTTPProcessMultipartFormData(T_httpState *aState)
{
    TUInt32 i;
    char c;
    typedef enum {
        HTTP_MFD_MODE_LOOK_FOR_START,
        HTTP_MFD_MODE_LOOK_FOR_SECOND_START,
        HTTP_MFC_MODE_LOOK_FOR_BOUNDARY,
        HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_END_OR_CR,
        HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_END,
        HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_LF,
        HTTP_MFC_MODE_LOOK_FOR_CONTENT_LINE,
        HTTP_MFC_MODE_LOOK_FOR_CONTENT_LINE_LF,
        HTTP_MFC_MODE_LOOK_FOR_CONTENT_DATA,
        HTTP_MFC_MODE_LOOK_FOR_NOTHING,
    } T_httpMFDMode;
    #define MAX_MFC_LINE_LENGTH 100
    int32_t mode = HTTP_MFD_MODE_LOOK_FOR_START;
    T_uezError error;
    TUInt32 lineLength = 0;
    char line[MAX_MFC_LINE_LENGTH+1];
    int32_t numDashes;
    char boundaryCheck[MAX_HTTP_CONTENT_BOUNDARY+1];
    int32_t boundaryIndex;
    int32_t boundaryPos = 0;
    int32_t pos;

    for (pos = 0; pos < aState->iContentLength; pos++) {
        error = IHTTPReadByte(aState, &c);
        if (error != UEZ_ERROR_NONE) {
            dprintf("  Early end!\r\n");
            break;
        }

        //        dprintf("%d,%d ", pos, mode);
        switch (mode) {
            case HTTP_MFD_MODE_LOOK_FOR_START:
                if (c == '-') {
                    mode = HTTP_MFD_MODE_LOOK_FOR_SECOND_START;
                } else {
                    // Ignore this character
                }
                break;
            case HTTP_MFD_MODE_LOOK_FOR_SECOND_START:
                if (c == '-') {
                    // Now let's look for the boundary
                    boundaryIndex = 0;
                    mode = HTTP_MFC_MODE_LOOK_FOR_BOUNDARY;
                } else {
                    // Ignore this character, not the start
                    // Start over
                    mode = HTTP_MFD_MODE_LOOK_FOR_START;
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_BOUNDARY:
                if (c == aState->iContentBoundary[boundaryIndex]) {
                    boundaryIndex++;
                    if (aState->iContentBoundary[boundaryIndex] == '\0') {
                        // Done with boundary!  On to either a new line
                        // or a end marker
                        // Assume we are NOT an end marker
                        mode = HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_END_OR_CR;
                    } else {
                        // Keep checking this boundary
                    }
                } else {
                    // Boundary doesn't match.  Drop it here and start again
                    mode = HTTP_MFD_MODE_LOOK_FOR_START;
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_END_OR_CR:
                if (c == '\r') {
                     // We have found a boundary's CR, now for the LF
                    mode = HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_LF;
                } else if (c == '-') {
                     // Found a dash, we must be coming to an end
                    mode = HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_END;
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_END:
                if (c == '-') {
                    // We have found an end!
                    // Just eat any remaining characters
                    mode = HTTP_MFC_MODE_LOOK_FOR_NOTHING;
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_LF:
                if (c == '\n') {
                    // We are done with boundary, and now can go into lines
                    // of data!
                    mode = HTTP_MFC_MODE_LOOK_FOR_CONTENT_LINE;
                    lineLength = 0;
                } else {
                    // Don't know what this is!
                    // We've gotten so far, let's just stay here until we
                    // get a \n
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_CONTENT_LINE:
                // Work on feading in a line
                if (c == '\r') {
                    // End of the line!
                    line[lineLength] = '\0';
                    mode = HTTP_MFC_MODE_LOOK_FOR_CONTENT_LINE_LF;
                } else {
                    // Store the character if we have length
                    if (lineLength < MAX_MFC_LINE_LENGTH) {
                        line[lineLength++] = c;
                        line[lineLength] = '\0';
                    }
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_CONTENT_LINE_LF:
                if (c == '\n') {
                    // We are done with line
                    // is our line zero length or got data?
                    if (lineLength == 0) {
                        // Done with content line headers, we are now going
                        // into the raw data (until we hit a boundary)
                        IProcessMultipartFormDataStart(aState);
                        mode = HTTP_MFC_MODE_LOOK_FOR_CONTENT_DATA;
                        numDashes = 0;
                        boundaryIndex = 0;
                    } else {
                        IProcessMultipartFormDataLine(aState, line);
                        // See if we have another line
                        lineLength = 0;
                        mode = HTTP_MFC_MODE_LOOK_FOR_CONTENT_LINE;
                    }
                } else {
                    // Don't know what this is!
                    // We've gotten so far, let's just stay here until we
                    // get a \n
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_CONTENT_DATA:
                // Store the character, even if it is used in the boundary
                boundaryCheck[boundaryIndex++] = c;

                // Look for a leading \r\n followed by two dashes and THEN
                // check for the content boundary.
                if ((numDashes == 0) && (c == '\r')) {
                    numDashes++;
                } else if ((numDashes == 1) && (c == '\n')) {
                    numDashes++;
                } else if ((numDashes == 2) && (c == '-')) {
                    numDashes++;
                } else if ((numDashes == 3) && (c == '-')) {
                    numDashes++;
                    boundaryPos = 0;
                } else if (numDashes == 4) {
                    // The first four characters are correctly \r\n--, now
                    // check the boundary one character at a time.

                    // Does this last character match?
                    if (c == aState->iContentBoundary[boundaryPos]) {
                        // We have a match!
                        boundaryPos++;
                        // Have we reached the end of the boundary?
                        if (aState->iContentBoundary[boundaryPos] == '\0') {
                            // Yes, end of boundary
                            // This is a boundary match.
                            IProcessMultipartFormDataEnd(aState);
                            mode =
                                HTTP_MFC_MODE_LOOK_FOR_BOUNDARY_END_OR_CR;
                        } else {
                            // No, still need more in the boundary check
                            // and we have matched up to this point.
                            // Keep going in this state
                        }
                    } else {
                        // Just unmatching data, send out everything up to this point that
                        // was matching (should be at least one character)
                        for (i = 0; i < boundaryIndex; i++)
                            IProcessMultipartFormDataAdd(aState,
                                boundaryCheck[i]);
                        // reset it
                        boundaryIndex = 0;
                        numDashes = 0;
                        mode = HTTP_MFC_MODE_LOOK_FOR_CONTENT_DATA;
                    }
                } else {
                    if (numDashes == 0) {
                        // If we didn't even start to get a dash
                        // then just pass this on.
                        IProcessMultipartFormDataAdd(aState, c);
                        boundaryIndex = 0;
                    } else {
                        // Prefix failed to match \r\n--, dump out what we got up to.
                        for (i = 0; i < boundaryIndex; i++)
                            IProcessMultipartFormDataAdd(aState,
                                boundaryCheck[i]);

                        // reset it
                        boundaryIndex = 0;
                        numDashes = 0;
                        mode = HTTP_MFC_MODE_LOOK_FOR_CONTENT_DATA;
                    }
                }
                break;
            case HTTP_MFC_MODE_LOOK_FOR_NOTHING:
                // Eat characters, do nothing
                break;
        }
    }
    return error;

}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  IHTTPPost
 *---------------------------------------------------------------------------*/
/**
 *  A POST request is being performed.  After the header there is
 *      data (of Content-Length) to be grabbed.  Get this data and then
 *      handle the response the same way a GET is handled.
 *
 *  @param [in]    *aState        	State of the HTTP request and parsing.
 *
 *  @return        T_uezError   	Error if failed
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IHTTPPost(T_httpState *aState)
{

    // First, receive the remaining data up to the content length
    dprintf("POST DATA: [\r\n");

    if (strcmp((char *)aState->iContentType,
        "application/x-www-form-urlencoded") == 0) {
        IHTTPProcessURLEncodedFormData(aState);
#if 0 // in development
        } else if (strcmp((char *)aState->iContentType, "multipart/form-data;")==0) {
        IHTTPProcessMultipartFormData(aState);
#endif
    }
    dprintf("]\r\n");

    // From this point forward, its just a GET request
    return IHTTPGet(aState);
}

/*---------------------------------------------------------------------------*
 * Routine:  IProcessHeader
 *---------------------------------------------------------------------------*/
/**
 *  The header is complete.  Process it based on if this is a GET
 *      or a POST request.
 *
 *  @param [in]    *aState     	State of the HTTP request and parsing.
 *
 *  @return        T_uezError  	Error if failed
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IProcessHeader(T_httpState *aState)
{
    T_uezError error = UEZ_ERROR_NONE;

    // Determine type of request and information
    if (strncmp((char *)aState->iFirstLine, "GET", 3) == 0) {
        // We are getting a file or something, do a GET response
        error = IHTTPGet(aState);
    } else if (strncmp((char *)aState->iFirstLine, "POST", 4) == 0) {
        // We are getting a file or something, do a GET response
        error = IHTTPPost(aState);
    } else {
        // Unknown type of request!
        error = UEZ_ERROR_NOT_SUPPORTED;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IProcessHeaderLine
 *---------------------------------------------------------------------------*/
/**
 *  Process a single line of the header.  Use the passed in data
 *      as needed.  If a blank line is received, the header is complete and
 *      now can be processed as a whole.
 *
 *  @param [in]    *aState      	State of the HTTP request and parsing.
 *
 *  @return        T_uezError    	Error if failed
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IProcessHeaderLine(T_httpState *aState)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt8 length[20];

    // Was this a blank line?
    if (aState->iLength == 0) {
        error = IProcessHeader(aState);
    } else {
        aState->iLine[aState->iLength] = '\0';
        // Handle info here
        // Is this the first line?
        if (aState->iFirstLineLength == 0) {
            dprintf("First: %s\n", aState->iLine);
            // Grab the first line
            memcpy(aState->iFirstLine, aState->iLine, aState->iLength + 1);
            aState->iFirstLineLength = aState->iLength;
        } else {
            dprintf("Header: %s\n", aState->iLine);
            if (strncmp((char *)aState->iLine, "Content-Length:", 15) == 0) {
                IHTTPParameter(aState->iLine, aState->iLength, 1, length);
                aState->iContentLength = atoi((char *)length);
            } else if (strncmp((char *)aState->iLine, "Content-Type:", 13) == 0) {
                IHTTPParameter(aState->iLine, aState->iLength, 1, aState->iContentType);
                if (strcmp((char *)aState->iContentType, "multipart/form-data;") == 0) {
                    IHTTPParameter(aState->iLine, aState->iLength, 2, aState->iContentBoundary);
                    if (memcmp(aState->iContentBoundary, "boundary=", 9) == 0) {
                        // Trim off the boundary= prefix
                        strcpy((char *)aState->iContentBoundary, (char *)aState->iContentBoundary+9);
                    }
                }
            }
            // Ignore the other lines for now
        }
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IParseHeaderLine
 *---------------------------------------------------------------------------*/
/**
 *  Take the header in and append to previous data finding the different
 *      lines.  Each time a line is found, process it.
 *
 *  @param [in]    *aState    	Parse state
 *
 *  @return        T_uezError  	Any error reported when processing.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IParseHeaderLine(T_httpState *aState)
{
    TUInt8 c;
    T_uezError error = UEZ_ERROR_NONE;
    aState->iReceiveIndex = 0;

    while (aState->iReceiveIndex < aState->iReceiveLength) {
        // Process one character at a time.
        c = aState->iReceiveBuffer[aState->iReceiveIndex++];

        if (c == '\r') {
            // Ignore
        } else if (c == '\n') {
            // End of line!  Process!
            error = IProcessHeaderLine(aState);

            // Reset the length
            aState->iLength = 0;

            // If there was an error, go ahead and stop here
            if (error != UEZ_ERROR_NONE)
                break;
        } else {
            // Only store if we got room.
            if (aState->iLength < MAX_LINE_LENGTH) {
                aState->iLine[aState->iLength++] = c;
            }
        }
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  vProcessConnection
 *---------------------------------------------------------------------------*/
/**
 *  A connection for a HTTP request has been started.  Process the
 *      the data request as it comes and spawn off a request when
 *      the end is reached.
 *
 *  @param [in]    *aConn   	connection
 *
 *  @param [in]    aGetFunc		Pointer to a function that will handle
 *          			 		variable parsing responses (on HTML only).
 *  @param [in]    aSetFunc 	Pointer to a functiont that will handle
 *          					variables being set via POST or GET.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void vProcessConnection(int32_t aSocket,
    T_HTTPCallbackGetVarFunc aGetFunc, T_HTTPCallbackSetVarFunc aSetFunc,
    const char *aDrivePrefix)
{
    T_uezError error;
    T_httpState *p_parse;
    TBool done = EFalse;
    int32_t recved;

    p_parse = IHTTPStateAlloc(aSocket);
    if (p_parse) {
        p_parse->iGetFunc = aGetFunc;
        p_parse->iSetFunc = aSetFunc;
        p_parse->iDrivePrefix = aDrivePrefix;
        while (!done) {
            do {
                DEBUG_SV_Printf("RS");
                recved = recv(aSocket, p_parse->iReceiveBuffer, sizeof(p_parse->iReceiveBuffer), 0);
                DEBUG_SV_Printf("RC");
                if (recved > 0) {
                    // Got data, process it.
                    error = IParseHeaderLine(p_parse);
                    if (error)
                        done = ETrue;
                } else {
                    // If there are any errors (closed socket, etc.) then
                    // end the connection.
                    done = ETrue;
                }
            } while (!done);
        }

        IHTTPStateFree(p_parse);
    }
    close(p_parse->iSocket);
    DEBUG_SV_Printf("RE");
    p_parse->iSocket = -1;
}

/*---------------------------------------------------------------------------*
 * Routine:  HTTPServer
 *---------------------------------------------------------------------------*/
/**
 *  Sit in the HTTP server listening for connections, and handle
 *      one at a time.  This implementation is single threaded.
 *
 *  @param [in]    aMyTask      	This task (not used)
 *
 *  @param [in]    *aParameters   	Optional parameters (not used)
 *
 *  @return        TUInt32			Returns 0
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt32 HTTPServer(T_uezTask aMyTask, void *aParameters)
{
    int32_t socket_fd;
    int32_t accept_fd;
    socklen_t addr_size;
    struct sockaddr_in sa,isa;

#if WEB_SERVER_REPORT_VERSION
    char version[20];
#endif
    T_httpServerParameters *p_params = (T_httpServerParameters *)aParameters;
    (void)&IConvertErrorCode; // prevent unused function warning
    (void)&IHTTPParseFileForVars;
    (void)&IHTTPOutputParsedFile;

    dprintf("HTTPServer: Starting\n");

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd >= 0) {
        // Setup the socket to be on the HTTP port
        memset(&sa, 0, sizeof(struct sockaddr_in));
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = INADDR_ANY;
        sa.sin_port = htons(p_params->iPort);

        if (bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa)) != -1) {

            // Put the socket into listen mode
            if (listen(socket_fd,5) == 0) {

                /* Loop forever */
                for (;;) {
                    /* Wait for connection. */
                    addr_size = sizeof(isa);
                    accept_fd = accept(socket_fd, (struct sockaddr* )&isa,
                            &addr_size);
                    if (accept_fd >= 0) {

                        dprintf("HTTPServer: Socket accepted\n");
                        DEBUG_SV_Printf("SA");
                        /* Service connection. */
                        vProcessConnection(accept_fd,
                                (p_params) ? p_params->iGet : 0,
                                (p_params) ? p_params->iSet : 0,
                                (p_params) ? p_params->iDrivePrefix :
                                        "0:/HTTPROOT");
                        close(accept_fd);
                    }
                }
            }
        }
    }
    while (1) {
        // Sit here doing nothing
        // TODO: Need better response/handling
        dprintf("HTTPServer: Task failed!\n");        
        DEBUG_SV_PrintfE("HTTPServer: Task failed!"); // example error
        UEZTaskDelay(5000);
    }
#if ((COMPILER_TYPE!=Keil4) && (COMPILER_TYPE != IAR))
    return 0;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  WebServerStart
 *---------------------------------------------------------------------------*/
/**
 *   Start the Web Server
 *
 *	@param [in] *aParams pointer to input parameters
 *
 *	@return		T_uezError
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError WebServerStart(T_httpServerParameters *aParams)
{
    return UEZTaskCreate((T_uezTaskFunction)HTTPServer, "WebSrv",
        WEB_SERVER_STACK_SIZE, (void *)aParams, UEZ_PRIORITY_NORMAL, 0);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  HTTPServer.c
 *-------------------------------------------------------------------------*/
