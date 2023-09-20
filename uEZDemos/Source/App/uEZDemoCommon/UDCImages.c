/*-------------------------------------------------------------------------*
 * File:  Images.c
 *-------------------------------------------------------------------------*
 * Description:
 *      All the graphics for the images are stored here.
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
#include <uEZ.h>
#include "AppDemo.h"

#ifndef UEZ_ICONS_SET
    #define UEZ_ICONS_SET ICONS_SET_UEZ_OPEN_SOURCE
#endif

/*---------------------------------------------------------------------------*
 * Global Graphics:
 *---------------------------------------------------------------------------*/
#if (UEZ_ICONS_SET == ICONS_SET_UEZ_OPEN_SOURCE)
#include "Images/FDI_Icons/CalibrationIcon.h"
#include "Images/FDI_Icons/DrawIcon.h"
#include "Images/FDI_Icons/SlideshowIcon.h"
#include "Images/FDI_Icons/ConsoleIcon.h"
#include "Images/FDI_Icons/ExitIcon.h"
#include "Images/FDI_Icons/LoadIcon.h"
#include "Images/FDI_Icons/SaveIcon.h"
#include "Images/FDI_Icons/TitleScreen.h"
#include "Images/FDI_Icons/SettingsIcon.h"
#include "Images/FDI_Icons/AccelIcon.h"
#include "Images/FDI_Icons/TimeTempIcon.h"
#include "Images/FDI_Icons/BallIcon.h"
#include "Images/FDI_Icons/FuncTestIcon.h"
#include "Images/FDI_Icons/UpArrowIcon.h"
#include "Images/FDI_Icons/DownArrowIcon.h"
#include "Images/FDI_Icons/FuncTestLoopIcon.h"
#include "Images/FDI_Icons/SlideshowSmallIcon.h"
#include "Images/FDI_Icons/ForwardButtonIcon.h"
#include "Images/FDI_Icons/PauseButtonIcon.h"
#include "Images/FDI_Icons/PlayButtonIcon.h"
#include "Images/FDI_Icons/RewindButtonIcon.h"
#include "Images/FDI_Icons/StopButtonIcon.h"
#include "Images/FDI_Icons/AppFolderIcon.h"
#include "Images/FDI_Icons/QuestionIcon.h"
#include "Images/FDI_Icons/TimeDateIcon.h"
#include "Images/FDI_Icons/TemperatureIcon.h"
#include "Images/FDI_Icons/RotatedBanner.h"
#include "Images/FDI_Icons/DownExit.h"
#include "Images/FDI_Icons/YourAppWindow.h"
#include "Images/FDI_Icons/ContrastIcon.h"
#include "Images/FDI_Icons/HourglassIcon.h"
#include "Images/FDI_Icons/VNCIcon.h"
#include "Images/FDI_Icons/GainSpanLogo.h"
#endif

#if (UEZ_ICONS_SET == ICONS_SET_PROFESSIONAL_ICONS)
#include "Images/Professional_Icons/CalibrationIcon.h"
#include "Images/Professional_Icons/DrawIcon.h"
#include "Images/Professional_Icons/SlideshowIcon.h"
#include "Images/Professional_Icons/ConsoleIcon.h"
#include "Images/Professional_Icons/ExitIcon.h"
#include "Images/Professional_Icons/LoadIcon.h"
#include "Images/Professional_Icons/SaveIcon.h"
#include "Images/Professional_Icons/SettingsIcon.h"
#include "Images/Professional_Icons/AccelIcon.h"
#include "Images/Professional_Icons/TimeTempIcon.h"
#include "Images/Professional_Icons/BallIcon.h"
#include "Images/Professional_Icons/FuncTestIcon.h"
#include "Images/Professional_Icons/UpArrowIcon.h"
#include "Images/Professional_Icons/DownArrowIcon.h"
#include "Images/Professional_Icons/FuncTestLoopIcon.h"
#include "Images/Professional_Icons/SlideshowSmallIcon.h"
#include "Images/Professional_Icons/ForwardButtonIcon.h"
#include "Images/Professional_Icons/PauseButtonIcon.h"
#include "Images/Professional_Icons/PlayButtonIcon.h"
#include "Images/Professional_Icons/RewindButtonIcon.h"
#include "Images/Professional_Icons/StopButtonIcon.h"
#include "Images/Professional_Icons/AppFolderIcon.h"
#include "Images/Professional_Icons/QuestionIcon.h"
#include "Images/Professional_Icons/TimeDateIcon.h"
#include "Images/Professional_Icons/TemperatureIcon.h"
#include "Images/Professional_Icons/RotatedBanner.h"
#include "Images/Professional_Icons/DownExit.h"
#include "Images/Professional_Icons/YourAppWindow.h"
#include "Images/Professional_Icons/ContrastIcon.h"
#include "Images/Professional_Icons/HourglassIcon.h"
#include "Images/Professional_Icons/seggerLogo.h"
#include "Images/Professional_Icons/3D.h"
#include "Images/Professional_Icons/VNCIcon.h"
#include "Images/Professional_Icons/AudioPlayerIcon.h"
#include "Images/Professional_Icons/AudioFileIconPlay.h"
#include "Images/Professional_Icons/AudioFileIconStop.h"
#include "Images/Professional_Icons/ArrowUp.h"
#include "Images/Professional_Icons/ArrowDown.h"
#include "Images/Professional_Icons/GainSpanLogo.h"
#include "Images/Professional_Icons/VideoIcon.h"
#if SIMPLEUI_DOUBLE_SIZED_ICONS
#include "Images/Professional_Icons/uEZLogoLarge.h"
#else
#include "Images/Professional_Icons/uEZLogo.h"
#endif
#endif

#if (UEZ_ICONS_SET == ICONS_SET_PROFESSIONAL_ICONS_LARGE)
#include "Images/Professional_Icons128/CalibrationIcon128.h"
#include "Images/Professional_Icons128/DrawIcon128.h"
#include "Images/Professional_Icons128/SlideshowIcon128.h"
#include "Images/Professional_Icons128/ConsoleIcon128.h"
#include "Images/Professional_Icons128/ExitIcon.h"
#include "Images/Professional_Icons128/LoadIcon.h"
#include "Images/Professional_Icons128/SaveIcon.h"
#include "Images/Professional_Icons128/SettingsIcon128.h"
#include "Images/Professional_Icons128/AccelIcon128.h"
#include "Images/Professional_Icons128/BallIcon.h"
#include "Images/Professional_Icons128/FuncTestIcon128.h"
#include "Images/Professional_Icons128/UpArrowIcon.h"
#include "Images/Professional_Icons128/DownArrowIcon.h"
#include "Images/Professional_Icons128/FuncTestLoopIcon128.h"
#include "Images/Professional_Icons128/SlideshowSmallIcon.h"
#include "Images/Professional_Icons/ForwardButtonIcon.h"
#include "Images/Professional_Icons/PauseButtonIcon.h"
#include "Images/Professional_Icons/PlayButtonIcon.h"
#include "Images/Professional_Icons/RewindButtonIcon.h"
#include "Images/Professional_Icons/StopButtonIcon.h"
#include "Images/Professional_Icons128/AppFolderIcon128.h"
#include "Images/Professional_Icons128/QuestionIcon128.h"
#include "Images/Professional_Icons128/TimeDateIcon128.h"
#include "Images/Professional_Icons128/TemperatureIcon128.h"
//#include "Images/Professional_Icons/RotatedBanner.h"
//#include "Images/Professional_Icons/DownExit.h"
#include "Images/Professional_Icons128/YourAppWindow.h"
#include "Images/Professional_Icons128/ContrastIcon128.h"
#include "Images/Professional_Icons128/HourglassIcon.h"
#include "Images/Professional_Icons128/seggerLogo.h"
#include "Images/Professional_Icons128/3D.h"
#include "Images/Professional_Icons128/VNCIcon.h"
#include "Images/Professional_Icons128/AudioPlayerIcon.h"
#include "Images/Professional_Icons128/AudioFileIconPlay.h"
#include "Images/Professional_Icons128/AudioFileIconStop.h"
#include "Images/Professional_Icons128/ArrowUp.h"
#include "Images/Professional_Icons128/ArrowDown.h"
#include "Images/Professional_Icons128/GainSpanLogo.h"
#include "Images/Professional_Icons128/uEZLogo.h"
#include "Images/Professional_Icons128/VideoIcon128.h"
#endif

/*-------------------------------------------------------------------------*
 * File:  Images.c
 *-------------------------------------------------------------------------*/
