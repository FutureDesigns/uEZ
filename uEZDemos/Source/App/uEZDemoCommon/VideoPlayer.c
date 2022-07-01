/*-------------------------------------------------------------------------*
 * File:  VideoPlayer.c
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uEZ.h>
#include <uEZFile.h>
#include <uEZLCD.h>
#include <uEZRTOS.h>
#include <uEZTickCounter.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include "uEZDemoCommon.h"
#include "AppDemo.h"
#include "VideoPlayer.h"
#include <uEZKeypad.h>
#include <uEZPlatform.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#if(UEZ_PROCESSOR != NXP_LPC4357)
#define MAX_VIDEO_WIDTH     480
#define MAX_VIDEO_HEIGHT    272
#else
//LPC4357 can play video at a higher resolution
//SDCard WVGA @ 10fps
//USB HS (on-board) WVGA @ 15fps
#define MAX_VIDEO_WIDTH     800
#define MAX_VIDEO_HEIGHT    480
#endif
#define MAX_DISPLAY_FRAMES  2

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezDevice iLCD;
    SWIM_WINDOW_T iWin[MAX_DISPLAY_FRAMES];
    TUInt32 iFrame;
    TUInt32 iFrameCount;
    float iMSPerFrame;
    TUInt32 iHiddenFrame;
    TUInt32 iVideoPosY;
    TUInt32 iVideoPosX;
    TUInt32 iVideoOffset;
    T_uezFile iFile;
    TBool iExit;
} T_VideoPlayerWorkspace;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_VideoInfo *G_pVideoInfo;
static wavFileHeader G_WaveFile;

void VideoPlayer_Open(T_VideoPlayerWorkspace *p_ws)
{
    T_uezError error;
    TUInt32 length;

    error = UEZFileOpen(G_pVideoInfo->iVideoPath, FILE_FLAG_READ_ONLY, &p_ws->iFile);
    if (!error) {
        error = UEZFileGetLength(p_ws->iFile, &length);
        p_ws->iFrameCount = length / (G_pVideoInfo->iVideoHeight * G_pVideoInfo->iVideoWidth * 2);
        p_ws->iFrame = 0;
    } else if(error == UEZ_ERROR_NOT_FOUND) {
        UEZFailureMsg("UEZFileOpen Failed: Video File Not Found!");
    } else {
        UEZFailureMsg("UEZFileOpen failed to open Video File!");
    }
}

void VideoPlayer_Screen(T_VideoPlayerWorkspace *p_ws, TUInt8 aFrame) {
    T_pixelColor *pixels;
    TUInt32 fontHeight, xPos, yPos, videoSectionHeight;

    if(aFrame >= MAX_DISPLAY_FRAMES)
        return;

    UEZLCDGetFrame(p_ws->iLCD, aFrame, (void **)&pixels);
    swim_window_open(&p_ws->iWin[aFrame], DISPLAY_WIDTH, DISPLAY_HEIGHT, pixels, 0, 0,
            DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, 2, BLACK, RGB(0, 0, 0), RED);

    swim_set_font(&p_ws->iWin[aFrame], &APP_DEMO_DEFAULT_FONT);
    swim_set_fill_color(&p_ws->iWin[aFrame], BLACK);
    swim_set_pen_color(&p_ws->iWin[aFrame], YELLOW);
    fontHeight = swim_get_font_height(&p_ws->iWin[aFrame]);

    if( (G_pVideoInfo->iVideoHeight + (fontHeight*6)) < DISPLAY_HEIGHT) {

        videoSectionHeight = G_pVideoInfo->iVideoHeight + fontHeight*3;

        xPos = (DISPLAY_WIDTH/2) - (G_pVideoInfo->iVideoWidth/2);
        yPos = (DISPLAY_HEIGHT/2) - (videoSectionHeight/2);
        p_ws->iVideoOffset = ((DISPLAY_WIDTH*yPos) + xPos)*2;

        swim_put_text_horizontal_centered(
           &p_ws->iWin[aFrame],
           G_pVideoInfo->iTextLine1,
           DISPLAY_WIDTH,
           yPos+G_pVideoInfo->iVideoHeight+fontHeight);
        swim_put_text_horizontal_centered(
           &p_ws->iWin[aFrame],
           G_pVideoInfo->iTextLine2,
           DISPLAY_WIDTH,
           yPos+G_pVideoInfo->iVideoHeight+fontHeight*2);
    } else {
        // No text, center the video offset
        xPos = (DISPLAY_WIDTH/2) - (G_pVideoInfo->iVideoWidth/2);
        yPos = (DISPLAY_HEIGHT/2) - (G_pVideoInfo->iVideoHeight/2);
        p_ws->iVideoOffset = ((DISPLAY_WIDTH*yPos) + xPos)*2;
    }
}

#if (COMPILER_TYPE==RowleyARM)
#define ATTR_LOAD_SPACE 
#else
#define ATTR_LOAD_SPACE
#endif

void VideoPlayer_DrawNextFrame(T_VideoPlayerWorkspace *p_ws)
{
    T_uezError error;
    TUInt32 numRead, i;
    char *pSource, *pDest;
    static char G_videoBuffer[MAX_VIDEO_WIDTH*MAX_VIDEO_HEIGHT*2] ATTR_LOAD_SPACE; // MAX video size


    if( (DISPLAY_WIDTH == G_pVideoInfo->iVideoWidth) && (DISPLAY_HEIGHT == G_pVideoInfo->iVideoHeight) ) {
        error = UEZFileRead(p_ws->iFile, FRAME(p_ws->iHiddenFrame), (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2), &numRead);
        if(error)
            UEZFailureMsg("Frame Read Error");
        if(numRead != (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2))
            UEZFailureMsg("Invalid Frame Size");
    } else {
        error = UEZFileRead(p_ws->iFile, (void *)G_videoBuffer, (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2), &numRead);
        if(error)
            UEZFailureMsg("Frame Read Error");
        if(numRead != (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2))
            UEZFailureMsg("Invalid Frame Size");

        // Center the video
        pDest = FRAME(p_ws->iHiddenFrame) + p_ws->iVideoOffset;

        pSource = G_videoBuffer;
        for(i=0;i<G_pVideoInfo->iVideoHeight;i++) {
            memcpy((void *)pDest, (void *)pSource, G_pVideoInfo->iVideoWidth*2);
            pSource+=(G_pVideoInfo->iVideoWidth*2);
            pDest+=(DISPLAY_WIDTH*2);
        }
    }

    SUICallbackSetLCDBase((void *)(FRAME(p_ws->iHiddenFrame)));
    UEZLCDWaitForVerticalSync(p_ws->iLCD, 1000);

    p_ws->iHiddenFrame ^= 1;
    p_ws->iFrame++;
}

static TUInt32 G_totalSkipCount = 0;

void VideoPlayer_SyncWithAudio(T_VideoPlayerWorkspace *p_ws) {
    TUInt32 skipCount, filePosition;
    TUInt32 soundPos;
    float soundMS, soundNext;

    soundPos = UEZDACWAVGetSamplePos();
    soundMS = (float)(((float)(soundPos)*100.0) / (float)(G_WaveFile.iSampleRate));

    // At 15 fps (66.66 ms per frame), where should we be next?
    soundNext = (p_ws->iFrame + 1) * p_ws->iMSPerFrame;

    if (soundNext > soundMS) {
        // If the frames are ahead of audio, wait
        UEZTaskDelay((TUInt32)(soundNext - soundMS));
    } else {
        // If the frames are behind the audio, we need to skip a frame or two
        // to catch up.
        skipCount = 0;

        while((soundMS - soundNext) > p_ws->iMSPerFrame) {
            if (p_ws->iFrame == p_ws->iFrameCount)
                break;
            skipCount++;
            p_ws->iFrame++;
            soundMS -= p_ws->iMSPerFrame;
        }
        if(skipCount>0) {
            UEZFileTellPosition(p_ws->iFile, &filePosition);
            UEZFileSeekPosition(p_ws->iFile, filePosition+(G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2 * skipCount));
        }

        G_totalSkipCount+=skipCount;
    }
}

void VideoPlayer_SyncWithTimer(T_VideoPlayerWorkspace *p_ws) {
    TUInt32 timeMS, timeNext;

    timeMS = UEZTickCounterGet();
    timeNext = (TUInt32)((p_ws->iFrame + 1) * p_ws->iMSPerFrame);

    if (timeNext > timeMS) {
        // Add a delay to meet the desired frame rate
        UEZTaskDelay(timeNext - timeMS);
    }
}

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void VideoPlayer(const T_choice *aChoice)
{
    T_VideoPlayerWorkspace ws;
    T_uezDevice tsDevice;
    T_uezQueue queue;
    T_uezInputEvent inputEvent;
    T_uezError error;
    TBool isAudioPlaying;
#if UEZ_ENABLE_BUTTON_BOARD
    T_uezDevice keypadDevice;
#endif
    PARAM_NOT_USED(aChoice);

    G_pVideoInfo = (T_VideoInfo *)aChoice->iData;

    // Calculate the LCD offset
    ws.iExit = EFalse;
    ws.iHiddenFrame = 1;
    //ws.iMSPerFrame = (1000 / G_pVideoInfo->iFPS);
    ws.iMSPerFrame = ((float)1000 / (float)G_pVideoInfo->iFPS);

    VideoPlayer_Open(&ws);

    UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue);
#if UEZ_REGISTER
        UEZQueueSetName(queue, "VideoPlayer", "\0");
#endif
#if UEZ_ENABLE_BUTTON_BOARD
    UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
    UEZTSOpen("Touchscreen", &tsDevice, &queue);
    UEZLCDOpen("LCD", &ws.iLCD);

    // Clear both frames
    memset(FRAME(0), 0, DISPLAY_WIDTH*DISPLAY_HEIGHT*2);
    memset(FRAME(1), 0, DISPLAY_WIDTH*DISPLAY_HEIGHT*2);

    // Put the draw screen up
    VideoPlayer_Screen(&ws, 0);
    VideoPlayer_Screen(&ws, 1);

    error = UEZDACWAVPlay(G_pVideoInfo->iAudioPath, &G_WaveFile);
    G_WaveFile.iSampleRate /= 10;
    if(error == UEZ_ERROR_NONE)
        isAudioPlaying = ETrue;
    else
        isAudioPlaying = EFalse;

    G_totalSkipCount = 0;

    while (!ws.iExit) {

        if (UEZQueueReceive(queue, &inputEvent, 0) == UEZ_ERROR_NONE) {
            // Pen down or up?
            if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
                ws.iExit = ETrue;
            }
            if (inputEvent.iEvent.iButton.iAction == BUTTON_ACTION_PRESS) {
                ws.iExit = ETrue;
            }
        }

        VideoPlayer_DrawNextFrame(&ws);

        if(isAudioPlaying)
            VideoPlayer_SyncWithAudio(&ws);
        else
            VideoPlayer_SyncWithTimer(&ws);

        if (ws.iFrame == ws.iFrameCount)
            ws.iExit = ETrue;
    }
    printf("Total Skip Count: %d\r\n", G_totalSkipCount);

    UEZDACWAVStop();
    UEZFileClose(ws.iFile);
    UEZLCDClose(ws.iLCD);
#if UEZ_ENABLE_BUTTON_BOARD
    UEZKeypadClose(keypadDevice, &queue);
#endif
    UEZTSClose(tsDevice, queue);
    UEZQueueDelete(queue);
}

/*-------------------------------------------------------------------------*
 * End of File:  VideoPlayer.c
 *-------------------------------------------------------------------------*/
