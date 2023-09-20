/*-------------------------------------------------------------------------*
 * File:  MultiSlideshowMode.c
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
#include "uEZDemoCommon.h"
#include <uEZMemory.h>
#include "Slideshow.h"
#include "UDCImages.h"
#include <uEZINI.h>
#include <uEZLCD.h>
#ifndef UEZ_SLIDESHOW_NAME
    #define UEZ_SLIDESHOW_NAME      ""
#endif
#include <uEZKeypad.h>

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#define MAX_CHOICES       10

typedef struct {
    SWIM_WINDOW_T iWin;
    T_slideshowList iSlideshowList;
    T_choice iChoices[MAX_CHOICES+1];
    T_region iTop;
    T_region iChoiceBox;
    TBool iExit;
    T_uezDevice iLCD;
    TUInt32 iDrive;
    T_uezError iDriveError;
} T_MSMWorkspace;

static T_MSMWorkspace *G_ws;
#define G_win           (G_ws->iWin)

static void MSMSetupChoices(void);
static void MSMDraw(void);
static void MultiSlideshowScreen(T_uezDevice lcd);

static void SlideshowAction(const T_choice *p_choice)
{
    SingleSlideshowMode((T_slideshowDefinition *)p_choice->iData);
    MultiSlideshowScreen(G_ws->iLCD);
    MSMSetupChoices();
    MSMDraw();
}

static void SlideshowOptionDraw(const T_choice *p_choice)
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
    RegionSplitFromLeft(&r, &icon, SLIDESHOW_ICON_WIDTH, 2);
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

static void SlideshowExit(const T_choice *aChoice)
{
    PARAM_NOT_USED(aChoice);
    G_ws->iExit = ETrue;
}

static void MSMSetupChoices(void)
{
    T_region r;
    T_region rlist;
    T_region rline;
    T_region rbottom;
    TUInt16 fontHeight;
    TUInt16 lineHeight;
    int i;
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
    if (lineHeight < SLIDESHOW_ICON_HEIGHT+4)
        lineHeight = SLIDESHOW_ICON_HEIGHT+4;

    for (i=0; i<MAX_CHOICES-1; i++, p++) {
        if (i>=G_ws->iSlideshowList.iCount)
            break;

        // Stop if there is not enough room for another line
        if ((1+rlist.iBottom - rlist.iTop)<(lineHeight+1))
            break;

        // Pull off some space
        RegionSplitFromTop(&rlist, &rline, lineHeight, 1);

        // Setup new choice
        p->iLeft = rline.iLeft;
        p->iTop = rline.iTop;
        p->iRight = rline.iRight;
        p->iBottom = rline.iBottom;
        p->iText = G_ws->iSlideshowList.iList[i].iName;
        p->iAction = SlideshowAction;
        p->iIcon = G_slideshowSmallIcon;
        p->iData = &G_ws->iSlideshowList.iList[i];
        p->iDraw = SlideshowOptionDraw;
        G_ws->iChoiceBox.iBottom = rline.iBottom+2;
    }

    // Now add the back button
    RegionShrink(&rbottom, 1);
    p->iLeft = rbottom.iLeft;
    p->iRight = rbottom.iLeft+EXIT_BUTTON_WIDTH-1;
    p->iTop = rbottom.iTop;
    p->iBottom = rbottom.iTop+EXIT_BUTTON_HEIGHT-1;
    p->iText = "Exit";
    p->iAction = SlideshowExit;
    p->iIcon = G_exitIcon;
    p->iData = 0;
    p->iDraw = 0; // Use default

    // Next entry is the end of choices marker
    p++;
    p->iText = 0; // end of list marker
}

static void MSMDraw(void)
{
    static const char *noFiles = "< No SLIDES.TXT file found >";
    static const char *noDrive = "< No Drive Found >";
    static const char *noListINI = "< Show not found in SLIDES.INI file >";
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

    if (G_ws->iSlideshowList.iCount==0) {
        p_msg = noDrive;
        if (G_ws->iDriveError == UEZ_ERROR_READ_WRITE_ERROR)
            p_msg = noDrive;
        else if (G_ws->iDriveError == UEZ_ERROR_NOT_FOUND)
            p_msg = noFiles;
        else if (G_ws->iDriveError == UEZ_ERROR_EMPTY)
            p_msg = noListINI;

        width = swim_get_text_line_width(&G_win, p_msg);
        RegionCenterLeftRight(&G_ws->iChoiceBox, &r1, width);
        RegionCenterTopBottom(&r1, &r2, fontHeight);
        swim_put_text_xy(&G_win, p_msg, r2.iLeft, r2.iTop);
    }

    ChoicesDraw(&G_win, G_ws->iChoices);

    SUIShowPage0();
}

/*---------------------------------------------------------------------------*
 * Routine:  MultiSlideshowScreen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the slideshow screen before it loads the pages.
 *---------------------------------------------------------------------------*/
static void MultiSlideshowScreen(T_uezDevice lcd)
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
    swim_set_title(&G_win, "uEZ(tm) MultiSlideshow Demonstration", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_font(&G_win, &font_winfreesys14x16);

    SUIShowPage0();
}

/*---------------------------------------------------------------------------*
 * Routine:  OldSlideshowCheckAvailable
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if a slideshow is available on the given drive number
 *      by looking for slide #1.
 * Inputs:
 *      TUInt32 aDrive              -- Drive index to check
 * Outputs:
 *      T_uezError                  -- Error from reading the drive
 *                        UEZ_ERROR_NOT_FOUND - media exists, but no file
 *                        UEZ_ERROR_NOT_READY - no media found
 *                        UEZ_ERROR_NONE      - no errors, found slideshow
 *---------------------------------------------------------------------------*/
T_uezError OldSlideshowCheckAvailable(TUInt32 aDrive)
{
    char filename[60]; // allow up to 20 extra char for filename on top of directory path.
    char directory[40];
    T_uezError error;
    T_uezFile file;

    // Look for slide directory
    sprintf(directory, "%d:/", aDrive);

    // Look for the standard name
    sprintf(filename, SLIDESHOW_PICTURE_NAMING, directory, 1);
    error=UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file);
    if (error == UEZ_ERROR_NONE)
        UEZFileClose(file);

#if SLIDESHOW_BACKWARDS_COMPATIBLE
    // Try again with the old backward compatible naming
    sprintf(filename, SLIDESHOW_BACKWARD_NAMING, directory, 1);
    error=UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file);
    if (error == UEZ_ERROR_NONE)
        UEZFileClose(file);
#endif
    // Return the resulting error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  MultiSlideshowCheckAvailable
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if a slideshow is available on the given drive number
 *      by looking for slide #1.
 * Inputs:
 *      TUInt32 aDrive              -- Drive index to check
 * Outputs:
 *      T_uezError                  -- Error from reading the drive
 *                        UEZ_ERROR_NOT_FOUND - media exists, but no file
 *                        UEZ_ERROR_NOT_READY - no media found
 *                        UEZ_ERROR_NONE      - no errors, found slideshow
 *---------------------------------------------------------------------------*/
T_uezError MultiSlideshowCheckAvailable(TUInt32 aDrive)
{
    char filename[40];
    T_uezError error;
    T_uezFile file;

    // Look for slide directory
    sprintf(filename, "%d:/SLIDES/SLIDES.TXT" , aDrive);
    error=UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file);
    if (error == UEZ_ERROR_NONE)
        UEZFileClose(file);

    // Return the resulting error
    return error;
}

T_uezError MultiSlideshowCheckAvailableINI(TUInt32 aDrive)
{
    char filename[40];
    T_uezError error;
    T_uezFile file;

    // Look for slide directory
    sprintf(filename, "%d:/SLIDES/SLIDES.INI" , aDrive);
    error=UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file);
    if (error == UEZ_ERROR_NONE)
        UEZFileClose(file);

    // Return the resulting error
    return error;
}

T_uezError IMSLoadDirectoryINI(TUInt32 aDrive)
{
    T_uezINISession ini;
    TInt32 numShows = 0;
    char filename[40];
    char title[9];
    char dir[7];
    TUInt8 i;
    T_slideshowDefinition *p_list = G_ws->iSlideshowList.iList;

    sprintf(filename, "%d:/SLIDES/SLIDES.INI" , aDrive);
    if(UEZINIOpen(filename, &ini) == UEZ_ERROR_NONE){
        if (UEZINIGotoSection(ini, UEZ_SLIDESHOW_NAME) == UEZ_ERROR_NONE){
          UEZINIGetInteger32(ini, "numShows", 0, &numShows);
          if(!numShows){
              UEZINIGotoSection(ini, "default");
              UEZINIGetInteger32(ini, "numShows", 0, &numShows);
          }
        }
        if(numShows){
            for( i = 1; i <= numShows; i++){
                sprintf(title, "Title%d", i);
                if(UEZINIGetString(ini, title, "",p_list->iName, MAX_SLIDESHOW_DEF_NAME_LEN) == UEZ_ERROR_NONE){
                    sprintf(dir, "Dir%d", i);
                    if(UEZINIGetString(ini, dir, "",p_list->iDirectory, MAX_SLIDESHOW_DEF_DIR_LEN) == UEZ_ERROR_NONE){
                        p_list->iDrive = aDrive;
                        p_list++;
                        G_ws->iSlideshowList.iCount++;
                    }
                }
                else
                   return UEZ_ERROR_EMPTY;
            }
        }
        else{
            UEZINIClose(ini);
            return UEZ_ERROR_EMPTY;
        }
        return UEZINIClose(ini);
    }
    return UEZ_ERROR_EMPTY;
}

T_uezError IMSLoadDirectory(TUInt32 aDrive)
{
    char filename[40];
    T_uezError error;
    T_uezFile file;
    TUInt32 len;
    TUInt32 count;
    TUInt32 numRead;
    int mode;
    TUInt32 index;
    T_slideshowDefinition *p_list = G_ws->iSlideshowList.iList;
    char c;

    // Look for slide directory
    sprintf(filename, "%d:/SLIDES/SLIDES.TXT" , aDrive);
    error=UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file);
    if (error == UEZ_ERROR_NONE) {
        UEZFileGetLength(file, &len);
        // Parse the file
        mode = 0;
        index = 0;
        for (count=0; count<len; count++) {
            error = UEZFileRead(file, &c, 1, &numRead);
            if ((error) || (numRead==0))
                break;
            // Skip carriage returns
            if (c == '\r')
                continue;
            switch (mode) {
                case 0:
                    // Pull out the first name
                    if (c == ',') {
                        mode = 1;
                        p_list->iName[index] = '\0';
                        index = 0;
                    } else if (index < MAX_SLIDESHOW_DEF_NAME_LEN) {
                        p_list->iName[index++] = c;
                    }
                    break;
                case 1:
                    if (c == '\n') {
                        mode = 0;
                        p_list->iDirectory[index] = '\0';
                        p_list->iDrive = aDrive;
                        p_list++;
                        index = 0;
                        G_ws->iSlideshowList.iCount++;
                        if (G_ws->iSlideshowList.iCount == MAX_NUMBER_OF_SLIDESHOWS)
                            break;
                    } else if (index < MAX_SLIDESHOW_DEF_DIR_LEN) {
                        p_list->iDirectory[index++] = c;
                    }
                    break;
            }
        }
        UEZFileClose(file);
    }

    // Return the resulting error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  MultiSlideshowMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the slideshow, drawing the slideshow load screen.  Load
 *      images onto the screen and then go into slideshow scroll mode or
 *      report an error.
 * Inputs:
 *      const T_choice *aChoice     -- Choice used to get here.
 *---------------------------------------------------------------------------*/
void MultiSlideshowMode(const T_choice *aChoice)
{
    T_uezDevice lcd;
//    T_uezError error;
//    T_uezFile file;
    TUInt32 drive;
    T_uezError driveError[2];
    T_uezQueue queue;
    T_uezDevice ts;
    T_uezInputEvent inputEvent;
#if UEZ_ENABLE_BUTTON_BOARD
    T_uezDevice keypadDevice;
#endif
    G_ws = UEZMemAlloc(sizeof(T_MSMWorkspace));
    if (!G_ws)
        return;
    memset(G_ws, 0, sizeof(*G_ws));

    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_ENABLE_BUTTON_BOARD
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
#if UEZ_REGISTER
        UEZQueueSetName(queue, "MultiSlideShow", "\0");
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                G_ws->iLCD = lcd;
                Storage_PrintInfo('0', ETrue);

                // Is there old slideshows available?
                driveError[SLIDESHOW_FLASH_DRIVE] = OldSlideshowCheckAvailable(SLIDESHOW_FLASH_DRIVE);
                driveError[SLIDESHOW_SDCARD_DRIVE] = OldSlideshowCheckAvailable(SLIDESHOW_SDCARD_DRIVE);
                if (driveError[SLIDESHOW_FLASH_DRIVE] == UEZ_ERROR_NONE)  {
                    drive = SLIDESHOW_FLASH_DRIVE;
                } else if (driveError[SLIDESHOW_SDCARD_DRIVE] == UEZ_ERROR_NONE) {
                    drive = SLIDESHOW_SDCARD_DRIVE;
                } else {
                    drive = SLIDESHOW_DRIVE_NONE;
                }

                if (drive == SLIDESHOW_DRIVE_NONE)  {
                    // MultiSlideshow is not loaded, load the slideshow list
                    // Determine on which drives the slideshow is located

                    //check for INI File on both drives
                    driveError[SLIDESHOW_FLASH_DRIVE] = MultiSlideshowCheckAvailableINI(SLIDESHOW_FLASH_DRIVE);
                    driveError[SLIDESHOW_SDCARD_DRIVE] = MultiSlideshowCheckAvailableINI(SLIDESHOW_SDCARD_DRIVE);
                    //if INI
                    if(!driveError[SLIDESHOW_FLASH_DRIVE] || !driveError[SLIDESHOW_SDCARD_DRIVE])
                    {
                        if ((driveError[SLIDESHOW_FLASH_DRIVE] == UEZ_ERROR_NONE) || (driveError[SLIDESHOW_SDCARD_DRIVE] == UEZ_ERROR_NOT_READY))
                            drive = SLIDESHOW_FLASH_DRIVE;
                        else
                            drive = SLIDESHOW_SDCARD_DRIVE;
                        G_ws->iDrive = drive;

                        // Load the directory if the drive is active
                        G_ws->iSlideshowList.iCount = 0;
                        if (driveError[drive] == UEZ_ERROR_NONE)
                            driveError[drive] = IMSLoadDirectoryINI(drive);
                        G_ws->iDriveError = driveError[drive];
                    }
                    else
                    {
                        driveError[SLIDESHOW_FLASH_DRIVE] = MultiSlideshowCheckAvailable(SLIDESHOW_FLASH_DRIVE);
                        driveError[SLIDESHOW_SDCARD_DRIVE] = MultiSlideshowCheckAvailable(SLIDESHOW_SDCARD_DRIVE);

                        // Choose the flash drive if available, otherwise use SDCard
                        if ((driveError[SLIDESHOW_FLASH_DRIVE] == UEZ_ERROR_NONE) || (driveError[SLIDESHOW_SDCARD_DRIVE] == UEZ_ERROR_NOT_READY))
                            drive = SLIDESHOW_FLASH_DRIVE;
                        else
                            drive = SLIDESHOW_SDCARD_DRIVE;
                        G_ws->iDrive = drive;

                        // Load the directory if the drive is active
                        G_ws->iSlideshowList.iCount = 0;
                        if (driveError[drive] == UEZ_ERROR_NONE)
                            driveError[drive] = IMSLoadDirectory(drive);
                        G_ws->iDriveError = driveError[drive];
                    }

                    // Put the screen (with options)
                    MultiSlideshowScreen(lcd);
                    MSMSetupChoices();
                    MSMDraw();

                    // Sit here in a loop until we are done
                    while (!G_ws->iExit) {
                      //Wait till we get a touchscreen event
                      if (UEZQueueReceive(queue, &inputEvent, UEZ_TIMEOUT_INFINITE)==UEZ_ERROR_NONE) {
                        ChoicesUpdate(&G_win, G_ws->iChoices, queue, 500);
                      }
                    }
                } else {
                    // Got an old slideshow to show
                    T_slideshowDefinition def;
                    def.iDrive = drive;
                    def.iName[0] = '\0';
                    def.iDirectory[0] = '\0';
                    SingleSlideshowMode(&def);
                }
                UEZLCDClose(lcd);
            }
            UEZTSClose(ts, queue);
        }
#if UEZ_ENABLE_BUTTON_BOARD
        UEZKeypadClose(keypadDevice, &queue);
#endif
        UEZQueueDelete(queue);
    }
    UEZMemFree(G_ws);
    G_ws = 0;
}

/*-------------------------------------------------------------------------*
 * End of File:  MultiSlideshowMode.c
 *-------------------------------------------------------------------------*/
