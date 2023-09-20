#include "BouncingLogoSS.h"
#include <uEZ.h>
#include <uEZPlatform.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_DrawBitmap.h>

static TUInt32 G_ImageWidth, G_ImageHeight, G_displayWidth, G_displayHeight;
static TUInt32 G_SS_X_pos=100, G_SS_Y_pos=100;
static TInt32 G_SS_X_vel=10, G_SS_Y_vel=20;
static SWIM_WINDOW_T G_ssWin;
TUInt8 *G_LogoImage;

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

void BouncingLogoSS_Start() {
    T_pixelColor *pixels;
    T_uezDevice lcd;
    SWIM_WINDOW_T lastWin;
    
    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    UEZTaskSchedulerSuspend();
    lastWin = *SUIGetDrawWindow();
    
    UEZLCDGetFrame(lcd, 2, (void **)&pixels); // clear first image to black before show frame below
    swim_window_open(&G_ssWin, G_displayWidth, G_displayHeight, pixels, 0, 0,
            G_displayWidth - 1, G_displayHeight - 1, 2, BLACK, RGB(0, 0, 0), RED);
    
    G_SS_X_pos = G_displayWidth/2 - G_ImageWidth/2;
    G_SS_Y_pos = G_displayHeight/2 - G_ImageHeight/2;

    UEZLCDShowFrame(lcd, 2);

    SUISetDrawWindow(&lastWin);
    UEZTaskSchedulerResume();
}


void BouncingLogoSS_Update() {

    TUInt32 previousXPos = G_SS_X_pos;
    TUInt32 previousYPos = G_SS_Y_pos;
    T_uezDevice lcd;
    SWIM_WINDOW_T lastWin;
    
    UEZTaskSchedulerSuspend();
    
    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    UEZLCDShowFrame(lcd, 2);
    lastWin = *SUIGetDrawWindow();
    SUISetDrawWindow(&G_ssWin);
    
    if(G_SS_X_vel > 0) {
        if((G_ImageWidth+G_SS_X_pos+G_SS_X_vel) > G_displayWidth) {
           G_SS_X_pos = G_displayWidth-G_ImageWidth;
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
        if((G_ImageHeight+G_SS_Y_pos+G_SS_Y_vel) > G_displayHeight) {
           G_SS_Y_pos = G_displayHeight-G_ImageHeight;
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
    
    SUIFillRect((T_pixelColor *)FRAME(2),
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
void BouncingLogoSS_Sleep() {
    T_uezDevice lcd;
    SWIM_WINDOW_T lastWin;
    UEZTaskSchedulerSuspend();
    
    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    UEZLCDShowFrame(lcd, 2);
    lastWin = *SUIGetDrawWindow();
    SUISetDrawWindow(&G_ssWin);
   
    SUIFillRect((T_pixelColor *)FRAME(2), 0,0,G_displayWidth,G_displayHeight, RGB(0, 0, 0));
        
    SUISetDrawWindow(&lastWin);
    UEZTaskSchedulerResume();
}

void BouncingLogoSS_End() {
    T_uezDevice lcd;
    
    if (UEZLCDOpen("LCD", &lcd) != UEZ_ERROR_NONE) {
        UEZFailureMsg("AppScreensaverStart Failed to open lcd device 'LCD'");
    }
    
    UEZLCDShowFrame(lcd, 0);
}