/*-------------------------------------------------------------------------*
 * File:  uEZVNC_rfb.c
 *-------------------------------------------------------------------------*/
/*
 *  @{
 *  @brief     uEZ VNC Device implementation
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    Device implementation of the Network for the PCA9551
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
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uEZ.h>
#include "uEZVNC.h"
#include "uEZVNC_private.h"
#include "uEZVNC_rfb.h"
//#include "sockets.h"
#include "vncauth.h"
#include "uEZVNC_io.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    T_uezDevice iNetwork;
    T_uezNetworkSocket iSocket;

    T_UEZVNCServer iServer;
    const T_UEZVNCCallbacks *iCallbacks;
    char *iDesktopName;
    unsigned char iBuffer[16 * 16 * 4];
    //int iConnection;
    rfbPixelFormat iFormat;
    rfbServerInitMsg iServerInitMsg;
    TUInt16 iVirtualWidth;
    TUInt16 iVirtualHeight;
    TUInt16 iFrameWidth;
    TUInt16 iFrameHeight;

    TUInt8 *iBuffer2;
} T_VNCWorkspace;

/*---------------------------------------------------------------------------*
 * Globls:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool HandleHextileEncoding8(
    T_VNCWorkspace *p,
    TUInt16 x,
    TUInt16 y,
    TUInt16 w,
    TUInt16 h);
static TBool HandleHextileEncoding16(
    T_VNCWorkspace *p,
    TUInt16 x,
    TUInt16 y,
    TUInt16 w,
    TUInt16 h);
static TBool HandleRawEncoding32(
    T_VNCWorkspace *p,
    TUInt16 x,
    TUInt16 y,
    TUInt16 w,
    TUInt16 h);
static TBool HandleRawEncoding16(
    T_VNCWorkspace *p,
    TUInt16 rx,
    TUInt16 ry,
    TUInt16 rw,
    TUInt16 rh);
static TBool HandleRawEncoding8(
    T_VNCWorkspace *p,
    TUInt16 x,
    TUInt16 y,
    TUInt16 w,
    TUInt16 h);
static TBool UEZVNC_Read(T_VNCWorkspace *p, TUInt8 *aBuffer, TUInt32 aNumBytes);
static T_uezError UEZVNC_ReadFast(T_VNCWorkspace *p, TUInt8 *aBuffer, TUInt32 aNumBytes);
static TBool
UEZVNC_Write(T_VNCWorkspace *p, TUInt8 *aBuffer, TUInt32 aNumBytes);

/* note that the CoRRE encoding uses this buffer and assumes it is big enough
 to hold 255 * 255 * 32 bits -> 260100 bytes.  640*480 = 307200 bytes */
/* also hextile assumes it is big enough to hold 16 * 16 * 32 bits */
#if 0
/*---------------------------------------------------------------------------*
 * Routine:  ConnectToRFBServer
 *---------------------------------------------------------------------------*/
/**
 *  connects on the networkl level
 *
 *	@param [in] *hostname	hostname to use
 *
 *	@param [in] port		point to use
 *
 *	@return			TBool		UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
Bool ConnectToRFBServer(char *hostname, int port)
{
    unsigned long host = 0;

    if (!StringToIPAddr(hostname, &host)) {
        dprintf("Cannot resolve \"%s\"\n", hostname);
        return EFalse;
    }

    p = ConnectToTcpAddr(host, port);

    if (p < 0) {
        dprintf("Unable to connect to \"%s\":%d\n", hostname, port);
        return EFalse;
    }

    dprintf("Network connected\n");
    return ETrue;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  VNCInitialiseRFBConnection
 *---------------------------------------------------------------------------*/
/**
 *  Initialize Remote Framebuffer connection (connects on the VNC protocol
 * 								level, i.e. handshake, authentication, etc)
 *
 *	@param [in] 	*aWorkspace	Pointer to workspace
 *                          
 *	@return			TBool 		UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool VNCInitialiseRFBConnection(void *aWorkspace/*, int sock -> iConnection */)
{
    rfbProtocolVersionMsg pv;
    int majorV, minorV;
    TBool authWillWork = ETrue;
    TUInt32 authScheme, reasonLen;
    char *reason;
    TUInt32 authResult;
    TUInt8 challenge[CHALLENGESIZE];
    rfbClientInitMsg clientInitMsg;
    char testVer[20];
    char mypass[33];
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;

    // Read the protocol version from the target server or
    // fail trying (and null terminate)
    if (!UEZVNC_Read(p, (unsigned char*)pv, sz_rfbProtocolVersionMsg))
        return EFalse;
    pv[sz_rfbProtocolVersionMsg] = 0;

    // Report that we are indeed handshaking...
    p->iCallbacks->StateUpdate(aWorkspace, UEZVNC_STATE_HANDSHAKING, 0);

    // What is the version number (major.minor)?
    // Put into a format we can compare to.
    majorV = rfbProtocolMajorVersion;
    minorV = rfbProtocolMinorVersion;
    sprintf(testVer, "00%d.00%d\n", majorV, minorV);

    // Make sure the letters RFB are in the message
    if (strstr(pv, "RFB") == 0) {
        dprintf("Not a valid VNC currentServer\n");
        return EFalse;
    }

    // Output the version for debugging.
    dprintf("VNC currentServer supports protocol %s", pv);
    dprintf("UEZVNC supports %s\n", testVer);

    // if currentServer is before 3.3 authentication won't work
    if (strcmp(pv, testVer) < 0) {
        authWillWork = EFalse;
    }

    // Format the returned version as well.
    sprintf(pv, rfbProtocolVersionFormat, majorV, minorV);

    // Write out our version information back to the server.
    if (!UEZVNC_Write(p, (unsigned char*)pv, sz_rfbProtocolVersionMsg)) {
        return EFalse;
    }

    // Next, get the authorization scheme.
    if (!UEZVNC_Read(p, (unsigned char*)&authScheme, 4)) {
        return EFalse;
    }

    authScheme = Swap32IfLE(authScheme);

    switch (authScheme) {
        case rfbConnFailed:
            // The authentication scheme is not accepted and we have failed.
            // Read back the reason size.
            if (!UEZVNC_Read(p, (unsigned char*)&reasonLen, 4)) {
                return EFalse;
            }
            reasonLen = Swap32IfLE(reasonLen);

            // Store the reason in some temporary memory.
            if ((reason = UEZMemAlloc(reasonLen + 1)) == 0) {
                dprintf("ERROR: MemPtrNew\n");
                return EFalse;
            }
            // Read back the reason
            if (!UEZVNC_Read(p, (unsigned char*)reason, (int)reasonLen)) {
                return EFalse;
            }
            reason[reasonLen] = 0;
            dprintf("VNC connection failed: %s\n", reason);
            p->iCallbacks->StateUpdate(aWorkspace,
                UEZVNC_STATE_CONNECTION_FAILED, (void *)reason);

            // release the reason and exit with a false return.
            UEZMemFree(reason);
            return EFalse;

        case rfbNoAuth:
            // No authentication was needed.
            dprintf("No authentication needed\n");
            p->iCallbacks->StateUpdate(aWorkspace,
                UEZVNC_STATE_AUTHENTICATION_NOT_NEEDED, 0);
            break;

        case rfbVncAuth:
            if (!authWillWork) {
                // If neither version is correct, stop here
                p->iCallbacks->StateUpdate(aWorkspace,
                    UEZVNC_STATE_VERSIONS_INCOMPATIBLE, 0);
                dprintf("\nVNC versions incompatible.\n\n");
                return EFalse;
            }

            // Read in the challenge
            if (!UEZVNC_Read(p, (unsigned char*)challenge, CHALLENGESIZE)) {
                return EFalse;
            }

            // Setup our password, encrypting based on the challenge.
            memset(mypass, 32, 0);
            strcpy(mypass, p->iServer.iPassword);
            vncEncryptBytes(challenge, mypass);

            // Send back the response to the challenge
            if (!UEZVNC_Write(p, (unsigned char*)challenge, CHALLENGESIZE))
                return EFalse;

            // What is the response?
            if (!UEZVNC_Read(p, (unsigned char*)&authResult, 4))
                return EFalse;
            authResult = Swap32IfLE(authResult);
            switch (authResult) {
                case rfbVncAuthOK:
                    dprintf("Authenticated\n");
                    p->iCallbacks->StateUpdate(aWorkspace,
                        UEZVNC_STATE_AUTHENTICATED, 0);
                    break;
                case rfbVncAuthFailed:
                    dprintf("\nAuthentication failed\n\n");
                    p->iCallbacks->StateUpdate(aWorkspace,
                        UEZVNC_STATE_AUTHENTICATION_FAILED, (void *)authResult);
                    return EFalse;
                case rfbVncAuthTooMany:
                    dprintf("Authentication failed - too many tries\n");
                    p->iCallbacks->StateUpdate(aWorkspace,
                        UEZVNC_STATE_AUTHENTICATION_FAILED, (void *)authResult);
                    return EFalse;
                default:
                    dprintf("Authentication failed - code %d\n",
                        (int)authResult);
                    p->iCallbacks->StateUpdate(aWorkspace,
                        UEZVNC_STATE_AUTHENTICATION_FAILED, (void *)authResult);
                    return EFalse;
            }
            break;
        default:
            dprintf("Unknown authentication: %d\n", (int)authScheme);
            p->iCallbacks->StateUpdate(aWorkspace,
                UEZVNC_STATE_UNKNOWN_AUTHENTICATION, (void *)authScheme);
            return EFalse;
    }

    // Build a client init message and send
    clientInitMsg.shared = (p->iServer.iIsSharedDesktop ? 1 : 0);
    if (!UEZVNC_Write(p, (unsigned char*)&clientInitMsg, sz_rfbClientInitMsg))
        return EFalse;

    // Get back the server init message
    if (!UEZVNC_Read(p, (unsigned char*)&p->iServerInitMsg, sz_rfbServerInitMsg))
        return EFalse;

    // Record information about the screen (in the correct endian format)
    p->iServerInitMsg.framebufferWidth = Swap16IfLE(
        p->iServerInitMsg.framebufferWidth);
    p->iServerInitMsg.framebufferHeight = Swap16IfLE(
        p->iServerInitMsg.framebufferHeight);
    p->iServerInitMsg.format.redMax = Swap16IfLE(
        p->iServerInitMsg.format.redMax);
    p->iServerInitMsg.format.greenMax = Swap16IfLE(
        p->iServerInitMsg.format.greenMax);
    p->iServerInitMsg.format.blueMax = Swap16IfLE(
        p->iServerInitMsg.format.blueMax);
    p->iServerInitMsg.nameLength = Swap32IfLE(p->iServerInitMsg.nameLength);

    // Record the target's width and height in an easier to access location
    p->iVirtualWidth = p->iServerInitMsg.framebufferWidth;
    p->iVirtualHeight = p->iServerInitMsg.framebufferHeight;

    // Get the name of the desktop (if it matters)
    if ((p->iDesktopName = UEZMemAlloc(p->iServerInitMsg.nameLength + 1)) == 0) {
        dprintf("ERROR: mem ptr\n");
        return EFalse;
    }

    // Read back the desktop name following the above server message
    // and null terminate.
    if (!UEZVNC_Read(p, (unsigned char*)p->iDesktopName,
        (int)p->iServerInitMsg.nameLength))
        return EFalse;
    p->iDesktopName[p->iServerInitMsg.nameLength] = 0;

    // For debugging, output the name
    dprintf("Desktop size: %dx%d\n", p->iServerInitMsg.framebufferWidth,
        p->iServerInitMsg.framebufferHeight);
    dprintf("Desktop name \"%s\"\n", p->iDesktopName);

    // Don't free it, hold onto it.
    //UEZMemFree(p->iDesktopName);

    // Finally, debug output some protocol information
    dprintf("Connected using protocol %d.%d\n", rfbProtocolMajorVersion,
        rfbProtocolMinorVersion);

    // Alas, Success!
    return ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  VNCSetFormatAndEncodings
 *---------------------------------------------------------------------------*/
/**
 *  Set VCN format and encoding method
 *
 *	@param [in] *aWorkspace		Pointer to workspace
 *           
 *	@return		TBool			Returns EFalse if failed, else ETrue
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool VNCSetFormatAndEncodings(void *aWorkspace)
{
    rfbSetPixelFormatMsg spf;
    unsigned char buf[sz_rfbSetEncodingsMsg + MAX_ENCODINGS * 4];
    rfbSetEncodingsMsg *se = (rfbSetEncodingsMsg *)buf;
    TUInt32 *encs = (TUInt32 *)(&buf[sz_rfbSetEncodingsMsg]);
    int len = 0;
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;

    // Create a proper Set Pixel Format Message
    // and send it.
    spf.type = rfbSetPixelFormat;
#if (UEZ_LCD_DEFAULT_COLOR_DEPTH == UEZLCD_COLOR_DEPTH_16_BIT) // 16-bit
    p->iFormat.bitsPerPixel = 16;
    p->iFormat.depth = 16;
    p->iFormat.bigEndian = 0;
    p->iFormat.trueColour = 1;
    p->iFormat.redMax = 31;
    p->iFormat.greenMax = 63;
    p->iFormat.blueMax = 31;
    p->iFormat.redShift = 11;
    p->iFormat.greenShift = 5;
    p->iFormat.blueShift = 0;
#elif (UEZ_LCD_DEFAULT_COLOR_DEPTH == UEZLCD_COLOR_DEPTH_I15_BIT)
    p->iFormat.bitsPerPixel = 16;
    p->iFormat.depth = 16;
    p->iFormat.bigEndian = 0;
    p->iFormat.trueColour = 1;
    p->iFormat.redMax = 31;
    p->iFormat.greenMax = 31;
    p->iFormat.blueMax = 31;
    p->iFormat.redShift = 10;
    p->iFormat.greenShift = 5;
    p->iFormat.blueShift = 0;
#else // 8-bit
    p->iFormat.bitsPerPixel = 8;
    p->iFormat.depth = 8;
    p->iFormat.bigEndian = 0;
    p->iFormat.trueColour = 1;
    p->iFormat.redMax = 3;
    p->iFormat.greenMax = 7;
    p->iFormat.blueMax = 7;
    p->iFormat.redShift = 6;
    p->iFormat.greenShift = 3;
    p->iFormat.blueShift = 0;
#endif

    spf.format = p->iFormat;
    spf.format.redMax = Swap16IfLE(spf.format.redMax);
    spf.format.greenMax = Swap16IfLE(spf.format.greenMax);
    spf.format.blueMax = Swap16IfLE(spf.format.blueMax);

    if (!UEZVNC_Write(p, (unsigned char*)&spf, sz_rfbSetPixelFormatMsg))
        return EFalse;

    // Prepare and send a Set Encoding Message
    // We only support HexTile format (currently)
    se->type = rfbSetEncodings;
    se->nEncodings = 0;
    encs[se->nEncodings++] = Swap32IfLE(rfbEncodingHextile);
    encs[se->nEncodings++] = Swap32IfLE(rfbEncodingRaw);
    len = sz_rfbSetEncodingsMsg + se->nEncodings * 4;
    se->nEncodings = Swap16IfLE(se->nEncodings);
    if (!UEZVNC_Write(p, buf, len))
        return EFalse;

    return ETrue;
}
/*---------------------------------------------------------------------------*
 * Routine:  VNCSetScaleFactor
 *---------------------------------------------------------------------------*/
/**
 *  Set the VNC scalefactor
 *
 *	@param [in] 	*aWorkspace		Pointer to workspace
 *                          
 *	@param [in]		scaleFactor		scale factor
 *
 *	@return			TBool			Returns EFalse if failed, else ETrue
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool VNCSetScaleFactor(void *aWorkspace, unsigned short scaleFactor)
{
    rfbSetScaleFactorMsg ssf;
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;

    // Prepare and send a Set Scale Factor Message
    ssf.type = rfbSetScaleFactor;
    ssf.scale = Swap16IfLE(scaleFactor);
    if (!UEZVNC_Write(p, (unsigned char*)&ssf, sz_rfbSetScaleFactorMsg)) {
        return EFalse;
    }

    return ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  VNCSendIncrementalFramebufferUpdateRequest
 *---------------------------------------------------------------------------*/
/**
 *  Send Incremental Framebuffer Update request
 *
 *	@param [in] 	*aWorkspace		Pointer to workspace
 *                          
 *	@return			TBool			true if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool VNCSendIncrementalFramebufferUpdateRequest(void *aWorkspace)
{
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;
    return VNCSendFramebufferUpdateRequest(aWorkspace, 0, 0, p->iFrameWidth,
        p->iFrameHeight, ETrue);
}

/*---------------------------------------------------------------------------*
 * Routine:  VNCSendFramebufferUpdateRequest
 *---------------------------------------------------------------------------*/
/**
 *  Send Framebuffer update request
 *
 *	@param [in] 	*aWorkspace	Pointer to workspace
 *                          
*	@param [in]		x			TODO: ?
 *                          
 *	@param [in]		y			TODO: ?
 *                          
 *	@param [in]		w			TODO: ?
 *                          
 *	@param [in]		h			TODO: ?
 *                          
 *	@param [in]		incremental	TODO: ?
 *
 *	@return			TBool		True if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool VNCSendFramebufferUpdateRequest(
    void *aWorkspace,
    TUInt16 x,
    TUInt16 y,
    TUInt16 w,
    TUInt16 h,
    TBool incremental)
{
    rfbFramebufferUpdateRequestMsg fur;
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;

    // Setup and Set up a Send Framebuffer Update Request
    fur.type = rfbFramebufferUpdateRequest;
    fur.incremental = incremental ? 1 : 0;
    fur.x = Swap16IfLE(x);
    fur.y = Swap16IfLE(y);
    fur.w = Swap16IfLE(w);
    fur.h = Swap16IfLE(h);

    dprintf("updt=%d,%d %d,%d,%d\n", x, y, w, h, incremental);
    if (!UEZVNC_Write(p, (unsigned char*)&fur,
        sz_rfbFramebufferUpdateRequestMsg))
        return EFalse;

    // we do this in the event loop, it was: sendUpdateRequest = EFalse;

    return ETrue;
}
/*---------------------------------------------------------------------------*
 * Routine:  VNCSendPointerEvent
 *---------------------------------------------------------------------------*/
/**
 *  Send Pointer even to server
 *
 *	@param [in] 	*aWorkspace	Pointer to workspace
 *                          
 *	@param [in]		x			TODO: ?
 *                          
 *	@param [in]		y			TODO: ?
 *           
*	@param [in]		buttonMask	TODO: ?
 *
 *	@return			TBool		True if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool VNCSendPointerEvent(
    void *aWorkspace,
    TInt16 x,
    TInt16 y,
    TUInt8 buttonMask)
{
    rfbPointerEventMsg pe;
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;

    // Build and send a Pointer Event Message
    pe.type = rfbPointerEvent;
    pe.buttonMask = buttonMask;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    pe.x = Swap16IfLE(x);
    pe.y = Swap16IfLE(y);
    return UEZVNC_Write(p, (unsigned char*)&pe, sz_rfbPointerEventMsg);
}

/*---------------------------------------------------------------------------*
 * Routine:  VNCSendKeyEvent
 *---------------------------------------------------------------------------*/
/**
 *  Send key event to server
 *
 *	@param [in] 	*aWorkspace	Pointer to workspace
 *                          
 *	@param [in] 	key			which key was pressed
 *                          
*	@param [in]		down		TODO: ?
 *
 *	@return		TBool	true if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool VNCSendKeyEvent(void *aWorkspace, TUInt32 key, TBool down)
{
    rfbKeyEventMsg ke;
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;

    // Build and send a Key Event Message
    ke.type = rfbKeyEvent;
    ke.down = down ? 1 : 0;
    ke.key = Swap32IfLE(key);
    return UEZVNC_Write(p, (unsigned char*)&ke, sz_rfbKeyEventMsg);
}

/******************************************************************
 * SendClientCutText.
 *****************************************************************/
TBool SendClientCutText(void *aWorkspace, char *str, int len)
{
    rfbClientCutTextMsg cct;
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;

    // Build and send a Client Cut Text Message
    // and send the text that goes with it.
    cct.type = rfbClientCutText;
    cct.length = Swap32IfLE(len);
    return (UEZVNC_Write(p, (unsigned char*)&cct, sz_rfbClientCutTextMsg)
        && UEZVNC_Write(p, (unsigned char*)str, len));
}

/*---------------------------------------------------------------------------*
 * Routine:  VNCHandleRFBServerMessage
 *---------------------------------------------------------------------------*/
/**
 *  Handle Remote Framebuffer Server message
 *
 *	@param [in] 	*aWorkspace		Pointer to workspace
 *               
 *	@return			T_uezError		error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError VNCHandleRFBServerMessage(void *aWorkspace)
{
    rfbServerToClientMsg msg;
    //    unsigned short old_w, old_h, old_x, old_y;
    signed short new_x, new_y;
    T_VNCWorkspace *p = (T_VNCWorkspace *)aWorkspace;
    T_uezError error;

    // Read a message's type
    error = UEZVNC_ReadFast(p, (unsigned char*)&msg, 1);
    if (error != UEZ_ERROR_NONE)
        return error;

    // Depending on the type of message, process that message
    switch (msg.type) {
        case rfbSetColourMapEntries: {
            // Read the colour map entry structure (no other data)
            if (!UEZVNC_Read(p, ((unsigned char*)&msg) + 1,
                sz_rfbSetColourMapEntriesMsg - 1))
                return UEZ_ERROR_INTERNAL_ERROR;

            break;
        }

        case rfbFramebufferUpdate: {
            // Process a framebuffer update message
            rfbFramebufferUpdateRectHeader rect;
            int i;

            // Get the rest of the buffer
            if (!UEZVNC_Read(p, ((unsigned char*)&msg.fu) + 1,
                sz_rfbFramebufferUpdateMsg - 1))
                return UEZ_ERROR_INTERNAL_ERROR;

            // How many rectangles in this update?
            msg.fu.nRects = Swap16IfLE(msg.fu.nRects);

            // Walk through the rectangles
            for (i = 0; i < msg.fu.nRects; i++) {
                // Read an framebuffer update header
                if (!UEZVNC_Read(p, (unsigned char*)&rect,
                    sz_rfbFramebufferUpdateRectHeader))
                    return UEZ_ERROR_INTERNAL_ERROR;

                // Convert this to the right type of info
                rect.r.x = Swap16IfLE(rect.r.x);
                rect.r.y = Swap16IfLE(rect.r.y);
                rect.r.w = Swap16IfLE(rect.r.w);
                rect.r.h = Swap16IfLE(rect.r.h);

                // What type of encoding is this data?
                rect.encoding = Swap32IfLE(rect.encoding);

                dprintf("Rxd rect=%d,%d %d,%d\n", rect.r.x, rect.r.y, rect.r.w,
                    rect.r.h);

                // Don't allow processing past our visible area
                if ((rect.r.x + rect.r.w > p->iServerInitMsg.framebufferWidth)
                    || (rect.r.y + rect.r.h
                        > p->iServerInitMsg.framebufferHeight)) {
                    dprintf("rect too large: %dx%d at (%d, %d)\n", rect.r.w,
                        rect.r.h, rect.r.x, rect.r.y);
                    return UEZ_ERROR_INTERNAL_ERROR;
                }

                // Zero sizes are ignored
                if ((rect.r.h * rect.r.w) == 0) {
                    dprintf("zero size rect - ignoring\n");
                    continue;
                }

                // Depending on the encoding, we'll process this
                // (Currently, only Hextile is supported)
                switch (rect.encoding) {
                    case rfbEncodingRaw:
                        dprintf("->raw encoding: rec %dx%d\n", rect.r.h,
                            rect.r.w);
                        switch (p->iFormat.bitsPerPixel) {
                            case 8:
                                if (!HandleRawEncoding8(p, rect.r.x,
                                    rect.r.y, rect.r.w, rect.r.h))
                                    return UEZ_ERROR_INTERNAL_ERROR;
                                break;
                            case 16:
                                if (!HandleRawEncoding16(p, rect.r.x,
                                    rect.r.y, rect.r.w, rect.r.h))
                                    return UEZ_ERROR_INTERNAL_ERROR;
                                break;
                            case 32:
                                if (!HandleRawEncoding32(p, rect.r.x,
                                    rect.r.y, rect.r.w, rect.r.h))
                                    return UEZ_ERROR_INTERNAL_ERROR;
                                break;
                            default:
                                break;
                        }
                        break;

                    case rfbEncodingCopyRect: {
                        dprintf("-> copy rect, rect %dx%d, NOT SUPPORTED\n",
                            rect.r.h, rect.r.w);
                        return UEZ_ERROR_INTERNAL_ERROR;
                        //break;
                    }

                    case rfbEncodingRRE: {
                        dprintf("-> REE encoding, NOT SUPPORTED\n");
                        return UEZ_ERROR_INTERNAL_ERROR;
                    }

                    case rfbEncodingCoRRE: {
                        dprintf("-> CoREE encoding, NOT SUPPORTED\n");
                        return UEZ_ERROR_INTERNAL_ERROR;
                    }

                    case rfbEncodingHextile: {
                        // Process a Hextile
                        dprintf("-> Hext r %dx%d (%d,%d)\n", rect.r.w,
                            rect.r.h, rect.r.x, rect.r.y);

                        // Process the hextile based on the bits per pixel
                        switch (p->iFormat.bitsPerPixel) {
                            case 8:
                                if (!HandleHextileEncoding8(p, rect.r.x,
                                    rect.r.y, rect.r.w, rect.r.h))
                                    return UEZ_ERROR_INTERNAL_ERROR;
                                break;
                            case 16:
                                if (!HandleHextileEncoding16(p, rect.r.x,
                                    rect.r.y, rect.r.w, rect.r.h))
                                    return UEZ_ERROR_INTERNAL_ERROR;
                                break;
                            default:
                                dprintf("-> Hextile %d bit, NOT SUPPORTED\n",
                                    p->iFormat.bitsPerPixel);
                                return UEZ_ERROR_INTERNAL_ERROR;
                        }
                        break;
                    }

                    default:
                        dprintf("unknown encoding %d\n", (int)rect.encoding);
                        return UEZ_ERROR_INTERNAL_ERROR;
                }
            }
            //??? sendUpdateRequest = ETrue;
            p->iCallbacks->Event(aWorkspace, UEZVNC_EVENT_UPDATE_COMPLETE, 0);
            break;
        }

        case rfbBell:
            //SndPlaySystemSound((SndSysBeepType)sndInfo);
            dprintf("Bell!\n");
            p->iCallbacks->Event(aWorkspace, UEZVNC_EVENT_PLAY_BELL, 0);
            break;

        case rfbServerCutText: {
            char *str;

            // A cut text event has occurred.
            // Read the Cut Text Message
            if (!UEZVNC_Read(p, ((unsigned char*)&msg) + 1,
                sz_rfbServerCutTextMsg - 1))
                return UEZ_ERROR_INTERNAL_ERROR;

            // Allocate space for the clipping
            msg.sct.length = Swap32IfLE(msg.sct.length);
            str = UEZMemAlloc(msg.sct.length);
            memset(str, msg.sct.length, 0);

            // Read the rest of the clipping into the allocated memory
            if (!UEZVNC_Read(p, (unsigned char*)str, msg.sct.length))
                return UEZ_ERROR_INTERNAL_ERROR;

            // Debug info sent out and clipped
            dprintf("%d bytes in CutText received\n", msg.sct.length);
            dprintf("\t\"%s\"\n", str);

            // Use the callback to pass the data back
            p->iCallbacks->Event(aWorkspace, UEZVNC_EVENT_CUT_TEXT, str);

            UEZMemFree(str);
            break;
        }

        case rfbReSizeFrameBuffer: {
            // The frame buffer has been resized
            if (!UEZVNC_Read(p, ((unsigned char*)&msg) + 1,
                sz_rfbReSizeFrameBufferMsg - 1))
                return UEZ_ERROR_INTERNAL_ERROR;

            //old_w = p->iVirtualWidth;
            //old_h = p->iVirtualHeight;
            //old_x = 0;
            //old_y = 0;
            //p->iVirtualWidth = Swap16IfLE(msg.rsfb.desktop_w);
            //p->iVirtualHeight = Swap16IfLE(msg.rsfb.desktop_h);
            p->iVirtualWidth = Swap16IfLE(msg.rsfb.buffer_w);
            p->iVirtualHeight = Swap16IfLE(msg.rsfb.buffer_h);

            dprintf("Got resize=%d,%d\n", p->iVirtualWidth, p->iVirtualHeight);

            new_x = new_y = 0;

            if (new_x < 0)
                new_x = 0;
            if (new_y < 0)
                new_y = 0;

            //palmFB.viewable.x = palmFB.virtual.x = new_x;
            //palmFB.viewable.y = palmFB.virtual.y = new_y;

            //updateViewportSize = ETrue;
            p->iCallbacks->Event(aWorkspace,
                UEZVNC_EVENT_FRAMEBUFFER_SIZE_CHANGED, 0);
            break;
        }

        default:
            // Other messages?
            dprintf("unknown message %d\n", msg.type);
            return UEZ_ERROR_INTERNAL_ERROR;
    }

    return UEZ_ERROR_NONE;
}

#define GET_PIXEL8(pix,ptr) ((pix)=*(ptr)++)
#define GET_PIXEL16(pix,ptr) { ((pix)=*((TUInt16 *)ptr)); ptr+=2; }

static TBool HandleRawEncoding32(
    T_VNCWorkspace *p,
    TUInt16 rx,
    TUInt16 ry,
    TUInt16 rw,
    TUInt16 rh)
{
    short x, y;
    TUInt8 rgb[4];

    dprintf("raw32 %d, %d (%d by %d)\n", rx, ry, rw, rh);
    for (y = ry; y < ry + rh; y++) {
        for (x = rx; x < rx + rw; x++) {
            if (!UEZVNC_Read(p, rgb, 4))
                return EFalse;
            p->iCallbacks->DrawPixel((void *)p, RGB(rgb[2], rgb[1], rgb[0]), x, y);
        }
    }
    return ETrue;
}

static TBool G_initTable = EFalse;
static T_pixelColor G_table[256];
static void ISetupTable(void)
{
    TUInt16 red, green, blue;
    int index = 0;

    for (red=0; red<4; red++) {
        for (green=0; green<8; green++) {
            for (blue=0; blue<8; blue++, index++)  {
                G_table[index] =
                    RGB((red*(255/3)),
                        (green*(255/7)),
                        (blue*(255/7)));
            }
        }
    }
}

static TBool HandleRawEncoding8(
    T_VNCWorkspace *p,
    TUInt16 rx,
    TUInt16 ry,
    TUInt16 rw,
    TUInt16 rh)
{
    short y;
//    TUInt8 rgb;
    TUInt8 *p_rgb;
    T_pixelColor *p_pixel;
    TUInt32 count;

    dprintf("raw8 %d, %d (%d by %d)\n", rx, ry, rw, rh);
    if (!G_initTable) {
        G_initTable = ETrue;
        ISetupTable();
    }
    for (y = ry; y < ry + rh; y++) {
        p_rgb = p->iBuffer2;
        if (!UEZVNC_Read(p, p_rgb, rw))
            return EFalse;
        p_pixel = ((T_pixelColor *)FRAME(0)) + y*DISPLAY_WIDTH + rx;
////        for (x = rx; x < rx + rw; x++) {
//            if (!UEZVNC_Read(p, &rgb, 1))
//                return EFalse;
///            rgb = *(p_rgb++);
//            p->iCallbacks->DrawPixel((void *)p, (((rgb >> 6) & 3) << 6)+0x1F, (((rgb
//                >> 3) & 7) << 5)+0x0F, (((rgb >> 0) & 7) << 5)+0x0F, x, y);
///            p->iCallbacks->DrawPixel((void *)p, G_table[rgb], x, y);
            count = rw;
            while (count & 0x0008) {
                p_pixel[0] = G_table[p_rgb[0]];
                p_pixel[1] = G_table[p_rgb[1]];
                p_pixel[2] = G_table[p_rgb[2]];
                p_pixel[3] = G_table[p_rgb[3]];
                p_pixel[4] = G_table[p_rgb[4]];
                p_pixel[5] = G_table[p_rgb[5]];
                p_pixel[6] = G_table[p_rgb[6]];
                p_pixel[7] = G_table[p_rgb[7]];
                count -= 8;
                p_pixel += 8;
                p_rgb += 8;
            }
            while (count--)
                *(p_pixel++) = G_table[*(p_rgb++)];
////        }
    }
    return ETrue;
}

static TBool HandleRawEncoding16(
    T_VNCWorkspace *p,
    TUInt16 rx,
    TUInt16 ry,
    TUInt16 rw,
    TUInt16 rh)
{
    short y;
//    TUInt8 rgb;
//    TUInt8 *p_rgb;
    T_pixelColor *p_pixel;
//    TUInt32 count;

    dprintf("raw16 %d, %d (%d by %d)\n", rx, ry, rw, rh);
    for (y = ry; y < ry + rh; y++) {
//        p_rgb = p->iBuffer2;
        p_pixel = ((T_pixelColor *)FRAME(0)) + y*DISPLAY_WIDTH + rx;
        if (!UEZVNC_Read(p, (TUInt8 *)p_pixel, rw*2))
            return EFalse;
    }
    return ETrue;
}

static TBool HandleHextileEncoding8(
    T_VNCWorkspace *p,
    TUInt16 rx,
    TUInt16 ry,
    TUInt16 rw,
    TUInt16 rh)
{
    TUInt8 bg, fg;
    int i;
    TUInt8 *ptr;
    short x, y;
    unsigned short w, h;
    short sx, sy;
    unsigned short sw, sh;
    TUInt8 subencoding;
    TUInt8 nSubrects;

    dprintf("hextile %d, %d (%d by %d)\n", rx, ry, rw, rh);

    // Walk vertically through the region in 16 pixel lines
    for (y = ry; y < ry + rh; y += 16) {
        //        if ((rh * rw) > 2000)
        //            PostProgressMessage("Receiving Data...", y - ry, rh);

        // Walk across the region in 16 pixel sections
        for (x = rx; x < rx + rw; x += 16) {
            //            if ((rh < 32) && (rw > 64))
            //                PostProgressMessage("Receiving Data...", x - rx, rw);
            w = h = 16;
            if (rx + rw - x < 16)
                w = rx + rw - x;
            if (ry + rh - y < 16)
                h = ry + rh - y;

            if (!UEZVNC_Read(p, (unsigned char*)&subencoding, 1))
                return EFalse;

            // Raw section of bytes coming
            if (subencoding & rfbHextileRaw) {
                if (!UEZVNC_Read(p, p->iBuffer, (int)(w * h)))
                    return EFalse;

                // Copy from the buffer to the screen
                p->iCallbacks->OffscreenCopyAreaFromBuffer((void *)p,
                    (T_pixelColor *)p->iBuffer, x, y, w, h);
                continue;
            }

            // Background color changed?
            if (subencoding & rfbHextileBackgroundSpecified)
                if (!UEZVNC_Read(p, (unsigned char*)&bg, 1))
                    return EFalse;
            dprintf(" spec bg %d r %dx%d (%d,%d)\n", bg, w, h, x, y);
            // Draw a solid region of background here
            p->iCallbacks->OffscreenDrawFilledRectangle((void *)p, bg, x, y, w,
                h);

            // Get a foreground color?
            if (subencoding & rfbHextileForegroundSpecified)
                if (!UEZVNC_Read(p, (unsigned char*)&fg, 1))
                    return EFalse;

            // Are we doing subrects?
            if (!(subencoding & rfbHextileAnySubrects)) {
                continue;
            }

            // Yes, doing subrects.  Get details
            if (!UEZVNC_Read(p, (unsigned char*)&nSubrects, 1))
                return EFalse;

            ptr = (TUInt8 *)p->iBuffer;

            // Does the subrect start with a color?
            if (subencoding & rfbHextileSubrectsColoured) {
                // Get the color information into the buffer (one per subrect)
                if (!UEZVNC_Read(p, p->iBuffer, nSubrects * (2 + 1)))
                    return EFalse;
                dprintf(" %d subr. colored\n", nSubrects);

                // Fill in the solid subrects
                for (i = 0; i < nSubrects; i++) {
                    // What color?
                    GET_PIXEL8(fg, ptr);
                    fg = *(ptr)++;

                    // Where is the xy in the subrect?
                    sx = rfbHextileExtractX(*ptr);
                    sy = rfbHextileExtractY(*ptr);
                    ptr++;

                    // What is the height and width of this region?
                    sw = rfbHextileExtractW(*ptr);
                    sh = rfbHextileExtractH(*ptr);
                    ptr++;

                    dprintf(" color %d\n", fg);
                    // Draw this region to the screen
                    p->iCallbacks->OffscreenDrawFilledRectangle((void *)p, fg,
                        x + sx, y + sy, sw, sh);
                }
            } else {
                // Not colored, use the foreground color for this rectangle
                if (!UEZVNC_Read(p, p->iBuffer, nSubrects * 1))
                    return EFalse;

                for (i = 0; i < nSubrects; i++) {
                    // Where is the xy in the subrect?
                    sx = rfbHextileExtractX(*ptr);
                    sy = rfbHextileExtractY(*ptr);
                    ptr++;

                    // What is the height and width of this region?
                    sw = rfbHextileExtractW(*ptr);
                    sh = rfbHextileExtractH(*ptr);
                    ptr++;

                    dprintf(" color %d\n", fg);
                    // Draw this region to the screen
                    p->iCallbacks->OffscreenDrawFilledRectangle((void *)p, fg,
                        x + sx, y + sy, sw, sh);
                }
            }
        }
    }

    return ETrue;
}

static TBool HandleHextileEncoding16(
    T_VNCWorkspace *p,
    TUInt16 rx,
    TUInt16 ry,
    TUInt16 rw,
    TUInt16 rh)
{
    TUInt16 bg, fg;
    int i;
    TUInt8 *ptr;
    short x, y;
    unsigned short w, h;
    short sx, sy;
    unsigned short sw, sh;
    TUInt8 subencoding;
    TUInt8 nSubrects;

    //dprintf("hextile16 %d, %d (%d by %d)\n", rx, ry, rw, rh);

    // Walk vertically through the region in 16 pixel lines
    for (y = ry; y < ry + rh; y += 16) {
        //        if ((rh * rw) > 2000)
        //            PostProgressMessage("Receiving Data...", y - ry, rh);

        // Walk across the region in 16 pixel sections
        for (x = rx; x < rx + rw; x += 16) {
            //            if ((rh < 32) && (rw > 64))
            //                PostProgressMessage("Receiving Data...", x - rx, rw);
            w = h = 16;
            if (rx + rw - x < 16)
                w = rx + rw - x;
            if (ry + rh - y < 16)
                h = ry + rh - y;

            if (!UEZVNC_Read(p, (unsigned char*)&subencoding, 1))
                return EFalse;

            // Raw section of bytes coming
            if (subencoding & rfbHextileRaw) {
                if (!UEZVNC_Read(p, p->iBuffer, (int)(w * h * 2)))
                    return EFalse;

                // Copy from the buffer to the screen
                p->iCallbacks->OffscreenCopyAreaFromBuffer((void *)p,
                    (T_pixelColor *)p->iBuffer, x, y, w, h);
                continue;
            }

            // Background color changed?
            if (subencoding & rfbHextileBackgroundSpecified)
                if (!UEZVNC_Read(p, (TUInt8 *)&bg, 2))
                    return EFalse;
            //dprintf(" spec bg %d r %dx%d (%d,%d)\n", bg, w, h, x, y);
            // Draw a solid region of background here
            p->iCallbacks->OffscreenDrawFilledRectangle((void *)p, bg, x, y, w,
                h);

            // Get a foreground color?
            if (subencoding & rfbHextileForegroundSpecified)
                if (!UEZVNC_Read(p, (unsigned char*)&fg, 2))
                    return EFalse;

            // Are we doing subrects?
            if (!(subencoding & rfbHextileAnySubrects)) {
                continue;
            }

            // Yes, doing subrects.  Get details
            if (!UEZVNC_Read(p, (unsigned char*)&nSubrects, 1))
                return EFalse;

            ptr = (TUInt8 *)p->iBuffer;

            // Does the subrect start with a color?
            if (subencoding & rfbHextileSubrectsColoured) {
                // Get the color information into the buffer (one per subrect)
                if (!UEZVNC_Read(p, p->iBuffer, nSubrects * (2 + 2)))
                    return EFalse;
                //dprintf(" %d subr. colored\n", nSubrects);

                // Fill in the solid subrects
                for (i = 0; i < nSubrects; i++) {
                    // What color?
                    GET_PIXEL16(fg, ptr);
                    //fg = *(ptr)++;

                    // Where is the xy in the subrect?
                    sx = rfbHextileExtractX(*ptr);
                    sy = rfbHextileExtractY(*ptr);
                    ptr++;

                    // What is the height and width of this region?
                    sw = rfbHextileExtractW(*ptr);
                    sh = rfbHextileExtractH(*ptr);
                    ptr++;

                    //dprintf(" color %d\n", fg);
                    // Draw this region to the screen
                    p->iCallbacks->OffscreenDrawFilledRectangle((void *)p, fg,
                        x + sx, y + sy, sw, sh);
                }
            } else {
                // Not colored, use the foreground color for this rectangle
                if (!UEZVNC_Read(p, p->iBuffer, nSubrects * 2))
                    return EFalse;

                for (i = 0; i < nSubrects; i++) {
                    // Where is the xy in the subrect?
                    sx = rfbHextileExtractX(*ptr);
                    sy = rfbHextileExtractY(*ptr);
                    ptr++;

                    // What is the height and width of this region?
                    sw = rfbHextileExtractW(*ptr);
                    sh = rfbHextileExtractH(*ptr);
                    ptr++;

                    //dprintf(" color %d\n", fg);
                    // Draw this region to the screen
                    p->iCallbacks->OffscreenDrawFilledRectangle((void *)p, fg,
                        x + sx, y + sy, sw, sh);
                }
            }
        }
    }

    return ETrue;
}
/*---------------------------------------------------------------------------*
 * Routine:  VNCAllocWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Allocate VNC workspace
 *
 *	@param [in] 	aNetwork	network device
 *                          
 *	@param [in]		aSocket		network socket
 *
 *	@param [out] 	*aCallbacks	TODO: pointer to VNC callbacks
 *                          
 *	@param [out]	*aServer	TODO: Pointer to VNC server
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void *VNCAllocWorkspace(
    T_uezDevice aNetwork,
    T_uezNetworkSocket aSocket,
    const T_UEZVNCCallbacks *aCallbacks,
    const T_UEZVNCServer *aServer)
{
    T_VNCWorkspace *p;

    // Allocate and abort if failed
    p = (T_VNCWorkspace *)UEZMemAlloc(sizeof(T_VNCWorkspace));
    if (!p)
        return 0;

    // Clear and initialize the structure
    memset(p, 0, sizeof(*p));
    p->iNetwork = aNetwork;
    p->iSocket = aSocket;
    p->iCallbacks = aCallbacks;
    p->iServer = *aServer;

    // Setup a scan width of 16x16 of pixels
    p->iBuffer2 = UEZMemAlloc(DISPLAY_WIDTH*16*sizeof(T_pixelColor));

    if (!p->iBuffer2) {
        UEZMemFree(p);
        p = 0;
    }
    return p;
}

/*---------------------------------------------------------------------------*
 * Routine:  VNCFreeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Free the VNC Work space
 *
 *	@param [in] 	*aWorkspace	Pointer to workspace
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void VNCFreeWorkspace(void *aWorkspace)
{
    T_VNCWorkspace *p;
    if (aWorkspace) {
        p = (T_VNCWorkspace *)aWorkspace;
        if (p->iBuffer2) {
            UEZMemFree(p->iBuffer2);
            p->iBuffer2 = 0;
        }
        UEZMemFree(aWorkspace);
    }
}

static TBool UEZVNC_Read(T_VNCWorkspace *p, TUInt8 *aBuffer, TUInt32 aNumBytes)
{
    TUInt32 readBytes;
    if (UEZNetworkSocketRead(p->iNetwork, p->iSocket, (void *)aBuffer,
        aNumBytes, &readBytes, VNC_READ_TIMEOUT) == UEZ_ERROR_NONE) {
        if (readBytes == aNumBytes)
            return ETrue;
    }
    return EFalse;
}

static T_uezError UEZVNC_ReadFast(T_VNCWorkspace *p, TUInt8 *aBuffer, TUInt32 aNumBytes)
{
    TUInt32 readBytes;
    T_uezError error;

    error = UEZNetworkSocketRead(p->iNetwork, p->iSocket, (void *)aBuffer,
            aNumBytes, &readBytes, 25);
    return error;
}

static TBool UEZVNC_Write(T_VNCWorkspace *p, TUInt8 *aBuffer, TUInt32 aNumBytes)
{
    if (UEZNetworkSocketWrite(p->iNetwork, p->iSocket, (void *)aBuffer,
        aNumBytes, ETrue, VNC_READ_TIMEOUT) == UEZ_ERROR_NONE) {
        return ETrue;
    }
    return EFalse;
}

/*-------------------------------------------------------------------------*
 * End of File:  uEZVNC_rfb.c
 *-------------------------------------------------------------------------*/
