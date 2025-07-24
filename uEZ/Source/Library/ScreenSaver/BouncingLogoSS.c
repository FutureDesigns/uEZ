#include "BouncingLogoSS.h"
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZLCD.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_DrawBitmap.h>

static TUInt32 G_ImageWidth, G_ImageHeight, G_displayWidth, G_displayHeight;
static TUInt32 G_SS_X_pos=100, G_SS_Y_pos=100;
static TInt32 G_SS_X_vel=10, G_SS_Y_vel=20;
static SWIM_WINDOW_T G_ssWin;
TUInt8 *G_LogoImage;

#define BUFFER_FRAME_NUM        1 // confine all of this to only the second framebuffer

void BouncingLogoSS_Setup(
    TUInt8 *aLogoImage,
    TUInt32 aImageWidth,
    TUInt32 aImageHeight,
    TUInt32 aDisplayWidth,
    TUInt32 aDisplayHeight)
{
    G_LogoImage = aLogoImage;
    G_ImageWidth = aImageWidth;
    G_ImageHeight = aImageHeight;
    G_displayWidth = aDisplayWidth;
    G_displayHeight = aDisplayHeight;
}

void BouncingLogoSS_Start(void) {
    T_pixelColor *pixels;
    T_uezDevice lcd;
    SWIM_WINDOW_T lastWin;

    UEZTaskSchedulerSuspend();

    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    lastWin = *SUIGetDrawWindow();
    
    UEZLCDGetFrame(lcd, BUFFER_FRAME_NUM, (void **)&pixels); // clear first image to black before show frame below
    swim_window_open(&G_ssWin, G_displayWidth, G_displayHeight, pixels, 0, 0,
            G_displayWidth - 1, G_displayHeight - 1, 2, BLACK, RGB(0, 0, 0), RED);
    
    G_SS_X_pos = G_displayWidth/2 - G_ImageWidth/2;
    G_SS_Y_pos = G_displayHeight/2 - G_ImageHeight/2;

    UEZLCDShowFrame(lcd, BUFFER_FRAME_NUM);

    SUISetDrawWindow(&lastWin);
    UEZTaskSchedulerResume();
}


void BouncingLogoSS_Update(void) {

    TUInt32 previousXPos = G_SS_X_pos;
    TUInt32 previousYPos = G_SS_Y_pos;
    T_uezDevice lcd;
    SWIM_WINDOW_T lastWin;
    
    UEZTaskSchedulerSuspend();
    
    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    UEZLCDShowFrame(lcd, BUFFER_FRAME_NUM); // This may not be needed or useful unless we could switch away from the window?
    lastWin = *SUIGetDrawWindow();
    SUISetDrawWindow(&G_ssWin);
    
    if(G_SS_X_vel > 0) {
        if((G_ImageWidth+G_SS_X_pos+G_SS_X_vel) > (G_displayWidth-1)) { // out of bounds check
           G_SS_X_pos = (G_displayWidth-1)-G_ImageWidth; // always make sure to be 1 less than LCD width since we start from 0
           G_SS_X_vel *= -1;
        } else {
           G_SS_X_pos += G_SS_X_vel;
        }
    } else {
        if(((TInt32)G_SS_X_pos+G_SS_X_vel) < 0) {
           G_SS_X_pos = 0;
           G_SS_X_vel *= -1;
        } else {
           G_SS_X_pos += G_SS_X_vel;
        }
    }
    
    if(G_SS_Y_vel > 0) {
        if((G_ImageHeight+G_SS_Y_pos+G_SS_Y_vel) > (G_displayHeight-1)) { // out of bounds check
           G_SS_Y_pos = (G_displayHeight-1)-G_ImageHeight; // always make sure to be 1 less than LCD height since we start from 0
           G_SS_Y_vel *= -1;
        } else {
           G_SS_Y_pos += G_SS_Y_vel;
        }
    } else {
        if(((TInt32)G_SS_Y_pos+G_SS_Y_vel) < 0) {
           G_SS_Y_pos = 0;
           G_SS_Y_vel *= -1;
        } else {
           G_SS_Y_pos += G_SS_Y_vel;
        }
    }
    
    SUIFillRect((T_pixelColor *)FRAME(BUFFER_FRAME_NUM),
        previousXPos, previousYPos,
        previousXPos+G_ImageWidth,
        previousYPos+G_ImageHeight, RGB(0, 0, 0));
    
    SUIDrawBitmap(
        G_LogoImage,
        G_SS_X_pos,
        G_SS_Y_pos);
        
    SUISetDrawWindow(&lastWin);
    UEZTaskSchedulerResume();
}
// fill screen with black to turn picture off.
void BouncingLogoSS_Sleep(void) {
    T_uezDevice lcd;
    SWIM_WINDOW_T lastWin;
    UEZTaskSchedulerSuspend();
    
    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    UEZLCDShowFrame(lcd, BUFFER_FRAME_NUM);
    lastWin = *SUIGetDrawWindow();
    SUISetDrawWindow(&G_ssWin);
    // SUISetDrawWindow must be setup for  G_SUISettings.iWindow first or we will crash
    SUIFillRect((T_pixelColor *)FRAME(BUFFER_FRAME_NUM), 0,0,(G_displayWidth-1),(G_displayHeight-1), RGB(0, 0, 0));
        
    SUISetDrawWindow(&lastWin);
    UEZTaskSchedulerResume();
}

void BouncingLogoSS_End(void) {
    T_uezDevice lcd;
    
    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    UEZLCDShowFrame(lcd, 0);
}
