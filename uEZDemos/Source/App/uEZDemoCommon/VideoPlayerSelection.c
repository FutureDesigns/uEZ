/*-------------------------------------------------------------------------*
 * File:  MultiVideoPlayerSelect.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Load several USB pages and give a slide show.
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
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <uEZFile.h>
#include <uEZProcessor.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>
#include "uEZDemoCommon.h"
#include <uEZMemory.h>
#include "UDCImages.h"
#include <uEZINI.h>
#include <uEZLCD.h>
#include "AppDemo.h"
#include "VideoPlayer.h"
#include <uEZKeypad.h>

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#define MAX_VIDEOS       10

#if SIMPLEUI_DOUBLE_SIZED_ICONS
#define VIDEO_ICON_HEIGHT 64
#define VIDEO_ICON_WIDTH 64
#elif (UEZ_ICONS_SET == ICONS_SET_PROFESSIONAL_ICONS_LARGE)
#define VIDEO_ICON_HEIGHT 64
#define VIDEO_ICON_WIDTH 64
#else
#define VIDEO_ICON_HEIGHT 32
#define VIDEO_ICON_WIDTH 32
#endif

typedef struct {
    SWIM_WINDOW_T iWin;
    T_VideoInfo iVideos[MAX_VIDEOS];
    T_choice iChoices[MAX_VIDEOS];
    TUInt8 iNumVideos;
    T_region iTop;
    T_region iChoiceBox;
    TBool iExit;
    T_uezDevice iLCD;
    TUInt32 iDrive;
    T_uezError iDriveError;
} T_VideoPlayerSelectWorkspace;

static T_VideoPlayerSelectWorkspace *G_ws;
#define G_win           (G_ws->iWin)
static TUInt8 G_topFileIndex;

static void VideoPlayerSelectChoices(void);
static void VideoPlayerSelectDraw(void);
static void VideoPlayerSelectScreen(T_uezDevice lcd);

static void VideoPlayerSelectAction(const T_choice *aChoice)
{
    VideoPlayer(aChoice);

    VideoPlayerSelectScreen(G_ws->iLCD);
    VideoPlayerSelectChoices();
    VideoPlayerSelectDraw();
}

static void VideoPlayerSelectScrollUp(const T_choice *aChoice)
{
    if(G_topFileIndex > 0) {
        G_topFileIndex--;

        VideoPlayerSelectChoices();
        VideoPlayerSelectDraw();
    }
}

static void VideoPlayerSelectScrollDown(const T_choice *aChoice)
{
    if((G_topFileIndex+5) < G_ws->iNumVideos) {
        G_topFileIndex++;

        VideoPlayerSelectChoices();
        VideoPlayerSelectDraw();
    }
}

static void VideoPlayerSelectExit(const T_choice *aChoice)
{
    PARAM_NOT_USED(aChoice);
    G_ws->iExit = ETrue;
}


static void VideoPlayerSelectOptionDraw(const T_choice *p_choice)
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
    RegionSplitFromLeft(&r, &icon, VIDEO_ICON_WIDTH, 2);
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

static void VideoPlayerSelectChoices(void)
{
    T_region r;
    T_region rlist;
    T_region rline;
    T_region rbottom;
    TUInt16 fontHeight;
    TUInt16 lineHeight;
    int i = 0;
    T_choice *p = G_ws->iChoices;
    int count = 0;

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    fontHeight = swim_get_font_height(&G_win);

    RegionShrink(&r, 5);

    RegionSplitFromBottom(&r, &rbottom, 4+fontHeight+2+EXIT_BUTTON_HEIGHT, 2);
    rlist = r;

    G_ws->iChoiceBox = rlist;
    RegionShrink(&rlist, 2);

    lineHeight = fontHeight+4;
    if (lineHeight < VIDEO_ICON_HEIGHT+4)
        lineHeight = VIDEO_ICON_HEIGHT+4;

    for (i=G_topFileIndex; i<MAX_VIDEOS; i++, p++) {

        if (i>=G_ws->iNumVideos)
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
        p->iText = G_ws->iVideos[i].iTitle;
        p->iAction = VideoPlayerSelectAction;
        p->iIcon = G_slideshowSmallIcon;
        p->iData = &G_ws->iVideos[i];
        p->iDraw = VideoPlayerSelectOptionDraw;
        G_ws->iChoiceBox.iBottom = rline.iBottom+2;

        count++;
    }

    /*
    // Added to make sure window goes to the bottom even if list is too small
    while ((1+rlist.iBottom - rlist.iTop)>=(lineHeight+1)) {
        RegionSplitFromTop(&rlist, &rline, lineHeight, 1);
    }
    */

    // Now add the back button
    RegionShrink(&rbottom, 1);
    p->iLeft = rbottom.iLeft;
    p->iRight = rbottom.iLeft+EXIT_BUTTON_WIDTH-1;
    p->iTop = rbottom.iTop;
    p->iBottom = rbottom.iTop+EXIT_BUTTON_HEIGHT-1;
    p->iText = "Exit";
    p->iAction = VideoPlayerSelectExit;
    p->iIcon = G_exitIcon;
    p->iData = 0;
    p->iDraw = 0; // Use default
    p++;

	// Now add the up button (but only if needed)
    if (count < G_ws->iNumVideos) {
        // Need to be able to scroll
        RegionShrink(&rbottom, 1);
        p->iLeft = G_ws->iChoiceBox.iRight-33;
        p->iRight = G_ws->iChoiceBox.iRight-1;
        p->iTop = G_ws->iChoiceBox.iTop+1;
        p->iBottom = G_ws->iChoiceBox.iTop+33;
        p->iText = "";
        p->iAction = VideoPlayerSelectScrollUp;
        p->iIcon = G_arrowUp;
        p->iData = 0;
        p->iDraw = 0;
        p++;
    }

	// Now add the down button
    if (count < G_ws->iNumVideos) {
        RegionShrink(&rbottom, 1);
        p->iLeft = G_ws->iChoiceBox.iRight-33;
        p->iRight = G_ws->iChoiceBox.iRight-1;
        p->iTop = G_ws->iChoiceBox.iBottom-33;
        p->iBottom = G_ws->iChoiceBox.iBottom-1;
        p->iText = "";
        p->iAction = VideoPlayerSelectScrollDown;
        p->iIcon = G_arrowDown;
        p->iData = 0;
        p->iDraw = 0;
        p++;
    }

    // Next entry is the end of choices marker
    p->iText = 0; // end of list marker*/
}

static void VideoPlayerSelectDraw(void)
{
    static const char *noFiles = "< No audio files found >";
#if(UEZ_PROCESSOR != NXP_LPC4357)
	static const char *noDrive = "< No SD card found >";
#else
	static const char *noDrive = "< No drive found >";
#endif
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

    if (G_ws->iNumVideos==0) {
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

/*---------------------------------------------------------------------------*
 * Routine:  MultiVideoPlayerSelectScreen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the video player screen before it loads the pages.
 *---------------------------------------------------------------------------*/
static void VideoPlayerSelectScreen(T_uezDevice lcd)
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
    swim_set_title(&G_win, "uEZ(tm) Video Player", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_font(&G_win, &font_winfreesys14x16);

    SUIShowPage0();
}


T_uezError AudioPlayerSelectionLoad(TUInt32 aDrive)
{
    T_uezINISession ini;
    char key[10];
    char section[64];
    char filename[40];
    char type[20];
    TUInt8 i=0;
    T_uezError error = UEZ_ERROR_NOT_READY;

    G_topFileIndex = 0;
    G_ws->iNumVideos = 0;

    sprintf(filename, "%d:/VIDEOS.INI" , aDrive);
    if(UEZINIOpen(filename, &ini) == UEZ_ERROR_NONE){

        for(i=0; i<MAX_VIDEOS; i++) {
            error = UEZINIGotoSection(ini, "Videos");

            if(!error) {
                sprintf(key, "video%d", i+1);
                if(UEZINIGetString(ini, key, "", section, 63) != UEZ_ERROR_NONE)
                    break; // Reached the end of the video list
            }

            if(!error)
                error = UEZINIGotoSection(ini, section);

            if(!error)
                error = UEZINIGetString(ini, "title", "", G_ws->iVideos[i].iTitle, 63);

            if(!error) {
                error = UEZINIGetString(ini, "type", "", type, 19);

                if(strcmp("BIN",type) == 0) {
                    G_ws->iVideos[i].iType = VIDEO_TYPE_BIN;
                } else {
                    G_ws->iVideos[i].iType = VIDEO_TYPE_FRAMES;
                }
            }

            if(!error) {
                if(UEZ_LCD_DEFAULT_COLOR_DEPTH == UEZLCD_COLOR_DEPTH_I15_BIT)
                    error = UEZINIGetString(ini, "video555", "", G_ws->iVideos[i].iVideoPath, 127);
                else
                    error = UEZINIGetString(ini, "video565", "", G_ws->iVideos[i].iVideoPath, 127);
            }

            if(!error)
                error = UEZINIGetString(ini, "audio", "", G_ws->iVideos[i].iAudioPath, 127);

            if(!error)
                error = UEZINIGetInteger32(ini, "fps", 10, (TInt32*)&G_ws->iVideos[i].iFPS);

            if(!error)
                error = UEZINIGetInteger32(ini, "height", 10, (TInt32*)&G_ws->iVideos[i].iVideoHeight);

            if(!error)
                error = UEZINIGetInteger32(ini, "width", 10, (TInt32*)&G_ws->iVideos[i].iVideoWidth);

            // Check for text lines. Ignore error.
            UEZINIGetString(ini, "TextLine1", "", G_ws->iVideos[i].iTextLine1, 127);
            UEZINIGetString(ini, "TextLine2", "", G_ws->iVideos[i].iTextLine2, 127);

            if(error != UEZ_ERROR_NONE)
                break;

            G_ws->iNumVideos++;
        }
        error = UEZ_ERROR_NONE;
        UEZINIClose(ini);
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  VideoPlayerSelect
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the video player, drawing the video player load screen.  Load
 *      images onto the screen and then go into video player scroll mode or
 *      report an error.
 * Inputs:
 *      const T_choice *aChoice     -- Choice used to get here.
 *---------------------------------------------------------------------------*/
void VideoPlayerSelection(const T_choice *aChoice)
{
    T_uezDevice lcd;
    T_uezQueue queue;
    T_uezDevice ts;
    T_uezInputEvent inputEvent;
#if ENABLE_UEZ_BUTTON
    T_uezDevice keypadDevice;
#endif

    G_ws = UEZMemAlloc(sizeof(T_VideoPlayerSelectWorkspace));
    if (!G_ws)
        return;
    memset(G_ws, 0, sizeof(*G_ws));

    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if ENABLE_UEZ_BUTTON
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
#if UEZ_REGISTER
        UEZQueueSetName(queue, "VideoSelection", "\0");
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                G_ws->iLCD = lcd;

                G_ws->iDriveError = AudioPlayerSelectionLoad(1);
#if (UEZ_ENABLE_USB_HOST_STACK && (UEZ_USB_HOST_DRIVE_NUMBER == 0) && UEZ_PROCESSOR == NXP_LPC4357)
                if(G_ws->iDriveError != UEZ_ERROR_NONE){
                    G_ws->iDriveError = AudioPlayerSelectionLoad(0);
                }
#endif
                VideoPlayerSelectScreen(lcd);
                VideoPlayerSelectChoices();
                VideoPlayerSelectDraw();

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
#if ENABLE_UEZ_BUTTON
        UEZKeypadClose(keypadDevice, &queue);
#endif
        UEZQueueDelete(queue);
    }

    UEZMemFree(G_ws);
}

/*-------------------------------------------------------------------------*
 * End of File:  MultiVideoPlayerSelect.c
 *-------------------------------------------------------------------------*/
