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
#include <uEZMemory.h>
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
#include "task.h"
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include <HAL/Interrupt.h>

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

#define PRINT_VIDEO_STATISTICS 0

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezDevice iLCD;
    SWIM_WINDOW_T iWin[MAX_DISPLAY_FRAMES];
    TUInt32 iFrame;
    TUInt32 iFrameCount;
    float iMSPerFrame;
    float iSoundNextPartial;
    TUInt32 iTimeStart;
    TUInt32 iHiddenFrame;
    TUInt32 iVideoPosY;
    TUInt32 iVideoPosX;
    TUInt32 iVideoOffset;
    T_uezFile iFile;
    TBool iExit;
} T_VideoPlayerWorkspace;

#if (UEZBSP_SDRAM_SIZE > (8*1024*1024))
#define FILE_SYSTEM_TABLE_BUFFER_SIZE (512*1024)
uint32_t clmt[FILE_SYSTEM_TABLE_BUFFER_SIZE];// Cluster link map table buffer
#else
#define FILE_SYSTEM_TABLE_BUFFER_SIZE (4*1024)
UEZ_PUT_SECTION(".IRAM", uint32_t clmt[FILE_SYSTEM_TABLE_BUFFER_SIZE]); // Cluster link map table buffer
#endif

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_VideoInfo *G_pVideoInfo;
static wavFileHeader G_WaveFile;

T_uezError VideoPlayer_Open(T_VideoPlayerWorkspace *p_ws)
{
    T_uezError error;
    TUInt32 length;

    error = UEZFileOpen(G_pVideoInfo->iVideoPath, FILE_FLAG_READ_ONLY, &p_ws->iFile);
    if (!error) {
        error = UEZFileGetLength(p_ws->iFile, &length);
        p_ws->iFrameCount = length / (G_pVideoInfo->iVideoHeight * G_pVideoInfo->iVideoWidth * 2);
        p_ws->iFrame = 0;
        
        clmt[0] = FILE_SYSTEM_TABLE_BUFFER_SIZE; // Set table size
        // If not supported (disabled) or buffer is too small, we can just ignore the error return and it will read the file normally with slow seek.
        // This will populate the buffer table with a sector map for faster seeking to avoid re-reading the table over and over again.
        error = UEZFileSetTableBuffer(p_ws->iFile, &clmt[0]);
    } else if(error == UEZ_ERROR_NOT_FOUND) {
        //UEZFailureMsg("UEZFileOpen Failed: Video File Not Found!");
    } else {
        //UEZFailureMsg("UEZFileOpen failed to open Video File!");
    }
    return error;
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
    // T_VideoInfo.iTitle not printed on screen during video playback.

    if(G_pVideoInfo->iTextLine1[0] == 0) { // check if both text strings are empty
      if(G_pVideoInfo->iTextLine2[0] == 0) {
        fontHeight = 0; // Make sure the video is centered if there is no text.
      }
    }

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

#if (APP_DEMO_VIDEO_PLAYER == 1)
UEZ_PUT_SECTION(".video", static TUInt8 G_videoBuffer[MAX_VIDEO_WIDTH*MAX_VIDEO_HEIGHT*2]); // MAX video size
#else
UEZ_PUT_SECTION(".video", static TUInt8 G_videoBuffer[4]); // dummy variable in case it tries to place memory
#endif

T_uezError VideoPlayer_DrawNextFrame(T_VideoPlayerWorkspace *p_ws)
{
    T_uezError error;
    TUInt32 numRead, i;
    TUInt8 *pSource, *pDest;

    if( (DISPLAY_WIDTH == G_pVideoInfo->iVideoWidth) && (DISPLAY_HEIGHT == G_pVideoInfo->iVideoHeight) ) {
        error = UEZFileRead(p_ws->iFile, FRAME(p_ws->iHiddenFrame), (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2), &numRead);
        if(error) {
            return error;
            //UEZFailureMsg("Frame Read Error");
        }
        if(numRead != (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2)) {
            return error;
            //UEZFailureMsg("Invalid Frame Size");
        }
    } else { // playing video that isn't full screen
        error = UEZFileRead(p_ws->iFile, (void *)G_videoBuffer, (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2), &numRead);
        if(error) {
            return error;
            //UEZFailureMsg("Frame Read Error");
        }
        if(numRead != (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2)) {
            return error;
            //UEZFailureMsg("Invalid Frame Size");
        }

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
    return UEZ_ERROR_NONE;
}

static TUInt32 G_totalSkipCount = 0;

void VideoPlayer_SyncWithAudio(T_VideoPlayerWorkspace *p_ws) {
    TUInt32 skipCount, filePosition;
    TUInt32 soundPos;
    float soundMS, soundNext;

    soundPos = UEZDACWAVGetSamplePos();
    
    soundMS = (float)(((float)(soundPos)*(float)100.0) / (float)(G_WaveFile.iSampleRate));
    //soundMS = (float)((float)soundPos*(float)p_ws->iSoundNextPartial);

    // At 15 fps (66.66 ms per frame), where should we be next?
    soundNext = (p_ws->iFrame + 1) * p_ws->iMSPerFrame;

    if (soundNext > soundMS) { // too fast
        // If the frames are ahead of audio, wait
        UEZTaskDelay((TUInt32)(soundNext - soundMS));
    } else {
        // If the frames are behind the audio, we need to skip a frame or two to catch up.
        skipCount = 0;

        while((soundMS - soundNext) > p_ws->iMSPerFrame) { // too slow, skip a frame
            if (p_ws->iFrame == p_ws->iFrameCount) // if this is the last frame, don't skip
                break;
            skipCount++;
            p_ws->iFrame++;
            soundMS -= p_ws->iMSPerFrame;
        }
        if(skipCount>0) { // takes around ? us on 4357 4-bit mode SD 51MHz
            UEZFileTellPosition(p_ws->iFile, &filePosition);
            UEZFileSeekPosition(p_ws->iFile, filePosition+(G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2 * skipCount));
        }

        G_totalSkipCount+=skipCount;
    }
}

void VideoPlayer_SyncWithTimer(T_VideoPlayerWorkspace *p_ws) {
    TUInt32 timeMS, timeNext;
    TUInt32 skipCount, filePosition;

    timeMS = UEZTickCounterGetDelta(p_ws->iTimeStart);
    timeNext = (TUInt32)((p_ws->iFrame + 1) * p_ws->iMSPerFrame);

    if (timeNext > timeMS) { // too fast
        // Add a delay to meet the desired frame rate
        UEZTaskDelay(timeNext - timeMS);
    } else { // not too fast
        // If the frames are behind, we need to skip a frame or two to catch up.
        skipCount = 0;

        while((timeMS - timeNext) > p_ws->iMSPerFrame) { // too slow, skip a frame
            if (p_ws->iFrame == p_ws->iFrameCount) // if this is the last frame, don't skip
                break;
            skipCount++;
            p_ws->iFrame++;
            timeMS -= p_ws->iMSPerFrame;
        }
        if(skipCount>0) { // takes around ? us on 4357 4-bit mode SD 51MHz
            UEZFileTellPosition(p_ws->iFile, &filePosition);
            UEZFileSeekPosition(p_ws->iFile, filePosition+(G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2 * skipCount));
        }

        G_totalSkipCount+=skipCount;
    }
}

// Currently uEZ sets up to 10 tasks, but its not defined in a header file yet.
T_uezPriority PriorityList[10] = {UEZ_PRIORITY_IDLE}; // TODO can't find the actual FreeRTOS max task num define or it doesn't exist in this version.
extern T_uezTask G_mainTask;
extern T_uezTask G_hTouchTask;
extern T_uezTask G_tsMonitorTask;
extern T_uezTask G_usbHostTask;
extern T_uezTask G_heartBeatTask;
extern T_uezTask G_DACAudioTask;

extern void * G_UEZGUIWorkspace;

void VideoPlayer_deprioritize_other_tasks(void)
{
#if (APP_ENABLE_HEARTBEAT_LED == 1)
  UEZTaskSuspend(G_heartBeatTask);
#endif

  T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)G_UEZGUIWorkspace;
  if(p != 0) {
     UEZTaskSuspend(p->iTask);
  }

  UEZTaskPriorityGet(G_mainTask, &PriorityList[0]);
  //EZTaskPrioritySet(G_mainTask, UEZ_PRIORITY_HIGH);
  UEZTaskPrioritySet(G_mainTask, UEZ_PRIORITY_NORMAL);

  //G_tsMonitorTask = (T_uezTask) xTaskGetHandle("TS_Mon");
  // change TS_Mon task (separate from emWin touch task)
  UEZTaskPriorityGet(G_tsMonitorTask, &PriorityList[1]);
  UEZTaskPrioritySet(G_tsMonitorTask, UEZ_PRIORITY_VERY_LOW);
  //UEZTaskSuspend(G_tsMonitorTask);  // could disable touchscreen task to prevent being able to stop video.

#if (UEZ_ENABLE_USB_HOST_STACK==1) // lower prio of USB tasks
#if(UEZ_PROCESSOR == NXP_LPC4357)
  UEZTaskPriorityGet(G_usbHostTask, &PriorityList[2]);
  UEZTaskPrioritySet(G_usbHostTask, UEZ_PRIORITY_LOW);
  UEZTaskSuspend(G_usbHostTask);
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)
  UEZTaskPriorityGet(G_usbHostTask, &PriorityList[2]);
  UEZTaskPrioritySet(G_usbHostTask, UEZ_PRIORITY_LOW);
  UEZTaskSuspend(G_usbHostTask);
#endif
#if(UEZ_PROCESSOR == NXP_LPC1788)
  UEZTaskPriorityGet(G_usbHostTask, &PriorityList[2]);
  UEZTaskPrioritySet(G_usbHostTask, UEZ_PRIORITY_LOW);
  UEZTaskSuspend(G_usbHostTask);
#endif
#endif
  
  if(G_DACAudioTask != 0) { // want lower prio dac audio task that just reads files every few seconds, 
    // and task sleeps until buffer empty. real action happens in timer ISR
    UEZTaskPriorityGet(G_DACAudioTask, &PriorityList[3]);
    UEZTaskPrioritySet(G_DACAudioTask, UEZ_PRIORITY_NORMAL);
  }

#if (UEZ_ENABLE_TCPIP_STACK==1)
  // TODO
#endif
  
#if(UEZ_PROCESSOR == NXP_LPC4357)
  InterruptSetPriority(TIMER2_IRQn, INTERRUPT_PRIORITY_HIGHEST);
#endif
}

void VideoPlayer_return_other_task_priorities(void)
{
#if (APP_ENABLE_HEARTBEAT_LED == 1)
  UEZTaskResume(G_heartBeatTask);
#endif

  T_FDICmdWorkspace *p = (T_FDICmdWorkspace *)G_UEZGUIWorkspace;
  if(p != 0) {
     UEZTaskResume(p->iTask);
  }

  UEZTaskPrioritySet(G_mainTask, PriorityList[0]);

  UEZTaskPrioritySet(G_tsMonitorTask, PriorityList[1]); //  return touch prio
  //UEZTaskResume(G_tsMonitorTask); // resume touchscreen task if disabled

#if (UEZ_ENABLE_USB_HOST_STACK==1) // return prio of USB tasks
#if(UEZ_PROCESSOR == NXP_LPC4357)
  UEZTaskPrioritySet(G_usbHostTask, PriorityList[2]);
  UEZTaskResume(G_usbHostTask);
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)
  UEZTaskPrioritySet(G_usbHostTask, PriorityList[2]);
  UEZTaskResume(G_usbHostTask);
#endif
#if(UEZ_PROCESSOR == NXP_LPC1788)
  UEZTaskPrioritySet(G_usbHostTask, PriorityList[2]);
  UEZTaskResume(G_usbHostTask);
#endif
#endif

  if(G_DACAudioTask != 0) {
    UEZTaskPrioritySet(G_DACAudioTask, PriorityList[3]);
  }

#if (UEZ_ENABLE_TCPIP_STACK==1)
  // TODO
#endif
  
#if(UEZ_PROCESSOR == NXP_LPC4357)
  InterruptSetPriority(TIMER2_IRQn, INTERRUPT_PRIORITY_HIGH);
#endif
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
    TUInt32 backlightLevel, aNumberBacklightLevels;
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

#if (PRINT_VIDEO_STATISTICS == 1)
    printf("\r\nVideo FPS %u\n", G_pVideoInfo->iFPS);
#endif

    if(VideoPlayer_Open(&ws) != UEZ_ERROR_NONE){
      return;
    }

    UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue);
#if UEZ_REGISTER
        UEZQueueSetName(queue, "VideoPlayer", "\0");
#endif
#if UEZ_ENABLE_BUTTON_BOARD
    UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
    UEZTSOpen("Touchscreen", &tsDevice, &queue);
    UEZLCDOpen("LCD", &ws.iLCD);
    UEZLCDGetBacklightLevel(ws.iLCD, &backlightLevel, &aNumberBacklightLevels);
    VideoPlayer_deprioritize_other_tasks();
    if(backlightLevel < 100) {
      backlightLevel = 31;
      UEZLCDBacklight(ws.iLCD, backlightLevel);
    }

    // Clear both frames
    memset(FRAME(0), 0, DISPLAY_WIDTH*DISPLAY_HEIGHT*2);
    memset(FRAME(1), 0, DISPLAY_WIDTH*DISPLAY_HEIGHT*2);

    // Put the draw screen up
    VideoPlayer_Screen(&ws, 0);
    VideoPlayer_Screen(&ws, 1);

    G_totalSkipCount = 0;
    ws.iTimeStart = UEZTickCounterGet();

    error = UEZDACWAVPlay(G_pVideoInfo->iAudioPath, &G_WaveFile);
    G_WaveFile.iSampleRate /= 10;
    if(G_WaveFile.iSampleRate>0) {
      ws.iSoundNextPartial = (float)(((float)100.0) / (float)(G_WaveFile.iSampleRate));
    }

    if(error == UEZ_ERROR_NONE)
        isAudioPlaying = ETrue;
    else
        isAudioPlaying = EFalse;


    while (!ws.iExit) {

        if(VideoPlayer_DrawNextFrame(&ws) == UEZ_ERROR_NONE) {
          // keep going
          if(isAudioPlaying) {
            VideoPlayer_SyncWithAudio(&ws);
          } else {
            VideoPlayer_SyncWithTimer(&ws);
          }
        } else { // attempt to re-init SD card or just skip/exit if we fail
          ws.iFrame++;
          G_totalSkipCount+=1;
          if (ws.iFrame == ws.iFrameCount) {
              ws.iExit = ETrue;
          }
          // TODO finalize this, maybe only retry 5 times total then fail if we couldn't re-init.
          //UEZPlatform_SDCard_Drive_Require(1);
          //UEZFileOpen(G_pVideoInfo->iVideoPath, FILE_FLAG_READ_ONLY, &p_ws->iFile);
          //UEZFileSeekPosition(ws.iFile, (G_pVideoInfo->iVideoWidth * G_pVideoInfo->iVideoHeight * 2 * ws.iFrame));

          //if(isAudioPlaying) {
              //VideoPlayer_SyncWithAudio(&ws);
          //} else {
            VideoPlayer_SyncWithTimer(&ws);
          //}
        }
    
        if (UEZQueueReceive(queue, &inputEvent, 0) == UEZ_ERROR_NONE) {
            // Pen down or up?
            if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
                ws.iExit = ETrue;
            }
            if (inputEvent.iEvent.iButton.iAction == BUTTON_ACTION_PRESS) {
                ws.iExit = ETrue;
            }
        }
        
        if(backlightLevel < 224) {
            backlightLevel = backlightLevel + 32;
            UEZLCDBacklight(ws.iLCD, backlightLevel);
        }

        if (ws.iFrame == ws.iFrameCount) {
            ws.iExit = ETrue;
        }
    } // while (!ws.iExit)

#if (PRINT_VIDEO_STATISTICS == 1)
    printf("Total Skip Count: %u out of %u total frames\r\n", G_totalSkipCount, ws.iFrameCount);
#endif

    UEZDACWAVStop();
    UEZFileClose(ws.iFile);
    UEZLCDClose(ws.iLCD);
    VideoPlayer_return_other_task_priorities();
#if UEZ_ENABLE_BUTTON_BOARD
    UEZKeypadClose(keypadDevice, &queue);
#endif
    UEZTSClose(tsDevice, queue);
    UEZQueueDelete(queue);
}

/*-------------------------------------------------------------------------*
 * End of File:  VideoPlayer.c
 *-------------------------------------------------------------------------*/
