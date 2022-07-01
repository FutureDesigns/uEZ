/*-------------------------------------------------------------------------*
 * File:  SingleSlideshowMode.c
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
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <uEZMemory.h>
#include "uEZDemoCommon.h"
#include <uEZWAVFile.h>
#include "SingleSlideshowMode.h"
#include <uEZLCD.h>
#include <uEZKeypad.h>
#include <UEZPlatform.h>
/*---------------------------------------------------------------------------*
 * Defines:
 *---------------------------------------------------------------------------*/
#if (UEZ_DEFAULT_LCD_RES_VGA||UEZ_DEFAULT_LCD_RES_WVGA)
#define SLIDE_ICON_HEIGHT                64
#define SLIDE_ICON_WIDTH                 64
#define SLIDE_ICON_PADDING               40
#define SCROLL_SENSITIVITY               128
#define HOURGLASS_WIDTH                  72
#define HOURGLASS_HEIGHT                 84
#else
#define SLIDE_ICON_HEIGHT                32
#define SLIDE_ICON_WIDTH                 32
#define SLIDE_ICON_PADDING               20
#define SCROLL_SENSITIVITY               64
#define HOURGLASS_WIDTH                  36
#define HOURGLASS_HEIGHT                 42
#endif

#define PANEL_PADDING               5
#define PANEL_TOP_EDGE_HEIGHT       (PANEL_PADDING*2+SLIDE_ICON_HEIGHT)
#define PANEL_BOTTOM_EDGE_HEIGHT    (PANEL_PADDING*2+SLIDE_ICON_HEIGHT)
#ifndef SLIDESHOW_PANEL_DISAPPEAR_TIME
#define SLIDESHOW_PANEL_DISAPPEAR_TIME        4000
#endif
#ifndef SLIDESHOW_PANEL_AUTO_NEXT_SLIDE_TIME
#define SLIDESHOW_PANEL_AUTO_NEXT_SLIDE_TIME  3000
#endif
#ifndef SLIDESHOW_PREFETCH_AHEAD
#define SLIDESHOW_PREFETCH_AHEAD           1
#endif
#ifndef SLIDESHOW_PREFETCH_BEHIND
#define SLIDESHOW_PREFETCH_BEHIND          1
#endif
#ifndef SLIDESHOW_NUM_CACHED_SLIDES
#define SLIDESHOW_NUM_CACHED_SLIDES         5
#endif
#ifndef SLIDESHOW_EVENT_DRIVEN_CONTROL
#define SLIDESHOW_EVENT_DRIVEN_CONTROL      1
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#define MAX_CHOICES       10
#define CACHED_SLIDE_FIRST_FRAME    3

typedef enum {
    CACHE_ENTRY_STATE_FREE = 0,
    CACHE_ENTRY_STATE_LOADING = 1,
    CACHE_ENTRY_STATE_READY = 2
} T_cacheEntryState;

typedef struct {
    T_cacheEntryState iState;
    TUInt32 iSlideNumber;
} T_cacheEntry;

typedef struct {
    SWIM_WINDOW_T iWin;
    T_slideshowDefinition *iDef;
    T_choice iChoices[MAX_CHOICES + 1];
    TUInt32 iNumSlides;
    TUInt32 iSlideNum;
    T_cacheEntry iCache[SLIDESHOW_NUM_CACHED_SLIDES];
    T_uezQueue iSlideRequest;
    T_uezQueue iSlideReady;
    T_uezTask iLoadTask;
    TUInt32 iDrawFrame;
    T_region iRSlideText;
    T_region iBottom;
    T_region iMiddle;
    TInt32 iTouchStartY;
    TInt32 iTouchEndY;
    TBool iExit;
    TBool iShowPanel;
    TBool iNeedDraw;
    TBool iNeedLoad;
    TBool iShowHourglass;
    TBool iTouched;
    TBool iAbortFlag;
    TBool iAutoSlideshow;
    TBool iAnimateUp;
    TBool iAnimateDown;
    TBool iOldFormat;
} T_SSMWorkspace;

typedef struct {
    TUInt32 iSlideNum;
    TInt32 iFrame; // -1 means this slide was not loaded
} T_slideLoadResponse;

//static T_slideshowList *G_slideshowList;
static T_SSMWorkspace *G_ws = 0;
#define G_win           (G_ws->iWin)
#define G_slideshowList (&G_ws->iSlideshowList)

#if SLIDESHOW_EVENT_DRIVEN_CONTROL
static T_uezQueue G_slideshowEventQueue = 0;
#endif

static TUInt32 ICacheEntryToFrame(TUInt32 aCacheEntry);
static TUInt32 ICacheEntryFind(TUInt32 aSlideNumber);
static void SSMDrawSlide(TBool doAnimate);

static void SSMExit(const T_choice *aChoice)
{
    PARAM_NOT_USED(aChoice);
    G_ws->iExit = ETrue;
    // Also abort all loads
    G_ws->iAbortFlag = ETrue;
    G_ws->iShowPanel = EFalse;
    G_ws->iNeedDraw = ETrue;
    G_ws->iShowHourglass = EFalse;

#if UEZ_ENABLE_I2S_AUDIO
     if(UEZWAVGetStatus()){
        UEZStopWAV();
    }
    UEZWAVCleanUp();
#endif
}

static void SSMNext(const T_choice *aChoice)
{
    PARAM_NOT_USED(aChoice);

    // Hide panel first
    if (G_ws->iShowPanel) {
        G_ws->iShowPanel = EFalse;
        //G_ws->iShowHourglass = ETrue;
        SSMDrawSlide(EFalse);
    }

    G_ws->iSlideNum++;
    if (G_ws->iSlideNum >= G_ws->iNumSlides)
        G_ws->iSlideNum = 0;
    G_ws->iNeedLoad = ETrue;
    G_ws->iAnimateDown = ETrue;
#if UEZ_ENABLE_I2S_AUDIO
    if(UEZWAVGetStatus()){
        UEZStopWAV();
    }
#endif
}

static void SSMPrevious(const T_choice *aChoice)
{
    PARAM_NOT_USED(aChoice);

    // Hide panel first
    if (G_ws->iShowPanel) {
        G_ws->iShowPanel = EFalse;
        //G_ws->iShowHourglass = ETrue;
        SSMDrawSlide(EFalse);
    }

    if (G_ws->iSlideNum == 0)
        G_ws->iSlideNum = G_ws->iNumSlides;
    G_ws->iSlideNum--;
    G_ws->iShowPanel = EFalse;
    G_ws->iNeedLoad = ETrue;
    G_ws->iAnimateUp = ETrue;
#if UEZ_ENABLE_I2S_AUDIO
     if(UEZWAVGetStatus()){
        UEZStopWAV();
    }
#endif
}

static void SSMAutoOff(const T_choice *aChoice);
static void SSMAutoOn(const T_choice *aChoice);

static void SSMAutoOff(const T_choice *aChoice)
{
    T_choice *p = (T_choice *)aChoice;

    G_ws->iAutoSlideshow = EFalse;
    G_ws->iNeedDraw = ETrue;
    p->iIcon = G_playButtonIcon;
    p->iAction = SSMAutoOn;
}

static void SSMAutoOn(const T_choice *aChoice)
{
    T_choice *p = (T_choice *)aChoice;

    G_ws->iAutoSlideshow = ETrue;
    G_ws->iNeedDraw = ETrue;
    p->iIcon = G_pauseButtonIcon;
    p->iAction = SSMAutoOff;

    // Immediately go to the next slide (we're ready to go on)
#if UEZ_ENABLE_I2S_AUDIO
     if(!UEZWAVGetStatus())
#endif
     SSMNext(0);
}

static void SSMDrawTransparentIcon(const T_choice *aChoice)
{
    // First, load the background into the buffer
    const TUInt8 *p_palette;
    TUInt32 x, y;
    const TUInt8 *p_pixels;
    TUInt32 width;
    TUInt32 height;
    T_pixelColor rgbArray[256];
    TUInt32 i;
    TUInt32 aXOffset = aChoice->iLeft;
    TUInt32 aYOffset = aChoice->iTop;
    const TUInt8 *p = aChoice->iIcon;
    const TUInt8 *rgb;

    width = *((TUInt16 *)(p + 12));
    height = *((TUInt16 *)(p + 14));

    p_palette = p + 18;
    p_pixels = p + 18 + 256 * 3;

    // Convert the palette to native pixels
    rgb = p_palette;
    for (i = 0; i < 256; i++, rgb += 3)
        rgbArray[i] = RGB(rgb[2], rgb[1], rgb[0]);

    for (y = 0; y < height; y++) {
        const TUInt8 *p_raster = &p_pixels[(height - 1 - y) * width];
        for (x = 0; x < width; x++) {
            T_pixelColor color = rgbArray[*(p_raster++)];
            if (color != 0) {
#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
                // Icons in VGA are drawn double sized.  Draw this
                // one by scaling 2:1 with larger pixels
                swim_driver_put_pixel(&G_win, x*2+aXOffset+0, y*2+aYOffset+0, color);
                swim_driver_put_pixel(&G_win, x*2+aXOffset+1, y*2+aYOffset+0, color);
                swim_driver_put_pixel(&G_win, x*2+aXOffset+0, y*2+aYOffset+1, color);
                swim_driver_put_pixel(&G_win, x*2+aXOffset+1, y*2+aYOffset+1, color);
#else
                swim_driver_put_pixel(&G_win, x + aXOffset, y + aYOffset, color);
#endif
            } else {
            }
        }
    }
}

static void SSMSetupChoices(void)
{
    T_choice *p = G_ws->iChoices;
    T_region r;
    T_region rsub;
    T_region ricon;

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);

    //    RegionSplitFromTop(&r, &G_ws->iTop, PANEL_TOP_EDGE_HEIGHT, 0);
    RegionSplitFromBottom(&r, &G_ws->iBottom, PANEL_BOTTOM_EDGE_HEIGHT, 0);
    G_ws->iMiddle = r;

    rsub = G_ws->iBottom;
    RegionShrink(&rsub, PANEL_PADDING);
    RegionSplitFromLeft(&rsub, &ricon, SLIDE_ICON_WIDTH, SLIDE_ICON_PADDING);
    p->iLeft = ricon.iLeft;
    p->iTop = ricon.iTop;
    p->iRight = ricon.iRight;
    p->iBottom = ricon.iBottom;
    p->iText = "";
    p->iAction = SSMPrevious;
    p->iIcon = G_rewindButtonIcon;
    p->iData = 0;
    p->iDraw = SSMDrawTransparentIcon;
    p++;

    RegionSplitFromLeft(&rsub, &ricon, SLIDE_ICON_WIDTH, SLIDE_ICON_PADDING);
    p->iLeft = ricon.iLeft;
    p->iTop = ricon.iTop;
    p->iRight = ricon.iRight;
    p->iBottom = ricon.iBottom;
    p->iText = "";
    p->iAction = SSMAutoOn;
    p->iIcon = G_playButtonIcon;
    p->iData = 0;
    p->iDraw = SSMDrawTransparentIcon;
    p++;

    RegionSplitFromLeft(&rsub, &ricon, SLIDE_ICON_WIDTH, SLIDE_ICON_PADDING);
    p->iLeft = ricon.iLeft;
    p->iTop = ricon.iTop;
    p->iRight = ricon.iRight;
    p->iBottom = ricon.iBottom;
    p->iText = "";
    p->iAction = SSMNext;
    p->iIcon = G_forwardButtonIcon;
    p->iData = 0;
    p->iDraw = SSMDrawTransparentIcon;
    p++;

    RegionSplitFromRight(&rsub, &G_ws->iRSlideText, 100, 2);

    RegionSplitFromRight(&rsub, &ricon, SLIDE_ICON_WIDTH, SLIDE_ICON_PADDING);
    p->iLeft = ricon.iLeft;
    p->iTop = ricon.iTop;
    p->iRight = ricon.iRight;
    p->iBottom = ricon.iBottom;
    p->iText = "";
    p->iAction = SSMExit;
    p->iIcon = G_stopButtonIcon;
    p->iData = 0;
    p->iDraw = SSMDrawTransparentIcon;
    p++;

    // No choices currently
    p->iText = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  SingleSlideshowScreen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the slideshow screen before it loads the pages.
 *---------------------------------------------------------------------------*/
void SingleSlideshowScreen(T_uezDevice lcd)
{
    T_pixelColor *pixels;
    /*
     const char *p_loading = "Loading ...";
     TUInt32 lineWidth;
     T_region r;
     T_region rcenter;
     */

    SUIHidePage0();

    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    swim_window_open(&G_win, DISPLAY_WIDTH, DISPLAY_HEIGHT, pixels, 0, 0,
            DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, 2, YELLOW, RGB(0, 0, 0), RED);
    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    //    swim_set_title(&G_win, "uEZ(tm) MultiSlideshow Demonstration", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_font(&G_win, &font_winfreesys14x16);

    /*
     lineWidth = swim_get_text_line_width(&G_win, p_loading);

     r.iLeft = 0;
     r.iTop = 0;
     r.iRight = G_win.xvsize;
     r.iBottom = G_win.yvsize;
     RegionCenter(&r, &rcenter, swim_get_font_height(&G_win), lineWidth);
     swim_put_text_xy(&G_win, p_loading, rcenter.iLeft, rcenter.iTop);
     */

    SUIShowPage0();
}

void SSMDetermineNumSlides(void)
{
    char filename[80];
    char directory[80];
    T_uezFile file;

    // Load the slides
    G_ws->iNumSlides = 0;
    while (1) {
        if (G_ws->iOldFormat)
            directory[0] = '\0';
        else
            sprintf(directory, SLIDES_DIRECTORY, G_ws->iDef->iDirectory);
        sprintf(filename, "%d:" SLIDESHOW_PICTURE_NAMING, G_ws->iDef->iDrive,
                directory, G_ws->iNumSlides + 1);
        if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) != UEZ_ERROR_NONE) {
#if SLIDESHOW_BACKWARDS_COMPATIBLE
            directory[0] = '\0';
            // If in compatible mode, try the old name if the new name is not found
            sprintf(filename, "%d:" SLIDESHOW_BACKWARD_NAMING,
                    G_ws->iDef->iDrive, directory, G_ws->iNumSlides + 1);
            if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file)
                    != UEZ_ERROR_NONE) {
                // Could not open file, break
                break;
            }
#else
            // Could not open file, break
            break;
#endif
        }
        // File could be opened, it exists.  Add it to the count
        G_ws->iNumSlides++;
        UEZFileClose(file);
    }
}

static T_uezError IDoLoad(TUInt32 aSlideNum, TUInt32 aFrame, TBool *aAbortFlag)
{
    char filename[80];
    char directory[80];
    T_uezFile file;

    if (G_ws->iAbortFlag)
        return UEZ_ERROR_CANCELLED;

    if (G_ws->iOldFormat)
        directory[0] = '\0';
    else
        sprintf(directory, SLIDES_DIRECTORY, G_ws->iDef->iDirectory);
    sprintf(filename, "%d:" SLIDESHOW_PICTURE_NAMING, G_ws->iDef->iDrive,
            directory, aSlideNum + 1);
    if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) != UEZ_ERROR_NONE) {
#if SLIDESHOW_BACKWARDS_COMPATIBLE
        // If in compatible mode, try the old name if the new name is not found
        sprintf(filename, "%d:" SLIDESHOW_BACKWARD_NAMING, G_ws->iDef->iDrive,
                G_ws->iDef->iDirectory, aSlideNum + 1);
        if (UEZFileOpen(filename, FILE_FLAG_READ_ONLY, &file) != UEZ_ERROR_NONE) {
            // Could not open file, break
            return UEZ_ERROR_NOT_FOUND;
        }
#else
        // Could not open file, break
        return UEZ_ERROR_NOT_FOUND;
#endif
    }
    // File could be opened, it exists.
    UEZFileClose(file);

    //printf("Load: %s (%d, %d)\n", filename, SLIDESHOW_PREFETCH_AHEAD, SLIDESHOW_PREFETCH_BEHIND);
    // Use the LoadPicture routine
    if (SUILoadPicture(filename, aFrame, aAbortFlag, ((TUInt8 *)LCD_FRAMES_END + 0x1)))
        return UEZ_ERROR_CANCELLED;

    return UEZ_ERROR_NONE;
}

void SSMLoadSlide(void)
{
    TUInt32 slideNum = G_ws->iSlideNum;
    T_slideLoadResponse resp;
    //TUInt32 entry;

    // Request the slide from the Load system (he'll check the cache for us)
    // Request this slide
    UEZQueueSendFront(G_ws->iSlideRequest, &G_ws->iSlideNum,
            UEZ_TIMEOUT_INFINITE);

    while (1) {
        if (UEZQueueReceive(G_ws->iSlideReady, &resp, 250) == UEZ_ERROR_NONE) {
            if (resp.iSlideNum == slideNum) {
                // Do we have it?
                if (resp.iFrame == -1) {
                } else {
                    if (G_ws->iShowHourglass) {
                        G_ws->iShowHourglass = EFalse;
                        SSMDrawSlide(EFalse);
                    }

                    G_ws->iNeedLoad = EFalse;
                    G_ws->iNeedDraw = ETrue;
                    G_ws->iDrawFrame = resp.iFrame;
                    G_ws->iShowHourglass = EFalse;
                }
                break;
            }
        } else {
            // Slideshow was not ready immediately, put up an hour glass to
            // show the system is working.
            if (!G_ws->iShowHourglass) {
                G_ws->iShowHourglass = ETrue;
                SSMDrawSlide(EFalse);
            }
        }
    }
}

static void SSMDrawSlide(TBool aDoAnimate)
{
    T_region r;
    T_region rtext;
    char slideCount[40];
    TUInt32 lineWidth;

    // Show the current slide loaded
    SUIHidePage0();
    SUICopyFast32((TUInt32 *)FRAME(0), (TUInt32 *)FRAME(G_ws->iDrawFrame),
            FRAME_SIZE);

    // Now draw the gadgets if we have any
    if (G_ws->iShowPanel) {
        swim_set_fill_color(&G_win, BLACK);
        swim_set_pen_color(&G_win, BLACK);
        swim_set_fill_transparent(&G_win, FALSE);
        swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
        r = G_ws->iBottom;
        swim_set_fill_transparent(&G_win, TRUE);
        ChoicesDraw(&G_win, G_ws->iChoices);

        sprintf(slideCount, "Slide %d of %d", 1 + G_ws->iSlideNum,
                G_ws->iNumSlides);
        lineWidth = swim_get_text_line_width(&G_win, slideCount);

        r = G_ws->iRSlideText;
        RegionShrink(&r, 10);
        //        RegionSplitFromRight(&r, &rtext, lineWidth, 0);
        RegionCenterLeftRight(&r, &rtext, lineWidth);
        r = rtext;
        RegionCenterTopBottom(&r, &rtext, swim_get_font_height(&G_win));
        swim_set_pen_color(&G_win, YELLOW);
        swim_set_font_transparency(&G_win, 0);
        swim_put_text_xy(&G_win, slideCount, rtext.iLeft, rtext.iTop);
    }

    if (G_ws->iShowHourglass) {
        SUIDrawIcon(G_hourglassIcon, (DISPLAY_WIDTH - HOURGLASS_WIDTH) / 2,
                (DISPLAY_HEIGHT - HOURGLASS_HEIGHT) / 2);
    }

    if (aDoAnimate) {
        if (G_ws->iAnimateUp) {
            SUIShowPage0FancyUp();
            G_ws->iAnimateUp = EFalse;
        } else if (G_ws->iAnimateDown) {
            SUIShowPage0FancyDown(); // down
            G_ws->iAnimateDown = EFalse;
        } else {
            SUIShowPage0();
        }
    } else {
        SUIShowPage0();
    }
    G_ws->iNeedDraw = EFalse;
}

// Grab a cache entry so no one else can modify it
static TUInt32 ICacheEntryFind(TUInt32 aSlideNumber)
{
    TUInt32 cacheEntryIndex;
    T_cacheEntry *p;

    // Need to find a slide.
    // Is it already in the cache?
    p = G_ws->iCache;
    for (cacheEntryIndex = 0; cacheEntryIndex < SLIDESHOW_NUM_CACHED_SLIDES; cacheEntryIndex++, p++) {
        if ((p->iSlideNumber == aSlideNumber) && (p->iState
                == CACHE_ENTRY_STATE_READY)) {
            // Found a free entry?  Stop here and we'll report this entry
            break;
        }
    }

    // If not found, will return SLIDESHOW_NUM_CACHED_SLIDES
    return cacheEntryIndex;
}

static TUInt32 ICacheEntryToFrame(TUInt32 aCacheEntry)
{
    return aCacheEntry + CACHED_SLIDE_FIRST_FRAME;
}

static void ICacheEntryDetermineNextSlide(
        TUInt32 *aCenterSlide,
        TUInt32 *aCenterCacheEntry)
{
    // Find the next slide we need to load, if any
    TUInt32 numCacheCacheEntrys;
    TInt32 cacheEntry = (TInt32)*aCenterCacheEntry;
    TInt32 slide = (TInt32)*aCenterSlide;
    TInt32 neededSlideForward;
    TInt32 neededSlideBackward;
    TInt32 neededCacheEntryForward;
    TInt32 neededCacheEntryBackward;
    //TUInt32 neededSlide;
    TUInt32 cacheEntryCount = 0;
    TUInt32 rolledBack = 0;
    TUInt32 rolledForward = 0;

    numCacheCacheEntrys
            = (G_ws->iNumSlides > SLIDESHOW_NUM_CACHED_SLIDES) ? SLIDESHOW_NUM_CACHED_SLIDES
                    : G_ws->iNumSlides;
    neededCacheEntryForward = (cacheEntry + 1);
    neededCacheEntryBackward = (cacheEntry - 1);
    neededSlideForward = (slide + 1);
    neededSlideBackward = (slide - 1);

    for (cacheEntryCount = 0; cacheEntryCount < numCacheCacheEntrys; cacheEntryCount++) {
        // Ensure we are still in range
        cacheEntry %= numCacheCacheEntrys;
        slide %= G_ws->iNumSlides;

        // Determine if we have this cacheEntry at the proper location
        // If not, stop here and return this as our result
        if ((G_ws->iCache[cacheEntry].iSlideNumber != slide)
                || (G_ws->iCache[cacheEntry].iState == CACHE_ENTRY_STATE_FREE)) {
            *aCenterSlide = slide;
            *aCenterCacheEntry = cacheEntry;
            return;
        }

        // Go forward or go backward, but give forward a preference
        if ((rolledForward >= rolledBack) && (rolledForward
                < SLIDESHOW_PREFETCH_AHEAD)) {
            // Need more forward, determine the next forward slide and cacheEntry
            // rolling around as necessary
            if (neededCacheEntryForward > numCacheCacheEntrys)
                neededCacheEntryForward -= numCacheCacheEntrys;
            cacheEntry = neededCacheEntryForward++;
            if (neededSlideForward > G_ws->iNumSlides)
                neededSlideForward -= G_ws->iNumSlides;
            slide = neededSlideForward++;
            rolledForward++;
        } else {
            // Do we need previous slides?
            if (rolledBack < SLIDESHOW_PREFETCH_BEHIND) {
                // Need more backward, determine the next back slide and cacheEntry
                // rolling around as necessary
                if (neededCacheEntryBackward < 0)
                    neededCacheEntryBackward += numCacheCacheEntrys;
                cacheEntry = neededCacheEntryBackward--;
                if (neededSlideBackward < 0)
                    neededSlideBackward += G_ws->iNumSlides;
                slide = neededSlideBackward--;
                rolledBack++;
            }
        }

    }

    // Never found a need to change anything.  Report it
    // by returning an illegal cacheEntry
    *aCenterCacheEntry = SLIDESHOW_NUM_CACHED_SLIDES;
}

TUInt32 SSMLoadTask(T_uezTask aMyTask, void *aParameters)
{
    TUInt32 slideNum;
    T_slideLoadResponse slideResponse;
    //TUInt32 i;
    TBool doResponse = EFalse;
    T_uezError error;
    TUInt32 centerSlide = 0;
    TUInt32 centerCacheEntry = 0;
    TUInt32 cacheEntry;
    TUInt32 timeout = UEZ_TIMEOUT_INFINITE;

    //TBD: Configure for larger prefetch based on the space available and test very
    //large slideshows.
    //TBD: Also check why we only have 3 slides in the cache, should have 5.

    // Wait for a request
    while (1) {
        // Check for a request
        error = UEZQueueReceive(G_ws->iSlideRequest, &slideNum, timeout);
        // Don't wait now until we fill the cache
        timeout = 0;
        if (error == UEZ_ERROR_NONE) {
            // Request has arrived
            if (slideNum == (TUInt32) - 1) {
                // This task has been asked to be killed.  Tell we are ending.
                slideResponse.iSlideNum = (TUInt32) - 1;
                UEZQueueSend(G_ws->iSlideReady, &slideResponse,
                        UEZ_TIMEOUT_INFINITE);
                break;
            }
            // Since a slide was requested, we'll need to send a response
            centerSlide = slideNum;

            // Find and grab the slide number
            cacheEntry = ICacheEntryFind(slideNum);

            // If we found that entry, just report we found it immediately
            if (cacheEntry < SLIDESHOW_NUM_CACHED_SLIDES) {
                slideResponse.iSlideNum = slideNum;
                slideResponse.iFrame = ICacheEntryToFrame(cacheEntry);
                UEZQueueSend(G_ws->iSlideReady, &slideResponse, 0);
                doResponse = EFalse;
                // Center around this slide's frame
                centerCacheEntry = cacheEntry;
                continue;
            } else {
                // otherwise fall through and wait for the next
                // slide in the cache to load.
                slideResponse.iSlideNum = slideNum;
                doResponse = ETrue;
            }
        }

        cacheEntry = centerCacheEntry;
        slideNum = centerSlide;
        ICacheEntryDetermineNextSlide(&slideNum, &cacheEntry);

        if (cacheEntry < SLIDESHOW_NUM_CACHED_SLIDES) {
            // Try loading a slide
            if (IDoLoad(slideNum, ICacheEntryToFrame(cacheEntry),
                    &G_ws->iAbortFlag) != UEZ_ERROR_NONE) {
                // This load failed.  Response with a page number
                // of -1.
                if (doResponse) {
                    slideResponse.iSlideNum = slideNum;
                    slideResponse.iFrame = -1;

                    // Send, don't timeout
                    UEZQueueSend(G_ws->iSlideReady, &slideResponse, 0);
                    doResponse = EFalse;
                }
            }

            // The entry is now loaded (regardless of failure above)
            G_ws->iCache[cacheEntry].iState = CACHE_ENTRY_STATE_READY;
            G_ws->iCache[cacheEntry].iSlideNumber = slideNum;

            // If we are to report what we just did, report it now
            if ((doResponse) && (slideResponse.iSlideNum == slideNum)) {
                // Send the response that this frame is now available
                slideResponse.iFrame = ICacheEntryToFrame(cacheEntry);
                UEZQueueSend(G_ws->iSlideReady, &slideResponse, 0);
                doResponse = EFalse;

                // Center around this slide's frame
                centerCacheEntry = cacheEntry;
                centerSlide = slideNum;
            }
        } else {
            // No more slides to load!
            // Wait for a new command, but wait until for a whole time
            timeout = UEZ_TIMEOUT_INFINITE;
        }
    }

    return UEZ_ERROR_NONE;
}

#if SLIDESHOW_EVENT_DRIVEN_CONTROL
T_uezError SingleSlideshowEvent(T_singleSlideshowEvent aEvent)
{
    T_uezError error;

    if (G_slideshowEventQueue)
        error = UEZQueueSend(G_slideshowEventQueue, &aEvent,
                UEZ_TIMEOUT_INFINITE);
    else
        error = UEZ_ERROR_NOT_ACTIVE;

    return error;
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  SingleSlideshowMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a single slideshow
 * Inputs:
 *      T_slideshowDefinition *aDef     -- Slideshow definition to use
 *---------------------------------------------------------------------------*/
void SingleSlideshowMode(T_slideshowDefinition *aDef)
{
    T_uezDevice lcd;
    T_uezQueue queue;
    T_uezDevice ts;
    T_uezInputEvent inputEvent;
    TUInt32 slideNum;
    T_slideLoadResponse slideResponse;
    TUInt32 time = 0;
    TInt32 diffY;
#if UEZ_ENABLE_BUTTON_BOARD
    T_uezDevice keypadDevice;
#endif
#if UEZ_ENABLE_I2S_AUDIO
    char filename[80];
    char directory[80];
    T_uezError error;
#endif
#if SLIDESHOW_EVENT_DRIVEN_CONTROL
    T_singleSlideshowEvent event;
#endif
    G_ws = UEZMemAlloc(sizeof(T_SSMWorkspace));
    if (!G_ws)
        return;
    memset(G_ws, 0, sizeof(*G_ws));

    if (UEZQueueCreate(SLIDESHOW_NUM_CACHED_SLIDES + 1, sizeof(TUInt32),
            &G_ws->iSlideRequest) != UEZ_ERROR_NONE)
        goto abortSlideshow;
    if (UEZQueueCreate(SLIDESHOW_NUM_CACHED_SLIDES + 1,
            sizeof(T_slideLoadResponse), &G_ws->iSlideReady) != UEZ_ERROR_NONE)
        goto abortSlideshow;

    if ((aDef->iName[0] == '\0') && (aDef->iDirectory[0] == '\0'))
        G_ws->iOldFormat = ETrue;
    else
        G_ws->iOldFormat = EFalse;

    G_ws->iLoadTask = 0;
    if (UEZTaskCreate(SSMLoadTask, "SSMLoad", UEZ_TASK_STACK_BYTES(2048), 0,
            UEZ_PRIORITY_LOW, &G_ws->iLoadTask) != UEZ_ERROR_NONE)
        goto abortSlideshow;

    G_ws->iShowPanel = ETrue;
    G_ws->iShowHourglass = EFalse;
    G_ws->iExit = EFalse;
    G_ws->iDef = aDef;
    G_ws->iSlideNum = 0;
    G_ws->iNeedLoad = ETrue;
    G_ws->iNeedDraw = ETrue;
    G_ws->iTouched = EFalse;
#if DKTS_BUTTON_SLIDE_SHOW_DEMO
    G_ws->iAutoSlideshow = ETrue;
#endif

#if UEZ_ENABLE_I2S_AUDIO
    UEZWAVConfig(48);
#endif

    // Sestup a quest to receive slides if not already created
    if (G_slideshowEventQueue == 0)
        UEZQueueCreate(3, sizeof(T_singleSlideshowEvent),
                &G_slideshowEventQueue);

    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_ENABLE_BUTTON_BOARD
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue) == UEZ_ERROR_NONE) {
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
                // SingleSlideshow length is not determined.
                // Determine the number of slides
                SSMDetermineNumSlides();

                // Only do this slideshow if there is a count
                if (G_ws->iNumSlides) {
                    // Put the screen (with options)
                    SingleSlideshowScreen(lcd);
                    SSMSetupChoices();

                    // Sit here in a loop until we are done
                    while (!G_ws->iExit) {

                        // Prevent Screen Saver operation while in slide show
                        UEZLCDScreensaverWake();

                        // If we need to load a slide, do it now
                        if (G_ws->iNeedLoad) {
                            SSMLoadSlide();
                            time = UEZTickCounterGet();
#if UEZ_ENABLE_I2S_AUDIO
                            sprintf(directory, SLIDES_DIRECTORY, G_ws->iDef->iDirectory);
                            sprintf(filename, "%d:" SLIDESHOW_AUDIO_NAMING, G_ws->iDef->iDrive, directory, G_ws->iSlideNum+1);
                            if(!UEZWAVGetStatus()){
                                error = UEZWAVPlayFile(filename);
                            }
#endif
                        }

                        // If we need to draw a new screen, do it now
                        if (G_ws->iNeedDraw) {
                            SSMDrawSlide(ETrue);
                            time = UEZTickCounterGet();
                        }

#if SLIDESHOW_EVENT_DRIVEN_CONTROL
                        // Check for any waiting events
                        if (UEZQueueReceive(G_slideshowEventQueue, &event, 0)
                                != UEZ_ERROR_TIMEOUT) {
                            // Got an event!  See which button
                            // to push
                            switch (event) {
                                case SINGLE_SLIDESHOW_EVENT_EXIT:
                                    G_ws->iExit = ETrue;
                                    break;
                                case SINGLE_SLIDESHOW_EVENT_NEXT:
                                    SSMNext(0);
                                    break;
                                case SINGLE_SLIDESHOW_EVENT_PREVIOUS:
                                    SSMPrevious(0);
                                    break;
                                default:
                                    //ignore
                                    break;
                            }
                        } else
#endif

                        //Wait till we get a touchscreen event
                        if (UEZQueueReceive(queue, &inputEvent,
                                UEZ_TIMEOUT_INFINITE) == UEZ_ERROR_NONE) {
                            if (G_ws->iShowPanel) {
                                ChoicesUpdate(&G_win, G_ws->iChoices, queue,
                                        500);
                                if (UEZTickCounterGetDelta(time)
                                        >= SLIDESHOW_PANEL_DISAPPEAR_TIME) {
                                    G_ws->iShowPanel = EFalse;
                                    G_ws->iNeedDraw = ETrue;
                                    time = UEZTickCounterGet();
                                }
                            } else {
                                if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
                                    // Show panel now
                                    G_ws->iShowPanel = ETrue;
                                    G_ws->iNeedDraw = ETrue;
                                } else {
                                    // Nothing being pressed and no event,
                                    // check if we are doing an auto slideshow
                                    if (G_ws->iAutoSlideshow) {
                                        if (UEZTickCounterGetDelta(time)
                                                >= SLIDESHOW_PANEL_AUTO_NEXT_SLIDE_TIME) {
                                            // Act as if pressed next
#if UEZ_ENABLE_I2S_AUDIO
                                            if(UEZWAVGetStatus());

                                            else
#endif
                                            SSMNext(0);
                                        }
                                    }
                                }
                            }

                            // Watch to see if we are dragging
                            if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
                                if (!G_ws->iTouched) {
                                    // Start recording touch
                                    G_ws->iTouchStartY = inputEvent.iEvent.iXY.iY;
                                } else {
                                    G_ws->iTouchEndY = inputEvent.iEvent.iXY.iY;
                                }
                                G_ws->iTouched = ETrue;
                            } else {
                                // End of stroke?
                                if (G_ws->iTouched) {
                                    // No longer touching
                                    // What length of stroke did we get vertically?
                                    diffY = G_ws->iTouchEndY
                                            - G_ws->iTouchStartY;
                                    if (diffY < -SCROLL_SENSITIVITY) {
                                        // Dragged down
                                        SSMNext(0);
                                    } else if (diffY > SCROLL_SENSITIVITY) {
                                        // Dragged up
                                        SSMPrevious(0);
                                    }
                                }
                                G_ws->iTouched = EFalse;
                            }
                        }
                    }
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

    abortSlideshow: if (G_ws->iLoadTask) {
        // Try to kill the load task
        slideNum = (TUInt32)-1;
        UEZQueueSendFront(G_ws->iSlideRequest, &slideNum, UEZ_TIMEOUT_INFINITE);

        // Wait for a response of -1 to confirm task is dead
        while (1) {
            slideResponse.iSlideNum = 0;
            UEZQueueReceive(G_ws->iSlideReady, &slideResponse,
                    UEZ_TIMEOUT_INFINITE);
            if (slideResponse.iSlideNum == (TUInt32)-1)
                break;
        }
    }
    if (G_ws->iSlideRequest)
        UEZQueueDelete(G_ws->iSlideRequest);
    if (G_ws->iSlideReady)
        UEZQueueDelete(G_ws->iSlideReady);
    UEZMemFree(G_ws);
    G_ws = 0;
}

TUInt32 SingleSlideshowGetSlideNum(void)
{
    if (G_ws)
        return G_ws->iSlideNum+1;
    return 0;
}

TUInt32 SingleSlideshowGetNumSlides(void)
{
    if (G_ws)
        return G_ws->iNumSlides;
    return 0;
}

void SlideShowExit()
{
    if(G_ws)
      G_ws->iExit = ETrue;
}


/*-------------------------------------------------------------------------*
 * End of File:  SingleSlideshowMode.c
 *-------------------------------------------------------------------------*/
