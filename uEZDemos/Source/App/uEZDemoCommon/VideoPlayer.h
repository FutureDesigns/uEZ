#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

enum {
    VIDEO_TYPE_BIN,
    VIDEO_TYPE_FRAMES    
}typedef T_VIDEO_TYPE;

typedef struct {
    char iTitle[64];
    T_VIDEO_TYPE iType;
    char iVideoPath[128];
    char iAudioPath[128];
    TUInt32 iFPS;
    TUInt32 iVideoHeight;
    TUInt32 iVideoWidth;
    char iTextLine1[128];
    char iTextLine2[128];
} T_VideoInfo;

extern T_VideoInfo G_NextVideoInfo;

void VideoPlayerBIN(const T_choice *aChoice);
void VideoPlayerFrames(const T_choice *aChoice);

#endif //VIDEO_PLAYER_H