/*-------------------------------------------------------------------------*
 * File:  AudioPlayerMode.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Load and play audio files from the MUSIC directory on the SD card
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZFile.h>
#include <uEZProcessor.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include "uEZDemoCommon.h"
#include <uEZMemory.h>
#include "UDCImages.h"
#include <Source/Library/Audio/MIDI/MIDIPlayer.h>
#include "AudioPlayer.h"

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#define MAX_CHOICES       10
#define AUDIO_TASK_SIZE  UEZ_TASK_STACK_BYTES(2048)

typedef struct {
    SWIM_WINDOW_T iWin;
    T_audioList iAudioList;
    T_choice iChoices[MAX_CHOICES+1];
    T_region iTop;
    T_region iChoiceBox;
    TBool iExit;
    T_uezDevice iLCD;
    T_uezError iDriveError;
} T_AudioPlayerWorkspace;

static T_AudioPlayerWorkspace *G_ws;
#define G_win           (G_ws->iWin)

TBool G_AudioIsRunning;
T_uezTask G_playerTask;
T_uezSemaphore G_AudioSemaphore;
T_midiPlayParameters G_MidiParams;
static TUInt8 G_topFileIndex;

static void AudioPlayerChoices(void);
static void AudioPlayerDraw(void);
static void AudioPlayerScreen(T_uezDevice lcd);
static void AudioPlayerStop(const T_choice *p_choice);

static void AudioPlayerPlay(const T_choice *p_choiceC)
{
	T_choice *p_choice = (T_choice *)p_choiceC;
	
	T_uezError error;
	char filepath[100] = {'\0'};
	//char filepath[100];
	
	if(G_AudioIsRunning)
	{
		G_AudioIsRunning = EFalse;
	}
	
	error = UEZSemaphoreGrab(G_AudioSemaphore, 1000);
	
	if(!error)        
	{		
		strcat(filepath, "1:/AUDIO/");
		strcat(filepath, p_choice->iText);	
	
		G_MidiParams.iFilename = filepath;
		G_MidiParams.iRun = &G_AudioIsRunning;
		G_MidiParams.iSem = &G_AudioSemaphore;

		UEZTaskCreate((T_uezTaskFunction)playMIDITask, "playMIDI",
	            AUDIO_TASK_SIZE, (void *)&G_MidiParams, UEZ_PRIORITY_NORMAL, 0);

	    AudioPlayerChoices();

		p_choice->iAction = AudioPlayerStop;
	    p_choice->iIcon = G_audioFileIconStop;

		AudioPlayerDraw();
	}
}

static void AudioPlayerStop(const T_choice *p_choiceC)
{
	if(G_AudioIsRunning)
	{
		G_AudioIsRunning = EFalse;		
	}

    AudioPlayerChoices();
    AudioPlayerDraw();
}

static void AudioPlayerOptionDraw(const T_choice *p_choice)
{
	
    T_region r;
    T_region icon;
    INT_32 imageX, imageY;
    TUInt16 fontHeight;
    T_region rtext;

    r.iLeft = p_choice->iLeft;
    r.iTop = p_choice->iTop;
    r.iRight = p_choice->iRight;
    r.iBottom = p_choice->iBottom;

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    fontHeight = swim_get_font_height(&G_win);

    // Inset a little for this inner area
    RegionShrinkTopBottom(&r, 2);


    // Draw the icon to the left
    RegionSplitFromLeft(&r, &icon, AUDIO_FILE_ICON_WIDTH, 2);
    if (p_choice->iIcon) {
        imageX = icon.iLeft;
        imageY = icon.iTop;
        swim_get_physical_xy(&G_win, &imageX, &imageY);
        SUIDrawIcon(
            p_choice->iIcon,
            imageX,
            imageY);
    }

    // Center vertically and draw the text to the right
    RegionCenterTopBottom(&r, &rtext, fontHeight);
    swim_put_text_xy(
        &G_win,
        p_choice->iText,
        rtext.iLeft,
        rtext.iTop);
	
}

static void AudioScrollUp(const T_choice *aChoice)
{
	if(G_topFileIndex > 0)
	{
    	G_topFileIndex--;
	
		AudioPlayerChoices();
	    AudioPlayerDraw();
	}
}

static void AudioScrollDown(const T_choice *aChoice)
{
    if(G_ws->iAudioList.iCount - G_topFileIndex > 4)
	{
    	G_topFileIndex++;
		
		AudioPlayerChoices();
	    AudioPlayerDraw();
	}
}

static void AudioPlayerExit(const T_choice *aChoice)
{
	G_AudioIsRunning = EFalse;
    PARAM_NOT_USED(aChoice);
    G_ws->iExit = ETrue;
}

static void AudioPlayerChoices(void)
{
    T_region r;
    T_region rlist;
    T_region rline;
    T_region rbottom;
    TUInt16 fontHeight;
    TUInt16 lineHeight;
    int i = 0;
    T_choice *p = G_ws->iChoices;

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    fontHeight = swim_get_font_height(&G_win);

    RegionShrink(&r, 10);

    RegionSplitFromBottom(&r, &rbottom, 4+fontHeight+2+EXIT_BUTTON_HEIGHT, 2);
    rlist = r;
	
    G_ws->iChoiceBox = rlist;
    RegionShrink(&rlist, 2);
    
	
	lineHeight = fontHeight+4;
    if (lineHeight < AUDIO_FILE_ICON_HEIGHT+4)
        lineHeight = AUDIO_FILE_ICON_HEIGHT+4;
	
    for (i=G_topFileIndex; i<MAX_CHOICES-1; i++, p++) {
        if (i>=G_ws->iAudioList.iCount)
            break;

        // Stop if there is not enough room for another line
        if ((1+rlist.iBottom - rlist.iTop)<(lineHeight+1))
            break;

        // Pull off some space
        RegionSplitFromTop(&rlist, &rline, lineHeight, 1);

        // Setup new choice
        p->iLeft = rline.iLeft;
        p->iTop = rline.iTop;
        p->iRight = rline.iRight-33;
        p->iBottom = rline.iBottom;
        p->iText = G_ws->iAudioList.iList[i].iName;
        p->iAction = AudioPlayerPlay;
        p->iIcon = G_audioFileIconPlay ;
        p->iData = &G_ws->iAudioList.iList[i];
        p->iDraw = AudioPlayerOptionDraw;
        G_ws->iChoiceBox.iBottom = rline.iBottom+2;
    }
	
	
    // Now add the back button
    RegionShrink(&rbottom, 1);
    p->iLeft = rbottom.iLeft;
    p->iRight = rbottom.iLeft+EXIT_BUTTON_WIDTH-1;
    p->iTop = rbottom.iTop;
    p->iBottom = rbottom.iTop+EXIT_BUTTON_HEIGHT-1;
    p->iText = "";
    p->iAction = AudioPlayerExit;
    p->iIcon = G_exitIcon;
    p->iData = 0;
    p->iDraw = 0; // Use default
	p++;
	
	// Now add the up button
    RegionShrink(&rbottom, 1);
    p->iLeft = G_ws->iChoiceBox.iRight-33;
    p->iRight = G_ws->iChoiceBox.iRight-1;
    p->iTop = G_ws->iChoiceBox.iTop+1;
    p->iBottom = G_ws->iChoiceBox.iTop+33;
    p->iText = "";
    p->iAction = AudioScrollUp;
    p->iIcon = G_arrowUp;
    p->iData = 0;
    p->iDraw = 0;
	p++;
	
	// Now add the down button
    RegionShrink(&rbottom, 1);
    p->iLeft = G_ws->iChoiceBox.iRight-33;
    p->iRight = G_ws->iChoiceBox.iRight-1;
    p->iTop = G_ws->iChoiceBox.iBottom-33;
    p->iBottom = G_ws->iChoiceBox.iBottom-1;
    p->iText = "";
    p->iAction = AudioScrollDown;
    p->iIcon = G_arrowDown;
    p->iData = 0;
    p->iDraw = 0;

    // Next entry is the end of choices marker
    p++;
    p->iText = 0; // end of list marker*/
}

static void AudioPlayerDraw(void)
{
    static const char *noFiles = "< No audio files found >";
	static const char *noDrive = "< No SD card found >";
    const char *p_msg = noFiles;
    TUInt32 width;
    TUInt16 fontHeight;
    T_region r1, r2;
    SUIHidePage0();

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    fontHeight = swim_get_font_height(&G_win);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_fill_color(&G_win, BLACK);

    swim_put_box(&G_win, 
          G_ws->iChoiceBox.iLeft,
          G_ws->iChoiceBox.iTop,
          G_ws->iChoiceBox.iRight,
          G_ws->iChoiceBox.iBottom);

    if (G_ws->iAudioList.iCount==0) {
        p_msg = noDrive;
        if (G_ws->iDriveError == UEZ_ERROR_READ_WRITE_ERROR)
            p_msg = noDrive;
        else if (G_ws->iDriveError == UEZ_ERROR_NOT_FOUND)
            p_msg = noFiles;
        
        width = swim_get_text_line_width(&G_win, p_msg);
        RegionCenterLeftRight(&G_ws->iChoiceBox, &r1, width);
        RegionCenterTopBottom(&r1, &r2, fontHeight);
        swim_put_text_xy(&G_win, p_msg, r2.iLeft, r2.iTop);
    }

    ChoicesDraw(&G_win, G_ws->iChoices);

    SUIShowPage0();
}

static void AudioPlayerScreen(T_uezDevice lcd)
{
    T_pixelColor *pixels;

    SUIHidePage0();

    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
	
    swim_window_open(
        &G_win,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        pixels,
        0,
        0,
        DISPLAY_WIDTH-1,
        DISPLAY_HEIGHT-1,
        2,
        YELLOW,
        RGB(0, 0, 0),
        RED);
		
    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&G_win, "uEZ(tm) Audio Player", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_font(&G_win, &font_winfreesys14x16);
    
    SUIShowPage0();
}

T_uezError AudioPlayerLoadDirectory()
{
	T_uezFileEntry entry;
	T_uezError error;
	
	error = UEZFileFindStart("1:/AUDIO", &entry);
    if (!error)
	{
		TUInt32 fileNum = 0;
        do 
		{
			if(strstr(entry.iFilename, ".mid"))
			{
				TUInt32 i = 0;
				while( i < MAX_FILE_DEF_NAME_LEN )
					G_ws->iAudioList.iList[fileNum].iName[i++] = entry.iFilename[i];
				
				G_ws->iAudioList.iList[fileNum++].iSize = entry.iFilesize;
					
				G_ws->iAudioList.iCount++;
				printf("Audio File: %s\n", entry.iFilename);
			}
        } 
  		while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
    } 
 	else
	{
        printf("No files found.\n");
    }
 
    UEZFileFindStop(&entry);
	return error;
}


void AudioPlayerMode(const T_choice *aChoice)
{
    T_uezDevice lcd;
    T_uezQueue queue;
    T_uezDevice ts;
    T_uezInputEvent inputEvent;
	
	UEZSemaphoreCreateCounting(&G_AudioSemaphore, 10, 1);

    G_ws = UEZMemAlloc(sizeof(T_AudioPlayerWorkspace));
    if (!G_ws)
        return;
    memset(G_ws, 0, sizeof(*G_ws));

    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) 
	{
#if UEZ_REGISTER
        UEZQueueSetName(queue, "AudioPlayer", "\0");
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  
		{
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  
			{
                G_ws->iLCD = lcd;
      
	  			G_ws->iDriveError = AudioPlayerLoadDirectory();
                AudioPlayerScreen(lcd);
                AudioPlayerChoices();
                AudioPlayerDraw();
        
                // Sit here in a loop until we are done
                while (!G_ws->iExit) 
				{
                  //Wait till we get a touchscreen event
                  if (UEZQueueReceive(queue, &inputEvent, UEZ_TIMEOUT_INFINITE)==UEZ_ERROR_NONE)
				  {
                    ChoicesUpdate(&G_win, G_ws->iChoices, queue, 500);
                  }
                }
				
                UEZLCDClose(lcd);
            }
            UEZTSClose(ts, queue);
        }
        UEZQueueDelete(queue);
    }

    UEZMemFree(G_ws);
}


/*-------------------------------------------------------------------------*
 * End of File:  AudioPlayerMode.c
 *-------------------------------------------------------------------------*/
