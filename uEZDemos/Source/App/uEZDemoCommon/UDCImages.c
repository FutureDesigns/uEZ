/*-------------------------------------------------------------------------*
 * File:  Images.c
 *-------------------------------------------------------------------------*
 * Description:
 *      All the graphics for the images are stored here.
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
#include <uEZ.h>
#include "AppDemo.h"

#ifndef UEZ_ICONS_SET
    #define UEZ_ICONS_SET ICONS_SET_UEZ_OPEN_SOURCE
#endif

/*---------------------------------------------------------------------------*
 * Global Graphics:
 *---------------------------------------------------------------------------*/
#if (UEZ_ICONS_SET == ICONS_SET_UEZ_OPEN_SOURCE)
#include "images/FDI_Icons/CalibrationIcon.h"
#include "images/FDI_Icons/DrawIcon.h"
#include "images/FDI_Icons/SlideshowIcon.h"
#include "images/FDI_Icons/ConsoleIcon.h"
#include "images/FDI_Icons/ExitIcon.h"
#include "images/FDI_Icons/LoadIcon.h"
#include "images/FDI_Icons/SaveIcon.h"
#include "images/FDI_Icons/TitleScreen.h"
#include "images/FDI_Icons/SettingsIcon.h"
#include "images/FDI_Icons/AccelIcon.h"
#include "images/FDI_Icons/TimeTempIcon.h"
#include "images/FDI_Icons/BallIcon.h"
#include "images/FDI_Icons/FuncTestIcon.h"
#include "images/FDI_Icons/UpArrowIcon.h"
#include "images/FDI_Icons/DownArrowIcon.h"
#include "images/FDI_Icons/FuncTestLoopIcon.h"
#include "images/FDI_Icons/SlideshowSmallIcon.h"
#include "images/FDI_Icons/ForwardButtonIcon.h"
#include "images/FDI_Icons/PauseButtonIcon.h"
#include "images/FDI_Icons/PlayButtonIcon.h"
#include "images/FDI_Icons/RewindButtonIcon.h"
#include "images/FDI_Icons/StopButtonIcon.h"
#include "images/FDI_Icons/AppFolderIcon.h"
#include "images/FDI_Icons/QuestionIcon.h"
#include "images/FDI_Icons/TimeDateIcon.h"
#include "images/FDI_Icons/TemperatureIcon.h"
#include "images/FDI_Icons/RotatedBanner.h"
#include "images/FDI_Icons/DownExit.h"
#include "images/FDI_Icons/YourAppWindow.h"
#include "images/FDI_Icons/ContrastIcon.h"
#include "images/FDI_Icons/HourglassIcon.h"
#include "images/FDI_Icons/VNCIcon.h"
#include "images/FDI_Icons/GainSpanLogo.h"
#endif

#if (UEZ_ICONS_SET == ICONS_SET_PROFESSIONAL_ICONS)
#include "images/Professional_Icons/CalibrationIcon.h"
#include "images/Professional_Icons/DrawIcon.h"
#include "images/Professional_Icons/SlideshowIcon.h"
#include "images/Professional_Icons/ConsoleIcon.h"
#include "images/Professional_Icons/ExitIcon.h"
#include "images/Professional_Icons/LoadIcon.h"
#include "images/Professional_Icons/SaveIcon.h"
#include "images/Professional_Icons/SettingsIcon.h"
#include "images/Professional_Icons/AccelIcon.h"
#include "images/Professional_Icons/TimeTempIcon.h"
#include "images/Professional_Icons/BallIcon.h"
#include "images/Professional_Icons/FuncTestIcon.h"
#include "images/Professional_Icons/UpArrowIcon.h"
#include "images/Professional_Icons/DownArrowIcon.h"
#include "images/Professional_Icons/FuncTestLoopIcon.h"
#include "images/Professional_Icons/SlideshowSmallIcon.h"
#include "images/Professional_Icons/ForwardButtonIcon.h"
#include "images/Professional_Icons/PauseButtonIcon.h"
#include "images/Professional_Icons/PlayButtonIcon.h"
#include "images/Professional_Icons/RewindButtonIcon.h"
#include "images/Professional_Icons/StopButtonIcon.h"
#include "images/Professional_Icons/AppFolderIcon.h"
#include "images/Professional_Icons/QuestionIcon.h"
#include "images/Professional_Icons/TimeDateIcon.h"
#include "images/Professional_Icons/TemperatureIcon.h"
#include "images/Professional_Icons/RotatedBanner.h"
#include "images/Professional_Icons/DownExit.h"
#include "images/Professional_Icons/YourAppWindow.h"
#include "images/Professional_Icons/ContrastIcon.h"
#include "images/Professional_Icons/HourglassIcon.h"
#include "images/Professional_Icons/seggerLogo.h"
#include "images/Professional_Icons/3D.h"
#include "images/Professional_Icons/VNCIcon.h"
#include "images/Professional_Icons/AudioPlayerIcon.h"
#include "images/Professional_Icons/AudioFileIconPlay.h"
#include "images/Professional_Icons/AudioFileIconStop.h"
#include "images/Professional_Icons/ArrowUp.h"
#include "images/Professional_Icons/ArrowDown.h"
#include "images/Professional_Icons/GainSpanLogo.h"
#include "images/Professional_Icons/VideoIcon.h"
#if SIMPLEUI_DOUBLE_SIZED_ICONS
#include "images/Professional_Icons/uEZLogoLarge.h"
#else
#include "images/Professional_Icons/uEZLogo.h"
#endif
#endif

#if (UEZ_ICONS_SET == ICONS_SET_PROFESSIONAL_ICONS_LARGE)
#include "images/Professional_Icons128/CalibrationIcon128.h"
#include "images/Professional_Icons128/DrawIcon128.h"
#include "images/Professional_Icons128/SlideshowIcon128.h"
#include "images/Professional_Icons128/ConsoleIcon128.h"
#include "images/Professional_Icons128/ExitIcon.h"
#include "images/Professional_Icons128/LoadIcon.h"
#include "images/Professional_Icons128/SaveIcon.h"
#include "images/Professional_Icons128/SettingsIcon128.h"
#include "images/Professional_Icons128/AccelIcon128.h"
#include "images/Professional_Icons128/BallIcon.h"
#include "images/Professional_Icons128/FuncTestIcon128.h"
#include "images/Professional_Icons128/UpArrowIcon.h"
#include "images/Professional_Icons128/DownArrowIcon.h"
#include "images/Professional_Icons128/FuncTestLoopIcon128.h"
#include "images/Professional_Icons128/SlideshowSmallIcon.h"
#include "images/Professional_Icons/ForwardButtonIcon.h"
#include "images/Professional_Icons/PauseButtonIcon.h"
#include "images/Professional_Icons/PlayButtonIcon.h"
#include "images/Professional_Icons/RewindButtonIcon.h"
#include "images/Professional_Icons/StopButtonIcon.h"
#include "images/Professional_Icons128/AppFolderIcon128.h"
#include "images/Professional_Icons128/QuestionIcon128.h"
#include "images/Professional_Icons128/TimeDateIcon128.h"
#include "images/Professional_Icons128/TemperatureIcon128.h"
//#include "images/Professional_Icons/RotatedBanner.h"
//#include "images/Professional_Icons/DownExit.h"
#include "images/Professional_Icons128/YourAppWindow.h"
#include "images/Professional_Icons128/ContrastIcon128.h"
#include "images/Professional_Icons128/HourglassIcon.h"
#include "images/Professional_Icons128/seggerLogo.h"
#include "images/Professional_Icons128/3D.h"
#include "images/Professional_Icons128/VNCIcon.h"
#include "images/Professional_Icons128/AudioPlayerIcon.h"
#include "images/Professional_Icons128/AudioFileIconPlay.h"
#include "images/Professional_Icons128/AudioFileIconStop.h"
#include "images/Professional_Icons128/ArrowUp.h"
#include "images/Professional_Icons128/ArrowDown.h"
#include "images/Professional_Icons128/GainSpanLogo.h"
#include "images/Professional_Icons128/uEZLogo.h"
#include "images/Professional_Icons128/VideoIcon128.h"
#endif

/*-------------------------------------------------------------------------*
 * File:  Images.c
 *-------------------------------------------------------------------------*/
