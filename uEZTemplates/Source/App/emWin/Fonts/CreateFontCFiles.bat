REM This is an example of generating some commonly used fonts for use with emWin. Place the desired ttf file into this folder then run this script to generate the desired sizes.
REM See the emWin user's manual and "-?" command for details on available options.
setlocal enabledelayedexpansion
set SEGGER_TOOLS_DIR=..\..\..\..\..\uEZ\Source\Library\GUI\SEGGER\emWin\Libraries\NXP\UtilityTools
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe -?
timeout /T 2 /NOBREAK > nul

REM Generate files for font size 24
set fontSize=24
timeout /T 2 /NOBREAK > nul

REM 0x20-0x7F is standard ASCII,  0xA6-0xB5
REM For Adwaita Sans fonts the following extra characters could be added:
REM set rangesExtra=0x3A9,0x3C0,0x2013-0x2015,0x2018-0x2019,0x201C,0x201D,0x2022,0x2122,0x2150-0x217C,0x2190-0x21D4,0x221E,0x2264-0x2265,0x232B,0x23CF,0x2713,0x2B06,0xEE64-0xEE65
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaMono_Regular.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaMono_Regular_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaSans_BoldItalic.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaSans_BoldItalic_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaMono_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaMono_Bold_%fontSize%_bpp_4.c" -v

REM For LiberationSans_Bold fonts the following extra characters could be added:
REM set rangesExtra=0x3A9,0x3C0,0x2013-0x2015,0x2018-0x2019,0x201C,0x201D,0x2022,0x2122,0x2150-0x217C,0x2190-0x21D4,0x221E,0x2264-0x2265
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "LiberationSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_LiberationSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "NotoSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_NotoSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "OpenSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_OpenSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "OpenSans_CondensedBold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_OpenSans_CondensedBold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul

REM Generate fonts for a larger size font also
set fontSize=36
timeout /T 2 /NOBREAK > nul


%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaMono_Regular.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaMono_Regular_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaSans_BoldItalic.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaSans_BoldItalic_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "AdwaitaMono_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_AdwaitaMono_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "LiberationSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_LiberationSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "NotoSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_NotoSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "OpenSans_Bold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_OpenSans_Bold_%fontSize%_bpp_4.c" -v
timeout /T 2 /NOBREAK > nul
%SEGGER_TOOLS_DIR%\FontCvtNXP.exe "OpenSans_CondensedBold.ttf" -h %fontSize%px -f "AA4" -enc "UC" -e 0x20-0x7F,0xA6-0xB5,0xB7,0xBB-0xBE -s "emFont_OpenSans_CondensedBold_%fontSize%_bpp_4.c" -v

REM Add additional font files here.

pause
