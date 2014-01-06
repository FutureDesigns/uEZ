#ifndef BOUNCING_LOGO_SS
#define BOUNCING_LOGO_SS

#include <uEZ.h>

void BouncingLogoSS_Setup(
    TUInt8 *aLogoImage,
    TUInt32 aImageWidth,
    TUInt32 aImageHeight,
    TUInt32 aDisplayWidth,
    TUInt32 aDisplayHeight);

void BouncingLogoSS_Start();
void BouncingLogoSS_Update();
void BouncingLogoSS_End();
void BouncingLogoSS_Sleep();

#endif