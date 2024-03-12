REM set SEGGER_TOOLS_DIR=..\..\..\..\..\uEZ\Source\Library\GUI\SEGGER\emWin\Libraries\NXP\UtilityTools
REM Fix the path
%SEGGER_TOOLS_DIR%\BmpCvtNXP.exe "mqtt-ver-64.png" -convertintobestpalette -saveas"mqttLogo64",1,7,0 -exit
%SEGGER_TOOLS_DIR%\BmpCvtNXP.exe "mqtt-ver-128.png" -convertintobestpalette -saveas"mqttLogo128",1,7,0 -exit
%SEGGER_TOOLS_DIR%\BmpCvtNXP.exe "iotQrCode131.png" -convertintobestpalette -saveas"iotQrCode",1,7,0 -exit
REM Add additional files here.
