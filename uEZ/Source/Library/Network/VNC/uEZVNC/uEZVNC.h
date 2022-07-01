/*-------------------------------------------------------------------------*
 * File:  uEZVNC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZVNC_H_
#define UEZVNC_H_

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
#include <uEZ.h>
#include <uEZNetwork.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define UEZVNC_PASSWORD_MAX_LENGTH      32
#define MAX_ENCODINGS 10

#ifndef VNC_READ_TIMEOUT
#define VNC_READ_TIMEOUT            5000 // 15000 // ms
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    UEZVNC_STATE_IDLE,
    UEZVNC_STATE_CONNECTING,
    UEZVNC_STATE_HANDSHAKING,
    UEZVNC_STATE_CONNECTION_FAILED,
    UEZVNC_STATE_AUTHENTICATION_FAILED,
    UEZVNC_STATE_AUTHENTICATION_NOT_NEEDED,
    UEZVNC_STATE_VERSIONS_INCOMPATIBLE,
    UEZVNC_STATE_AUTHENTICATED,
    UEZVNC_STATE_UNKNOWN_AUTHENTICATION,
} T_UEZVNCState;

typedef enum {
    UEZVNC_EVENT_PLAY_BELL,
    UEZVNC_EVENT_CUT_TEXT,
    UEZVNC_EVENT_UPDATE_COMPLETE,
    UEZVNC_EVENT_FRAMEBUFFER_SIZE_CHANGED,
} T_UEZVNCEvent;
typedef struct {
    void (*StateUpdate)(void *aWorkspace, T_UEZVNCState aState, void *aData);
    void (*Event)(void *aWorkspace, T_UEZVNCEvent aEvent, void *aData);
    void (*OffscreenDrawFilledRectangle)(
        void *aWorkspace,
        T_pixelColor aColor,
        TInt16 x,
        TInt16 y,
        TInt16 aWidth,
        TInt16 aHeight);
    void (*OffscreenCopyAreaFromBuffer)(
        void *aWorkspace,
        T_pixelColor *aBuffer,
        TInt16 x,
        TInt16 y,
        TInt16 aWidth,
        TInt16 aHeight);
    void (*DrawPixel)(
        void *aWorkspace,
        T_pixelColor color,
//        TUInt8 red,
//        TUInt8 green,
//        TUInt8 blue,
        TInt16 x,
        TInt16 y);
} T_UEZVNCCallbacks;

typedef struct {
    char iAddress[80];
    char iPassword[UEZVNC_PASSWORD_MAX_LENGTH + 1];
    TBool iIsSharedDesktop;
} T_UEZVNCServer;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void *VNCAllocWorkspace(
    T_uezDevice aNetwork,
    T_uezNetworkSocket aSocket,
    const T_UEZVNCCallbacks *aCallbacks,
    const T_UEZVNCServer *aServer);
void VNCFreeWorkspace(void *aWorkspace);

TBool VNCInitialiseRFBConnection(void *aWorkspace);
TBool VNCSetScaleFactor(void *aWorkspace, unsigned short scaleFactor);
TBool VNCSendIncrementalFramebufferUpdateRequest(void *aWorkspace);
TBool VNCSendFramebufferUpdateRequest(
    void *aWorkspace,
    TUInt16 x,
    TUInt16 y,
    TUInt16 w,
    TUInt16 h,
    TBool incremental);
T_uezError VNCHandleRFBServerMessage(void *aWorkspace);
TBool VNCSendPointerEvent(
    void *aWorkspace,
    TInt16 x,
    TInt16 y,
    TUInt8 buttonMask);
TBool VNCSendKeyEvent(void *aWorkspace, TUInt32 key, TBool down);
TBool VNCSetFormatAndEncodings(void *aWorkspace);

#ifdef __cplusplus
}
#endif

#endif // UEZVNC_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZVNC.h
 *-------------------------------------------------------------------------*/
